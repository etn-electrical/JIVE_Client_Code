/**
 *****************************************************************************************
 *	@file
 *
 *	@brief This is used to feed the DCI with the appropriate data on the communication loss.
 *
 *	@details Will feed the DCI with the appropriate data on comm loss.  Stores a master list of
 * 				parameters and their behavior on comm loss.  Source ID is used to identify the
 * 				trigger of the fault.
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef COM_DOG_H_
	#define COM_DOG_H_

#include "DCI_Patron.h"
#include "DCI_Owner.h"
#include "Timers_Lib.h"
#include "Linked_List.h"

namespace BF_Misc
{
/*
 **************************************************************************************************
 *  Constants
 **************************************************************************************************
 */
typedef enum COM_DOG_TRIG_TD
{
	COM_DOG_NO_TRIGGER,
	COM_DOG_COMM_LOSS,
	COM_DOG_COMM_IDLE,
	COM_DOG_GOTO_SAFE
} COM_DOG_TRIG_TD;

/*
 **************************************************************************************************
 *  Typedefs
 **************************************************************************************************
 */
// Need a list of Parameters and behavior.
typedef void* COM_DOG_TIMEOUT_CB_PARAM;
typedef void COM_DOG_TIMEOUT_CB( COM_DOG_TIMEOUT_CB_PARAM param,
								 DCI_SOURCE_ID_TD source_responsible, COM_DOG_TRIG_TD trigger );

typedef struct COM_DOG_CB_STRUCT
{
	COM_DOG_TIMEOUT_CB_PARAM cb_param;
	COM_DOG_TIMEOUT_CB* cb;
} COM_DOG_CB_STRUCT;

typedef uint16_t COM_DOG_TIMEOUT_TD;

enum COM_DOG_STATE
{
	COM_DOG_DISABLED_STATE,
	COM_DOG_ENABLED_STATE
};

/*
 **************************************************************************************************
 *  @brief The Com_Dog contains the functionality to feed DCI in case of the communication loss.
 *  @details TODO
 **************************************************************************************************
 */
class Com_Dog
{
	public:
		/**
		 * @details The comm loss timeout dcid links to the parameter in control of the timeout.
		 * If the timeout value is zero - then the timeout activity is disabled.
		 * @param[in] comm_loss_timeout_owner: is the DCID of the timeout.
		 * @param[in] source_id: The source ID provides an indication of whos comm dog is whos.
		 */
		Com_Dog( DCI_Owner* comm_loss_timeout_owner, DCI_SOURCE_ID_TD source_id );

		/**
		 * @details Com_Dog constructor for com ports on external modules.  Used when adding
		 * activity counters
		 */
		Com_Dog( void );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object Com_Dog goes out of scope or deleted.
		 *  @return None
		 */
		virtual ~Com_Dog( void ) {}

		/**
		 * @brief Once created the dog owner must continuously feed the dog.
		 * @details updates the state to the comm dog state enabled.
		 * @return None
		 */
		virtual void Feed_Dog( void );


		/**
		 * @details Indicates that the communication channel has been put into an idle mode.
		 * @return None
		 */
		void Goto_Idle( void );


		/**
		 * @details Used by functions that want to be informed when a timeout has occurred.
		 */
		static void Attach_State_Change_Callback( COM_DOG_TIMEOUT_CB callback,
												  COM_DOG_TIMEOUT_CB_PARAM param );

		/**
		 * @details Forces the comm loss action.
		 */
		static void Trigger_Comm_Loss( DCI_SOURCE_ID_TD source_responsible = DCI_UNKNOWN_SOURCE_ID )
		{ Trigger_Comm_State( COM_DOG_COMM_LOSS, source_responsible ); }

		/**
		 * @details Triggers the new comm state whether it be idle or comm loss.
		 */
		static void Trigger_Comm_State( COM_DOG_TRIG_TD trigger = COM_DOG_COMM_IDLE,
										DCI_SOURCE_ID_TD source_responsible = DCI_UNKNOWN_SOURCE_ID );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		COM_DOG_STATE Dog_Status()
		{
			return  ( m_state );
		}

	private:
		Com_Dog( const Com_Dog& rhs );
		Com_Dog & operator =( const Com_Dog& object );

		static void Timeout_Checker( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );

		static void Handle_Comm_State_Change( COM_DOG_TRIG_TD trigger,
											  DCI_SOURCE_ID_TD source_responsible );

		static void Initialize_Dog_List( void );

		static const COM_DOG_TIMEOUT_TD COM_DOG_MAX_VAL = ~( static_cast<COM_DOG_TIMEOUT_TD>( 0 ) );
		static const uint16_t COM_DOG_TICK_INTERVAL = 11U;			// in ms
		static const uint16_t COM_DOG_DISABLE_TIMEOUT = 0U;
		static const COM_DOG_TIMEOUT_TD COM_DOG_MAX_VAL_CHECK = ( COM_DOG_MAX_VAL - COM_DOG_TICK_INTERVAL );

		static Com_Dog* m_dog_list;
		static BF_Lib::Linked_List* m_cb_list;
		static DCI_SOURCE_ID_TD m_com_dog_source_id;

	protected:
		DCI_Owner* m_timeout_owner;
		DCI_SOURCE_ID_TD m_source_id;
		COM_DOG_TIMEOUT_TD m_timer_value;
		COM_DOG_TIMEOUT_TD m_timeout_value;
		COM_DOG_STATE m_state;
		Com_Dog* m_next_dog;

};

}
#endif	/*COM_DOG_H_*/
