/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef     LOG_EVENTS_H
	#define     LOG_EVENTS_H

typedef uint8_t log_event_t;
typedef uint8_t log_id_t;
typedef uint8_t log_version_t;

/* Toolkit Audit event logging code list */

/* Audit Power code list */
static const log_event_t LOG_EVENT_NO_EVENT = 0U;
static const log_event_t LOG_EVENT_DEVICE_RESTART = 1U;
static const log_event_t LOG_EVENT_FACTORY_RESET = 2U;

/* Firmware update code list */
static const log_event_t LOG_EVENT_FW_UPDATE_STARTED = 3U;
static const log_event_t LOG_EVENT_FW_UPDATE_COMPLETE = 4U;
static const log_event_t LOG_EVENT_CHECKSUM_FAILURE = 5U;
static const log_event_t LOG_EVENT_SIGNATURE_VERIFICATION_FAILURE = 6U;
static const log_event_t LOG_EVENT_VERSION_ROLLBACK_FAILURE = 7U;

/* Configuration */
static const log_event_t LOG_EVENT_NV_DATA_RESTORE_FAIL = 8U;
static const log_event_t LOG_EVENT_NV_PARAM_WRITE_FAIL = 9U;
static const log_event_t LOG_EVENT_NV_PARAM_ERASE_FAIL = 10U;
static const log_event_t LOG_EVENT_NV_PARAM_ERASE_ALL_FAIL = 11U;

/* User audit annd Log memory clear code list */
static const log_event_t LOG_EVENT_SELF_USER_PWD_CHANGED = 12U;
static const log_event_t LOG_EVENT_OTHER_USER_PWD_CHANGED = 13U;
static const log_event_t LOG_EVENT_USER_CREATED = 14U;
static const log_event_t LOG_EVENT_USER_DELETED = 15U;
static const log_event_t LOG_EVENT_ADMIN_USER = 16U;
static const log_event_t LOG_EVENT_LOG_IN_USER_X = 17U;
static const log_event_t LOG_EVENT_LOG_OUT_USER_X = 18U;
static const log_event_t LOG_EVENT_LOG_LOCKED_USER_X = 19U;
static const log_event_t LOG_EVENT_RESET_DEFAULT_ADMIN_USER = 20U;
static const log_event_t LOG_EVENT_RESET_ALL_ACCOUNT_USERS = 21U;
static const log_event_t LOG_EVENT_DATA_LOG_MEM_CLEAR = 22U;
static const log_event_t LOG_EVENT_EVENT_LOG_MEM_CLEAR = 23U;
static const log_event_t LOG_EVENT_AUDIT_POWER_LOG_MEM_CLEAR = 24U;
static const log_event_t LOG_EVENT_AUDIT_FW_UPDATE_LOG_MEM_CLEAR = 25U;
static const log_event_t LOG_EVENT_AUDIT_USER_LOG_MEM_CLEAR = 26U;
static const log_event_t LOG_EVENT_AUDIT_CONFIG_LOG_MEM_CLEAR = 27U;

/* IOT FUS code list */
static const log_event_t LOG_EVENT_IOT_FUS_IMAGE_UPGRADE_STARTED = 28U;
static const log_event_t LOG_EVENT_IOT_FUS_IMAGE_UPGRADE_COMPLETED = 29U;
static const log_event_t LOG_EVENT_IOT_FUS_IMAGE_UPGRADE_ABORTED = 30U;
static const log_event_t LOG_EVENT_IOT_FUS_FAILED_DUE_TO_BAD_PARAM = 31U;
static const log_event_t LOG_EVENT_IOT_FUS_CHECKSUM_FAILURE = 32U;
static const log_event_t LOG_EVENT_IOT_FUS_SIGNATURE_VERIFICATION_FAILURE = 33U;
static const log_event_t LOG_EVENT_IOT_FUS_VERSION_ROLLBACK_FAILURE = 34U;
static const log_event_t LOG_EVENT_IOT_FUS_REJECTED_BY_PRODUCT = 35U;

/* Log memory Integrity fail code list ( added in release 2.17 ) */
static const log_event_t LOG_EVENT_MEM_INTEGRITY_FAIL_DATA_LOG_MEM_CLEAR = 36U;
static const log_event_t LOG_EVENT_MEM_INTEGRITY_FAIL_EVENT_LOG_MEM_CLEAR = 37U;
static const log_event_t LOG_EVENT_MEM_INTEGRITY_FAIL_AUDIT_POWER_LOG_MEM_CLEAR = 38U;
static const log_event_t LOG_EVENT_MEM_INTEGRITY_FAIL_AUDIT_FW_UPDATE_LOG_MEM_CLEAR = 39U;
static const log_event_t LOG_EVENT_MEM_INTEGRITY_FAIL_AUDIT_USER_LOG_MEM_CLEAR = 40U;
static const log_event_t LOG_EVENT_MEM_INTEGRITY_FAIL_AUDIT_CONFIG_LOG_MEM_CLEAR = 41U;

/* Project Specific Event logging code list can be started from 101 */
static const log_event_t LOG_EVENT_TEST_EVENT_101 = 101U;
static const log_event_t LOG_EVENT_TEST_EVENT_102 = 102U;

#endif
