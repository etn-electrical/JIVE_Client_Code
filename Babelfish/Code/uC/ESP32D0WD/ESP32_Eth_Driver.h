/**
 **********************************************************************************************
 * @file           ESP32_Eth_Driver.h Header File for interface class.
 *
 * @brief           The file contains ESP32_Eth_Driver.h, an interface class required
 *
 * @details
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#ifndef ESP32_ETH_DRIVER_H
#define ESP32_ETH_DRIVER_H

#include "OS_Tasker.h"
#include "freertos/semphr.h"

#include "Includes.h"
#include "EMAC_Driver.h"
#include "esp_err.h"
#include "hal/emac.h"
#include "hal/gpio_types.h"
#include "esp_eth_com.h"
#include "esp_eth.h"
#include "esp_intr_alloc.h"
#include "uC_Base.h"

static const uint8_t MAC_BUFFER_SIZE = 6U;

typedef enum
{
	RESET = 0U,
	SET = !RESET
} Flag_Status, IT_Status;

/**
 *****************************************************************************************
 * @brief    The ESP32 microcontroller specific class for ethernet EMAC.
 *           This class shall derive the basic interfaces from the abstract class which
 *           EMAC_Driver.
 *****************************************************************************************
 */
class ESP32_Eth_Driver : public EMAC_Driver
{
	public:
		/**
		 * @brief                -  Constructs a ESP32_Eth_Driver object.
		 * @param mac_config    -  The MAC_Config for MAC layer configuration
		 * @param mac_address    -  MAC address of the emac interface
		 * @param emac_dcids     -  The structure of DCIDs. This structure shall contain
		 *                          DCIDs for setting up ethernet emac configuration.
		 *                          e.g. multicast/broadcast settings etc.
		 */
		ESP32_Eth_Driver( eth_mac_config_t mac_config, uint8_t* mac_address, dcid_emac_params_t const* emac_dcids );


		/**
		 * @brief                -  Destructor to delete an instance of ESP32_Eth_Driver
		 *                          class when object goes out of scope.
		 */
		virtual ~ESP32_Eth_Driver( void );

		/**
		 * @brief                -  Initializes the descriptors of ethernet EMAC
		 *                          module. Configures the Tx and Rx memory memory
		 *                          space in descriptors. The Tx?Rx memory space is
		 *                          received from ethernetif.
		 * @param Tx_Buff        -  The RAM address from where data shall be read
		 *                          by EMAC and transmitted over ethernet
		 * @param rx_payload     -  The address pointer array. The received data packets
		 *                          shall be stored at this addresses by EMAC DMA.
		 * @param rx_buffer_size -  The received packet buffer size
		 */
		void Init_Buff_Desc( uint8_t* Tx_Buff, uint32_t** rx_payload, uint16_t rx_buffer_size );

		/**
		 * @brief                -  The function updates the speed and mode of
		 *                          ethernet communication. LAN driver may call this
		 *                          function for updating EMAC.
		 * @param speed          -  The speed of ethernet communication. Possible values
		 * @n                       1 - 10MBps
		 * @n                       2 - 100MBps
		 * @param duplex          - The mode of communication. Possible values
		 * @n                       0 - Half Duplex
		 * @n                       1 - Full duplex
		 */
		void Update_PHY_Config_To_MAC( uint8_t speed, uint8_t duplex );

		/**
		 * @brief                -  The function is used to pass the ethernet packet
		 *                          to application layer(ethernetif).The function returns
		 *                          the state of the packet to the ethernetif.
		 * @param new_payload    -  The RAM address at which ethernet packet is received
		 *                          over a network. The ethernet if layer shall copy the
		 *                          packet from this address
		 * @param pkt_index      -  The packet index
		 * @return data_states_t -  The function returns the various states signifying the
		 *                          health and state of packet. Possible values are-
		 * @n                       INIT  - The frame is not recived yet. EMAC is in
		 *                                  initialization state
		 * @n                       START - First packet of multipacket frame received
		 * @n                       SINGLE_PKT_FRM  - The packet received is single packet
		 *                                             frame.
		 * @n                       INTERIM - The packet recived is intermediate packet.
		 * @n                       ERROR_STATE  - The recievd packet is corrupt. Err
		 * @n                       LAST - The recived packet is the last packet of
		 *                                 the data frame
		 */
		data_states_t Get_Next_Rx_Pkt( uint32_t* new_payload, uint8_t* pkt_index );

		/**
		 * @brief                -  The function returns the RAM address of descriptor.
		 *                          The upper layer(ethrnetif) should write/copy the
		 *                          data to be transmitted at this location.
		 * @return uint8_t*      -  Memory address of theavailable descriptor at which
		 *                          data shall be written/copied for transmission
		 *                          purpose.
		 */
		uint8_t* Get_Next_Avail_Desc_Buff( void );

		/**
		 * @brief                -  Wait while EMAC prepares itself for next transmission
		 *                          purpose.
		 */
		void Wait_Tx_Ready( void );

