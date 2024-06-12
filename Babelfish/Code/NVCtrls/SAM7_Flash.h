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
#ifndef SAM7_FLASH_H
   #define SAM7_FLASH_H

#include "uC_Base.h"
#include "NV_Ctrl.h"

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class SAM7_Flash : public NV_Ctrl
{
	public:
		SAM7_Flash( NV_CTRL_ADDRESS_TD start_address,
					NV_CTRL_LENGTH_TD size, uint8_t flash_ctrl_id );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~SAM7_Flash() {}

		NV_Ctrl::nv_status_t Read( uint8_t* data, NV_CTRL_ADDRESS_TD address,
								   NV_CTRL_LENGTH_TD length, bool use_protection = true );

		NV_Ctrl::nv_status_t Write( uint8_t* data, NV_CTRL_ADDRESS_TD address,
									NV_CTRL_LENGTH_TD length, bool use_protection = true );

		NV_Ctrl::nv_status_t Erase( NV_CTRL_ADDRESS_TD address,
									NV_CTRL_LENGTH_TD length, uint8_t erase_data = 0, bool protected_data = true );

		NV_Ctrl::nv_status_t Erase_All( void );

	private:
		bool Read_Now( uint8_t* dest_data, uint8_t* read_ptr,
					   NV_CTRL_LENGTH_TD length, bool use_protection );

		void Write_Byte( uint8_t data, uint8_t* dest_byte_ptr, bool start, bool finish );

		bool Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length, bool protection = true );

		__arm void Write_Page_Now( uint16_t page );

		_AT91S_MC* m_efc;

		NV_CTRL_ADDRESS_TD m_start_address;
		NV_CTRL_ADDRESS_TD m_mirror_address;
		NV_CTRL_LENGTH_TD m_size;

		uint_fast8_t m_temp_word;						///< Used to store a word between writes of single bytes.
};


#endif
