/**
 *****************************************************************************************
 * @file            K6x_Eth_Driver.cpp Header File for interface class.
 *
 * @brief           The file contains K6x_Eth_Driver.cpp, an interface class required
 *
 * @details
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 *****************************************************************************************
 */

#include "K6x_Eth_Driver.h"
#include "OS_Tasker.h"
#ifdef uC_KINETISK60x_USAGE
#include "uC_Base_K60F12.h"
#endif

#ifdef uC_KINETISK60DNx_USAGE
#include "uC_Base_K60D10.h"
#endif

#ifdef uC_KINETISK66x_USAGE
#include "uC_Base_K66F18.h"
#endif
#include "uC_Ext_Int.h"
#include "Eth_if.h"
#include "Babelfish_Assert.h"

extern uint8_t Tx_Buff[EMAC_Driver::ETH_TX_BUFNB][EMAC_Driver::ETH_MAX_PACKET_SIZE];
extern struct pbuf* rx_buffptr[EMAC_Driver::ETH_RX_BUFNB];
extern struct netif lwip_netif;

OS_Binary_Semaphore* ETHRxBinSemaphore = nullptr;
OS_Binary_Semaphore* ETHTxBinSemaphore = nullptr;
OS_Binary_Semaphore* PhySemaphore = nullptr;

volatile enet_bd_t* tx_bd;
enet_bd_t* rx_bd;

static uC_Ext_Int* phy_interrupt;

static unsigned char tx_bd_unaligned[( EMAC_Driver::ETH_TX_BUFNB* sizeof( enet_bd_t ) ) + 16];
static unsigned char rx_bd_unaligned[( EMAC_Driver::ETH_RX_BUFNB* sizeof( enet_bd_t ) ) + 16];

static unsigned volatile s_enter = 0;

static uint32_t rx_next_buf = 0;
static uint32_t tx_next_buf = 0;

enum
{
	uC_ETH_PORT_ENABLE, uC_ETH_AUTONEG_ENABLE, uC_ETH_FULL_DUPLEX, uC_ETH_SPEED_100
};

