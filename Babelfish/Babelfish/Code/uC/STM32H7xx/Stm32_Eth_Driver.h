/**
 **********************************************************************************************
 * @file           Stm32_Eth_Driver.h Header File for interface class.
 *
 * @brief           The file contains Stm32_Eth_Driver.h, an interface class required
 *
 * @details
 * @copyright       2020 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#if !defined( _STM32_ETH_H )
#define _STM32_ETH_H

#include "Includes.h"
#include "uC_Interrupt.h"
#include "uC_OS_Interrupt.h"
#include "uC_Base.h"
#include "uC_IO_Config.h"
#include "OS_Binary_Semaphore.h"
#include "Sys_arch.h"
#include "EMAC_Driver.h"
#include "Device_Config.h"
#include "stm32h7xx_hal_rcc.h"
#include "stm32h7xx_hal_eth.h"
#include "DCI_Owner.h"
#include "uC_RAM.h"

/** @defgroup PHY_Register_address
 * @{
 */
#define PHY_BCR     0 /*!< Tranceiver Basic Control Register */		// prem_recheck
#define PHY_BSR     1	/*!< Tranceiver Basic Status Register */

/**
 *****************************************************************************************
 * @brief    The STM32F207 microcontroller specific class for ethernet EMAC.
 *           This class shall derive the basic interfaces from the abstract class which
 *           EMAC_Driver.
 *****************************************************************************************
 */
class Stm32_Eth_Driver : public EMAC_Driver
{
	public:
		/**
		 * @brief                -  Constructs a Stm32_Eth_Driver object.
		 * @param emac_select    -  The emac channel of microcontroller to be used
		 * @param emac_interface -  EMAC interface of the MAC to be configured. Two
		 *                          possible values.
		 * @n                       uC_EMAC_MII_MODE - MII Mode
		 * @n                       uC_EMAC_RMII_MODE - RMII Mode
		 * @param mac_address    -  MAC address of the emac interface
		 * @param output_clock   -  Output clock from microcontroller to external
		 *                          PHY chip if required. Two possible values
		 * @n                       TRUE - Generate clock on MCO pin for PHY chip
		 * @n                       FALSE - Otherwise
		 * @param emac_dcids     -  The structure of DCIDs. This structure shall contain
		 *                          DCIDs for setting up ethernet emac configuration.
		 *                          e.g. multicast/broadcast settings etc.
		 */
		Stm32_Eth_Driver( uint8_t emac_select, uC_EMAC_INTERFACE_ETD emac_interface,
						  uint8_t* mac_address,
						  bool_t output_clock, dcid_emac_params_t const* emac_dcids,
						  uC_BASE_EMAC_IO_PORT_STRUCT const* io_port =
						  reinterpret_cast<uC_BASE_EMAC_IO_PORT_STRUCT const*>( nullptr ) );

