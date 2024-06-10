/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef AUDIT_LOG_DCID_LIST_H
	#define AUDIT_LOG_DCID_LIST_H

/*DCI Parameters for power audit logging*/
const DCI_ID_TD audit_power_log_dcid_array[] =
{
	DCI_UNIX_EPOCH_TIME_DCID,
	DCI_LOGGING_EVENT_CODE_DCID,
	DCI_RESET_CAUSE_DCID,
	DCI_DEVICE_STATE_DCID
};

/*DCI Parameters for power audit logging*/
const DCI_ID_TD audit_config_log_dcid_array[] =
{
	DCI_UNIX_EPOCH_TIME_DCID,
	DCI_LOGGING_EVENT_CODE_DCID,
	DCI_NV_MEM_ID_DCID,
	DCI_NV_PARAMETER_ADDRESS_DCID,
	DCI_NV_FAIL_OPERATION_DCID
};

/*DCI Parameters for Firmware update audit logging*/
const DCI_ID_TD audit_fw_update_log_dcid_array[] =
{
	DCI_UNIX_EPOCH_TIME_DCID,
	DCI_LOGGING_EVENT_CODE_DCID,
	DCI_LOG_FIRMWARE_VERSION_DCID,
	DCI_LOGGED_USERNAME_DCID,
	DCI_LOG_PROC_IMAGE_ID_DCID
};

/*DCI Parameters for User audit logging*/
const DCI_ID_TD audit_user_log_dcid_array[] =
{
	DCI_UNIX_EPOCH_TIME_DCID,
	DCI_LOGGING_EVENT_CODE_DCID,
	DCI_LOGGED_USERNAME_DCID,
	DCI_LOG_IP_ADDRESS_DCID,
	DCI_LOG_PORT_NUMBER_DCID
};
#endif
