/**
 **********************************************************************************************
 * @file            Prod_Spec_MEM.cpp  product specific initialization
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the initialization routines specific to Memory
 *                  component.
 *
 *
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

/*
 *****************************************************************************************
 *		Include Files
 *****************************************************************************************
 */
#include "Includes.h"
#include "NV_Mem.h"
#include "Prod_Spec_MEM.h"
#include "DCI_Owner.h"
#include "NV_Ctrl.h"
#include "Ram.h"
#include "Ram_Static.h"
#include "Ram_Hybrid_ESP32.h"
#include "Scratch_Ram.h"
#include "DCI_NV_Mem.h"
#include "Flash_NV.h"
#include "uC_Code_Range.h"
#include "uC_Flash.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "uC_Code_Range.h"
#include "Prod_Spec_LTK_ESP32.h"
#include "uC_Fw_Code_Range.h"
#if defined NV_CRYPTO
#include "NV_Crypto.h"
#include "Crypt_Aes_CBC_Matrix.h"
#endif

#include "Prod_Spec_NV_Refresh.h"
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
/* The size of the DCI variable space. */
static const uint32_t SCRATCH_RAM_SIZE = 0x40U;
const uint32_t DCI_RAM_BLOCK_SIZE = Mem_Aligned_Size( DCI_TOTAL_DCID_RAM_SIZE );

/* This is a key which is used to reset all the DCI parameters.  This can be used
   to refresh the NV when the DCI database gets shuffled.  This is mostly used during
   the development phase for when the NV memory is re-arranged.
 */
const uint8_t nv_flash_flush_key = 0x01U;
const uint8_t DCI_NV_FRAM_FACTORY_RESET_KEY = 0x64U;
const uint8_t DCI_NV_RAM_FACTORY_RESET_KEY = 0x0U;

#if defined NV_CRYPTO
const uint8_t nv_crypto_flush_key = 0x01U;

const NV_Ctrls::NV_Crypto::nv_crypto_config_t NV_CRYPTO_CFG =
{
	NV_DATA_START_NVCTRL1_NVADD& NV_ADDRESS_CTRL_ADDRESS_MASK,
	/* NV_DATA_START_NVCTRL1_NVADD,- Start Address of NV_Crypto */
	NV_DATA_END_NVCTRL1_NVADD - NV_DATA_START_NVCTRL1_NVADD,
	/* NV_DATA_END_NVCTRL1_NVADD,	- End Address of NV_Crypto */
	NV_CRYPTO_META_DATA_END_NVCTRL1_NVADD - NV_DATA_END_NVCTRL1_NVADD,
	/* Extra bytes added to verifying crypto result (decrypt) is good */
	CRYPTO_NV_BLOCK_NVADD & NV_ADDRESS_CTRL_ADDRESS_MASK,
	/* nv_ctrl_offset - required to find address of CRYPTO_NV_BLOCK */
};

/* Crypt object ( which is going to use these keys ) is located at /Babelfish/Lib/Crypt.
   These keys are used for encryption or decryption of Crypto_Block present inside real NV control.
   Note: Following key values are randomly choosen. Adopter can use any key values.*/
// 16 Bytes AES 128 Key
static const uint8_t crypt_aes_key[16U] =
{ 14, 27, 65, 3, 8, 9, 12, 23, 99, 44, 77, 99, 44, 1, 65, 78 };
// 16 Bytes Initialization vector
static const uint8_t crypt_aes_iv[16U] =
{ 95, 1, 56, 89, 78, 56, 73, 19, 27, 34, 61, 62, 100, 180, 149, 20 };
static NV_Ctrl* crypto_space_nv_ctrl = nullptr;
#endif

/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
BF_Lib::Ram_Hybrid_ESP32* hybrid_ram_ctrl = nullptr;
bool_t nv_working_status = false;
static uint8_t dci_ram_block[DCI_RAM_BLOCK_SIZE];
static NV_Ctrl* nv_ctrl = nullptr;

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */
void PROD_SPEC_NV_flash_Callback( NV_Ctrl::cback_param_t param, NV_Ctrl::nv_status_t result );

