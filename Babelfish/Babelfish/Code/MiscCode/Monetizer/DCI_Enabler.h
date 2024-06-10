/**
 *****************************************************************************************
 *	@file DCI_Enabler.h
 *
 *	@brief File contains DCI_Enabler class for handling locking and unlocking the DCID set.
 *  @li In case of DCID set, the set of dcid can be locked or unlocked for reading/writing/both.
 *  @li These set of DCIDs can be one which will be unblocked with the Licenser Module enables
 *  @li the set at the event of customer makes the payment through Revenera or some other payment
    @li gateway. Same is applicable for App enablement.
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DCI_ENABLER_H
	#define DCI_ENABLER_H
#include "Enabler.h"

/**
 **************************************************************************************************
 *  @brief DCI_Enabler class is responsible for handling locking and unlocking the DCID set of the application.
 **************************************************************************************************
 */
class DCI_Enabler : public Enabler
{
	public:
		/**
		 *  @brief Constructor for the purely virtual class.
		 *  @n @b DCI_Enabler( Param_Lock* param_lock_module_handle );
		 *  @param[in] param_lock_module_handle: Handle of the Param Lock class which as the the list of DCIDs.
		 * @param[in] lock_type :Lock type for which the module is being locked or unlocked (read , write or both).
		 */
		DCI_Enabler( Param_Lock* param_lock_module_handle, DCI_LOCK_TYPE_TD lock_type = BF::DCI_Lock::LOCK_ALL_MASK );

		/**
		 *  @brief Destructor
		 *  @return None
		 */
		~DCI_Enabler( void );

		/**
		 *  @brief Locks the DCI set of App or complete App for a particular fid.
		 *  @param[in] : none
		 *  @return ret_status_t: Return success or failure. True - SUCCESS. False - FAIL.
		 */
		ret_status_t Lock( void );

		/**
		 *  @brief Unlocks the DCI set of App or complete App for a particular fid.
		 *  @param[in] : none
		 *  @return ret_status_t: Return success or failure. True - SUCCESS. False - FAIL.
		 */
		ret_status_t Unlock( void );

	private:
		DCI_Enabler( const DCI_Enabler& object );		///< Private copy constructor
		DCI_Enabler & operator =( const DCI_Enabler& object );		///< Private copy assignment operator

		/**
		 * @brief module specific lock type
		 */
		DCI_LOCK_TYPE_TD m_lock_type;


};

#endif

