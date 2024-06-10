/**
 **********************************************************************************************
 * @file            Prod_Spec_FUS.cpp  product specific initialization
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the initialization routines specific to product Fw
 *                  Upgrade.
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
#include "Prod_Spec_LTK_ESP32.h"
#include "DCI_Owner.h"
#include "DCI_Data.h"
#include "DCI_Defines.h"
#include "uC_Fw_Code_Range.h"
#include "uC_Code_Range.h"
#include "DCI_Constants.h"
#include "uC_Flash.h"
#include "NV_Mem.h"
#include "NV_Address.h"
#include "FUS.h"
#include "Esp32_Image_Ctrl.h"
#include "Local_Component.h"
#include "http_auth.h"
#include "Proc_Mem_ID_Codes.h"
#include "Prod_Spec_FUS.h"
#include "Commit_Ctrl.h"
#include "Commit_Copy.h"
#include "Commit_None.h"
#include "INT_ASCII_Conversion.h"
#include "Prod_Spec_PKI.h"
#ifndef ESP32_SETUP
#include "Log_Events.h"
#endif
#include "FUS_Config.h"
#include "Format_Handler.h"
#include "Image_Header.h"
#include "Timecommon.h"
#if ( defined( WEB_SERVER_SUPPORT ) || defined( REST_I18N_LANGUAGE ) )
#include "REST.h"
#include "REST_FUS_Interface.h"
#include "REST_Common.h"
#endif
#include "App_IO_Config.h"
#include "Prod_Spec_MEM.h"
#include "Babelfish_Assert.h"
#include "Prod_Spec_Debug.h"
#include "IOT_Fus.h"
#include "Esp32_Partition.h"

/*
 *****************************************************************************************
 *		Defines and Constants(Custom Header for ESP32 definition)
 *****************************************************************************************
 */

/**
 **********************************************************************************************
 * @brief                       This definition is inserting the PXGreen header of PROD_INFO_HEADER for ESP32
 *                              at "rodata_custom_desc" in image file.
 **********************************************************************************************
 */
extern "C" const __attribute__( ( section( ".rodata_custom_desc" ) ) ) prodinfo_for_boot_s PROD_INFO_HEADER =
{
	.serial_number_nvadd = DCI_PRODUCT_SERIAL_NUM_NVADD,
	.hardware_rev_nvadd = DCI_HARDWARE_VER_ASCII_NVADD,
	.rtu_slave_nvadd = DCI_EXAMPLE_MODBUS_SERIAL_ADDRESS_NVADD,
	.rtu_baud_rate_nvadd = DCI_EXAMPLE_MODBUS_SERIAL_BAUD_NVADD,
	.rtu_parity_type_nvadd = DCI_EXAMPLE_MODBUS_SERIAL_PARITY_NVADD,
	.reserved = { 0 }
};

/**
 **********************************************************************************************
 * @brief                       This definition is inserting the PXGreen header of APP_HEADER for ESP32
 *                              at "rodata_custom_desc" in image file.
 **********************************************************************************************
 */
extern "C" const __attribute__( ( section( ".rodata_custom_desc" ) ) ) fw_header APP_HEADER =
{
	.magic_endian = MAGIC_ENDIAN,
	/* Set this to the maximum possible length.
	 * data_length should be equal max OTA partition size length
	 * This must not be greater than OTA area size defined in partition table.
	 * ESP32 has two OTA partitions, size of both partitions is same size.
	 * Ie, .data_length = (uint32_t) ( ( uC_App_Fw_End_Address() + 1U ) - uC_App_Fw_Start_Address() )
	 */
	.data_length = ( APP_IMAGE_SIZE* 1024U ) - CODE_SIGN_INFO_MAX_SIZE,
	.msg_for_boot_nvadd = MSG_FOR_BOOT_NVADD,
	.header_format_version = 0,
	.image_guid = LOCAL_COMP_APP_IMG_GUID,
	.version_info =
	{
		CONST_FW_REV_MAJOR,
		CONST_FW_REV_MINOR,
		CONST_FW_REV_BUILD & 0xFF,
		CONST_FW_REV_BUILD >> 8
	},
	// uint16_t product_code is stored byte by byte(MSB first)
	.product_code =
	{
		PRODUCT_CODE& 0xFF,
		PRODUCT_CODE >> 8
	},
	.compatibility_num = CONST_APP_FW_COMPATIBILITY_CODE
};

/**
 **********************************************************************************************
 * @brief                       This definition is inserting the CRC value location in memory with default value.
 **********************************************************************************************
 */
extern "C" const __attribute__( ( section( ".rodata_custom_desc" ) ) ) crc_info crc_header =
{
	.crc = 0xFFFFU,
	.reserved = 0xFFFFU		///< Initializing to FF

};

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */


// Product Definition
#ifdef PKI_SUPPORT
static BF_FUS::Component_Ctrl::comp_config_t comp_config[TOTAL_COMPONENTS];
static BF_FUS::Component_Ctrl* comp_ctrl_list[TOTAL_COMPONENTS];
static BF_FUS::prod_config_t prod_config;
static BF_FUS::Commit_Ctrl* commit_ctrl[LOCAL_COMP_TOTAL_IMAGES];
static BF_FUS::Image::img_config_t image_config[LOCAL_COMP_TOTAL_IMAGES];
static BF_FUS::Image* image_ctrl_list[LOCAL_COMP_TOTAL_IMAGES];
#endif	/// PKI_SUPPORT

BF_FUS::op_status_t FUS_Event_Cb( BF_FUS::event_t* fus_status );

bool FUS_Upgrade( BF_FUS::event_t* fus_status );

