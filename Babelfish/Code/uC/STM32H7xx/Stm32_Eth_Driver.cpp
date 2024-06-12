/**
 *****************************************************************************************
 * @file            Stm32_Eth_Driver.cpp Header File for interface class.
 *
 * @brief           The file contains Stm32_Eth_Driver.cpp, an interface class required
 *
 * @details
 * @copyright       2020 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed here on.  This technical
 * information may not be reproduced or used without direct written permission from Eaton Corporation.
 *****************************************************************************************
 */

#include "Stm32_Eth_Driver.h"
#include "OS_Tasker.h"
#include "stm32h743xx.h"
#include "Babelfish_Assert.h"

OS_Binary_Semaphore* Stm32_Eth_Driver::ETHRxBinSemaphore = static_cast<OS_Binary_Semaphore*>
	( NULL );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Stm32_Eth_Driver::Stm32_Eth_Driver( uint8_t emac_select, uC_EMAC_INTERFACE_ETD emac_interface,
									uint8_t* mac_address, bool_t output_clock,
									dcid_emac_params_t const* emac_dcids,
									uC_BASE_EMAC_IO_PORT_STRUCT const* io_port ) :
	m_emac_pio( ( uC_Base::Self() )->Get_EMAC_Ctrl( emac_select ) ),
	m_emac_ctrl( m_emac_pio->reg_ctrl ),
	DMATxDescToSet( static_cast<ETH_DMADescTypeDef*>( NULL ) ),
	DMARxDescToGet( static_cast<ETH_DMADescTypeDef*>( NULL ) ),
	m_ETHRxBinSemaphore( static_cast<OS_Binary_Semaphore*>( NULL ) ),
	EMAC_Driver()
{

	m_ETHRxBinSemaphore = static_cast<OS_Binary_Semaphore*>( NULL );

	/* Enable SYSCFG clock */
	__SYSCFG_CLK_ENABLE();

	m_emac_pio = ( uC_Base::Self() )->Get_EMAC_Ctrl( emac_select );
	uC_PERIPH_IO_STRUCT ETH_MCO_CLOCK_OUT_GPIO =
	{ GPIOA, static_cast<uint8_t>( IO_PIN_8 ),
	  static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 ) };

	BF_ASSERT( m_emac_pio != 0U )

	m_emac_ctrl = m_emac_pio->reg_ctrl;

	uC_Base::Reset_Periph( &m_emac_pio->periph_def );
	uC_Base::Enable_Periph_Clock( &m_emac_pio->periph_def );

	if ( io_port == NULL )
	{
		io_port = m_emac_pio->default_port_io;
	}

	if ( emac_interface == uC_EMAC_MII_MODE )
	{
		if ( output_clock == true )
		{
			ETH_MCO_CLOCK_OUT_GPIO.alt_map = static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 );
			uC_IO_Config::Enable_Periph_Output_Pin( &ETH_MCO_CLOCK_OUT_GPIO );
			HAL_RCC_MCOConfig( RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1 );
		}
		else
		{
			// uC_IO_Config::Enable_Periph_Output_Pin( &ETH_MCO_CLOCK_OUT_GPIO );
		}

		// Ethernet PHY Interface Selection: MII
		/* Comment below line to configure the ethernet in MII mode ( LTK-11061 ) assuming default value of selection
		   bits is zero which sets it to MII on reset. */
		// MODIFY_REG( SYSCFG->PMCR, SYCFG_PMCR_EPIS_SEL_Msk, m_emac_pio->phy_if_sel );

		for ( uint8_t i = 0U; i < m_emac_pio->mii_pio_out_count; i++ )
		{
			uC_IO_Config::Enable_Periph_Output_Pin( io_port->mii_pio_out[i] );
		}
		for ( uint8_t i = 0U; i < m_emac_pio->mii_pio_in_count; i++ )
		{
			uC_IO_Config::Enable_Periph_Input_Pin( io_port->mii_pio_in[i] );
		}
	}
	else
	{
		if ( output_clock == true )
		{
			ETH_MCO_CLOCK_OUT_GPIO.alt_map = static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_0 );
			uC_IO_Config::Enable_Periph_Output_Pin( &ETH_MCO_CLOCK_OUT_GPIO );

			if ( SystemCoreClock == 480000000UL )
			{
				// MCO1 freq = 480 / 10 = 48Mhz
				HAL_RCC_MCOConfig( RCC_MCO1, RCC_MCO1SOURCE_PLL1QCLK, RCC_MCODIV_10 );
			}
			else if ( SystemCoreClock == 400000000UL )
			{
				// MCO1 freq = 400 / 8 = 50Mhz
				HAL_RCC_MCOConfig( RCC_MCO1, RCC_MCO1SOURCE_PLL1QCLK, RCC_MCODIV_8 );
			}
			else if ( SystemCoreClock == 240000000UL )
			{
				// MCO1 freq = 240 / 5 = 48Mhz
				HAL_RCC_MCOConfig( RCC_MCO1, RCC_MCO1SOURCE_PLL1QCLK, RCC_MCODIV_5 );
			}
			else if ( SystemCoreClock == 100000000UL )
			{
				// MCO1 freq = 100 / 2 = 50Mhz
				HAL_RCC_MCOConfig( RCC_MCO1, RCC_MCO1SOURCE_PLL1QCLK, RCC_MCODIV_2 );
			}
			else
			{
				BF_ASSERT( false );		/// > Choose correct master clock frequency.
			}

		}
		else
		{
			// uC_IO_Config::Enable_Periph_Output_Pin( &ETH_MCO_CLOCK_OUT_GPIO );
		}

		// Ethernet PHY Interface Selection: RMII
		MODIFY_REG( SYSCFG->PMCR, SYCFG_PMCR_EPIS_SEL_Msk, m_emac_pio->phy_if_sel );

		for ( uint8_t i = 0U; i < m_emac_pio->rmii_pio_out_count; i++ )
		{
			uC_IO_Config::Enable_Periph_Output_Pin( io_port->rmii_pio_out[i] );
		}
		for ( uint8_t i = 0U; i < m_emac_pio->rmii_pio_in_count; i++ )
		{
			uC_IO_Config::Enable_Periph_Input_Pin( io_port->rmii_pio_in[i] );
		}
	}

	/* Software reset */
	ETH_SoftwareReset();

	/* Wait for software reset */
	while ( ETH_GetSoftwareResetStatus() == SET )
	{}
	Init_Eth_Emac( mac_address, emac_dcids );

	uC_OS_Interrupt::Set_OS_Vect( &Stm32_Eth_Driver::ETH_Handler, m_emac_pio->irq_num,
								  static_cast<uint8_t>( uC_ETHERNET_PRIORITY ) );
	uC_OS_Interrupt::Enable_Int( m_emac_pio->irq_num );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::ETH_Init( void ) const
{
	uint32_t hclk;
	/* ------------------------ MDIO CSR Clock Range Configuration --------------------
	   Get the ETHERNET MACMDIOAR value*/
	uint32_t tmpreg = ETH->MACMDIOAR;

	/* Get hclk frequency value */
	hclk = HAL_RCC_GetHCLKFreq();

	/* Clear CSR Clock Range CR[2:0] bits */
	tmpreg &= ~ETH_MACMDIOAR_CR_Msk;

	/* Set CR bits depending on hclk value */
	if ( ( hclk >= 20000000U ) && ( hclk < 35000000U ) )
	{
		/* CSR Clock Range between 20-35 MHz */
		tmpreg |= static_cast<uint32_t>( ETH_MACMDIOAR_CR_DIV16 );
	}
	else if ( ( hclk >= 35000000U ) && ( hclk < 60000000U ) )
	{
		/* CSR Clock Range between 35-60 MHz */
		tmpreg |= static_cast<uint32_t>( ETH_MACMDIOAR_CR_DIV26 );
	}
	else if ( ( hclk >= 60000000U ) && ( hclk < 100000000U ) )
	{
		/* CSR Clock Range between 60-100 MHz */
		tmpreg |= ( uint32_t )ETH_MACMDIOAR_CR_DIV42;
	}
	else if ( ( hclk >= 100000000U ) && ( hclk < 150000000U ) )
	{
		/* CSR Clock Range between 100-150 MHz */
		tmpreg |= ( uint32_t )ETH_MACMDIOAR_CR_DIV62;
	}
	else
	{
		tmpreg |= ( uint32_t )ETH_MACMDIOAR_CR_DIV102;
	}

	/* Configure the CSR Clock Range */
	ETH->MACMDIOAR = ( uint32_t )tmpreg;

	/*------------------ MAC LPI 1US Tic Counter Configuration --------------------*/
	ETH->MAC1USTCR = ( ( ( uint32_t )hclk / ETH_MAC_US_TICK ) - 1U );

	/*------------------------ ETHERNET MACCR Configuration --------------------*/
	tmpreg = ETH->MACCR;
	tmpreg = ( m_ethmac_init.InterPacketGapVal |
			   m_ethmac_init.SourceAddrControl |
			   ( ( uint32_t )m_ethmac_init.ChecksumOffload << 27 ) |
			   ( ( uint32_t )m_ethmac_init.GiantPacketSizeLimitControl << 23 ) |
			   ( ( uint32_t )m_ethmac_init.Support2KPacket << 22 ) |
			   ( ( uint32_t )m_ethmac_init.CRCStripTypePacket << 21 ) |
			   ( ( uint32_t )m_ethmac_init.AutomaticPadCRCStrip << 20 ) |
			   ( ( uint32_t )( ( m_ethmac_init.Watchdog == DISABLE ) ? 1U : 0U ) << 19 ) |
			   ( ( uint32_t )( ( m_ethmac_init.Jabber == DISABLE ) ? 1U : 0U ) << 17 ) |
			   ( ( uint32_t )m_ethmac_init.JumboPacket << 16 ) |
			   m_ethmac_init.Speed |
			   m_ethmac_init.DuplexMode |
			   ( ( uint32_t )m_ethmac_init.LoopbackMode << 12 ) |
			   ( ( uint32_t )m_ethmac_init.CarrierSenseBeforeTransmit << 11 ) |
			   ( ( uint32_t )( ( m_ethmac_init.ReceiveOwn == DISABLE ) ? 1U : 0U ) << 10 ) |
			   ( ( uint32_t )m_ethmac_init.CarrierSenseDuringTransmit << 9 ) |
			   ( ( uint32_t )( ( m_ethmac_init.RetryTransmission == DISABLE ) ? 1U : 0U ) << 8 ) |
			   m_ethmac_init.BackOffLimit |
			   ( ( uint32_t )m_ethmac_init.DeferralCheck << 4 ) |
			   m_ethmac_init.PreambleLength );

	/* Write to MACCR */
	MODIFY_REG( ETH->MACCR, ETH_MACCR_MASK, tmpreg );

	/*------------------------ MACECR Configuration --------------------*/
	tmpreg = ETH->MACECR;
	tmpreg = ( ( m_ethmac_init.ExtendedInterPacketGapVal << 25 ) |
			   ( ( uint32_t )m_ethmac_init.ExtendedInterPacketGap << 24 ) |
			   ( ( uint32_t )m_ethmac_init.UnicastSlowProtocolPacketDetect << 18 ) |
			   ( ( uint32_t )m_ethmac_init.SlowProtocolDetect << 17 ) |
			   ( ( uint32_t )( ( m_ethmac_init.CRCCheckingRxPackets == DISABLE ) ? 1U : 0U ) << 16 ) |
			   m_ethmac_init.GiantPacketSizeLimit );

	/* Write to MACECR */
	MODIFY_REG( ETH->MACECR, ETH_MACECR_MASK, tmpreg );

	/*------------------------ MACWTR Configuration --------------------*/
	tmpreg = ETH->MACWTR;
	tmpreg = ( ( ( uint32_t )m_ethmac_init.ProgrammableWatchdog << 8 ) | m_ethmac_init.WatchdogTimeout );

	/* Write to MACWTR */
	MODIFY_REG( ETH->MACWTR, ETH_MACWTR_MASK, tmpreg );

	/*------------------------ MACTFCR Configuration --------------------*/
	tmpreg = ETH->MACTFCR;
	tmpreg = ( ( ( uint32_t )m_ethmac_init.TransmitFlowControl << 1 ) |
			   m_ethmac_init.PauseLowThreshold |
			   ( ( uint32_t )( ( m_ethmac_init.ZeroQuantaPause == DISABLE ) ? 1U : 0U ) << 7 ) |
			   ( m_ethmac_init.PauseTime << 16 ) );

	/* Write to MACTFCR */
	MODIFY_REG( ETH->MACTFCR, ETH_MACTFCR_MASK, tmpreg );

	/*------------------------ MACRFCR Configuration --------------------*/
	tmpreg = ETH->MACRFCR;
	tmpreg = ( ( uint32_t )m_ethmac_init.ReceiveFlowControl |
			   ( ( uint32_t )m_ethmac_init.UnicastPausePacketDetect << 1 ) );

	/* Write to MACRFCR */
	MODIFY_REG( ETH->MACRFCR, ETH_MACRFCR_MASK, tmpreg );

	/* ------------------------ MTLTQOMR Configuration --------------------
	   Write to MTLTQOMR*/
	MODIFY_REG( ETH->MTLTQOMR, ETH_MTLTQOMR_MASK, m_ethmac_init.TransmitQueueMode );

	/*------------------------ MTLRQOMR Configuration --------------------*/
	tmpreg = ETH->MTLRQOMR;
	tmpreg = ( m_ethmac_init.ReceiveQueueMode |
			   ( ( uint32_t )( ( m_ethmac_init.DropTCPIPChecksumErrorPacket == DISABLE ) ? 1U : 0U ) << 6 ) |
			   ( ( uint32_t )m_ethmac_init.ForwardRxErrorPacket << 4 ) |
			   ( ( uint32_t )m_ethmac_init.ForwardRxUndersizedGoodPacket << 3 ) );

	/* Write to MTLRQOMR */
	MODIFY_REG( ETH->MTLRQOMR, ETH_MTLRQOMR_MASK, tmpreg );

	/*------------------------   MAC Filters   -----------------------------------*/
	tmpreg = ETH->MACPFR;
	tmpreg = ( ( uint32_t )m_ethmac_filter_init.PromiscuousMode |
			   ( ( uint32_t )m_ethmac_filter_init.HashUnicast << 1 ) |
			   ( ( uint32_t )m_ethmac_filter_init.HashMulticast << 2 ) |
			   ( ( uint32_t )m_ethmac_filter_init.DestAddrInverseFiltering << 3 ) |
			   ( ( uint32_t )m_ethmac_filter_init.PassAllMulticast << 4 ) |
			   ( ( uint32_t )( ( m_ethmac_filter_init.BroadcastFilter == DISABLE ) ? 1U : 0U ) << 5 ) |
			   ( ( uint32_t )m_ethmac_filter_init.SrcAddrInverseFiltering << 8 ) |
			   ( ( uint32_t )m_ethmac_filter_init.SrcAddrFiltering << 9 ) |
			   ( ( uint32_t )m_ethmac_filter_init.HachOrPerfectFilter << 10 ) |
			   ( ( uint32_t )m_ethmac_filter_init.ReceiveAllMode << 31 ) |
			   m_ethmac_filter_init.ControlPacketsFilter );

	MODIFY_REG( ETH->MACPFR, ETH_MACPFR_MASK, tmpreg );

	/*-------------------------------- DMA Config ------------------------------*/

	/*------------------------ DMAMR Configuration --------------------*/
	MODIFY_REG( ETH->DMAMR, ETH_DMAMR_MASK, m_ethdma_init.DMAArbitration );

	/*------------------------ DMASBMR Configuration --------------------*/
	tmpreg = ETH->DMASBMR;
	tmpreg = ( ( ( uint32_t )m_ethdma_init.AddressAlignedBeats << 12 ) |
			   m_ethdma_init.BurstMode |
			   ( ( uint32_t )m_ethdma_init.RebuildINCRxBurst << 15 ) );

	MODIFY_REG( ETH->DMASBMR, ETH_DMASBMR_MASK, tmpreg );

	/*------------------------ DMACCR Configuration --------------------*/
	tmpreg = ETH->DMACCR;
	tmpreg = ( ( ( uint32_t )m_ethdma_init.PBLx8Mode << 16 ) |
			   m_ethdma_init.MaximumSegmentSize );

	MODIFY_REG( ETH->DMACCR, ETH_DMACCR_MASK, tmpreg );

	/*------------------------ DMACTCR Configuration --------------------*/
	tmpreg = ETH->DMACTCR;
	tmpreg = ( m_ethdma_init.TxDMABurstLength |
			   ( ( uint32_t )m_ethdma_init.SecondPacketOperate << 4 ) |
			   ( ( uint32_t )m_ethdma_init.TCPSegmentation << 12 ) );

	MODIFY_REG( ETH->DMACTCR, ETH_DMACTCR_MASK, tmpreg );

	/*------------------------ DMACRCR Configuration --------------------*/
	tmpreg = ETH->DMACRCR;
	tmpreg = ( ( ( uint32_t )m_ethdma_init.FlushRxPacket << 31 ) |
			   m_ethdma_init.RxDMABurstLength );

	/* Write to DMACRCR */
	MODIFY_REG( ETH->DMACRCR, ETH_DMACRCR_MASK, tmpreg );

	/* SET DSL to 64 bit */
	MODIFY_REG( ETH->DMACCR, ETH_DMACCR_DSL, ETH_DMACCR_DSL_64BIT );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::ETH_Load_Defaults()
{
	/* ETH_InitStruct members default value
	   ------------------------   MAC   -----------------------------------*/
	m_ethmac_init.AutomaticPadCRCStrip = DISABLE;
	m_ethmac_init.BackOffLimit = ETH_BACKOFFLIMIT_10;
	m_ethmac_init.CarrierSenseBeforeTransmit = DISABLE;
	m_ethmac_init.CarrierSenseDuringTransmit = DISABLE;
	m_ethmac_init.ChecksumOffload = DISABLE;
	m_ethmac_init.CRCCheckingRxPackets = ENABLE;
	m_ethmac_init.CRCStripTypePacket = ENABLE;
	m_ethmac_init.DeferralCheck = DISABLE;
	m_ethmac_init.DropTCPIPChecksumErrorPacket = ENABLE;
	m_ethmac_init.DuplexMode = ETH_HALFDUPLEX_MODE;
	m_ethmac_init.ExtendedInterPacketGap = DISABLE;
	m_ethmac_init.ExtendedInterPacketGapVal = 0x0;
	m_ethmac_init.ForwardRxErrorPacket = DISABLE;
	m_ethmac_init.ForwardRxUndersizedGoodPacket = DISABLE;
	m_ethmac_init.GiantPacketSizeLimit = 0x618;
	m_ethmac_init.GiantPacketSizeLimitControl = DISABLE;
	m_ethmac_init.InterPacketGapVal = ETH_INTERPACKETGAP_96BIT;
	m_ethmac_init.Jabber = ENABLE;
	m_ethmac_init.JumboPacket = DISABLE;
	m_ethmac_init.LoopbackMode = DISABLE;
	m_ethmac_init.PauseLowThreshold = ETH_PAUSELOWTHRESHOLD_MINUS_4;
	m_ethmac_init.PauseTime = 0x0;
	m_ethmac_init.PreambleLength = ETH_PREAMBLELENGTH_7;
	m_ethmac_init.ProgrammableWatchdog = DISABLE;
	m_ethmac_init.ReceiveFlowControl = DISABLE;
	m_ethmac_init.ReceiveOwn = ENABLE;
	m_ethmac_init.ReceiveQueueMode = ETH_RECEIVESTOREFORWARD;
	m_ethmac_init.RetryTransmission = ENABLE;
	m_ethmac_init.SlowProtocolDetect = DISABLE;
	m_ethmac_init.SourceAddrControl = ETH_SOURCEADDRESS_DISABLE;
	m_ethmac_init.Speed = ETH_SPEED_10M;
	m_ethmac_init.Support2KPacket = DISABLE;
	m_ethmac_init.TransmitQueueMode = ETH_TRANSMITSTOREFORWARD;
	m_ethmac_init.TransmitFlowControl = DISABLE;
	m_ethmac_init.UnicastPausePacketDetect = DISABLE;
	m_ethmac_init.UnicastSlowProtocolPacketDetect = DISABLE;
	m_ethmac_init.Watchdog = ENABLE;
	m_ethmac_init.WatchdogTimeout = ETH_MACWTR_WTO_2KB;
	m_ethmac_init.ZeroQuantaPause = DISABLE;

	/*------------------------   MAC Filter   -----------------------------------*/
	m_ethmac_filter_init.BroadcastFilter = ENABLE;	/// >ENABLE: Pass all received broadcast frames
	m_ethmac_filter_init.ControlPacketsFilter = ETH_MACPFR_PCF_BLOCKALL;
	m_ethmac_filter_init.DestAddrInverseFiltering = DISABLE;
	m_ethmac_filter_init.HachOrPerfectFilter = DISABLE;
	m_ethmac_filter_init.HashMulticast = DISABLE;
	m_ethmac_filter_init.HashUnicast = DISABLE;
	m_ethmac_filter_init.PassAllMulticast = ENABLE;
	m_ethmac_filter_init.PromiscuousMode = DISABLE;
	m_ethmac_filter_init.ReceiveAllMode = DISABLE;
	m_ethmac_filter_init.SrcAddrFiltering = DISABLE;
	m_ethmac_filter_init.SrcAddrInverseFiltering = DISABLE;

	/*------------------------   DMA   -----------------------------------*/
	m_ethdma_init.AddressAlignedBeats = ENABLE;
	m_ethdma_init.BurstMode = ETH_BURSTLENGTH_FIXED;
	m_ethdma_init.DMAArbitration = ETH_DMAARBITRATION_RX1_TX1;
	m_ethdma_init.FlushRxPacket = DISABLE;
	m_ethdma_init.PBLx8Mode = DISABLE;
	m_ethdma_init.RebuildINCRxBurst = DISABLE;
	m_ethdma_init.RxDMABurstLength = ETH_RXDMABURSTLENGTH_32BEAT;
	m_ethdma_init.SecondPacketOperate = DISABLE;
	m_ethdma_init.TxDMABurstLength = ETH_TXDMABURSTLENGTH_32BEAT;
	m_ethdma_init.TCPSegmentation = DISABLE;
	m_ethdma_init.MaximumSegmentSize = 1500;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::Init_EMAC_Hardware( void )
{
	/* Enable transmit state machine of the MAC for transmission */
	ETH_MACTransmissionCmd( ENABLE );
	/* Flush Transmit FIFO */
	ETH_FlushTransmitFIFO();
	/* Enable receive state machine of the MAC for reception */
	ETH_MACReceptionCmd( ENABLE );
	/* Start DMA transmission */
	ETH_DMATransmissionCmd( ENABLE );
	/* Start DMA reception */
	ETH_DMAReceptionCmd( ENABLE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::ETH_MACTransmissionCmd( FunctionalState NewState ) const
{
	if ( NewState != DISABLE )
	{
		/* Enable the MAC transmission */
		SET_BIT( ETH->MACCR, ETH_MACCR_TE );
	}
	else
	{
		/* Disable the MAC transmission */
		CLEAR_BIT( ETH->MACCR, ETH_MACCR_TE );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::ETH_MACReceptionCmd( FunctionalState NewState ) const
{
	if ( NewState != DISABLE )
	{
		/* Enable the MAC reception */
		SET_BIT( ETH->MACCR, ETH_MACCR_RE );
	}
	else
	{
		/* Disable the MAC reception */
		CLEAR_BIT( ETH->MACCR, ETH_MACCR_RE );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::Init_Buff_Desc( uint8_t* Tx_Buff, uint32_t** rx_payload,
									   uint16_t rx_buffer_size )
{
	ETH_DMARxDescChainInit( rx_payload, rx_buffer_size );
	ETH_DMATxDescChainInit( Tx_Buff, EMAC_Driver::ETH_TX_BUFNB );

	/* Set Receive Buffers Length (must be a multiple of 4) */
	MODIFY_REG( ETH->DMACRCR, ETH_DMACRCR_RBSZ, ( rx_buffer_size << 1 ) );

	// Note: We have removed RAM_Diag::uC_RAM::Mark_As_Volatile() code from here because
	// lwip rx buffer , tx buffer and descriptors are moved to non cacheable region. This change is only
	// applicable to those cotrollers who has enabled cache and made lwip rx, tx and descriptors
	// as non cacheable. We have excluded these buffers from main ram region thus we can not perform
	// ram test over this region.
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::ETH_DMARxDescChainInit( uint32_t** rx_payload, uint16_t rx_buffer_size )
{
	for ( uint8_t idx = 0U; idx < EMAC_Driver::ETH_RX_BUFNB; idx++ )
	{
		DMARxDescToGet = &DMARxDscrTab[idx];

		WRITE_REG( DMARxDescToGet->DESC0, 0x0 );
		WRITE_REG( DMARxDescToGet->DESC1, 0x0 );
		WRITE_REG( DMARxDescToGet->DESC2, 0x0 );
		WRITE_REG( DMARxDescToGet->DESC3, 0x0 );
		WRITE_REG( DMARxDescToGet->BackupAddr0, 0x0 );
		WRITE_REG( DMARxDescToGet->BackupAddr1, 0x0 );

		/* write buffer address to DESC0 */
		WRITE_REG( DMARxDescToGet->DESC0, ( uint32_t )rx_payload[idx] );

		/* store buffer address */
		WRITE_REG( DMARxDescToGet->BackupAddr0, ( uint32_t )rx_payload[idx] );

		/* set buffer address valid bit to RDES3 */
		WRITE_REG( DMARxDescToGet->DESC3, ETH_DMARXNDESCRF_BUF1V );

		/* set OWN bit to RDES3 */
		SET_BIT( DMARxDescToGet->DESC3, ETH_DMARXNDESCRF_OWN );

		/* Interrupt Enabled on Completion */
		ETH_DMARxDescReceiveITConfig( &DMARxDscrTab[idx], ENABLE );
	}

	/* Set Receive Descriptor Ring Length */
	WRITE_REG( ETH->DMACRDRLR, ( ( uint32_t )( ETH_RX_BUFNB - 1 ) ) );

	/* Set Receive Descriptor List Address */
	WRITE_REG( ETH->DMACRDLAR, ( uint32_t ) DMARxDscrTab );

	/* Set Receive Descriptor Tail pointer Address */
	WRITE_REG( ETH->DMACRDTPR, ( ( uint32_t )( DMARxDscrTab + ( ( uint32_t )( ETH_RX_BUFNB - 1 ) ) ) ) );

	/* At initial the current descriptor shall be at position reset or 0 */
	DMARxDescToGet = &DMARxDscrTab[0U];
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Stm32_Eth_Driver::~Stm32_Eth_Driver( void )
{
	m_emac_pio = reinterpret_cast<uC_BASE_EMAC_IO_STRUCT const*>( NULL );
	m_emac_ctrl = reinterpret_cast<ETH_TypeDef*>( NULL );
	DMATxDescToSet = reinterpret_cast<ETH_DMADescTypeDef*>( NULL );
	DMARxDescToGet = reinterpret_cast<ETH_DMADescTypeDef*>( NULL );
	m_ETHRxBinSemaphore = reinterpret_cast<OS_Binary_Semaphore*>( NULL );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::ETH_MACAddressConfig( uint32_t MacAddr, uint8_t* Addr )
{
	uint32_t tmpreg;
	uint8_t* ETH_MAC_ADDR_HBASE = reinterpret_cast<uint8_t*>( const_cast<uint32_t*>( &ETH->MACA0HR ) );
	uint8_t* ETH_MAC_ADDR_LBASE = reinterpret_cast<uint8_t*>( const_cast<uint32_t*>( &ETH->MACA0LR ) );

	/* Calculate the selected MAC address high register */
	tmpreg =
		( static_cast<uint32_t>( ( ( ( ( static_cast<uint32_t>( Addr[5U] ) ) << 8U ) ) )
								 | ( static_cast<uint32_t>( Addr[4U] ) ) ) );
	/* Load the selected MAC address high register */
	( *( reinterpret_cast<__IO uint32_t*>( ETH_MAC_ADDR_HBASE + MacAddr ) ) ) = tmpreg;

	/* Calculate the selected MAC address low register */
	tmpreg =
		( static_cast<uint32_t>( ( ( ( ( static_cast<uint32_t>( Addr[3U] ) ) << 24U ) ) )
								 | ( ( ( ( static_cast<uint32_t>
										   ( Addr[2U] ) ) << 16U ) ) )
								 | ( ( ( ( static_cast<uint32_t>
										   ( Addr[1U] ) ) << 8U ) ) )
								 | ( static_cast<uint32_t>( Addr[0U] ) ) ) );

	/* Load the selected MAC address low register */
	( *( reinterpret_cast<__IO uint32_t*>( ETH_MAC_ADDR_LBASE + MacAddr ) ) ) = tmpreg;

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::Get_MAC_Address( uint8_t* Addr ) const
{
	Read_MAC_Address_Reg( ETH_MAC_Address_0, Addr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::Read_MAC_Address_Reg( uint32_t MacAddrOffSet, uint8_t* Addr ) const
{
	uint32_t tmpreg;
	uint8_t* ETH_MAC_ADDR_HBASE = reinterpret_cast<uint8_t*>( const_cast<uint32_t*>( &ETH->MACA0HR ) );
	uint8_t* ETH_MAC_ADDR_LBASE = reinterpret_cast<uint8_t*>( const_cast<uint32_t*>( &ETH->MACA0LR ) );

	/* Get the selectecd MAC address high register */
	tmpreg = ( *( reinterpret_cast<__IO uint32_t*>( ETH_MAC_ADDR_HBASE + MacAddrOffSet ) ) );

	/* Calculate the selectecd MAC address buffer */
	Addr[5U] = static_cast<uint8_t>
		( ( ( tmpreg >> 8U ) & ( static_cast<uint8_t>( 0xFFU ) ) ) );
	Addr[4U] = static_cast<uint8_t>( tmpreg & ( static_cast<uint8_t>( 0xFFU ) ) );
	/* Load the selectecd MAC address low register */
	tmpreg = ( *( reinterpret_cast<__IO uint32_t*>( ETH_MAC_ADDR_LBASE + MacAddrOffSet ) ) );
	/* Calculate the selectecd MAC address buffer */
	Addr[3U] = static_cast<uint8_t>
		( ( tmpreg >> 24U ) & ( static_cast<uint8_t>( 0xFFU ) ) );
	Addr[2U] = static_cast<uint8_t>
		( ( tmpreg >> 16U ) & ( static_cast<uint8_t>( 0xFFU ) ) );
	Addr[1U] = static_cast<uint8_t>( ( tmpreg >> 8U ) & ( static_cast<uint8_t>( 0xFFU ) ) );
	Addr[0U] = static_cast<uint8_t>( tmpreg & ( static_cast<uint8_t>( 0xFFU ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::ETH_DMARxDescReceiveITConfig( ETH_DMADescTypeDef* DMARxDesc,
													 FunctionalState NewState ) const
{
	if ( NewState != DISABLE )
	{
		/* Enable the DMA Rx Desc receive interrupt */
		SET_BIT( DMARxDesc->DESC3, ETH_DMARXNDESCRF_IOC );
	}
	else
	{
		/* Disable the DMA Rx Desc receive interrupt */
		CLEAR_BIT( DMARxDesc->DESC3, ETH_DMARXNDESCRF_IOC );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::ETH_SoftwareReset( void ) const
{
	/* Set the SWR bit: resets all MAC subsystem internal registers and logic After reset all the registers holds their
	   respective reset values*/
	ETH->DMAMR |= ETH_DMAMR_SWR;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
FlagStatus Stm32_Eth_Driver::ETH_GetSoftwareResetStatus( void ) const
{
	FlagStatus bitstatus = RESET;

	if ( ( ETH->DMAMR & ETH_DMAMR_SWR_Msk ) == ETH_DMAMR_SWR )
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
void Stm32_Eth_Driver::ETH_DMAClearITPendingBit( uint32_t ETH_DMA_IT )
{
	/* Clear the selected ETHERNET DMA IT */
	ETH->DMACSR = ( static_cast<uint32_t>( ETH_DMA_IT ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
FlagStatus Stm32_Eth_Driver::ETH_GetDMAFlagStatus( uint32_t ETH_DMA_FLAG )
{
	FlagStatus bitstatus = RESET;

	if ( ( static_cast<bool_t>( ( ETH->DMACSR & ETH_DMA_FLAG ) != 0U ) ) )
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
void Stm32_Eth_Driver::ETH_FlushTransmitFIFO( void ) const
{
	/* Set the Flush Transmit FIFO bit */
	SET_BIT( ETH->MTLTQOMR, ETH_MTLTQOMR_FTQ );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::ETH_DMATransmissionCmd( FunctionalState NewState ) const
{
	if ( NewState != DISABLE )
	{
		/* Enable the DMA transmission */
		SET_BIT( ETH->DMACTCR, ETH_DMACTCR_ST );
	}
	else
	{
		/* Disable the DMA transmission */
		CLEAR_BIT( ETH->DMACTCR, ETH_DMACTCR_ST );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::ETH_DMAReceptionCmd( FunctionalState NewState ) const
{
	if ( NewState != DISABLE )
	{
		/* Enable the DMA reception */
		SET_BIT( ETH->DMACRCR, ETH_DMACRCR_SR );
	}
	else
	{
		/* Disable the DMA reception */
		CLEAR_BIT( ETH->DMACRCR, ETH_DMACRCR_SR );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::ETH_DMAITConfig( uint32_t ETH_DMA_IT, FunctionalState NewState ) const
{
	if ( NewState != DISABLE )
	{
		/* Enable the selected ETHERNET DMA interrupts */
		SET_BIT( ETH->DMACIER, ETH_DMA_IT );
	}
	else
	{
		/* Disable the selected ETHERNET DMA interrupts */
		CLEAR_BIT( ETH->DMACIER, ETH_DMA_IT );
	}
}

#ifndef USE_Delay
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::ETH_Delay( __IO uint32_t nCount )
{
	for ( __IO uint32_t index = nCount; index != 0U; index-- )
	{}
}

#endif  /* USE_Delay*/

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Stm32_Eth_Driver::SMIRead16Bits( uint16_t phy_addr, uint16_t reg_addr,
										uint16_t* read_ptr )
{
	volatile uint32_t timeout = 0U;
	bool_t success = true;

	/* Get the ETHERNET MACMIIAR value */
	uint32_t temp_reg = ETH->MACMDIOAR;

	/* Keep only the CSR Clock Range CR[2:0] bits value */
	temp_reg &= ETH_MACMDIOAR_CR_Msk;	// 0x00000F00

	/* Prepare the MII address register value Set the PHY device address   //0x03E00000*/
	temp_reg |= ( ( static_cast<uint32_t>( phy_addr ) << 21U ) & ETH_MACMDIOAR_PA_Msk );

	/* Set the PHY register address //0x001F0000 */
	temp_reg |= ( ( static_cast<uint32_t>( reg_addr ) << 16U ) & ETH_MACMDIOAR_RDA_Msk );

	/* Set the read mode     //((uint32_t)0x00000002) */
	temp_reg &= ~ETH_MACMDIOAR_MOC_Msk;
	temp_reg |= ETH_MACMDIOAR_MOC_RD;

	/*  Set the MII Busy bit  //((uint32_t)0x00000001) */
	temp_reg |= ETH_MACMDIOAR_MB;

	/* Write the result value into the MII Address register */
	ETH->MACMDIOAR = temp_reg;

	/* Check for the Busy flag */
	do
	{
		timeout++;
		temp_reg = ETH->MACMDIOAR;
		if ( timeout > ETH_SMI_TIMEOUT )
		{
			/* Return ERROR in case of timeout */
			success = false;
			break;
		}
	} while ( static_cast<bool_t>( temp_reg & ETH_MACMDIOAR_MB ) );

	/* Return data register value */
	if ( success )
	{
		*read_ptr = static_cast<uint16_t>( ETH->MACMDIODR );
	}
	return ( static_cast<bool_t>( success ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Stm32_Eth_Driver::SMIWrite16Bits( uint16_t phy_addr, uint16_t reg_addr,
										 const uint16_t* write_ptr )
{
	__IO
	uint32_t timeout = 0U;
	bool_t success = true;

	/* Get the ETHERNET MACMIIAR value */
	uint32_t temp_reg = ETH->MACMDIOAR;

	/* Keep only the CSR Clock Range CR[2:0] bits value */
	temp_reg &= ETH_MACMDIOAR_CR_Msk;	// 0x00000F00

	/* Prepare the MII address register value Set the PHY device address   //0x03E00000*/
	temp_reg |= ( ( static_cast<uint32_t>( phy_addr ) << 21U ) & ETH_MACMDIOAR_PA_Msk );

	/* Set the PHY register address //0x001F0000 */
	temp_reg |= ( ( static_cast<uint32_t>( reg_addr ) << 16U ) & ETH_MACMDIOAR_RDA_Msk );

	/* Set the read mode     //((uint32_t)0x00000002) */
	temp_reg &= ~ETH_MACMDIOAR_MOC_Msk;
	temp_reg |= ETH_MACMDIOAR_MOC_WR;

	/*  Set the MII Busy bit  //((uint32_t)0x00000001) */
	temp_reg |= ETH_MACMDIOAR_MB;

	/* Write data register */
	ETH->MACMDIODR = static_cast<uint16_t>( *write_ptr );

	/* Write address register and trigger transfer */
	ETH->MACMDIOAR = temp_reg;

	/* Check for the Busy flag */
	do
	{
		timeout++;
		temp_reg = ETH->MACMDIOAR;
		if ( timeout > ETH_SMI_TIMEOUT )
		{
			/* Return ERROR in case of timeout */
			success = false;
			break;
		}
	} while ( static_cast<bool_t>( temp_reg & ETH_MACMDIOAR_MB ) );

	return ( static_cast<bool_t>( success ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Stm32_Eth_Driver::SMIRead32Bits( uint16_t sys_reg_addr, uint32_t* read_ptr )
{
	uint16_t lo_word = 0xFFFFU;
	uint16_t hi_word = 0xFFFFU;

	bool_t success = false;

	bool_t lo_success = SMIRead16Bits( SMI_SYS_REG_TO_PHY_ADDR( sys_reg_addr ),
									   SMI_SYS_REG_TO_LO_REG_ADDR( sys_reg_addr ),
									   &lo_word );

	bool_t hi_success = SMIRead16Bits( SMI_SYS_REG_TO_PHY_ADDR( sys_reg_addr ),
									   SMI_SYS_REG_TO_HI_REG_ADDR( sys_reg_addr ),
									   &hi_word );

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
BOOL Stm32_Eth_Driver::SMIWrite32Bits( uint16_t sys_reg_addr,
									   const uint32_t* write_ptr )
{
	BOOL success = false;
	uint16_t lo_word = static_cast<uint16_t>( ( ( *write_ptr ) & 0xFFFFU ) );
	uint16_t hi_word = static_cast<uint16_t>( ( ( *write_ptr ) >> 16U ) );

	BOOL lo_success = SMIWrite16Bits( SMI_SYS_REG_TO_PHY_ADDR( sys_reg_addr ),
									  SMI_SYS_REG_TO_LO_REG_ADDR( sys_reg_addr ),
									  &lo_word );

	BOOL hi_success = SMIWrite16Bits( SMI_SYS_REG_TO_PHY_ADDR( sys_reg_addr ),
									  SMI_SYS_REG_TO_HI_REG_ADDR( sys_reg_addr ),
									  &hi_word );

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
uint8_t* Stm32_Eth_Driver::Get_Next_Avail_Desc_Buff( void )
{
	return ( reinterpret_cast<uint8_t*>( DMATxDescToSet->DESC0 ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::Wait_Tx_Ready( void )
{
	uint32_t ulAttempts = 0UL;

	while ( static_cast<bool_t>( ( DMATxDescToSet->DESC3 & ETH_DMATXNDESCRF_OWN ) != 0U ) )
	{
		/* Return ERROR: OWN bit set */
		OS_Tasker::Delay( 1U );

		ulAttempts++;

		if ( ulAttempts > 90U )
		{
			DMATxDescToSet->DESC3 &= ~ETH_DMATXNDESCRF_OWN;
			break;
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::Tx_Frame( uint32_t l )
{
	/* Set header or buffer 1 Length */
	MODIFY_REG( DMATxDescToSet->DESC2, ETH_DMATXNDESCRF_B1L, l );

	/* Set payload length */
	MODIFY_REG( DMATxDescToSet->DESC3, ETH_DMATXNDESCRF_FL, l );

	/* Setting the last segment and first segment bits (in this case a frame is transmitted in one descriptor) */
	DMATxDescToSet->DESC3 |= ( ETH_DMATXNDESCRF_LD | ETH_DMATXNDESCRF_FD );

	/* CRC and Pad Insertion */
	MODIFY_REG( DMATxDescToSet->DESC3, ETH_DMATXNDESCRF_CPC, ETH_CRC_PAD_INSERT );

	/* Mark it as NORMAL descriptor */
	CLEAR_BIT( DMATxDescToSet->DESC3, ETH_DMATXNDESCRF_CTXT );

	/* Clear Interrupt on completition bit */
	CLEAR_BIT( DMATxDescToSet->DESC2, ETH_DMATXNDESCRF_IOC );

	/* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
	DMATxDescToSet->DESC3 |= ETH_DMATXNDESCRF_OWN;

	__DSB();

	/* When Tx Buffer unavailable flag is set: clear it and resume transmission */
	if ( ( static_cast<bool_t>( ( ETH->DMACSR & ETH_DMACSR_TBU ) != 0U ) ) )
	{
		/* Clear TBUS ETHERNET DMA flag */
		ETH->DMACSR = ETH_DMACSR_TBU;

	}

	/* Start transmission issue a poll command to Tx DMA by writing address of next immediate free descriptor*/
	ETH->DMACTDTPR = DMATxDescToSet->DESC1;

	/* Selects the next DMA Tx descriptor list for next buffer to send */
	DMATxDescToSet = ( reinterpret_cast<ETH_DMADescTypeDef*>( DMATxDescToSet->DESC1 ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Stm32_Eth_Driver::Wait_Rx_Data_Available( void ) const
{
	return ( !( static_cast<bool_t>( ( DMARxDescToGet->DESC3 & ETH_DMARXNDESCRF_OWN ) != 0U ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::Config_Eth_DMA_Init( void ) const
{
	/* Enable the Ethernet Rx Interrupt */
	ETH_DMAITConfig( ( static_cast<uint32_t>( ETH_DMACIER_NIE | ETH_DMACIER_RIE ) ), ENABLE );
}

void Stm32_Eth_Driver::ETH_DMATxDescChecksumInsertionConfig( ETH_DMADescTypeDef* DMATxDesc,
															 uint32_t DMATxDesc_Checksum )
{
	/* Set the selected DMA Tx desc checksum insertion control */
	DMATxDesc->DESC3 |= ( DMATxDesc_Checksum & ETH_DMATXNDESCRF_CIC );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::Update_PHY_Config_To_MAC( uint8_t speed, uint8_t duplex )
{
	uint32_t tmpreg;

	tmpreg = ETH->MACCR;
	if ( speed == 1U )
	{
		m_ethmac_init.Speed = ETH_SPEED_10M;
	}
	else if ( speed == 2U )
	{
		m_ethmac_init.Speed = ETH_SPEED_100M;
	}
	else
	{
		m_ethmac_init.Speed = ETH_SPEED_10M;
	}

	if ( duplex == 0U )
	{
		m_ethmac_init.DuplexMode = ETH_HALFDUPLEX_MODE;
	}
	else if ( duplex == 1U )
	{
		m_ethmac_init.DuplexMode = ETH_FULLDUPLEX_MODE;
	}
	else
	{
		/* MISRA */
	}

	// clear speed and duplex register setting
	tmpreg &= ~( ETH_SPEED_100M | ETH_FULLDUPLEX_MODE );

	// Set new speed and duplex mode
	tmpreg |= ( static_cast<uint32_t>( m_ethmac_init.Speed |
									   m_ethmac_init.DuplexMode ) );

	/* Write to ETHERNET MACCR */
	ETH->MACCR = static_cast<uint32_t>( tmpreg );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
data_states_t Stm32_Eth_Driver::Get_Next_Rx_Pkt( uint32_t* new_payload, uint8_t* pkt_index )
{
	static data_states_t statemachine = INIT;
	data_states_t return_value;
	static uint8_t dma_desc_num = 0U;
	bool_t error;
	bool_t fs_bit;
	bool_t ls_bit;
	bool_t dma_suspend;
	static bool_t first_packet_received = false;

	while ( false == Wait_Rx_Data_Available() )
	{
		Stm32_Eth_Driver::ETHRxBinSemaphore->Pend(
			( ETH_BLOCK_TIME_WAITING_FOR_INPUT_MS / OS_TICK_RATE_MS ) );
	}

	// Receive normal descriptor (write-back format)
	fs_bit = static_cast<bool_t>( DMARxDescToGet->DESC3 & ETH_DMARXNDESCWBF_FD );
	ls_bit = static_cast<bool_t>( DMARxDescToGet->DESC3 & ETH_DMARXNDESCWBF_LD );

	error =
		( ( ( static_cast<bool_t>( ( DMARxDescToGet->DESC3 & ETH_DMARXNDESCWBF_ES ) ) ) != 0U ) ||
		  ( !first_packet_received && !fs_bit ) || ( fs_bit && first_packet_received ) ||
		  ( new_payload == _NULL ) );

	dma_suspend = static_cast<bool_t>( ( ETH->DMACSR & ETH_DMACSR_RBU ) != 0U );

	/* set OWN bit of descriptor */
	DMARxDescToGet->DESC3 = ETH_DMARXNDESCWBF_OWN;

	if ( dma_suspend )
	{
		/* Clear RBUS ETHERNET DMA flag */
		ETH->DMACSR = ETH_DMACSR_RBU;
	}
	if ( _NULL != new_payload )
	{
		DMARxDescToGet->DESC0 = reinterpret_cast<uint32_t>( new_payload );
		DMARxDescToGet->DESC1 = 0;
		SET_BIT( DMARxDescToGet->DESC3, ETH_DMARXNDESCRF_BUF1V );
	}

	/* Interrupt Enabled on Completion */
	ETH_DMARxDescReceiveITConfig( DMARxDescToGet, ENABLE );

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

	DMARxDescToGet = &DMARxDscrTab[dma_desc_num];

	/* Set the Tail pointer address to resume DMA reception */
	WRITE_REG( ETH->DMACRDTPR, ( uint32_t )DMARxDescToGet );

	return_value = statemachine;
	if ( ( ERROR_STATE == statemachine ) || ( LAST == statemachine ) ||
		 ( SINGLE_PKT_FRM == statemachine ) )
	{
		statemachine = INIT;
	}

	return ( return_value );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::ETH_DMATxDescChainInit( uint8_t* TxBuff, uint32_t TxBuffCount )
{
	uint32_t count;
	ETH_DMADescTypeDef* DMATxDesc;

	/* Set Transmit Descriptor Ring Length */
	WRITE_REG( ETH->DMACTDRLR, ( TxBuffCount - 1 ) );

	/* Set Transmit Descriptor List Address */
	WRITE_REG( ETH->DMACTDLAR, ( uint32_t ) DMATxDscrTab );

	/* Set Transmit Descriptor Tail pointer */
	WRITE_REG( ETH->DMACTDTPR, ( uint32_t ) DMATxDscrTab );

	/* Set the DMATxDescToSet pointer with the first one of the DMATxDescTab list */
	DMATxDescToSet = DMATxDscrTab;

	/* Fill each DMATxDesc descriptor with the right values */
	for ( count = 0U; count < TxBuffCount; count++ )
	{
		/* Get the pointer on the ith member of the Tx Desc list */
		DMATxDesc = DMATxDscrTab + count;

		/* Reset Tx descriptor */
		WRITE_REG( DMATxDesc->DESC0, 0x0 );
		WRITE_REG( DMATxDesc->DESC1, 0x0 );
		WRITE_REG( DMATxDesc->DESC2, 0x0 );
		WRITE_REG( DMATxDesc->DESC3, 0x0 );

		/* Set header or buffer 1 address */
		WRITE_REG( DMATxDesc->DESC0, reinterpret_cast<uint32_t>( ( &TxBuff[count * MAX_PACKET_SIZE] ) ) );

		/* Initialize the next descriptor with the Next Desciptor Polling Enable */
		if ( count < ( TxBuffCount - 1U ) )
		{
			/* Set next descriptor address register with next descriptor base address */
			DMATxDesc->DESC1 =
				reinterpret_cast<uint32_t>( ( DMATxDscrTab + count + 1U ) );
		}
		else
		{
			/* For last descriptor, set next descriptor address register equal to the first descriptor base address */
			DMATxDesc->DESC1 = reinterpret_cast<uint32_t>( DMATxDscrTab );
		}

		/* Set header or buffer 1 Length */
		MODIFY_REG( DMATxDesc->DESC2, ETH_DMATXNDESCRF_B1L, MAX_PACKET_SIZE );

		/* Set buffer 2 Length */
		MODIFY_REG( DMATxDesc->DESC2, ETH_DMATXNDESCRF_B2L, 0x0U );

		/* This field is equal to the length of the packet to be transmitted */
		MODIFY_REG( DMATxDesc->DESC3, ETH_DMATXNDESCRF_FL, MAX_PACKET_SIZE );

#ifdef CHECKSUM_BY_HARDWARE
		/* IP Header checksum and payload checksum calculation and insertion are enabled and pseudo header checksum is
		   calculated in hardware */
		MODIFY_REG( DMATxDesc->DESC3, ETH_DMATXNDESCRF_CIC, ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC );
#endif
		/* Mark it as First Descriptor */
		SET_BIT( DMATxDesc->DESC3, ETH_DMATXNDESCRF_FD );

		/* Mark it as NORMAL descriptor */
		CLEAR_BIT( DMATxDesc->DESC3, ETH_DMATXNDESCRF_CTXT );

		/* set OWN bit of descriptor */
		SET_BIT( DMATxDesc->DESC3, ETH_DMATXNDESCRF_OWN );

		/* Clear Interrupt on completition bit */
		CLEAR_BIT( DMATxDesc->DESC2, ETH_DMATXNDESCRF_IOC );

		/* Mark it as LAST descriptor */
		SET_BIT( DMATxDesc->DESC3, ETH_DMATXNDESCRF_LD );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::ETHDriverInit( uint8_t* mac_address )
{
	uint8_t hwaddr[EMAC_Driver::ETH_MAC_HW_ADDRESS_LEN] = { 0U };

	// Initialize semaphores and mutexes.
	ETHRxBinSemaphore = new OS_Binary_Semaphore( false );

	BF_ASSERT( mac_address != NULL );

	hwaddr[0U] = mac_address[5];
	hwaddr[1U] = mac_address[4];
	hwaddr[2U] = mac_address[3];
	hwaddr[3U] = mac_address[2];
	hwaddr[4U] = mac_address[1];
	hwaddr[5U] = mac_address[0];

	ETH_MACAddressConfig( ETH_MAC_Address_0, hwaddr );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::ETH_Handler( void )
{
	#ifdef FREERTOS_V10
	static BaseType_t xHigherPriorityTaskWoken;
	#else
	static bool_t xHigherPriorityTaskWoken;
	#endif  // FREERTOS_V10

	// Read and Clear the interrupt.
	if ( RESET != Stm32_Eth_Driver::ETH_GetDMAFlagStatus( ETH_DMACSR_RI ) )
	{
		/* Clear the Eth DMA Rx IT pending bits */
		Stm32_Eth_Driver::ETH_DMAClearITPendingBit( ETH_DMACSR_RI );
		Stm32_Eth_Driver::ETH_DMAClearITPendingBit( ETH_DMACSR_NIS );

		ETHRxBinSemaphore->Post_From_ISR( &xHigherPriorityTaskWoken );
	}
	else
	{
		/* Else loop to suppress MISRA */
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::Init_Eth_Emac( uint8_t* mac_address, dcid_emac_params_t const* emac_dcids )
{

	uint32_t broadcast_settings = 0x00000000;
	uint32_t multicast_settings = 0x00000010;
	DCI_Owner* broadcast_owner = new DCI_Owner( emac_dcids->broadcast_settings );
	DCI_Owner* multicast_owner = new DCI_Owner( emac_dcids->multicast_settings );

	broadcast_owner->Check_Out_Init( reinterpret_cast<uint8_t*>( &broadcast_settings ) );// Pending to link with DCID
	multicast_owner->Check_Out_Init( reinterpret_cast<uint8_t*>( &multicast_settings ) );// Pending to link with DCID

	ETH_Load_Defaults();

	m_ethmac_init.LoopbackMode = DISABLE;
	m_ethmac_init.RetryTransmission = ENABLE;
	m_ethmac_init.AutomaticPadCRCStrip = DISABLE;
#ifdef CHECKSUM_BY_HARDWARE
	m_ethmac_init.ChecksumOffload = ENABLE;
#endif

	/*------------------------   EMAC Filter   -----------------------------------*/
	if ( broadcast_settings > 0 )
	{
		// ENABLE: Pass all received broadcast frames
		// DISABLE: Block all received broadcast frames
		m_ethmac_filter_init.BroadcastFilter = DISABLE;
	}

	if ( multicast_settings > 0 )
	{
		// ENABLE: Pass all received broadcast frames
		// DISABLE: Block all received broadcast frames
		m_ethmac_filter_init.PassAllMulticast = ENABLE;
	}

	m_ethmac_filter_init.HashUnicast = DISABLE;
	m_ethmac_filter_init.PromiscuousMode = DISABLE;
	m_ethmac_filter_init.ReceiveAllMode = DISABLE;

	/*------------------------   DMA   -----------------------------------*/

	/* When we use the Checksum offload feature, we need to enable the Store and Forward mode:
	   the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the
	      checksum. If the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
	m_ethdma_init.DMAArbitration = ETH_DMAARBITRATION_RX2_TX1;

	/* Configure Ethernet */
	ETH_Init();

	/* Enable the Ethernet Rx Interrupt */
	ETH_DMAITConfig( ( static_cast<uint32_t>( ETH_DMACIER_NIE | ETH_DMACIER_RIE ) ), ENABLE );

	Stm32_Eth_Driver::ETHDriverInit( mac_address );
}
