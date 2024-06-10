/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-2-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation casts shall not be used
 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style casts are required for
 * all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-0-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-0-4, Implicit integral conversions shall not change the
 * signedness of the underlying type
 * @n Justification: The ST supplied includes are needed for the HAL - The includes will be used as is The note is
 * re-enabled at the end of this file
 */
/*lint -e1960  */

#include "Includes.h"
#include "uC_DMA.h"
#include "uC_Base.h"
#include "uC_RAM.h"
#include "Babelfish_Assert.h"
/*
 *****************************************************************************************
 *		Constants and Macros
 *****************************************************************************************
 */
#define DMA_STREAM_MASK             ( ( static_cast<uint32_t>( 1U ) << DMA_STREAM_CHANNEL_SHIFT ) - 1U )
#define Get_DMA_Stream_Num( id )    ( ( id ) & DMA_STREAM_MASK )
#define Get_DMA_Channel_Num( id )   ( static_cast<uint32_t>( id ) >> DMA_STREAM_CHANNEL_SHIFT )

uint8_t* const uC_DMA::NULL_8b_DATA = reinterpret_cast<uint8_t*>( nullptr );
uint16_t* const uC_DMA::NULL_16b_DATA = reinterpret_cast<uint16_t*>( nullptr );
uint32_t* const uC_DMA::NULL_32b_DATA = reinterpret_cast<uint32_t*>( nullptr );

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
uint32_t uC_DMA::m_active_dma = 0U;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_DMA::uC_DMA( uint8_t dma_channel, uC_DMA_DIR_ENUM direction,
				uint8_t datatype_size, uC_DMA_MEM_INC_ENUM mem_inc,
				uC_DMA_PERIPH_INC_ENUM periph_inc, uC_DMA_CIRC_ENUM circular,
				uC_DMA_MEM2MEM_ENUM mem_to_mem ) :
	m_dummy_data( 0U ),
	m_dma( reinterpret_cast<uC_BASE_DMA_IO_STRUCT const*>( nullptr ) ),
	m_total_item_count( 0U ),
	m_datatype_size( 1U ),
	m_advanced_vect( reinterpret_cast<Advanced_Vect*>( nullptr ) ),
	m_mem_inc( false )
{
	uint8_t dma_stream_id;

	// dma_stream_id = static_cast<uint8_t>(Get_DMA_Stream_Num( dma_channel ));
	dma_stream_id = dma_channel;
	m_dma = ( uC_Base::Self() )->Get_DMA_Ctrl( dma_stream_id );

	BF_ASSERT( ( m_dma != NULL ) && ( !Test_Bit( m_active_dma, dma_stream_id ) ) );
	Set_Bit( m_active_dma, dma_stream_id );

	// Turn on DMA clock    (if necessary)
	// if ( 0 == (SIM->SCGC7 & SIM_SCGC7_DMA_MASK) )
	// if(!(Is_Periph_Clock_Enabled(&m_dma->periph_def)))
	// {
	// SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	// }
	//// Turn on DMAMUX clock (if necessary)
	//// DMAMUX0 services channels 0-15, DMAMUX1 services channels 16-31
	// if ( dma_stream_id < uC_DMA_CHANNEL_16 )
	// {
	// if ( 0 == (SIM->SCGC6 & SIM_SCGC6_DMAMUX0_MASK) )
	////if(!(Is_Periph_Clock_Enabled(&m_dma->periph_def)))
	// {
	// SIM->SCGC6 |= SIM_SCGC6_DMAMUX0_MASK;
	// }
	// }
	// else
	// {
	// if ( 0 == (SIM->SCGC6 & SIM_SCGC6_DMAMUX1_MASK) )
	// {
	// SIM->SCGC6 |= SIM_SCGC6_DMAMUX1_MASK;
	// }
	// }
	uC_Base::Reset_Periph( &m_dma->periph_def[0] );
	uC_Base::Reset_Periph( &m_dma->periph_def[1] );
	uC_Base::Enable_Periph_Clock( &m_dma->periph_def[0] );
	uC_Base::Enable_Periph_Clock( &m_dma->periph_def[1] );
	// disable channel and set DMAMUX source
	m_dma->dmamux_ctrl->CHCFG[m_dma->dmamux_num] = ( DMAMUX_CHCFG_SOURCE_MASK & m_dma->dmamux_src );

	// only support matching data sizes
	switch ( datatype_size )
	{
		case 2:
			m_dma->dma_ctrl->TCD[dma_stream_id].ATTR = DMA_ATTR_SSIZE( 1 ) | DMA_ATTR_DSIZE( 1 );
			break;

		case 4:
			m_dma->dma_ctrl->TCD[dma_stream_id].ATTR = DMA_ATTR_SSIZE( 2 ) | DMA_ATTR_DSIZE( 2 );
			break;

		case 1:
		default:
			m_dma->dma_ctrl->TCD[dma_stream_id].ATTR = DMA_ATTR_SSIZE( 0 ) | DMA_ATTR_DSIZE( 0 );
			break;
	}

	if ( mem_to_mem == uC_DMA_MEM2MEM_TRUE )
	{
		if ( mem_inc == uC_DMA_MEM_INC_TRUE )
		{
			m_dma->dma_ctrl->TCD[dma_stream_id].SOFF = datatype_size;
			m_dma->dma_ctrl->TCD[dma_stream_id].DOFF = datatype_size;
		}
		else
		{
			m_dma->dma_ctrl->TCD[dma_stream_id].SOFF = 0;
			m_dma->dma_ctrl->TCD[dma_stream_id].DOFF = 0;
		}
	}
	else if ( direction == uC_DMA_DIR_FROM_MEMORY )
	{
		if ( mem_inc == uC_DMA_MEM_INC_TRUE )
		{
			m_dma->dma_ctrl->TCD[dma_stream_id].SOFF = datatype_size;
		}
		else
		{
			m_dma->dma_ctrl->TCD[dma_stream_id].SOFF = 0;
		}
		if ( periph_inc == uC_DMA_PERIPH_INC_TRUE )
		{
			m_dma->dma_ctrl->TCD[dma_stream_id].DOFF = datatype_size;
		}
		else
		{
			m_dma->dma_ctrl->TCD[dma_stream_id].DOFF = 0;
		}
	}
	else// from peripheral
	{
		if ( mem_inc == uC_DMA_MEM_INC_TRUE )
		{
			m_dma->dma_ctrl->TCD[dma_stream_id].SOFF = 0;
		}
		else
		{
			m_dma->dma_ctrl->TCD[dma_stream_id].SOFF = datatype_size;
		}
		if ( periph_inc == uC_DMA_PERIPH_INC_TRUE )
		{
			m_dma->dma_ctrl->TCD[dma_stream_id].DOFF = 0;
		}
		else
		{
			m_dma->dma_ctrl->TCD[dma_stream_id].DOFF = datatype_size;
		}
	}
	// assuming one transfer per request for the moment
	// TODO: handle other potential options
	m_dma->dma_ctrl->TCD[dma_stream_id].NBYTES_MLNO = datatype_size;

	// clear out stale control bits.
	m_dma->dma_ctrl->TCD[m_dma->dma_channel_num].CSR = 0;

	m_advanced_vect = reinterpret_cast<Advanced_Vect*>( nullptr );

	m_volatile_item.start = reinterpret_cast<uint8_t*>( nullptr );
	m_volatile_item.end = reinterpret_cast<uint8_t*>( nullptr );
	m_volatile_item.next = reinterpret_cast<volatile_item_t*>( nullptr );
	m_volatile_item.prev = reinterpret_cast<volatile_item_t*>( nullptr );
	m_mem_to_mem = mem_to_mem;
	m_direction = direction;
	m_advanced_vect = NULL;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_DMA::Attach_Interrupt( INT_CALLBACK_FUNC* int_handler, uint16_t priority ) const
{
	return ( uC_Interrupt::Set_Vector( int_handler, m_dma->irq_num, priority ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_DMA::Attach_Callback( cback_func_t cback_func,
							  cback_param_t param, uint16_t priority )
{
	if ( m_advanced_vect == NULL )
	{
		m_advanced_vect = new Advanced_Vect();
	}

	m_advanced_vect->Set_Vect( cback_func, param );
	return ( uC_Interrupt::Set_Vector( m_advanced_vect->Get_Int_Func(),
									   m_dma->irq_num, priority ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_DMA::Set_Buff( uint8_t* mem_ptr, volatile void* periph_ptr, uint16_t item_count )
{
	// shut off current requests
	m_dma->dmamux_ctrl->CHCFG[m_dma->dmamux_num] &= ~DMAMUX_CHCFG_ENBL_MASK;
	m_dma->dma_ctrl->CERQ |= DMA_CERQ_CERQ( m_dma->dma_channel_num );

	m_total_item_count = item_count;

	// set up the intial and current loop counts
	m_dma->dma_ctrl->TCD[m_dma->dma_channel_num].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER( item_count );
	m_dma->dma_ctrl->TCD[m_dma->dma_channel_num].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER( item_count );
	// set the channel to disable further dma requests once the item count is finished
	m_dma->dma_ctrl->TCD[m_dma->dma_channel_num].CSR |= DMA_CSR_DREQ_MASK;

	// set up the source and destination addresses
	BF_ASSERT( m_mem_to_mem != uC_DMA_MEM2MEM_TRUE );

	if ( m_direction == uC_DMA_DIR_FROM_MEMORY )
	{
		m_dma->dma_ctrl->TCD[m_dma->dma_channel_num].SADDR = ( uint32_t )mem_ptr;
		m_dma->dma_ctrl->TCD[m_dma->dma_channel_num].DADDR = ( uint32_t )periph_ptr;
	}
	else// from peripheral
	{
		m_dma->dma_ctrl->TCD[m_dma->dma_channel_num].DADDR = ( uint32_t )mem_ptr;
		m_dma->dma_ctrl->TCD[m_dma->dma_channel_num].SADDR = ( uint32_t )periph_ptr;
	}
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void uC_DMA::Enable_End_Buff_Int( void )const
{
	// clear any current interrupt flags
	m_dma->dma_ctrl->CINT |= DMA_CINT_CINT( m_dma->dma_channel_num );
	// enable the end of loop interrupt
	m_dma->dma_ctrl->TCD[m_dma->dma_channel_num].CSR |= DMA_CSR_INTMAJOR_MASK;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_DMA::Disable_End_Buff_Int( void ) const
{
	// disable the end of loop interrupt
	m_dma->dma_ctrl->TCD[m_dma->dma_channel_num].CSR &= ~DMA_CSR_INTMAJOR_MASK;
	// clear any current interrupt flags
	m_dma->dma_ctrl->CINT |= DMA_CINT_CINT( m_dma->dma_channel_num );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_DMA::Enable( void )
{
	m_dma->dmamux_ctrl->CHCFG[m_dma->dmamux_num] |= DMAMUX_CHCFG_ENBL_MASK;
	m_dma->dma_ctrl->SERQ |= DMA_SERQ_SERQ( m_dma->dma_channel_num );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_DMA::Disable( void )
{
	// disable the DMA channel
	m_dma->dmamux_ctrl->CHCFG[m_dma->dmamux_num] &= ~DMAMUX_CHCFG_ENBL_MASK;
	m_dma->dma_ctrl->CERQ |= DMA_CERQ_CERQ( m_dma->dma_channel_num );

	// disable the end of loop interrupt
	m_dma->dma_ctrl->TCD[m_dma->dma_channel_num].CSR &= ~DMA_CSR_INTMAJOR_MASK;
	// clear any current interrupt flags
	m_dma->dma_ctrl->CINT |= DMA_CINT_CINT( m_dma->dma_channel_num );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_DMA::Enable_Int( void ) const
{
	uC_Interrupt::Enable_Int( m_dma->irq_num );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_DMA::Disable_Int( void ) const
{
	uC_Interrupt::Disable_Int( m_dma->irq_num );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_DMA::Clear_Int( void ) const
{
	uC_Interrupt::Clear_Int( m_dma->irq_num );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_DMA::Busy( void ) const
{
	BOOL busy;

	busy = ( ( m_dma->dma_ctrl->TCD[m_dma->dma_channel_num].CSR ) & DMA_CSR_ACTIVE_MASK ) == 0;

	return ( busy );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t uC_DMA::Counts_Remaining( void ) const
{
	volatile uint16_t temp_count;

	temp_count = ( m_dma->dma_ctrl->TCD[m_dma->dma_channel_num].CITER_ELINKNO ) & DMA_CITER_ELINKNO_CITER_MASK;
	// Removing this if-check. DMA_CCR1_DIR is non-zero, the if condition is always false.
	// if ( ( m_dma->dma_channel_ctrl->CR | DMA_SxCR_DIR ) == 0U )
	{
		temp_count = m_total_item_count - temp_count;
	}

	return ( temp_count );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint16_t uC_DMA::Counts_Done( void ) const
{
	volatile uint16_t temp_count;

	temp_count = ( m_dma->dma_ctrl->TCD[m_dma->dma_channel_num].CITER_ELINKNO ) & DMA_CITER_ELINKNO_CITER_MASK;
	// Removing this if-check. DMA_CCR1_DIR is non-zero, the if condition is always true.
	// if ( ( m_dma->dma_channel_ctrl->CR | DMA_SxCR_DIR ) != 0U )
	{
		temp_count = m_total_item_count - temp_count;
	}

	return ( temp_count );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_DMA::Deallocate_DMA_Stream( uint8_t dma_channel )
{
	uint8_t dma_stream_id = 0;

	// dma_stream_id = static_cast<uint8_t>(Get_DMA_Stream_Num( dma_channel ));
	Clr_Bit( m_active_dma, dma_stream_id );
}

/* lint +e1924
   lint +e1960*/
