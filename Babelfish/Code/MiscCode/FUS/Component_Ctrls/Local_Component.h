/**
 *****************************************************************************************
 *  @file
 *	@brief Local component control.
 *
 *	@details This class represents local component interfaces required for firmware upgrade service.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef LOCAL_COMPONENT_H
	#define LOCAL_COMPONENT_H

#include "Component_Ctrl.h"
#include "Image.h"
#include "FUS_Config.h"
#include "StdLib_MV.h"
#include "Proc_MEM_ID_Codes.h"

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
class Local_Component : public Component_Ctrl
{
	public:
		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members,
		 * configuration members initialized appropriately.
		 * @param[in] image_ctrls: image Control .
		 * @param[in] num_image_ctrls: Total number of images
		 * @param[in] comp_config: Component information.
		 * @return Status
		 */
		Local_Component( Image** image, uint8_t num_image_ctrls, comp_config_t* comp_config );

		~Local_Component( void );

		/*
		 *****************************************************************************************
		 * @ Get the status of the component to create session
		 * @return Wait time of the component to go into firmware upgrade
		 *****************************************************************************************
		 */
		uint32_t Goto_FUM( void );

		/*
		 *****************************************************************************************
		 * @ Get the image count of the respective component
		 * @ param[out] image_count : image_count
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		fus_op_status_t Get_Image_Count( uint8_t* image_count );

		/*
		 *****************************************************************************************
		 * @ collect the component image related information .
		 * @param[in] comp_info_req: requested component
		 * @ param[out] comp_info_resp : component information response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		fus_op_status_t Get_Comp_Desc( comp_info_resp_t* comp_resp );

		/*******************************************************************************************
		 * @ Get the Image information.
		 * @param[in] image_info_req: requested Image
		 * @ param[out] image_info_resp_t : Image information response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		fus_op_status_t Get_Image_Specs( image_info_req_t* image_req,
										 image_info_resp_t* image_resp );

		/*
		 *****************************************************************************************
		 * @ Write the data into the image memory space.
		 * @ param[in] image_write_req_t: Writing the data info
		 * @ param[out] image_write_resp_t : Wait time as response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		fus_op_status_t Write( image_write_req_t* write_req,
							   image_write_resp_t* write_resp );

		/*
		 *****************************************************************************************
		 * TODO
		 *****************************************************************************************
		 */
		virtual fus_op_status_t Read( image_read_req_t* read_mem,
									  image_read_resp_t* read_mem_resp ) {return ( FUS_NO_ERROR ); }

		/*
		 *****************************************************************************************
		 * @ Validate the Written image in the memory space
		 * @param[in] image_validate_req_t: Validating image info
		 * @ param[out] image_validate_resp_t : Wait time and status as response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		fus_op_status_t Validate( image_validate_req_t* validate_req,
								  image_validate_resp_t* validate_resp );

		/*
		 *****************************************************************************************
		 * @ Commit the image API to copy data from source to destination
		 * @ param[in] image_commit_req_t: Commit operation required information
		 * @ param[out] image_commit_resp_t : Wait time as response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		fus_op_status_t Commit( image_commit_req_t* commit_req,
								image_commit_resp_t* commit_resp );

		/*
		 *****************************************************************************************
		 * @ Get the current status of the memory controls.
		 * @ param[in] image_status_req_t: requested image
		 * @ param[out] image_status_resp_t : Image information response
		 * @return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		fus_op_status_t Status( image_status_req_t* status_req,
								image_status_resp_t* status_resp );

		/*
		 *****************************************************************************************
		 * @ Reset the all the state, variable after delete the firmware upgrade session
		   @ return fus_op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		fus_op_status_t Exit_FUM( void );

	private:
		Image** m_mem_ctrls;
		comp_config_t* m_comp_config;
		uint8_t m_num_mem_ctrls;
};

}
#endif