// Not used in code but keeping it for further reference.
// extern DCI_CB_RET_TD Update_User_App_Name_CB( DCI_CBACK_PARAM_TD cback_param, DCI_ACCESS_ST_TD* access_struct );

#if ( defined( WEB_SERVER_SUPPORT ) || defined( REST_I18N_LANGUAGE ) )
extern uint32_t Get_Epoch_Time( void );

/* Get user related DCI information .*/
void Get_Image_User_info( uint8_t image_id, image_upgrade_info_t* image_upgrade_info );

/* Set user related DCI information . */
void Set_Image_User_info( uint8_t image_id, uint8_t user_id );

#endif

/*
 *****************************************************************************************
 *		Globals
 *****************************************************************************************
 */
fw_upgrade_dcid_s fw_dcid_list[LOCAL_COMP_TOTAL_IMAGES];
DCI_Owner* fw_upgrade_mode = nullptr;
DCI_Owner* web_fus_status = nullptr;
uint8_t* g_fus_op_buf = nullptr;
static DCI_Patron* patron_handle;
static DCI_SOURCE_ID_TD source_id;
static bool init_status[TOTAL_COMPONENTS][LOCAL_COMP_TOTAL_IMAGES];

extern bool key_upgrade_msg;
static uint8_t fw_image_id = 0U;

/*
 *****************************************************************************************
 *		Functions
 *****************************************************************************************
 */

void PROD_SPEC_FUS_Init( void )
{

#ifdef PKI_SUPPORT
	g_fus_op_buf = new uint8_t[MAX_CHUNK_SIZE];
	fw_upgrade_mode = new DCI_Owner( DCI_FW_UPGRADE_MODE_DCID );
	web_fus_status = new DCI_Owner( DCI_WEB_FIRM_UPGRADE_STATUS_DCID );
	DCI_Owner* app_firm_num = new DCI_Owner( DCI_APP_FIRM_VER_DCID );

	// Image 0/ App DCI Owner

	fw_dcid_list[LOCAL_COMP_APP_IMG_INDEX_0].version = new DCI_Owner( DCI_APP_FIRM_VER_NUM_DCID );
	fw_dcid_list[LOCAL_COMP_APP_IMG_INDEX_0].last_user = new DCI_Owner(
		DCI_LAST_APP_FIRM_UPGRADE_USER_DCID );
	fw_dcid_list[LOCAL_COMP_APP_IMG_INDEX_0].last_time = new DCI_Owner(
		DCI_APP_FIRM_UPGRADE_EPOCH_TIME_DCID );

	// Image 1/ Web DCI Owner
#ifdef WEB_SERVER_SUPPORT
	fw_dcid_list[LOCAL_COMP_WEB_IMG_INDEX_1].version = new DCI_Owner( DCI_WEB_FIRM_VER_NUM_DCID );

	fw_dcid_list[LOCAL_COMP_WEB_IMG_INDEX_1].last_user = new DCI_Owner(
		DCI_LAST_WEB_FIRM_UPGRADE_USER_DCID );

	fw_dcid_list[LOCAL_COMP_WEB_IMG_INDEX_1].last_time = new DCI_Owner(
		DCI_WEB_FIRM_UPGRADE_EPOCH_TIME_DCID );
#endif
#ifdef REST_I18N_LANGUAGE
	// Image 2/ Language image DCI Owner

	fw_dcid_list[LOCAL_COMP_LANG_IMG_INDEX_2].version = new DCI_Owner( DCI_LANG_FIRM_VER_NUM_DCID );

	fw_dcid_list[LOCAL_COMP_LANG_IMG_INDEX_2].last_user = new DCI_Owner(
		DCI_LAST_LANG_FIRM_UPGRADE_USER_DCID );

	fw_dcid_list[LOCAL_COMP_LANG_IMG_INDEX_2].last_time = new DCI_Owner(
		DCI_LANG_FIRM_UPGRADE_EPOCH_TIME_DCID );
#endif	// REST_I18N_LANGUAGE

	patron_handle = new DCI_Patron( true );
	source_id = DCI_SOURCE_IDS_Get();
	BF_ASSERT( patron_handle );

	// Product Information
	if ( DCI_ERR_NO_ERROR !=
		 Get_DCID_Value( DCI_PRODUCT_SERIAL_NUM_DCID, reinterpret_cast<uint8_t*>( &prod_config.serial_num ) ) )
	{
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get product serial number" );
	}

	// Processor information
	if ( fw_dcid_list[LOCAL_COMP_APP_IMG_INDEX_0].version->Check_Out( comp_config[LOCAL_COMP_INDEX_0].firm_rev ) )
	{
		if ( DCI_ERR_NO_ERROR !=
			 Get_DCID_Value( DCI_PRODUCT_PROC_SERIAL_NUM_DCID,
							 reinterpret_cast<uint8_t*>( &comp_config[LOCAL_COMP_INDEX_0].serial_num ) ) )
		{
			PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get processor serial number" );
		}
	}

	image_config[LOCAL_COMP_APP_IMG_INDEX_0].name = new uint8_t[DCID_STRING_SIZE];
	// Image 0/ App DCI Owner
	if ( true == fw_upgrade_mode->Check_Out( image_config[LOCAL_COMP_APP_IMG_INDEX_0].upgrade_mode ) )
	{
		if ( true ==
			 fw_dcid_list[LOCAL_COMP_APP_IMG_INDEX_0].version->Check_Out( image_config[LOCAL_COMP_APP_IMG_INDEX_0].
																		  version ) )
		{
			if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) ==
				 Get_DCID_Value( DCI_USER_APP_NAME_DCID, image_config[LOCAL_COMP_APP_IMG_INDEX_0].name ) )
			{
				image_config[LOCAL_COMP_APP_IMG_INDEX_0].name_len =
					NAME_LEN( image_config[LOCAL_COMP_APP_IMG_INDEX_0].name );
				image_config[LOCAL_COMP_APP_IMG_INDEX_0].guid = LOCAL_COMP_APP_IMG_GUID;

			}
			else
			{
				PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get App image name" );
			}
		}
		else
		{
			PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get App image version" );
		}
	}
	else
	{
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get firmware upgrade mode from DCID" );
	}

