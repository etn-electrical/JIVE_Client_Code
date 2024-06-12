/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Prod_Spec_LTK_ESP32.h"

#ifdef PKI_SUPPORT
#include "Prod_Spec_PKI.h"
#include "Code_Sign_RSA_Matrix.h"
#include "Code_Sign_ECC_Matrix.h"
#include "uC_Watchdog.h"
#include "NV_Ctrl.h"
#include "Babelfish_Assert.h"
#include "Cert_Store_Flash.h"
#include "Cert_Store_NV.h"
#include "uC_Fw_Code_Range.h"
#include "Prod_Spec_MEM.h"
#include "Fw_Store_Flash.h"
#include "Fw_Store_NV.h"
#ifdef ETHERNET_SETUP
#include "Eth_if.h"
#endif
#include "Prod_Spec_Debug.h"
#include "Babelfish_Assert.h"

#ifndef DISABLE_PKI_CERT_GEN_SUPPORT
#include "PROD_SPEC_CERT_ECC_CA.h"
#endif

bool Prod_Code_Sign_Complete = false;

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
/**
 * @brief CR task to handle code signing and certificate generation
 * @param[in] cr_task: Pointer to CR Task.
 * @param[in] handle: Handle to call task handler function.
 * @return None
 */
static void Prod_Spec_Pki_Handler( CR_Tasker* cr_task, CR_TASKER_PARAM handle );

/**
 * @brief This function is called during firmware verification.
 * @details Reload the watchdog register.
 * This can be modified to update any parameter during firmware signature verification.
 * @param None.
 * @return Status of requested operation.
 */
static void Prod_Spec_User_Callback( void );


/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */

static Cert_Store* cert_store_if = nullptr;
static Fw_Store* fw_ctrl = nullptr;

bool key_upgrade_msg = false;

#ifdef DISABLE_PKI_CERT_GEN_SUPPORT
pki_state_enum_t pki_state = PKI_CS_INIT;
#else
static pki_state_enum_t pki_state = PKI_CERT_INIT;
#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_PKI_Init( void )
{
	static const PKI_Common::nv_chip_config_t DEVICE_CERT_NV_CONFIG =
	{
		reinterpret_cast<NV_CTRL_ADDRESS_TD>( uC_Device_Cert_Start_Address() ),
		reinterpret_cast<NV_CTRL_LENGTH_TD>(
			reinterpret_cast<uint32_t>( uC_Device_Cert_End_Address() ) -
			reinterpret_cast<uint32_t>( uC_Device_Cert_Start_Address() ) )
	};

	NV_Ctrl* cert_store_flash_ctrl = nullptr;

	PROD_SPEC_MEM_Dev_Cert_NV_Handle( &cert_store_flash_ctrl );

	BF_ASSERT( cert_store_flash_ctrl );
	/* If cert store is located in external memory then call Cert_Store_NV() else call Cert_Store_Flash() */
	cert_store_if = new Cert_Store_NV( cert_store_flash_ctrl, &DEVICE_CERT_NV_CONFIG );

#ifndef DISABLE_PKI_CERT_GEN_SUPPORT
#if defined ( WEB_SERVER_SUPPORT ) || ( PX_GREEN_DTLS_SUPPORT )
	PROD_SPEC_CERT_ECC_CA_Init();
#endif
#endif	// DISABLE_PKI_CERT_GEN_SUPPORT
	new CR_Tasker( Prod_Spec_Pki_Handler,
				   reinterpret_cast<CR_TASKER_PARAM>( nullptr ),
				   static_cast<uint8_t>( CR_TASKER_PRIORITY_2 ),
				   "Cert Gen and Code Sign Handler" );
	PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "PKI initialised." );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Prod_Spec_User_Callback( void )
{
	uC_Watchdog::Force_Feed_Dog();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Prod_Spec_Pki_Handler( CR_Tasker* cr_task, CR_TASKER_PARAM handle )
{
	Code_Sign_ECC_Matrix* code_sign = nullptr;

	CR_Tasker_Begin( cr_task );

	while ( true )
	{
#if !defined( DISABLE_PKI_CERT_GEN_SUPPORT ) && defined( HTTPS_SUPPORT )
		if ( pki_state == PKI_CERT_INIT )
		{
			/* wait till ip is configured, certificate field requires Device IP, for eg - common
			   name */

			if ( Eth_if::Is_Ip_Configured() == false )
			{
				/* Sleep task */
				CR_Tasker_Delay( cr_task, PKI_TIMER_PERIOD_250_MS );
			}
			else
			{
				pki_state = PKI_CERT_PROGRESS;
			}
		}
		if ( pki_state == PKI_CERT_PROGRESS )
		{
			/*  Generate device certificate */
			uC_Watchdog::Init( WATCHDOG_TIMEOUT + 5000 );
	#if defined ( WEB_SERVER_SUPPORT ) || ( PX_GREEN_DTLS_SUPPORT )

			uint32_t cert_gen_start_time = BF_Lib::Timers::Get_Time();
			if ( Prod_Spec_Certificate_Handler( cert_store_if ) == PKI_Common::SUCCESS )
			{
				pki_state = PKI_CS_INIT;
			}
			else
			{
				pki_state = PKI_CERT_FAILURE;
			}
			uint32_t cert_gen_time = BF_Lib::Timers::Time_Passed( cert_gen_start_time );
			PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "PKI Cert Generation Time : %d ms", cert_gen_time );
	#else
			pki_state = PKI_CS_INIT;
	#endif
			uC_Watchdog::Init( WATCHDOG_TIMEOUT );
		}
#endif	// DISABLE_PKI_CERT_GEN_SUPPORT
		if ( pki_state == PKI_CS_PROGRESS )
		{
			uint32_t code_sign_start_time = BF_Lib::Timers::Get_Time();

			code_sign = new Code_Sign_ECC_Matrix();

			if ( code_sign != nullptr )
			{
                ets_printf("start code signing validation\n");
				uC_Watchdog::Init( WATCHDOG_TIMEOUT + 5000 );
				if ( code_sign->Verify_Signature_ECC( cert_store_if, fw_ctrl,
													  &key_upgrade_msg ) ==
					 PKI_Common::SUCCESS )
				{
					pki_state = PKI_CS_SUCCESS;
					PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG,
										   "FW Signature Verification Successful, Key Update Required: %d",
										   key_upgrade_msg );
					Prod_Code_Sign_Complete = true;
				}
				else
				{
					pki_state = PKI_CS_FAILURE;
					PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "Firmware Signature Verification Failed" );
				}

				uC_Watchdog::Init( WATCHDOG_TIMEOUT );
				code_sign->~Code_Sign_ECC_Matrix();
				delete( code_sign );
				code_sign = nullptr;
			}

			uint32_t code_sign_time = BF_Lib::Timers::Time_Passed( code_sign_start_time );
			PROD_SPEC_DEBUG_PRINT( DBG_MSG_DEBUG, "PKI Code Signing Time : %d ms", code_sign_time );
		}
		/* Sleep task */
		CR_Tasker_Delay( cr_task, PKI_TIMER_PERIOD_250_MS );

	}
	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
