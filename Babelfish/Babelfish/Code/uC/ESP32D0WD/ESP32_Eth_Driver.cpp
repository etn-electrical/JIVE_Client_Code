/**
 *****************************************************************************************
 * @file            ESP32_Eth_Driver.cpp Header File for interface class.
 *
 * @brief           The file contains ESP32_Eth_Driver.cpp, an interface class required
 *
 * @details
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 *****************************************************************************************
 */

#include "ESP32_Eth_Driver.h"
#include "esp_err.h"
#include "esp32/rom/ets_sys.h"
#include "hal/emac.h"
#include "driver/periph_ctrl.h"
#include "driver/gpio.h"
#include "uC_Base.h"
#include "OS_Tasker.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "Ethernet_Debug.h"
#include "Babelfish_Debug.h"

/*************************************************************
 * Function Declarations
 **************************************************************/
static IRAM_ATTR void EMAC_ISR_Handler( void* args );

static void EMAC_Rx_Task( void* arg );

/*************************************************************
 * Global Variables
 **************************************************************/
TaskHandle_t ESP32_Eth_Driver::m_rx_task_hdl = NULL;
extern uint8_t* Tx_Buf;
eth_dma_tx_descriptor_t* desc_iter = nullptr;
/*************************************************************
 * MACRO and Static Variables
 **************************************************************/
#define PHY_OPERATION_TIMEOUT_US ( DELAY_1000_MS )

static const uint16_t DELAY_1000_MS = 1000U;
static const uint8_t DELAY_100_MS = 100U;
static const uint8_t DELAY_10_MS = 10U;
static const uint8_t SPEED_10MBPS = 1U;
static const uint8_t SPEED_100MBPS = 2U;
static const uint8_t HALF_DUPLEX = 0U;
static const uint8_t FULL_DUPLEX = 1U;
static const uint8_t RESET_TIMEOUT_DIVISOR = 10U;
static const uint16_t STACK_DEPTH = 4096;
static const uint8_t MEM_ALLOC_NUM_1 = 1U;
static const uint8_t TASK_PRIORITY_19 = 19U;
static const uint16_t COUNTER_READ_MAX = 1000U;
static const uint16_t COUNTER_WRITE_MAX = 100U;
static const uint8_t TWO_BYTE = 16U;
static const uint8_t TX_WAIT_INIT = 0U;
static const uint8_t TX_WAIT_MAX = 90U;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
ESP32_Eth_Driver::ESP32_Eth_Driver( eth_mac_config_t mac_config, uint8_t* mac_address,
									dcid_emac_params_t const* emac_dcids ) :
	m_rx_frame_fptr( nullptr ),
	m_netif( nullptr ),
	m_isr_need_yield( false ),
	m_frames_remain( 0U ),
	m_free_desc( 0U ),
	m_mac_config( mac_config ),
	m_intr_hdl( nullptr ),
	DMA_Tx_Desc_To_Set( nullptr ),
	DMA_Rx_Desc_To_Get( nullptr )
{
	m_hal = { };
	m_addr[0] = 0;
	DMA_Tx_Dscr_Tab[0].Buffer1Addr = 0;
	DMA_Tx_Dscr_Tab[0].Buffer2NextDescAddr = 0;
	DMA_Tx_Dscr_Tab[0].Reserved1 = 0;
	DMA_Tx_Dscr_Tab[0].Reserved2 = 0;
	DMA_Tx_Dscr_Tab[0].TimeStampHigh = 0;
	DMA_Tx_Dscr_Tab[0].TimeStampLow = 0;
	DMA_Rx_Dscr_Tab[0].Buffer1Addr = 0;
	DMA_Rx_Dscr_Tab[0].Buffer2NextDescAddr = 0;
	DMA_Rx_Dscr_Tab[0].Reserved = 0;
	DMA_Rx_Dscr_Tab[0].TimeStampHigh = 0;
	DMA_Rx_Dscr_Tab[0].TimeStampLow = 0;

	esp_err_t ret_code = ESP_OK;
	void* descriptors = nullptr;

	if ( mac_address == nullptr )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Null mac_address pointer\n\r" );
	}
	else
	{
		// coverity[var_deref_model]
		memcpy( this->m_addr, mac_address, MAC_BUFFER_SIZE );
	}
	/* alloc memory for ethernet dma descriptor */
	uint32_t desc_size = CONFIG_ETH_DMA_RX_BUFFER_NUM * sizeof( eth_dma_rx_descriptor_t ) +
		CONFIG_ETH_DMA_TX_BUFFER_NUM * sizeof( eth_dma_tx_descriptor_t );

	descriptors = heap_caps_calloc( MEM_ALLOC_NUM_1, desc_size, MALLOC_CAP_DMA );
	if ( !( descriptors ) )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Calloc Descriptors Failed\n\r" );
	}
	int i = 0;

	/* alloc memory for ethernet dma buffer */
	for ( i = 0; i < CONFIG_ETH_DMA_RX_BUFFER_NUM; i++ )
	{
		this->m_rx_buf[i] = ( uint8_t* )heap_caps_calloc( MEM_ALLOC_NUM_1, CONFIG_ETH_DMA_BUFFER_SIZE,
														  MALLOC_CAP_DMA );
		if ( !( this->m_rx_buf[i] ) )
		{
			ETH_DEBUG_PRINT( DBG_MSG_ERROR,  "Rx_Buf : Heap_Caps_Calloc Error\r\n" );
		}
	}
	for ( i = 0; i < CONFIG_ETH_DMA_TX_BUFFER_NUM; i++ )
	{
		this->m_tx_buf[i] = ( uint8_t* )heap_caps_calloc( 1, CONFIG_ETH_DMA_BUFFER_SIZE, MALLOC_CAP_DMA );
		if ( !( this->m_tx_buf[i] ) )
		{
			ETH_DEBUG_PRINT( DBG_MSG_ERROR,  "Tx_Buf : Heap_Caps_Calloc Error\r\n" );
		}
	}

	/* initialize hal layer driver */
	emac_hal_init( &this->m_hal, descriptors, this->m_rx_buf, this->m_tx_buf );

	/* Interrupt configuration */
	ret_code = esp_intr_alloc( ETS_ETH_MAC_INTR_SOURCE, 0,
							   EMAC_ISR_Handler, this, &( this->m_intr_hdl ) );
	if ( ret_code != ESP_OK )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR,  "Alloc EMAC Interrupt Failed\r\n" );
	}

	/* create rx task */
	BaseType_t core_num = tskNO_AFFINITY;

	BaseType_t xReturned = xTaskCreatePinnedToCore( EMAC_Rx_Task, "emac_rx", STACK_DEPTH, this,
													TASK_PRIORITY_19, &ESP32_Eth_Driver::m_rx_task_hdl, core_num );

	if ( xReturned != pdPASS )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR,  "Create EMAC_Rx Task Failed\r\n" );
	}
}