#ifdef WEB_SERVER_SUPPORT	// Image 1/ Web DCI Owner

	image_config[LOCAL_COMP_WEB_IMG_INDEX_1].name = new uint8_t[DCID_STRING_SIZE];
	if ( true == fw_upgrade_mode->Check_Out( image_config[LOCAL_COMP_WEB_IMG_INDEX_1].upgrade_mode ) )
	{
		if ( true ==
			 fw_dcid_list[LOCAL_COMP_WEB_IMG_INDEX_1].version->Check_Out( image_config[LOCAL_COMP_WEB_IMG_INDEX_1].
																		  version ) )
		{
			uint16_t tot_length = 0U;
			if ( static_cast<uint32_t>( DCI_ERR_NO_ERROR ) ==
				 get_dcid_ram_data( DCI_USER_WEB_FW_NAME_DCID, image_config[LOCAL_COMP_WEB_IMG_INDEX_1].name,
									&tot_length, 0U ) )
			{
				image_config[LOCAL_COMP_WEB_IMG_INDEX_1].name_len =
					NAME_LEN( image_config[LOCAL_COMP_WEB_IMG_INDEX_1].name );
				image_config[LOCAL_COMP_WEB_IMG_INDEX_1].guid = LOCAL_COMP_WEB_IMG_GUID;
			}
			else
			{
				PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get Web image name" );
			}
		}
		else
		{
			PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to get Web image version" );
		}
	}
	else
	{
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, " Failed to get firmware upgrade mode from DCID" );
	}

#endif
#ifdef REST_I18N_LANGUAGE
	// Image 2/ Language DCI Owner

	image_config[LOCAL_COMP_LANG_IMG_INDEX_2].name = new uint8_t[DCID_STRING_SIZE];
	if ( true == fw_upgrade_mode->Check_Out( image_config[LOCAL_COMP_LANG_IMG_INDEX_2].upgrade_mode ) )
	{
		if ( true ==
			 fw_dcid_list[LOCAL_COMP_LANG_IMG_INDEX_2].version->Check_Out(
				 image_config[LOCAL_COMP_LANG_IMG_INDEX_2].version ) )
		{
			if ( true == BF_Lib::Copy_Data( image_config[LOCAL_COMP_LANG_IMG_INDEX_2].name,
											DCID_STRING_SIZE, IMAGE_NAME, DCID_STRING_SIZE ) )
			{
				image_config[LOCAL_COMP_LANG_IMG_INDEX_2].name_len = IMAGE_NAME_LEN;
				image_config[LOCAL_COMP_LANG_IMG_INDEX_2].guid = LOCAL_COMP_LANG_IMG_GUID;
			}
			else
			{
				PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "Unable to set language image name" );
			}
		}
		else
		{
			PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "Language image version DCID check-out failed" );
		}
	}
	else
	{
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "Upgrade mode DCID check-out failed for Language image" );
	}
#endif	// REST_I18N_LANGUAGE

#ifdef WEB_SERVER_SUPPORT
	uint8_t fus_status = WEB_FW_UPGRADE_IDLE;
	web_fus_status->Check_Out( fus_status );
	if ( WEB_FW_UPGRADE_IDLE != fus_status )
	{
		fus_status = WEB_FW_UPGRADE_IDLE;
		web_fus_status->Check_In( &fus_status );
		web_fus_status->Check_In_Init( &fus_status );
	}
#endif	// WEB_SERVER_SUPPORT

	for ( uint8_t i = 0U; i < TOTAL_COMPONENTS; i++ )
	{
		for ( uint8_t j = 0U; j < LOCAL_COMP_TOTAL_IMAGES; j++ )
		{
			init_status[i][j] = false;
		}
	}

	NV_Ctrl* app_nv_ctrl = nullptr;
	NV_Ctrl* scratch_nv_ctrl = nullptr;
	PROD_SPEC_MEM_Get_Scratch_NV_Handle( &scratch_nv_ctrl );
	BF_ASSERT( scratch_nv_ctrl );
	PROD_SPEC_MEM_Get_APP_NV_Handle( &app_nv_ctrl );
	BF_ASSERT( app_nv_ctrl );

#ifdef WEB_SERVER_SUPPORT
	NV_Ctrl* web_nv_ctrl = nullptr;
	PROD_SPEC_MEM_Get_WEB_NV_Handle( &web_nv_ctrl );
	BF_ASSERT( web_nv_ctrl );
#endif	// WEB_SERVER_SUPPORT
	commit_ctrl[LOCAL_COMP_APP_IMG_INDEX_0] = new BF_FUS::Commit_None();
#ifdef WEB_SERVER_SUPPORT

	if ( PROD_SPEC_MEM_Is_Scratch_Web_Space_Shared() == true )
	{
		/*
		 * ESP32 has scratch and memory space same so no need to copy the image,
		 * can use same memory location for web operation
		 */
		commit_ctrl[LOCAL_COMP_WEB_IMG_INDEX_1] = new BF_FUS::Commit_None();
	}
	else
	{
		commit_ctrl[LOCAL_COMP_WEB_IMG_INDEX_1] = new BF_FUS::Commit_Copy();
	}
