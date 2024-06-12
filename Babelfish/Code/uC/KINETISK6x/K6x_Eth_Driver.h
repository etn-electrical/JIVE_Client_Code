/**
 **********************************************************************************************
 * @file           K6x_Eth_Driver.h Header File for interface class.
 *
 * @brief           The file contains K6x_Eth_Driver.h, an interface class required
 *
 * @details
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#if !defined( _K6x_ETH_DRIVER_H )
#define _K6x_ETH_DRIVER_H

#include "Includes.h"
#include "uC_Interrupt.h"
#include "uC_OS_Interrupt.h"
#include "uC_Base.h"
#include "uC_IO_Config.h"
#include "OS_Binary_Semaphore.h"
#include "Sys_arch.h"
#include "EMAC_Driver.h"
#include "Exception.h"
#include "Device_Config.h"
#include "DCI_Owner.h"
#include "uC_RAM.h"
#include "EthConfig.h"
#include "pbuf.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
// #include "netif/ppp_oe.h"
#include "netif.h"

/*******************************************************************/

#define ENET_LITTLE_ENDIAN

/* Ethernet standard lengths in bytes*/
#define ETH_ADDR_LEN    ( 6 )
#define ETH_TYPE_LEN    ( 2 )
#define ETH_CRC_LEN     ( 4 )
#define ETH_MAX_DATA    ( 1500 )
#define ETH_MIN_DATA    ( 46 )
#define ETH_HDR_LEN     ( ETH_ADDR_LEN * 2 + ETH_TYPE_LEN )

/* Defined Ethernet Frame Types */
#define ETH_FRM_IP      ( 0x0800 )
#define ETH_FRM_ARP     ( 0x0806 )
#define ETH_FRM_RARP    ( 0x8035 )
#define ETH_FRM_TEST    ( 0xA5A5 )

/* Maximum and Minimum Ethernet Frame Sizes */
#define ETH_MAX_FRM     ( ETH_HDR_LEN + ETH_MAX_DATA + ETH_CRC_LEN )
#define ETH_MIN_FRM     ( ETH_HDR_LEN + ETH_MIN_DATA + ETH_CRC_LEN )
// #define ETH_MTU         (ETH_HDR_LEN + ETH_MAX_DATA)

// Choose Enhanced Buffer Descriptor or Legacy
#define ENHANCED_BD

/* ethernet rx buffers setings */
#define ENET_RX_SINGLE_BUFFER   0		/* multiple buffers per frame - much more effective for buffer usage*/

#if ENET_RX_SINGLE_BUFFER
#define ENET_RX_BUF_SIZE        1520
#if ( LWIP_PERF_SETTINGS == 1 )
#define NUM_ENET_RX_BUFS        12
#else
#define NUM_ENET_RX_BUFS        8
#endif
#else
#define ENET_RX_BUF_SIZE        256		/* must be PBUF_POOL_BUFSIZE multiple */
#if ( LWIP_PERF_SETTINGS == 1 )
#define NUM_ENET_RX_BUFS        72
#else
#define NUM_ENET_RX_BUFS        10
#endif

/* PBUF_POOL_BUFSIZE macro must be greater than ENET min pkt */
#endif	/* ENET_RX_SINGLE_BUFFER */

/* ethernet tx buffers setings */
#define NUM_ENET_TX_BUFS        2		/* more is useless for this specific driver: might try with lower */
#define ENET_TX_BUF_SIZE        1520	/* no need to reduce this number */

#define ENET_MAX_PKT_SIZE       1520

#define BUF_WAIT_ATTEMPTS       10
#define ETH_BLOCK_TIME_WAITING_FOR_OUTPUT_BUF_MS ( 100 )
#define ETH_BLOCK_TIME_WAITING_FOR_OUTPUT_MS ( 5000 )

/* TODO use constants from MK6xF12.h */
/** tx buffer descriptor bits
 */
