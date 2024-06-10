/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "uC_Code_Range.h"
#include "Stdlib_MV.h"

/*
 *****************************************************************************************
 *		Declarations
 *****************************************************************************************
 */
extern "C"
{
extern uint8_t __APP_FW_START__;
extern uint8_t __APP_FW_END__;
extern uint8_t __ICFEDIT_region_ROM_end__;
extern uint8_t __FW_CHECKSUM_START__;
extern uint8_t __APP_REAL_FW_START__;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_App_Fw_Start_Address( void )
{
	return ( ( uint8_t* ) ( &( __APP_FW_START__ ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_App_Real_Fw_Start_Address( void )
{
	return ( ( uint8_t* ) ( &( __APP_REAL_FW_START__ ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_App_Fw_End_Address( void )
{
	return ( ( uint8_t* ) ( &( __APP_FW_END__ ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_ROM_End_Address( void )
{
	return ( ( uint8_t* )&( __ICFEDIT_region_ROM_end__ ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* uC_App_Fw_Crc_Start_Address( void )
{
	return ( ( uint8_t* ) ( &( __FW_CHECKSUM_START__ ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t uC_CODE_RANGE_CRC_Val( void )
{
	uint16_t crc_val;
	uint8_t* crc_src;

	crc_src = reinterpret_cast<uint8_t*>( uC_App_Fw_End_Address() ) + 1U;
	crc_src -= sizeof ( crc_val );
	BF_Lib::Copy_String( reinterpret_cast<uint8_t*>( &crc_val ), crc_src, sizeof( uint16_t ) );

	return ( crc_val );
}
