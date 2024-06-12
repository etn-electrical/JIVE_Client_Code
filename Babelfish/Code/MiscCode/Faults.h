/**
 *****************************************************************************************
 *	@file Faults.h
 *	@Creator Mark A Verheyen
 *
 *	@brief This handles register and message specific behavior.
 *
 *	@details General Requirements:
 * 			- Needs active faults.
 * 			- Needs active warnings.
 * 			- Needs order of incidence queue.
 * 			- Needs sorting queue.
 * 			- Needs active fault.
 * 			- Needs active warning.
 * 			- Needs active fault/warning.
 *
 * 			The following NV Addresses need to be defined in the NV Space.  Their associated length
 * 			calculation is indicated.
 * 			FAULTS_SORTED_QUE_NVADD: len >= ( FAULTS_SORTED_QUEUE_LEN * sizeof(FAULTS_ID_TD) )
 * 			FAULTS_EVENT_ORDERED_QUE_NVADD: len >= ( FAULTS_EVENT_ORDER_QUEUE_LEN * sizeof(FAULTS_ID_TD) )
 * 			FAULTS_LATCHED_BITS_NVADD: len >= ( FAULTS_LIST_TOTAL_INDS / 8 )
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef FAULTS_H
#define FAULTS_H

#include "CR_Tasker.h"
#include "DCI_Owner.h"
#include "System_Reset.h"
#include "Bit_List.h"
#include "Fault_Data.h"
#include "Linked_List.h"

namespace BF_Misc
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define FAULTS_STATUS_FAULTED_BIT       0U
#define FAULTS_STATUS_WARNED_BIT        1U

#define FAULTS_SORTED_QUEUE_LEN         3U		///< This is the number of sorted events allowed in the table.
#define FAULTS_EVENT_ORDER_QUEUE_LEN    3U		///< This is the number of events stored in event order.
/*
 *****************************************************************************************
 *		More Constants
 *****************************************************************************************
 */
#if 0
#define FAULTS_Word_Select( a )         ( ( a ) >> ( ( sizeof( FAULTS_BIT_TD ) / 2 ) + 3 ) )
#define FAULTS_Bit_Mask( a )            ( 1 << ( ( a ) & ( ( sizeof( FAULTS_BIT_TD ) * 8 ) - 1 ) ) )

#define FAULTS_TOTAL_BIT_WORDS          ( FAULTS_Word_Select( FAULTS_LIST_TOTAL_INDS - 1 ) + 1 )
#endif

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
typedef void* STATE_CHANGE_CBACK_PARAM;
typedef void STATE_CHANGE_CBACK( STATE_CHANGE_CBACK_PARAM param );

/**
 ****************************************************************************************
 * @brief Faults is a static only module. This allows pretty much global interface.
 *
 * @details  This module requires some NV memory allocated for it as well as some DCI parameters.
 * - DCI_BF_FAULT_QUEUE_SORTED_DCID
 * - DCI_BF_FAULT_QUEUE_EVENT_ORDER_DCID
 * - DCI_BF_FAULT_STATUS_BITS_DCID
 * - FAULTS_SORTED_QUE_NVADD
 * - FAULTS_EVENT_ORDERED_QUE_NVADD
 *
 ****************************************************************************************
 */
class Faults
{

	public:

		/**
		 *  @brief Constructor
		 *  @details Mainly does initialization of members, creates the status bits array along with queues, reads the
		 * faults from NV memory,
		 *  initializer CR Tasker with Fault Handler task. Then we have an initialization fault.
		 *  @param[in] sorted_que_dcid: It is passed in the DCI_Owner, to create the space associated with this id and
		 * assigns the start address to appropriate member. TODO (more to add)
		 *  @param[in] event_order_dcid: It is passed in the DCI_Owner, to create the space associated with this id and
		 * assigns the start address to appropriate member. TODO
		 *  @param[in] status_bits_dcid: It is passed in the DCI_Owner, to create the space associated with this id and
		 * assigns the start address to appropriate member. TODO
		 */
		Faults( DCI_ID_TD sorted_que_dcid,
				DCI_ID_TD event_order_dcid, DCI_ID_TD status_bits_dcid );

		/**
		 *  @brief Destructor for the Faults Module.
		 *  @details This will get called when object of Faults goes out of scope.
		 */
		~Faults( void )
		{}

		/**
		 *  @brief If the fault set queue is empty, it sets the initialization fault with the passed in fault id.
		 *  @param[in] fault_id: The fault id corresponding to a particular fault of an internal application, e.g. could
		 * be
		 *  a communication related fault id in case of a running communication.
		 *  @return None
		 */
		static void Set( FAULTS_IND_TD fault_id );

