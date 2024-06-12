/**
 *****************************************************************************************
 *	@file uC_Queued_SPI.h
 *
 *	@brief Processor Independent Queued SPI base class
 *
 *	@details
 *
 *	@version C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_Queued_SPI_H
#define uC_Queued_SPI_H

/**
 ****************************************************************************************
 * @brief Processor independent queued SPI base class
 *
 * @details	Base class for SPI objects that queue requests and provides call-back
 *              options to support asynchronous access to multiple devices sharing a
 *              single SPI bus. The class must be specialized via a child class to
 *              provide platform-specific control of relevant SPI resources.
 *
 ****************************************************************************************
 */
class uC_Queued_SPI
{
	public:
		/** @brief The enumeration which signifies the SPI communication mode */
		/* Mode   CPOL 	  CPHA
		   0 	   0	   0
		   1 	   0	   1
		   2 	   1	   0
		   3 	   1	   1
		 */
		enum spi_mode
		{
			/** @brief SPI mode 0 (CPOL =0, CPHA =0) */
			MODE_0,
			/** @brief SPI mode 1 (CPOL =0, CPHA =1) */
			MODE_1,
			/** @brief SPI mode 2 (CPOL =1, CPHA =0) */
			MODE_2,
			/** @brief SPI mode 3 (CPOL =1, CPHA =1) */
			MODE_3,
		};

		struct exchange_struct_t;
		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );

		/**
		 * @brief    Structure used to define the buffers and length to use
		 *           for a segement of the SPI exchange.
		 */
		struct exchange_segment_t
		{
			/** @brief  Pointer to Next exchange segment */
			struct exchange_segment_t* next_exchange_segment;
			/** @brief  Pointer to receive data buffer */
			uint8_t* rx_data;
			/** @brief  Pointer to transmit data buffer */
			uint8_t* tx_data;
			/** @brief  length of data to be exchanged in bytes */
			uint16_t length;
		};

		/**
		 * @brief    Structure used to define the device select, series of
		 *           exchange segments and post exchange call back function
		 *           to use for the SPI exchange.
		 */
		struct exchange_struct_t
		{
			/** @brief  Pointer to Next exchange structure/trasaction
			 *          request
			 */
			struct exchange_struct_t* next_exchange_struct;
			/** @brief  Pointer to exchange segment */
			struct exchange_segment_t* exchange_segment;
			/** @brief  The callback function to be called after
			 *          exchange is finished
			 */
			cback_func_t post_exchange_cback;
			/** @brief The callback argument */
			cback_param_t call_back_arg;
			/** @brief transaction status pass/fail */
			uint8_t status;
			/** @brief Assigned Slave Id of the SPI device */
			uint8_t select_id;
		};

		/**
		 * @brief                      Queues a SPI exchange.
		 * @param[in]  exchange_struct A pointer to the exchange structure to be
		 *                             used for the SPI exchange.
		 */
		void Process_Exchange( exchange_struct_t* exchange_struct );

		/**
		 * @brief                 SPI Communication Configuration. The SPI chip
		 *                        configuration which variesfrom SPI based chip
		 *                        to chip.e.g it could be different for SPI based
		 *                        EEPROM and lets say SPI based RTC.
		 * @param[in] mode        Mode of the SPI slave chip. Possible values are
		 * @n                     MODE_0 - CPOL  = 0 & CPHASE = 0
		 * @n                     MODE_1 - CPOL  = 0 & CPHASE = 1
		 * @n                     MODE_2 - CPOL  = 1 & CPHASE = 0
		 * @n                     MODE_3 - CPOL  = 1 & CPHASE = 1
		 * @param[in] chip_clock_freq SPI chip clock frequency in Hz
		 * @param[in] assign_chip_id Assign the user defined chip id to each slave
		 *                        The chip id should start from 0 and maximum 255
		 *                        The chip should be in continuos succession
		 *                        e.g. 0,1,2,3....and so on
		 */
		virtual void SPI_Chip_Configuration( spi_mode mode, uint32_t chip_clock_freq,
											 uint8_t assign_chip_id ) = 0;

		/**
		 * @brief                 Fills two exchange segment structures based on the
		 *                        passed arguments.
		 * @param[in] segment1    Pointer to exchange segment to be filled by function.
		 * @param[in] segment2    Pointer to exchange segment to be filled by function
		 *                        (if necessary).
		 * @param[in] rx_buffer   Pointer to receive buffer for SPI exchange.
		 * @param[in] tx_buffer   Pointer to transmit buffer for SPI exchange.
		 * @param[in] rx_length   Number of bytes to accept into receive buffer.
		 * @param[in] tx_length   Number of bytes to transmit from transmit buffer.
		 * @return                A pointer to the segment1 exchange segment.
		 *                        If required, this segment will be chained
		 *                        to the segment2 segment.
		 */
		static exchange_segment_t* Generate_Segment_Chain( exchange_segment_t* segment1,
														   exchange_segment_t* segment2,
														   uint8_t* rx_buffer,
														   uint8_t* tx_buffer,
														   uint32_t rx_length,
														   uint32_t tx_length );

		/**
		 * @brief                   Fills exchange structure with default
		 *                          initialization values.
		 * @param exchange_struct - Pointer to exchange structure to be filled.
		 */
		static void Initialize_Exchange_Struct( exchange_struct_t* exchange_struct );

		static const uint8_t EXCHANGE_COMPLETE = 0x80;
		static const uint8_t EXCHANGE_FAILED = EXCHANGE_COMPLETE | 1;
		static const uint8_t EXCHANGE_PENDING = 0;

	protected:
		/**
		 * @brief                Constructs a SPI interface (can only be instantiated
		 *                       by child class).
		 * @param select_count - Number of device selects to be supported.
		 */
		uC_Queued_SPI( uint8_t select_count = 0 );

		/**
		 * @brief                 Destructor to delete an instance of uC_Queued_SPI class
		 */
		virtual ~uC_Queued_SPI( void )
		{}

		/**
		 * @brief                 Perform end of post-segment exchange operations.
		 *                        This includes initiating the next SPI segment exchange
		 *                        or if the full exchange is complete, the next new exchange
		 *                        (if queue is not empty) and performing post-exchange
		 *                        call backs (if requested).
		 * @param[in] exchange_failure Two possible values.
		 * @n                      true - SUCCESS
		 * @n                      false - FAILURE
		 *
		 */
		void Exchange_Complete( bool exchange_failure );

		/**
		 * @brief Check queue and initiate next exchange if not empty.
		 */
		void Check_Queue( void );

		/**
		 * @brief Perform the necessary setup of platform resources to exchange the
		 * next segment of the active SPI exchange.
		 * @param new_exchange - Flag to indicate the start of a new exchange.
		 */
		virtual void Start_Exchange( bool new_exchange ) = 0;

		exchange_struct_t* m_exchange_queue;
		exchange_segment_t* m_active_segment;
		uint8_t m_active_select;
		uint8_t m_select_count;
};

#endif