#ifdef PKI_SUPPORT
static NV_Ctrl* scratch_space_nv_ctrl = nullptr;
static NV_Ctrl* app_space_nv_ctrl = nullptr;
#ifdef WEB_SERVER_SUPPORT
static NV_Ctrl* web_space_nv_ctrl = nullptr;
#endif
static NV_Ctrl* cert_space_nv_ctrl = nullptr;
#endif

void PROD_SPEC_MEM_Init( void )
{
	/* Target NV init */
	PROD_SPEC_Target_NV_Init();
	/* NV Refresh functionality help set the default value to newly added DCI ( nv supported ) at  first power-up*/
	PROD_SPEC_NV_REFRESH_Init();
#ifdef PKI_SUPPORT
	/* Application NV init */
	PROD_SPEC_MEM_APP_Space_Init();
	/* Web NV init */
#ifdef WEB_SERVER_SUPPORT
	PROD_SPEC_MEM_WEB_Space_Init();
#endif
	/* scratch NV init */
	PROD_SPEC_MEM_Scratch_Space_Init();

#ifdef REST_I18N_LANGUAGE
	/* lang NV init */
	PROD_SPEC_MEM_Lang_Pack_Init();
#endif	// REST_I18N_LANGUAGE

	/* Device certificate NV init */
	PROD_SPEC_MEM_Dev_Cert_Init();
#endif	/// PKI_SUPPORT
}

/**
 **********************************************************************************************
 * @brief                     Product Specific RAM Initialization
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_Target_RAM_Init( void )
{
	Ram::Init();

	hybrid_ram_ctrl = new BF_Lib::Ram_Hybrid_ESP32( sizeof( UINT32 ) );

	Ram::Assign_Block( hybrid_ram_ctrl, MAIN_RAM_BLOCK );

	Ram::Assign_Block( new BF_Lib::Ram_Static( dci_ram_block,
											   sizeof ( dci_ram_block ),
											   sizeof( uint8_t ) ), DCI_RAM_BLOCK );

	hybrid_ram_ctrl->Enable_Dynamic_Mem();
	Ram::Set_Default_New_Op_Alloc_Block( MAIN_RAM_BLOCK );

	/* This is the scratch ram space.  Scratch RAM is a module which provides a variable length
	   stack space inside of a function.  It is for when you need a string of unknown length
	   and don't know how much to pull from the stack itself.
	 */
	BF_Lib::Scratch_Ram::Init( Mem_Aligned_Size( SCRATCH_RAM_SIZE ), sizeof( uint_fast8_t ) );
}

/**
 **********************************************************************************************
 * @brief                     Product Specific Nonvolatile memory Initialization
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_Target_NV_Init( void )
{
	/// ESP32 NVS_flash initialization
	esp_err_t ret = nvs_flash_init();

	if ( ( ret == ESP_ERR_NVS_NO_FREE_PAGES ) || ( ret == ESP_ERR_NVS_NEW_VERSION_FOUND ) )
	{
		ESP_ERROR_CHECK( nvs_flash_erase() );
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK( ret );
	/* Commenting this code because it is restricting to connect with SNTP.*/

	/* Moving DCI_NV_FLASH_CONFIG from global to local to remove this file from .ctors list which are getting executed
	 * before Main(). This is causing problem for Update_Ota_Data functionality.
	 * If we make this as global, flash chip information will not be updated while reading the partition.*/
	static const NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD DCI_NV_FLASH_CONFIG =
	{
		reinterpret_cast<uint32_t>( uC_DCI_NV_Start_Address() ),
		( reinterpret_cast<uint32_t>( uC_DCI_NV_Start_Address() ) +
		  reinterpret_cast<uint32_t>( uC_DCI_NV_End_Address() ) + 1U ) / 2,
		reinterpret_cast<uint32_t>( uC_DCI_NV_End_Address() ),
	};

	uint8_t nv_index = 0;
	NV_Ctrl* flash_space_nv_ctrl = nullptr;

#if defined NV_CRYPTO
	/* NV Memory + NV_CRYPTO */
	static const uint8_t NV_CTRL_MAX_CTRLS = 2U;
#else
	/* NV Memory or Ram_Nv */
	static const uint8_t NV_CTRL_MAX_CTRLS = 1U;
