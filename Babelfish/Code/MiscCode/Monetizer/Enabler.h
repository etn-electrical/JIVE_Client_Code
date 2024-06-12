/**
 *****************************************************************************************
 *	@file Enabler.h
 *
 *	@brief File contains Enabler abstract class for handling locking and unlocking the DCID set or Application.
 *  @li In case of DCID set, the set of dcid can be locked or unlocked for reading/writing/both.
 *  @li These set of DCIDs can be one which will be unblocked with the Licenser Module enables
 *  @li the set at the event of customer makes the payment through Revenera or some other payment
    @li gateway. Same is applicable for App enablement.
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef ENABLER_H
	#define ENABLER_H
#include "Param_Lock.h"

/**
 **************************************************************************************************
 *  @brief Enabler class is abstract class for handling locking and unlocking the DCID set or Application.
 *  @details TODO
 **************************************************************************************************
 */


class Enabler
{
	public:
		/**
		 * @brief Feature ID
		 */
		typedef uint16_t fid_t;
		/**
		 * @brief Return status: True - Success. False - Fail.
		 */
		typedef bool ret_status_t;

		/**
		 *  @brief Destructor
		 *  @return None
		 */
		virtual ~Enabler( void )
		{}

		/**
		 *  @brief Locks the DCI set of App or complete App for a particular fid.
		 *  @param[in] command: Command to read lock, write lock or both lock.
		 *  @return ret_status_t: Return success or failure. True - SUCCESS. False - FAIL.
		 */
		virtual ret_status_t Lock( void ) = 0;

		/**
		 *  @brief Unlocks the DCI set of App or complete App for a particular fid.
		 *  @param[in] command: Command to read lock, write lock or both lock.
		 *  @return ret_status_t: Return success or failure. True - SUCCESS. False - FAIL.
		 */
		virtual ret_status_t Unlock( void ) = 0;

	protected:
		/**
		 * @brief A protected constructor to ensure the class is only instantiated via specialized
		 * classes.
		 */
		Enabler()
		{
			m_param_lock_module_handle = nullptr;
		}

		/**
		 * @brief Handle for param lock
		 */
		Param_Lock* m_param_lock_module_handle;

	private:
		Enabler( const Enabler& object );		///< Private copy constructor
		Enabler & operator =( const Enabler& object );		///< Private copy assignment operator

};

#endif
