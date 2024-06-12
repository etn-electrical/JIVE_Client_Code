/**
 *****************************************************************************************
 *	@file AVObject.cpp
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "vsbhp.h"
#include "AVObject.h"
#include "BACnetStackInterface.h"
#include "Ram.h"
#include "BACnet_DCI.h"
#include "BACnetDCIInterface.h"
#include "Binary_Search.h"
#include "DCI_Data.h"

BACnetObject avObject;
extern BACnet_DCI* BACnetDCIPtr;

BACNET_ID any_param_av_instance = 0U;
static const BACNET_TARGET_INFO_ST_TD* m_bacnet_target_info;

/** @fn createAvObjects(uint8_t numObjects)
 *   @brief :create  AV objects
 *   @details :This function has to call before to perform any
 *             operation on AV object. This function creates the instancess of
 * 			  the AV object
 *   @param[in] numObjects: number of instances
 *   @return:0 on success and -1 otherwise
 */
int32_t createAvObjects( const BACNET_TARGET_INFO_ST_TD* bacnet_target_info, uint8_t numObjects )
{
	uint8_t arrayIndex = 0U;

	avObject.numObjects = numObjects;
	BACnetAV* avPtr;
	float32_t val = 0.0F;
	uint16_t* convertedVal;
	DCI_ID_TD dcid;
	int32_t retVal = 0;

	m_bacnet_target_info = bacnet_target_info;

	if ( numObjects == 0U )
	{
		retVal = -1;
	}
	else
	{
		uint16_t i;
#ifdef MODBUS_PARAM_READ_FEATURE_ENABLE
		for ( i = 0U; i < TOTAL_AV; i++ )
		{
			if ( m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[i].dcid ==
				 DCI_AV_ANY_PARAM_VAL_DCID )
			{
				any_param_av_instance = m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[i]
					.bacnet_id;
			}
		}
#endif
		avObject.objects = ( uint32_t* )Ram::Allocate( ( uint16_t )numObjects * sizeof( uint32_t ) );
		if ( avObject.objects != NULL )
		{
			for ( i = 0U; i < numObjects; i++ )
			{
				avObject.objects[i] = ( uint32_t )Ram::Allocate( sizeof( BACnetAV ) );
				if ( avObject.objects[i] != 0U )
				{
					avPtr = ( BACnetAV* )avObject.objects[i];
					// Get default value of COV increment for each AV object
					avPtr->covincrement = m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[i]
						.cov_increment;
					for ( arrayIndex = 0U; arrayIndex < 16U; arrayIndex++ )
					{
						avPtr->priority_array.d[arrayIndex] = nullvalue;// By default all the priority array is empty
					}
					dcid = getDCIDForAVObject( i );
					retVal = BACnetDCIPtr->Read_Param( dcid, ( uint8_t* )&val, DCI_ACCESS_GET_RAM_CMD );
					if ( retVal == vsbe_noerror )
					{
						convertedVal = ( uint16_t* )&val;
						avPtr->reli_default = ( float32_t ) ( *convertedVal );
						// **check this if priority array is empty reli default is PV ?
						// avPtr->present_value = avPtr->reli_default;  //as priority array is empty, reli default is
						// present value
						retVal = 0;
					}
					else
					{
						avPtr->reli_default = 0.0F;
					}
				}
				else
				{
					retVal = -1;
				}
			}
		}
		else
		{
			retVal = -1;
		}
	}
	return ( retVal );
}

/** @fn int32_t setPresentValue(uint32_t objId,  float32_t val)
 *   @brief : Writes the Present value
 *   @details :	If the Object is not supported or PV is not writable then respond with error
 *   @param[in] objId Object identifier whose PV has to be writtten
 *   @param[in] val value to be written
 *   @return:0 else error code
 */