#define ENET_TX_BD_R            0x0080
#define ENET_TX_BD_TO1          0x0040
#define ENET_TX_BD_W            0x0020
#define ENET_TX_BD_TO2          0x0010
#define ENET_TX_BD_L            0x0008
#define ENET_TX_BD_TC           0x0004

#define ENET_TX_BD_STAT_TXE     0x8000
#define ENET_TX_BD_STAT_UE      0x2000
#define ENET_TX_BD_STAT_EE      0x1000
#define ENET_TX_BD_STAT_FE      0x0800
#define ENET_TX_BD_STAT_LCE     0x0400
#define ENET_TX_BD_STAT_OE      0x0200
#define ENET_TX_BD_STAT_TSE     0x0100

/* TODO use constants from MK6xF12.h */
/** rx buffer descriptor bits
 */
#define ENET_RX_BD_E            0x0080
#define ENET_RX_BD_R01          0x0040
#define ENET_RX_BD_W            0x0020
#define ENET_RX_BD_R02          0x0010
#define ENET_RX_BD_L            0x0008
#define ENET_RX_BD_M            0x0001
#define ENET_RX_BD_BC           0x8000
#define ENET_RX_BD_MC           0x4000
#define ENET_RX_BD_LG           0x2000
#define ENET_RX_BD_NO           0x1000
#define ENET_RX_BD_CR           0x0400
#define ENET_RX_BD_OV           0x0200
#define ENET_RX_BD_TR           0x0100

/** TX Enhanced BD Bit Definitions
 */
#define TX_BD_INT       0x00000040
#define TX_BD_TS        0x00000020
#define TX_BD_PINS      0x00000010
#define TX_BD_IINS      0x00000008
#define TX_BD_TXE       0x00800000
#define TX_BD_UE        0x00200000
#define TX_BD_EE        0x00100000
#define TX_BD_FE        0x00080000
#define TX_BD_LCE       0x00040000
#define TX_BD_OE        0x00020000
#define TX_BD_TSE       0x00010000

#define TX_BD_BDU       0x00000080

#define RX_BD_INT       0x00008000

enum init_status_t
{
	SUCCEED, FAILURE
};

/********INTERFACE**********/
typedef enum
{
	mac_mii, mac_rmii,
} ENET_INTERFACE;
/********AUTONEG**********/
typedef enum
{
	autoneg_on, autoneg_off
} ENET_AUTONEG;
/********SPEED**********/
typedef enum
{
	MII_10BASET, MII_100BASET
} ENET_SPEED;
/* *******DUPLEX*********
   MII Duplex Settings*/
typedef enum
{
	MII_HDX,/// < half-duplex
	MII_FDX
	/// < full-duplex
} ENET_DUPLEX;
/********LOOPBACK**********/
typedef enum
{
	INTERNAL_LOOPBACK, EXTERNAL_LOOPBACK, NO_LOOPBACK
} ENET_LOOPBACK;
/********EXTERNAL**********/
typedef enum
{
	EXTERNAL_NONE, EXTERNAL_YES
} ENET_EXTERNAL_CONN;

/**
 * @brief FEC Configuration Parameters
 */
typedef struct
{
	uint8_t ch;		/// < FEC channel
	ENET_INTERFACE interface;		/// < Transceiver mode
	ENET_AUTONEG neg;		/// < FEC autoneg
	ENET_SPEED speed;		/// < Ethernet Speed
	ENET_DUPLEX duplex;		/// < Ethernet Duplex
	ENET_LOOPBACK loopback;		/// < Loopback Mode
	ENET_EXTERNAL_CONN external;	/// < outside test?
	uint8_t prom;		/// < Promiscuous Mode?
	uint8_t mac[6];		/// < Ethernet Address
} ENET_CONFIG;

/**
 * @brief Buffer descriptor
 */
typedef struct _enet_bd_t
{
	uint16_t status;	///< control and status
	uint16_t length;	///< transfer length
	uint8_t* data;		///< buffer address
#ifdef ENHANCED_BD
	uint32_t ebd_status;
	uint8_t hdr_len;	///< header length
	uint8_t prot;		///< protocol type
	uint16_t checksum;		///< payload checksum
	uint16_t bdu;
	uint16_t resvd0;
	uint32_t timestamp;
	uint16_t resvd1;
	uint16_t resvd2;
	uint16_t resvd3;
	uint16_t resvd4;
#endif	/* ENHANCED_BD */
} enet_bd_t;

