/**
 *****************************************************************************************
 *	@file		uC_UHCI.h
 *
 *	@brief		UHCI class is the configuration interface between UART and ESP32 DMA Engine
 *
 *	@copyright	2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef uC_UHCI_H
   #define uC_UHCI_H

#include "uC_Base.h"
#include "string.h"
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "soc/uhci_reg.h"
#include "soc/uhci_struct.h"
#include "driver/periph_ctrl.h"
#include "freertos/ringbuf.h"

/**
 ****************************************************************************************
 * @brief UHCI numbers,Total 2 UHCI numbers.
 *
 ****************************************************************************************
 */
enum uc_uhci_dir_t
{
	uC_UHCI_DIR_FROM_PERIPH,				///< UHCI direction from peripheral.
	uC_UHCI_DIR_FROM_MEMORY					///< UHCI direction from Memory.
};

/**
 ****************************************************************************************
 * @brief UHCI memory Conditions
 *
 ****************************************************************************************
 */
enum uc_uhci_mem_inc_t
{
	uC_UHCI_MEM_INC_FALSE,					///< UHCI Memory true condition
	uC_UHCI_MEM_INC_TRUE					///< UHCI Memory false condition
};

/**
 ****************************************************************************************
 * @brief UHCI pheripheral Conditions
 *
 ****************************************************************************************
 */
enum uc_uhci_periph_inc_t
{
	uC_UHCI_PERIPH_INC_FALSE,				///< UHCI Peripheral true condition
	uC_UHCI_PERIPH_INC_TRUE					///< UHCI Peripheral false condition
};

/**
 ****************************************************************************************
 * @brief UHCI CIRC true or false conditions
 *
 ****************************************************************************************
 */
enum uc_uhci_circ_t
{
	uC_UHCI_CIRC_FALSE,						///< CIRC true condition
	uC_UHCI_CIRC_TRUE						///< CIRC false condition
};

/**
 ****************************************************************************************
 * @brief UHCI memory to memory true or false conditions
 *
 ****************************************************************************************
 */
enum uc_uhci_mem2mem_t
{
	uC_UHCI_MEM2MEM_FALSE,					///< Memory to memory true condition
	uC_UHCI_MEM2MEM_TRUE					///< Memory to memory false condition
};

/**
 ****************************************************************************************
 * @brief			Provides a uC_UHCI. Class Declaration
 ****************************************************************************************
 */
class uC_UHCI
{
	public:
		/**
		 * @brief							Constructor to create instance of uC_UHCI  class.
		 */
		uC_UHCI( uhci_num_t uhci_num, uint32_t uart_num );

		/**
		 * @brief							Destructor to delete the uC_UHCI instance
		 * @n								when it goes out of scope
		 * @param[in] void					none
		 */
		~uC_UHCI( void ) {}

