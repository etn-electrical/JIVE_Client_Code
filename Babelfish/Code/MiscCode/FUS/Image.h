/**
 *****************************************************************************************
 *  @file

 *	@brief Abstract class for image control.
 *
 *	@details Defines interfaces for any image.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef IMAGE_H
#define IMAGE_H

#include "FUS_Msg_Structs.h"
#include "FUS_Defines.h"
#include "NV_Ctrl.h"

namespace BF_FUS
{
/*
 *****************************************************************************************
 *		Typedefs
 *****************************************************************************************
 */

/**
 * @brief Return State for Image Ctrl operations
 */
enum image_state_t
{
	IDLE,
	INIT,
	WAIT,
	RETRY,
	ERASE,
	WRITE,
	READ,
	VALIDATE,
	COMMIT,
	ERROR,
	RESET
};

/**
 * @brief Return sub events for Image Ctrl operations
 */
enum image_sub_event_t
{
	COMMON_IDLE,
	COMMIT_ERASE_REAL_NV,
	COMMIT_READ_HEADER,
	COMMIT_EXTRACT_HEADER,
	COMMIT_READ_DATA,
	COMMIT_WRITE_DATA,
	COMMIT_WAIT,
	VALIDATE_READ_HEADER,
	VALIDATE_EXTRACT_HEADER,
	VALIDATE_READ_DATA,
	CALCULATE_CRC_DATA,
	VALIDATE_CRC_DATA,
	VALIDATE_CODE_SIGNING,
	VALIDATE_PROD_INFO,
};

/**
           @brief Structure used to define the various operation while executing image.
 */
struct fw_state_op_t
{
	NV_Ctrl* untrusted_nv_ctrl;
	NV_Ctrl* trusted_nv_ctrl;
	uint32_t address;
	uint8_t* data;
	uint32_t length;
	uint32_t fw_length;
	uint32_t retry_timeout;
	uint8_t retry_count;
	uint32_t wait_time;
	uint32_t start_time;
	uint16_t fw_crc;
	image_state_t state;
	image_sub_event_t sub_state;
	image_state_t state_prev;
	fus_op_status_t status;
};

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class Image
{
	public:
		/**
		   @brief Structure used to get the image information.
		 */
		struct img_config_t
		{
			uint8_t* name;
			uint8_t name_len;
			uint32_t version;
			uint16_t guid;
			uint8_t upgrade_mode;
		};

		Image();
		~Image( void );

		/*
		 *****************************************************************************************
		 * @ Get the Image information.
		 * @ param[out] image_info_resp_t : Image information response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		virtual fus_op_status_t Get_Image_Specs( image_info_resp_t* image_resp ) = 0;

		/*
		 *****************************************************************************************
		 * @ Write the data into the image memory space.
		 * @ param[in] image_write_req_t: Writing the data info
		 * @ param[out] image_write_resp_t : Wait time as response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		virtual fus_op_status_t Write( image_write_req_t* write_req,
									   image_write_resp_t* write_resp ) = 0;

		/*
		 *****************************************************************************************
		 * TODO
		 *****************************************************************************************
		 */
		virtual fus_op_status_t Read( image_read_req_t* read_mem,
									  image_read_resp_t* read_mem_resp ) = 0;

		/*
		 *****************************************************************************************
		 * @ Validate the Written image in the memory space
		 * @ param[out] image_validate_resp_t : Wait time and status as response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		virtual fus_op_status_t Validate( image_validate_req_t* validate_req,
										  image_validate_resp_t* validate_resp ) = 0;

		/*
		 *****************************************************************************************
		 * @ Commit the image API to copy data from source to destination
		 * @ param[out] image_commit_resp_t : Wait time as response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		virtual fus_op_status_t Commit( image_commit_resp_t* read_mem_resp ) = 0;

		/*
		 * *****************************************************************************************
		 * @ Get the current status of the memory controls.
		 * @ param[out] image_status_resp_t : Image information response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		virtual fus_op_status_t Status( image_status_resp_t* status_resp ) = 0;

		/*
		 *****************************************************************************************
		 * @ Reset the all the state, variable after delete the firmware upgrade session
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		virtual fus_op_status_t Exit_FUM( void ) = 0;

	/**
	 *  @brief Destructor
	 *  @details It will get called when object Image goes out of scope or deleted.
	 *  @return None
	 */
	/**
	 * @brief A protected constructor to ensure the class is only instantiated via specialized
	 * classes.
	 */

	protected:

	private:


};

}
#endif
