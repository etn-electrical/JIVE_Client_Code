/**
 **********************************************************************************************
 * @file            EMAC_Driver.h Header File for interface class.
 *
 * @brief           Processor Independent Abstract Class for EMAC driver implementation
 *
 * @details
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */

#if !defined( _EMAC_DRIVER_H )
#define _EMAC_DRIVER_H
#include "Includes.h"
#include "DCI_Defines.h"

/** @brief enum which contains the various states of emac frames */
enum data_states_t
{
	/** @brief  INIT STATE */
	INIT,
	/** @brief  START State valid first packet received*/
	START,
	/** @brief  Frame contains only a single packet */
	SINGLE_PKT_FRM,
	/** @brief  The packet received is neither the first packet nor the last one */
	INTERIM,
	/** @brief  valid complete frame received */
	LAST,
	/** @brief  Error occured during packet reception */
	ERROR_STATE
};

#ifdef ESP32_SETUP
/** @brief typedef for receive frame function pointer */
typedef void (Receive_Frame_Func_Ptr)( void* h, uint8_t* buf, uint32_t length );
#endif

/** @brief EMAC interface modes */
typedef enum uC_EMAC_INTERFACE_ETD
{
	/** @brief Media Independent Interface */
	uC_EMAC_MII_MODE,
	/** @brief Reduced Media Independent Interface */
	uC_EMAC_RMII_MODE,
	uC_EMAC_INTERFACE_MODE_MAX
} uC_EMAC_INTERFACE_ETD;

/**
 ****************************************************************************************
 * @brief       Processor Independent Abstract Class for EMAC driver implementation
 *
 * @details	Base class for EMAC implementation. This class shall include the generalized
 *              interfaces which shall be used for information excahnge between
 *              microcontroller EMAC hardware and upper application layer
 *              (Noteably ethernetIf.cpp of LWIP)
 *
 ****************************************************************************************
 */
class EMAC_Driver
{
	public:

		/**  @brief  An array of DCIDs. The configuration settings of EMAC shall
		 *           be checked out using these DCIDs
		 */
		struct  dcid_emac_params_t
		{
			DCI_ID_TD broadcast_settings;
			DCI_ID_TD multicast_settings;
		};

		/**
		 *    @brief Constructor to create an instance of EMAC_driver class
		 */
		EMAC_Driver( void )
		{}

		/**
		 *   @brief Destructor to delete an instance of EMAC_driver class
		 */
		~EMAC_Driver( void )
		{}

		/**
		 * @brief                -  The function returns the RAM address loaction.
		 *                          The upper layer(ethrnetif) should write/copy the
		 *                          data to be transmitted at this location.
		 * @return uint8_t*      -  Memory address location at which
		 *                          data shall be written/copied for transmission
		 *                          purpose.
		 */
		virtual uint8_t* Get_Next_Avail_Desc_Buff( void ) = 0;

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
		virtual data_states_t Get_Next_Rx_Pkt( uint32_t* new_payload, uint8_t* pkt_index ) = 0;

		/**
		 * @brief                -  Initializes the descriptors of ethernet EMAC
		 *                          module. Configures the Tx and Rx memory memory
		 *                          space in descriptors. The Tx/Rx memory locations are
		 *                          received from ethernetif.
		 * @param Tx_Buff        -  The RAM address from where data shall be read
		 *                          by EMAC and transmitted over ethernet
		 * @param rx_payload     -  The address pointer array. The received data packets
		 *                          shall be stored at this addresses by EMAC DMA.
		 * @param rx_buffer_size -  The received packet buffer size
		 */
		virtual void Init_Buff_Desc( uint8_t* Tx_Buff, uint32_t** rx_payload,
									 uint16_t rx_buffer_size ) = 0;

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
		virtual void Update_PHY_Config_To_MAC( uint8_t speed, uint8_t duplex ) = 0;

		/**
		 * @brief                   This function reads 16-bit register content from the PHY
		 * @param[in] phy_addr      PHY Port Number .
		 * @n                       For single port PHY it is also the Default PHY address.
		 * @param[in] reg_addr      PHY Register number / offset .
		 * @n
		 * @param[in] read_ptr      The register value read from PHY is copied into the read pointer .
		 * @n
		 */
		virtual BOOL SMIRead16Bits( uint16_t phy_addr, uint16_t reg_addr,
									uint16_t* read_ptr ) = 0;

		/**
		 * @brief                   This function writes 16-bit value to specified register in PHY
		 * @param[in] phy_addr      PHY Port Number .
		 * @n                       For single port PHY it is also the Default PHY address.
		 * @param[in] reg_addr      PHY Register number / offset .
		 * @n
		 * @param[in] write_ptr     The value to be written to PHY register
		 * @n
		 */
		virtual BOOL SMIWrite16Bits( uint16_t phy_addr, uint16_t reg_addr,
									 const uint16_t* write_ptr ) = 0;

		/**
		 * @brief                   This function reads 32-bit value from the PHY register.
		 * @param[in] sys_reg_addr  PHY Register address .
		 * @n
		 * @param[in] read_ptr      The register value read from PHY is copied into the read pointer .
		 * @n
		 */
		virtual BOOL SMIRead32Bits( uint16_t sys_reg_addr, uint32_t* read_ptr ) = 0;

		/**
		 * @brief                   This function writes 32-bit value from the PHY register.
		 * @param[in] sys_reg_addr  PHY System Register address .
		 * @n
		 * @param[in] write_ptr     The value to be written to the  PHY register
		 * @n
		 */
		virtual BOOL SMIWrite32Bits( uint16_t sys_reg_addr,
									 const uint32_t* write_ptr ) = 0;

		/**
		 * @brief                -  The function transmits MAC frame
		 * @param[in] l              -  Length of data to be transmitted
		 */
		virtual void Tx_Frame( uint32_t l ) = 0;

		/**
		 * @brief                -  Wait while EMAC prepares itself for next transmission
		 *                          purpose.
		 */
		virtual void Wait_Tx_Ready( void ) = 0;

		/**
		 * @brief                -  The function returns the configured MAC address
		 *                          of the EMAC hardware.
		 * @param Addr           -  Pointer to MAC address location
		 */
		virtual void Get_MAC_Address( uint8_t* Addr ) const = 0;

		/**
		 * @brief                -  Enables transmit and receive state machine of MAC.
		 */
		virtual void Init_EMAC_Hardware( void ) = 0;

#ifdef ESP32_SETUP
		/**
		 * @brief                -  Attach the Callback function for Receive Frame from MAC Driver.
		 */
		virtual void Receive_Frame_Attach_FuncPtr( Receive_Frame_Func_Ptr* rx_frame_fptr, void* netif ) = 0;

		/**
		 * @brief                -  Trasmit the Frame to MAC Driver.
		 */


#endif

		/** @brief Number of transmit buffers */
		static const uint8_t ETH_TX_BUFNB = 2U;

		/** @brief Number of receive buffers */
		static const uint8_t ETH_RX_BUFNB = 10U;

		/** @brief Number of receive buffers */
		static const uint16_t ETH_MTU = 1500U;

		/** @brief Maximum ethernetpacket size */
		static const uint16_t ETH_MAX_PACKET_SIZE = 1520U;

		/** @brief MAC hardware address length */
		static const unsigned char ETH_MAC_HW_ADDRESS_LEN = 6U;
};

#endif	// _EMAC_DRIVER_H
