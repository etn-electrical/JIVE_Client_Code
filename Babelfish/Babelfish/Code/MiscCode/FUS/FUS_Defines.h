/**
 *****************************************************************************************
 *  @file
 *	@brief FUS defines used under firmware upgrade service
 *
 *	@details This file provides macros used for firmware upgrade service.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef FUS_DEFINES_H
#define FUS_DEFINES_H

namespace BF_FUS
{
/// *
// *****************************************************************************************
// *		Protocol structure
// *****************************************************************************************
// */
/**
 * @brief events for FUS operations
 */
enum fus_event_t
{
	EVENT_IDLE,
	INIT_VALID,
	INIT_INVALID,
	GOTO_FUM,
	SESSION_CREATED,
	WRITE_IN_PROGRESS,
	VALIDATE_COMPLETE,
	COMMIT_COMPLETE,
	SESSION_DELETED,
	FUS_OP_ERROR
};

/**
 * @brief Return Status for FUS Operation operations
 */
enum op_status_t
{
	SUCCESS,
	BUSY,		// Try again later.
	REJECT,		// Can't ever do it.
	REQUEST_INVALID,			// You asked for something but asked incorrectly.
	FAILURE,
};

typedef enum fus_upgrade_mode_t
{
	UNRESTRICTED_FW_UPGRADE,	/// < Upgrade to any version allowed
	RESTRICTED_FW_UPGRADE,		/// < Upgrade to same or higher version allowed
	NO_FW_UPGRADE				/// < Upgrade not allowed
} fus_upgrade_mode_t;

// Error Codes
enum fus_op_status_t
{
	FUS_NO_ERROR,
	FUS_BAD_COMMAND_ERROR,
	FUS_INVALID_PROCESSOR_ID_ERROR,
	FUS_INVALID_MEM_SELECTED_ERROR,
	FUS_MEMORY_ACCESS_VIOLATION_ERROR,	///< Memory was unable to be accessed due to being read only or currently
	///< active.
	FUS_BAD_DATA_ERROR,
	FUS_BAD_ADDRESS_ERROR,
	FUS_BAD_DATA_LENGTH_ERROR,
	FUS_PROCESSOR_NOT_READY_ERROR,
	FUS_PREVIOUS_CMD_ERROR,
	FUS_ACCESS_DENIED,					///< Read/Write Access to this particular memory device is restricted.
	FUS_ACCESS_TIME_PENDING,				/// Indicate that memory access time is undetermined now
	FUS_INVALID_ADDRESS,
	FUS_PENDING_CALL_BACK,
	FUS_COMMIT_DONE,
	FUS_VALID_IMAGE,
	FUS_VALIDATION_IN_PROGRESS,
	FUS_INVALID_IMAGE_CHECKSUM_FAILED,
	FUS_INVALID_IMAGE_SIGNATURE_FAILED,
	FUS_INVALID_IMAGE_PRODUCT_CODE_FAILED,
	FUS_INVALID_IMAGE_VERSION_ROLLBACK_FAILED,
	FUS_REQUEST_ERROR = 0x80			///< The access needs to be resent due to the processor being busy.
};

}

#endif