extern OS_Binary_Semaphore* ETHRxBinSemaphore;
extern OS_Binary_Semaphore* ETHTxBinSemaphore;
/*
 **************************************************************************************************
 *  Constants
   --------------------------------------------------------------------------------------------------
 */
/* ETHERNET MACMIIAR register Mask */
#define MACMIIAR_CR_Mask    ( ( UINT32 )0xFFFFFFE3 )

/* bits in a SMI transmission:
   32 (pream)+ 4 (SOF + OPCode) + 10 (PHY+REG add) + 2 (TA) + 16 (data) = 64
   slowest MDC clock: min(60MHz/42, 20MHz/16, 35Mhz/26) ~= 1.25MHz
   fastest SYSCLK 72MHz
   64 * 72 / 1.25 ~= 3686 < 4095 = 0x0FFF*/
#define ETH_SMI_TIMEOUT     ( ( UINT32 )0x000FFF )

#define SMI_SYS_REG_ADDR_MASK       ( ( UINT16 ) 0xFC03 )
/* SMI address shall be 0b000000xxxxxxxx00 */
#define SMI_VALID_SYS_REG_ADDR( addr ) ( ( ( addr ) & SMI_SYS_REG_ADDR_MASK ) == 0 )
/* phy_addr[4] = 1; phy_addr[3:0] = sys_reg_addr[9:6] */
#define SMI_SYS_REG_TO_PHY_ADDR( addr ) ( ( UINT16 )( 0x10 | ( ( ( addr ) & 0x3FF ) ) >> 6 ) )
/*
 * reg_addr[4:1] = sys_reg_addr[5:2]
 * reg_addr[0] = 1 if higher 16 bits, 0 if lower 16 bits
 */
#define SMI_SYS_REG_TO_LO_REG_ADDR( addr ) ( ( UINT16 ) ( ( ( addr ) >> 1 ) & 0x1E ) )
#define SMI_SYS_REG_TO_HI_REG_ADDR( addr ) ( ( UINT16 ) ( ( ( ( addr ) >> 1 ) & 0x1E ) | 1 ) )
/* LAN9303 only uses 0~3 */
#define LAN9303_VALID_PHY_ADDR( addr ) ( ( ( ( UINT16 ) addr ) & 0xFFFC ) == 0 )
/* register address and phy address are both 5 bits */
#define VALID_PHY_ADDR( addr ) ( ( ( ( UINT16 ) addr ) & 0xFFE0 ) == 0 )
#define VALID_REG_ADDR( addr ) ( ( ( ( UINT16 ) addr ) & 0xFFE0 ) == 0 )

#define VPHY_ADDR 1

/** @defgroup PHY_Register_address
 * @{
 */
#define PHY_BCR                          0	///< Tranceiver Basic Control Register
#define PHY_BSR                          1			/*!< Tranceiver Basic Status Register */
/** @defgroup PHY_basic_Control_register
 * @{
 */
#define PHY_Reset                       ( ( uint16_t )0x8000 )	/// < PHY Reset
#define PHY_Loopback                    ( ( uint16_t )0x4000 )	/// < Select loop-back mode
#define PHY_FULLDUPLEX_100M             ( ( uint16_t )0x2100 )	/// < Set the full-duplex mode at 100 Mb/s
#define PHY_HALFDUPLEX_100M             ( ( uint16_t )0x2000 )	/// < Set the half-duplex mode at 100 Mb/s
#define PHY_FULLDUPLEX_10M              ( ( uint16_t )0x0100 )	/// < Set the full-duplex mode at 10 Mb/s
#define PHY_HALFDUPLEX_10M              ( ( uint16_t )0x0000 )	/// < Set the half-duplex mode at 10 Mb/s
#define PHY_AutoNegotiation             ( ( uint16_t )0x1000 )	/// < Enable auto-negotiation function
#define PHY_Restart_AutoNegotiation     ( ( uint16_t )0x0200 )	/// < Restart auto-negotiation function
#define PHY_Powerdown                   ( ( uint16_t )0x0800 )	/// < Select the power down mode
#define PHY_Isolate                     ( ( uint16_t )0x0400 )	/// < Isolate PHY from MII
/**
 * @}
 */