		/**
		 * @brief							Callback types.  These are used by the object owner to define what
		 * @n								types of callback you want.  The status is the bitfield indicating what
		 * @n								status triggered the callback.  Be aware that the object owners callback
		 * @n								will likely be called from within an interrupt to keep things
		 * @n								simpler and quicker.
		 */
		typedef uint8_t cback_status_t;
		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param, cback_status_t status );

		/**
		 * @brief							Function to set the buffer to send and receive data
		 * @param[in] mem_ptr				Data buffer to send/receive (uint8_t)
		 * @param[in] link_type				Differentiate Transmit/receive link
		 * @param[in] item_count			Data Length
		 * @return							none
		 */
		void Set_Buff( uint8_t* mem_ptr, uhci_link_type_t link_type, uint16_t item_count );

		/**
		 * @brief							Function to set the buffer to send and receive data
		 * @param[in] mem_ptr				Data buffer to send/receive (uint16_t)
		 * @param[in] link_type				Differentiate Transmit/receive link
		 * @param[in] length				Data Length
		 * @return							none
		 */
		inline void Set_Buff( uint16_t* mem_ptr, uhci_link_type_t link_type, uint16_t length )
		{ Set_Buff( reinterpret_cast<uint8_t*>( mem_ptr ), link_type, length ); }

		/**
		 * @brief							Function to set the buffer to send and receive data
		 * @param[in] mem_ptr				Data buffer to send/receive (uint32_t)
		 * @param[in] link_type				Differentiate Transmit/receive link
		 * @param[in] length				Data Length
		 * @return							none
		 */
		inline void Set_Buff( uint32_t* mem_ptr, uhci_link_type_t link_type, uint16_t length )
		{ Set_Buff( reinterpret_cast<uint8_t*>( mem_ptr ), link_type, length ); }

		/**
		 * @brief							Function to Enable buffer end
		 * @param[in] void					void
		 * @return							None
		 */
		void Enable_End_Buff_Int( void ) const {}

		/**
		 * @brief							Function to disable buffer end
		 * @param[in] none					None
		 * @return							none
		 */
		void Disable_End_Buff_Int( void ) const {}

		/**
		 * @brief							Enable interrupt.
		 * @param[in] none					None
		 * @return							None
		 */
		void Enable_Int( void ) const;

		/**
		 * @brief							Disable interrupt.
		 * @param[in] none					None
		 * @return							None
		 */
		void Disable_Int( void ) const;

		/**
		 * @brief							Clear interrupt.
		 * @param[in] uhci_num				Uhci number
		 * @return							None
		 */
		static void Clear_Int( uhci_num_t uhci_num );

		/**
		 * @brief							Deallocate DMA Stream;
		 * @param[in] dma_channel			DMA channel information
		 * @return							None
		 */
		static void Deallocate_DMA_Stream( uint8_t dma_channel ) {}

		/**
		 * @brief							Enable DMA link for transmit\receive.
		 * @param[in] link_type				Differentiate Transmit/receive link
		 * @return							None
		 */
		void Enable( uhci_link_type_t link_type );

		/**
		 * @brief							Disable DMA link for transmit\receive.
		 * @param[in] link_type				Differentiate Transmit/receive link
		 * @return							None
		 */
		void Disable( uhci_link_type_t link_type );

		/**
		 * @brief                                       Configures the callback.  You pass in the function and parameter
		 * @n											you want to be called
		 * @n											as well as the reason(s) you want to be called.
		 * @param[in] cback_func                        The actual function.
		 * @param[in] cback_param                       Typically the "this" pointer.  Can be null.
		 * @param[in] cback_request					    The bit field indicating what reasons you want to be called.
		 * @return[out]									none
		 */
		void Config_Callback( cback_func_t cback_func,
							  cback_param_t cback_param, cback_status_t cback_request );

	private:

		/**
		 * @brief							 pointer to Base UHCI structure for DMA interface.
		 */
		uC_BASE_UHCI_STRUCT* m_udma;

		/**
		 * @brief							Function to attach UHCI port
		 * @param[in] uhci_num				UHCI number
		 * @param[in] uart_num				UART hardware interface number
		 * @return esp_err_t				esp error type
		 */
		esp_err_t Uhci_Attach_Uart( uhci_num_t uhci_num, uint32_t uart_num );

		/**
		 * @brief							Function to set  UHCI link address for receive/transmit
		 * @param[in] uhci_num				UHCI number
		 * @param[in] link_type				Differentiate Transmit/receive link
		 * @param[in] link_addr				UHCi link address
		 * @return esp_err_t				esp error type
		 */
		esp_err_t Uhci_Set_Link_Addr( uhci_num_t uhci_num, uhci_link_type_t link_type, uint32_t link_addr );

		/**
		 * @brief							Function to Restart  UHCI link for receive/transmit
		 * @param[in] uhci_num				UHCI number
		 * @param[in] link_type				To Differentiate Transmit/receive link
		 * @return esp_err_t				esp error type
		 */
		static esp_err_t Uhci_Link_Restart( uhci_num_t uhci_num, uhci_link_type_t link_type );

		/**
		 * @brief							Function to check the UHCI
		 * @param[in] validation_result		true if the validation is correct or else false will be sent
		 * @param[in] str					UHCI string used for displaying the error message
		 * @param[in] error_type			esp Error type
		 * @return esp_err_t				esp error type
		 */
		static esp_err_t Check_UHCI( bool validation_result, const char* str, esp_err_t error_type );

		/**
		 * @brief							Function to handle the uhci interrupt
		 * @param[in] parm					Interrupt function pointer
		 * @return void * Param				Parameters for interrupt
		 */
		static void Uhci_Intr_Handle( void* parm );

		/**
		 * @brief							Function to register the uhci interrupt
		 * @param[in] uhci_num				UHCI number
		 * @param[in] fun					call back function for interrupt
		 * @param[in] intr_alloc_flags		interrupt allocation flags
		 * @param[in] handle				interrupt Handler
		 * @return esp_err_t				esp error type
		 */
		esp_err_t Uhci_Isr_Register( uhci_num_t uhci_num, void ( * fun )(
										 void* ), void* arg, int intr_alloc_flags, uhci_isr_handle_t* handle );

		/**
		 * @brief							Callback Status bits.  The following will be used as bits and this is
		 * @n								why they are not a type.
		 */
		static const cback_status_t RX_BYTE_TIMEOUT = 0U;						///< This is triggered by a bus quiet
																				///< time.
		static const cback_status_t RX_BUFF_FULL = 1U;							///< The passed in array has hit the end
																				///< of the
		///< buffer.
		static const cback_status_t TX_BUFF_EMPTY = 2U;							///< The data buffer is empty and ready
																				///< to transmit
		///< again.
		static const cback_status_t TX_COMPLETE = 3U;							///< The shift register is empty and the
																				///< wire
		///< should be clear.
		static const cback_status_t TXRX_HOLDOFF_COMPLETE = 4U;					///< Half duplex holdoff.  RX resets
																				///< holdoff.
		///< This will override TX_HOLDOFF_COMPLETE.
		static const cback_status_t TX_HOLDOFF_COMPLETE = 5U;					///< We only monitor the TX for holdoff.

		/**
		 * @brief											uC_UHCI Private member variables
		 * @n												Call-Back status request variable
		 * @n												Call-back function pointer .
		 * @n												Call-back param
		 */
		static cback_status_t m_cback_request;
		static cback_func_t m_cback_func;
		static cback_param_t m_cback_param;

};

#endif
