/**
 *****************************************************************************************
 * @file				uc_watchdog header file for the watchdog functionality
 *
 * @brief				This file shall wrap all functions required for watchdog software functionality.
 *
 * @details				The watchdog timer serves to detect and resolve malfunctions due to software
 *						failure and to trigger system reset or an interrupt when the counter reaches
 *						a given timeout value.
 *
 * @copyright			2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_WATCHDOG_H
   #define uC_WATCHDOG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "uC_Base.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint32_t ENABLE_VALUE = 1U;				///< Enable value.

/*
 *****************************************************************************************
 *		Typedefs
 *****************************************************************************************
 */
typedef uint_fast8_t WATCHDOG_BONE_ID_TYPE;			///< Typedef for watchdog bone id type.

/*
 **************************************************************************************************
 * @brief			uc_Watchdog class declaration
 * @details			This class includes all the functions which will help in enabling the watchdog timer.
 **************************************************************************************************
 */
class uC_Watchdog
{
	public:
		/**
		 * @brief							Constructor to create an instance of uc_watchdog class.
		 * @details							This constructor is used to assign the value returned by Get_ bone()
		 * @n								to the variable m_bone.
		 * @return void						none
		 */
		uC_Watchdog( void );

		/**
		 * @brief							Destructor to delete the uC_Watchdog instance when it goes out of scope
		 * @return void						none
		 */
		~uC_Watchdog( void );

		/**
		 * @brief							This function is used to initialize the timeout value.
		 * @param[in] timeout_in_ms:		input the timeout in ms e.g 1000.
		 * @return void						none
		 */
		static void Init( uint16_t timeout_in_ms );

		/**
		 * @brief							This function is used to assign the reload value.
		 * @return void						none
		 */
		static void Force_Feed_Dog( void );

		/**
		 * @brief							This function is used to test the reset condition.
		 * @param[in] param0				Pass NULL value to test reset condition.
		 * @return void						none
		 */
		static void Handler( void* param0 = NULL );

		/**
		 * @brief							Set bit that bone_ID reported.
		 * @return void						none
		 */
		inline void Feed_Dog( void ) { m_dog_bones_returned_mask |= ( ( ENABLE_VALUE ) << m_bone ); }

		/**
		 * @brief							This function is used to set bits of the variable m_bones_thrown_mask
		 * @n								by left shifting m_bone by 1.
		 * @details							This is used to set bits of the variable m_dog_bones_returned_mask
		 * @n								by left shifting m_bone.
		 * @return void						none
		 */
		void Enable_Bone( void ) const;

		/**
		 * @brief							This function is used to clear bits of the variable m_bones_thrown_mask.
		 * @return void						none
		 */
		void Disable_Bone( void ) const;

		/**
		 * @brief							This function is used to test whether Bone is enabled or not.
		 * @return bool						status of requested operation.
		 */
		bool Is_Bone_Enabled( void ) const;

	private:
		/**
		 * @brief							Copy Constructor and Copy Assignment Operator.
		 * definitions						made private to disallow usage.
		 */
		uC_Watchdog( const uC_Watchdog& watchdog );
		uC_Watchdog & operator =( const uC_Watchdog& object );

		/**
		 * @brief							The function puts the controller in soft reset condition.
		 * @n								Integrator should use this function when he wants to perform soft reset
		 * @return void						None
		 */
		static void Soft_Reset( void );

		/**
		 * @brief							Get_Bone.
		 * @n								Returns the watchdog bone id type
		 * @return WATCHDOG_BONE_ID_TYPE	status of requested operation.
		 */
		static WATCHDOG_BONE_ID_TYPE Get_Bone( void );

		/**
		 * @brief							Private member variable for a uint8_t.
		 * @n								Number of bones thrown.
		 * @n								bone.
		 */
		static uint8_t m_number_bones_thrown;
		uint8_t m_bone;

		/**
		 * @brief							Private member variable for a uint16_t.
		 * @n								bone thrown mask.
		 * @n								dog bones return mask.
		 */
		static uint_fast16_t m_bones_thrown_mask;
		static uint_fast16_t m_dog_bones_returned_mask;
};

#ifdef __cplusplus
}
#endif

#endif