		/**
		 *  @brief It calls to set the fault with the passed in fault id if the clear fault queue is not pointing to
		 * null.
		 *  @param[in] fault_id: The fault id corresponding to a particular fault of an internal application.
		 *  @return None
		 */
		static void Clr( FAULTS_IND_TD fault_id );

		/**
		 *  @brief Return the pointer to the ram static heap array created.
		 *  @param[in] fault_id: The fault id corresponding to a particular fault of an internal application.
		 *  @return
		 */
		static bool Test( FAULTS_IND_TD fault_id );

		/**
		 *  @brief This sets the bit FAULTS_RESET_FAULT_REQ_BIT.
		 *  @return None
		 */
		static void Reset( void );

		/**
		 *  @brief This sets the bit FAULTS_RESET_FLUSH_REQ_BIT.
		 *  @return None
		 */
		static void Flush_Lists( void );

		/**
		 *  @brief Gives the status of the fault counter.
		 *  @return  true if the fault counter value is non zero.
		 */
		static bool Faulted( void );

		/**
		 *  @brief Gives the status of the warning counter.
		 *  @return true is returned if warning counter is non zero.
		 */
		static bool Warned( void );

		/**
		 *  @brief Initializes a new linked list if the call back list pointer is null, new node to linked list is added
		 * with the new block being allocated.
		 *  @param[in] cback: The callback to be assigned to the new Ram block that shall be created.
		 *  @param[in] param: The callback param to be assigned to the new Ram block that shall be created.
		 *  @return None
		 */
		static void Attach_State_Change_Cback( STATE_CHANGE_CBACK* cback,
											   STATE_CHANGE_CBACK_PARAM param );

		static void FAULTS_Set_Bit( FAULTS_BIT_TD* bit_array, FAULTS_IND_TD fault );

		static void FAULTS_Clr_Bit( FAULTS_BIT_TD* bit_array, FAULTS_IND_TD fault );

		static bool FAULTS_Test_Bit( FAULTS_BIT_TD* bit_array, FAULTS_IND_TD fault );

	private:
		typedef struct FAULTS_STATE_CHANGE_CBACK_ST
		{
			STATE_CHANGE_CBACK* cback;
			STATE_CHANGE_CBACK_PARAM param;
		} FAULTS_STATE_CHANGE_CBACK_ST;

		/// There is a distinction beteween IND and ID.  IND is the value you pass into the Set_Fault/Clr_Fault.
		/// IND is the internal representation.  Internal to the application.  ID is the external user
		/// fault code.  The ID is the ID presented to the user identifying the fault or warning.
		/// the BIT is the bitfield which is used to find the faults.
		typedef uint8_t FAULTS_IND_TD;	///< This is what is passed into the Fault::Set()/Clr()/Test()
		typedef uint16_t FAULTS_ID_TD;
		typedef uint8_t FAULTS_BIT_TD;

		Faults( const Faults& rhs );
		Faults & operator =( const Faults& object );

		static void System_Handler( CR_Tasker* cr_task, CR_TASKER_PARAM param );

		static void Add_New_Fault( FAULTS_IND_TD fault );

		static void Add_New_ID_To_Sorted_Que( FAULTS_IND_TD new_fault );

		static void Add_New_ID_To_Event_Order_Que( FAULTS_IND_TD new_fault );

		static void Remove_Fault( FAULTS_IND_TD fault );

		static void Store_Lists( void );

		static void Get_Faults_From_NV( void );

		static void Process_Flush_Lists( void );

		static void Update_Status_Bits( void );

		static bool Reset_Handler( BF::System_Reset::cback_param_t param,
								   BF::System_Reset::reset_select_t reset_req );

		static DCI_Owner* m_sorted_que_owner;
		static DCI_Owner* m_event_que_owner;
		static DCI_Owner* m_status_bits;

		static FAULTS_BIT_TD m_latched[FAULTS_TOTAL_BIT_WORDS];
		static FAULTS_BIT_TD m_active[FAULTS_TOTAL_BIT_WORDS];
		static BF_Lib::Bit_List* m_set_que;
		static BF_Lib::Bit_List* m_clr_que;

		static FAULTS_ID_TD m_sorted_que[FAULTS_SORTED_QUEUE_LEN];
		static FAULTS_ID_TD m_event_order_que[FAULTS_EVENT_ORDER_QUEUE_LEN];
		static FAULTS_IND_TD m_nv_sorted_que[FAULTS_SORTED_QUEUE_LEN];
		static FAULTS_IND_TD m_nv_event_order_que[FAULTS_EVENT_ORDER_QUEUE_LEN];

		static uint8_t m_reset_request;

		static uint8_t m_latched_fault_ctr;
		static uint8_t m_fault_ctr;
		static uint8_t m_warn_ctr;

		static uint8_t m_initialization_fault;	///< This indicates that something went wrong during init before Faults
												///< was fired up.

		static BF_Lib::Linked_List* m_cback_list;

};

}
#endif
