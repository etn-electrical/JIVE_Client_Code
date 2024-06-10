/**
 *****************************************************************************************
 *	@file
 *
 *	@brief this file contains the PTP class which can be used to enable timestamping
 *         (IEEE 1588)feature
 *	@details The PTP protocol can be used to enable timestamping as well as get a system time
 *           which can have a nano second accuracy based on the PTP refernce clock and the
 *         master clock providing the time reference.
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef UC_PTP_SLAVE_H
#define UC_PTP_SLAVE_H

#include "Includes.h"
#include "stm32H7xx_hal_rcc.h"
#include "stm32_eth.h"
#include "uC_Base.h"
#include "Device_Config.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
struct ptptime_t
{
	int32_t tv_sec;
	int32_t tv_nsec;
};

/**
 *****************************************************************************************
 * @brief    The STM32 micro controller specific class for IEEE 1588 (PTP).
 *           This class shall serve as the basic interfaces for enabling PTP in EMAC
 *****************************************************************************************
 */

class uC_PTP_Slave
{
	public:

		enum node_type
		{
			/* a clock that a single PTP port in a domain and maintains time scale in domain */
			ORDINARY_CLOCK = 0U,
			/* a clock that has multiple PTP port in a domain and maintains time scale in domain.
			 * It may serve as master clock or can be slave clock*/
			BOUNDARY_CLOCK,
			/* a transparent clock that supports the use of End to End delay mechanism between
			 * master and slave */
			E2E_TC_CLOCK,
			/* a transparent clock in addition to providing PTP event transit time also provides
			   link delay
			 * correction methods*/
			P2P_TC_CLOCK
		};

		enum packet_type
		{
			PTP_VER_1 = 1U,
			PTP_VER_2
		};

		/**
		 * @brief           -  Constructs a uC_PTP_Slave object.
		 * @param node_type -  PTP devices category
		 * @n                  ordinary clock
		 * @n                  boundary clock
		 * @n                  E2E Transparent clock
		 * @n                  P2P Transparent clock
		 * @param update_method  -  Fine correction or Coarse method
		 * @param pkt_snooping   -  select PTP version 1 or version 2
		 */
		uC_PTP_Slave ( uint8_t node_type, uint8_t update_method, uint8_t pkt_snooping );

		/**
		 * @brief                -  Destructor to delete an instance of uC_PTP_Slave
		 *                          class when object goes out of scope.
		 */
		~uC_PTP_Slave ();


		/**
		 * @brief           -  Get PTP system time.
		 * @param timestamp - pointer to get system time value
		 */
		void Get_System_Time( struct ptptime_t* timestamp );

		/**
		 * @brief           -  Set PTP system time.
		 * @param timestamp - pointer to set system time value
		 */
		void Set_Time( struct ptptime_t* timestamp );

		/**
		 * @brief      -  Updates time base offset
		 * @param offset - Time offset with sign
		 *
		 */
		void Time_Stamp_Update_Offset( struct ptptime_t* offset );

		/**
		 * @brief      -  Updates time stamp frequency
		 * @param adj - correction value in thousandth of ppm (Adj*10^9)
		 *
		 */
		void Time_Stamp_Adj_Frq( int32_t adj );

	private:

		/**
		 * @brief                -  Start PTP initialization.
		 * @param update_method - Time stamp update method
		 *                        Fine correction or Coarse method
		 */
		void Start_Initialization( uint8_t update_method );

		/**
		 * @brief                - enable or disable specified MAC interrupt
		 * @param state - enable or disable interrupt
		 * @param eth_mac_int - bit to enable or disable interrupt
		 */
		void ETHMACIT_Config( uint32_t eth_mac_int, FunctionalState state );

		/**
		 * @brief - Updated the PTP block for fine correction with the Time Stamp Addend register
		 * value.
		 */
		void Enable_Time_Stamp_Addend( void );

		/**
		 * @brief       - Enables or disables the PTP time stamp for transmit and receive frames.
		 * @param state - new state of the PTP time stamp for transmit and receive frames
		 */
		void Time_Stamp_Cmd( FunctionalState state );

		/**
		 * @brief       - Checks whether the specified ETHERNET PTP flag is set or not.
		 * @param ptp_flag - specifies the flag to check
		 * @retval      - new state of ETHERNET PTP Flag
		 */
		FlagStatus Get_TSCR_Flag_Status( uint32_t ptp_flag );

		/**
		 * @brief                -  set PTP target time
		 * @param target_high_val -  seconds value of timestamp
		 * @param target_low_val  -  fractional part of seconds value of timestamp
		 *
		 */
		void Set_Target_Time( uint32_t target_high_val, uint32_t target_low_val );

		/**
		 * @brief                -  set PTP TimeStamp addend value
		 * @param addend_val -  addend value calculated based on PTP reference clock
		 *
		 */
		void Set_Time_Stamp_Addend_Value( uint32_t addend_val );