		/**
		 * @brief                -  The function transmits MAC frame
		 * @param[in] l              -  Length of data to be transmitted
		 */
		void Tx_Frame( uint32_t l );

		/**
		 * @brief                -  The function returns the configured MAC address
		 *                          of the EMAC hardware.
		 * @param Addr           -  Pointer to MAC address location
		 */
		void Get_MAC_Address( uint8_t* Addr ) const;

		/**
		 * @brief                   This function reads 16-bit register content from the PHY
		 * @param[in] phy_addr      PHY Port Number .
		 * @n                       For single port PHY it is also the Default PHY address.
		 * @param[in] reg_addr      PHY Register number / offset .
		 * @n
		 * @param[in] read_ptr      The register value read from PHY is copied into the read pointer
		 *.
		 * @n
		 */
		BOOL SMIRead16Bits( uint16_t phy_addr, uint16_t reg_addr,
							uint16_t* read_ptr );

		/**
		 * @brief                   This function writes 16-bit value to specified register in PHY
		 * @param[in] phy_addr      PHY Port Number .
		 * @n                       For single port PHY it is also the Default PHY address.
		 * @param[in] reg_addr      PHY Register number / offset .
		 * @n
		 * @param[in] write_ptr     The value to be written to PHY register
		 * @n
		 */
		BOOL SMIWrite16Bits( uint16_t phy_addr, uint16_t reg_addr,
							 const uint16_t* write_ptr );

		/**
		 * @brief                   This function reads 32-bit value from the PHY register.
		 * @n                       Calls  Higher & Lower 16 bit read functions to read 32bit value
		 * @param[in] sys_reg_addr  PHY Register address .
		 * @n
		 * @param[in] read_ptr      The register value read from PHY is copied into the read pointer
		 *.
		 * @n
		 */
		BOOL SMIRead32Bits( uint16_t sys_reg_addr, uint32_t* read_Ptr );

		/**
		 * @brief                   This function writes 32-bit value from the PHY register.
		 * @n                       Calls  Higher & Lower 16 bit write functions to write 32-bit
		 * value
		 * @param[in] sys_reg_addr  PHY System Register address .
		 * @n
		 * @param[in] write_ptr     The value to be written to the  PHY register
		 * @n
		 */
		BOOL SMIWrite32Bits( uint16_t sys_reg_addr,
							 const uint32_t* write_ptr );

		/**
		 * @brief                -  Enables transmit and receive state machine of MAC.
		 *                          Starts DMAC transmission and reception.
		 */
		void Init_EMAC_Hardware( void );

		/**
		 * @brief                -  Clears DMA interrupt pending bits.
		 * @param  ETH_DMA_IT    -  The uint32 type. Set the bit loaction of DMA interrupts
		 *                          which you want to clear.
		 */
		void ETH_DMA_Clear_IT_Pending_Bit( uint32_t ETH_DMA_IT );

		/**
		 * @brief                -  Configures MAC address of the EMAC
		 * @param  MacAddr       -  EMAC supports 4 different MAC addresss. User can configure
		 *                          the each MAC address using this MAC address offset parameter.
		 * @param Addr           -  pointer to location which holds MAC address to be configures.
		 */
		void ETH_MAC_Address_Config( uint32_t mac_addr, uint8_t* addr );

		/**
		 * @brief                -  Returns DMA flag status
		 * @param  ETH_DMA_FLAG  -  The DMA flag bit loaction to be checked
		 * @return Flag_Status    -  status of the flag if it is set or reset.
		 */
		Flag_Status ETH_Get_DMA_Flag_Status( uint8_t ETH_DMA_FLAG );

		void ETH_Handler( void );

		/**
		 * @brief                -  it basically initializes the ethernet driver and configures
		 *                          the MAC address.
		 * @param  mac_address       -  EMAC supports 4 different MAC addresss. User can configure
		 *                          the each MAC address using this MAC address offset parameter.
		 */
		void ETH_Driver_Init( uint8_t* mac_address );

		/**
		 * @brief                -  Function which is used to generate ethernet delay.
		 * @param nCount         -  Delay count
		 */
		static void ETH_Delay( uint32_t nCount );

		void Receive_Frame_Attach_FuncPtr( Receive_Frame_Func_Ptr Rx_Frame_fptr, void* netif );


		esp_err_t EMAC_Receive( uint8_t* buf, uint32_t* length );

		Receive_Frame_Func_Ptr* m_rx_frame_fptr;
		void* m_netif;
		emac_hal_context_t m_hal;
		bool m_isr_need_yield;
		static TaskHandle_t m_rx_task_hdl;
		uint32_t m_frames_remain;
		uint32_t m_free_desc;

	private:

		eth_mac_config_t m_mac_config;
		uint8_t m_addr[MAC_BUFFER_SIZE];
		uint8_t* m_rx_buf[CONFIG_ETH_DMA_RX_BUFFER_NUM];
		uint8_t* m_tx_buf[CONFIG_ETH_DMA_TX_BUFFER_NUM];
		intr_handle_t m_intr_hdl;

