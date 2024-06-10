/**
 *****************************************************************************************
 *  @file

 *	@brief Web image control.
 *
 *	@details This class will provide the commit operation from one memory to another.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef COMMIT_COPY_H
#define COMMIT_COPY_H

#include "includes.h"
#include "Commit_Ctrl.h"
#include "FUS_Defines.h"
#include "NV_Ctrl.h"
#include "Image.h"
#include "Timers_Lib.h"
#include "FUS_Config.h"
#include "Fw_Store.h"
#include "FUS_Debug.h"

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
class Commit_Copy : public Commit_Ctrl
{
	public:

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members,
		 * configuration members initialized appropriately.
		 * @param[in] reset_req: flag required for reset operation after commit done
		 */
		Commit_Copy( void );

		~Commit_Copy( void ) {}

		/*
		 *****************************************************************************************
		 * @ Handle the Commit operation of the image
		 * @ param[in] process_time : Operation execution time
		 * @ return fus_op_status_t : operation status of the function.
		 *****************************************************************************************
		 */
		void Image_Handler( fw_state_op_t* commit );

		/*
		 *****************************************************************************************
		 * @ Reset the commit controls flags and variables values
		 * @ return : None
		 *****************************************************************************************
		 */
		void Reset( void );

		/*
		 *****************************************************************************************
		 * @ Call back function for the NV Ctrls operation status
		 * @param[in] status: NV Ctrl status
		 * @return : None
		 *****************************************************************************************
		 */
		void Commit_Status_Cb( NV_Ctrl::nv_status_t status );

	private:
		fw_state_op_t* m_commit;
		image_state_t m_state;
		/*
		 *****************************************************************************************
		 * @ handle the commit operation of the image control
		 * @ return : none
		 *****************************************************************************************
		 */
		void Commit_State_Handler( void );

		/*
		 *****************************************************************************************
		 * @ handle the retry for the NV operation of the image control
		 * @ return : none
		 *****************************************************************************************
		 */
		void Retry_Image_Handler( void );

		/*
		 *****************************************************************************************
		 * @ Handle the read header data for the commit operation
		 * @ return : none
		 *****************************************************************************************
		 */
		void Read_Header_Image_Handler( void );

		/*
		 *****************************************************************************************
		 * @ Handle the read data for the commit  operation
		 * @ return : none
		 *****************************************************************************************
		 */
		void Read_Data_Image_Handler( void );

		/*
		 *****************************************************************************************
		 * @ Handle the write data for the commit operation
		 * @ return : none
		 *****************************************************************************************
		 */
		void Write_Data_Image_Handler( void );

		/*
		 *****************************************************************************************
		 * @ Handle the header extraction for the commit operation
		 * @ return : none
		 *****************************************************************************************
		 */
		void Extract_Header_Image_Handler( void );

		/*
		 *****************************************************************************************
		 * @ Handle the erase real NV memory space for the commit operation
		 * @ return : none
		 *****************************************************************************************
		 */
		void Erase_Real_Nv_Image_Handler( void );

};

}

#endif
