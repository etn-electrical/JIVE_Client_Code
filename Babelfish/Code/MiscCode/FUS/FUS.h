/**
 *****************************************************************************************
 *  @file
 *	@brief Firmware upgrade service header file.
 *
 *	@details FUS contains all the necessary structures and methods to run firmware upgrade service.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef FUS_H
#define FUS_H

#include "FUS_Msg_Structs.h"
#include "Component_Ctrl.h"
#include "Timers_Lib.h"
#include "CR_Tasker.h"
#include "FUS_Config.h"
#include "Proc_MEM_ID_Codes.h"
#include "FUS_Debug.h"

namespace BF_FUS
{

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
/* multiplier to convert sec in to mili second*/
static const uint16_t SEC_TO_MS_MULTIPLIER = 1000U;
/*
 *****************************************************************************************
 *		Typedefs
 *****************************************************************************************
 */
typedef struct
{
	uint8_t count;
#if ( MAX_IMAGES_PER_COMPONENT <= 8U )
	uint8_t data;
	uint8_t checkbox;
#elif ( MAX_IMAGES_PER_COMPONENT <= 16U )
	uint16_t data;
	uint16_t checkbox;
#else
#error "FUS: Please check MAX_IMAGES_PER_COMPONENT, can it go more than 16 for any single component?"
#endif
} image_data_t;

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class FUS
{
	public:
		/**
		 * Definitions:
		 * FUM = Firmware Upgrade Mode
		 * FUS = Firmware Upgrade Service
		 * FUP = Firmware Upgrade Package
		 * FUPT = Firmware Upgrade Package Tool
		 * FFUP = Factory Image Firmware Upgrade Package
		 */
		/**
		 * @brief FUS state of the different operation handled by FUS
		 */
		enum fus_state_t
		{
			FUS_IDLE,
			FUS_INIT_READ,
			FUS_INIT,
			FUS_SESSION,
		};

		/**
		 * @brief Constructor
		 * @details This does basic initialization of the data members,
		 * configuration members initialized appropriately.
		 * @param[in] component_list:Total Component list
		 * @param[in] component_cnt: Total component count.
		 * @param[in] fus_config: Product information.
		 */
		FUS( Component_Ctrl** component_list, uint8_t component_cnt,
			 prod_config_t* fus_config, event_cb_t FUS_Event_Cb );
		~FUS( void );

		// Product specific interface
		/*
		 *****************************************************************************************
		 * @ Get the Static FUS handle .
		 * @return FUS_HANDLE.
		 *****************************************************************************************
		 */
		static FUS* Get_FUS_Handle_Static( void );

		/*
		 *****************************************************************************************
		 * @ Get the Product related information.
		 * @ param[out] prod_info_t : Product related information of requested operation.
		 * @return op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		op_status_t Product_Info( prod_info_t* prod_info );

		/*
		 *****************************************************************************************
		 * @ Get the Component related information.
		 * @param[in] comp_info_req: requested component
		 * @ param[out] comp_info_resp : component information response
		 * @return op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		op_status_t Component( comp_info_req_t* comp_req,
							   comp_info_resp_t* comp_resp );

		/*
		 *****************************************************************************************
		 * @ Get the Image information.
		 * @param[in] image_info_req: requested Image
		 * @ param[out] session_info_resp_t : Image information response
		 * @return op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		op_status_t Image( image_info_req_t* image_req, image_info_resp_t* image_resp );

		/*
		 *****************************************************************************************
		 * @ Create the FUS session for the firmware upgrade.
		 * @ param[in] session_info_req_t: firmware info
		 * @ param[out] session_info_resp_t : Image information response
		 * @return op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		op_status_t Goto_FUM( session_info_req_t* session_info,
							  session_info_resp_t* session_resp );

		/*
		 *****************************************************************************************
		 * @ Write the data into the image memory space.
		 * @param[in] image_write_req_t: Writing the data info
		 * @ param[out] image_write_resp_t : Wait time as response
		 * @return op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		op_status_t Write( image_write_req_t* write_req, image_write_resp_t* write_resp );

		/*
		 *****************************************************************************************
		 * @ Validate the Written image in the memory space
		 * @param[in] validate_req: Validating image info
		 * @ param[out] image_validate_resp_t : Wait time and status as response
		 * @return op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		op_status_t Validate( image_validate_req_t* validate_req,
							  image_validate_resp_t* validate_resp );

		/*
		 *****************************************************************************************
		 * @ Commit the image API to copy data from source to destination
		 * @ param[in] image_commit_req_t: Commit operation required information
		 * @ param[out] image_commit_resp_t : Wait time as response
		 * @return op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		op_status_t Commit( image_commit_req_t* commit_req,
							image_commit_resp_t* commit_resp );

		/*
		 *****************************************************************************************
		 * @ Get the current status of the memory controls.
		 * @ param[in] image_status_req_t: requested image
		 * @ param[out] image_status_resp_t : Image information response
		 * @return op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		op_status_t Status( image_status_req_t* status_req,
							image_status_resp_t* status_resp );

		/*
		 *****************************************************************************************
		 * TODO
		 *****************************************************************************************
		 */
		op_status_t Read( image_read_req_t* read_mem, image_read_resp_t* read_mem_resp );

		/*
		 *****************************************************************************************
		 * @ Delete the FUS session for the firmware upgrade.
		 * @param[in] session_info_req_t: Session delete required information
		 * @ param[out] session_info_resp_t : Wait time as response
		 * @return op_status_t : operation status of the function
		 *****************************************************************************************
		 */
		op_status_t Exit_FUM( session_info_req_t* session_info,
							  session_info_resp_t* session_resp );

		/*
		 *****************************************************************************************
		 * @ Checking for the firmware upgrade session is active or not
		 * @ return : session active Flag
		 *****************************************************************************************
		 */
		bool Is_Session_Alive( void );

		/*
		 *****************************************************************************************
		 * @ Delete the session created for the FUS upgrade
		 * @return : None
		 *****************************************************************************************
		 */
		void Delete_Session( void );

		/*
		 *****************************************************************************************
		 * @ Get the active session id
		 * @return : session id
		 *****************************************************************************************
		 */
		uint32_t Get_Session_Id( void );

		/*
		 *****************************************************************************************
		 * @ Get the number of components in the product (processors)
		 * @return : component count
		 *****************************************************************************************
		 */
		uint32_t Get_Component_Count( void );

		/*
		 *****************************************************************************************
		 * @brief   Whenever a FUS interface for example REST, IOT, BLE etc create FUS session,
		 * on successful session creation, corresponding interface is provided ownership by
		 * setting fus interface. This function then returns the interface which has occupied the
		 * fus currently.
		 * @param none
		 * @return Selected interface for FUS
		 *****************************************************************************************
		 */
		fus_if_t Get_Interface( void );

	private:
		Component_Ctrl** m_comp_ctrls;
		prod_config_t* m_prod_config;
		static FUS* m_fus_handle;
		event_cb_t m_fus_event_cb;
		uint32_t m_session_id;
		uint32_t m_session_logout_time;
		uint32_t m_wait_time;
		uint32_t m_time_out;
		uint8_t m_num_comp_ctrls;
		image_data_t m_image_data[TOTAL_COMPONENTS];
		fus_state_t m_fus_state;
		bool m_init_complete;
		uint8_t m_comp_sel;
		uint8_t m_image_sel;
		fus_if_t m_interface_sel;
		bool m_write_first_chunk;

		/*
		 *****************************************************************************************
		 * @ CR tasker for the session handler
		 * @param[in] param: A  pointer to an object passed as an argument CR tasker handler .
		 * @return : None
		 *****************************************************************************************
		 */
		void Session_Handler_Task( CR_Tasker* cr_task );

		/*
		 *****************************************************************************************
		 * @ Static CR tasker for the session handler
		 * @param[in] param: A  pointer to an object passed as an argument CR tasker handler .
		 * @param[in] handle: Parameters passed for task creation.
		 * @return : None
		 *****************************************************************************************
		 */
		static void Session_Handler_Static( CR_Tasker* cr_task, CR_TASKER_PARAM handle )
		{
			reinterpret_cast<FUS*>( handle )->Session_Handler_Task( cr_task );
		}

		/*
		 *****************************************************************************************
		 * @ generate the 5 digit random number
		 * @ return : None
		 *****************************************************************************************
		 */
		void Generate_Session_Id( void );

		/*
		 *****************************************************************************************
		 * @ Returns status of boot up validation
		 * @ return : True : Boot up validation complete
		 *			: False: Boot up validation is still in process
		 *****************************************************************************************
		 */
		bool Is_Init_Complete( void );

		/*
		 *****************************************************************************************
		 * @ Handle the multiple operation through CR tasker
		 * @ return : None
		 *****************************************************************************************
		 */
		void Fus_Operation_Handler( void );

		/*
		 *****************************************************************************************
		 * @ Initialize the bootup validation of the images
		 * @ return : None
		 *****************************************************************************************
		 */
		void Init_Read_Handler( void );

		/*
		 *****************************************************************************************
		 * @ Execute the bootup validation of the images
		 * @ return : None
		 *****************************************************************************************
		 */
		void Init_Handler( void );

		/*
		 *****************************************************************************************
		 * @ Session timeout functionality handled
		 * @ return : None
		 *****************************************************************************************
		 */
		void Session_Handler( void );

		/*
		 *****************************************************************************************
		 * @brief   This function sets the interface for FUS.
		 * @details The interface has to be selected before FUS session creation.
		 * For e.g IOT_FUS will be selected when firmware upgrade is requested over IOT.
		 * @param selected_interface: Selected interface for FUS
		 * @return none
		 *****************************************************************************************
		 */
		void Set_Interface( fus_if_t selected_interface );

		/*
		 *****************************************************************************************
		 * @brief   This function clears the selected interface for FUS.
		 * @details This function clears the selected interface once FUS session is deleted
		 * successfully. It sets the interface to default NO_FUS.
		 * @return none
		 *****************************************************************************************
		 */
		void Clear_Interface( void );

};

}

#endif
