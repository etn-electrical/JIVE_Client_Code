/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "uC_FMC.h"
#include "stm32f7xx_ll_fmc.h"
#include "Device_Defines.h"
#include "stm32f7xx_hal_conf.h"
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_FMC::uC_FMC( void )
{ }

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_FMC::~uC_FMC( void )
{ }

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_FMC::FMC_SDRAM_Init( FMC_SDRAM_TypeDef* Device, FMC_SDRAM_InitTypeDef* Init )
{
	uint32_t tmpr1 = 0;
	uint32_t tmpr2 = 0;

	/* Check the parameters */
	assert_param( IS_FMC_SDRAM_DEVICE( Device ) );
	assert_param( IS_FMC_SDRAM_BANK( Init->SDBank ) );
	assert_param( IS_FMC_COLUMNBITS_NUMBER( Init->ColumnBitsNumber ) );
	assert_param( IS_FMC_ROWBITS_NUMBER( Init->RowBitsNumber ) );
	assert_param( IS_FMC_SDMEMORY_WIDTH( Init->MemoryDataWidth ) );
	assert_param( IS_FMC_INTERNALBANK_NUMBER( Init->InternalBankNumber ) );
	assert_param( IS_FMC_CAS_LATENCY( Init->CASLatency ) );
	assert_param( IS_FMC_WRITE_PROTECTION( Init->WriteProtection ) );
	assert_param( IS_FMC_SDCLOCK_PERIOD( Init->SDClockPeriod ) );
	assert_param( IS_FMC_READ_BURST( Init->ReadBurst ) );
	assert_param( IS_FMC_READPIPE_DELAY( Init->ReadPipeDelay ) );

	/* Set SDRAM bank configuration parameters */
	if ( Init->SDBank != FMC_SDRAM_BANK2 )
	{
		tmpr1 = Device->SDCR[FMC_SDRAM_BANK1];

		/* Clear NC, NR, MWID, NB, CAS, WP, SDCLK, RBURST, and RPIPE bits */
		tmpr1 &= ( ( uint32_t ) ~( FMC_SDCR1_NC | FMC_SDCR1_NR | FMC_SDCR1_MWID | \
								   FMC_SDCR1_NB | FMC_SDCR1_CAS | FMC_SDCR1_WP | \
								   FMC_SDCR1_SDCLK | FMC_SDCR1_RBURST | FMC_SDCR1_RPIPE ) );

		tmpr1 |= ( uint32_t )( Init->ColumnBitsNumber | \
							   Init->RowBitsNumber | \
							   Init->MemoryDataWidth | \
							   Init->InternalBankNumber | \
							   Init->CASLatency | \
							   Init->WriteProtection | \
							   Init->SDClockPeriod | \
							   Init->ReadBurst | \
							   Init->ReadPipeDelay
							   );
		Device->SDCR[FMC_SDRAM_BANK1] = tmpr1;
	}
	else/* FMC_Bank2_SDRAM */
	{
		tmpr1 = Device->SDCR[FMC_SDRAM_BANK1];

		/* Clear SDCLK, RBURST, and RPIPE bits */
		tmpr1 &= ( ( uint32_t ) ~( FMC_SDCR1_SDCLK | FMC_SDCR1_RBURST | FMC_SDCR1_RPIPE ) );

		tmpr1 |= ( uint32_t )( Init->SDClockPeriod | \
							   Init->ReadBurst | \
							   Init->ReadPipeDelay );

		tmpr2 = Device->SDCR[FMC_SDRAM_BANK2];

		/* Clear NC, NR, MWID, NB, CAS, WP, SDCLK, RBURST, and RPIPE bits */
		tmpr2 &= ( ( uint32_t ) ~( FMC_SDCR1_NC | FMC_SDCR1_NR | FMC_SDCR1_MWID | \
								   FMC_SDCR1_NB | FMC_SDCR1_CAS | FMC_SDCR1_WP | \
								   FMC_SDCR1_SDCLK | FMC_SDCR1_RBURST | FMC_SDCR1_RPIPE ) );

		tmpr2 |= ( uint32_t )( Init->ColumnBitsNumber | \
							   Init->RowBitsNumber | \
							   Init->MemoryDataWidth | \
							   Init->InternalBankNumber | \
							   Init->CASLatency | \
							   Init->WriteProtection );

		Device->SDCR[FMC_SDRAM_BANK1] = tmpr1;
		Device->SDCR[FMC_SDRAM_BANK2] = tmpr2;
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void uC_FMC::FMC_SDRAM_Timing_Init( FMC_SDRAM_TypeDef* Device, FMC_SDRAM_TimingTypeDef* Timing, uint32_t Bank )
{
	uint32_t tmpr1 = 0;
	uint32_t tmpr2 = 0;

	/* Check the parameters */
	assert_param( IS_FMC_SDRAM_DEVICE( Device ) );
	assert_param( IS_FMC_LOADTOACTIVE_DELAY( Timing->LoadToActiveDelay ) );
	assert_param( IS_FMC_EXITSELFREFRESH_DELAY( Timing->ExitSelfRefreshDelay ) );
	assert_param( IS_FMC_SELFREFRESH_TIME( Timing->SelfRefreshTime ) );
	assert_param( IS_FMC_ROWCYCLE_DELAY( Timing->RowCycleDelay ) );
	assert_param( IS_FMC_WRITE_RECOVERY_TIME( Timing->WriteRecoveryTime ) );
	assert_param( IS_FMC_RP_DELAY( Timing->RPDelay ) );
	assert_param( IS_FMC_RCD_DELAY( Timing->RCDDelay ) );
	assert_param( IS_FMC_SDRAM_BANK( Bank ) );

	/* Set SDRAM device timing parameters */
	if ( Bank != FMC_SDRAM_BANK2 )
	{
		tmpr1 = Device->SDTR[FMC_SDRAM_BANK1];

		/* Clear TMRD, TXSR, TRAS, TRC, TWR, TRP and TRCD bits */
		tmpr1 &= ( ( uint32_t ) ~( FMC_SDTR1_TMRD | FMC_SDTR1_TXSR | FMC_SDTR1_TRAS | \
								   FMC_SDTR1_TRC | FMC_SDTR1_TWR | FMC_SDTR1_TRP | \
								   FMC_SDTR1_TRCD ) );

		tmpr1 |= ( uint32_t )( ( ( Timing->LoadToActiveDelay ) - 1 ) | \
							   ( ( ( Timing->ExitSelfRefreshDelay ) - 1 ) << 4 ) | \
							   ( ( ( Timing->SelfRefreshTime ) - 1 ) << 8 ) | \
							   ( ( ( Timing->RowCycleDelay ) - 1 ) << 12 ) | \
							   ( ( ( Timing->WriteRecoveryTime ) - 1 ) << 16 ) | \
							   ( ( ( Timing->RPDelay ) - 1 ) << 20 ) | \
							   ( ( ( Timing->RCDDelay ) - 1 ) << 24 ) );
		Device->SDTR[FMC_SDRAM_BANK1] = tmpr1;
	}
	else/* FMC_Bank2_SDRAM */
	{
		tmpr1 = Device->SDTR[FMC_SDRAM_BANK1];

		/* Clear TRC and TRP bits */
		tmpr1 &= ( ( uint32_t ) ~( FMC_SDTR1_TRC | FMC_SDTR1_TRP ) );

		tmpr1 |= ( uint32_t )( ( ( ( Timing->RowCycleDelay ) - 1 ) << 12 ) | \
							   ( ( ( Timing->RPDelay ) - 1 ) << 20 ) );

		tmpr2 = Device->SDTR[FMC_SDRAM_BANK2];

		/* Clear TMRD, TXSR, TRAS, TRC, TWR, TRP and TRCD bits */
		tmpr2 &= ( ( uint32_t ) ~( FMC_SDTR1_TMRD | FMC_SDTR1_TXSR | FMC_SDTR1_TRAS | \
								   FMC_SDTR1_TRC | FMC_SDTR1_TWR | FMC_SDTR1_TRP | \
								   FMC_SDTR1_TRCD ) );

		tmpr2 |= ( uint32_t )( ( ( Timing->LoadToActiveDelay ) - 1 ) | \
							   ( ( ( Timing->ExitSelfRefreshDelay ) - 1 ) << 4 ) | \
							   ( ( ( Timing->SelfRefreshTime ) - 1 ) << 8 ) | \
							   ( ( ( Timing->WriteRecoveryTime ) - 1 ) << 16 ) | \
							   ( ( ( Timing->RCDDelay ) - 1 ) << 24 ) );

		Device->SDTR[FMC_SDRAM_BANK1] = tmpr1;
		Device->SDTR[FMC_SDRAM_BANK2] = tmpr2;
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

void uC_FMC::FMC_SDRAM_DeInit( FMC_SDRAM_TypeDef* Device, uint32_t Bank )
{
	/* Check the parameters */
	assert_param( IS_FMC_SDRAM_DEVICE( Device ) );
	assert_param( IS_FMC_SDRAM_BANK( Bank ) );

	/* De-initialize the SDRAM device */
	Device->SDCR[Bank] = 0x000002D0;
	Device->SDTR[Bank] = 0x0FFFFFFF;
	Device->SDCMR = 0x00000000;
	Device->SDRTR = 0x00000000;
	Device->SDSR = 0x00000000;

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_FMC::FMC_SDRAM_WriteProtection_Enable( FMC_SDRAM_TypeDef* Device, uint32_t Bank )
{
	/* Check the parameters */
	assert_param( IS_FMC_SDRAM_DEVICE( Device ) );
	assert_param( IS_FMC_SDRAM_BANK( Bank ) );

	/* Enable write protection */
	Device->SDCR[Bank] |= FMC_SDRAM_WRITE_PROTECTION_ENABLE;

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_FMC::FMC_SDRAM_WriteProtection_Disable( FMC_SDRAM_TypeDef* Device, uint32_t Bank )
{
	/* Check the parameters */
	assert_param( IS_FMC_SDRAM_DEVICE( Device ) );
	assert_param( IS_FMC_SDRAM_BANK( Bank ) );

	/* Disable write protection */
	Device->SDCR[Bank] &= ~FMC_SDRAM_WRITE_PROTECTION_ENABLE;

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_FMC::FMC_SDRAM_SendCommand( FMC_SDRAM_TypeDef* Device, FMC_SDRAM_CommandTypeDef* Command, uint32_t Timeout )
{
	__IO uint32_t tmpr = 0;

	/* Check the parameters */
	assert_param( IS_FMC_SDRAM_DEVICE( Device ) );
	assert_param( IS_FMC_COMMAND_MODE( Command->CommandMode ) );
	assert_param( IS_FMC_COMMAND_TARGET( Command->CommandTarget ) );
	assert_param( IS_FMC_AUTOREFRESH_NUMBER( Command->AutoRefreshNumber ) );
	assert_param( IS_FMC_MODE_REGISTER( Command->ModeRegisterDefinition ) );

	/* Set command register */
	tmpr = ( uint32_t )( ( Command->CommandMode ) | \
						 ( Command->CommandTarget ) | \
						 ( ( ( Command->AutoRefreshNumber ) - 1 ) << 5 ) | \
						 ( ( Command->ModeRegisterDefinition ) << 9 )
						  );

	Device->SDCMR = tmpr;

	/* Get tick */
	// tickstart = HAL_GetTick();

	/* wait until command is send */
	while ( HAL_IS_BIT_SET( Device->SDSR, FMC_SDSR_BUSY ) )
	{
		/* Check for the Timeout */
		// uC_Delay(Timeout);
		//
		// if(Timeout != HAL_MAX_DELAY)
		// {
		// if((Timeout == 0)||((HAL_GetTick() - tickstart ) > Timeout))
		// {
		// return HAL_TIMEOUT;
		// }
		// }
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_FMC::FMC_SDRAM_ProgramRefreshRate( FMC_SDRAM_TypeDef* Device, uint32_t RefreshRate )
{
	/* Check the parameters */
	assert_param( IS_FMC_SDRAM_DEVICE( Device ) );
	assert_param( IS_FMC_REFRESH_RATE( RefreshRate ) );

	/* Set the refresh rate in command register */
	Device->SDRTR |= ( RefreshRate << 1 );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_FMC::FMC_SDRAM_SetAutoRefreshNumber( FMC_SDRAM_TypeDef* Device, uint32_t AutoRefreshNumber )
{
	/* Check the parameters */
	assert_param( IS_FMC_SDRAM_DEVICE( Device ) );
	assert_param( IS_FMC_AUTOREFRESH_NUMBER( AutoRefreshNumber ) );

	/* Set the Auto-refresh number in command register */
	Device->SDCMR |= ( AutoRefreshNumber << 5 );

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_FMC::FMC_SDRAM_GetModeStatus( FMC_SDRAM_TypeDef* Device, uint32_t Bank )
{
	uint32_t tmpreg = 0;

	/* Check the parameters */
	assert_param( IS_FMC_SDRAM_DEVICE( Device ) );
	assert_param( IS_FMC_SDRAM_BANK( Bank ) );

	/* Get the corresponding bank mode */
	if ( Bank == FMC_SDRAM_BANK1 )
	{
		tmpreg = ( uint32_t )( Device->SDSR & FMC_SDSR_MODES1 );
	}
	else
	{
		tmpreg = ( ( uint32_t )( Device->SDSR & FMC_SDSR_MODES2 ) >> 2 );
	}

	/* Return the mode status */
	return ( tmpreg );
}