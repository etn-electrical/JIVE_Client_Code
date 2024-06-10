/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *	This class provides a random number generator. In the STM32WB family,
 *	after the BLE stack has been started, the use of the True Random Number
 *	Generator (TRNG) of the microcontroller is protected with a Semaphore (see
 *	https://community.st.com/s/question/0D50X0000BedYuBSQU/rng-access-after-starting-ble-stack).
 *	In particular, a running BLE advertising is one of the conditions that block
 *	access to the TRNG. Therefore, a Mersenne Twister pseudorandom number
 *	generator (PRNG) is initialized together with the TRNG and seeded by the latter. It
 *	is suggested to initialize the TRNG (creating a new uC_Rand_Num object) and
 *	then deinitialize it (calling De_Init()) before the BLE stack is
 *	initialized. Further random numbers will be provided by the Mersenne Twister.
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_RAND_NUM_H
   #define uC_RAND_NUM_H

#include "uC_Base.h"
#include <random>

/*
 ***********************************
 *	Variables and Constants
 ***********************************
 */

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_Rand_Num
{
	public:
		/**
		 * @brief                   Initializes the TRNG and PRNG.
		 */
		uC_Rand_Num( void );
		/**
		 * @brief                   Empty Destructor
		 */
		~uC_Rand_Num() {}

		/**
		 * @brief                   Get a set of random numbers using the TRNG
		 *                          if available, otherwise it uses the PRNG.
		 * @param[in] rand_data     Pointer to location where to store the random data.
		 * @param[in] length        Amount of random bytes to be generated.
		 */
		static void Get( uint8_t* rand_data, uint16_t length );

		/**
		 * @brief                   Deinitializes the TRNG.
		 */
		static void De_Init( void );

		/**
		 * @brief                   Provides information on whether the TRNG is initialized.
		 * @return                  True if the TRNG is active, false otherwise.
		 */
		static bool Is_Active( void );

	private:
		static std::mt19937 prng;
		static bool TRNG_active;

		/**
		 * @brief                   Performs TRNG seed error recovery as outlined
		 *                          in sec. 21.3.7 of the programming manual.
		 *                          The functions returns only when the seed error
		 *                          is fully recovered.
		 */
		static void TRNG_Error_Recovery( void );

};


#endif