int32_t setPresentValueAV( uint32_t objId, float32_t val )
{
	uint32_t instance;
	int32_t retVal = 0;
	int32_t dciRetVal = vsbe_noerror;

	instance = static_cast<uint32_t>( objId ) & static_cast<uint32_t>( OBJ_INSTANCE_MASK );
	DCI_ID_TD dcid;
	DCI_DATA_BLOCK_TD const* data_block;

#ifdef MODBUS_PARAM_READ_FEATURE_ENABLE
	if ( ( any_param_av_instance == instance ) )// special case Any param Value
	{
		uint16_t modbusRegNum = 0U;	// required to access any drive parameter
		BACnetDCIPtr->Read_Param( DCI_AV_ANY_PARAM_NUM_DCID, ( uint8_t* )&modbusRegNum,
								  DCI_ACCESS_GET_RAM_CMD );	// read the value of AV21, i.e. Modbus Reg #
		// Modbus Reg Number is converted to Modbus Display ID instead of Modbus ID by GSB
		dcid = Find_DCID_From_Modbus_ID( modbusRegNum - 1U );	// retrive the corresponding DCID
	}
	else
#endif
	{
		dcid = getDCIDForAVObject( instance );
	}
	data_block = DCI::Get_Data_Block( dcid );

	val = ( val ) * static_cast<float32_t>( m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[instance].scale );
	// Handle the scale

	switch ( data_block->datatype )
	{
		case DCI_DTYPE_STRING8:
		case DCI_DTYPE_BYTE:
		case DCI_DTYPE_UINT8:
		case DCI_DTYPE_BOOL:
			uint8_t convertedToUINT8;
			convertedToUINT8 = ( uint8_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToUINT8,
												   DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_SINT8:
			// int8_t
			int8_t convertedToSINT8;
			convertedToSINT8 = ( int8_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToSINT8,
												   DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_WORD:
		case DCI_DTYPE_UINT16:
			// uint16_t
			uint16_t convertedToUINT16;
			convertedToUINT16 = ( uint16_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToUINT16,
												   DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_SINT16:
			// int16_t
			int16_t convertedToSINT16;
			convertedToSINT16 = ( int16_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToSINT16,
												   DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_DWORD:
		case DCI_DTYPE_UINT32:
			// uint32_t
			uint32_t convertedToUINT32;
			convertedToUINT32 = ( uint32_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToUINT32,
												   DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_SINT32:
			// int32_t
			int32_t convertedToSINT32;
			convertedToSINT32 = ( int32_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToSINT32,
												   DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_UINT64:
			// uint64_t
			uint64_t convertedToUINT64;
			convertedToUINT64 = ( uint64_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToUINT64,
												   DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_SINT64:
			// int64_t
			int64_t convertedToSINT64;
			convertedToSINT64 = ( int64_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToSINT64,
												   DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_FLOAT:
			// float32_t
			float32_t convertedToFLOAT;
			convertedToFLOAT = ( float32_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToFLOAT,
												   DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_DFLOAT:
			// float64_t
			float64_t convertedToDFLOAT;
			convertedToDFLOAT = ( float64_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToDFLOAT,
												   DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_LFLOAT:
		default:
			break;
	}

	if ( dciRetVal == vsbe_noerror )
	{
		retVal = 0;
	}
	else
	{
		retVal = vsbe_writeaccessdenied;// later return specific error
	}
	return ( retVal );
}

/** @fn int32_t setPresentValue(uint32_t instance,  float32_t val)
 *   @brief : Writes the Present value
 *   @details :	If the Object is not supported or PV is not writable then respond with error
 *   @param[in] instance Object instance whose PV has to be writtten
 *   @param[in] val value to be written
 *   @return:0 else error code
 */

int32_t setPresentValueAV_using_Instance( uint32_t instance, float32_t val )
{
	DCI_ID_TD dcid;
	DCI_DATA_BLOCK_TD const* data_block;
	int32_t retVal = 0;
	int32_t dciRetVal = vsbe_noerror;

	dcid = getDCIDForAVObject( instance );
	data_block = DCI::Get_Data_Block( dcid );

	val = ( val ) * static_cast<float32_t>( m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[instance].scale );
	// handle the scale

	switch ( data_block->datatype )
	{
		// Considering the possible AV DCID data types
		case DCI_DTYPE_WORD:
		case DCI_DTYPE_UINT16:
			// uint16_t
			uint16_t convertedToUINT16;
			convertedToUINT16 = ( uint16_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToUINT16,
												   DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_SINT16:
			// int16_t
			int16_t convertedToSINT16;
			convertedToSINT16 = ( int16_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToSINT16,
												   DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_DWORD:
		case DCI_DTYPE_UINT32:
			// uint32_t
			uint32_t convertedToUINT32;
			convertedToUINT32 = ( uint32_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToUINT32,
												   DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_BOOL:
		case DCI_DTYPE_UINT8:
		case DCI_DTYPE_SINT8:
		case DCI_DTYPE_SINT32:
		case DCI_DTYPE_UINT64:
		case DCI_DTYPE_SINT64:
		case DCI_DTYPE_FLOAT:
		case DCI_DTYPE_LFLOAT:
		case DCI_DTYPE_DFLOAT:
		case DCI_DTYPE_BYTE:
		case DCI_DTYPE_STRING8:
		default:
			break;
	}

	if ( dciRetVal == vsbe_noerror )
	{
		retVal = 0;
	}
	else
	{
		retVal = vsbe_writeaccessdenied;// later return specific error
	}
	return ( retVal );
}

/** @fn float32_t getPresentValue(uint32_t objId)
 *   @brief :get the PV of Object
 *   @details :retrieve the PV of object if non supported then repond with error
 *   @param[in] objId Object Identifier whose PV has to be read
 *   @return: value of PV
 */
int32_t getPresentValueAV( uint32_t objId, float32_t& val )
{
	uint32_t instance;
	float32_t temp_val = 0.0F;

	instance = static_cast<uint32_t>( objId ) & static_cast<uint32_t>( OBJ_INSTANCE_MASK );

	DCI_ID_TD dcid;
	DCI_DATA_BLOCK_TD const* data_block;
	int32_t retVal;

#ifdef MODBUS_PARAM_READ_FEATURE_ENABLE
	if ( any_param_av_instance == instance )
	{
		uint16_t modbusRegNum = 0U;
		retVal = BACnetDCIPtr->Read_Param( DCI_AV_ANY_PARAM_NUM_DCID, ( uint8_t* )&modbusRegNum,
										   DCI_ACCESS_GET_RAM_CMD );// read the value of AV21, i.e. Modbus Reg #
		// Modbus Reg Number is converted to Modbus Display ID instead of Modbus ID by GSB
		dcid = Find_DCID_From_Modbus_ID( modbusRegNum - 1U );	// retrive the corresponding DCID
	}
	else
#endif
	{
		dcid = getDCIDForAVObject( instance );
	}
	data_block = DCI::Get_Data_Block( dcid );
	retVal = BACnetDCIPtr->Read_Param( dcid, ( uint8_t* )&val, DCI_ACCESS_GET_RAM_CMD );
	if ( retVal == vsbe_noerror )
	{
		switch ( data_block->datatype )
		{
			case DCI_DTYPE_STRING8:
			case DCI_DTYPE_BYTE:
			case DCI_DTYPE_UINT8:
			case DCI_DTYPE_BOOL:
				uint8_t* convertedToUINT8;
				convertedToUINT8 = reinterpret_cast<uint8_t*>( &val );
				temp_val = static_cast<float32_t>( *convertedToUINT8 );
				val = temp_val;
				break;

			case DCI_DTYPE_SINT8:
				int8_t* convertedToSINT8;
				convertedToSINT8 = reinterpret_cast<int8_t*>( &val );
				temp_val = static_cast<float32_t>( *convertedToSINT8 );
				val = temp_val;
				break;

			case DCI_DTYPE_WORD:
			case DCI_DTYPE_UINT16:
				uint16_t* convertedToUINT16;
				convertedToUINT16 = reinterpret_cast<uint16_t*>( &val );
				temp_val = static_cast<float32_t>( *convertedToUINT16 );
				val = temp_val;
				break;

			case DCI_DTYPE_SINT16:
				int16_t* convertedToSINT16;
				convertedToSINT16 = reinterpret_cast<int16_t*>( &val );
				temp_val = static_cast<float32_t>( *convertedToSINT16 );
				val = temp_val;
				break;

			case DCI_DTYPE_DWORD:
			case DCI_DTYPE_UINT32:
				uint32_t* convertedToUINT32;
				convertedToUINT32 = reinterpret_cast<uint32_t*>( &val );
				temp_val = static_cast<float32_t>( *convertedToUINT32 );
				val = temp_val;
				break;

			case DCI_DTYPE_SINT32:
				int32_t* convertedToSINT32;
				convertedToSINT32 = reinterpret_cast<int32_t*>( &val );
				temp_val = static_cast<float32_t>( *convertedToSINT32 );
				val = temp_val;
				break;

			case DCI_DTYPE_UINT64:
				uint64_t* convertedToUINT64;
				convertedToUINT64 = reinterpret_cast<uint64_t*>( &val );
				temp_val = static_cast<float32_t>( *convertedToUINT64 );
				val = temp_val;
				break;

			case DCI_DTYPE_SINT64:
				int64_t* convertedToSINT64;
				convertedToSINT64 = reinterpret_cast<int64_t*>( &val );
				temp_val = static_cast<float32_t>( *convertedToSINT64 );
				val = temp_val;
				break;

			case DCI_DTYPE_FLOAT:
				float32_t* convertedToFLOAT;
				convertedToFLOAT = reinterpret_cast<float32_t*>( &val );
				temp_val = static_cast<float32_t>( *convertedToFLOAT );
				val = temp_val;
				break;

			case DCI_DTYPE_DFLOAT:
				float64_t* convertedToDFLOAT;
				convertedToDFLOAT = reinterpret_cast<float64_t*>( &val );
				temp_val = static_cast<float32_t>( *convertedToDFLOAT );
				val = temp_val;
				break;

			case DCI_DTYPE_LFLOAT:
			default:
				break;
		}
		val = val / static_cast<float32_t>( m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[instance].scale );
	}
	return ( retVal );
}

/** @fn int32_t getPresentValueAV_from_Instance(uint32_t instance,  float32_t &val)
 *   @brief :get the PV of Object
 *   @details :retrieve the PV of the known instance if non supported then repond with error
 *   @param[in] instance Object instance whose PV has to be read
 *   @return: status of the operation
 */
int32_t getPresentValueAV_from_Instance( uint32_t instance, float32_t& val )
{
	DCI_ID_TD dcid;
	DCI_DATA_BLOCK_TD const* data_block;
	float32_t temp_val = 0.0F;
	int32_t retVal;

	dcid = getDCIDForAVObject( instance );
	data_block = DCI::Get_Data_Block( dcid );
	retVal = BACnetDCIPtr->Read_Param( dcid, ( uint8_t* )&val, DCI_ACCESS_GET_RAM_CMD );
	if ( retVal == vsbe_noerror )
	{	// possible cases in AV are uint16_t and int16_t
		switch ( data_block->datatype )
		{

			case DCI_DTYPE_WORD:
			case DCI_DTYPE_UINT16:
				uint16_t* convertedToUINT16;
				convertedToUINT16 = reinterpret_cast<uint16_t*>( &val );
				temp_val = static_cast<float32_t>( *convertedToUINT16 );
				val = temp_val;
				break;

			case DCI_DTYPE_SINT16:
				int16_t* convertedToSINT16;
				convertedToSINT16 = reinterpret_cast<int16_t*>( &val );
				temp_val = static_cast<float32_t>( *convertedToSINT16 );
				val = temp_val;
				break;

			case DCI_DTYPE_BOOL:
			case DCI_DTYPE_UINT8:
			case DCI_DTYPE_SINT8:
			case DCI_DTYPE_UINT32:
			case DCI_DTYPE_SINT32:
			case DCI_DTYPE_UINT64:
			case DCI_DTYPE_SINT64:
			case DCI_DTYPE_FLOAT:
			case DCI_DTYPE_LFLOAT:
			case DCI_DTYPE_DFLOAT:
			case DCI_DTYPE_BYTE:
			case DCI_DTYPE_DWORD:
			case DCI_DTYPE_STRING8:

			default:
				break;
		}
		val = val / static_cast<float32_t>( m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[instance].scale );
	}
	return ( retVal );
}