		/**
		 * @brief                -  set PTP TimeStamp Update register
		 * @param Sign           -  whether time is positive or negative value
		 * @param SecondValue    -  second value
		 * @param SubSecondValue -  fractional subsecond value
		 *
		 */
		void Set_Time_Stamp_Update( uint32_t sign, uint32_t secondValue,
									uint32_t subSecondValue );

		/**
		 * @brief                -  set PTP subsecond increment value
		 * @param subsecondvalue -  value to increment subsecond value
		 *
		 */
		void Set_Subsecond_Increment( uint32_t subsecondvalue );

		/**
		 * @brief               -  Selects the PTP Update method
		 * @param update_method -  the PTP Update method
		 * This parameter can be one of the following values:
		 * @arg ETH_PTP_FineUpdate   : Fine Update method
		 * @arg ETH_PTP_CoarseUpdate : Coarse Update method
		 */
		void Update_Method_Config( uint32_t update_method );

		/**
		 * @brief      -  Initialize the PTP Time Stamp TSCR bit
		 * @param void -  None
		 *
		 */
		void TSCR_Initialize_Time_Stamp_Bit( void );

		/**
		 * @brief      -  convert nanoseconds to subseconds
		 * @param subsecondvalue - Time offset with sign
		 *
		 */
		uint32_t Nanosecond_To_Subsecond( uint32_t subsecondvalue );

		/**
		 * @brief      -  convert subseconds to nanoseconds
		 * @param subsecondvalue - Time offset with sign
		 *
		 */
		uint32_t Subsecond_To_Nanosecond( uint32_t subsecondvalue );

		/**
		 * @brief      -  Get the specified ETHERNET PTP register value
		 * @param ptp_reg - specifies the ETHERNET PTP register.
		 *   This parameter can be one of the following values:
		 * @arg ETH_PTPTSCR  : Sub-Second Increment Register
		 * @arg ETH_PTPSSIR  : Sub-Second Increment Register
		 * @arg ETH_PTPTSHR  : Time Stamp High Register
		 * @arg ETH_PTPTSLR  : Time Stamp Low Register
		 * @arg ETH_PTPTSHUR : Time Stamp High Update Register
		 * @arg ETH_PTPTSLUR : Time Stamp Low Update Register
		 * @arg ETH_PTPTSAR  : Time Stamp Addend Register
		 * @arg ETH_PTPTTHR  : Target Time High Register
		 * @arg ETH_PTPTTLR  : Target Time Low Register
		 * @retval The value of ETHERNET PTP Register value.
		 */
		uint32_t Get_Register_Value( uint32_t ptp_reg );

		/**
		 * @brief     - Updated the PTP system time with the Time Stamp Update register value.
		 * @param None
		 *
		 */
		void Enable_Time_Stamp_Update();

		static const uint64_t nanosecond_to_subsecond_multiplier = 0x80000000ll;
		static const uint32_t nanosecond_to_subsecond_conv = 1000000000ll;
		static const uint8_t shift_val = 31U;
		static const uint32_t ADJ_FREQ_BASE_ADDEND = 0x7FD9A601;
		static const uint8_t ADJ_FREQ_BASE_INCREMENT = 43U;
		/**
		 * @brief  ETH PTP registers
		 */
		static const uint32_t ETH_MACTSCR = 0x00000B00;
		static const uint32_t ETH_MACSSIR = 0x00000B04;
		static const uint32_t ETH_MACSTSR = 0x00000B08;
		static const uint32_t ETH_MACSTNR = 0x00000B0C;
		static const uint32_t ETH_MACSTSUR = 0x00000B10;
		static const uint32_t ETH_MACSTNUR = 0x00000B14;
		static const uint32_t ETH_MACTSAR = 0x00000B18;
		static const uint32_t ETH_MACTSSR = 0x00000B20;
		static const uint32_t ETH_MACTXTSSNR = 0x00000B30;
		static const uint32_t ETH_MACTXTSSSR = 0x00000B34;
		static const uint32_t ETH_MACACR = 0x00000B40;
		static const uint32_t ETH_MACATSNR = 0x00000B48;
		static const uint32_t ETH_MACTATSSR = 0x00000B4C;
		static const uint32_t ETH_MACTSIACR = 0x00000B50;
		static const uint32_t ETH_MACTSEACR = 0x00000B54;
		static const uint32_t ETH_MACTSICNR = 0x00000B58;
		static const uint32_t ETH_MACTSECNR = 0x00000B5C;
		static const uint32_t ETH_MACPPSCR = 0x00000B70;
		static const uint32_t ETH_MACPPSTTSR = 0x00000B80;
		static const uint32_t ETH_MACPPSTTNR = 0x00000B84;
		static const uint32_t ETH_MACPPSIR = 0x00000B88;
		static const uint32_t ETH_MACPPSWR = 0x00000B8C;
		static const uint32_t ETH_MACPOCR = 0x00000BC0;
		static const uint32_t ETH_MACSPIOR = 0x00000BC4;
		static const uint32_t ETH_MACSPI1R = 0x00000BC8;
		static const uint32_t ETH_MACSPI2R = 0x00000BCC;
		static const uint32_t ETH_MACLMIR = 0x00000BD0;
};

#endif