/** @defgroup PHY_basic_status_register
 * @{
 */
#define PHY_AutoNego_Complete           ( ( uint16_t )0x0020 )	/// < Auto-Negotioation process completed
#define PHY_Linked_Status               ( ( uint16_t )0x0004 )	/// < Valid link established
#define PHY_Jabber_detection            ( ( uint16_t )0x0002 )	/// < Jabber condition detected
#define PHY_AUTONEG_ADVERTISE 4

/* Advertisement control register. */
#define ADVERTISE_SLCT      0x001f	/// < Selector bits
#define ADVERTISE_CSMA      0x0001	/// < Only selector supported
#define ADVERTISE_10HALF    0x0020	/// < Try for 10mbps half-duplex
#define ADVERTISE_1000XFULL 0x0020	/// < Try for 1000BASE-X full-duplex
#define ADVERTISE_10FULL    0x0040	/// < Try for 10mbps full-duplex
#define ADVERTISE_1000XHALF 0x0040	/// < Try for 1000BASE-X half-duplex
#define ADVERTISE_100HALF   0x0080	/// < Try for 100mbps half-duplex
#define ADVERTISE_1000XPAUSE    0x0080	/// < Try for 1000BASE-X pause
#define ADVERTISE_100FULL   0x0100	/// < Try for 100mbps full-duplex
#define ADVERTISE_1000XPSE_ASYM 0x0100	/// < Try for 1000BASE-X asym pause
#define ADVERTISE_100BASE4  0x0200	/// < Try for 100mbps 4k packets
#define ADVERTISE_PAUSE_CAP 0x0400	/// < Try for pause
#define ADVERTISE_PAUSE_ASYM    0x0800	/// < Try for asymetric pause
#define ADVERTISE_RESV      0x1000	/// < Unused...
#define ADVERTISE_RFAULT    0x2000	/// < ay we can detect faults
#define ADVERTISE_LPACK     0x4000	/// < Ack link partners response
#define ADVERTISE_NPAGE     0x8000	/// < Next page bit
#define PHY_LINK_PARTNER 5	/// < Link partner ability reg
enum
{
	uC_ETH_AUTO_NEG_STATE_LINK_INACTIVE,
	uC_ETH_AUTO_NEG_STATE_IN_PROGRESS,
	uC_ETH_AUTO_NEG_STATE_FAIL_DEFAULT_USED,
	uC_ETH_AUTO_NEG_STATE_DUPLEX_FAIL_SPEED_OK,
	uC_ETH_AUTO_NEG_STATE_SUCCESS,
	uC_ETH_AUTO_NEG_STATE_NEGOTIATION_DISABLED,
	uC_ETH_AUTO_NEG_STATE_PORT_DISABLED
};

enum
{
	uC_ETH_LINK_SPEED_UNDETERMINED = 0, uC_ETH_LINK_SPEED_10M = 10, uC_ETH_LINK_SPEED_100M = 100,
};

/**
 *****************************************************************************************
 * @brief    The STM32F207 microcontroller specific class for ethernet EMAC.
 *           This class shall derive the basic interfaces from the abstract class which
 *           EMAC_Driver.
 *****************************************************************************************
 */
