/**
 *****************************************************************************************
 *	@file AIObject.cpp
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "vsbhp.h"
#include "AIObject.h"
#include "BACnetStackInterface.h"
#include "Ram.h"
#include "BACnet_DCI.h"
#include "BACnetDCIInterface.h"
#include "Binary_Search.h"

BACnetObject aiObject;

extern BACnet_DCI* BACnetDCIPtr;
static const BACNET_TARGET_INFO_ST_TD* m_bacnet_target_info;

/** @fn createAIObjects(uint8_t numObjects)
 *   @brief :create  AI objects
 *   @details :This function has to call before to perform any
 *             operation on AI object. This function creates the instancess of
 * 			  the AI object
 *   @param[in] numObjects: number of instances
 *   @return:0 on success and -1 otherwise
 */
int32_t createAIObjects( const BACNET_TARGET_INFO_ST_TD* bacnet_target_info, uint8_t numObjects )
{
	uint8_t i = 0U;

	aiObject.numObjects = numObjects;
	BACnetAI* aiPtr;
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
		aiObject.objects = ( uint32_t* )Ram::Allocate( ( uint16_t )numObjects * sizeof( uint32_t ) );
		if ( aiObject.objects != NULL )
		{
			for ( i = 0U; i < numObjects; i++ )
			{
				aiObject.objects[i] = ( uint32_t )Ram::Allocate( sizeof( BACnetAI ) );
				if ( aiObject.objects[i] != 0U )
				{
					aiPtr = ( BACnetAI* )aiObject.objects[i];

					/*  AI is RO only
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
					// Get default value of COV increment for each AI object
					aiPtr->covincrement = m_bacnet_target_info->class_list->BACnet_AI_to_DCID_map[i]
						.cov_increment;

					// for(arrayIndex = 0U; arrayIndex < 16 ; arrayIndex++)
					// {
					// aiPtr->priority_array.d[arrayIndex] = nullvalue; //By default all the priority array is empty
					// }
					dcid = getDCIDForAIObject( i );
					retVal = BACnetDCIPtr->Read_Param( dcid, ( uint8_t* )&val, DCI_ACCESS_GET_RAM_CMD );
					if ( retVal == vsbe_noerror )
					{
						convertedVal = ( uint16_t* )&val;
						aiPtr->reli_default = ( float32_t )( *convertedVal );
						// **check this if priority array is empty reli default is PV ?
						// avPtr->present_value = avPtr->reli_default;  //as priority array is empty, reli default is
						// present value
						retVal = 0;
					}
					else
					{
						aiPtr->reli_default = 0.0F;
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

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int32_t getPresentValueAI_from_Instance( uint32_t instance, float32_t& val )
{
	DCI_ID_TD dcid;
	float32_t temp_val = 0.0F;
	DCI_DATA_BLOCK_TD const* data_block;
	int32_t retVal;

	dcid = getDCIDForAIObject( instance );
	data_block = DCI::Get_Data_Block( dcid );
	retVal = BACnetDCIPtr->Read_Param( dcid, ( uint8_t* )&val, DCI_ACCESS_GET_RAM_CMD );
	if ( retVal == vsbe_noerror )
	{
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
		val = val / static_cast<float32_t>( m_bacnet_target_info->class_list->BACnet_AI_to_DCID_map[instance].scale );
	}
	return ( retVal );
}

/** @fn float32_t getPresentValue(uint32_t objId)
 *   @brief :get the AI of Object
 *   @details :retrieve the AI of object if non supported then repond with error
 *   @param[in] objId Object Identifier whose AI has to be read
 *   @return: value of AI
 */

int32_t getPresentValueAI( uint32_t objId, float32_t& val )
{
	uint32_t instance;
	float32_t temp_val = 0.0F;
	DCI_ID_TD dcid;
	DCI_DATA_BLOCK_TD const* data_block;
	int32_t retVal;

	instance = static_cast<uint32_t>( objId ) & static_cast<uint32_t>( OBJ_INSTANCE_MASK );

	dcid = getDCIDForAIObject( instance );

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
				temp_val = ( float32_t )*convertedToSINT8;
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
		val = val / static_cast<float32_t>( m_bacnet_target_info->class_list->BACnet_AI_to_DCID_map[instance].scale );
	}
	return ( retVal );
}
