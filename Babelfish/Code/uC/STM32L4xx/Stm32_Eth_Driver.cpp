/**
 *****************************************************************************************
 * @file            Stm32_Eth_Driver.cpp Header File for interface class.
 *
 * @brief           The file contains Stm32_Eth_Driver.cpp, an interface class required
 *
 * @details
 * @copyright       2014 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 *****************************************************************************************
 */

#include "Stm32_Eth_Driver.h"
#include "OS_Tasker.h"
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
	DMATxDescToSet( static_cast<ETH_DMADESCTypeDef*>( NULL ) ),
	DMARxDescToGet( static_cast<ETH_DMADESCTypeDef*>( NULL ) ),
	m_ETHRxBinSemaphore( static_cast<OS_Binary_Semaphore*>( NULL ) ),
	EMAC_Driver()
{

	m_ETHRxBinSemaphore = static_cast<OS_Binary_Semaphore*>( NULL );
	/* Enable SYSCFG clock */
	// RCC_APB2PeriphClockCmd( RCC_APB2Periph_SYSCFG, ENABLE ); //Prayas - change due to modification
	// in ST driver files
	__SYSCFG_CLK_ENABLE();
	m_emac_pio = ( uC_Base::Self() )->Get_EMAC_Ctrl( emac_select );
	uC_PERIPH_IO_STRUCT ETH_MCO_CLOCK_OUT_GPIO =
	{ GPIOA, static_cast<uint8_t>( IO_PIN_8 ),
	  static_cast<uint8_t>( uC_IO_ALTERNATE_MAP_11 ) };

	BF_ASSERT( m_emac_pio != nullptr );
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
			// RCC_MCO1Config( RCC_MCO1Source_HSE, RCC_MCO1Div_1 ); //Prayas - change due to
			// modification in ST driver files
			HAL_RCC_MCOConfig( RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1 );
		}
		else
		{
			// uC_IO_Config::Enable_Periph_Output_Pin( &ETH_MCO_CLOCK_OUT_GPIO );
		}

		SYSCFG->PMC &= ~m_emac_pio->rmii_sel_mask;
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
			// RCC_MCO1Config( RCC_MCO1Source_PLLCLK, RCC_MCO1Div_2 ); //Prayas - change due to
			// modification in ST driver files
			HAL_RCC_MCOConfig( RCC_MCO1, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_2 );
		}
		else
		{
			// uC_IO_Config::Enable_Periph_Output_Pin( &ETH_MCO_CLOCK_OUT_GPIO );
		}

		SYSCFG->PMC |= m_emac_pio->rmii_sel_mask;
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
	// RCC_ClocksTypeDef rcc_clocks; //Prayas - change due to modification in ST driver files
	uint32_t hclk;
	uint32_t tmpreg = ETH->MACMIIAR;

	/* Get the ETHERNET MACMIIAR value */

	// RCC_GetClocksFreq( &rcc_clocks ); //Prayas - change due to modification in ST driver files
	// hclk = rcc_clocks.HCLK_Frequency; //Prayas - change due to modification in ST driver files

	hclk = HAL_RCC_GetHCLKFreq();

	/* Clear CSR Clock Range CR[2:0] bits */
	tmpreg &= MACMIIAR_CR_MASK;
	/* Get hclk frequency value */

	/* Set CR bits depending on hclk value */
	if ( ( hclk >= 20000000U ) && ( hclk < 35000000U ) )
	{
		/* CSR Clock Range between 20-35 MHz */
		tmpreg |= static_cast<uint32_t>( ETH_MACMIIAR_CR_Div16 );
	}
	else if ( ( hclk >= 35000000U ) && ( hclk < 60000000U ) )
	{
		/* CSR Clock Range between 35-60 MHz */
		tmpreg |= static_cast<uint32_t>( ETH_MACMIIAR_CR_Div26 );
	}
	else if ( ( hclk >= 60000000U ) && ( hclk < 100000000U ) )
	{
		/* CSR Clock Range between 60-100 MHz */
		tmpreg |= ( uint32_t )ETH_MACMIIAR_CR_Div42;
	}
	else if ( ( hclk >= 100000000U ) && ( hclk < 150000000U ) )
	{
		/* CSR Clock Range between 100-150 MHz */
		tmpreg |= ( uint32_t )ETH_MACMIIAR_CR_Div62;
	}
	else
	{
		tmpreg |= ( uint32_t )ETH_MACMIIAR_CR_Div102;
	}

	/* Write to ETHERNET MAC MIIAR: Configure the ETHERNET CSR Clock Range */ ETH->MACMIIAR =
		static_cast<uint32_t>( tmpreg );

	/* ------------------------ ETHERNET MACCR Configuration --------------------
	   Get the ETHERNET MACCR value*/
	tmpreg = ETH->MACCR;
	/* Clear WD, PCE, PS, TE and RE bits */
	tmpreg &= MACCR_CLEAR_MASK;
	/* Set the WD bit according to ETH_Watchdog value
	   Set the JD: bit according to ETH_Jabber value
	   Set the IFG bit according to ETH_InterFrameGap value
	   Set the DCRS bit according to ETH_CarrierSense value
	   Set the FES bit according to ETH_Speed value
	   Set the DO bit according to ETH_ReceiveOwn value
	   Set the LM bit according to ETH_LoopbackMode value
	   Set the DM bit according to ETH_Mode value
	   Set the IPC bit according to ETH_ChecksumOffload value
	   Set the DR bit according to ETH_RetryTransmission value
	   Set the ACS bit according to ETH_AutomaticPadCRCStrip value
	   Set the BL bit according to ETH_BackOffLimit value
	   Set the DC bit according to ETH_DeferralCheck value*/
	tmpreg |= static_cast<uint32_t>( ( m_ETH_InitStructure.ETH_Watchdog |
									   m_ETH_InitStructure.ETH_Jabber |
									   m_ETH_InitStructure.ETH_InterFrameGap |
									   m_ETH_InitStructure.ETH_CarrierSense |
									   m_ETH_InitStructure.ETH_Speed |
									   m_ETH_InitStructure.ETH_ReceiveOwn |
									   m_ETH_InitStructure.ETH_LoopbackMode |
									   m_ETH_InitStructure.ETH_Mode |
									   m_ETH_InitStructure.ETH_ChecksumOffload |
									   m_ETH_InitStructure.ETH_RetryTransmission |
									   m_ETH_InitStructure.ETH_AutomaticPadCRCStrip |
									   m_ETH_InitStructure.ETH_BackOffLimit |
									   m_ETH_InitStructure.ETH_DeferralCheck ) );
	/* Write to ETHERNET MACCR */ ETH->MACCR = static_cast<uint32_t>( tmpreg );

	/* ----------------------- ETHERNET MACFFR Configuration --------------------
	   Set the RA bit according to ETH_ReceiveAll value
	   Set the SAF and SAIF bits according to ETH_SourceAddrFilter value
	   Set the PCF bit according to ETH_PassControlFrames value
	   Set the DBF bit according to ETH_BroadcastFramesReception value
	   Set the DAIF bit according to ETH_DestinationAddrFilter value
	   Set the PR bit according to ETH_PromiscuousMode value
	   Set the PM, HMC and HPF bits according to ETH_MulticastFramesFilter value
	   Set the HUC and HPF bits according to ETH_UnicastFramesFilter value
	   Write to ETHERNET MACFFR*/ETH->MACFFR = static_cast<uint32_t>( m_ETH_InitStructure
										 .ETH_ReceiveAll |
										 m_ETH_InitStructure.
										 ETH_SourceAddrFilter |
										 m_ETH_InitStructure.
										 ETH_PassControlFrames |
										 m_ETH_InitStructure.
										 ETH_BroadcastFramesReception
										 |
										 m_ETH_InitStructure.
										 ETH_DestinationAddrFilter
										 |
										 m_ETH_InitStructure.
										 ETH_PromiscuousMode |
										 m_ETH_InitStructure.
										 ETH_MulticastFramesFilter
										 |
										 m_ETH_InitStructure.
										 ETH_UnicastFramesFilter );
	/* --------------- ETHERNET MACHTHR and MACHTLR Configuration ---------------
	   Write to ETHERNET MACHTHR*/ETH->MACHTHR = static_cast<uint32_t>( m_ETH_InitStructure
										  .ETH_HashTableHigh );
	/* Write to ETHERNET MACHTLR */ ETH->MACHTLR = static_cast<uint32_t>( m_ETH_InitStructure
																		  .ETH_HashTableLow );
	/* ----------------------- ETHERNET MACFCR Configuration --------------------
	   Get the ETHERNET MACFCR value*/
	tmpreg = ETH->MACFCR;
	/* Clear xx bits */
	tmpreg &= MACFCR_CLEAR_MASK;

	/* Set the PT bit according to ETH_PauseTime value
	   Set the DZPQ bit according to ETH_ZeroQuantaPause value
	   Set the PLT bit according to ETH_PauseLowThreshold value
	   Set the UP bit according to ETH_UnicastPauseFrameDetect value
	   Set the RFE bit according to ETH_ReceiveFlowControl value
	   Set the TFE bit according to ETH_TransmitFlowControl value*/
	tmpreg |= static_cast<uint32_t>( ( m_ETH_InitStructure.ETH_PauseTime << 16U ) |
									 m_ETH_InitStructure.ETH_ZeroQuantaPause |
									 m_ETH_InitStructure.ETH_PauseLowThreshold |
									 m_ETH_InitStructure.ETH_UnicastPauseFrameDetect |
									 m_ETH_InitStructure.ETH_ReceiveFlowControl |
									 m_ETH_InitStructure.ETH_TransmitFlowControl );
	/* Write to ETHERNET MACFCR */ ETH->MACFCR = static_cast<uint32_t>( tmpreg );
	/* ----------------------- ETHERNET MACVLANTR Configuration -----------------
	   Set the ETV bit according to ETH_VLANTagComparison value
	   Set the VL bit according to ETH_VLANTagIdentifier value*/ETH->MACVLANTR =
		static_cast<uint32_t>( m_ETH_InitStructure.ETH_VLANTagComparison |
							   m_ETH_InitStructure.ETH_VLANTagIdentifier );

	/* -------------------------------- DMA Config ------------------------------
	   ----------------------- ETHERNET DMAOMR Configuration --------------------
	   Get the ETHERNET DMAOMR value*/
	tmpreg = ETH->DMAOMR;
	/* Clear xx bits */
	tmpreg &= DMAOMR_CLEAR_MASK;

	/* Set the DT bit according to ETH_DropTCPIPChecksumErrorFrame value
	   Set the RSF bit according to ETH_ReceiveStoreForward value
	   Set the DFF bit according to ETH_FlushReceivedFrame value
	   Set the TSF bit according to ETH_TransmitStoreForward value
	   Set the TTC bit according to ETH_TransmitThresholdControl value
	   Set the FEF bit according to ETH_ForwardErrorFrames value
	   Set the FUF bit according to ETH_ForwardUndersizedGoodFrames value
	   Set the RTC bit according to ETH_ReceiveThresholdControl value
	   Set the OSF bit according to ETH_SecondFrameOperate value*/
	tmpreg |= static_cast<uint32_t>( m_ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame |
									 m_ETH_InitStructure.ETH_ReceiveStoreForward |
									 m_ETH_InitStructure.ETH_FlushReceivedFrame |
									 m_ETH_InitStructure.ETH_TransmitStoreForward |
									 m_ETH_InitStructure.ETH_TransmitThresholdControl |
									 m_ETH_InitStructure.ETH_ForwardErrorFrames |
									 m_ETH_InitStructure.ETH_ForwardUndersizedGoodFrames |
									 m_ETH_InitStructure.ETH_ReceiveThresholdControl |
									 m_ETH_InitStructure.ETH_SecondFrameOperate );
	/* Write to ETHERNET DMAOMR */ ETH->DMAOMR = static_cast<uint32_t>( tmpreg );

	/* ----------------------- ETHERNET DMABMR Configuration --------------------
	   Set the AAL bit according to ETH_AddressAlignedBeats value
	   Set the FB bit according to ETH_FixedBurst value
	   Set the RPBL and 4*PBL bits according to ETH_RxDMABurstLength value
	   Set the PBL and 4*PBL bits according to ETH_TxDMABurstLength value
	   Set the DSL bit according to ETH_DesciptorSkipLength value
	   Set the PR and DA bits according to ETH_DMAArbitration value*/
	ETH->DMABMR = static_cast<uint32_t>( m_ETH_InitStructure.ETH_AddressAlignedBeats |
										 m_ETH_InitStructure.ETH_FixedBurst |
										 m_ETH_InitStructure.ETH_RxDMABurstLength |		/* !! if
																						   4xPBL is
																						   selected
																						   for Tx or
																						   Rx it is
																						   applied
																						   for the
																						   other */
										 m_ETH_InitStructure.ETH_TxDMABurstLength |
										 ( m_ETH_InitStructure.ETH_DescriptorSkipLength <<
										   2U ) |
										 m_ETH_InitStructure.ETH_DMAArbitration |
										 ETH_DMABMR_USP );		/* Enable use of separate PBL for Rx
																   and Tx */
	/* Return Ethernet configuration success */
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
	m_ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable;
	m_ETH_InitStructure.ETH_Watchdog = ETH_Watchdog_Enable;
	m_ETH_InitStructure.ETH_Jabber = ETH_Jabber_Enable;
	m_ETH_InitStructure.ETH_InterFrameGap = ETH_InterFrameGap_96Bit;
	m_ETH_InitStructure.ETH_CarrierSense = ETH_CarrierSense_Enable;
	m_ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
	m_ETH_InitStructure.ETH_ReceiveOwn = ETH_ReceiveOwn_Enable;
	m_ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
	m_ETH_InitStructure.ETH_Mode = ETH_Mode_HalfDuplex;
	m_ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Disable;
	m_ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Enable;
	m_ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
	m_ETH_InitStructure.ETH_BackOffLimit = ETH_BackOffLimit_10;
	m_ETH_InitStructure.ETH_DeferralCheck = ETH_DeferralCheck_Disable;
	m_ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
	m_ETH_InitStructure.ETH_SourceAddrFilter = ETH_SourceAddrFilter_Disable;
	m_ETH_InitStructure.ETH_PassControlFrames = ETH_PassControlFrames_BlockAll;
	m_ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Disable;
	m_ETH_InitStructure.ETH_DestinationAddrFilter = ETH_DestinationAddrFilter_Normal;
	m_ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
	m_ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
	m_ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
	m_ETH_InitStructure.ETH_HashTableHigh = 0x0U;
	m_ETH_InitStructure.ETH_HashTableLow = 0x0U;
	m_ETH_InitStructure.ETH_PauseTime = 0x0U;
	m_ETH_InitStructure.ETH_ZeroQuantaPause = ETH_ZeroQuantaPause_Disable;
	m_ETH_InitStructure.ETH_PauseLowThreshold = ETH_PauseLowThreshold_Minus4;
	m_ETH_InitStructure.ETH_UnicastPauseFrameDetect = ETH_UnicastPauseFrameDetect_Disable;
	m_ETH_InitStructure.ETH_ReceiveFlowControl = ETH_ReceiveFlowControl_Disable;
	m_ETH_InitStructure.ETH_TransmitFlowControl = ETH_TransmitFlowControl_Disable;
	m_ETH_InitStructure.ETH_VLANTagComparison = ETH_VLANTagComparison_16Bit;
	m_ETH_InitStructure.ETH_VLANTagIdentifier = 0x0U;
	/*------------------------   DMA   -----------------------------------*/
	m_ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Disable;
	m_ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
	m_ETH_InitStructure.ETH_FlushReceivedFrame = ETH_FlushReceivedFrame_Disable;
	m_ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;
	m_ETH_InitStructure.ETH_TransmitThresholdControl = ETH_TransmitThresholdControl_64Bytes;
	m_ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;
	m_ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
	m_ETH_InitStructure.ETH_ReceiveThresholdControl = ETH_ReceiveThresholdControl_64Bytes;
	m_ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Disable;
	m_ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;
	m_ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Disable;
	m_ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_1Beat;
	m_ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_1Beat;
	m_ETH_InitStructure.ETH_DescriptorSkipLength = 0x0U;
	m_ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_1_1;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::Init_EMAC_Hardware( void )
{
	/* Enable transmit state machine of the MAC for transmission on the MII */
	ETH_MACTransmissionCmd( ENABLE );
	/* Flush Transmit FIFO */
	ETH_FlushTransmitFIFO();
	/* Enable receive state machine of the MAC for reception from the MII */
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
		ETH->MACCR |= ETH_MACCR_TE;
	}
	else
	{
		/* Disable the MAC transmission */
		ETH->MACCR &= ~ETH_MACCR_TE;
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
		ETH->MACCR |= ETH_MACCR_RE;
	}
	else
	{
		/* Disable the MAC reception */
		ETH->MACCR &= ~ETH_MACCR_RE;
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

	/* Exclude RxDescPbuf[] from the run-time RAM test because the Eth DMA Controller uses it. */
	volatile_item_DMARxDscrTab.start = reinterpret_cast<uint8_t*>( &DMARxDscrTab[0] );
	volatile_item_DMARxDscrTab.end = ( volatile_item_DMARxDscrTab.start + sizeof( DMARxDscrTab ) ) -
		1U;
	volatile_item_DMARxDscrTab.next = reinterpret_cast<volatile_item_t*>( nullptr );
	volatile_item_DMARxDscrTab.prev = reinterpret_cast<volatile_item_t*>( nullptr );

	RAM_Diag::uC_RAM::Mark_As_Volatile( &volatile_item_DMARxDscrTab );

	/* Exclude DMATxDscrTab[] from the run-time RAM test because the Eth DMA Controller uses it. */
	volatile_item_DMATxDscrTab.start = reinterpret_cast<uint8_t*>( &DMATxDscrTab[0] );
	volatile_item_DMATxDscrTab.end = ( volatile_item_DMATxDscrTab.start + sizeof( DMATxDscrTab ) ) -
		1U;
	volatile_item_DMATxDscrTab.next = reinterpret_cast<volatile_item_t*>( nullptr );
	volatile_item_DMATxDscrTab.prev = reinterpret_cast<volatile_item_t*>( nullptr );

	RAM_Diag::uC_RAM::Mark_As_Volatile( &volatile_item_DMATxDscrTab );
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

		/* Set Buffer1 size and Second Address Chained bit */
		DMARxDescToGet->ControlBufferSize = ETH_DMARxDesc_RCH
			| ( static_cast<uint32_t>( rx_buffer_size ) );
		/* Set the Buffer 1 Address */
		DMARxDescToGet->Buffer1Addr = ( reinterpret_cast<uint32_t>( rx_payload[idx] ) );

		/* Initialize the next descriptor with the Next Desciptor Polling Enable */
		if ( idx < ( EMAC_Driver::ETH_RX_BUFNB - 1U ) )
		{
			/* Set next descriptor address register with next descriptor base address */
			DMARxDescToGet->Buffer2NextDescAddr = ( reinterpret_cast<uint32_t>( DMARxDscrTab + idx
																				+ 1 ) );
		}
		else
		{
			/* For last descriptor, set next descriptor address register equal to the first
			   descriptor base address */
			DMARxDescToGet->Buffer2NextDescAddr = ( reinterpret_cast<uint32_t>( DMARxDscrTab ) );
		}

		/* Set Own bit of the Rx descriptor Status */
		DMARxDescToGet->Status = ETH_DMARxDesc_OWN;
		ETH_DMARxDescReceiveITConfig( &DMARxDscrTab[idx], ENABLE );
		DMARxDescToGet++;
	}

	/* At initial the current descriptor shall be at position reset or 0 */
	DMARxDescToGet = &DMARxDscrTab[0U];
	/* Set Receive Desciptor List Address Register */
	ETH->DMARDLAR = ( reinterpret_cast<uint32_t>( DMARxDscrTab ) );
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
	DMATxDescToSet = reinterpret_cast<ETH_DMADESCTypeDef*>( NULL );
	DMARxDescToGet = reinterpret_cast<ETH_DMADESCTypeDef*>( NULL );
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

	/* Calculate the selectecd MAC address high register */
	tmpreg =
		( static_cast<uint32_t>( ( ( ( ( static_cast<uint32_t>( Addr[5U] ) ) << 8U ) ) )
								 | ( static_cast<uint32_t>( Addr[4U] ) ) ) );
	/* Load the selectecd MAC address high register */
	( *( reinterpret_cast<__IO uint32_t*>( ETH_MAC_ADDR_HBASE + MacAddr ) ) ) = tmpreg;
	/* Calculate the selectecd MAC address low register */
	tmpreg =
		( static_cast<uint32_t>( ( ( ( ( static_cast<uint32_t>( Addr[3U] ) ) << 24U ) ) )
								 | ( ( ( ( static_cast<uint32_t>
										   ( Addr[2U] ) ) << 16U ) ) )
								 | ( ( ( ( static_cast<uint32_t>
										   ( Addr[1U] ) ) << 8U ) ) )
								 | ( static_cast<uint32_t>( Addr[0U] ) ) ) );

	/* Load the selectecd MAC address low register */
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
void Stm32_Eth_Driver::ETH_DMARxDescReceiveITConfig( ETH_DMADESCTypeDef* DMARxDesc,
													 FunctionalState NewState ) const
{
	if ( NewState != DISABLE )
	{
		/* Enable the DMA Rx Desc receive interrupt */
		DMARxDesc->ControlBufferSize &= ( ~( static_cast<uint32_t>( ETH_DMARxDesc_DIC ) ) );
	}
	else
	{
		/* Disable the DMA Rx Desc receive interrupt */
		DMARxDesc->ControlBufferSize |= ETH_DMARxDesc_DIC;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::ETH_SoftwareReset( void ) const
{
	/* Set the SWR bit: resets all MAC subsystem internal registers and logic
	   After reset all the registers holds their respective reset values*/
	ETH->DMABMR |= ETH_DMABMR_SR;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
FlagStatus Stm32_Eth_Driver::ETH_GetSoftwareResetStatus( void ) const
{
	FlagStatus bitstatus = RESET;

	if ( ( static_cast<bool_t>( ( ETH->DMABMR & ETH_DMABMR_SR ) != 0U ) ) )
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
	ETH->DMASR = ( static_cast<uint32_t>( ETH_DMA_IT ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
FlagStatus Stm32_Eth_Driver::ETH_GetDMAFlagStatus( uint32_t ETH_DMA_FLAG )
{
	FlagStatus bitstatus = RESET;

	if ( ( static_cast<bool_t>( ( ETH->DMASR & ETH_DMA_FLAG ) != 0U ) ) )
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
	ETH->DMAOMR |= ETH_DMAOMR_FTF;
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
		ETH->DMAOMR |= ETH_DMAOMR_ST;
	}
	else
	{
		/* Disable the DMA transmission */
		ETH->DMAOMR &= ~ETH_DMAOMR_ST;
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
		ETH->DMAOMR |= ETH_DMAOMR_SR;
	}
	else
	{
		/* Disable the DMA reception */
		ETH->DMAOMR &= ~ETH_DMAOMR_SR;
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
		ETH->DMAIER |= ETH_DMA_IT;
	}
	else
	{
		/* Disable the selected ETHERNET DMA interrupts */
		ETH->DMAIER &= ( ~( static_cast<uint32_t>( ETH_DMA_IT ) ) );
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

#endif	/* USE_Delay*/

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
	uint32_t temp_reg = ETH->MACMIIAR;

	/* Keep only the CSR Clock Range CR[2:0] bits value */
	temp_reg &= ~MACMIIAR_CR_Mask;		// 0xFFFFFFE3

	/* Prepare the MII address register value
	   Set the PHY device address   //0x0000F800*/
	temp_reg |= ( ( static_cast<uint32_t>( phy_addr ) << 11U ) & ETH_MACMIIAR_PA );
	/* Set the PHY register address //0x000007C0 */
	temp_reg |= ( ( static_cast<uint32_t>( reg_addr ) << 6U ) & ETH_MACMIIAR_MR );
	/* Set the read mode     //((uint32_t)0x00000002) */
	temp_reg &= ~ETH_MACMIIAR_MW;
	/*  Set the MII Busy bit  //((uint32_t)0x00000001) */
	temp_reg |= ETH_MACMIIAR_MB;

	/* Write the result value into the MII Address register */ ETH->MACMIIAR = temp_reg;

	/* Check for the Busy flag */
	do
	{
		timeout++;
		temp_reg = ETH->MACMIIAR;
		if ( timeout > ETH_SMI_TIMEOUT )
		{
			/* Return ERROR in case of timeout */
			success = false;
			break;
		}
	} while ( static_cast<bool_t>( temp_reg & ETH_MACMIIAR_MB ) );

	/* Return data register value */
	if ( success )
	{
		*read_ptr = static_cast<uint16_t>( ETH->MACMIIDR );		// *read_ptr = (uint16_t) (
																// ETH->MACMIIDR );
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
	uint32_t temp_reg = ETH->MACMIIAR;

	/* Keep only the CSR Clock Range CR[2:0] bits value */
	temp_reg &= ~MACMIIAR_CR_Mask;

	/* Prepare the MII address register value
	   Set the PHY device address*/
	temp_reg |= ( ( static_cast<uint32_t>( phy_addr ) << 11U ) & ETH_MACMIIAR_PA );
	/* Set the PHY register address */
	temp_reg |= ( ( static_cast<uint32_t>( reg_addr ) << 6U ) & ETH_MACMIIAR_MR );
	/* Set the write mode */
	temp_reg |= ETH_MACMIIAR_MW;
	/* Set the MII Busy bit */
	temp_reg |= ETH_MACMIIAR_MB;

	/* Write the result value into the MII Address register */ ETH->MACMIIDR =
		static_cast<uint16_t>( *write_ptr );
	ETH->MACMIIAR = temp_reg;

	/* Check for the Busy flag */
	do
	{
		timeout++;
		temp_reg = ETH->MACMIIAR;
		if ( timeout > ETH_SMI_TIMEOUT )
		{
			/* Return ERROR in case of timeout */
			success = false;
			break;
		}
	} while ( static_cast<bool_t>( temp_reg & ETH_MACMIIAR_MB ) );

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
	return ( reinterpret_cast<uint8_t*>( DMATxDescToSet->Buffer1Addr ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::Wait_Tx_Ready( void )
{
	uint32_t ulAttempts = 0UL;

	while ( static_cast<bool_t>( ( DMATxDescToSet->Status & ETH_DMATxDesc_OWN ) != 0U ) )
	{
		/* Return ERROR: OWN bit set */
		OS_Tasker::Delay( 1U );

		ulAttempts++;

		if ( ulAttempts > 90U )
		{
			DMATxDescToSet->Status &= ~ETH_DMATxDesc_OWN;
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
	DMATxDescToSet->ControlBufferSize = ( l & ETH_DMATxDesc_TBS1 );

	/* Setting the last segment and first segment bits (in this case a frame is transmitted in one
	   descriptor) */
	DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;

	/* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
	DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;

	/* When Tx Buffer unavailable flag is set: clear it and resume transmission */
	if ( ( static_cast<bool_t>( ( ETH->DMASR & ETH_DMASR_TBUS ) != 0U ) ) )
	{
		/* Clear TBUS ETHERNET DMA flag */
		ETH->DMASR = ETH_DMASR_TBUS;
		/* Resume DMA transmission*/
		ETH->DMATPDR = 0U;
	}

	/* Update the ETHERNET DMA global Tx descriptor with next Tx decriptor
	   Chained Mode
	   Selects the next DMA Tx descriptor list for next buffer to send*/
	DMATxDescToSet =
		( reinterpret_cast<ETH_DMADESCTypeDef*>( DMATxDescToSet->Buffer2NextDescAddr ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t Stm32_Eth_Driver::Wait_Rx_Data_Available( void ) const
{
	return ( !( static_cast<bool_t>( ( DMARxDescToGet->Status & ETH_DMARxDesc_OWN ) != 0U ) ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::Config_Eth_DMA_Init( void ) const
{
	/* Enable the Ethernet Rx Interrupt */
	ETH_DMAITConfig( ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::ETH_DMATxDescChecksumInsertionConfig( ETH_DMADESCTypeDef* DMATxDesc,
															 uint32_t DMATxDesc_Checksum )
{
	/* Set the selected DMA Tx desc checksum insertion control */
	DMATxDesc->Status |= DMATxDesc_Checksum;
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
		m_ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
	}
	else if ( speed == 2U )
	{
		m_ETH_InitStructure.ETH_Speed = ETH_Speed_100M;
	}
	else
	{
		// m_ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
	}

	if ( duplex == 0U )
	{
		m_ETH_InitStructure.ETH_Mode = ETH_Mode_HalfDuplex;
	}
	else if ( duplex == 1U )
	{
		m_ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
	}
	else
	{
		/* MISRA */
	}
	// clear speed and duplex register setting
	tmpreg &= ~( ETH_Speed_100M | ETH_Mode_FullDuplex );
	// Set new speed and duplex mode
	tmpreg |= ( static_cast<uint32_t>( m_ETH_InitStructure.ETH_Speed |
									   m_ETH_InitStructure.ETH_Mode ) );

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

	fs_bit = static_cast<bool_t>( DMARxDescToGet->Status & ETH_DMARxDesc_FS );
	ls_bit = static_cast<bool_t>( DMARxDescToGet->Status & ETH_DMARxDesc_LS );

	error =
		( ( ( static_cast<bool_t>( ( DMARxDescToGet->Status & ETH_DMARxDesc_ES ) ) ) != 0U ) ||
		  ( !first_packet_received && !fs_bit ) || ( fs_bit && first_packet_received ) ||
		  ( new_payload == _NULL ) );

	dma_suspend = static_cast<bool_t>( ( ETH->DMASR & ETH_DMASR_RBUS ) != 0U );

	DMARxDescToGet->Status = ETH_DMARxDesc_OWN;
	if ( dma_suspend )
	{
		/* Clear RBUS ETHERNET DMA flag */
		ETH->DMASR = ETH_DMASR_RBUS;
		/* Resume DMA reception */
		ETH->DMARPDR = 0U;
	}
	if ( _NULL != new_payload )
	{
		DMARxDescToGet->Buffer1Addr = reinterpret_cast<uint32_t>( new_payload );
	}
	DMARxDescToGet = reinterpret_cast<ETH_DMADESCTypeDef*>( DMARxDescToGet->Buffer2NextDescAddr );

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

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Stm32_Eth_Driver::ETH_DMATxDescChainInit( uint8_t* TxBuff, uint32_t TxBuffCount )
{
	uint32_t count;
	ETH_DMADESCTypeDef* DMATxDesc;

	/* Set the DMATxDescToSet pointer with the first one of the DMATxDescTab list */
	DMATxDescToSet = DMATxDscrTab;
	/* Fill each DMATxDesc descriptor with the right values */
	for ( count = 0U; count < TxBuffCount; count++ )
	{
		/* Get the pointer on the ith member of the Tx Desc list */
		DMATxDesc = DMATxDscrTab + count;
		/* Set Second Address Chained bit */
		DMATxDesc->Status = ETH_DMATxDesc_TCH;

		/* Set Buffer1 address pointer */
		DMATxDesc->Buffer1Addr =
			reinterpret_cast<uint32_t>( ( &TxBuff[count * ETH_MAX_PACKET_SIZE] ) );

		/* Initialize the next descriptor with the Next Desciptor Polling Enable */
		if ( count < ( TxBuffCount - 1U ) )
		{
			/* Set next descriptor address register with next descriptor base address */
			DMATxDesc->Buffer2NextDescAddr =
				reinterpret_cast<uint32_t>( ( DMATxDscrTab + count + 1U ) );
		}
		else
		{
			/* For last descriptor, set next descriptor address register equal to the first
			   descriptor base address */
			DMATxDesc->Buffer2NextDescAddr = reinterpret_cast<uint32_t>( DMATxDscrTab );
		}
	}

#ifdef CHECKSUM_BY_HARDWARE
	/* Enable the checksum insertion for the Tx frames */
	{
		for ( count = 0U; count < ETH_TXBUFNB; count++ )
		{
			ETH_DMATxDescChecksumInsertionConfig( &DMATxDscrTab[count],
												  ETH_DMATxDesc_ChecksumTCPUDPICMPFull );
		}
	}

#endif

	/* Set Transmit Desciptor List Address Register */
	ETH->DMATDLAR = reinterpret_cast<uint32_t>( DMATxDscrTab );
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
	#endif	// FREERTOS_V10

	// Read and Clear the interrupt.
	if ( RESET != Stm32_Eth_Driver::ETH_GetDMAFlagStatus( ETH_DMA_FLAG_R ) )
	{
		/* Clear the Eth DMA Rx IT pending bits */
		Stm32_Eth_Driver::ETH_DMAClearITPendingBit( ETH_DMA_IT_R );
		Stm32_Eth_Driver::ETH_DMAClearITPendingBit( ETH_DMA_IT_NIS );

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

	UINT32 broadcast_settings = ETH_BroadcastFramesReception_Enable;
	UINT32 multicast_settings = ETH_MulticastFramesFilter_None;
	DCI_Owner* broadcast_owner = new DCI_Owner( emac_dcids->broadcast_settings );

	DCI_Owner* multicast_owner = new DCI_Owner( emac_dcids->multicast_settings );

	ETH_Load_Defaults();

	m_ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
	m_ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
	m_ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
	m_ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
	m_ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
	m_ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
	m_ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
	m_ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_None;
	m_ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;

	broadcast_owner->Check_Out_Init( reinterpret_cast<uint8_t*>( &broadcast_settings ) );
	multicast_owner->Check_Out_Init( reinterpret_cast<uint8_t*>( &multicast_settings ) );

	m_ETH_InitStructure.ETH_BroadcastFramesReception = broadcast_settings;
	m_ETH_InitStructure.ETH_MulticastFramesFilter = multicast_settings;

#ifdef CHECKSUM_BY_HARDWARE
	m_ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif

	/*------------------------   DMA   -----------------------------------*/

	/* When we use the Checksum offload feature, we need to enable the Store and Forward mode:
	   the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can
	      insert/verify the checksum,
	   if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
	m_ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame =
		ETH_DropTCPIPChecksumErrorFrame_Enable;
	m_ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
	m_ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;

	m_ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;
	m_ETH_InitStructure.ETH_ForwardUndersizedGoodFrames =
		ETH_ForwardUndersizedGoodFrames_Disable;
	m_ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
	m_ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;
	m_ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;
	m_ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;
	m_ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
	m_ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

	/* Configure Ethernet */
	ETH_Init();
	/* Enable the Ethernet Rx Interrupt */
	ETH_DMAITConfig( ( static_cast<uint32_t>( ETH_DMA_IT_NIS | ETH_DMA_IT_R ) ), ENABLE );
	Stm32_Eth_Driver::ETHDriverInit( mac_address );
	// broadcast_owner = reinterpret_cast<DCI_Owner*>( NULL );
	multicast_owner = reinterpret_cast<DCI_Owner*>( NULL );

}