#endif
	static NV_Ctrl* nv_ctrl_list[NV_CTRL_MAX_CTRLS];
	static uint8_t nv_flush_keys[NV_CTRL_MAX_CTRLS] = { 0 };

	nv_ctrl = new NV_Ctrls::uC_Flash(
		reinterpret_cast<NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD const*>( &DCI_NV_FLASH_CONFIG ),
		nullptr );


	flash_space_nv_ctrl = new NV_Ctrls::Flash_NV( nv_ctrl, &DCI_NV_FLASH_CONFIG, NV_DATA_CTRL0_RANGE_SIZE, 10U, false,
												  PROD_SPEC_NV_flash_Callback );

	if ( flash_space_nv_ctrl != nullptr )
	{
		nv_working_status = true;
	}

	if ( nv_working_status == true )
	{
		nv_ctrl_list[nv_index] = flash_space_nv_ctrl;
		nv_flush_keys[nv_index] = nv_flash_flush_key;
	}
	/* NV_Crypto inititialization */
#if defined NV_CRYPTO

	Crypt* crypt_obj = new Crypt_Aes_CBC_Matrix( const_cast<uint8_t const*>( &crypt_aes_key[0] ),
												 Crypt::AES_128_KEY,
												 const_cast<uint8_t const*>( &crypt_aes_iv[0] ),
												 Crypt::AES_IV_SIZE_KEY );


	crypto_space_nv_ctrl = new NV_Ctrls::NV_Crypto( nv_ctrl_list[0], crypt_obj, &NV_CRYPTO_CFG,
													PROD_SPEC_NV_Crypto_Callback );

	nv_ctrl_list[nv_index + 1] = crypto_space_nv_ctrl;
	nv_flush_keys[nv_index + 1] = nv_crypto_flush_key;		///< Used same key for NV_Crypto block
#endif

	NV::NV_Mem::Init( &nv_ctrl_list[0], NV_CTRL_MAX_CTRLS, &nv_flush_keys[0] );
	// coverity[leaked_storage]

	// If DCI Flash NV is used, this initialization should be done after Flash NV init
	DCI_NV_Mem::Init( DCI_NV_FRAM_FACTORY_RESET_KEY );
}

/*
 **********************************************************************************************
 * @brief                     Product Specific NV Flash callback to notify error events
 * @param[in] param           Callback parameter
 * @param[in] result          Error type
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_NV_flash_Callback( NV_Ctrl::cback_param_t param, NV_Ctrl::nv_status_t result )
{
	/* Adapter should handle error conditions here */
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_NV_Crypto_Callback( NV_Ctrl::cback_param_t param, NV_Ctrl::nv_status_t result )
{
	/* Adopter should handle error conditions here */
}

/**
 **********************************************************************************************
 * @brief                     Product Specific Scratch space area init
 * @param[in] void            none
 * @return[out] void		  none
 * @n
 **********************************************************************************************
 */

#ifdef PKI_SUPPORT

void PROD_SPEC_MEM_Scratch_Space_Init( void )
{
	/* Moving SCRATCH_AREA_CONFIG from global to local to remove this file from .ctors list which are getting executed
	 * before Main(). This is causing problem for Update_Ota_Data functionality.
	 * If we make this as global, flash chip information will not be updated while reading the partition.*/
	static const NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD SCRATCH_AREA_CONFIG =
	{
		reinterpret_cast<uint32_t>( uC_Scratch_Area_Start_Address() ),
		reinterpret_cast<uint32_t>( uC_Scratch_Area_Start_Address() ),
		reinterpret_cast<uint32_t>( uC_Scratch_Area_End_Address() ),
	};

#if defined ( LOGGING_ON_EXT_FLASH ) || defined ( SCRATCH_ON_EXT_FLASH )
	static uC_USER_IO_STRUCT const* const SERIAL_FLASH_CS_CTRLS[1U] = { &SERIAL_FLASH_SPI_CS_IO_CTRL };
	serial_flash_ctrl = new uC_SPI( SERIAL_FLASH_SPI_PORT, true, &SERIAL_FLASH_CS_CTRLS[0],
									1U, &SERIAL_FLASH_SPI_PIO, TRUE );
#endif

	scratch_space_nv_ctrl = new NV_Ctrls::uC_Flash(
		reinterpret_cast<NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD const*>( &SCRATCH_AREA_CONFIG ),
		nullptr );
}

