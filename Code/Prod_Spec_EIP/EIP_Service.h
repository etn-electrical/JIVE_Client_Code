/**
 *****************************************************************************************
 * @file       EIP_Services.h
 * @details    EIP services specific declarations and definitions.
 * @copyright  2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef EIP_SERVICE_H
#define EIP_SERVICE_H

#include "EtherNetIP_DCI.h"
#include "Change_Tracker.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "eipinc.h"
#ifdef __cplusplus
}
#endif

#include "EIP_Display.h"
#include "OS_Binary_Semaphore.h"
#include "OS_Mutex.h"
#include "CIP_Asm.h"
#include "Faults.h"
#include "eipid.h"
#include "Shadow_Product.h"

/*
 ********************************************************************************
 *		Variables
 ********************************************************************************
 */
// The following is exposed for the use of the stack.
extern OS_Mutex* m_EipApiMutex;


/*
 ********************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------
 */
class EIP_Service
{
	public:
		/**
		 **********************************************************************************************
		 * @brief                     Constructor to create instance of EIP_Service
		 * @param[in] eip_target_info EIP target info pointer
		 * @param[in] eip_display     EIP_display instance
		 * @param[in] product_shadow  Shadow_Product instance
		 * @return[out]
		 * @n
		 **********************************************************************************************
		 */
		EIP_Service( EIP_TARGET_INFO_ST_TD const* eip_target_info,
					 EIP_Display* eip_display, Shadow_Product* product_shadow );
		/**
		 **********************************************************************************************
		 * @brief                    Destructor to delete instance of EIP_Service
		 * @param[in] none           none
		 * @return[out] none         none
		 * @n
		 **********************************************************************************************
		 */
		~EIP_Service( void )
		{
			delete mEFT_test_owner;
		}

		/**
		 **********************************************************************************************
		 * @brief                    Start EIP Stack by releasing bin semaphore
		 * @param[in] none           none
		 * @return[out] none         none
		 * @n
		 **********************************************************************************************
		 */
		static void Start_EIP_Stack( void );

		/**
		 **********************************************************************************************
		 * @brief                    Restart EIP stack
		 * @param[in] none           none
		 * @return[out] none         none
		 * @n
		 **********************************************************************************************
		 */
		static void Restart_EIP_Stack( void ) { m_restart_eip_stack = TRUE; }


		/**
		 **********************************************************************************************
		 * @brief                    Loads EIP Identity as per given attribute
		 * @param[in] attribute      attribute id
		 * @return[out] none         none
		 * @n
		 **********************************************************************************************
		 */
		static void Load_EIP_Identity_Attribute( CIP_ATTRIB_TD attribute );

		/**
		 **********************************************************************************************
		 * @brief                    Get CIP asm instance
		 * @param[in]                none
		 * @return[out] m_cip_asm    returns CIP_Asm instance
		 * @n
		 **********************************************************************************************
		 */
		static CIP_Asm* GetCIP_Asm_Object() { return ( m_cip_asm ); }

		/**
		 * @brief parameter type for ACD call back
		 */
		// typedef void EIP_SVC_CALLBACK_PARAM;

		/**
		 * @brief ACD call back function pointer
		 */
		typedef void (* EIP_SVC_CALLBACK)( void );

		/**
		 **********************************************************************************************
		 * @brief                    Set active message count callback
		 * @param[in] pCallback      pCallback to be executed
		 * @return[out] none         none
		 * @n
		 **********************************************************************************************
		 */
		static void Set_Active_Msg_Count_Callback( EIP_SVC_CALLBACK pCallback )
		{
			m_active_msg_count_callback = pCallback;
		}

	private:
		EIP_Service ( const EIP_Service& object );
		EIP_Service & operator =( const EIP_Service& object );

		/**
		 **********************************************************************************************
		 * @brief                    EIP task
		 * @param[in] handle         void pointer, not used in the function
		 * @return[out] none         none
		 * @n
		 **********************************************************************************************
		 */
		static void EIP_Stack_Task( OS_TASK_PARAM handle );

		/**
		 **********************************************************************************************
		 * @brief                    Executes EIP event callback
		 * @param[in] event          event name
		 * @param[in] param1         param1 of the event
		 * @param[in] param2         param2 of the event
		 * @return[out] none         none
		 * @n
		 **********************************************************************************************
		 */
		static void Event_Callback( INT32 event, INT32 param1, INT32 param2 );

		/**
		 **********************************************************************************************
		 * @brief                    Get the request from the Ethernet/IP library, process the request
		 *                           and send the response
		 * @param[in] request_id     Request ID
		 * @return[out] none         none
		 * @n
		 **********************************************************************************************
		 */
		static void Process_Object_Request( int32_t request_id );

		/**
		 **********************************************************************************************
		 * @brief                   Processes assembly
		 * @param[in] instance      instance
		 * @return[out] none        none
		 * @note                    Function definition is not implemented
		 **********************************************************************************************
		 */
		static void Process_Assembly( INT32 instance );