#endif	/// WEB_SERVER_SUPPORT

	// Construction of Image controls. Careful with order.

	image_ctrl_list[LOCAL_COMP_APP_IMG_INDEX_0] = new BF_FUS::Esp32_Image_Ctrl( scratch_nv_ctrl, app_nv_ctrl,
																				&image_config[LOCAL_COMP_APP_IMG_INDEX_0],
																				commit_ctrl[LOCAL_COMP_APP_IMG_INDEX_0] );

#ifdef WEB_SERVER_SUPPORT
	// We are not using passive partition for web image. So passing first argument same as web_nv_ctrl
	image_ctrl_list[LOCAL_COMP_WEB_IMG_INDEX_1] = new BF_FUS::Esp32_Image_Ctrl( web_nv_ctrl, web_nv_ctrl,
																				&image_config[LOCAL_COMP_WEB_IMG_INDEX_1],
																				commit_ctrl[LOCAL_COMP_WEB_IMG_INDEX_1] );
#endif	// WEB_SERVER_SUPPORT

#ifdef REST_I18N_LANGUAGE
	/* Construction of Image control for Language Image */
	NV_Ctrl* lang_nv_ctrl = nullptr;
	PROD_SPEC_MEM_Get_Lang_NV_Handle( &lang_nv_ctrl );
	BF_ASSERT( lang_nv_ctrl );
	commit_ctrl[LOCAL_COMP_LANG_IMG_INDEX_2] = new BF_FUS::Commit_Copy();
	image_ctrl_list[LOCAL_COMP_LANG_IMG_INDEX_2] = new BF_FUS::Esp32_Image_Ctrl( scratch_nv_ctrl, lang_nv_ctrl,
																				 &image_config[
																					 LOCAL_COMP_LANG_IMG_INDEX_2],
																				 commit_ctrl[LOCAL_COMP_LANG_IMG_INDEX_2] );
#endif	// REST_I18N_LANGUAGE

	// Construction of Component controls.
	comp_ctrl_list[LOCAL_COMP_INDEX_0] =
		new BF_FUS::Local_Component( image_ctrl_list, LOCAL_COMP_TOTAL_IMAGES, &comp_config[LOCAL_COMP_INDEX_0] );

	// Construction of Product controls.
	// coverity[leaked_storage]
	new BF_FUS::FUS( comp_ctrl_list, TOTAL_COMPONENTS, &prod_config, FUS_Event_Cb );

	uint16_t app_fw_rev_eip = ( ( image_config[LOCAL_COMP_APP_IMG_INDEX_0].version ) >> 16 ) & 0XFFFF;

	if ( false == app_firm_num->Check_In( app_fw_rev_eip ) )
	{
		PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR, "Failed to save App image version for eip in DCID" );
	}

#if ( defined( WEB_SERVER_SUPPORT ) || defined( REST_I18N_LANGUAGE ) )
	User_Firmware_Upgrade_Info( Get_Image_User_info, Set_Image_User_info );
#endif

	// coverity[leaked_storage]
#endif	// PKI_SUPPORT
}

