/**
 *****************************************************************************************
 *	@file        uc _watchdog header file for the watchdog functionality
 *
 *	@brief       This file shall wrap all functions required for watchdog software functionality.
 *
 *	@details     The watchdog timer serves to detect and resolve malfunctions due to software
 *	             failure and to trigger system reset or an interrupt when the counter reaches
                 a given timeout value.
 *
 *  @copyright   2014 Eaton Corporation. All Rights Reserved.
 *
 *	@note        Eaton Corporation claims proprietary rights to the material disclosed
 *               here in.  This technical information should not be reproduced or used
 *               without direct written permission from Eaton Corporation.*
 *
 *
 *
 *
 *
 *
 *****************************************************************************************
 */
#ifndef uC_WATCHDOG_H
#define uC_WATCHDOG_H

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

#define MAX_BONES       16U

/*
 *****************************************************************************************
 *	Typedefs

 *****************************************************************************************
 */
/**
 * @brief         Typedef for watchdog bone
 */

typedef uint_fast8_t WATCHDOG_BONE_ID_TYPE;

/*
 **************************************************************************************************
 *   uc_Watchdog class declaration     This class includes all the functions which will help in
 * enabling the watchdog timer.
   --------------------------------------------------------------------------------------------------
 */
class uC_Watchdog
{
	public:
		/**
		 * @brief      Constructor to create an instance of uc_watchdog class.
		 * @note       This constructor is used to assign the value returned by Get_ bone() to the
		 * variable m_bone.
		 */

		uC_Watchdog( void );
		/**
		 **********************************************************************************************
		 * @brief       Destructor to delete the uC_Watchdog instance when it goes out of scope

		 **********************************************************************************************
		 */

		~uC_Watchdog();

		/**
		 * @brief       This function is used to initialise the timeout value.
		 * @param[in]   uint16_t    timeout_in_ms[input the timeout in ms e.g 1000]
		 * @return[out] void      none
		 * @note        This function is used to set the timeout value for the downcounter which is
		 * used to trigger system reset.
		 */

		static void Init( uint16_t timeout_in_ms );

		/**
		 * @brief       This function is used to assign reload value 0xAAAA into key register of
		 * IWDG.
		 */

		static void Force_Feed_Dog( void );

		/**
		 * @brief       This function is used to test the reset condition of the IWDG .
		 * @param[in]   void*   param0
		 * @return[out] void    none

		 */

		static void Handler( void* param0 = NULL );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Feed_Dog( void )	// Set bit that bone_ID reported in.
		{
			Set_Bit( m_dog_bones_returned_mask, m_bone );
		}

		/**
		 * @brief      This function is used to set bits of the variable m_bones_thrown_mask by left
		 * shifting m_bone by 1 .
		 *  @note       This is used to set bits of the variable m_dog_bones_returned_mask by left
		 * shifting m_bone .
		 */
		void Enable_Bone( void ) const;

		/**
		 * @brief       This function is used to clear bits of the variable m_bones_thrown_mask .
		 */

		void Disable_Bone( void ) const;

		/**
		 * @brief       This function is used to test whether IWDG is enabled or not.
		 * @param[in]   void   none
		 * @return[out] bool
		 */

		bool Is_Bone_Enabled( void ) const;

	private:
		static void Soft_Reset( void );	/// This is private because it may not be used any longer.

		// uC_Reset has superseded it.

		static WATCHDOG_BONE_ID_TYPE Get_Bone( void );

		static uint8_t m_number_bones_thrown;
		static uint_fast16_t m_bones_thrown_mask;
		static uint_fast16_t m_dog_bones_returned_mask;

		uint8_t m_bone;
};

#endif
