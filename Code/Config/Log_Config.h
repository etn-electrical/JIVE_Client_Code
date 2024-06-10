/**
 *****************************************************************************************
 * @file Log_Config.h
 * @details This file has definitions related to log configuration.
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef LOG_CONFIG_H
	#define LOG_CONFIG_H

#include "Includes.h"
#include "uC_FLASH.h"
#include "Log_Events.h"
#include "Log_Mem_NV.h"
#include "Log_Mem_RAM.h"
#include "Log_DCI.h"
#include "Audit_Log_DCID_list.h"
#include "Prod_Spec_LTK_ESP32.h"


#ifdef LOGGING_SUPPORT
#define REST_LOGGING
#endif

static const log_id_t DATA_LOG_ID = 0U;
static const log_id_t EVENT_LOG_ID = 1U;
static const log_id_t AUDIT_POWER_LOG_ID = 2U;
static const log_id_t AUDIT_FW_UPDATE_LOG_ID = 3U;
static const log_id_t AUDIT_USER_LOG_ID = 4U;
static const log_id_t AUDIT_CONFIG_LOG_ID = 5U;
/*Assign log ids here for log types (like data,alarm, audit) and increment TOTAL_LOG_ID count*/
#ifdef LOGGING_ON_INT_FLASH
static const log_id_t TOTAL_LOG_ID = 2U;
#else
static const log_id_t TOTAL_LOG_ID = 6U;
#endif

/*Log version must be changed if user data parameters changed ( change in logging format ) */
static const log_version_t DATA_LOG_SPEC_VERSION = 1U;
static const log_version_t EVENT_LOG_SPEC_VERSION = 1U;
static const log_version_t AUDIT_POWER_LOG_SPEC_VERSION = 1U;
static const log_version_t AUDIT_FW_UPDATE_LOG_SPEC_VERSION = 1U;
static const log_version_t AUDIT_USER_LOG_SPEC_VERSION = 1U;
static const log_version_t AUDIT_CONFIG_LOG_SPEC_VERSION = 1U;

/* Log types DCIDs, keep an order of log types same as log ids  */
const DCI_ID_TD log_type_dcid_array[] =
{
	DCI_DATA_LOG_DCID,
	DCI_EVENT_LOG_DCID,
#ifndef LOGGING_ON_INT_FLASH
	DCI_AUDIT_POWER_LOG_DCID,
	DCI_AUDIT_FW_UPGRADE_LOG_DCID,
	DCI_AUDIT_USER_LOG_DCID,
	DCI_AUDIT_CONFIG_LOG_DCID
#endif
};


/* ***********************DCID list for Data and Event logging types *********************
   User data parameters for data logging*/
const DCI_ID_TD data_log_dcid_array[] =
{
	DCI_UNIX_EPOCH_TIME_DCID,
	DCI_PHY1_LINK_SPEED_ACTUAL_DCID,
	DCI_SIMPLE_APP_CONTROL_WORD_DCID
};

/*	Logging parameters to be watched for change based logging*/
const DCI_ID_TD data_log_dcid_watch_array[] =
{
	// DCI_LOG_TEST_1_DCID,
	DCI_SIMPLE_APP_CONTROL_WORD_DCID
};

/*Parameters for event logging*/
const DCI_ID_TD event_log_dcid_array[] =
{
	DCI_UNIX_EPOCH_TIME_DCID,
	DCI_LOGGING_EVENT_CODE_DCID,
	DCI_DEVICE_STATE_DCID
};

/* Flash sector assignment for Data log */
#ifdef LOGGING_ON_INT_FLASH
/* Flash sector assignment for Data log */

static const uint8_t DATA_LOG_MAX_SECTOR = 2U;
static const BF_Misc::Log_Mem_NV::memory_block_t DATA_LOG_ROM_MEM_MAP[DATA_LOG_MAX_SECTOR] =
{
	// todo correct memory map need to add for esp32 for data log
	{  0U,  0U,  0U, 0U },
	{  0U,  0U,  0U, 0U },
};


static const NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD DATA_LOG_FLASH_CFG =
{
	DATA_LOG_ROM_MEM_MAP[0U].sector_start_addr,	/* start_address */
	DATA_LOG_ROM_MEM_MAP[0U].sector_start_addr,	/* mirror_start_address */
	DATA_LOG_ROM_MEM_MAP[DATA_LOG_MAX_SECTOR - 1U].sector_end_addr,	/* end_address */
};


/* Flash sector assignment for Power Audit log */

static const uint8_t AUDIT_POWER_LOG_MAX_SECTOR = 2U;
static const BF_Misc::Log_Mem_NV::memory_block_t AUDIT_POWER_LOG_ROM_MEM_MAP[
	AUDIT_POWER_LOG_MAX_SECTOR] =
{
	// todo correct memory map need to add for esp32 for power events
	{  0U,  0U,  0U, 0U },
	{  0U,  0U,  0U, 0U },
};


static const NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD AUDIT_POWER_LOG_FLASH_CFG =
{
	AUDIT_POWER_LOG_ROM_MEM_MAP[0U].sector_start_addr,	/*start_address*/
	AUDIT_POWER_LOG_ROM_MEM_MAP[0U].sector_start_addr,	/*mirror_start_address*/
	AUDIT_POWER_LOG_ROM_MEM_MAP[AUDIT_POWER_LOG_MAX_SECTOR - 1U].sector_end_addr,	/*end_address*/
};