#if ( defined( WEB_SERVER_SUPPORT ) || defined( REST_I18N_LANGUAGE ) )
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Get_Image_User_info( uint8_t image_id, image_upgrade_info_t* image_upgrade_info )
{
	if ( DCID_STRING_SIZE >= ( fw_dcid_list[image_id].last_user->Get_Length() ) )
	{
		if ( true == fw_dcid_list[image_id].last_user->Check_Out( image_upgrade_info->last_user ) )
		{
			image_upgrade_info->user_len = NAME_LEN( image_upgrade_info->last_user );
			if ( false == fw_dcid_list[image_id].last_time->Check_Out( image_upgrade_info->last_time ) )
			{
				BF_ASSERT( false );
			}
		}
		else
		{
			BF_ASSERT( false );
		}
	}
	else
	{
		BF_ASSERT( false );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Set_Image_User_info( uint8_t image_id, uint8_t user_id )
{
	date_time_format_t current_date_time = {0};
	uint8_t dest_data[MIN_DEST_BUFFER_SIZE];

	if ( DCI_ERR_NO_ERROR ==
		 Get_DCID_Value( USER_MGMT_USER_LIST[user_id - 1U].user_name, reinterpret_cast<uint8_t*>( &dest_data ) ) )
	{
		fw_dcid_list[image_id].last_user->Check_In( &dest_data );
		fw_dcid_list[image_id].last_user->Check_In_Init( &dest_data );
	}
	else
	{
		BF_ASSERT( false );
	}

	/* call epoch time API */
	current_date_time.date_time = Get_Epoch_Time();

	fw_dcid_list[image_id].last_time->Check_In( &current_date_time.date_time );
	fw_dcid_list[image_id].last_time->Check_In_Init( &current_date_time.date_time );
}

#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
BF_FUS::op_status_t FUS_Event_Cb( BF_FUS::event_t* fus_status )
{
	BF_FUS::fus_event_t fus_event = fus_status->event;
	BF_FUS::op_status_t op_status = BF_FUS::REJECT;

#ifdef WEB_SERVER_SUPPORT
	uint8_t web_status = 0U;
	uint16_t comp_num[LOCAL_COMP_TOTAL_IMAGES];
	bool alloc_status = true;
	BF_FUS::FUS* fus_handle = BF_FUS::FUS::Get_FUS_Handle_Static();
	NV_Ctrl* web_ui_nv_ctrl = nullptr;

	PROD_SPEC_MEM_Get_WEB_NV_Handle( &web_ui_nv_ctrl );

#endif
	bool clear_buff = false;

	switch ( fus_event )
	{
		case BF_FUS::EVENT_IDLE:
			break;

		case BF_FUS::INIT_VALID:
		case BF_FUS::INIT_INVALID:
			/* Bootup Validation callback */
#ifdef WEB_SERVER_SUPPORT
			BF_FUS::Image_Header* image_header[LOCAL_COMP_TOTAL_IMAGES];
#endif
			init_status[fus_status->comp_id][fus_status->image_id] = true;

#ifdef WEB_SERVER_SUPPORT
			if ( ( ( fus_status->image_id == LOCAL_COMP_APP_IMG_INDEX_0 ) ||
				   ( fus_status->image_id == LOCAL_COMP_WEB_IMG_INDEX_1 ) ) &&
				 ( init_status[LOCAL_COMP_INDEX_0][LOCAL_COMP_APP_IMG_INDEX_0] == true ) &&
				 ( init_status[LOCAL_COMP_INDEX_0][LOCAL_COMP_WEB_IMG_INDEX_1] == true ) )
			{
				if ( fus_event == BF_FUS::INIT_VALID )
				{

					image_header[LOCAL_COMP_APP_IMG_INDEX_0] =
						reinterpret_cast<BF_FUS::Esp32_Image_Ctrl*>( image_ctrl_list[LOCAL_COMP_APP_IMG_INDEX_0] );
					image_header[LOCAL_COMP_WEB_IMG_INDEX_1] =
						reinterpret_cast<BF_FUS::Esp32_Image_Ctrl*>( image_ctrl_list[LOCAL_COMP_WEB_IMG_INDEX_1] );
					image_header[LOCAL_COMP_APP_IMG_INDEX_0]->Get_Compatibility_Num( &comp_num[
																						 LOCAL_COMP_APP_IMG_INDEX_0] );
					image_header[LOCAL_COMP_WEB_IMG_INDEX_1]->Get_Compatibility_Num( &comp_num[
																						 LOCAL_COMP_WEB_IMG_INDEX_1] );
					if ( comp_num[LOCAL_COMP_APP_IMG_INDEX_0] == comp_num[LOCAL_COMP_WEB_IMG_INDEX_1] )
					{
						Assign_Web_Pages( RUN_WEB_UI, web_ui_nv_ctrl );
					}
					else
					{
						Assign_Web_Pages( RUN_MAINTENANCE_UI, web_ui_nv_ctrl );
					}
				}
				else
				{
					Assign_Web_Pages( RUN_MAINTENANCE_UI, web_ui_nv_ctrl );
				}
			}
#endif	// WEB_SERVER_SUPPORT
#ifdef REST_I18N_LANGUAGE
			else if ( ( fus_status->image_id == LOCAL_COMP_LANG_IMG_INDEX_2 ) &&
					  ( init_status[LOCAL_COMP_INDEX_0][LOCAL_COMP_LANG_IMG_INDEX_2] == true ) )
			{
				PROD_SPEC_REST_LANG_DCI_Init();

				if ( fus_event == BF_FUS::INIT_VALID )
				{
					image_header[LOCAL_COMP_APP_IMG_INDEX_0] =
						reinterpret_cast<BF_FUS::Esp32_Image_Ctrl*>( image_ctrl_list[LOCAL_COMP_APP_IMG_INDEX_0] );
					image_header[LOCAL_COMP_LANG_IMG_INDEX_2] =
						reinterpret_cast<BF_FUS::Esp32_Image_Ctrl*>( image_ctrl_list[LOCAL_COMP_LANG_IMG_INDEX_2] );

					/* Verify compatibility of Language image with application firmware */
					image_header[LOCAL_COMP_APP_IMG_INDEX_0]->Get_Compatibility_Num( &comp_num[
																						 LOCAL_COMP_APP_IMG_INDEX_0] );
					image_header[LOCAL_COMP_LANG_IMG_INDEX_2]->Get_Compatibility_Num( &comp_num[
																						  LOCAL_COMP_LANG_IMG_INDEX_2] );
					if ( comp_num[LOCAL_COMP_APP_IMG_INDEX_0] == comp_num[LOCAL_COMP_LANG_IMG_INDEX_2] )
					{
						/* Reset after completing boot-up validation of Language image */
						if ( PROD_SPEC_REST_LANG_Init() == false )
						{
							PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR,
												   "Language data initialization failed during boot-up validation" );
						}
					}
					else
					{
						PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR,
											   "Language image is incompatible with Application firmware" );
					}
				}
			}
#endif	// REST_I18N_LANGUAGE

			for ( uint8_t comp_sel = LOCAL_COMP_INDEX_0; comp_sel < TOTAL_COMPONENTS; comp_sel++ )
			{
				for ( uint8_t image_sel = LOCAL_COMP_APP_IMG_INDEX_0; image_sel < LOCAL_COMP_TOTAL_IMAGES; image_sel++ )
				{
					if ( init_status[comp_sel][image_sel] != true )
					{
						image_sel = LOCAL_COMP_TOTAL_IMAGES;
						comp_sel = TOTAL_COMPONENTS;
						clear_buff = false;
					}
					else
					{
						clear_buff = true;
					}
				}
			}
			if ( clear_buff == true )
			{
				delete[] g_fus_op_buf;
				g_fus_op_buf = nullptr;
			}
			op_status = BF_FUS::SUCCESS;
			break;

		case BF_FUS::GOTO_FUM:
			if ( true == Is_Fw_Upgrade_Enabled() )
			{
				op_status = BF_FUS::SUCCESS;
			}
			else
			{
				/* Firmware upgrade not permitted */
				op_status = BF_FUS::REJECT;
			}
			break;


		case BF_FUS::SESSION_CREATED:

			g_fus_op_buf = new uint8_t[MAX_CHUNK_SIZE];

#ifdef WEB_SERVER_SUPPORT
			alloc_status = true;

			if ( fus_handle->Get_Interface() == fus_if_t::REST_FUS )
			{
				/* Allocate the memory for the Rest_FUS_Buffer for firmware upgrade */
				alloc_status = Allocate_Rest_FUS_Buffer();
			}
			if ( alloc_status == true )
			{
				key_upgrade_msg = false;

				op_status = BF_FUS::SUCCESS;
			}
			else
			{
				/* Memory not allocated for the firmware upgrade, suspend the firmware upgrade */
				op_status = BF_FUS::FAILURE;
			}
#else

			key_upgrade_msg = false;

			op_status = BF_FUS::SUCCESS;
#endif	// WEB_SERVER_SUPPORT
			break;

		case BF_FUS::WRITE_IN_PROGRESS:
		{
#ifdef WEB_SERVER_SUPPORT
			if ( fus_status->image_id == LOCAL_COMP_WEB_IMG_INDEX_1 )
			{
				web_fus_status->Check_Out( web_status );

				if ( WEB_FW_UPGRADE_IDLE == web_status )
				{
					fw_image_id = fus_status->image_id;

					web_status = WEB_FW_UPGRADE_IN_PROGRESS;
					web_fus_status->Check_In( &web_status );
					web_fus_status->Check_In_Init( &web_status );
					/*
					 * For ESP32, checking the both memory space has same memory location.
					 * Then after the session creation web server load the maintenance page.
					 */
					if ( PROD_SPEC_MEM_Is_Scratch_Web_Space_Shared() == true )
					{
						Assign_Web_Pages( RUN_MAINTENANCE_UI, web_ui_nv_ctrl );
					}
				}
			}
#endif
			op_status = BF_FUS::SUCCESS;
		}
		break;

		case BF_FUS::VALIDATE_COMPLETE:
			/* Image validation successful */
#ifdef WEB_SERVER_SUPPORT
			if ( fus_status->image_id == LOCAL_COMP_WEB_IMG_INDEX_1 )
			{
				web_fus_status->Check_Out( web_status );
				if ( WEB_FW_UPGRADE_IN_PROGRESS == web_status )
				{
					web_status = WEB_FW_VALIDATION_COMPLETED;
					web_fus_status->Check_In( &web_status );
					web_fus_status->Check_In_Init( &web_status );
				}
			}
#endif
			op_status = BF_FUS::SUCCESS;
			break;

		case BF_FUS::COMMIT_COMPLETE:
			/* Copied image from temporary memory location to fix memory location */
			if ( true == FUS_Upgrade( fus_status ) )
			{
				op_status = BF_FUS::SUCCESS;

#ifdef WEB_SERVER_SUPPORT

				if ( fus_status->image_id == LOCAL_COMP_WEB_IMG_INDEX_1 )
				{
					web_fus_status->Check_Out( web_status );
					if ( WEB_FW_VALIDATION_COMPLETED == web_status )
					{
						web_status = WEB_FW_UPGRADE_IDLE;
						web_fus_status->Check_In( &web_status );
						web_fus_status->Check_In_Init( &web_status );
					}
				}
#endif	// WEB_SERVER_SUPPORT
			}
			else
			{
				op_status = BF_FUS::REJECT;
			}
			break;

		case BF_FUS::SESSION_DELETED:
			/* Delete the firmware upgrade session */
			if ( true == FUS_Upgrade( fus_status ) )
			{
				op_status = BF_FUS::SUCCESS;
			}
			else
			{
				op_status = BF_FUS::REJECT;
			}
			delete[] g_fus_op_buf;
			g_fus_op_buf = nullptr;
#ifdef WEB_SERVER_SUPPORT
			if ( fus_handle->Get_Interface() == fus_if_t::REST_FUS )
			{
				/* Free the memory buffer assigned for the Rest_FUS operation */
				Deallocate_Rest_FUS_Buffer();
			}
#endif	// WEB_SERVER_SUPPORT
			break;

		case BF_FUS::FUS_OP_ERROR:
			op_status = BF_FUS::SUCCESS;
			break;

		default:
			break;
	}
	return ( op_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool FUS_Upgrade( BF_FUS::event_t* fus_status )
{
	static bool reset_trigger = false;
	bool return_status = false;
	uint8_t comp_id = fus_status->comp_id;
	uint8_t image_id = fus_status->image_id;
	uint8_t msg_for_boot = 0U;
	NV_Ctrl* web_ui_nv_ctrl = nullptr;

#ifdef WEB_SERVER_SUPPORT
	uint16_t comp_num[LOCAL_COMP_TOTAL_IMAGES];
	uint8_t web_status = 0U;
#endif
	switch ( fus_status->event )
	{
		case BF_FUS::COMMIT_COMPLETE:
			if ( ( LOCAL_COMP_INDEX_0 == comp_id ) && ( LOCAL_COMP_APP_IMG_INDEX_0 == image_id ) )
			{
				/* Reset should get triggered after session delete */
				reset_trigger = true;
				/* Update the OTA Data partition during APP Image Upgrade only,
				 * Bootloader read the OTA data while booting to select active OTA partition.
				 */
				return_status = Update_Ota_Data();
			}

#ifdef WEB_SERVER_SUPPORT

			else if ( ( LOCAL_COMP_INDEX_0 == comp_id ) && ( LOCAL_COMP_WEB_IMG_INDEX_1 == image_id ) )
			{
				uint32_t web_rev_num = image_config[LOCAL_COMP_WEB_IMG_INDEX_1].version;
				BF_ASSERT( true == fw_dcid_list[LOCAL_COMP_WEB_IMG_INDEX_1].version->Check_In( &web_rev_num ) );
				BF_ASSERT( true == fw_dcid_list[LOCAL_COMP_WEB_IMG_INDEX_1].version->Check_In_Init( &web_rev_num ) );

				BF_FUS::Image_Header* image_header[LOCAL_COMP_TOTAL_IMAGES];
				image_header[LOCAL_COMP_APP_IMG_INDEX_0] =
					reinterpret_cast<BF_FUS::Esp32_Image_Ctrl*>( image_ctrl_list[LOCAL_COMP_APP_IMG_INDEX_0] );
				image_header[LOCAL_COMP_WEB_IMG_INDEX_1] =
					reinterpret_cast<BF_FUS::Esp32_Image_Ctrl*>( image_ctrl_list[LOCAL_COMP_WEB_IMG_INDEX_1] );
				image_header[LOCAL_COMP_APP_IMG_INDEX_0]->Get_Compatibility_Num( &comp_num[
																					 LOCAL_COMP_APP_IMG_INDEX_0] );
				image_header[LOCAL_COMP_WEB_IMG_INDEX_1]->Get_Compatibility_Num( &comp_num[
																					 LOCAL_COMP_WEB_IMG_INDEX_1] );
				PROD_SPEC_MEM_Get_WEB_NV_Handle( &web_ui_nv_ctrl );

				if ( comp_num[LOCAL_COMP_APP_IMG_INDEX_0] == comp_num[LOCAL_COMP_WEB_IMG_INDEX_1] )
				{
					Assign_Web_Pages( RUN_WEB_UI, web_ui_nv_ctrl );
				}
				else
				{
					Assign_Web_Pages( RUN_MAINTENANCE_UI, web_ui_nv_ctrl );
				}
				return_status = true;
			}
#endif	// WEB_SERVER_SUPPORT
#ifdef REST_I18N_LANGUAGE
			else if ( ( LOCAL_COMP_INDEX_0 == comp_id ) && ( LOCAL_COMP_LANG_IMG_INDEX_2 == image_id ) )
			{
				uint32_t lang_rev_num = image_config[LOCAL_COMP_LANG_IMG_INDEX_2].version;
				BF_ASSERT( true == fw_dcid_list[LOCAL_COMP_LANG_IMG_INDEX_2].version->Check_In( &lang_rev_num ) );
				BF_ASSERT( true == fw_dcid_list[LOCAL_COMP_LANG_IMG_INDEX_2].version->Check_In_Init( &lang_rev_num ) );
				BF_FUS::Image_Header* image_header[LOCAL_COMP_TOTAL_IMAGES];
				image_header[LOCAL_COMP_APP_IMG_INDEX_0] =
					reinterpret_cast<BF_FUS::Esp32_Image_Ctrl*>( image_ctrl_list[LOCAL_COMP_APP_IMG_INDEX_0] );
				image_header[LOCAL_COMP_LANG_IMG_INDEX_2] =
					reinterpret_cast<BF_FUS::Esp32_Image_Ctrl*>( image_ctrl_list[LOCAL_COMP_LANG_IMG_INDEX_2] );
				image_header[LOCAL_COMP_APP_IMG_INDEX_0]->Get_Compatibility_Num( &comp_num[
																					 LOCAL_COMP_APP_IMG_INDEX_0] );
				image_header[LOCAL_COMP_LANG_IMG_INDEX_2]->Get_Compatibility_Num( &comp_num[
																					  LOCAL_COMP_LANG_IMG_INDEX_2] );

				if ( comp_num[LOCAL_COMP_APP_IMG_INDEX_0] == comp_num[LOCAL_COMP_LANG_IMG_INDEX_2] )
				{
					/* Reset after upgrading the Language image */
					if ( PROD_SPEC_REST_LANG_Init() == true )
					{
						return_status = true;
					}
					else
					{
						PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR,
											   "Language data initialization failed during firmware upgrade" );
					}
				}
				else
				{
					PROD_SPEC_DEBUG_PRINT( DBG_MSG_ERROR,
										   "Language image is incompatible with Application firmware" );
				}
			}
#endif	// REST_I18N_LANGUAGE
			break;

		case BF_FUS::SESSION_DELETED:
			if ( reset_trigger == true )
			{
				if ( true == key_upgrade_msg )
				{
					return_status = Update_Code_Sign_Cert();
					if ( true == return_status )
					{
						FUS_DEBUG_PRINT( DBG_MSG_DEBUG, "Cert store updated successfully" );
					}
					else
					{
						FUS_DEBUG_PRINT( DBG_MSG_DEBUG, "Cert store update failed" );
					}
				}
#ifdef WEB_SERVER_SUPPORT
				/*
				 * Reboot device since ota partition switch is required for new app image
				 * Reboot is not required for web image upgrade.
				 * Since web image is directly copied to its actual partition.
				 * If App firmware is being upgraded by IoT then reboot should not be done here.
				 * Once success response is published to Azure IoT hub then
				 * reboot will be done in IOT_Net::Timer_Task()
				 */
				if ( !( IOT_Fus::Reboot_Required() ) )
				{
					if ( 0U != Fw_Reboot() )
					{}
				}
#endif
			}
			else
			{
#ifdef WEB_SERVER_SUPPORT
				/*
				 * Check and assign web page as maintenance page
				 * in case incomplete web image upgrade
				 */
				if ( fw_image_id == LOCAL_COMP_WEB_IMG_INDEX_1 )
				{
					fw_image_id = 0;
					web_fus_status->Check_Out( &web_status );

					PROD_SPEC_MEM_Get_WEB_NV_Handle( &web_ui_nv_ctrl );

					if ( WEB_FW_UPGRADE_IDLE != web_status )
					{
						Assign_Web_Pages( RUN_MAINTENANCE_UI, web_ui_nv_ctrl );

						/*
						 * Set web_status to idle again
						 */
						web_status = WEB_FW_UPGRADE_IDLE;
						web_fus_status->Check_In( &web_status );
						web_fus_status->Check_In_Init( &web_status );
					}
				}
				return_status = true;
#endif	// WEB_SERVER_SUPPORT
			}

			key_upgrade_msg = false;

			break;

		default:
			break;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Is_Fw_Upgrade_Enabled( void )
{
	uint8_t fw_upgrade_ena = 0U;
	bool status = true;	/// < Upgrade allowed by default

	fw_upgrade_mode->Check_Out( &fw_upgrade_ena );
	switch ( fw_upgrade_ena )
	{
		case BF_FUS::NO_FW_UPGRADE:
			// Firmware upgrade not allowed
			status = false;
			break;

		case BF_FUS::UNRESTRICTED_FW_UPGRADE:
		case BF_FUS::RESTRICTED_FW_UPGRADE:
		default:
			break;
	}
	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Set_App_Fw_Upgrade_State( uint8_t state )
{
	bool ret_val;

#ifdef USE_NO_INIT_RAM
	no_init_ram.field.msg_for_boot = state;
	ret_val = true;
#else
	ret_val = NV::NV_Mem::Write( reinterpret_cast<uint8_t*>( &state ), MSG_FOR_BOOT_NVADD,
								 0x01U );
#endif
	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Get_App_Fw_Upgrade_State( uint8_t* state )
{
	bool ret_val;

#ifdef USE_NO_INIT_RAM
	*state = no_init_ram.field.msg_for_boot;
	ret_val = true;
#else
	ret_val = NV::NV_Mem::Read( reinterpret_cast<uint8_t*>( state ), MSG_FOR_BOOT_NVADD, 0x01U );
#endif
	return ( ret_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Prod_Spec_FUS_Get_Image_NV_Handle( uint8_t processor_index, uint8_t image_index, NV_Ctrl** image_nv_ctrl )
{
	/* Get the size of particular image of head processor */
	if ( processor_index == LOCAL_COMP_INDEX_0 )
	{
		/* Get specific image handle */
		switch ( image_index )
		{
#ifdef PKI_SUPPORT
			case 0:
				PROD_SPEC_MEM_Get_APP_NV_Handle( image_nv_ctrl );
				break;

#endif
	#ifdef WEB_SERVER_SUPPORT
			case 1:
				PROD_SPEC_MEM_Get_WEB_NV_Handle( image_nv_ctrl );
				break;

	#endif
	#ifdef REST_I18N_LANGUAGE
			case 2:
				PROD_SPEC_MEM_Get_Lang_NV_Handle( image_nv_ctrl );
				break;

	#endif
			default:
				// Do nothing
				break;
		}
	}
	else
	{
		/* Product code will handle the logic to get image start address of
		 * processors other than head processor */
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#ifdef PKI_SUPPORT
void PROD_SPEC_FUS_Get_Scratch_NV_Handle( NV_Ctrl** nv_ctrl )
{
	PROD_SPEC_MEM_Get_Scratch_NV_Handle( nv_ctrl );
}

#endif
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Prod_Spec_FUS_Is_Compatible( unsigned long compat_number_from_interface )
{
	bool return_status = true;

	/* Adopter may add code here to restrict compatibility number as shown in below example
	   if (0U == compat_number_from_interface )
	   {
	    return_status = false;
	   }*/
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Prod_Spec_FUS_Is_App_Image( uint8_t processor_index, uint8_t image_index )
{
	bool return_status = false;

	/* Check if this image is app image */
	if ( ( processor_index == LOCAL_COMP_INDEX_0 ) && ( image_index == LOCAL_COMP_APP_IMG_INDEX_0 ) )
	{
		return_status = true;
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_ERROR_TD Get_DCID_Value( DCI_ID_TD dcid, uint8_t* data )
{
	DCI_ERROR_TD return_status;
	DCI_ACCESS_ST_TD access_struct;

	BF_ASSERT( patron_handle );
	access_struct.command = DCI_ACCESS_GET_RAM_CMD;
	access_struct.data_id = dcid;
	access_struct.source_id = source_id;
	access_struct.data_access.data = data;
	access_struct.data_access.length = 0U;
	patron_handle->Get_Length( dcid, &access_struct.data_access.length );
	access_struct.data_access.offset = 0U;

	return_status = patron_handle->Data_Access( &access_struct );

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool Update_Ota_Data( void )
{
	bool partition_status = false;
	esp_partition_t ota_partition = { };
	bool ota_status = false;

	partition_status = ESP32_Partition::Get_Next_Update_Partition( &ota_partition, nullptr );

	if ( partition_status == true )
	{
		ota_status = ESP32_Partition::Ota_Set_Boot_Partition( &ota_partition );

		if ( ota_status )
		{
			FUS_DEBUG_PRINT( DBG_MSG_DEBUG, "OTA Data partition is updated successfully" );
		}
		else
		{
			FUS_DEBUG_PRINT( DBG_MSG_DEBUG, "OTA Data partition update has failed" );
		}
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_DEBUG,  "Failed to get next update partition details" );
	}

	return ( ota_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Get_Image_Start_Address( uint8_t processor_index, uint8_t image_index )
{
	uint32_t image_start_address = 0;
	NV_Ctrl* image_nv_ctrl = nullptr;

	Prod_Spec_FUS_Get_Image_NV_Handle( processor_index, image_index, &image_nv_ctrl );
	if ( image_nv_ctrl != nullptr )
	{
		NV_Ctrl::mem_range_info_t mem_range = {0U, 0U, 0U};
		image_nv_ctrl->Get_Memory_Info( &mem_range );
		image_start_address = mem_range.start_address;
	}
	else
	{
		FUS_DEBUG_PRINT( DBG_MSG_INFO, "Failed to get image NV handle" );
	}

	return ( image_start_address );
}
