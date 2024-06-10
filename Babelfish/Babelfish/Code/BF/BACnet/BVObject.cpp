/**
 *****************************************************************************************
 *	@file BVObject.cpp
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "string.h"
#include "vsbhp.h"
#include "BVObject.h"
#include "BACnetStackInterface.h"
#include "Ram.h"
#include "BACnet_DCI.h"
#include "BACnetDCIInterface.h"
#include "Binary_Search.h"
BACnetObject bvObject;
extern BACnet_DCI* BACnetDCIPtr;
static const BACNET_TARGET_INFO_ST_TD* m_bacnet_target_info;

/** @fn createBvObjects(uint8_t numObjects)
 *   @brief :create  BV objects
 *   @details :This function has to call before to perform any
 *             operation on BV object. This function creates the instancess of
 * 			  the AV object
 *   @param[in] numObjects: number of instances
 *   @return:0 on success and -1 otherwise
 */
int32_t createBvObjects( const BACNET_TARGET_INFO_ST_TD* bacnet_target_info, uint8_t numObjects )
{
	uint8_t i = 0U;
	uint8_t arrIndex = 0U;
	BACnetBV* bvPtr;
	uint32_t valTotal = 0U;
	DCI_ID_TD dcid;
	uint8_t bitNumber;
	int32_t retVal = 0;

	m_bacnet_target_info = bacnet_target_info;
	bvObject.numObjects = numObjects;
	if ( numObjects == 0U )
	{
		retVal = -1;
	}
	else
	{

		bvObject.objects = ( uint32_t* )Ram::Allocate( ( uint16_t )numObjects * sizeof( uint32_t ) );

		if ( bvObject.objects != NULL )
		{
			for ( i = 0U; i < numObjects; i++ )
			{
				bvObject.objects[( uint16_t )i] = ( uint32_t )Ram::Allocate( sizeof( BACnetBV ) );

				if ( bvObject.objects[( uint16_t )i] != NULL )
				{
					bvPtr = ( BACnetBV* )bvObject.objects[( uint16_t )i];
					for ( arrIndex = 0U; arrIndex < 16U; arrIndex++ )
					{
						bvPtr->priority_array[arrIndex] = nullvalue;
					}
					dcid = getDCIDForBVObject( i, bitNumber );
					retVal = BACnetDCIPtr->Read_Param( dcid, ( uint8_t* )&valTotal,
													   DCI_ACCESS_GET_RAM_CMD );
					if ( retVal == vsbe_noerror )
					{
						bvPtr->reli_default = Test_Bit( valTotal, bitNumber );
						retVal = 0U;
					}
					else
					{
						bvPtr->reli_default = 0U;
					}
					// bvPtr->reli_default = 1;
					// bvPtr->present_value = bvPtr->reli_default;
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
int32_t setPresentValueBV( uint32_t objId, uint8_t val )
{
	// uint32_t objType, instance;
	uint32_t instance;
	int32_t retVal = 0U;
	uint8_t bitNumber = 0U;
	int32_t dciRetVal = vsbe_noerror;
	DCI_ID_TD dcid;
	DCI_DATA_BLOCK_TD const* data_block;

	// objType = (objId >> SHIFT_GET_OBJECT_ID);
	instance = objId & OBJ_INSTANCE_MASK;

	if ( m_bacnet_target_info->class_list->BACnet_BV_to_DCID_map[( uint16_t )instance].accessType !=
		 IS_READABLE )
	{
		dcid = getDCIDForBVObject( instance, bitNumber );
		data_block = DCI::Get_Data_Block( dcid );

		switch ( data_block->datatype )
		{
			case DCI_DTYPE_STRING8:
			case DCI_DTYPE_BYTE:
			case DCI_DTYPE_UINT8:
			case DCI_DTYPE_BOOL:
			{
				uint8_t valTotalUINT8 = 0U;
				dciRetVal = BACnetDCIPtr->Read_Param( dcid, ( uint8_t* )&valTotalUINT8,
													  DCI_ACCESS_GET_RAM_CMD );
				if ( vsbe_noerror == dciRetVal )
				{
					if ( 1U == val )
					{
						Set_Bit( valTotalUINT8, bitNumber );
					}
					else if ( 0U == val )
					{
						Clr_Bit( valTotalUINT8, bitNumber );
					}
					else
					{
						retVal = vsbe_inconsistentparameters;
					}
					dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&valTotalUINT8,
														   DCI_ACCESS_SET_RAM_CMD );
				}
			}
			break;

			case DCI_DTYPE_WORD:
			case DCI_DTYPE_UINT16:
			{
				uint16_t valTotalUINT16 = 0U;
				dciRetVal = BACnetDCIPtr->Read_Param( dcid, ( uint8_t* )&valTotalUINT16,
													  DCI_ACCESS_GET_RAM_CMD );
				if ( vsbe_noerror == dciRetVal )
				{
					if ( 1U == val )
					{
						Set_Bit( valTotalUINT16, bitNumber );
					}
					else if ( 0U == val )
					{
						Clr_Bit( valTotalUINT16, bitNumber );
					}
					else
					{
						retVal = vsbe_inconsistentparameters;
					}
					dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&valTotalUINT16,
														   DCI_ACCESS_SET_RAM_CMD );
				}
			}
			break;

			case DCI_DTYPE_DWORD:
			case DCI_DTYPE_UINT32:
			{
				uint32_t valTotalUINT32 = 0U;
				dciRetVal = BACnetDCIPtr->Read_Param( dcid, ( uint8_t* )&valTotalUINT32,
													  DCI_ACCESS_GET_RAM_CMD );
				if ( vsbe_noerror == dciRetVal )
				{
					if ( 1U == val )
					{
						Set_Bit( valTotalUINT32, bitNumber );
					}
					else if ( 0U == val )
					{
						Clr_Bit( valTotalUINT32, bitNumber );
					}
					else
					{
						retVal = vsbe_inconsistentparameters;
					}
					dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&valTotalUINT32,
														   DCI_ACCESS_SET_RAM_CMD );
				}
			}
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
			retVal = vsbe_writeaccessdenied;
		}
	}
	else
	{
		retVal = vsbe_writeaccessdenied;
	}

	return ( retVal );
}

// Set the known BV instance value using below function.
int32_t setPresentValueBV_using_Instance( uint32_t instance, uint8_t val )
{
	uint8_t bitNumber = 0U;
	int32_t retVal = 0;
	DCI_ID_TD dcid;
	DCI_DATA_BLOCK_TD const* data_block;
	int32_t dciRetVal = vsbe_noerror;

	dcid = getDCIDForBVObject( instance, bitNumber );
	data_block = DCI::Get_Data_Block( dcid );

	if ( m_bacnet_target_info->class_list->BACnet_BV_to_DCID_map[( uint16_t )instance].accessType !=
		 IS_READABLE )
	{
		dcid = getDCIDForBVObject( instance, bitNumber );
		data_block = DCI::Get_Data_Block( dcid );

		switch ( data_block->datatype )
		{
			case DCI_DTYPE_STRING8:
			case DCI_DTYPE_BYTE:
			case DCI_DTYPE_UINT8:
			case DCI_DTYPE_BOOL:
			{
				uint8_t valTotalUINT8 = 0U;
				dciRetVal = BACnetDCIPtr->Read_Param( dcid, ( uint8_t* )&valTotalUINT8,
													  DCI_ACCESS_GET_RAM_CMD );
				if ( vsbe_noerror == dciRetVal )
				{
					if ( 1U == val )
					{
						Set_Bit( valTotalUINT8, bitNumber );
					}
					else if ( 0U == val )
					{
						Clr_Bit( valTotalUINT8, bitNumber );
					}
					else
					{
						retVal = vsbe_inconsistentparameters;
					}
					dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&valTotalUINT8,
														   DCI_ACCESS_SET_RAM_CMD );
				}
			}
			break;

			case DCI_DTYPE_WORD:
			case DCI_DTYPE_UINT16:
			{
				uint16_t valTotalUINT16 = 0U;
				dciRetVal = BACnetDCIPtr->Read_Param( dcid, ( uint8_t* )&valTotalUINT16,
													  DCI_ACCESS_GET_RAM_CMD );
				if ( vsbe_noerror == dciRetVal )
				{
					if ( 1U == val )
					{
						Set_Bit( valTotalUINT16, bitNumber );
					}
					else if ( 0U == val )
					{
						Clr_Bit( valTotalUINT16, bitNumber );
					}
					else
					{
						retVal = vsbe_inconsistentparameters;
					}
					dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&valTotalUINT16,
														   DCI_ACCESS_SET_RAM_CMD );
				}
			}
			break;

			case DCI_DTYPE_DWORD:
			case DCI_DTYPE_UINT32:
			{
				uint32_t valTotalUINT32 = 0U;
				dciRetVal = BACnetDCIPtr->Read_Param( dcid, ( uint8_t* )&valTotalUINT32,
													  DCI_ACCESS_GET_RAM_CMD );
				if ( vsbe_noerror == dciRetVal )
				{
					if ( 1U == val )
					{
						Set_Bit( valTotalUINT32, bitNumber );
					}
					else if ( 0U == val )
					{
						Clr_Bit( valTotalUINT32, bitNumber );
					}
					else
					{
						retVal = vsbe_inconsistentparameters;
					}
					dciRetVal = BACnetDCIPtr->Write_Param( dcid, ( uint8_t* )&valTotalUINT32,
														   DCI_ACCESS_SET_RAM_CMD );
				}
			}
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
			retVal = vsbe_writeaccessdenied;
		}
	}
	else
	{
		retVal = vsbe_writeaccessdenied;
	}

	return ( retVal );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t getPresentValueBV( uint32_t objId )
{
	// uint32_t objType, instance;
	uint32_t instance;
	uint8_t val = 0U;
	uint32_t valTotal = 0U;

	// objType = (objId >> SHIFT_GET_OBJECT_ID);
	instance = objId & OBJ_INSTANCE_MASK;
	uint8_t bitNumber = 0U;
	DCI_ID_TD dcid;

	dcid = getDCIDForBVObject( instance, bitNumber );

	BACnetDCIPtr->Read_Param( dcid, ( uint8_t* )&valTotal, DCI_ACCESS_GET_RAM_CMD );
	val = Test_Bit( valTotal, bitNumber );

	return ( val );
}

// Get the present value of the Known Instance using this function.
uint8_t getPresentValueBV_from_Instance( uint32_t instance )
{
	uint8_t val = 0U;
	uint32_t valTotal = 0U;
	uint8_t bitNumber = 0U;
	DCI_ID_TD dcid;

	dcid = getDCIDForBVObject( instance, bitNumber );

	BACnetDCIPtr->Read_Param( dcid, ( uint8_t* )&valTotal, DCI_ACCESS_GET_RAM_CMD );
	val = Test_Bit( valTotal, bitNumber );

	return ( val );
}