		/**
		 * @brief                -  Destructor to delete an instance of Stm32_Eth_Driver
		 *                          class when object goes out of scope.
		 */
		~Stm32_Eth_Driver( void );

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
		bool_t SMIRead16Bits( uint16_t phy_addr, uint16_t reg_addr,
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
		bool_t SMIWrite16Bits( uint16_t phy_addr, uint16_t reg_addr,
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
		bool_t SMIRead32Bits( uint16_t sys_reg_addr, uint32_t* read_ptr );

		/**
		 * @brief                   This function writes 32-bit value from the PHY register.
		 * @n                       Calls  Higher & Lower 16 bit write functions to write 32-bit
		 * value
		 * @param[in] sys_reg_addr  PHY System Register address .
		 * @n
		 * @param[in] write_ptr     The value to be written to the  PHY register
		 * @n
		 */
		bool_t SMIWrite32Bits( uint16_t sys_reg_addr,
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
		static void ETH_DMAClearITPendingBit( uint32_t ETH_DMA_IT );

		/**
		 * @brief                -  Configures MAC address of the EMAC
		 * @param  MacAddr       -  EMAC supports 4 different MAC addresss. User can configure
		 *                          the each MAC address using this MAC address offset parameter.
		 * @param Addr           -  pointer to location which holds MAC address to be configures.
		 */
		static void ETH_MACAddressConfig( uint32_t MacAddr, uint8_t* Addr );

		/**
		 * @brief                -  Returns DMA flag status
		 * @param  ETH_DMA_FLAG  -  The DMA flag bit loaction to be checked
		 * @return FlagStatus    -  status of the flag if it is set or reset.
		 */
		static FlagStatus ETH_GetDMAFlagStatus( uint32_t ETH_DMA_FLAG );

		/**
		 * @brief                -  Ethernet handler function which is called when an ethernet
		 *                          receive interrupt occurs. It basically clears interrupts bits
		 *                          and wake up the task for data reception
		 * @param  ETH_DMA_FLAG  -  The DMA flag bit loaction to be checked
		 * @return FlagStatus    -  status of the flag if it is set or reset.
		 */
		static void ETH_Handler( void );

		/**
		 * @brief                -  it basically initializes the ethernet driver and configures
		 *                          the MAC address.
		 * @param  MacAddr       -  EMAC supports 4 different MAC addresss. User can configure
		 *                          the each MAC address using this MAC address offset parameter.
		 */
		static void ETHDriverInit( uint8_t* mac_address );

		/**
		 * @brief                -  Function which is used to generate ethernet delay.
		 * @param nCount         -  Delay count
		 */
		static void ETH_Delay( __IO uint32_t nCount );

		static OS_Binary_Semaphore* ETHRxBinSemaphore;

	private:

		/**
		 * @brief                -  Initializes ethernet EMAC hardware.
		 */
		void Init_Eth_Emac( uint8_t* mac_address, dcid_emac_params_t const* emac_dcids );

		/**
		 * @brief                -  Configures ethernet.
		 */
		void ETH_Config( void );

		/**
		 * @brief                -  Loads default communication parameters for ethernet
		 *                          communication
		 */
		void ETH_Load_Defaults( void );

		/**
		 * @brief                -  Initializes DMA transmit Descriptor chain
		 */
		void ETH_DMATxDescChainInit( uint8_t* TxBuff, uint32_t TxBuffCount );

		/**
		 * @brief                -  Initializes DMA receive Descriptor chain
		 */
		void ETH_DMARxDescChainInit( uint32_t**, uint16_t );

		/**
		 * @brief                -  Performs EMAC software reset
		 */
		void ETH_SoftwareReset( void ) const;

		/**
		 * @brief                -  Returns the status of EMAC software reset
		 */
		FlagStatus ETH_GetSoftwareResetStatus( void ) const;

		/**
		 * @brief                -  Clears transmit FIFO
		 */
		void ETH_FlushTransmitFIFO( void ) const;

		/**
		 * @brief                -  Enables/Disables EMAC DMA transmission
		 */
		void ETH_DMATransmissionCmd( FunctionalState NewState ) const;

		/**
		 * @brief                -  Enables/Disables EMAC DMA reception
		 */
		void ETH_DMAReceptionCmd( FunctionalState NewState ) const;

		/**
		 * @brief                -  Configures EMAC DMA interrupts
		 */
		void ETH_DMAITConfig( uint32_t ETH_DMA_IT, FunctionalState NewState ) const;

		/**
		 * @brief                -  Enables/Disbales MAC reception
		 */
		void ETH_MACReceptionCmd( FunctionalState NewState ) const;

		/**
		 * @brief                -  Enables/Disbales MAC transmission
		 */
		void ETH_MACTransmissionCmd( FunctionalState NewState ) const;

		/**
		 * @brief                -  Configures interrupts for DMA receive descriptors
		 */
		void ETH_DMARxDescReceiveITConfig( ETH_DMADescTypeDef* DMARxDesc,
										   FunctionalState NewState ) const;

		/**
		 * @brief                -  Ethernet Initialization
		 */
		void ETH_Init( void ) const;

		/**
		 * @brief                -  Ethernet DMA initialization
		 */
		void Config_Eth_DMA_Init( void ) const;

		/**
		 * @brief                -  wait while receive data is not available
		 */
		bool_t Wait_Rx_Data_Available( void ) const;

		/**
		 * @brief                -  Read MAC address registers
		 */
		void Read_MAC_Address_Reg( uint32_t MacAddrOffSet, uint8_t* Addr ) const;

		void ETH_DMATxDescChecksumInsertionConfig( ETH_DMADescTypeDef* DMATxDesc,
												   uint32_t DMATxDesc_Checksum );

		uC_BASE_EMAC_IO_STRUCT const* m_emac_pio;
		ETH_TypeDef* m_emac_ctrl;
		ETH_DMADescTypeDef* DMATxDescToSet;
		ETH_DMADescTypeDef* DMARxDescToGet;
		ETH_DMADescTypeDef DMARxDscrTab[EMAC_Driver::ETH_RX_BUFNB];
		ETH_DMADescTypeDef DMATxDscrTab[EMAC_Driver::ETH_TX_BUFNB];

		OS_Binary_Semaphore* m_ETHRxBinSemaphore;
		ETH_MACConfigTypeDef m_ethmac_init;
		ETH_DMAConfigTypeDef m_ethdma_init;
		ETH_MACFilterConfigTypeDef m_ethmac_filter_init;
		volatile_item_t volatile_item_DMATxDscrTab;
		volatile_item_t volatile_item_DMARxDscrTab;

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

		static const uint32_t ETH_MAC_US_TICK = 1000000U;
		static const uint32_t ETH_BLOCK_TIME_WAITING_FOR_INPUT_MS = 5000U;
		static const uint32_t ETH_SMI_TIMEOUT = 0x000FFFU;
		static const uint16_t SMI_SYS_REG_ADDR_MASK = 0xFC03U;

		static const uint32_t ETH_MACCR_MASK = 0xFFFB7F7CU;
		static const uint32_t ETH_MACECR_MASK = 0x3F077FFFU;
		static const uint32_t ETH_MACPFR_MASK = 0x800007FFU;
		static const uint32_t ETH_MACWTR_MASK = 0x0000010FU;
		static const uint32_t ETH_MACTFCR_MASK = 0xFFFF00F2U;
		static const uint32_t ETH_MACRFCR_MASK = 0x00000003U;
		static const uint32_t ETH_MTLTQOMR_MASK = 0x00000072U;
		static const uint32_t ETH_MTLRQOMR_MASK = 0x0000007BU;

		static const uint32_t ETH_DMAMR_MASK = 0x00007802U;
		static const uint32_t ETH_DMASBMR_MASK = 0x0000D001U;
		static const uint32_t ETH_DMACCR_MASK = 0x00013FFFU;
		static const uint32_t ETH_DMACTCR_MASK = 0x003F1010U;
		static const uint32_t ETH_DMACRCR_MASK = 0x803F0000U;
		static const uint32_t ETH_MACPCSR_MASK = ( ETH_MACPCSR_PWRDWN | ETH_MACPCSR_RWKPKTEN | \
												   ETH_MACPCSR_MGKPKTEN | ETH_MACPCSR_GLBLUCAST | \
												   ETH_MACPCSR_RWKPFE );

		/** @brief Ethernet ERROR */
		static const uint16_t ETH_ERROR = 0U;
		/** @brief NO Ethernet ERROR */
		static const uint16_t ETH_SUCCESS = 1U;
		/** @brief Ethernet MAC address 0 offset */
		static const uint32_t ETH_MAC_Address_0 = 0x00000000U;
		/** @brief Ethernet MAC address 1 offset */
		static const uint32_t ETH_MAC_Address_1 = 0x00000008U;
		/** @brief Ethernet MAC address 2 offset */
		static const uint32_t ETH_MAC_Address_2 = 0x00000010U;
		/** @brief Ethernet MAC address 3 offset */
		static const uint32_t ETH_MAC_Address_3 = 0x00000018U;
		/** @brief Ethernet interrupt priority */
		static const uint8_t uC_ETHERNET_PRIORITY = uC_OS_INT_PRIORITY_6;

		// prem_recheck
		/** @brief Maximum ethernetpacket size */
		static const uint16_t MAX_PACKET_SIZE = 1520U;
};

#endif	// _STM32_ETH_H