class K6x_Eth_Driver : public EMAC_Driver
{
	public:
		/**
		 * @brief	Constructs a Stm32_Eth_Driver object.
		 * @param emac_select	The emac channel of microcontroller to be used
		 * @param emac_interface	EMAC interface of the MAC to be configured. Two
		 * possible values.
		 * @n	uC_EMAC_MII_MODE - MII Mode
		 * @n	uC_EMAC_RMII_MODE - RMII Mode
		 * @param mac_address	MAC address of the emac interface
		 * @param output_clock	Output clock from microcontroller to external
		 * PHY chip if required. Two possible values
		 * @n	TRUE - Generate clock on MCO pin for PHY chip
		 * @n	FALSE - Otherwise
		 * @param emac_dcids	The structure of DCIDs. This structure shall contain
		 * DCIDs for setting up ethernet emac configuration.
		 * e.g. multicast/broadcast settings etc.
		 */
		K6x_Eth_Driver( uint8_t emac_select, uC_EMAC_INTERFACE_ETD emac_interface,
						uint8_t* mac_address, bool_t output_clock, dcid_emac_params_t const* emac_dcids,
						uC_BASE_EMAC_IO_PORT_STRUCT const* io_port = nullptr );

		/**
		 * @brief	Destructor to delete an instance of Stm32_Eth_Driver
		 * class when object goes out of scope.
		 */
		~K6x_Eth_Driver( void );

		/**
		 * @brief	Initializes the descriptors of ethernet EMAC
		 * module. Configures the Tx and Rx memory memory
		 * space in descriptors. The Tx?Rx memory space is
		 * received from ethernetif.
		 * @param Tx_Buff	The RAM address from where data shall be read
		 * by EMAC and transmitted over ethernet
		 * @param rx_payload	The address pointer array. The received data packets
		 * shall be stored at this addresses by EMAC DMA.
		 * @param rx_buffer_size	The received packet buffer size
		 */
		void Init_Buff_Desc( uint8_t* Tx_Buff, uint32_t** rx_payload, uint16_t rx_buffer_size );

		/**
		 * @brief	The function updates the speed and mode of
		 * ethernet communication. LAN driver may call this
		 * function for updating EMAC.
		 * @param speed	The speed of ethernet communication. Possible values
		 * @n	1 - 10MBps
		 * @n	2 - 100MBps
		 * @param duplex	The mode of communication. Possible values
		 * @n	0 - Half Duplex
		 * @n	1 - Full duplex
		 */
		void Update_PHY_Config_To_MAC( uint8_t speed, uint8_t duplex );

		/**
		 * @brief	The function is used to pass the ethernet packet
		 * to application layer(ethernetif).The function returns
		 * the state of the packet to the ethernetif.
		 * @param new_payload	The RAM address at which ethernet packet is received
		 * over a network. The ethernet if layer shall copy the
		 * packet from this address
		 * @param pkt_index	The packet index
		 * @return data_states_t	The function returns the various states signifying the
		 * health and state of packet. Possible values are-
		 * @n	INIT  - The frame is not recived yet. EMAC is in
		 * initialization state
		 * @n	START - First packet of multipacket frame received
		 * @n	SINGLE_PKT_FRM  - The packet received is single packet
		 * frame.
		 * @n	INTERIM - The packet recived is intermediate packet.
		 * @n	ERROR_STATE  - The recievd packet is corrupt. Err
		 * @n	LAST - The recived packet is the last packet of
		 * the data frame
		 */
		data_states_t Get_Next_Rx_Pkt( uint32_t* new_payload, uint8_t* pkt_index );

		/**
		 * @brief	The function returns the RAM address of descriptor.
		 * The upper layer(ethrnetif) should write/copy the
		 * data to be transmitted at this location.
		 * @return uint8_t*	Memory address of theavailable descriptor at which
		 * data shall be written/copied for transmission
		 * purpose.
		 */
		uint8_t* Get_Next_Avail_Desc_Buff( void );

		/**
		 * @brief	Wait while EMAC prepares itself for next transmission
		 * purpose.
		 */
		void Wait_Tx_Ready( void );

		/**
		 * @brief	The function transmits MAC frame
		 * @param[in] l	Length of data to be transmitted
		 */
		void Tx_Frame( uint32_t l );

		/**
		 * @brief	The function returns the configured MAC address
		 * of the EMAC hardware.
		 * @param mac	Pointer to MAC address location
		 */
		void Get_MAC_Address( uint8_t* mac ) const;

