/**
 *****************************************************************************************
 *  @file		Esp32_Image_Ctrl.h
 *	@brief      image control class for esp32.
 *
 *	@details    This class will provide interfaces of image controls for Firmware Upgrade Service.
 *
 *	@copyright  2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef ESP32_IMAGE_CTRL_H
#define ESP32_IMAGE_CTRL_H

#include "Image.h"
#include "NV_Ctrl.h"
#include "Commit_Ctrl.h"
#include "StdLib_MV.h"
#include "Timers_Lib.h"
#include "CR_Tasker.h"
#include "Image_Header.h"
#include "CRC16.h"
#include "FUS_Config.h"
#include "Proc_MEM_ID_Codes.h"
#include "Fw_Store.h"
#include "Prod_Spec_PKI.h"
#include "FUS_Debug.h"
#include "Fw_Store.h"
#include "esp_ota_ops.h"

namespace BF_FUS
{
/*
 *****************************************************************************************
 *		Typedefs
 *****************************************************************************************
 */

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class Esp32_Image_Ctrl : public Image
	, public Image_Header
{
	public:

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members,
		 * configuration members initialized appropriately.
		 * @param[in] scratch_nv_ctrl: Scratch NV Control .
		 * @param[in] real_nv_ctrl:REal NV Control
		 * @param[in] image_config: DCI names for configuration.
		 * @param[in] commit_ctrl: Commit control.
		 * @return Status
		 */
		Esp32_Image_Ctrl( NV_Ctrl* scratch_nv_ctrl, NV_Ctrl* real_nv_ctrl,
						  img_config_t* image_config, Commit_Ctrl* commit_ctrl );
		~Esp32_Image_Ctrl( void );

		/*
		 *****************************************************************************************
		 * @ Get the Image information.
		 * @ param[out] image_info_resp_t : Image information response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		fus_op_status_t Get_Image_Specs( image_info_resp_t* image_resp );

		/*
		 *****************************************************************************************
		 * @ Write the data into the image memory space.
		 * @ param[in] image_write_req_t: Writing the data info
		 * @ param[out] image_write_resp_t : Wait time as response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		fus_op_status_t Write( image_write_req_t* write_mem,
							   image_write_resp_t* write_mem_resp );

		/*
		 *****************************************************************************************
		 * TODO
		 *****************************************************************************************
		 */
		fus_op_status_t Read( image_read_req_t* read_mem,
							  image_read_resp_t* read_mem_resp )  { return ( FUS_NO_ERROR );}

		/*
		 *****************************************************************************************
		 * @ Validate the Written image in the memory space
		 * @ param[out] image_validate_resp_t : Wait time and status as response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		fus_op_status_t Validate( image_validate_req_t* validate_req,
								  image_validate_resp_t* validate_resp );

		/*
		 *****************************************************************************************
		 * @ Commit the image API to copy data from source to destination
		 * @ param[out] image_commit_resp_t : Wait time as response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		fus_op_status_t Commit( image_commit_resp_t* commit_resp );

		/*
		 * ***************************************************************************************
		 * @ Get the current status of the memory controls.
		 * @ param[out] image_status_resp_t : Image information response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		fus_op_status_t Status( image_status_resp_t* status_resp );

		/*
		 *****************************************************************************************
		 * @ Reset the all the state, variable after delete the firmware upgrade session
		 * @ return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		fus_op_status_t Exit_FUM( void );

		/*
		 *****************************************************************************************
		 * @ Call back function for the NV Ctrls operation status
		 * @param[in] status: NV Ctrl status
		 * @return : None
		 *****************************************************************************************
		 */
		void Image_Status_Cb( NV_Ctrl::nv_status_t status );

		/*
		 *****************************************************************************************
		 * @ STatic Call back function for the NV Ctrls operation status
		 * @param[in] param: A void pointer to an object passed as an argument to the cback function.
		 * @param[in] status: NV Ctrl status
		 * @return : None
		 *****************************************************************************************
		 */
		static void Image_Status_Cb_Static( NV_Ctrl::cback_param_t param, NV_Ctrl::nv_status_t status )
		{
			reinterpret_cast<Esp32_Image_Ctrl*>( param )->Image_Status_Cb( status );
		}

#ifdef OVERRIDE_FUS_TIME
		uint32_t m_set_validate_time;
		uint32_t m_set_commit_time;
		/*
		 *****************************************************************************************
		 * @brief: API to over-ride default validate wait time.
		 * @param[in] time_msec: Time in millisecond required for validation of image.
		 * @return : None
		 *****************************************************************************************
		 */
		void Set_Validate_Time( uint32_t time_msec );

		/*
		 *****************************************************************************************
		 * @brief: API to over-ride default commit wait time.
		 * @param[in] time_msec: Time in millisecond required for commit of image.
		 * @return : None
		 *****************************************************************************************
		 */
		void Set_Commit_Time( uint32_t time_msec );

#endif	// OVERRIDE_FUS_TIME

	protected:
		/*
		 *****************************************************************************************
		 * @ Verify address range against reference memory scope
		 * @param[in] address: Start address which needs to be validated
		 * @param[in] length: Length which needs to be validated
		 * @param[in] mem_range_ref: Reference memory range
		 * @return Status
		 *****************************************************************************************
		 */
		fus_op_status_t Check_Range( uint32_t address, uint32_t length,
									 const NV_Ctrl::mem_range_info_t* mem_range_ref );

	private:
		NV_Ctrl* m_scratch_nv_ctrl;
		img_config_t* m_image_config;
		Commit_Ctrl* m_commit_ctrl;
		fw_state_op_t m_image;
		uint32_t m_new_image_ver;
		uint16_t m_init_crc;
		uint16_t m_new_prod_code;
		bool m_is_scratch_erased;
		uint32_t m_esp_header_size;

		/*
		 *****************************************************************************************
		 * @ Check the incremental address request
		 * @ return : status
		 *****************************************************************************************
		 */
		bool New_Request( uint32_t address, uint8_t length );

		/*
		 *****************************************************************************************
		 * @ CR tasker for the image controller handler
		 * @param[in] param: A  pointer to an object passed as an argument CR tasker handler .
		 * @return : None
		 *****************************************************************************************
		 */
		void Image_Upgrade_Task( CR_Tasker* cr_task );

		/*
		 *****************************************************************************************
		 * @ STatic CR tasker for the image controller handler
		 * @param[in] param: A  pointer to an object passed as an argument CR tasker handler .
		 * @param[in] handle: Parameters passed for task creation.
		 * @return : None
		 *****************************************************************************************
		 */
		static void Image_Upgrade_Task_Static( CR_Tasker* cr_task, CR_TASKER_PARAM handle )
		{
			reinterpret_cast<Esp32_Image_Ctrl*>( handle )->Image_Upgrade_Task( cr_task );
		}

		// State machine handlers
		/*
		 *****************************************************************************************
		 * @ handle the write operation of the image control
		 * @ return : none
		 *****************************************************************************************
		 */
		void Write_Handler( void );

		/*
		 *****************************************************************************************
		 * @ handle the erase operation of the image
		 * @ return : none
		 *****************************************************************************************
		 */
		void Erase_Handler( void );

		/*
		 *****************************************************************************************
		 * @ handle the retry for the NV operation of the image control
		 * @ return : none
		 *****************************************************************************************
		 */
		void Retry_Handler( void );

		/*
		 *****************************************************************************************
		 * @ handle the all reset operation for the image
		 * @ return : none
		 *****************************************************************************************
		 */
		void Reset_Handler( void );

		/*
		 *****************************************************************************************
		 * @ handle the Error operation for the image
		 * @ return : none
		 *****************************************************************************************
		 */
		void Error_Handler( void );

		/*
		 *****************************************************************************************
		 * @ handle the validate operation of the image control
		 * @ return : none
		 *****************************************************************************************
		 */
		void Validate_Handler( void );

		/*
		 *****************************************************************************************
		 * @ handle the commit operation of the image control
		 * @ return : none
		 *****************************************************************************************
		 */
		void Commit_Handler( void );

		/*
		 *****************************************************************************************
		 * @ handle the wait time / delay while the execution of the image control operation
		 * @ return : none
		 *****************************************************************************************
		 */
		void Wait_Handler( void ){ }

		/*
		 *****************************************************************************************
		 * @ Handle the read header data for the validate operation
		 * @ return : none
		 *****************************************************************************************
		 */
		void Validate_Read_Header_Handler( void );

		/*
		 *****************************************************************************************
		 * @ Handle the header extraction for the validate operation
		 * @ return : none
		 *****************************************************************************************
		 */
		void Validate_Extract_Header_Handler( void );

		/*
		 *****************************************************************************************
		 * @ Handle the read data for the validate operation
		 * @ return : none
		 *****************************************************************************************
		 */
		void Validate_Read_Data_Handler( void );

		/*
		 *****************************************************************************************
		 * @ handle the CRC calculation of read data for the validate operation
		 * @ return : none
		 *****************************************************************************************
		 */
		void Validate_CRC_Calculation_Handler( void );

		/*
		 *****************************************************************************************
		 * @ handle the CRC validation operation
		 * @ return : none
		 *****************************************************************************************
		 */
		void Validate_CRC_Data_Handler( void );

		/*
		 *****************************************************************************************
		 * @ Handle the product signing operation.
		 * @ return : none
		 *****************************************************************************************
		 */
		void Validate_Code_Signing_Handler( void );

		/*
		 *****************************************************************************************
		 * @ Handle the product information validation operation.
		 * @ return : none
		 *****************************************************************************************
		 */
		void Validate_Product_Info_Handler( void );

		/*
		 *****************************************************************************************
		 * @ Checking the new firmware upgrade has roll back functionality or not.
		 * @ return : roll back functionality enable / disabled
		 *****************************************************************************************
		 */
		bool Check_Rollback_Enabled( void );

		/*
		 *****************************************************************************************
		 * @ Verifying the firmware upgrade version with the older one
		 * @ return : result for verifying firmware version
		 *****************************************************************************************
		 */
		bool Check_Version( void );

		/*
		 *****************************************************************************************
		 * @ Update the firmware version
		 * @ return : none
		 *****************************************************************************************
		 */
		void Update_Version( void );

};

}

#endif
