/**
 **************************************************************************************************
 *@file           uC_Queued_I2C.h Header File for I2C Queue implementation.
 *
 *@brief          Its a processor independent file for I2C queued class implementation
 *@details
 *@copyright      2014 Eaton Corporation. All Rights Reserved.
 *@note           Eaton Corporation claims proprietary rights to the material disclosed
 *                hereon.  This technical information may not be reproduced or used
 *                without direct written permission from Eaton Corporation.
 **************************************************************************************************
 */
#ifndef uC_Queued_I2C_H
#define uC_Queued_I2C_H

/**
 **************************************************************************************************
 * @brief    Processor independent queued I2C base class
 *
 * @details  Base class for I2C objects that queue requests and provides call-back
 *           options to support asynchronous access to multiple devices sharing a single I2C bus.
 *           The class must be specialized via a child class to provide platform-specific control
 *           of relevant I2 resources.
 *
 **************************************************************************************************
 */
class uC_Queued_I2C
{
	public:

		/** @brief The enumeration which signifies the I2C exchange */
		enum i2c_exchange_type
		{
			/** @brief I2C Read excahnge */
			I2C_WRITE_EXCHANGE,
			/** @brief I2C Write excahnge */
			I2C_READ_EXCHANGE
		};

		/** @brief Exchange structure */
		struct exchange_struct_t;
		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );

		/**
		 * @brief     Structure used to define the buffers and length to use for a segement
		 *            of the I2C exchange.
		 *            Its a self referntial structure in which the current segment will
		 *            also hold the details of next segment to be exchnaged over an I2C.
		 */
		struct exchange_segment_t
		{

			struct exchange_segment_t* next_exchange_segment;
			/** @brief pointer to the data to be received */
			uint8_t* rx_data;
			/** @brief pointer to the data to be transmitted */
			uint8_t* tx_data;
			/** @brief Amount of data to be transmitted (in bytes)*/
			uint8_t tx_length;
			/** @brief Amount of data to be received (in bytes)*/
			uint8_t rx_length;
			/** @brief exchange type either read exchange or write exchange*/
			i2c_exchange_type exchange_type;
			/** @brief chip address of I2C slave device*/
			uint16_t chip_address;
		};

		/**
		 * @brief       Structure used to define the device select, series of exchange
		 *              segments and post exchange call back function to use for the I2C
		 *              exchange.
		 */
		struct exchange_struct_t
		{
			/** @brief Next exchange structure in queue */
			struct exchange_struct_t* next_exchange_struct;
			/** @brief exchange segment address */
			struct exchange_segment_t* exchange_segment;
			/** @brief call back function when exchange is finished */
			cback_func_t post_exchange_cback;
			cback_param_t call_back_arg;
			/** @brief statusof exchange */
			uint8_t status;
		};

		/**
		 * @brief                      Queues a I2C exchange.
		 * @param[in] exchange_struct  A pointer to the exchange structure to be used for
		 *                             the I2C exchange.
		 */
		void Process_Exchange( exchange_struct_t* exchange_struct );

		/**
		 * @brief               Fills two exchange segment structures based on the passed
		 *                      arguments.
		 * @param[in] segment1  Pointer to exchange segment to be filled by function.
		 * @param[in] segment2  Pointer to exchange segment to be filled by function
		 *                      (if necessary).
		 * @return              A pointer to the segment1 exchange segment. If required,
		 *                      this segment will be chained to the segment2 segment.
		 */
		static exchange_segment_t* Generate_Segment_Chain( exchange_segment_t* segment1,
														   exchange_segment_t* segment2 );

		/**
		 * @brief Fills exchange structure with default initialization values.
		 * @param exchange_struct - Pointer to exchange structure to be filled.
		 */
		static void Initialize_Exchange_Struct( exchange_struct_t* exchange_struct );

		static const uint8_t EXCHANGE_COMPLETE = 0x80;
		static const uint8_t EXCHANGE_FAILED = EXCHANGE_COMPLETE | 1;
		static const uint8_t EXCHANGE_PENDING = 0;

	protected:

		/**
		 * @brief                 Constructor to create an instance of
		 *                        uC_Queued_I2C class. Itcan only be
		 *                        instantiated by child class
		 * @note
		 */
		uC_Queued_I2C( void );

		/**
		 * @brief                 Destructor to delete an instance of uc_I2C class
		 * @return
		 */
		virtual ~uC_Queued_I2C( void )
		{}

		/**
		 * @brief                 Perform end of post-segment exchange operations.
		 *                        This includes initiating the next I2C segment exchange
		 *                        or, if the full exchange is complete, the next new exchange
		 *                        if queue is not empty) and performing post-exchange call
		 *                        backs (if requested).
		 */
		void Exchange_Complete( bool_t exchange_failure );

		/**
		 * @brief                 Check queue and initiate next exchange if not empty.
		 */
		void Check_Queue( void );

		/**
		 * @brief                  Perform the necessary setup of platform resources to
		 *                         exchange the next segment of the active I2C exchange.
		 * @param[in] new_exchange Flag to indicate the start of a new exchange.
		 */
		virtual void Start_Exchange( bool_t new_exchange ) = 0;

		exchange_struct_t* m_exchange_queue;
		exchange_segment_t* m_active_segment;
};

#endif