		/**
		 * @brief	This function reads 16-bit register content from the PHY
		 * @param[in] phy_addr	PHY Port Number .
		 * @n	For single port PHY it is also the Default PHY address.
		 * @param[in] reg_addr	PHY Register number / offset .
		 * @param[in] read_ptr	The register value read from PHY is copied into the read pointer .
		 */
		bool_t SMIRead16Bits( uint16_t phy_addr, uint16_t reg_addr, uint16_t* read_ptr );

		/**
		 * @brief	This function writes 16-bit value to specified register in PHY
		 * @param[in] phy_addr	HY Port Number .
		 * @n	For single port PHY it is also the Default PHY address.
		 * @param[in] reg_addr	PHY Register number / offset .
		 * @n
		 * @param[in] write_ptr	The value to be written to PHY register
		 * @n
		 */
		bool_t SMIWrite16Bits( uint16_t phy_addr, uint16_t reg_addr, const uint16_t* write_ptr );

		/**
		 * @brief	This function reads 32-bit value from the PHY register.
		 * @n	Calls  Higher & Lower 16 bit read functions to read 32bit value
		 * @param[in] sys_reg_addr	PHY Register address .
		 * @n
		 * @param[in] read_ptr	The register value read from PHY is copied into the read pointer .
		 * @n
		 */
		bool_t SMIRead32Bits( uint16_t sys_reg_addr, uint32_t* read_ptr );

		/**
		 * @brief	This function writes 32-bit value from the PHY register.
		 * @n	Calls  Higher & Lower 16 bit write functions to write 32-bit value
		 * @param[in] sys_reg_addr	PHY System Register address .
		 * @n
		 * @param[in] write_ptr	The value to be written to the  PHY register
		 * @n
		 */
		bool_t SMIWrite32Bits( uint16_t sys_reg_addr, const uint32_t* write_ptr );

		/**
		 * @brief	Enables transmit and receive state machine of MAC.
		 * Starts DMAC transmission and reception.
		 */
		void Init_EMAC_Hardware( void );

		/**
		 * @brief	Ethernet Initialization
		 */
		void ETH_Init( ENET_CONFIG* config );

		/**
		 * @brief	Clears DMA interrupt pending bits.
		 * @param  ETH_DMA_IT	The uint32 type. Set the bit loaction of DMA interrupts
		 * which you want to clear.
		 */
		static void ETH_DMAClearITPendingBit( uint32_t ETH_DMA_IT );

		/**
		 * @brief	Configures MAC address of the EMAC
		 * @param  MacAddr	EMAC supports 4 different MAC addresss. User can configure
		 * the each MAC address using this MAC address offset parameter.
		 * @param Addr	pointer to location which holds MAC address to be configures.
		 */
		static void ETH_MACAddressConfig( uint32_t MacAddr, uint8_t* Addr );

		/**
		 * @brief	Ethernet handler function which is called when an ethernet
		 * receive interrupt occurs. It basically clears interrupts bits
		 * and wake up the task for data reception
		 */
		static void ETH_Handler( void );

		/**
		 * @brief	It basically initializes the ethernet driver and configures
		 * the MAC address.
		 * @param  MacAddr	EMAC supports 4 different MAC addresss. User can configure
		 * the each MAC address using this MAC address offset parameter.
		 */
		static void ETHDriverInit( uint8_t* mac_address );

		/**
		 * @brief	Ethernet Tx ISR.
		 */
		static void vEMAC_TxISRHandler( void );

		/**
		 * @brief	Ethernet Rx ISR.
		 */
		static void vEMAC_RxISRHandler( void );

		/**
		 * @brief	Ethernet Error ISR.
		 */
		static void vEMAC_ErrorISRHandler( void );

		/**
		 * @brief	Ethernet Timer ISR.
		 */
		static void vEMAC_TimerISRHandler( void );

		/**
		 * @brief	Function which is used to generate ethernet delay.
		 * @param nCount	Delay count
		 */
		static void ETH_Delay( __IO uint32_t nCount );

