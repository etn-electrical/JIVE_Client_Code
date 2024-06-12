/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
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

// 16 because we have a 16 bit variable for storing flags.
#define MAX_BONES       16U

/*
 *****************************************************************************************
 *		Typedefs
 *****************************************************************************************
 */

typedef uint_fast8_t WATCHDOG_BONE_ID_TYPE;

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_Watchdog
{
	public:
		uC_Watchdog( void );
		~uC_Watchdog();

		static void Init( uint16_t timeout_in_ms );

		static void Force_Feed_Dog( void );

		static void Handler( void* param0 = NULL );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Feed_Dog( void )		// Set bit that bone_ID reported in.
		{ Set_Bit( m_dog_bones_returned_mask, m_bone ); }

		void Enable_Bone( void ) const;

		void Disable_Bone( void ) const;

		bool Is_Bone_Enabled( void ) const;

	private:
		static void Soft_Reset( void );			/// This is private because it may not be used any longer.  uC_Reset has
												/// superseded it.

		static WATCHDOG_BONE_ID_TYPE Get_Bone( void );

		static uint8_t m_number_bones_thrown;
		static uint_fast16_t m_bones_thrown_mask;
		static uint_fast16_t m_dog_bones_returned_mask;

		uint8_t m_bone;
};


#endif
