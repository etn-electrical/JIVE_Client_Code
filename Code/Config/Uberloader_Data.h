/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef UBERLOADER_DATA_H
#define UBERLOADER_DATA_H

/* Note the below structure should not exceed size limit of 128bytes */
typedef struct prodinfo_for_boot_s
{
	UINT32 serial_number_nvadd;
	UINT32 hardware_rev_nvadd;
	UINT32 rtu_slave_nvadd;
	UINT32 rtu_baud_rate_nvadd;
	UINT32 rtu_parity_type_nvadd;
	UINT32 keypad_upgrade_nvadd;
	UINT32 keypad_firmware_ok_nvadd;	// use to mark whether keypad firmware ok or not
	// UINT32 keypad_code_start_nvadd; //use to record start address keypad storing in serial flash
	// UINT32 language1_start_nvadd;
	// UINT32 language2_start_nvadd;
	UINT8 reserved[100];	// future expansion
} prodinfo_for_boot;

/* The size of this structure  should not exceed 60 bytes.
   This structure shall be placed exactly before checksum field in flash area*/
typedef struct UBELDR_DESCRIPTION_ST_TD
{
	UINT32 uberloader_app_block_key;	///< This is to verify that the block exists.  It should be
	///< loaded with the above.
	UINT32 boot_firmware_rev;
	UINT32 boot_specification_rev;
	UINT32 uberloader_firmware_id_key;
	UINT8 reserved[44];
} UBELDR_DESCRIPTION_ST_TD;

#define  EXIT_TO_APP                         ( ( uint8_t )0x00 )		/*!<Byte 00 */
#define  JUMP_TO_BOOT                        ( ( uint8_t )0x01 )		/*!<Bit 0 */
#define  SCR_APP_INT_GOOD                    ( ( uint32_t )0x02 )		/*!<Bit 1 */
#define  SCR_APP_UPGRADE                     ( ( uint32_t )0x04 )		/*!<Bit 2 */
#define  CODE_SIGN_KEY_UPGRADE            ( ( uint32_t )0x08 )		/*!<Bit 3 : update code
																	      signing keys
																	      (FVK/IVK/RVK) in cert
																	      store */

typedef union noinit_ram_t
{
	uint8_t byte[8];
	struct boot_msg_t
	{
		uint8_t msg_for_boot;
	} field;

} noinit_ram_st;

// We were using Discovery boards for STM32F437 which does not have
// external NV support. Thus we have managed NV region into RAM.
// When there is no NV support then manage in RAM
// by defining USE_NO_INIT_RAM = 1
// In ECAM, We are using external MRAM for NV support. Thus
// commenting USE_NO_INIT_RAM.
// #if defined( uC_STM32F437_USAGE )
// #define USE_NO_INIT_RAM 1
// #endif

#endif