		/**
		 * @brief                -  Wrapper Function to Transmit Frame.
		 */
		esp_err_t EMAC_Transmit( uint8_t* buf, uint32_t length );

		/**
		 * @brief                -  Initialize SMI pins.
		 */
		void EMAC_Init_SMI_GPIO();

		/**
		 * @brief                -  Initialize ESP32 EMAC.
		 */
		esp_err_t EMAC_Init();

		/**
		 * @brief                -  Deinitialize ESP32 EMAC.
		 */
		esp_err_t EMAC_Deinit();

		/**
		 * @brief                -  Read PHY Register.
		 */
		esp_err_t EMAC_Read_PHY_Reg( uint32_t PHY_Addr, uint32_t PHY_Reg, uint32_t* Reg_Value );

		/**
		 * @brief                -  Write PHY Register.
		 */
		esp_err_t EMAC_Write_PHY_Reg( uint32_t PHY_Addr, uint32_t PHY_Reg, uint32_t Reg_Value );

		/**
		 * @brief                -  Set EMAC speed parameter.
		 */
		esp_err_t EMAC_Set_Speed( uint8_t Speed );

		/**
		 * @brief                -  Set EMAC communication mode parameter.
		 */
		esp_err_t EMAC_Set_Duplex( uint8_t Duplex );

		/**
		 * @brief                -  Get MAC Address.
		 */
		esp_err_t EMAC_Esp32_Get_Addr( uint8_t* Addr );

		/**
		 * @brief                -  Initializes DMA transmit Descriptor chain
		 */
		void ETH_DMA_Tx_Desc_Chain_Init( uint8_t* Tx_Buff, uint32_t Tx_Buff_Count );

		/**
		 * @brief                -  Initializes DMA receive Descriptor chain
		 */
		void ETH_DMA_Rx_Desc_Chain_Init( uint32_t**, uint16_t );

		/**
		 * @brief                -  Enables/Disables EMAC DMA transmission
		 */

		void ETH_DMA_Transmission_Cmd( FunctionalState New_State ) const;


		/**
		 * @brief                -  Enables/Disables EMAC DMA reception
		 */
		void ETH_DMA_Reception_Cmd( FunctionalState New_State ) const;

		/**
		 * @brief                -  Configures EMAC DMA interrupts
		 */
		void ETH_DMA_IT_Config( uint32_t ETH_DMA_IT, FunctionalState New_State ) const;

		/**
		 * @brief                -  Enables/Disbales MAC reception
		 */
		void ETH_MAC_Reception_Cmd( FunctionalState New_State ) const;

		/**
		 * @brief                -  Enables/Disbales MAC transmission
		 */
		void ETH_MAC_Transmission_Cmd( FunctionalState New_State ) const;

		/**
		 * @brief                -  Configures interrupts for DMA receive descriptors
		 */
		void ETH_DMA_Rx_Desc_Receive_IT_Config( eth_dma_rx_descriptor_t* DMA_Rx_Desc,
												FunctionalState New_State ) const;

		/**
		 * @brief                -  wait while receive data is not available
		 */
		bool Wait_Rx_Data_Available( void ) const;

		uint32_t emac_hal_transmit_frame( emac_hal_context_t* hal, uint8_t* buf, uint32_t length );

		eth_dma_tx_descriptor_t* DMA_Tx_Desc_To_Set;
		eth_dma_rx_descriptor_t* DMA_Rx_Desc_To_Get;
		eth_dma_rx_descriptor_t DMA_Rx_Dscr_Tab[EMAC_Driver::ETH_RX_BUFNB];
		eth_dma_tx_descriptor_t DMA_Tx_Dscr_Tab[EMAC_Driver::ETH_TX_BUFNB];

		static inline uint16_t SMI_SYS_REG_TO_LO_REG_ADDR( uint16_t addr )
		{
			return ( static_cast<uint16_t>( ( ( ( addr ) >> 1U ) & 0x1EU ) ) );
		}

		static inline uint16_t SMI_SYS_REG_TO_HI_REG_ADDR( uint16_t addr )
		{
			return ( static_cast<uint16_t>( ( ( ( ( addr ) >> 1U ) & 0x1EU ) | 1U ) ) );
		}

		static inline uint16_t SMI_VALID_SYS_REG_ADDR( uint16_t addr )
		{
			return ( static_cast<uint16_t>( ( ( ( addr ) & SMI_SYS_REG_ADDR_MASK ) == 0U ) ) );
		}

		static inline uint16_t SMI_SYS_REG_TO_PHY_ADDR( uint16_t addr )
		{
			return ( static_cast<uint16_t>( ( 0x10U | ( ( ( ( addr ) & 0x3FFU ) ) >> 6U ) ) ) );
		}

		static const uint16_t SMI_SYS_REG_ADDR_MASK = 0xFC03U;

		/** @brief Ethernet MAC address 0 offset */
		static const uint32_t ETH_MAC_Address_0 = 0x00000000U;
};

#endif	// ESP32_ETH_DRIVER_H
