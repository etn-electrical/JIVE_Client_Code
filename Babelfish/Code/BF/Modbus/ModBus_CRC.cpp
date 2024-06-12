/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "ModBus_CRC.h"

namespace BF_Mbus
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#if MODBUS_CRC_METHOD == MODBUS_CRC_TABLE_METHOD
static const uint16_t CRC_table_modbus[] =
{
	0x0000U, 0xC0C1U, 0xC181U, 0x0140U, 0xC301U, 0x03C0U, 0x0280U, 0xC241U,
	0xC601U, 0x06C0U, 0x0780U, 0xC741U, 0x0500U, 0xC5C1U, 0xC481U, 0x0440U,
	0xCC01U, 0x0CC0U, 0x0D80U, 0xCD41U, 0x0F00U, 0xCFC1U, 0xCE81U, 0x0E40U,
	0x0A00U, 0xCAC1U, 0xCB81U, 0x0B40U, 0xC901U, 0x09C0U, 0x0880U, 0xC841U,
	0xD801U, 0x18C0U, 0x1980U, 0xD941U, 0x1B00U, 0xDBC1U, 0xDA81U, 0x1A40U,
	0x1E00U, 0xDEC1U, 0xDF81U, 0x1F40U, 0xDD01U, 0x1DC0U, 0x1C80U, 0xDC41U,
	0x1400U, 0xD4C1U, 0xD581U, 0x1540U, 0xD701U, 0x17C0U, 0x1680U, 0xD641U,
	0xD201U, 0x12C0U, 0x1380U, 0xD341U, 0x1100U, 0xD1C1U, 0xD081U, 0x1040U,
	0xF001U, 0x30C0U, 0x3180U, 0xF141U, 0x3300U, 0xF3C1U, 0xF281U, 0x3240U,
	0x3600U, 0xF6C1U, 0xF781U, 0x3740U, 0xF501U, 0x35C0U, 0x3480U, 0xF441U,
	0x3C00U, 0xFCC1U, 0xFD81U, 0x3D40U, 0xFF01U, 0x3FC0U, 0x3E80U, 0xFE41U,
	0xFA01U, 0x3AC0U, 0x3B80U, 0xFB41U, 0x3900U, 0xF9C1U, 0xF881U, 0x3840U,
	0x2800U, 0xE8C1U, 0xE981U, 0x2940U, 0xEB01U, 0x2BC0U, 0x2A80U, 0xEA41U,
	0xEE01U, 0x2EC0U, 0x2F80U, 0xEF41U, 0x2D00U, 0xEDC1U, 0xEC81U, 0x2C40U,
	0xE401U, 0x24C0U, 0x2580U, 0xE541U, 0x2700U, 0xE7C1U, 0xE681U, 0x2640U,
	0x2200U, 0xE2C1U, 0xE381U, 0x2340U, 0xE101U, 0x21C0U, 0x2080U, 0xE041U,
	0xA001U, 0x60C0U, 0x6180U, 0xA141U, 0x6300U, 0xA3C1U, 0xA281U, 0x6240U,
	0x6600U, 0xA6C1U, 0xA781U, 0x6740U, 0xA501U, 0x65C0U, 0x6480U, 0xA441U,
	0x6C00U, 0xACC1U, 0xAD81U, 0x6D40U, 0xAF01U, 0x6FC0U, 0x6E80U, 0xAE41U,
	0xAA01U, 0x6AC0U, 0x6B80U, 0xAB41U, 0x6900U, 0xA9C1U, 0xA881U, 0x6840U,
	0x7800U, 0xB8C1U, 0xB981U, 0x7940U, 0xBB01U, 0x7BC0U, 0x7A80U, 0xBA41U,
	0xBE01U, 0x7EC0U, 0x7F80U, 0xBF41U, 0x7D00U, 0xBDC1U, 0xBC81U, 0x7C40U,
	0xB401U, 0x74C0U, 0x7580U, 0xB541U, 0x7700U, 0xB7C1U, 0xB681U, 0x7640U,
	0x7200U, 0xB2C1U, 0xB381U, 0x7340U, 0xB101U, 0x71C0U, 0x7080U, 0xB041U,
	0x5000U, 0x90C1U, 0x9181U, 0x5140U, 0x9301U, 0x53C0U, 0x5280U, 0x9241U,
	0x9601U, 0x56C0U, 0x5780U, 0x9741U, 0x5500U, 0x95C1U, 0x9481U, 0x5440U,
	0x9C01U, 0x5CC0U, 0x5D80U, 0x9D41U, 0x5F00U, 0x9FC1U, 0x9E81U, 0x5E40U,
	0x5A00U, 0x9AC1U, 0x9B81U, 0x5B40U, 0x9901U, 0x59C0U, 0x5880U, 0x9841U,
	0x8801U, 0x48C0U, 0x4980U, 0x8941U, 0x4B00U, 0x8BC1U, 0x8A81U, 0x4A40U,
	0x4E00U, 0x8EC1U, 0x8F81U, 0x4F40U, 0x8D01U, 0x4DC0U, 0x4C80U, 0x8C41U,
	0x4400U, 0x84C1U, 0x8581U, 0x4540U, 0x8701U, 0x47C0U, 0x4680U, 0x8641U,
	0x8201U, 0x42C0U, 0x4380U, 0x8341U, 0x4100U, 0x81C1U, 0x8081U, 0x4040U
};
#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
/*uint16_t MODBUS_CRC_Calc_On_Byte( uint8_t data, uint16_t prev_crc )
   {
 #if MODBUS_CRC_METHOD == MODBUS_CRC_TABLE_METHOD

    return ( ( prev_crc >> 8U ) ^ CRC_table_modbus[ ( prev_crc ^ data ) & 0xffU] );

 #else

    SPLIT_UINT16 temp_crc;

    temp_crc.u16 = prev_crc ^ data;

    for ( uint8_t counter = 0U; counter < 8U; counter++ )
    {
        temp_crc.u16 = temp_crc.u16>>1U;
        if ( temp_crc.u8[0] != 0U )
        {
            temp_crc.u16 = temp_crc.u16 ^ 0xA001U;
        }
    }

    return ( temp_crc.u16 );

 #endif
   }*/

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t MODBUS_CRC_Calc_On_String( uint8_t* data, uint16_t length )
{
	BF_Lib::SPLIT_UINT16 temp_crc;
	uint_fast16_t i = length;

	temp_crc.u16 = MODBUS_CRC_INIT;

#if MODBUS_CRC_METHOD == MODBUS_CRC_TABLE_METHOD

	while ( i > 0U )
	{
		i--;
		temp_crc.u16 = ( temp_crc.u8[1] ^ CRC_table_modbus[( temp_crc.u16 ^ *data ) & 0xffU] );
		data++;
	}

#else

	while ( i-- )
	{
		temp_crc.u16 = temp_crc.u16 ^ *data++;

		for ( uint8_t counter = 0U; counter < 8U; counter++ )
		{
			if ( temp_crc.u8[0] & 0x01U )
			{
				temp_crc.u16 = temp_crc.u16 >> 1U;
				temp_crc.u16 = temp_crc.u16 ^ 0xA001U;
			}
			else
			{
				temp_crc.u16 = temp_crc.u16 >> 1U;
			}
		}
	}

#endif

	return ( temp_crc.u16 );
}