/* Flash sector assignment for Config Audit log */

static const uint8_t AUDIT_CONFIG_LOG_MAX_SECTOR = 2U;
static const BF_Misc::Log_Mem_NV::memory_block_t AUDIT_CONFIG_LOG_ROM_MEM_MAP[
	AUDIT_CONFIG_LOG_MAX_SECTOR] =
{
	// todo correct memory map need to add for esp32 for config log
	{  0U,  0U,  0U, 0U },
	{  0U,  0U,  0U, 0U },
};


static const NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD AUDIT_CONFIG_LOG_FLASH_CFG =
{
	AUDIT_CONFIG_LOG_ROM_MEM_MAP[0U].sector_start_addr,	/*start_address*/
	AUDIT_CONFIG_LOG_ROM_MEM_MAP[0U].sector_start_addr,	/*mirror_start_address*/
	AUDIT_CONFIG_LOG_ROM_MEM_MAP[AUDIT_CONFIG_LOG_MAX_SECTOR - 1U].sector_end_addr,		/*end_address*/
};
/*Flash sector assignment for Event log*/

static const uint8_t EVENT_LOG_MAX_SECTOR = 2U;
static const BF_Misc::Log_Mem_NV::memory_block_t EVENT_LOG_ROM_MEM_MAP[EVENT_LOG_MAX_SECTOR] =
{
	// todo correct memory map need to add for esp32 Event log
	{  0U,  0U,  0U, 0U },
	{  0U,  0U,  0U, 0U },
}
static const NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD EVENT_LOG_FLASH_CFG =
{
	EVENT_LOG_ROM_MEM_MAP[0U].sector_start_addr,	/*start_address*/
	EVENT_LOG_ROM_MEM_MAP[0U].sector_start_addr,	/*mirror_start_address*/
	EVENT_LOG_ROM_MEM_MAP[EVENT_LOG_MAX_SECTOR - 1U].sector_end_addr,		/*end_address*/
};

/* Flash sector assignment for Firmware update event log */

static const uint8_t AUDIT_FW_UPDATE_LOG_MAX_SECTOR = 2U;
static const BF_Misc::Log_Mem_NV::memory_block_t AUDIT_FW_UPDATE_LOG_ROM_MEM_MAP[
	AUDIT_FW_UPDATE_LOG_MAX_SECTOR] =
{
	/* currently using the same sector as used by event log.need to comment data log init to test firmware update
	   event */
	// todo correct memory map need to add for esp32 for Firmware update
	{  0U,  0U,  0U, 0U },
	{  0U,  0U,  0U, 0U },
};


static const NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD AUDIT_FW_UPDATE_LOG_FLASH_CFG =
{
	AUDIT_FW_UPDATE_LOG_ROM_MEM_MAP[0U].sector_start_addr,		/*start_address*/
	AUDIT_FW_UPDATE_LOG_ROM_MEM_MAP[0U].sector_start_addr,		/*mirror_start_address*/
	AUDIT_FW_UPDATE_LOG_ROM_MEM_MAP[AUDIT_FW_UPDATE_LOG_MAX_SECTOR - 1U].sector_end_addr,		/*end_address*/
};

/* Flash sector assignment for User event log */

static const uint8_t AUDIT_USER_LOG_MAX_SECTOR = 2U;
static const BF_Misc::Log_Mem_NV::memory_block_t AUDIT_USER_LOG_ROM_MEM_MAP[
	AUDIT_USER_LOG_MAX_SECTOR] =
{
	/* currently using the same sector as used by event log.need to comment data log init to test user event*/
	// todo correct memory map need to add for esp32 for user event
	{  0U,  0U,  0U, 0U },
	{  0U,  0U,  0U, 0U },
};


static const NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD AUDIT_USER_LOG_FLASH_CFG =
{
	AUDIT_USER_LOG_ROM_MEM_MAP[0U].sector_start_addr,		/*start_address*/
	AUDIT_USER_LOG_ROM_MEM_MAP[0U].sector_start_addr,		/*mirror_start_address*/
	AUDIT_USER_LOG_ROM_MEM_MAP[AUDIT_USER_LOG_MAX_SECTOR - 1U].sector_end_addr,		/*end_address*/
};


#elif defined( LOGGING_ON_EXT_FLASH )
/* Refer Ext_Mem_Config.h to find log memory configurations */

#elif defined( LOGGING_ON_RAM )

/* Allocate the size for different logtype in RAM */
static const uint32_t DATA_LOG_RAM_BLOCK_SIZE = 500U;
static const uint32_t EVENT_LOG_RAM_BLOCK_SIZE = 200U;
static const uint32_t AUDIT_POWER_LOG_RAM_BLOCK_SIZE = 200U;
static const uint32_t AUDIT_CONFIG_LOG_RAM_BLOCK_SIZE = 200U;
static const uint32_t AUDIT_FW_UPDATE_LOG_RAM_BLOCK_SIZE = 200U;
static const uint32_t AUDIT_USER_LOG_RAM_BLOCK_SIZE = 200U;

#endif

#endif	// LOG_CONFIG_H