void ESP32_Eth_Driver::EMAC_Init_SMI_GPIO()
{

	/* Setup SMI MDC GPIO */
	gpio_set_direction( ( gpio_num_t )m_mac_config.smi_mdc_gpio_num, GPIO_MODE_OUTPUT );
	gpio_matrix_out( ( gpio_num_t )m_mac_config.smi_mdc_gpio_num, EMAC_MDC_O_IDX, false, false );
	PIN_FUNC_SELECT( GPIO_PIN_MUX_REG[m_mac_config.smi_mdc_gpio_num], PIN_FUNC_GPIO );
	/* Setup SMI MDIO GPIO */
	gpio_set_direction( ( gpio_num_t )m_mac_config.smi_mdio_gpio_num, GPIO_MODE_INPUT_OUTPUT );
	gpio_matrix_out( ( gpio_num_t )m_mac_config.smi_mdio_gpio_num, EMAC_MDO_O_IDX, false, false );
	gpio_matrix_in( ( gpio_num_t )m_mac_config.smi_mdio_gpio_num, EMAC_MDI_I_IDX, false );
	PIN_FUNC_SELECT( GPIO_PIN_MUX_REG[m_mac_config.smi_mdio_gpio_num], PIN_FUNC_GPIO );
}

esp_err_t ESP32_Eth_Driver::EMAC_Init()
{
	/* enable peripheral clock */
	periph_module_enable( PERIPH_EMAC_MODULE );
	/* init clock, config gpio, etc */
	emac_hal_lowlevel_init( &this->m_hal );
	/* init gpio used by smi interface */
	EMAC_Init_SMI_GPIO();
	/* software reset */
	emac_hal_reset( &this->m_hal );
	uint32_t Counter = 0;

	for ( Counter = 0; Counter < this->m_mac_config.sw_reset_timeout_ms / RESET_TIMEOUT_DIVISOR; Counter++ )
	{
		if ( emac_hal_is_reset_done( &this->m_hal ) )
		{
			break;
		}
		vTaskDelay( pdMS_TO_TICKS( DELAY_10_MS ) );
	}
	if ( !( Counter < this->m_mac_config.sw_reset_timeout_ms / RESET_TIMEOUT_DIVISOR ) )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Reset Timeout\n\r" );
		return ( ESP_ERR_TIMEOUT );
	}

	/* set smi clock */
	emac_hal_set_csr_clock_range( &this->m_hal );
	/* reset descriptor chain */
	emac_hal_reset_desc_chain( &this->m_hal );
	/* init mac registers by default */
	emac_hal_init_mac_default( &this->m_hal );
	/* init dma registers by default */
	emac_hal_init_dma_default( &this->m_hal );
	/* get emac address from efuse */
	if ( esp_read_mac( this->m_addr, ESP_MAC_ETH ) != ESP_OK )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR, "Fetch Ethernet MAC Address Failed\n\r" );
		return ( ESP_FAIL );
	}
	/* set MAC address to emac register */
	emac_hal_set_address( &this->m_hal, this->m_addr );
