/**
 *****************************************************************************************
 *  @file
 *	@brief Abstract class for component control.
 *
 *	@details Defines interfaces for any component.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef COMPONENT_CTRL_H
	#define COMPONENT_CTRL_H

#include "Image.h"

namespace BF_FUS
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Global Variable declaration
 *****************************************************************************************
 */
/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class Component_Ctrl
{
	public:
		/**
		 *	 @brief Structure used to get the component information.
		 */
		struct comp_config_t
		{
			uint32_t firm_rev;
			uint32_t serial_num;
		};

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members,
		 * configuration members initialized appropriately.
		 * @param[in] image_ctrls: image Control .
		 * @param[in] num_image_ctrls: Total number of images
		 */
		Component_Ctrl();
		~Component_Ctrl( void );

		/*
		 *****************************************************************************************
		 * @ Get the status of the component to create session
		 * @return Wait time of the component to go into firmware upgrade
		 *****************************************************************************************
		 */
		virtual uint32_t Goto_FUM( void ) = 0;

		/*
		 *****************************************************************************************
		 * @ Get the image count of the respective component
		 * @ param[out] image_count : image_count
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		virtual fus_op_status_t Get_Image_Count( uint8_t* image_count ) = 0;

		/*
		 *****************************************************************************************
		 * @ collect the component image related information .
		 * @param[in] comp_info_req: requested component
		 * @ param[out] comp_info_resp : component information response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		virtual fus_op_status_t Get_Comp_Desc( comp_info_resp_t* comp_resp ) = 0;

		/*******************************************************************************************
		 * @ Get the Image information.
		 * @param[in] image_info_req: requested Image
		 * @ param[out] image_info_resp_t : Image information response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		virtual fus_op_status_t Get_Image_Specs( image_info_req_t* image_req,
												 image_info_resp_t* image_resp ) = 0;

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
		 * @param[in] image_validate_req_t: Validating image info
		 * @ param[out] image_validate_resp_t : Wait time and status as response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		virtual fus_op_status_t Validate( image_validate_req_t* validate_req,
										  image_validate_resp_t* validate_resp ) = 0;

		/*
		 *****************************************************************************************
		 * @ Commit the image API to copy data from source to destination
		 * @ param[in] image_commit_req_t: Commit operation required information
		 * @ param[out] image_commit_resp_t : Wait time as response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		virtual fus_op_status_t Commit( image_commit_req_t* commit_req,
										image_commit_resp_t* commit_resp ) = 0;

		/*
		 *****************************************************************************************
		 * @ Get the current status of the memory controls.
		 * @ param[in] image_status_req_t: requested image
		 * @ param[out] image_status_resp_t : Image information response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		virtual fus_op_status_t Status( image_status_req_t* status_req,
										image_status_resp_t* status_resp ) = 0;

		/*
		 *****************************************************************************************
		 * @ Reset the all the state, variable after delete the firmware upgrade session
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		virtual fus_op_status_t Exit_FUM( void ) = 0;

	private:

};

}
#endif