		/**
		 **********************************************************************************************
		 * @brief                   Get the response for a UCMM or an Unconnected Send message
		 *                          as per received request for class, instance and attribute id's
		 * @param[in] request       Pointer to transfer UCMM message structure
		 * @param[in] response      Pointer to get the response for a UCMM message structure
		 * @return[out] bool        returns true
		 * @n
		 **********************************************************************************************
		 */
		static bool_t Get_Class_Attr_Single( EtIPObjectRequest* request,
											 EtIPObjectResponse* response );

		/**
		 **********************************************************************************************
		 * @brief                   Set the response for a UCMM or an Unconnected Send message
		 *                          as per received request for class, instance and attribute id's
		 * @param[in] request       Pointer to transfer UCMM message structure
		 * @param[in] response      Pointer to get the response for a UCMM message structure
		 * @return[out] bool        returns true
		 * @n
		 **********************************************************************************************
		 */
		static bool_t Set_Class_Attr_Single( EtIPObjectRequest* request,
											 EtIPObjectResponse* response );

		/**
		 **********************************************************************************************
		 * @brief                   LED state change as per new status
		 * @param[in] new_status    new_status of the led
		 * @return[out] none        none
		 * @n
		 **********************************************************************************************
		 */
		static void Net_Status_LED_State_Change( INT32 new_status );

		/**
		 **********************************************************************************************
		 * @brief                    Initialize stack hooks
		 * @param[in] none           none
		 * @return[out] none         none
		 * @n
		 **********************************************************************************************
		 */
		static void Initialize_Stack_Hooks( void );

		/**
		 **********************************************************************************************
		 * @brief                    Load and initialize EIP assembly stack
		 * @param[in] none           none
		 * @return[out] none         none
		 * @n
		 **********************************************************************************************
		 */
		static void Load_EIP_Assembly_Stack_Hook( void );

		/**
		 **********************************************************************************************
		 * @brief                    Load EIP class stack and register event callback
		 * @param[in] none           none
		 * @return[out] none         none
		 * @n
		 **********************************************************************************************
		 */
		static void Load_EIP_Class_Stack_Hook( void );

		/**
		 **********************************************************************************************
		 * @brief                    Load EIP Identity data
		 * @param[in] none           none
		 * @return[out] none         none
		 * @n
		 **********************************************************************************************
		 */
		static void Load_EIP_Identity( void );

		/**
		 **********************************************************************************************
		 * @brief                     Executes reset service as per request
		 * @param[in] reset_requested Indicates type of reset required
		 * @return[out] none          none
		 * @n
		 **********************************************************************************************
		 */
		static void Ident_Reset_Svc( INT32 reset_requested );

		/**
		 **********************************************************************************************
		 * @brief                     Handle communication fault and change EIP Display state
		 * @param[in] param           unused
		 * @return[out] none          none
		 * @n
		 **********************************************************************************************
		 */
		static void Handle_Com_Fault_State_Change(
			BF_Misc::STATE_CHANGE_CBACK_PARAM param );

		static CIP_ASM_STRUCT_TD const* Find_ASM_From_DCID( DCI_ID_TD dci_id );

		static CIP_ASM_STRUCT_TD const* m_end_asm_struct;
		static void Verify_Connection( INT32 iConnectionInstance );

		static bool_t Flag_EFT_Test_Code;

		static bool_t m_restart_eip_stack;
		static EtherNetIP_DCI* m_eip_dci;
		static EtIPObjectRequest m_request;
		static EtIPObjectResponse m_response;
		static EIP_TARGET_INFO_ST_TD const* m_eip_target_info;
		static EIP_Display* m_eip_display;
		static Change_Tracker* m_change_tracker;
		static OS_Binary_Semaphore* m_BinSem_Lwip_Started;
		static CIP_Asm* m_cip_asm;
		static DCI_Owner* meip_restart_test_code;
		static Shadow_Product* m_shadow_product;
		static EIP_SVC_CALLBACK m_active_msg_count_callback;

		DCI_Owner* mEFT_test_owner;

		// For ESP32 less than 2048 stack size we are getting stack overflow
		static const uint16_t EIP_OS_STACK_SIZE = 2048U;

		static const uint16_t EIP_SERVICE_ASSEMBLY_CLASS_ID = 4U;
		static const uint16_t EIP_SERVICE_IDENTITY_CLASS_ID = 1U;

		// static const uint16_t EIP_SVC_ASSEMBLY_DATA_ATTRIBUTE=	3U;
		// static const uint16_t EIP_SVC_ASSEMBLY_CLASS		=	4U;
		static const uint16_t EIP_CLASS_1_MIN_RPI = 5U;
		static const uint16_t EIP_CLASS_3_MIN_RPI = 50U;

		static const CIP_CLASS_TD CIP_IDENTITY_CLASS = 1U;
		static const CIP_INSTANCE_TD CIP_IDENTITY_INSTANCE = 1U;
		static const CIP_ATTRIB_TD CIP_IDENTITY_END_ATTRIBUTE = 7U;	// ID_CLASS_ATTR_MAX_INST_ATTR
};

#endif