PKI_Common::pki_status_t Prod_Spec_Code_Sign( NV_Ctrl* nv_handle, bool* key_upgrade_status )
{
	PKI_Common::pki_status_t ret_status = PKI_Common::FAILURE;
	bool fw_ctrl_ret = true;
	static PKI_Common::nv_chip_config_t mem_config;

	/* This function is called multiple times till signature verification is in process, below null
	   check is added to avoid repeated memory allocation */
	if ( fw_ctrl == nullptr )
	{
		NV_Ctrl::mem_range_info_t mem_range;
		nv_handle->Get_Memory_Info( &mem_range );

		mem_config.start_address = mem_range.start_address;
		mem_config.length = mem_range.end_address - mem_range.start_address + 1U;
		/* If firmware package is located in external memory then call Fw_Store_NV(), else call Fw_Store_Flash() */
		fw_ctrl = new Fw_Store_NV( &mem_config, const_cast<NV_Ctrl*>( nv_handle ), &Prod_Spec_User_Callback,
								   &fw_ctrl_ret );
	}

	if ( ( fw_ctrl != nullptr ) && ( fw_ctrl_ret == true ) )
	{
		if ( pki_state == PKI_CS_INIT )
		{
			pki_state = PKI_CS_PROGRESS;///< start signature verification
		}
#if !defined( DISABLE_PKI_CERT_GEN_SUPPORT ) && defined( HTTPS_SUPPORT )
		else if ( ( pki_state == PKI_CERT_INIT ) || ( pki_state == PKI_CERT_PROGRESS ) )
		{
			ret_status = PKI_Common::PENDING_CALL_BACK;
		}
		else if ( pki_state == PKI_CERT_FAILURE )
		{
			pki_state = PKI_CS_PROGRESS;///< start signature verification
		}
#endif
		/* wait for signature verification result */
		if ( pki_state == PKI_CS_SUCCESS )
		{
			if ( key_upgrade_msg == true )
			{
				*key_upgrade_status = true;
			}
			else
			{
				*key_upgrade_status = false;
			}
			if ( fw_ctrl != nullptr )
			{
				fw_ctrl->~Fw_Store();
				delete fw_ctrl;
				fw_ctrl = nullptr;
			}
			ret_status = PKI_Common::SUCCESS;
			pki_state = PKI_CS_INIT;///< reset the status
		}
		else if ( pki_state == PKI_CS_PROGRESS )
		{
			/* return BUSY_ERROR if signature verification is in progress or if not started */
			ret_status = PKI_Common::BUSY_ERROR;
		}
		else if ( pki_state == PKI_CS_FAILURE )
		{
			if ( fw_ctrl != nullptr )
			{
				fw_ctrl->~Fw_Store();
				delete fw_ctrl;
				fw_ctrl = nullptr;
			}
			ret_status = PKI_Common::DATA_ERROR;
			pki_state = PKI_CS_INIT;///< reset the status
		}
	}
	else if ( ( fw_ctrl != nullptr ) && ( fw_ctrl_ret == false ) )
	{
		fw_ctrl->~Fw_Store();
		delete fw_ctrl;
		fw_ctrl = nullptr;
		Prod_Code_Sign_Complete = true;
	}
	return ( ret_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Update_Code_Sign_Cert( void )
{
	/*create local fw_ctrl temp*/
	static Fw_Store* update_code_sign_fw_ctrl = nullptr;
	uint8_t* fvk_cert = nullptr;
	uint16_t fvk_cert_len = 0U;
	uint8_t* fvk_key = nullptr;
	uint16_t fvk_key_len = 0U;
	uint8_t* rvk_key = nullptr;
	uint16_t rvk_key_len = 0U;
	uint8_t* ivk_cert = nullptr;
	uint16_t ivk_cert_len = 0U;
	bool return_status = false;
	bool integrity_status = false;
	NV_Ctrl* nv_handle;
	PKI_Common::nv_chip_config_t mem_config;
	NV_Ctrl::mem_range_info_t mem_range;

	/* Get the scratch area nv_handle and memory range */
	PROD_SPEC_MEM_Get_Scratch_NV_Handle( &nv_handle );
	nv_handle->Get_Memory_Info( &mem_range );

	mem_config.start_address = mem_range.start_address;
	mem_config.length = mem_range.end_address - mem_range.start_address + 1U;

	/* If firmware package is located in external memory then call Fw_Store_NV() else call Fw_Store_Flash() */
	update_code_sign_fw_ctrl = new Fw_Store_NV( &mem_config, const_cast<NV_Ctrl*>( nv_handle ),
												&Prod_Spec_User_Callback,
												&integrity_status );

	if ( ( integrity_status == true ) && ( update_code_sign_fw_ctrl != nullptr ) )
	{
		/* update IVK cert*/
		if ( update_code_sign_fw_ctrl->Get_IVK_Cert( &ivk_cert, &ivk_cert_len ) == PKI_Common::SUCCESS )
		{
			if ( cert_store_if->Update_IVK_Cert_Buf( ivk_cert,
													 ivk_cert_len ) == PKI_Common::SUCCESS )
			{
				return_status = true;
			}
		}

		/* update FVK cert*/
		if ( ( return_status == true ) &&
			 ( update_code_sign_fw_ctrl->Get_FVK_Cert( &fvk_cert, &fvk_cert_len ) == PKI_Common::SUCCESS ) )
		{
			return_status = false;
			if ( cert_store_if->Update_FVK_Cert_Buf( fvk_cert,
													 fvk_cert_len ) == PKI_Common::SUCCESS )
			{
				return_status = true;
			}
		}
		else
		{
			return_status = false;
		}

		/* update FVK key*/
		if ( ( return_status == true ) &&
			 ( update_code_sign_fw_ctrl->Get_FVK_Key( &fvk_key, &fvk_key_len ) == PKI_Common::SUCCESS ) )
		{
			return_status = false;
			if ( cert_store_if->Update_FVK_Key_Buf( fvk_key, fvk_key_len ) == PKI_Common::SUCCESS )
			{
				return_status = true;
			}
		}
		else
		{
			return_status = false;
		}

		/* update RVK key*/
		if ( ( return_status == true ) &&
			 ( update_code_sign_fw_ctrl->Get_RVK_Key( &rvk_key, &rvk_key_len ) == PKI_Common::SUCCESS ) )
		{
			return_status = false;
			if ( cert_store_if->Update_RVK_Key_Buf( rvk_key, rvk_key_len ) == PKI_Common::SUCCESS )
			{
				if ( cert_store_if->Cert_Store_Write() == PKI_Common::SUCCESS )
				{
					return_status = true;
				}
			}
		}
		else
		{
			return_status = false;
		}
	}

	if ( update_code_sign_fw_ctrl != nullptr )
	{
		update_code_sign_fw_ctrl->~Fw_Store();
		delete update_code_sign_fw_ctrl;
		update_code_sign_fw_ctrl = nullptr;
	}
	return ( return_status );
}

#endif	// PKI_SUPPORT
