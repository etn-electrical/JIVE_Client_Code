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
 *		Constants
 *****************************************************************************************
 */

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
	m_dma( reinterpret_cast<uC_BASE_DMA_IO_STRUCT const*>( nullptr ) ),
	m_total_item_count( 0U ),
	m_datatype_size( 1U ),
	m_advanced_vect( reinterpret_cast<Advanced_Vect*>( nullptr ) )
{
	m_dma = ( uC_Base::Self() )->Get_DMA_Ctrl( dma_channel );

	BF_ASSERT( ( m_dma != NULL ) && ( !Test_Bit( m_active_dma, dma_channel ) ) );

	Set_Bit32( m_active_dma, dma_channel );

	if ( !uC_Base::Is_Periph_Clock_Enabled( &m_dma->periph_def ) )
	{
		uC_Base::Reset_Periph( &m_dma->periph_def );
		uC_Base::Enable_Periph_Clock( &m_dma->periph_def );
	}

	if ( direction == uC_DMA_DIR_FROM_MEMORY )
	{
		m_dma->dma_channel_ctrl->CCR |= DMA_CCR_DIR;
	}
	else
	{
		m_dma->dma_channel_ctrl->CCR &= ~DMA_CCR_DIR;
	}

	m_dma->dma_channel_ctrl->CCR &= ~( DMA_CCR_PSIZE | DMA_CCR_MSIZE );

	switch ( datatype_size )
	{
		case 2:
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
					  ( DMA_CCR_PSIZE | DMA_CCR_MSIZE ),
					  ( DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 ) );
			break;

		case 4:
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
					  ( DMA_CCR_PSIZE | DMA_CCR_MSIZE ),
					  ( DMA_CCR_PSIZE_1 | DMA_CCR_MSIZE_1 ) );
			break;

		case 1:
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
					  ( DMA_CCR_PSIZE | DMA_CCR_MSIZE ), 0U );
			break;
	}

	if ( mem_inc == uC_DMA_MEM_INC_TRUE )
	{
		m_dma->dma_channel_ctrl->CCR |= DMA_CCR_MINC;
	}
	else
	{
		m_dma->dma_channel_ctrl->CCR &= ~DMA_CCR_MINC;
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

	if ( mem_to_mem == uC_DMA_MEM2MEM_TRUE )
	{
		m_dma->dma_channel_ctrl->CCR |= DMA_CCR_MEM2MEM;
	}
	else
	{
		m_dma->dma_channel_ctrl->CCR &= ~DMA_CCR_MEM2MEM;
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
void uC_DMA::Set_Buff( uint8_t* mem_ptr, volatile void* periph_ptr, uint16_t length )
{
	m_dma->dma_channel_ctrl->CCR &= ~DMA_CCR_EN;	/// Must disable the DMA to set it up.

	m_total_item_count = length;
	m_dma->dma_channel_ctrl->CNDTR = length;
	m_dma->dma_channel_ctrl->CMAR = reinterpret_cast<uint32_t>( mem_ptr );
	m_dma->dma_channel_ctrl->CPAR = reinterpret_cast<uint32_t>( periph_ptr );

	RAM_Diag::uC_RAM::Unmark_As_Volatile( &m_volatile_item );
	m_volatile_item.start = mem_ptr;
	m_volatile_item.end = ( mem_ptr + ( m_datatype_size * length ) ) - 1U;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_DMA::Enable_End_Buff_Int( void ) const
{
	m_dma->dma_ctrl->IFCR = static_cast<uint32_t>
		( 1U ) << ( static_cast<uint32_t>( m_dma->dma_num ) << MULT_BY_4 );										// Clear
																												// the
																												// appropriate
																												// flag.
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
	m_dma->dma_ctrl->IFCR = static_cast<uint32_t>
		( 1U ) << ( static_cast<uint32_t>( m_dma->dma_num ) << MULT_BY_4 );										// Clear
																												// the
																												// appropriate
																												// flag.
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
	m_dma->dma_ctrl->IFCR = static_cast<uint32_t>
		( uC_DMA_ALL_STATUS_FLAGS_MASK ) <<
		( static_cast<uint32_t>( m_dma->dma_num ) << MULT_BY_4 );

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
	return ( ( m_dma->dma_ctrl->ISR &
			   ( static_cast<uint32_t>
				 ( DMA_ISR_TCIF1 ) <<
				 ( static_cast<uint32_t>( m_dma->dma_num ) << MULT_BY_4 ) ) ) == 0U );
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

	// Removing this if-check. DMA_CCR_DIR is non-zero, the if condition is always false.
	// if ( ( m_dma->dma_channel_ctrl->CCR | DMA_CCR_DIR ) == 0U )
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

	// Removing this if-check. DMA_CCR_DIR is non-zero, the if condition is always true.
	// if ( ( m_dma->dma_channel_ctrl->CCR | DMA_CCR_DIR ) != 0U )
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
#if 0
	/* The changes done in the 207/407 shall be ported to 107 files as well. */
	uint8_t dma_stream_id;
	dma_stream_id = static_cast<uint8_t>( Get_DMA_Stream_Num( dma_channel ) );
	Clr_Bit( m_active_dma, dma_stream_id );
#endif
}

/* lint +e1924
  lint +e1960*/
