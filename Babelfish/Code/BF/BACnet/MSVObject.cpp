/**
 *****************************************************************************************
 *	@file MSVObject.cpp
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "vsbhp.h"
#include "MSVObject.h"
#include "BACnetStackInterface.h"
#include "Ram.h"
#include "BACnet_DCI.h"
#include "BACnetDCIInterface.h"
#include "Binary_Search.h"

BACnetObject MSVObject;

extern BACnet_DCI* BACnetDCIPtr;

/** @fn createMSVObjects(uint8_t numObjects)
 *   @brief :create  MSV objects
 *   @detMSVls :This function has to call before to perform any
 *             operation on MSV object. This function creates the instance of
 * 			  the MSV object
 *   @param[in] numObjects: number of instances
 *   @return:0 on success and -1 otherwise
 */
int32_t createMSVObjects( const BACNET_TARGET_INFO_ST_TD* bacnet_target_info, uint8_t numObjects )
{
	uint8_t i = 0U;
	uint8_t arrayIndex = 0U;

	MSVObject.numObjects = numObjects;
	BACnetMSV* MSVPtr;
	float32_t val = 0.0F;
	uint16_t* convertedVal;
	DCI_ID_TD dcid;
	int32_t retVal = 0;

	if ( numObjects == 0U )
	{
		retVal = -1;
	}
	else
	{
		MSVObject.objects = ( uint32_t* )Ram::Allocate( ( uint16_t )numObjects * sizeof( uint32_t ) );
		if ( MSVObject.objects != NULL )
		{
			for ( i = 0U; i < numObjects; i++ )
			{
				MSVObject.objects[( uint16_t )i] = ( uint32_t )Ram::Allocate( sizeof( BACnetMSV ) );
				if ( MSVObject.objects[( uint16_t )i] != NULL )
				{
					MSVPtr = ( BACnetMSV* )MSVObject.objects[( uint16_t )i];
					/*  MSV is RO only
					   else if ( m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[i].accessType == 2 )
					   {
					   avPtr->isCommendable = TRUE;
					   }
					   else //it is not read only not commendable then its writable
					   {
					   avPtr->isReadOnly = FALSE;
					   avPtr->isCommendable = FALSE;
					   }
					 */
					for ( arrayIndex = 0U; arrayIndex < 16U; arrayIndex++ )
					{
						MSVPtr->priority_array.d[arrayIndex] = nullvalue;	// By default all the priority array is
																			// empty
					}
					dcid = getDCIDForMSVObject( i );
					retVal = BACnetDCIPtr->Read_Param( dcid, ( uint8_t* )&val, DCI_ACCESS_GET_RAM_CMD );
					if ( retVal == vsbe_noerror )
					{
						convertedVal = ( uint16_t* )&val;
						MSVPtr->reli_default = ( float32_t ) ( *convertedVal );
						// **check this if priority array is empty reli default is PV ?
						// avPtr->present_value = avPtr->reli_default;  //as priority array is empty, reli default is
						// present value
						retVal = 0;
					}
					else
					{
						MSVPtr->reli_default = 0.0F;
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

#if 0	// MSV is RO, still kept for future
int32_t setPresentValueAV( uint32_t objId, float32_t val )
{
	uint32_t instance;
	uint32_t retVal = 0;
	int32_t dciRetVal = vsbe_noerror;

	instance = objId & OBJ_INSTANCE_MASK;
	DCI_ID_TD dcid;
	DCI_DATA_BLOCK_TD const* data_block;
	uint16_t modbusRegNum;	// required to access any drive parameter

	if ( ( 7 == instance ) )// special case Any param Value
	{
		BACnetDCIPtr->Read_Param( DCI_uwAnyParamNum_DCID, ( uint8_t* )&modbusRegNum, DCI_ACCESS_GET_RAM_CMD );	// read
																												// the
																												// value
																												// of
																												// AV21,
																												// i.e.
																												// Modbus
																												// Reg #
		// Modbus Reg Number is converted to Modbus Display ID instead of Modbus ID by GSB
		dcid = Find_DCID_From_Modbus_ID( modbusRegNum - 1 );// retrive the corresponding DCID
	}
	else
	{
		dcid = getDCIDForAVObject( instance );
	}
	data_block = DCI::Get_Data_Block( dcid );

	val = ( val ) * ( m_bacnet_target_info->class_list->BACnet_AV_to_DCID_map[instance].scale );// handle the scale

	switch ( data_block->datatype )
	{
		case DCI_DTYPE_STRING8:
		case DCI_DTYPE_BYTE:
		case DCI_DTYPE_UINT8:
		case DCI_DTYPE_BOOL:
			uint8_t convertedToUINT8;
			convertedToUINT8 = ( uint8_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToUINT8, DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_SINT8:
			// int8_t
			int8_t convertedToSINT8;
			convertedToSINT8 = ( int8_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToSINT8, DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_WORD:
		case DCI_DTYPE_UINT16:
			// uint16_t
			uint16_t convertedToUINT16;
			convertedToUINT16 = ( uint16_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToUINT16, DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_SINT16:
			// int16_t
			int16_t convertedToSINT16;
			convertedToSINT16 = ( int16_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToSINT16, DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_DWORD:
		case DCI_DTYPE_UINT32:
			// uint32_t
			uint32_t convertedToUINT32;
			convertedToUINT32 = ( uint32_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToUINT32, DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_SINT32:
			// int32_t
			int32_t convertedToSINT32;
			convertedToSINT32 = ( int32_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToSINT32, DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_UINT64:
			// uint64_t
			uint64_t convertedToUINT64;
			convertedToUINT64 = ( uint64_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToUINT64, DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_SINT64:
			// int64_t
			int64_t convertedToSINT64;
			convertedToSINT64 = ( int64_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToSINT64, DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_FLOAT:
			// float32_t
			float32_t convertedToFLOAT;
			convertedToFLOAT = ( float32_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToFLOAT, DCI_ACCESS_SET_RAM_CMD );
			break;

		case DCI_DTYPE_DFLOAT:
			// float64_t
			float64_t convertedToDFLOAT;
			convertedToDFLOAT = ( float64_t )val;
			dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&convertedToDFLOAT, DCI_ACCESS_SET_RAM_CMD );
			break;

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

#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t getPresentValueMSV_from_Instance( uint32_t instance, uint8_t& val )
{
	DCI_ID_TD dcid;
	DCI_DATA_BLOCK_TD const* data_block;
	int32_t retVal;

	dcid = getDCIDForMSVObject( instance );
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
				convertedToUINT8 = ( uint8_t* )&val;
				val = ( uint8_t )*convertedToUINT8;
				break;

			default:
				break;
		}
	}
	return ( retVal );
}

/** @fn float32_t getPresentValue(uint32_t objId)
 *   @brief :get the MSV of Object
 *   @details :retrieve the MSV of object if non supported then repond with error
 *   @param[in] objId Object Identifier whose MSV has to be read
 *   @return: value of MSV
 */

uint32_t getPresentValueMSV( uint32_t objId, uint8_t& val )
{
	uint32_t instance;
	uint8_t temp_val = 0U;
	DCI_ID_TD dcid;
	DCI_DATA_BLOCK_TD const* data_block;
	int32_t retVal;

	instance = objId & OBJ_INSTANCE_MASK;

	dcid = getDCIDForMSVObject( instance );

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
				temp_val = static_cast<uint8_t>( *convertedToUINT8 );
				val = temp_val;
				break;

			case DCI_DTYPE_SINT8:
				int8_t* convertedToSINT8;
				convertedToSINT8 = reinterpret_cast<int8_t*>( &val );
				temp_val = static_cast<uint8_t>( *convertedToSINT8 );
				val = temp_val;
				break;

			case DCI_DTYPE_WORD:
			case DCI_DTYPE_UINT16:
				uint16_t* convertedToUINT16;
				convertedToUINT16 = reinterpret_cast<uint16_t*>( &val );
				temp_val = static_cast<uint8_t>( *convertedToUINT16 );
				val = temp_val;
				break;

			case DCI_DTYPE_SINT16:
				int16_t* convertedToSINT16;
				convertedToSINT16 = reinterpret_cast<int16_t*>( &val );
				temp_val = static_cast<uint8_t>( *convertedToSINT16 );
				val = temp_val;
				break;

			case DCI_DTYPE_DWORD:
			case DCI_DTYPE_UINT32:
				uint32_t* convertedToUINT32;
				convertedToUINT32 = reinterpret_cast<uint32_t*>( &val );
				temp_val = static_cast<uint8_t>( *convertedToUINT32 );
				val = temp_val;
				break;

			case DCI_DTYPE_SINT32:
				int32_t* convertedToSINT32;
				convertedToSINT32 = reinterpret_cast<int32_t*>( &val );
				temp_val = static_cast<uint8_t>( *convertedToSINT32 );
				val = temp_val;
				break;

			case DCI_DTYPE_UINT64:
				uint64_t* convertedToUINT64;
				convertedToUINT64 = reinterpret_cast<uint64_t*>( &val );
				temp_val = static_cast<uint8_t>( *convertedToUINT64 );
				val = temp_val;
				break;

			case DCI_DTYPE_SINT64:
				int64_t* convertedToSINT64;
				convertedToSINT64 = reinterpret_cast<int64_t*>( &val );
				temp_val = static_cast<uint8_t>( *convertedToSINT64 );
				val = temp_val;
				break;

			case DCI_DTYPE_FLOAT:
				float32_t* convertedToFLOAT;
				convertedToFLOAT = reinterpret_cast<float32_t*>( &val );
				temp_val = static_cast<uint8_t>( *convertedToFLOAT );
				val = temp_val;
				break;

			case DCI_DTYPE_DFLOAT:
				float64_t* convertedToDFLOAT;
				convertedToDFLOAT = reinterpret_cast<float64_t*>( &val );
				temp_val = static_cast<uint8_t>( *convertedToDFLOAT );
				val = temp_val;
				break;

			default:
				break;
		}
		// val = val/BACnetMSVScale[instance].scale; //Nishchal: Not required
	}
	return ( retVal );
}