		/**
		 * @brief	Get the ENT Config structure.
		 */
		static ENET_CONFIG* GetEnetConfig( void );

	private:

		/**
		 * @brief	Initializes ethernet EMAC hardware.
		 */
		void Init_Eth_Emac( uint8_t* mac_address, dcid_emac_params_t const* emac_dcids );

		/**
		 * @brief	Configures ethernet.
		 */
		void ETH_Config( void );

		/**
		 * @brief	The function configures the MAC address
		 * of the EMAC hardware.
		 * @param mac	Pointer to MAC address location
		 */
		void Set_MAC_Address( uint8_t* mac );

		/**
		 *  @brief	Set the Physical (Hardware) Address and the Individual Address
		 * Hash in the selected FEC
		 */
		uint8_t ETH_Hash_Address( const uint8_t* addr );

		/**
		 *  @brief	Set the duplex on the selected FEC controller
		 */
		unsigned ETH_Duplex( int ch, ENET_DUPLEX duplex );

		/**
		 * @brief	Set the Physical (Hardware) Address and the Individual Address
		 * Hash in the selected FEC
		 */
		void ETH_Set_Address( uint16_t ch, const uint8_t* pa );

		/**
		 * @brief	Loads default communication parameters for ethernet
		 * communication
		 */
		void ETH_Load_Defaults( void );

		/**
		 * @brief	Initializes DMA transmit Descriptor chain
		 */
		void ETH_DMATxDescChainInit( uint8_t* TxBuff, uint32_t TxBuffCount );

		/**
		 * @brief	Initializes DMA receive Descriptor chain
		 */
		void ETH_DMARxDescChainInit( uint32_t**, uint16_t );

		/**
		 * @brief	Performs EMAC software reset
		 */
		void ETH_SoftwareReset( void ) const;

		/**
		 * @brief	Clears transmit FIFO
		 */
		void ETH_FlushTransmitFIFO( void ) const;

		/**
		 * @brief	Ethernet DMA initialization
		 */
		void Config_Eth_DMA_Init( void ) const;

		/**
		 * @brief	Wait while receive data is not available
		 */
		bool_t Wait_Rx_Data_Available( void ) const;

		/**
		 * @brief	Read MAC address registers
		 */
		void Read_MAC_Address_Reg( uint32_t MacAddrOffSet, uint8_t* Addr ) const;

		uC_BASE_EMAC_IO_STRUCT const* m_emac_pio;

		OS_Binary_Semaphore* m_ETHRxBinSemaphore;

		static const uint32_t ETH_BLOCK_TIME_WAITING_FOR_INPUT_MS = 1000U;
		static const uint32_t MACMIIAR_CR_MASK = 0xFFFFFFE3U;
		static const uint32_t MACCR_CLEAR_MASK = 0xFF20810FU;
		static const uint32_t DMAOMR_CLEAR_MASK = 0xF8DE3F23U;

		/**
		 * @brief	Ethernet ERROR
		 */
		static const uint16_t ETH_ERROR = 0U;
		/**
		 * @brief	NO Ethernet ERROR
		 */
		static const uint16_t ETH_SUCCESS = 1U;
		/**
		 * @brief	Ethernet MAC address 0 offset
		 */
		static const uint32_t ETH_MAC_Address_0 = 0x00000000U;
		/**
		 * @brief	Ethernet MAC address 1 offset
		 */
		static const uint32_t ETH_MAC_Address_1 = 0x00000008U;
		/**
		 * @brief	Ethernet MAC address 2 offset
		 */
		static const uint32_t ETH_MAC_Address_2 = 0x00000010U;
		/**
		 * @brief	Ethernet MAC address 3 offset
		 */
		static const uint32_t ETH_MAC_Address_3 = 0x00000018U;
		/**
		 * @brief	Ethernet interrupt priority
		 */
		static const uint8_t uC_ETHERNET_PRIORITY = uC_OS_INT_PRIORITY_6;
};

#endif	// _K6x_ETH_DRIVER_H
