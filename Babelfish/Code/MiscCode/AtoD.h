/**
 *****************************************************************************************
 *	@file               A to D header file  for ADC channel functionality.
 *
 *	@brief              Analog to Digital channel and control structure initialization.
 *
 *	@details            Functionality also used to read the 16 bit digital value from the ADC channel.
 *                      The 12-bit ADC is a successive approximation analog-to-digital converter.
 *	                    The A/D conversion of the channels can be performed in single, continuous,
 *	                    scan or discontinuous mode. The result of the ADC is stored into a left or
                        right-aligned 16-bit data register.
 *  @copyright          2014 Eaton Corporation. All Rights Reserved.
 *	@note               Eaton Corporation claims proprietary rights to the material disclosed
 *                      here in.  This technical information should not be reproduced or used
 *                      without direct written permission from Eaton Corporation.
 *

 *
 *****************************************************************************************
 */
#ifndef ATOD_H
#define ATOD_H

#include "uC_AtoD.h"
#include "Timers_Lib.h"


namespace BF_Misc
{
/*
 **************************************************************************************************
 *  @brief The AtoD contains the Analog to Digital functionality.
 *  @details There is basically the AtoD channel and the AtoD control initialization.
 *  @n @b Usage:
 *  @n @b 1. Initialize AtoD.
 *  @n @b 2. Gives the 16 bit value from a particular ADC channel.
 **************************************************************************************************
 */
class AtoD
{
	public:
		/**
		 *  @brief     Constructor for Ato D class i.e. Analog to Digital Function.
		 *  @details   This function is overlayed on top of the microprocessor
		 *             intrinsic AtoD functionality "uC_AtoD"
		 *  @param[in] uc_atod_ctrl: Analog to Digital Channel control structure
		 *  @param[in] channel: Analog to Digital Channel physical channel ID
		 *  @note:     This constructor is used to select the channel of the ADC.
		 *             It is used intrinsically with the uC_A to D class to turn on and off the ADC.
		 */
		AtoD( uC_AtoD* uc_atod_ctrl, uint8_t channel );

		/**
		 *  @brief Destructor
		 *  @details It will get called when object AtoD goes out of scope or deleted.
		 *  @return None
		 */
		~AtoD() {}

		/**
		 *  @brief
		 *  @details This gives the 16 bit value from a particular ADC channel.
		 *  @return The digital value of the channel read.
		 */
		inline uint16_t Get_Val( void )
		{
			return ( m_uc_atod_ctrl->Get_Channel( m_channel ) );
		}

	private:
		/**
		 * @brief Destructor, Copy Constructor and
		 * Copy Assignment Operator definitions made private to disallow usage.
		 * @n The fact that we are disallowing these standard items should be Doxygen
		 * documented.  Generally private members do not need to be Doxygen documented.
		 */
		AtoD( const AtoD& rhs );
		AtoD & operator =( const AtoD& object );

		uint8_t m_channel;
		uC_AtoD* m_uc_atod_ctrl;
};

}

#endif