/**
 **********************************************************************************************
 * @brief Get nv control handle of memory space where scratch space defined
 * @param[out] nv_ctrl : Pointer to scratch space NV control.
 * @return None
 **********************************************************************************************
 */
void PROD_SPEC_MEM_Get_Scratch_NV_Handle( NV_Ctrl** nv_ctrl )
{
	*nv_ctrl = scratch_space_nv_ctrl;

}

/**
 **********************************************************************************************
 * @brief                     Product Specific Scratch space area init
 * @param[in] void            none
 * @return[out] void	      none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_MEM_APP_Space_Init( void )
{
	/* Moving APP_AREA_CONFIG from global to local to remove this file from .ctors list which are getting executed
	 * before Main(). This is causing problem for Update_Ota_Data functionality.
	 * If we make this as global, flash chip information will not be updated while reading the partition.*/
	static const NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD APP_AREA_CONFIG =
	{
		reinterpret_cast<uint32_t>( uC_App_Fw_Start_Address() ),
		reinterpret_cast<uint32_t>( uC_App_Fw_End_Address() ),
		reinterpret_cast<uint32_t>( uC_App_Fw_End_Address() ),
	};

	app_space_nv_ctrl = new NV_Ctrls::uC_Flash( &APP_AREA_CONFIG, nullptr );
}

/**
 **********************************************************************************************
 * @brief Get nv control handle of memory space where App space defined
 * @param[out] nv_ctrl : Pointer to app space NV control.
 * @return None
 **********************************************************************************************
 */
void PROD_SPEC_MEM_Get_APP_NV_Handle( NV_Ctrl** nv_ctrl )
{
	*nv_ctrl = app_space_nv_ctrl;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_MEM_Dev_Cert_Init( void )
{
	/* Moving DEVICE_CERT_FLASH_CONFIG from global to local to remove this file from .ctors list which are getting
	 * executed before Main(). This is causing problem for Update_Ota_Data functionality.
	 * If we make this as global, flash chip information will not be updated while reading the partition.*/
	static const NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD DEVICE_CERT_FLASH_CONFIG =
	{
		reinterpret_cast<uint32_t>( uC_Device_Cert_Start_Address() ),
		reinterpret_cast<uint32_t>( uC_Device_Cert_Start_Address() ),
		reinterpret_cast<uint32_t>( uC_Device_Cert_End_Address() ),
	};

	/* This feature is currently only tested for ESP32-MINI-1 Dev Board. */
	cert_space_nv_ctrl = new NV_Ctrls::uC_Flash(
		reinterpret_cast<NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD const*>( &DEVICE_CERT_FLASH_CONFIG ),
		nullptr );
}

/**
 **********************************************************************************************
 * @brief Get nv control handle of memory space of the certificates
 * @param[out] nv_ctrl : Pointer to certificate data storage space NV control.
 * @return None
 **********************************************************************************************
 */
void PROD_SPEC_MEM_Dev_Cert_NV_Handle( NV_Ctrl** nv_ctrl )
{
	*nv_ctrl = cert_space_nv_ctrl;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_MEM_WEB_Space_Init( void )
{
#ifdef WEB_SERVER_SUPPORT

	static const NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD WEB_AREA_CONFIG =
	{
		reinterpret_cast<uint32_t>( uC_Web_Fw_Start_Address() ),
		reinterpret_cast<uint32_t>( uC_Web_Fw_End_Address() ),
		reinterpret_cast<uint32_t>( uC_Web_Fw_End_Address() ),
	};

	web_space_nv_ctrl = new NV_Ctrls::uC_Flash( &WEB_AREA_CONFIG, nullptr );

#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_MEM_Get_WEB_NV_Handle( NV_Ctrl** nv_ctrl )
{
#ifdef WEB_SERVER_SUPPORT
	*nv_ctrl = web_space_nv_ctrl;
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t PROD_SPEC_MEM_Is_Scratch_Web_Space_Shared( void )
{
	bool_t shared_mem_space = false;

#ifdef WEB_SERVER_SUPPORT
	/*
	 * For Web image we dont have scratch memory area.
	 * We are using static partition created in partition table for web image.*
	 * So, making this flag as true in case web image upgrade is required
	 */
	shared_mem_space = true;
#endif

	return ( shared_mem_space );
}

#endif