/*
 *****************************************************************************************
 * -----	Function Header	-----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool MODBUS_CRC_Check_String_For_Good_CRC( uint8_t* data, uint16_t length )
{
	return ( MODBUS_CRC_Calc_On_String( data, length ) ==
			 MODBUS_OVERRUN_CRC_GOOD );	// I have to check this out.  Not sure if it is inverted.
}

/*
 *****************************************************************************************
 * -----	Function Header	-----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
/*uint8_t MODBUS_LRC_Calc_On_Byte( uint8_t data, uint8_t prev_lrc )
   {
    return ( prev_lrc + data );
   }*/

/*
 *****************************************************************************************
 * -----	Function Header	-----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uint8_t MODBUS_LRC_Calc_On_String( uint8_t* data, uint16_t length )
{
	uint8_t lrc_val = 0U;
	uint_fast16_t i = length;
	int8_t temp_val;

	while ( i > 0U )
	{
		i--;
		lrc_val += *data;
		data++;
	}

	temp_val = static_cast<int8_t>( lrc_val );
	temp_val = -temp_val;
	return ( static_cast<uint8_t>( temp_val ) );
}

/*
 *****************************************************************************************
 * -----	Function Header	-----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool MODBUS_LRC_Check_String_For_Good_LRC( uint8_t* data, uint16_t length )
{
	return ( ( MODBUS_LRC_Calc_On_String( data, length - 1U ) == data[length - 1U] ) );
}

}	/* End namespace BF_Mbus for this module*/