#ifdef CONFIG_PM_ENABLE
	esp_pm_lock_acquire( this->pm_lock );
#endif
	return ( ESP_OK );
}

esp_err_t ESP32_Eth_Driver::EMAC_Deinit()
{
#ifdef CONFIG_PM_ENABLE
	esp_pm_lock_release( this->pm_lock );
#endif
	emac_hal_stop( &this->m_hal );
	periph_module_disable( PERIPH_EMAC_MODULE );
	return ( ESP_OK );
}

static IRAM_ATTR void EMAC_ISR_Handler( void* args )
{
	ESP32_Eth_Driver* obj = ( ESP32_Eth_Driver* ) args;

	emac_hal_isr( &obj->m_hal );
	if ( obj->m_isr_need_yield )
	{
		obj->m_isr_need_yield = false;
		portYIELD_FROM_ISR();
	}
}

IRAM_ATTR void emac_hal_rx_complete_cb( void* arg )
{
	ESP32_Eth_Driver* obj = ( ESP32_Eth_Driver* ) arg;

	BaseType_t high_task_wakeup;

	/* notify receive task */
	vTaskNotifyGiveFromISR( ESP32_Eth_Driver::m_rx_task_hdl, &high_task_wakeup );
	if ( high_task_wakeup == pdTRUE )
	{
		obj->m_isr_need_yield = true;
	}
}

IRAM_ATTR void emac_hal_rx_unavail_cb( void* arg )
{
	ESP32_Eth_Driver* obj = ( ESP32_Eth_Driver* ) arg;

	BaseType_t high_task_wakeup;

	/* notify receive task */
	vTaskNotifyGiveFromISR( ESP32_Eth_Driver::m_rx_task_hdl, &high_task_wakeup );
	if ( high_task_wakeup == pdTRUE )
	{
		obj->m_isr_need_yield = true;
	}
}

IRAM_ATTR void emac_hal_rx_early_cb( void* arg )
{
	ESP32_Eth_Driver* obj = ( ESP32_Eth_Driver* ) arg;

	BaseType_t high_task_wakeup;

	/* notify receive task */
	vTaskNotifyGiveFromISR( ESP32_Eth_Driver::m_rx_task_hdl, &high_task_wakeup );
	if ( high_task_wakeup == pdTRUE )
	{
		obj->m_isr_need_yield = true;
	}
}

