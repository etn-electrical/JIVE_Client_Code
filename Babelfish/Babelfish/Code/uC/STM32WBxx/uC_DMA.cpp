/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
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
#define DMA_STREAM_MASK             ( ( static_cast<uint32_t> \
										( 1U ) << DMA_STREAM_CHANNEL_SHIFT ) - 1U )
#define Get_DMA_Channel_Num( id )    ( ( id ) & DMA_STREAM_MASK )
#define Get_DMA_Request_Num( id )    ( static_cast<uint32_t>( id ) >> DMA_STREAM_CHANNEL_SHIFT )
const uint8_t IRQ_SHIFT[8] = { 0U, 0U, 4U, 8U, 12U, 16U, 20U, 24U };
uint8_t* const uC_DMA::NULL_8b_DATA = reinterpret_cast<uint8_t*>( nullptr );
uint16_t* const uC_DMA::NULL_16b_DATA = reinterpret_cast<uint16_t*>( nullptr );
uint32_t* const uC_DMA::NULL_32b_DATA = reinterpret_cast<uint32_t*>( nullptr );

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
uint32_t uC_DMA::m_active_dma = 0U;	// record of which DMA channel is in use to maintain exclusivity

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_DMA::uC_DMA( DMA_Channel_MUX* dma_channel_mux, uC_DMA_DIR_ENUM direction,
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
	m_dma = ( uC_Base::Self() )->Get_DMA_Ctrl( dma_channel_mux->DMA_Channel );
	BF_ASSERT( ( m_dma != NULL ) && ( !Test_Bit( m_active_dma, dma_channel_mux->DMA_Channel ) ) );
	Set_Bit32( m_active_dma, dma_channel_mux->DMA_Channel );

	if ( !uC_Base::Is_Periph_Clock_Enabled( &m_dma->periph_def ) )
	{
		uC_Base::Reset_Periph( &m_dma->periph_def );
		uC_Base::Enable_Periph_Clock( &m_dma->periph_def );
	}

	/**
	 *@remark Coding Standard Deviation:
	 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to
	 * underlying type for sub-integers
	 *@n Justification: Generic #define is used here, recasting in that #define would have
	 * adverse effects throughout.
	 */
	/*lint -e{1960} # MISRA Deviation */
	// Set_Mask( m_dma->dma_channel_ctrl->CR, DMA_SxCR_CHSEL,
	// ( DMA_SxCR_CHSEL_0 * Get_DMA_Channel_Num( dma_required_for ) ) );

	m_dma->dmamux_channel_ctrl->CCR = dma_channel_mux->DMA_Requested_by & DMAMUX_CxCR_DMAREQ_ID;
	if ( mem_to_mem == uC_DMA_MEM2MEM_TRUE )
	{
		/**
		 *@remark Coding Standard Deviation:
		 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to
		 * underlying type for sub-integers
		 *@n Justification: Generic #define is used here, recasting in that #define would have
		 * adverse effects throughout.
		 */
		/*lint -e{1960} # MISRA Deviation */
		Set_Mask( m_dma->dma_channel_ctrl->CCR, DMA_CCR_MEM2MEM, DMA_CCR_MEM2MEM );
	}
	else
	{
		Set_Mask( m_dma->dma_channel_ctrl->CCR, DMA_CCR_MEM2MEM, 0U );
	}

	if ( direction == uC_DMA_DIR_FROM_MEMORY )
	{
		/**
		 *@remark Coding Standard Deviation:
		 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to
		 * underlying type for sub-integers
		 *@n Justification: Generic #define is used here, recasting in that #define would have
		 * adverse effects throughout.
		 */
		/*lint -e{1960} # MISRA Deviation */
		Set_Mask( m_dma->dma_channel_ctrl->CCR, DMA_CCR_DIR_Msk, DMA_CCR_DIR );
	}
	else
	{
		/**
		 *@remark Coding Standard Deviation:
		 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to
		 * underlying type for sub-integers
		 *@n Justification: Generic #define is used here, recasting in that #define would have
		 * adverse effects throughout.
		 */
		/*lint -e{1960} # MISRA Deviation */
		Set_Mask( m_dma->dma_channel_ctrl->CCR, DMA_CCR_DIR_Msk, 0U );
	}

	switch ( datatype_size )
	{
		case 2U:
			m_datatype_size = 2U;
			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting
			 * to underlying type for sub-integers
			 *@n Justification: Generic #define is used here, recasting in that #define would
			 * have adverse effects throughout.
			 */
			/*lint -e{1960} # MISRA Deviation */
			Set_Mask( m_dma->dma_channel_ctrl->CCR,
					  ( DMA_CCR_MSIZE | DMA_CCR_PSIZE ),
					  ( DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 ) );
			break;

		case 4U:
			m_datatype_size = 4U;
			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting
			 * to underlying type for sub-integers
			 *@n Justification: Generic #define is used here, recasting in that #define would
			 * have adverse effects throughout.
			 */
			/*lint -e{1960} # MISRA Deviation */
			Set_Mask( m_dma->dma_channel_ctrl->CCR,
					  ( DMA_CCR_MSIZE | DMA_CCR_PSIZE ),
					  ( DMA_CCR_MSIZE_1 | DMA_CCR_PSIZE_1 ) );
			break;

		case 1U:
		default:
			m_datatype_size = 1U;
			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting
			 * to underlying type for sub-integers
			 *@n Justification: Generic #define is used here, recasting in that #define would
			 * have adverse effects throughout.
			 */
			/*lint -e{1960} # MISRA Deviation */
			Set_Mask( m_dma->dma_channel_ctrl->CCR,
					  ( DMA_CCR_MSIZE | DMA_CCR_PSIZE ), 0U );
			break;
	}

	if ( mem_inc == uC_DMA_MEM_INC_TRUE )
	{
		m_dma->dma_channel_ctrl->CCR |= DMA_CCR_MINC;
		m_mem_inc = true;
	}
	else
	{
		m_dma->dma_channel_ctrl->CCR &= ~DMA_CCR_MINC;
		m_mem_inc = false;
	}
	if ( periph_inc == uC_DMA_PERIPH_INC_TRUE )
	{
		m_dma->dma_channel_ctrl->CCR |= DMA_CCR_PINC;
	}
	else
	{
		m_dma->dma_channel_ctrl->CCR &= ~DMA_CCR_PINC;
	}

	if ( circular == uC_DMA_CIRC_TRUE )
	{
		m_dma->dma_channel_ctrl->CCR |= DMA_CCR_CIRC;
	}
	else
	{
		m_dma->dma_channel_ctrl->CCR &= ~DMA_CCR_CIRC;
	}

	m_advanced_vect = reinterpret_cast<Advanced_Vect*>( nullptr );

	m_volatile_item.start = reinterpret_cast<uint8_t*>( nullptr );
	m_volatile_item.end = reinterpret_cast<uint8_t*>( nullptr );
	m_volatile_item.next = reinterpret_cast<volatile_item_t*>( nullptr );
	m_volatile_item.prev = reinterpret_cast<volatile_item_t*>( nullptr );
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
	m_dma->dma_channel_ctrl->CCR &= ~DMA_CCR_EN;	/// Must disable the DMA to set it up.

	if ( ( m_mem_inc == true ) && ( mem_ptr != NULL_8b_DATA ) )
	{
		m_dma->dma_channel_ctrl->CCR |= DMA_CCR_MINC;
	}
	else
	{
		mem_ptr = reinterpret_cast<uint8_t*>( &m_dummy_data );
		m_dma->dma_channel_ctrl->CCR &= ~DMA_CCR_MINC;
	}
	m_total_item_count = item_count;
	m_dma->dma_channel_ctrl->CNDTR = item_count;
	m_dma->dma_channel_ctrl->CMAR = reinterpret_cast<uint32_t>( mem_ptr );
	m_dma->dma_channel_ctrl->CPAR = reinterpret_cast<uint32_t>( periph_ptr );

	RAM_Diag::uC_RAM::Unmark_As_Volatile( &m_volatile_item );
	m_volatile_item.start = mem_ptr;
	m_volatile_item.end = ( mem_ptr + ( m_datatype_size * item_count ) ) - 1U;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_DMA::Enable_End_Buff_Int( void ) const
{
	m_dma->dma_ctrl->IFCR = DMA_ISR_TCIF1 << IRQ_SHIFT[m_dma->dma_num];	// Clear the appropriate flag.

	m_dma->dma_channel_ctrl->CCR |= DMA_CCR_TCIE;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_DMA::Disable_End_Buff_Int( void ) const
{
	m_dma->dma_channel_ctrl->CCR &= ~DMA_CCR_TCIE;

	m_dma->dma_ctrl->IFCR = DMA_ISR_TCIF1 << IRQ_SHIFT[m_dma->dma_num];	// Clear the appropriate flag.
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_DMA::Enable( void )
{
	m_dma->dma_channel_ctrl->CCR |= DMA_CCR_EN;

	RAM_Diag::uC_RAM::Mark_As_Volatile( &m_volatile_item );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_DMA::Disable( void )
{
	m_dma->dma_channel_ctrl->CCR &= ~DMA_CCR_EN;
	m_dma->dma_channel_ctrl->CCR &= ~DMA_CCR_TCIE;
	m_dma->dma_ctrl->IFCR = static_cast<uint32_t>( uC_DMA_ALL_STATUS_FLAGS_MASK ) << IRQ_SHIFT[m_dma->dma_num];
	RAM_Diag::uC_RAM::Unmark_As_Volatile( &m_volatile_item );
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
	bool busy;

	busy = ( m_dma->dma_ctrl->ISR & ( DMA_ISR_TCIF1 << IRQ_SHIFT[m_dma->dma_num] ) ) == 0U;
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

	temp_count = static_cast<uint16_t>( m_dma->dma_channel_ctrl->CNDTR );

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

	temp_count = static_cast<uint16_t>( m_dma->dma_channel_ctrl->CNDTR );

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
	Clr_Bit( m_active_dma, /*dma_channel_id8*/ dma_channel );
}

/* lint +e1924
   lint +e1960*/