ENET_CONFIG enet_cfg =
{ 0,	/// < FEC channel
#ifdef __KSZ8041__
  mac_mii,	/// < Transceiver mode
#else
  mac_rmii,		/// < Transceiver mode
#endif
  autoneg_on,	/// < FEC autoneg
  MII_100BASET,		/// < Ethernet Speed
  MII_FDX,	/// < Ethernet Duplex
  NO_LOOPBACK,	/// < Loopback Mode
  EXTERNAL_NONE,	/// < outside test?
  0		/// < Promiscuous Mode?
	/* MAC gets set later. */
};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
ENET_CONFIG* K6x_Eth_Driver::GetEnetConfig( void )
{
	return ( &enet_cfg );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void on_graceful_stop( void )
{
	BaseType_t dummy = 0U;

	PhySemaphore->Post_From_ISR( &dummy );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void s_phy_interrupt_handler( void )
{
	/* Count phy interrupts. */
	BaseType_t dummy = 0U;

	PhySemaphore->Post_From_ISR( &dummy );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static struct pbuf* low_level_input( struct netif* netif )
{

	struct pbuf* first_pbuf, * next_pbuf;
	u8_t loop_state = 2;

	first_pbuf = NULL;
	next_pbuf = NULL;

	/* A few notes on how this loop works:
	 *
	 * The primary goal of this loop is to empty all the buffers (BD)
	 * associated with 1 single received packet. More data cannot be received
	 * if these buffers are not emptied.
	 *
	 * The secondary goal of this loop is to copy the packet data into dynamically
	 * allocated buffers. If there is no memory available, then
	 * first_pbuf and next_pbuf BOTH end up being NULL and no copies are made.
	 *
	 * If a receive error is detected (by inspecting the last BD, IE L flag is set)
	 * then the first_pbuf is deallocated and this function returns NULL.
	 *
	 * loop_state is 2 on the first iteration, and 1 on subsequent iterations.
	 * It is really used to determine if first_pbuf or next_pbuf is allocated
	 * */
	while ( loop_state )
	{
		u16_t len;

		if ( rx_bd[rx_next_buf].status & ENET_RX_BD_E )
		{
			return ( ( struct pbuf* )NULL );
		}

		if ( rx_bd[rx_next_buf].status & ENET_RX_BD_L )
		{
			if ( rx_bd[rx_next_buf].status & ( ENET_RX_BD_LG | ENET_RX_BD_NO | ENET_RX_BD_CR
											   | ENET_RX_BD_OV ) )
			{
				/* bad packet */ LINK_STATS_INC( link.err );
				len = 0;
				if ( first_pbuf )
				{
					pbuf_free( first_pbuf );
					first_pbuf = NULL;
				}
			}
			else
			{
				loop_state = 0;
#ifdef ENET_LITTLE_ENDIAN
				len = __REVSH( rx_bd[rx_next_buf].length );
#else
				len = rx_bd[rx_next_buf].length;
#endif
				LINK_STATS_INC( link.recv );
			}
		}
		else
		{
			/* if not L bit, then buffer's length */
			len = ENET_RX_BUF_SIZE;
		}

		if ( len )
		{
			struct pbuf* q;
			u16_t l, temp_l;
			if ( !first_pbuf )
			{
				if ( 1 != loop_state )
				{
					first_pbuf = pbuf_alloc( PBUF_RAW, len, PBUF_POOL );
					if ( !first_pbuf )
					{
						LINK_STATS_INC( link.memerr );
						LINK_STATS_INC( link.drop );
					}

					next_pbuf = first_pbuf;

					if ( loop_state )
					{
						loop_state = 1;
					}
				}
			}
			else if ( !next_pbuf )
			{
				/* Need more memory, so allocate more data. If not successful,
				 * then drop the packet. */
				next_pbuf = pbuf_alloc( PBUF_RAW, len, PBUF_POOL );
				if ( !next_pbuf )
				{
					LINK_STATS_INC( link.memerr );
					LINK_STATS_INC( link.drop );
					pbuf_free( first_pbuf );
					first_pbuf = NULL;
				}
			}

#if !defined ( __KSZ8081__ ) && !defined ( __KSZ8041__ )
			if ( next_pbuf && ( InvalidRamPtr( next_pbuf ) || InvalidRamPtr( next_pbuf->payload ) ) )
			{
				BF_ASSERT( false );
			}
#endif

			/* get data */
			l = 0;
			temp_l = 0;
			/* We iterate over the pbuf chain until we have read the entire
			 * packet into the pbuf. */
			for ( q = next_pbuf; q != NULL; q = q->next )
			{
#ifdef ENET_LITTLE_ENDIAN
				u8_t* data_temp;
#endif
				/* Read enough bytes to fill this pbuf in the chain. The
				 * available data in the pbuf is given by the q->len
				 * variable.
				 * This does not necessarily have to be a memcpy, you can also preallocate
				 * pbufs for a DMA-enabled MAC and after receiving truncate it to the
				 * actually received size. In this case, ensure the tot_len member of the
				 * pbuf is the sum of the chained pbuf len members.
				 */
				temp_l = LWIP_MIN( len, LWIP_MEM_ALIGN_SIZE( PBUF_POOL_BUFSIZE ) );

#ifdef ENET_LITTLE_ENDIAN
				data_temp = ( u8_t* )__REV( ( u32_t )rx_bd[rx_next_buf].data );
#if !defined ( __KSZ8081__ ) && !defined ( __KSZ8041__ )
				BF_ASSERT( !InvalidRamPtr( data_temp ) );
				BF_ASSERT( !InvalidRamPtr( data_temp + temp_l ) );
#endif
				memcpy( ( u8_t* )q->payload, &( data_temp[l] ), temp_l );
#else
#if !defined ( __KSZ8081__ ) && !defined ( __KSZ8041__ )
				BF_ASSERT( !InvalidRamPtr( rx_bd[rx_next_buf].data ) );
				BF_ASSERT( !InvalidRamPtr( rx_bd[rx_next_buf].data + temp_l ) );
#endif
				memcpy( ( u8_t* )q->payload, &( rx_bd[rx_next_buf].data[l] ), temp_l );
#endif
				l += temp_l;
				len -= temp_l;
			}

			/* link pbufs */
			if ( first_pbuf && ( next_pbuf != first_pbuf ) )
			{
				pbuf_cat( first_pbuf, next_pbuf );
			}
			next_pbuf = NULL;
		}

		if ( rx_bd[rx_next_buf].status & ENET_RX_BD_W )
		{
			rx_bd[rx_next_buf].status = ENET_RX_BD_W | ENET_RX_BD_E;
		}
		else
		{
			rx_bd[rx_next_buf].status = ENET_RX_BD_E;
		}
		ENET->RDAR = ENET_RDAR_RDAR_MASK;

		++rx_next_buf;
		if ( rx_next_buf >= NUM_ENET_RX_BUFS )
		{
			rx_next_buf = 0;
		}
	}

	return ( first_pbuf );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
K6x_Eth_Driver::K6x_Eth_Driver( uint8_t emac_select, uC_EMAC_INTERFACE_ETD emac_interface,
								uint8_t* mac_address, bool_t output_clock, dcid_emac_params_t const* emac_dcids,
								uC_BASE_EMAC_IO_PORT_STRUCT const* io_port ) :
	m_emac_pio( ( uC_Base::Self() )->Get_EMAC_Ctrl( emac_select ) ),
	/* m_emac_ctrl( m_emac_pio->reg_ctrl ), */
	m_ETHRxBinSemaphore( static_cast<OS_Binary_Semaphore*>( NULL ) ),
	EMAC_Driver()
{

	m_ETHRxBinSemaphore = static_cast<OS_Binary_Semaphore*>( NULL );
	/* Enable  clock */
	SIM->SCGC2 |= SIM_SCGC2_ENET_MASK;
	SIM->SCGC7 |= SIM_SCGC7_MPU_MASK;
	MPU->CESR = 0;	/* MPU off */

	/* Throw the Pin Muxes to connect the module to external I/O
	   Throw the Pin Muxes to connect the module to external I/O*/
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;

	m_emac_pio = ( uC_Base::Self() )->Get_EMAC_Ctrl( emac_select );

	/* Set MAC Address in emac structure */
	if ( mac_address != nullptr )
	{
		Set_MAC_Address( mac_address );
	}

	/* Software reset */
	ETH_SoftwareReset();

	BF_ASSERT( m_emac_pio != 0U );
	if ( io_port == NULL )
	{
		io_port = m_emac_pio->default_port_io;
	}

	if ( emac_interface != uC_EMAC_MII_MODE )
	{
		/* Initialize the Receive Control Register */
		ENET->RCR /*(ch)*/ = ENET_RCR_MAX_FL( ETH_MAX_FRM ) | ENET_RCR_MII_MODE_MASK
			| ENET_RCR_CRCFWD_MASK;	/*no CRC pad required*/

		ENET->RCR /*(ch)*/ |= ENET_RCR_RMII_MODE_MASK;

		for ( uint8_t i = 0U; i < m_emac_pio->rmii_pio_out_count; i++ )
		{
			uC_IO_Config::Enable_Periph_Output_Pin( io_port->rmii_pio_out[i] );
		}
		for ( uint8_t i = 0U; i < m_emac_pio->rmii_pio_in_count; i++ )
		{
			uC_IO_Config::Enable_Periph_Input_Pin( io_port->rmii_pio_in[i] );
		}
	}
	else
	{
		/* Initialize the Receive Control Register */
		ENET->RCR /*(ch)*/ = ENET_RCR_MAX_FL( ETH_MAX_FRM ) | ENET_RCR_MII_MODE_MASK
			| ENET_RCR_CRCFWD_MASK;	/*no CRC pad required*/

		for ( uint8_t i = 0U; i < m_emac_pio->mii_pio_out_count; i++ )
		{
			uC_IO_Config::Enable_Periph_Output_Pin( io_port->mii_pio_out[i] );
		}
		for ( uint8_t i = 0U; i < m_emac_pio->mii_pio_in_count; i++ )
		{
			uC_IO_Config::Enable_Periph_Input_Pin( io_port->mii_pio_in[i] );
		}
	}

	/* Minimum hold time is 10ns, which is a freq of 100 MHZ. */
	ENET->MSCR /*(ch)*/ = 0 | ENET_MSCR_HOLDTIME( 120 / 100 ) | ENET_MSCR_MII_SPEED( 120 / 5 - 1 );

	/* Initialize semaphores and mutexes. */
	ETHRxBinSemaphore = new OS_Binary_Semaphore( FALSE );
	PhySemaphore = new OS_Binary_Semaphore( FALSE );
	ETHTxBinSemaphore = new OS_Binary_Semaphore( TRUE );

	/* TODO: fix the magic number, between kernel and sys priorities */
	uC_OS_Interrupt::Set_OS_Vect( vEMAC_TxISRHandler, m_emac_pio->tx_irq_num,
								  uC_ETHERNET_PRIORITY );
	uC_OS_Interrupt::Set_OS_Vect( vEMAC_RxISRHandler, m_emac_pio->rx_irq_num,
								  uC_ETHERNET_PRIORITY );
	uC_OS_Interrupt::Set_OS_Vect( vEMAC_ErrorISRHandler, m_emac_pio->error_irq_num,
								  uC_ETHERNET_PRIORITY );
	uC_OS_Interrupt::Set_OS_Vect( vEMAC_TimerISRHandler, m_emac_pio->timer_irq_num,
								  uC_ETHERNET_PRIORITY );

	uC_OS_Interrupt::Enable_Int( m_emac_pio->tx_irq_num );
	uC_OS_Interrupt::Enable_Int( m_emac_pio->rx_irq_num );
	uC_OS_Interrupt::Enable_Int( m_emac_pio->error_irq_num );
	uC_OS_Interrupt::Enable_Int( m_emac_pio->timer_irq_num );

	phy_interrupt = new uC_Ext_Int( &PIOB_PIN_6_STRUCT );
	phy_interrupt->Attach_IRQ( s_phy_interrupt_handler, EXT_INT_FALLING_EDGE_TRIG, uC_IO_IN_CFG_HIZ,
							   uC_INT_HW_PRIORITY_15 );	// comment K
	phy_interrupt->Enable_Int();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void K6x_Eth_Driver::ETH_Init( ENET_CONFIG* config )
{
	/* Clear the Individual and Group Address Hash registers */
	ENET->IALR /*(ch)*/ = 0;
	ENET->IAUR /*(ch)*/ = 0;
	ENET->GALR /*(ch)*/ = 0;
	ENET->GAUR /*(ch)*/ = 0;

	/* Set the Physical Address for the selected FEC */
	ETH_Set_Address( config->ch, config->mac );

	/* Clear all ENET interrupt events */
	ENET->EIR = 0xFFFFFFFF;

	/* Enable interrupts. */

	/* Not using
	 * ENET_EIMR_TXB
	 * ENET_EIMR_RXB
	 * ENET_EIMR_MII
	 * ENET_EIMR_WAKEUP
	 * ENET_EIMR_TS_AVAIL
	 * ENET_EIMR_TS_TIMER
	 * ENET_EIMR_BABR_MASK
	 *
	 *
	 * */
	ENET->EIMR = 0
		/*rx irqs*/
		| ENET_EIMR_RXF_MASK/* only for complete frame, not partial buffer descriptor | ENET_EIMR_RXB_MASK*/
		/*xmit irqs*/
		| ENET_EIMR_TXF_MASK/* only for complete frame, not partial buffer descriptor | ENET_EIMR_TXB_MASK*/
		/*event irqs*/
		| ENET_EIMR_BABT_MASK | ENET_EIMR_GRA_MASK | ENET_EIMR_EBERR_MASK | ENET_EIMR_PLR_MASK;

	/* Initialize the Receive Control Register */
	ENET->RCR /*(ch)*/ = 0 | ENET_RCR_MAX_FL( ETH_MAX_FRM ) | ENET_RCR_MII_MODE_MASK/*always*/
		| ENET_RCR_CRCFWD_MASK;	/*no CRC pad required*/

	ENET->TCR /*(ch)*/ = 0;

	ENET->RAEM = 6;

	/* Again clear all FEC interrupt events */
	ENET->EIR /*(ch)*/ = 0xFFFFFFFF;

	/* Set the duplex */
	ETH_Duplex( config->ch, config->duplex );

	if ( config->speed == MII_10BASET )
	{
		ENET->RCR /*(ch)*/ |= ENET_RCR_RMII_10T_MASK;
	}

	if ( config->prom )
	{
		ENET->RCR /*(ch)*/ |= ENET_RCR_PROM_MASK;
	}

	if ( config->interface == mac_rmii )
	{
		ENET->RCR /*(ch)*/ |= ENET_RCR_RMII_MODE_MASK;
	}

	ENET->ECR = ENET_ECR_EN1588_MASK;

	/* Store and forward checksum. */
	ENET->TFWR = ENET_TFWR_STRFWD_MASK;

	/* Enable the MAC itself. */
	ENET->ECR |= ENET_ECR_ETHEREN_MASK;

	/* Indicate that there have been empty receive buffers produced */
	ENET->RDAR = ENET_RDAR_RDAR_MASK;

	ENET->MIBC &= ~ENET_MIBC_MIB_DIS_MASK;
}

/********************************************************************/
/*
 * Set the Physical (Hardware) Address and the Individual Address
 * Hash in the selected FEC
 *
 * Parameters:
 *  ch  FEC channel
 *  pa  Physical (Hardware) Address for the selected FEC
 */
void K6x_Eth_Driver::ETH_Set_Address( uint16_t ch, const uint8_t* pa )
{
	uint8_t crc;

	/*
	 * Set the Physical Address
	 */
	ENET->PALR /*(ch)*/ = ( uint32_t ) ( ( pa[0] << 24 ) | ( pa[1] << 16 ) | ( pa[2] << 8 ) | pa[3] );
	ENET->PAUR /*(ch)*/ = ( uint32_t ) ( ( pa[4] << 24 ) | ( pa[5] << 16 ) );

	/*
	 * Calculate and set the hash for given Physical Address
	 * in the  Individual Address Hash registers
	 */
	crc = ETH_Hash_Address( pa );
	if ( crc >= 32 )
	{
		ENET->IAUR /*(ch)*/ |= ( uint32_t ) ( 1 << ( crc - 32 ) );
	}
	else
	{
		ENET->IALR /*(ch)*/ |= ( uint32_t ) ( 1 << crc );
	}
}

/********************************************************************/
/*
 * Generate the hash table settings for the given address
 *
 * Parameters:
 *  addr    48-bit (6 byte) Address to generate the hash for
 *
 * Return Value:
 *  The 6 most significant bits of the 32-bit CRC result
 */
uint8_t K6x_Eth_Driver::ETH_Hash_Address( const uint8_t* addr )
{
	uint32_t crc;
	uint8_t byte;
	int i, j;

	crc = 0xFFFFFFFF;
	for ( i = 0; i < 6; ++i )
	{
		byte = addr[i];
		for ( j = 0; j < 8; ++j )
		{
			if ( ( byte & 0x01 ) ^ ( crc & 0x01 ) )
			{
				crc >>= 1;
				crc = crc ^ 0xEDB88320;
			}
			else
			{
				crc >>= 1;
			}
			byte >>= 1;
		}
	}
	return ( ( uint8_t ) ( crc >> 26 ) );
}

/********************************************************************/
/*
 * Set the duplex on the selected FEC controller
 *
 * Parameters:
 *  ch      FEC channel
 *  duplex  enet_MII_FULL_DUPLEX or enet_MII_HALF_DUPLEX
 */
unsigned K6x_Eth_Driver::ETH_Duplex( int ch, ENET_DUPLEX duplex )
{
	/* Cannot change duplex settings if ethernet is enabled! */
	if ( ENET->ECR & ENET_ECR_ETHEREN_MASK )
	{
		return ( 1 );
	}

	switch ( duplex )
	{
		case MII_HDX:
			ENET->RCR /*(ch)*/ |= ENET_RCR_DRT_MASK;
			ENET->TCR /*(ch)*/ &= ( uint32_t ) ~ENET_TCR_FDEN_MASK;
			break;

		case MII_FDX:
			ENET->RCR /*(ch)*/ &= ~ENET_RCR_DRT_MASK;
			ENET->TCR /*(ch)*/ |= ENET_TCR_FDEN_MASK;
			break;

		default:
			return ( 1 );
	}

	return ( 0 );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void K6x_Eth_Driver::Init_EMAC_Hardware( void )
{
	ETH_Init( &enet_cfg );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void K6x_Eth_Driver::Init_Buff_Desc( uint8_t* Tx_Buff, uint32_t** rx_payload,
									 uint16_t rx_buffer_size )
{
	int i;
	uint8_t* buf;

	tx_bd = ( enet_bd_t* ) ( ( ( uint32_t )tx_bd_unaligned + 0x0f ) & 0xfffffff0 );
	rx_bd = ( enet_bd_t* ) ( ( ( uint32_t )rx_bd_unaligned + 0x0f ) & 0xfffffff0 );

	/* setup the buffers and initialize buffers descriptors */
	buf = ( uint8_t* ) ( ( ( uint32_t )Tx_Buff + 0x0f ) & 0xfffffff0 );

	for ( i = 0; i < EMAC_Driver::ETH_TX_BUFNB; i++ )
	{
		tx_bd[i].status = ENET_TX_BD_TC;
#ifdef ENET_LITTLE_ENDIAN
		tx_bd[i].data = ( uint8_t* )__REV( ( uint32_t )buf );
#else
		tx_bd[i].data = buf;
#endif	/* ENET_LITTLE_ENDIAN */
		tx_bd[i].length = 0;
#ifdef ENHANCED_BD
		tx_bd[i].ebd_status = TX_BD_INT
#if ENET_HARDWARE_CHECKSUM
			| TX_BD_IINS
			| TX_BD_PINS
#endif	/* ENET_HARDWARE_CHECKSUM */
		;
#endif	/* ENHANCED_BD */
		buf += EMAC_Driver::ETH_MAX_PACKET_SIZE;
	}

	for ( i = 0; i < EMAC_Driver::ETH_RX_BUFNB; i++ )
	{
		buf = ( uint8_t* ) ( ( ( uint32_t )rx_buffptr[i]->payload + 0x0f ) & 0xfffffff0 );
		rx_bd[i].status = ENET_RX_BD_E;
		rx_bd[i].length = 0;
#ifdef ENET_LITTLE_ENDIAN
		rx_bd[i].data = ( uint8_t* )__REV( ( uint32_t )buf );
		rx_buffptr[i]->payload = ( uint8_t* ) ( buf );
#else
		rx_bd[i].data = buf;
#endif	/* ENET_LITTLE_ENDIAN */

#ifdef ENHANCED_BD
		rx_bd[i].bdu = 0x00000000;
		rx_bd[i].ebd_status = RX_BD_INT;
#endif	/* ENHANCED_BD */
	}

	/* set the wrap bit in the last descriptors to form a ring */
	tx_bd[EMAC_Driver::ETH_TX_BUFNB - 1].status |= ENET_TX_BD_W;
	rx_bd[EMAC_Driver::ETH_RX_BUFNB - 1].status |= ENET_RX_BD_W;

	rx_next_buf = 0;
	tx_next_buf = 0;

	/* point to the start of the circular rx buffer descriptor queue */
	ENET->RDSR = ( uint32_t )rx_bd;

	/* point to the start of the circular tx buffer descriptor queue */
	ENET->TDSR = ( uint32_t )tx_bd;

	/* Set Rx Buffer Size */
	ENET->MRBR = ( unsigned short )ENET_RX_BUF_SIZE;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void K6x_Eth_Driver::ETH_DMARxDescChainInit( uint32_t** rx_payload, uint16_t rx_buffer_size )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
K6x_Eth_Driver::~K6x_Eth_Driver( void )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void K6x_Eth_Driver::Set_MAC_Address( uint8_t* mac )
{
	ENET_CONFIG* cfg = K6x_Eth_Driver::GetEnetConfig();

	cfg->mac[0] = mac[0];
	cfg->mac[1] = mac[1];
	cfg->mac[2] = mac[2];
	cfg->mac[3] = mac[3];
	cfg->mac[4] = mac[4];
	cfg->mac[5] = mac[5];
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void K6x_Eth_Driver::Get_MAC_Address( uint8_t* mac ) const
{
	ENET_CONFIG* cfg = K6x_Eth_Driver::GetEnetConfig();

	mac[0] = cfg->mac[0];
	mac[1] = cfg->mac[1];
	mac[2] = cfg->mac[2];
	mac[3] = cfg->mac[3];
	mac[4] = cfg->mac[4];
	mac[5] = cfg->mac[5];
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void K6x_Eth_Driver::Read_MAC_Address_Reg( uint32_t MacAddrOffSet, uint8_t* Addr ) const
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void K6x_Eth_Driver::ETH_SoftwareReset( void ) const
{
	int i;

	/* Set the Reset bit and clear the Enable bit */
	ENET->ECR /*(ch)*/ = ENET_ECR_RESET_MASK;

	/* Wait at least 8 clock cycles */
	for ( i = 0; i < 10; ++i )
	{
		asm ( "NOP" );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t K6x_Eth_Driver::SMIRead16Bits( uint16_t phy_addr, uint16_t reg_addr, uint16_t* read_ptr )
{
	volatile uint32_t timeout = 0U;
	uint32_t temp_reg;
	bool_t success = true;

	/* Clear the MII interrupt bit */
	ENET->EIR /*(ch)*/ = ENET_EIR_MII_MASK;

	/* Initiatate the MII Management read */
	ENET->MMFR /*(ch)*/ = 0 | ENET_MMFR_ST( 0x01 ) | ENET_MMFR_OP( 0x2 ) | ENET_MMFR_PA( phy_addr )
		| ENET_MMFR_RA( reg_addr ) | ENET_MMFR_TA( 0x02 );

	/* Poll for the MII interrupt (interrupt should be masked) */
	do
	{
		timeout++;
		temp_reg = ENET->EIR;
		if ( timeout > ETH_SMI_TIMEOUT )
		{
			/* Return ERROR in case of timeout */
			success = false;
			break;
		}
	} while ( !( static_cast<bool_t>( temp_reg & ENET_EIR_MII_MASK ) ) );

	/* Return data register value */
	if ( success )
	{
		/* Clear the MII interrupt bit */
		ENET->EIR /*(ch)*/ = ENET_EIR_MII_MASK;
		*read_ptr = static_cast<uint16_t>( ENET->MMFR & 0x0000FFFF );	// *read_ptr = (uint16_t) ( ETH->MACMIIDR );
	}
	return ( static_cast<bool_t>( success ) );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t K6x_Eth_Driver::SMIWrite16Bits( uint16_t phy_addr, uint16_t reg_addr,
									   const uint16_t* write_ptr )
{
	__IO
	uint32_t timeout = 0U;
	bool_t success = true;
	uint32_t temp_reg;

	/* Clear the MII interrupt bit */
	ENET->EIR /*(ch)*/ = ENET_EIR_MII_MASK;

	/* Initiatate the MII Management write */
	ENET->MMFR /*(ch)*/ = 0 | ENET_MMFR_ST( 0x01 ) | ENET_MMFR_OP( 0x01 ) | ENET_MMFR_PA( phy_addr )
		| ENET_MMFR_RA( reg_addr ) | ENET_MMFR_TA( 0x02 ) | ENET_MMFR_DATA( *write_ptr );

	/* Check for the Busy flag */
	do
	{
		timeout++;
		temp_reg = ENET->EIR;
		if ( timeout > ETH_SMI_TIMEOUT )
		{
			/* Return ERROR in case of timeout */
			success = false;
			break;
		}
	} while ( !( static_cast<bool_t>( temp_reg & ENET_EIR_MII_MASK ) ) );

	/* Clear the MII interrupt bit */
	ENET->EIR /*(ch)*/ = ENET_EIR_MII_MASK;

	return ( static_cast<bool_t>( success ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t K6x_Eth_Driver::SMIRead32Bits( uint16_t sys_reg_addr, uint32_t* read_ptr )
{
	uint16_t lo_word = 0xFFFFU;
	uint16_t hi_word = 0xFFFFU;

	bool_t success = false;

	bool_t lo_success = SMIRead16Bits( SMI_SYS_REG_TO_PHY_ADDR( sys_reg_addr ),
									   SMI_SYS_REG_TO_LO_REG_ADDR( sys_reg_addr ), &lo_word );

	bool_t hi_success = SMIRead16Bits( SMI_SYS_REG_TO_PHY_ADDR( sys_reg_addr ),
									   SMI_SYS_REG_TO_HI_REG_ADDR( sys_reg_addr ), &hi_word );

	if ( lo_success && hi_success )
	{
		*read_ptr = ( ( static_cast<uint32_t>( hi_word ) ) << 16U )
			| ( static_cast<uint32_t>( lo_word ) );
		success = true;
	}

	return ( static_cast<bool_t>( success ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
BOOL K6x_Eth_Driver::SMIWrite32Bits( uint16_t sys_reg_addr, const uint32_t* write_ptr )
{
	BOOL success = false;
	uint16_t lo_word = static_cast<uint16_t>( ( ( *write_ptr ) & 0xFFFFU ) );
	uint16_t hi_word = static_cast<uint16_t>( ( ( *write_ptr ) >> 16U ) );

	BOOL lo_success = SMIWrite16Bits( SMI_SYS_REG_TO_PHY_ADDR( sys_reg_addr ),
									  SMI_SYS_REG_TO_LO_REG_ADDR( sys_reg_addr ), &lo_word );

	BOOL hi_success = SMIWrite16Bits( SMI_SYS_REG_TO_PHY_ADDR( sys_reg_addr ),
									  SMI_SYS_REG_TO_HI_REG_ADDR( sys_reg_addr ), &hi_word );

	if ( lo_success && hi_success )
	{
		success = true;
	}
	return ( static_cast<bool_t>( success ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t* K6x_Eth_Driver::Get_Next_Avail_Desc_Buff( void )
{
	uint8_t* buf = NULL;
	int i;

	++s_enter;

#if 0 == ENET_HARDWARE_SHIFT
#if ETH_PAD_SIZE
	pbuf_header( p, -ETH_PAD_SIZE );/* drop the padding word */
#endif
#endif

	/* Get a DMA buffer into which we can write the data to send. */
	for ( i = 0; i < BUF_WAIT_ATTEMPTS; i++ )
	{
		/* First make sure ethernet is enabled. */
		if ( !( ENET->ECR & ENET_ECR_ETHEREN_MASK ) )
		{
			--s_enter;
		}

		if ( tx_bd[tx_next_buf].status & ENET_TX_BD_R )
		{
			/* wait for the buffer to become available */
			OS_Tasker::Delay( ETH_BLOCK_TIME_WAITING_FOR_OUTPUT_BUF_MS / OS_TICK_RATE_MS );
		}
		else
		{
#ifdef ENET_LITTLE_ENDIAN
			buf = ( uint8_t* )__REV( ( uint32_t )tx_bd[tx_next_buf].data );
#else
			buf = tx_bd[tx_next_buf].data;
#endif
#if !defined ( __KSZ8081__ ) && !defined ( __KSZ8041__ )
			BF_ASSERT( !InvalidRamPtr( buf ) );
#endif
			break;
		}
	}

	if ( NULL == buf )
	{
		--s_enter;
	}

	return ( buf );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void K6x_Eth_Driver::Wait_Tx_Ready( void )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void K6x_Eth_Driver::Tx_Frame( uint32_t l )
{
	/* Setup the buffer descriptor for transmission */
#ifdef ENET_LITTLE_ENDIAN
	tx_bd[tx_next_buf].length = __REVSH( l );// nbuf->length + ETH_HDR_LEN;
#else
	tx_bd[tx_next_buf].length = l;	// nbuf->length + ETH_HDR_LEN;
#endif

	tx_bd[tx_next_buf].status |= ( ENET_TX_BD_R | ENET_TX_BD_L );

	/* Request xmit process to MAC-NET */
	ENET->TDAR = ENET_TDAR_TDAR_MASK;

	/* Will check this frame for transmit errors. */
	volatile const enet_bd_t* tx_last_frame = tx_bd + tx_next_buf;

	tx_next_buf++;
	if ( tx_next_buf >= EMAC_Driver::ETH_TX_BUFNB )
	{
		tx_next_buf = 0;
	}

#if 0 == ENET_HARDWARE_SHIFT
#if ETH_PAD_SIZE
	pbuf_header( p, ETH_PAD_SIZE );	/* reclaim the padding word */
#endif
#endif

	LINK_STATS_INC( link.xmit );

	static unsigned s_wait_counter = 0;
	/* only one task can be here. wait until pkt is sent, then go ahead
	   semaphore released inside isr
	   start expiring semaphore: no more than 3 ticks
	   no blocking code*/
	if ( !ETHTxBinSemaphore->Pend( 0 ) )
	{
		++s_wait_counter;
		/* Block until available. */
		if ( !ETHTxBinSemaphore->Pend( ( ETH_BLOCK_TIME_WAITING_FOR_OUTPUT_MS / OS_TICK_RATE_MS ) ) )
		{
			--s_enter;
		}
	}

	err_t ret = ERR_OK;

	/* Check transmit failures. */
#ifdef ENHANCED_BD
	if ( tx_last_frame->ebd_status & ENET_TX_BD_STAT_TXE )
	{
		ret = ERR_IF;
		ret = ret;
	}
#endif
	--s_enter;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t K6x_Eth_Driver::Wait_Rx_Data_Available( void ) const
{
	return ( !( static_cast<bool_t>( ( rx_bd[rx_next_buf].status & ENET_RX_BD_E ) != 0U ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void K6x_Eth_Driver::Update_PHY_Config_To_MAC( uint8_t speed, uint8_t duplex )
{
	if ( ENET->RCR & ENET_RCR_RMII_MODE_MASK )
	{
		if ( speed == 1U )
		{
			ENET->RCR /*(ch)*/ |= ENET_RCR_RMII_10T_MASK;
		}
		else if ( speed == 2U )
		{
			ENET->RCR /*(ch)*/ &= ~( ENET_RCR_RMII_10T_MASK );
		}
		else
		{
			/* MISRA */
		}
	}
	if ( duplex == 0U )
	{
		ENET->RCR /*(ch)*/ |= ENET_RCR_DRT_MASK;
		ENET->TCR /*(ch)*/ &= ( uint32_t ) ~ENET_TCR_FDEN_MASK;
	}
	else if ( duplex == 1U )
	{
		ENET->RCR /*(ch)*/ &= ~ENET_RCR_DRT_MASK;
		ENET->TCR /*(ch)*/ |= ENET_TCR_FDEN_MASK;
	}
	else
	{
		/* MISRA */
	}

	/* If got 100 at half or full, then speed is 100 */
	enet_cfg.speed = ( speed == 2 ) ? MII_100BASET : MII_10BASET;

	/* Check if full duplex at either speed.*/
	enet_cfg.duplex = ( duplex == 1 ) ? MII_FDX : MII_HDX;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
data_states_t K6x_Eth_Driver::Get_Next_Rx_Pkt( uint32_t* new_payload, uint8_t* pkt_index )
{
	struct eth_hdr* ethhdr;
	struct pbuf* p;

	while ( 1 )
	{
		/* Wait for incoming data. */
		do
		{
			/* move received packet into a new pbuf */
			if ( ENET->ECR & ENET_ECR_ETHEREN_MASK )
			{
				p = low_level_input( &lwip_netif );
			}
			else
			{
				p = NULL;
			}

			/* no packet could be read, silently ignore this */
			if ( p == NULL )
			{
#if 0
				/* Make sure that ALL descriptors are empty. */
				static unsigned s_unconsumed_error_counter = 0;
				for ( unsigned i = 0; i < NUM_ENET_RX_BUFS; ++i )
				{
					if ( !( rx_bd[i].status & ENET_RX_BD_E ) )
					{
						/* This data was not consumed. */
						++s_unconsumed_error_counter;
					}
				}
#endif

				/* No packet could be read.  Wait a for an interrupt to tell us
				   there is more data available. */
				while ( !ETHRxBinSemaphore->Pend(
							( ETH_BLOCK_TIME_WAITING_FOR_INPUT_MS / OS_TICK_RATE_MS ) ) )
				{
					/* Wait till the semaphore period expires. */
				}
			}
			else
			{
#if !defined ( __KSZ8081__ ) && !defined ( __KSZ8041__ )
				if ( InvalidRamPtr( p ) || InvalidRamPtr( p->payload ) )
				{
					BF_ASSERT( false );
				}
#endif
			}
		} while ( NULL == p );

		/* points to packet payload, which starts with an Ethernet header */
		ethhdr = ( struct eth_hdr* )p->payload;

		switch ( htons( ethhdr->type ) )
		{
			/* IP or ARP packet? */
			case ETHTYPE_IP:
			case ETHTYPE_ARP:
				/* full packet send to tcpip_thread to process */
				if ( ERR_OK != lwip_netif.input( p, &lwip_netif ) )
				{
					LWIP_DEBUGF( NETIF_DEBUG, ( "ethernetif_input: IP input error\n" ) );
					pbuf_free( p );
					p = NULL;
				}
				break;

			default:
				pbuf_free( p );
				p = NULL;
				break;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void K6x_Eth_Driver::ETH_DMATxDescChainInit( uint8_t* TxBuff, uint32_t TxBuffCount )
{}

/*-----------------------------------------------------------*/

void K6x_Eth_Driver::vEMAC_TxISRHandler( void )
{
	#ifdef FREERTOS_V10
	static BaseType_t xHigherPriorityTaskWoken;
	#else
	static bool_t xHigherPriorityTaskWoken;
	#endif	// FREERTOS_V10

	/* Clear the interrupt. */
	ENET->EIR = ENET_EIR_TXF_MASK;

	ETHTxBinSemaphore->Post_From_ISR( &xHigherPriorityTaskWoken );
}

/*-----------------------------------------------------------*/

void K6x_Eth_Driver::vEMAC_RxISRHandler( void )
{
	#ifdef FREERTOS_V10
	static BaseType_t xHigherPriorityTaskWoken;
	#else
	static bool_t xHigherPriorityTaskWoken;
	#endif	// FREERTOS_V10

	/* Clear the interrupt. */
	ENET->EIR = ENET_EIR_RXF_MASK;

	ETHRxBinSemaphore->Post_From_ISR( &xHigherPriorityTaskWoken );
}

/*-----------------------------------------------------------*/

void K6x_Eth_Driver::vEMAC_ErrorISRHandler( void )
{
	/* Clear the interrupt. */
	uint32_t events = ENET->EIR;
	uint32_t mask = ENET->EIMR;

	/* Clear interrupts. */
	ENET->EIR = ENET->EIR & mask;

	/* Check ethernet bus error. This is bad and probably necessitates reset. */
	if ( events & ENET_EIMR_EBERR_MASK )
	{
		/* TODO attempt to log this incident somehow? */
		BF_ASSERT( false );
	}

	/* Babbling transmit should never happen. */
	if ( events & ENET_EIMR_BABT_MASK )
	{
		/* TODO make more graceful for production use. */
		BF_ASSERT( false );
	}

	/* If graceful stop occurs, then we can safely stop the EMAC. */
	if ( events & ENET_EIMR_GRA_MASK )
	{
		on_graceful_stop();
	}

#if 0
	/* Attempt recovery.  Not very sophisticated. */
	prvInitialiseDescriptors();

	ENET->RDAR = ENET_RDAR_RDAR_MASK;
#endif
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void K6x_Eth_Driver::vEMAC_TimerISRHandler( void )
{
	/* Do not expect this interrupt to happen... */
	BF_ASSERT( false );
}

/*-----------------------------------------------------------*/