static void EMAC_Rx_Task( void* arg )
{
	uint8_t* buffer = nullptr;
	uint32_t length = ETH_MAX_PACKET_SIZE;
	ESP32_Eth_Driver* obj = ( ESP32_Eth_Driver* )arg;

	// coverity[no_escape]
	while ( 1 )
	{
		// block indefinitely until got notification from underlay event
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
		do
		{
			length = ETH_MAX_PACKET_SIZE;
			/*
			 * Creating memory newly for new packet, in order to avoid buffer overwrite due to
			 * interrupt for new packet before earlier packet getting processed completely.
			 * This allocated memory is getting deallocated from Ethernet_Free_Rx_Buf_L2
			 * after data processing is completed.
			 */
			buffer = reinterpret_cast<uint8_t*>( heap_caps_calloc( MEM_ALLOC_NUM_1, length, MALLOC_CAP_8BIT ) );
			if ( buffer == nullptr )
			{
				ETH_DEBUG_PRINT( DBG_MSG_ERROR, "No Mem For Receive Buffer" );
			}
			else if ( obj->EMAC_Receive( buffer, &length ) == ESP_OK )
			{
				/* pass the buffer to stack (e.g. TCP/IP layer) */
				if ( length )
				{
					obj->m_rx_frame_fptr( obj->m_netif, buffer, length );
				}
				else
				{
					free( buffer );
				}
			}
			else
			{
				free( buffer );
			}
		} while ( obj->m_frames_remain > 0 );
	}
	vTaskDelete( NULL );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ESP32_Eth_Driver::Init_EMAC_Hardware( void )
{
	if ( EMAC_Init() != ESP_OK )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR,  "EMAC_Init() Error\r\n" );
	}

	/* Taking from emac_esp32_set_link function */
	if ( esp_intr_enable( this->m_intr_hdl ) != ESP_OK )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR,  "Enable Interrupt Failed\r\n" );
	}
	emac_hal_start( &this->m_hal );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ESP32_Eth_Driver::ETH_DMA_Rx_Desc_Receive_IT_Config( eth_dma_rx_descriptor_t* DMA_Rx_Desc,
														  FunctionalState New_State ) const
{
	if ( New_State != DISABLE )
	{
		/* Enable the DMA Rx Desc receive interrupt */
		DMA_Rx_Desc->RDES1.DisableInterruptOnComplete = RESET;
	}
	else
	{
		/* Disable the DMA Rx Desc receive interrupt */
		DMA_Rx_Desc->RDES1.DisableInterruptOnComplete = SET;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ESP32_Eth_Driver::Init_Buff_Desc( uint8_t* tx_buff, uint32_t** rx_payload,
									   uint16_t rx_buffer_size )
{
	ETH_DMA_Rx_Desc_Chain_Init( rx_payload, rx_buffer_size );
	ETH_DMA_Tx_Desc_Chain_Init( tx_buff, EMAC_Driver::ETH_TX_BUFNB );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ESP32_Eth_Driver::ETH_DMA_Rx_Desc_Chain_Init( uint32_t** rx_payload, uint16_t rx_buffer_size )
{
	for ( uint8_t idx = 0; idx < EMAC_Driver::ETH_RX_BUFNB; idx++ )
	{
		DMA_Rx_Desc_To_Get = &DMA_Rx_Dscr_Tab[idx];

		/* Set Buffer1 size and Second Address Chained bit */
		DMA_Rx_Desc_To_Get->RDES1.SecondAddressChained = SET;
		DMA_Rx_Desc_To_Get->RDES1.ReceiveBuffer1Size = ( static_cast<uint32_t>( rx_buffer_size ) );
		/* Set the Buffer 1 Address */
		DMA_Rx_Desc_To_Get->Buffer1Addr = ( reinterpret_cast<uint32_t>( rx_payload[idx] ) );

		/* Initialize the next descriptor with the Next Desciptor Polling Enable */
		if ( idx < ( EMAC_Driver::ETH_RX_BUFNB - 1U ) )
		{
			/* Set next descriptor address register with next descriptor base address */
			DMA_Rx_Desc_To_Get->Buffer2NextDescAddr = ( reinterpret_cast<uint32_t>( DMA_Rx_Dscr_Tab + idx + 1 ) );
		}
		else
		{
			/* For last descriptor, set next descriptor address register equal to the first
			   descriptor base address */
			DMA_Rx_Desc_To_Get->Buffer2NextDescAddr = ( reinterpret_cast<uint32_t>( DMA_Rx_Dscr_Tab ) );
		}

		/* Set Own bit of the Rx descriptor Status */
		DMA_Rx_Desc_To_Get->RDES0.Own = SET;
		ETH_DMA_Rx_Desc_Receive_IT_Config( &DMA_Rx_Dscr_Tab[idx], ENABLE );
		DMA_Rx_Desc_To_Get++;
	}

	/* At initial the current descriptor shall be at position reset or 0 */
	DMA_Rx_Desc_To_Get = &DMA_Rx_Dscr_Tab[RESET];
	/* Set Receive Desciptor List Address Register */
	this->m_hal.dma_regs->dmarxbaseaddr = ( reinterpret_cast<uint32_t>( DMA_Rx_Dscr_Tab ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ESP32_Eth_Driver::ETH_DMA_Tx_Desc_Chain_Init( uint8_t* tx_buff, uint32_t tx_buff_count )
{
	uint32_t count;
	eth_dma_tx_descriptor_t* DMA_Tx_Desc;

	/* Set the DMATxDescToSet pointer with the first one of the DMATxDescTab list */
	DMA_Tx_Desc_To_Set = DMA_Tx_Dscr_Tab;
	/* Fill each DMATxDesc descriptor with the right values */
	for ( count = 0U; count < tx_buff_count; count++ )
	{
		/* Get the pointer on the ith member of the Tx Desc list */
		DMA_Tx_Desc = DMA_Tx_Dscr_Tab + count;
		/* Set Second Address Chained bit */
		DMA_Tx_Desc->TDES0.SecondAddressChained = SET;

		/* Set Buffer1 address pointer */
		DMA_Tx_Desc->Buffer1Addr =
			reinterpret_cast<uint32_t>( ( &tx_buff[count * ETH_MAX_PACKET_SIZE] ) );

		/* Initialize the next descriptor with the Next Descriptor Polling Enable */
		if ( count < ( tx_buff_count - 1U ) )
		{
			/* Set next descriptor address register with next descriptor base address */
			DMA_Tx_Desc->Buffer2NextDescAddr =
				reinterpret_cast<uint32_t>( ( DMA_Tx_Dscr_Tab + count + 1U ) );
		}
		else
		{
			/* For last descriptor, set next descriptor address register equal to the first
			   descriptor base address */
			DMA_Tx_Desc->Buffer2NextDescAddr = reinterpret_cast<uint32_t>( DMA_Tx_Dscr_Tab );
		}
	}

	/* Set Transmit Desciptor List Address Register */
	this->m_hal.dma_regs->dmatxbaseaddr = reinterpret_cast<uint32_t>( DMA_Tx_Dscr_Tab );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
ESP32_Eth_Driver::~ESP32_Eth_Driver( void )
{
	if ( EMAC_Deinit() != ESP_OK )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR,  "Deinit MAC Failed\r\n" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ESP32_Eth_Driver::ETH_MAC_Address_Config( uint32_t mac_addr, uint8_t* addr )
{
	emac_hal_set_address( &this->m_hal, addr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void ESP32_Eth_Driver::Get_MAC_Address( uint8_t* addr ) const
{
	memcpy( addr, this->m_addr, MAC_BUFFER_SIZE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void ESP32_Eth_Driver::ETH_DMA_Clear_IT_Pending_Bit( uint32_t ETH_DMA_IT )
{
	m_hal.dma_regs->dmastatus.val = ETH_DMA_IT;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Flag_Status ESP32_Eth_Driver::ETH_Get_DMA_Flag_Status( uint8_t ETH_DMA_FLAG )
{
	Flag_Status bitstatus = RESET;

	if ( ( static_cast<bool>( ( m_hal.dma_regs->dmastatus.val & ETH_DMA_FLAG ) != RESET ) ) )
	{
		bitstatus = SET;
	}
	else
	{
		bitstatus = RESET;
	}
	return ( bitstatus );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ESP32_Eth_Driver::ETH_DMA_IT_Config( uint32_t ETH_DMA_IT, FunctionalState New_State ) const
{
	if ( New_State != DISABLE )
	{
		/* Enable the selected ETHERNET DMA interrupts */
		m_hal.dma_regs->dmain_en.val |= ETH_DMA_IT;
	}
	else
	{
		/* Disable the selected ETHERNET DMA interrupts */
		m_hal.dma_regs->dmain_en.val &= ( ~( static_cast<uint32_t>( ETH_DMA_IT ) ) );
	}
}

#ifndef USE_Delay
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ESP32_Eth_Driver::ETH_Delay( uint32_t nCount )
{
	for ( uint32_t Index = nCount; Index != RESET; Index-- )
	{}
}

#endif	/* USE_Delay*/

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t ESP32_Eth_Driver::EMAC_Read_PHY_Reg( uint32_t phy_addr, uint32_t phy_reg, uint32_t* reg_value )
{
	esp_err_t ret = ESP_OK;

	if ( !reg_value )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR,  "Can't Set Reg_Value To Null\r\n" );
		return ( ESP_ERR_INVALID_ARG );
	}
	emac_hal_set_phy_cmd( &this->m_hal, phy_addr, phy_reg, false );
	/* polling the busy flag */
	uint32_t Counter = 0;
	bool busy;

	do
	{
		ets_delay_us( DELAY_1000_MS );
		busy = emac_hal_is_mii_busy( &this->m_hal );
		Counter += COUNTER_READ_MAX;
	} while ( busy && Counter < PHY_OPERATION_TIMEOUT_US );
	if ( busy )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR,  "PHY is Busy\r\n" );
		return ( ESP_ERR_TIMEOUT );
	}
	/* Store value */
	*reg_value = emac_hal_get_phy_data( &this->m_hal );
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t ESP32_Eth_Driver::EMAC_Write_PHY_Reg( uint32_t phy_addr, uint32_t phy_reg, uint32_t reg_value )
{
	esp_err_t ret = ESP_OK;

	if ( emac_hal_is_mii_busy( &this->m_hal ) )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR,  "PHY Is Busy\r\n" );
		ret = ESP_ERR_INVALID_STATE;
	}

	emac_hal_set_phy_data( &this->m_hal, reg_value );
	emac_hal_set_phy_cmd( &this->m_hal, phy_addr, phy_reg, true );
	/* polling the busy flag */
	uint32_t Counter = 0;
	bool busy;

	do
	{
		ets_delay_us( DELAY_100_MS );
		busy = emac_hal_is_mii_busy( &this->m_hal );
		Counter += COUNTER_WRITE_MAX;
	} while ( busy && Counter < PHY_OPERATION_TIMEOUT_US );
	if ( busy )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR,  "PHY Is Busy\r\n" );
		ret = ESP_ERR_TIMEOUT;
	}
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
BOOL ESP32_Eth_Driver::SMIRead16Bits( uint16_t phy_addr, uint16_t reg_addr, uint16_t* read_ptr )
{
	esp_err_t esp_ret;
	bool ret = false;

	esp_ret = EMAC_Read_PHY_Reg( ( uint32_t ) phy_addr, ( uint32_t ) reg_addr, ( uint32_t* ) read_ptr );
	if ( esp_ret == ESP_OK )
	{
		ret = true;
	}
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
BOOL ESP32_Eth_Driver::SMIWrite16Bits( uint16_t phy_addr, uint16_t reg_addr, const uint16_t* write_ptr )
{
	esp_err_t esp_ret;
	bool ret = false;

	esp_ret = EMAC_Write_PHY_Reg( ( uint32_t )phy_addr, ( uint32_t )reg_addr, ( uint32_t )*write_ptr );
	if ( esp_ret == ESP_OK )
	{
		ret = true;
	}
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
BOOL ESP32_Eth_Driver::SMIRead32Bits( uint16_t sys_reg_addr, uint32_t* read_ptr )
{
	uint16_t lo_word = 0xFFFFU;
	uint16_t hi_word = 0xFFFFU;

	bool_t success = false;

	bool lo_success = SMIRead16Bits( SMI_SYS_REG_TO_PHY_ADDR( sys_reg_addr ),
									 SMI_SYS_REG_TO_LO_REG_ADDR( sys_reg_addr ),
									 &lo_word );

	bool hi_success = SMIRead16Bits( SMI_SYS_REG_TO_PHY_ADDR( sys_reg_addr ),
									 SMI_SYS_REG_TO_HI_REG_ADDR( sys_reg_addr ),
									 &hi_word );

	if ( lo_success && hi_success )
	{
		*read_ptr = ( ( static_cast<uint32_t>( hi_word ) ) << 16U )
			| ( static_cast<uint32_t>( lo_word ) );
		success = true;
	}

	return ( static_cast<bool>( success ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
BOOL ESP32_Eth_Driver::SMIWrite32Bits( uint16_t sys_reg_addr, const uint32_t* write_ptr )
{
	bool success = false;
	uint16_t lo_word = static_cast<uint16_t>( ( ( *write_ptr ) & 0xFFFFU ) );
	uint16_t hi_word = static_cast<uint16_t>( ( ( *write_ptr ) >> TWO_BYTE ) );

	bool lo_success = SMIWrite16Bits( SMI_SYS_REG_TO_PHY_ADDR( sys_reg_addr ),
									  SMI_SYS_REG_TO_LO_REG_ADDR( sys_reg_addr ),
									  &lo_word );

	bool hi_success = SMIWrite16Bits( SMI_SYS_REG_TO_PHY_ADDR( sys_reg_addr ),
									  SMI_SYS_REG_TO_HI_REG_ADDR( sys_reg_addr ),
									  &hi_word );

	if ( lo_success && hi_success )
	{
		success = true;
	}
	return ( static_cast<bool>( success ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* ESP32_Eth_Driver::Get_Next_Avail_Desc_Buff( void )
{
	return ( reinterpret_cast<uint8_t*>( desc_iter->Buffer1Addr ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ESP32_Eth_Driver::Wait_Tx_Ready( void )
{
	uint32_t ulAttempts = TX_WAIT_INIT;

	while ( static_cast<bool>( ( DMA_Tx_Desc_To_Set->TDES0.Own ) != RESET ) )
	{
		/* Return ERROR: OWN bit set */
		OS_Tasker::Delay( 1U );

		ulAttempts++;

		if ( ulAttempts > TX_WAIT_MAX )
		{
			DMA_Tx_Desc_To_Set->TDES0.Own = 0;
			break;
		}

	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ESP32_Eth_Driver::Tx_Frame( uint32_t Length )
{
	ESP32_Eth_Driver::emac_hal_transmit_frame( &this->m_hal, Tx_Buf, Length );
}

esp_err_t ESP32_Eth_Driver::EMAC_Set_Speed( uint8_t speed )
{
	esp_err_t ret = ESP_OK;

	switch ( speed )
	{
		case SPEED_10MBPS:
			emac_hal_set_speed( &this->m_hal, EMAC_SPEED_10M );
			ETH_DEBUG_PRINT( DBG_MSG_DEBUG,  "Working In 10Mbps\r\n" );
			break;

		case SPEED_100MBPS:
			emac_hal_set_speed( &this->m_hal, EMAC_SPEED_100M );
			ETH_DEBUG_PRINT( DBG_MSG_DEBUG,  "Working In 100Mbps\r\n" );
			break;

		default:
			ETH_DEBUG_PRINT( DBG_MSG_DEBUG,  "Unknown Speed\r\n" );
			ret = ESP_ERR_INVALID_ARG;
			break;
	}
	return ( ret );
}

esp_err_t ESP32_Eth_Driver::EMAC_Set_Duplex( uint8_t duplex )
{
	esp_err_t ret = ESP_OK;

	switch ( duplex )
	{
		case HALF_DUPLEX:
			emac_hal_set_duplex( &this->m_hal, EMAC_DUPLEX_HALF );
			ETH_DEBUG_PRINT( DBG_MSG_DEBUG,  "Working In Half Duplex\r\n" );
			break;

		case FULL_DUPLEX:
			emac_hal_set_duplex( &this->m_hal, EMAC_DUPLEX_FULL );
			ETH_DEBUG_PRINT( DBG_MSG_DEBUG,  "Working In Full Duplex\r\n" );
			break;

		default:
			ETH_DEBUG_PRINT( DBG_MSG_DEBUG,  "Unknown  Mode\r\n" );
			ret = ESP_ERR_INVALID_ARG;
			break;
	}
	return ( ret );
}

// coverity[no_escape]
esp_err_t ESP32_Eth_Driver::EMAC_Receive( uint8_t* buf, uint32_t* length )
{
	esp_err_t ret = ESP_OK;

	if ( length != NULL )
	{
		uint32_t expected_len = *length;

		if ( !( buf && length ) )
		{
			ETH_DEBUG_PRINT( DBG_MSG_ERROR,  "Can't Set Buf And Length To Null\r\n" );
			ret = ESP_ERR_INVALID_ARG;
		}
		uint32_t receive_len = emac_hal_receive_frame( &this->m_hal, buf, expected_len, &this->m_frames_remain,
													   &this->m_free_desc );

		/* We need to check the return value in case the buffer size is not enough */
		if ( !( expected_len >= receive_len ) )
		{
			ETH_DEBUG_PRINT( DBG_MSG_ERROR,  "Received Buffer Longer Than Expected\r\n" );
			ret = ESP_ERR_INVALID_SIZE;
		}
		*length = receive_len;
	}
	return ( ret );
}

void ESP32_Eth_Driver::Receive_Frame_Attach_FuncPtr( Receive_Frame_Func_Ptr* rx_frame_fptr, void* net_if )
{
	this->m_rx_frame_fptr = rx_frame_fptr;
	this->m_netif = net_if;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void ESP32_Eth_Driver::Update_PHY_Config_To_MAC( uint8_t speed, uint8_t duplex )
{
	if ( EMAC_Set_Speed( ( uint8_t ) speed ) != ESP_OK )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR,  "Failed To Set Speed\r\n" );
	}
	if ( EMAC_Set_Duplex( ( uint8_t ) duplex ) != ESP_OK )
	{
		ETH_DEBUG_PRINT( DBG_MSG_ERROR,  "Failed To Set Mode\r\n" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool ESP32_Eth_Driver::Wait_Rx_Data_Available( void ) const
{
	return ( !( static_cast<bool>( ( DMA_Rx_Desc_To_Get->RDES0.Own ) != RESET ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
data_states_t ESP32_Eth_Driver::Get_Next_Rx_Pkt( uint32_t* new_payload, uint8_t* pkt_index )
{
	static data_states_t statemachine = INIT;
	data_states_t return_value;
	static uint8_t dma_desc_num = 0U;
	bool error;
	bool fs_bit;
	bool ls_bit;
	bool dma_suspend;
	static bool first_packet_received = false;

	// block indefinitely until got notification from underlay event
	ulTaskNotifyTake( pdTRUE, portMAX_DELAY );

	fs_bit = static_cast<bool>( DMA_Rx_Desc_To_Get->RDES0.FirstDescriptor );
	ls_bit = static_cast<bool>( DMA_Rx_Desc_To_Get->RDES0.LastDescriptor );

	error =
		( ( ( static_cast<bool>( ( DMA_Rx_Desc_To_Get->RDES0.ErrSummary ) ) ) != RESET ) ||
		  ( !first_packet_received && !fs_bit ) || ( fs_bit && first_packet_received ) ||
		  ( new_payload == _NULL ) );

	dma_suspend = static_cast<bool>( ( this->m_hal.dma_regs->dmastatus.recv_buf_unavail ) != RESET );


	DMA_Rx_Desc_To_Get->RDES0.Own = SET;
	if ( dma_suspend )
	{
		/* Clear RBUS ETHERNET DMA flag */
		this->m_hal.dma_regs->dmastatus.recv_buf_unavail = SET;
		/* Resume DMA reception */
		this->m_hal.dma_regs->dmarxpolldemand = RESET;
	}
	if ( _NULL != new_payload )
	{
		DMA_Rx_Desc_To_Get->Buffer1Addr = reinterpret_cast<uint32_t>( new_payload );
	}
	DMA_Rx_Desc_To_Get = reinterpret_cast<eth_dma_rx_descriptor_t*>( DMA_Rx_Desc_To_Get->Buffer2NextDescAddr );

	if ( error )
	{
		statemachine = ERROR_STATE;
		first_packet_received = false;
	}
	else
	{
		if ( first_packet_received )
		{
			if ( ls_bit )
			{
				statemachine = LAST;
				first_packet_received = false;
			}
			else
			{
				statemachine = INTERIM;
			}
		}
		else
		{
			if ( fs_bit )
			{
				statemachine = START;
				first_packet_received = true;
				if ( ls_bit )
				{
					statemachine = SINGLE_PKT_FRM;
					first_packet_received = false;
				}
			}
		}
	}

	/* To keep the track of current DMA descriptor */
	dma_desc_num++;
	/* DAM descriptor range spans from 0-9 instead of 1-10*/
	*pkt_index = ( dma_desc_num - 1U );
	if ( EMAC_Driver::ETH_RX_BUFNB == dma_desc_num )
	{
		dma_desc_num = 0U;
		/* Reset the dma descriptor number as descriptor are chained in ring */
	}

	return_value = statemachine;
	if ( ( ERROR_STATE == statemachine ) || ( LAST == statemachine ) ||
		 ( SINGLE_PKT_FRM == statemachine ) )
	{
		statemachine = INIT;
	}

	return ( return_value );
}

uint32_t ESP32_Eth_Driver::emac_hal_transmit_frame( emac_hal_context_t* hal, uint8_t* buf, uint32_t length )
{
	/* Get the number of Tx buffers to use for the frame */
	uint32_t bufcount = 0;
	uint32_t lastlen = length;
	uint32_t sentout = 0;

	while ( lastlen > CONFIG_ETH_DMA_BUFFER_SIZE )
	{
		lastlen -= CONFIG_ETH_DMA_BUFFER_SIZE;
		bufcount++;

	}
	if ( lastlen )
	{
		bufcount++;
	}
	if ( bufcount > CONFIG_ETH_DMA_TX_BUFFER_NUM )
	{}

	desc_iter = hal->tx_desc;
	/* A frame is transmitted in multiple descriptor */
	for ( int i = 0; i < bufcount; i++ )
	{
		/* Check if the descriptor is owned by the Ethernet DMA (when 1) or CPU (when 0) */
		if ( desc_iter->TDES0.Own != EMAC_DMADESC_OWNER_CPU )
		{
			Wait_Tx_Ready();
		}
		/* Clear FIRST and LAST segment bits */
		desc_iter->TDES0.FirstSegment = 0;
		desc_iter->TDES0.LastSegment = 0;
		desc_iter->TDES0.InterruptOnComplete = 0;
		if ( i == 0 )
		{
			/* Setting the first segment bit */
			desc_iter->TDES0.FirstSegment = 1;
		}
		if ( i == ( bufcount - 1 ) )
		{
			/* Setting the last segment bit */
			desc_iter->TDES0.LastSegment = 1;
			/* Enable transmit interrupt */
			desc_iter->TDES0.InterruptOnComplete = 1;
			/* Program size */
			desc_iter->TDES1.TransmitBuffer1Size = lastlen;
			/* copy data from uplayer stack buffer */
			memcpy( Get_Next_Avail_Desc_Buff(), buf + i * CONFIG_ETH_DMA_BUFFER_SIZE, lastlen );
			sentout += lastlen;
		}
		else
		{
			/* Program size */
			desc_iter->TDES1.TransmitBuffer1Size = CONFIG_ETH_DMA_BUFFER_SIZE;
			/* copy data from uplayer stack buffer */
			memcpy( Get_Next_Avail_Desc_Buff(), buf + i * CONFIG_ETH_DMA_BUFFER_SIZE, CONFIG_ETH_DMA_BUFFER_SIZE );
			sentout += CONFIG_ETH_DMA_BUFFER_SIZE;
		}
		/* Point to next descriptor */
		desc_iter = ( eth_dma_tx_descriptor_t* )( desc_iter->Buffer2NextDescAddr );
	}

	/* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
	for ( int i = 0; i < bufcount; i++ )
	{
		hal->tx_desc->TDES0.Own = EMAC_DMADESC_OWNER_DMA;
		hal->tx_desc = ( eth_dma_tx_descriptor_t* )( hal->tx_desc->Buffer2NextDescAddr );
	}
	hal->dma_regs->dmatxpolldemand = 0;
	return ( sentout );

}
