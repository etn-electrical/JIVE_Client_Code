/**
 *****************************************************************************************
 *	@file
 *
 *	@brief Contains the CRC Class. This module implements the 16bit CRC-CCITT
 *	algorithm (0x11021).
 *
 *	@details The algorithm used for this implementation is quite fast on an embedded
 *	system in the non-table format.  The table format may be a little quicker but this
 *	should be tested if speed is crucial.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef CRC16_H
#define CRC16_H

namespace BF_Lib
{
/*
 *****************************************************************************************
 *		Constants for internal switching between different CRC calc methods.
 *****************************************************************************************
 */
/**
 * @brief For the CRC16_VERSION it is possible to change the method of calculating the CRC.
 * If it is desired to use an alternate CRC method based on speed this can be defined in
 * Device_Config.h.  For example if the table version is quicker and more space is available
 * it may be better to choose that version instead.
 */
#ifndef CRC16_VERSION
	#define        TABLE_CRC_VERSION_8BIT                      0U
	#define        SHIFT_CRC_VERSION                           1U
	#define        TABLE_CRC_VERSION_16BIT                     2U
	#define        CRC16_VERSION                               TABLE_CRC_VERSION_16BIT
#endif

/**
 ****************************************************************************************
 * @brief This is the CRC16 class used for the implementation of the 16bit CRC-CCITT.
 * @details The CRC16 class provides the functionalities to calculate the CRC
 * on string and from byte.
 * @n @b Usage: Calculations of CRCs.
 ****************************************************************************************
 */
class CRC16
{
	public:

		/// The default CRC value.
		static const uint16_t DEFAULT_INIT = 0xffffU;

		/**
		 * @brief Calculates a CRC on a string.
		 * @param[in] data: The pointer to the data.
		 * @param[in] length: The length of data to check.
		 * @param[in] init_crc: The initial CRC value.
		 * @return The new CRC value.
		 */
		static uint16_t Calc_On_String( uint8_t const* data, uint32_t length,
										uint16_t init_crc = DEFAULT_INIT );

		/**
		 * @brief Will calculate the CRC value from a singly byte.  It can be used
		 * where a CRC is generated in steps.
		 * @param new_byte:  New byte to add.
		 * @param prev_CRC: The previous or initial CRC depending on how the string is
		 * checked.
		 * @return The new CRC value.
		 */
		static uint16_t Calc_On_Byte( uint8_t new_byte, uint16_t prev_CRC );

	private:
		/**
		 * Destructor, Copy Constructor and Copy Assignment Operator definitions
		 * to disallow usage.
		 */
		CRC16( const CRC16& rhs );
		CRC16 & operator =( const CRC16& object );

		CRC16( void );
		~CRC16( void );

#if CRC16_VERSION == TABLE_CRC_VERSION_8BIT
		static const uint8_t table_hi[256];
		static const uint8_t table_lo[256];
#elif CRC16_VERSION == TABLE_CRC_VERSION_16BIT
		static const uint16_t table[256];
#endif
};

}

#endif	/* CRC16_H */
