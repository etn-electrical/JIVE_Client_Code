/**
 *****************************************************************************************
 *  @file
 *	@brief This file provide the Master-Slave (Client-server) negotiation
 *
 *	@details Modbus slave shall negotiate with master with different baud rates to club
 *        with Modbus master for communication
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MODBUS_NEGOTIATION_H_
#define MODBUS_NEGOTIATION_H_

#include "Includes.h"
#include "CR_Tasker.h"
#include "Modbus_Net.h"
#include "Modbus_Defines.h"

/**
 ****************************************************************************************
 * @brief This is a Modbus_Negotiation class
 * @details Negotiates with client regarding appropriate baud rate to communicate with
 * It provides public methods to
 * @n @b 1. Negotiate baud rate
 * @n @b 2. identify parity, stop bits, tx mode for communication
 ****************************************************************************************
 */
class Modbus_Negotiation
{
	public:

		/// TX_MODES
		static const uint8_t RTU = 0U;
		static const uint8_t ASCII = RTU + 1U;

		/// PARITY_MODES
		static const uint8_t EVEN_PARITY = 0U;
		static const uint8_t ODD_PARITY = EVEN_PARITY + 1U;
		static const uint8_t NO_PARITY = ODD_PARITY + 1U;

		/// STOP_BITS
		static const uint8_t STOP_BITS_1 = 0U;
		static const uint8_t STOP_BITS_2 = STOP_BITS_1 + 1U;

		/// Long Holdoff "Bool"
		static const uint8_t USE_LONG_HOLDOFF_TRUE = 0xFFU;
		static const uint8_t USE_LONG_HOLDOFF_FALSE = 0U;

		/// Set Frame definition.
		static const uint16_t TX_MODE = 0U;
		static const uint16_t PARITY = TX_MODE + 1U;
		static const uint16_t BAUD_RATE = PARITY + 1U;
		static const uint16_t USE_LONG_HOLDOFF = BAUD_RATE + sizeof( uint32_t );
		static const uint16_t STOP_BIT = USE_LONG_HOLDOFF + 1U;
		static const uint16_t NEGOTIATION_FRAME_LENGTH = STOP_BIT + 1U;

		/**
		 *  @brief Constructor
		 *  @details Creates a Modbus negotiation
		 *  @param[in] modbus_net: Modbus Host port pointer which could be address by master
		 *  @return this
		 */
		Modbus_Negotiation( Modbus_Net* modbus_net );

		// Default Negotiation baud rate
		static const uint32_t NEGOTIATION_BAUD_DEFAULT = MODBUS_BAUD_19200;


		/**
		 *  @brief Destructor
		 *  @details It does nothing here.
		 */
		virtual ~Modbus_Negotiation( void );

		/**
		 *  @brief Modbus negotiation
		 *  @details create modbus negotiation frame with communication settings
		 *  @param[in] rx_struct: frame with modbus communication settings received
		 *  @param[out] tx_struct: frame with modbus communication settings transmitted
		 *  @return 0U (= MB_NO_ERROR_CODE)
		 */
		uint8_t Negotiate( MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct );

		/**
		 *  @brief Modbus negotiation
		 *  @details Attach user function for callback
		 *  @param[in] param: callback handle make sure to call negotiate
		 *  @param[in] rx_struct: frame with modbus communication settings received
		 *  @param[out] tx_struct: frame with modbus communication settings transmitted
		 *  @return 0U (= MB_NO_ERROR_CODE)
		 */
		static uint8_t Negotiate_Static( modbus_message_callback_param_t param,
										 MB_FRAME_STRUCT* rx_struct, MB_FRAME_STRUCT* tx_struct )
		{ return ( ( ( Modbus_Negotiation* )param )->Negotiate( rx_struct, tx_struct ) ); }

		static const uint8_t MODBUS_NEGOTIATION_CODE = 0x47U;

		/**
		 *  @brief serial communication parity
		 *  @details Identify communication parity
		 *  @param[in] parity: serial communication parity
		 *  @retval 0U: in case Even parity
		 *  @retval 1U: in case Odd parity
		 *  @retval 2U: in case No parity
		 */
		static uint8_t Parity( parity_t parity );

		static parity_t Parity( uint8_t parity );

		/**
		 *  @brief serial communication Transmission mode
		 *  @details Identify communication transmission mode(ASII/RTU)
		 *  @param[in] tx_mode: transmission mode
		 *  @retval 0U: in case RTU
		 *  @retval 1U: in case ASII
		 */
		static uint8_t TX_Mode( tx_mode_t tx_mode );

		static tx_mode_t TX_Mode( uint8_t tx_mode );

		/**
		 *  @brief serial communication Stop bits
		 *  @details Identify number of stop bits in communication.
		 *  @param[in] stop_bits: number of stop bits
		 *  @retval 0U: one stop bit
		 *  @retval 1U: two stop bits
		 */
		static uint8_t Stop_Bits( stop_bits_t stop_bits );

		static stop_bits_t Stop_Bits( uint8_t stop_bits );

		/**
		 *  @brief serial communication Long holdoff
		 *  @details Identify whether Long holdoff needed in comminication
		 *  @param[in] long_holdoff: long hold off indication
		 *  @retval 0U: long hold off false
		 *  @retval FFU: long hold off true
		 */
		static uint8_t Long_Holdoff( bool long_holdoff );

		static bool Long_Holdoff( uint8_t long_holdoff );

	private:

		// Set success response
		static const uint8_t CONFIG_SUCCESS = 0xFFU;
		static const uint8_t CONFIG_FAILURE = 0U;

		static const uint8_t CONFIG_DATA_OFFSET = 2U;

		// Response Frame Definition
		static const uint16_t CONFIG_SUCCESS_OFFSET = CONFIG_DATA_OFFSET;
		static const uint16_t NEGOTIATION_FRAME_RESP_LENGTH = CONFIG_SUCCESS_OFFSET + 1U;

		void Baud_Process( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Baud_Process_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param )
		{ ( reinterpret_cast<Modbus_Negotiation*>( param ) )->Baud_Process(); }

		Modbus_Net* m_modbus_net;
		CR_Tasker* m_change_tasker;

		uint32_t m_baud;
		parity_t m_parity;
		bool m_use_long_holdoff;
		stop_bits_t m_stop_bits;
		tx_mode_t m_tx_mode;
};

#endif	/* MODBUS_NEGOTIATION_H_ */
