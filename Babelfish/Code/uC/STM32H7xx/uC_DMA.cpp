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
#define DMA_STREAM_MASK             ( ( static_cast<uint32_t>( 1U ) << DMA_STREAM_CHANNEL_SHIFT ) - 1U )
#define Get_DMA_Stream_Num( id )    ( ( id ) & DMA_STREAM_MASK )
#define Get_DMA_Request_Num( id )   ( static_cast<uint32_t>( id ) >> DMA_STREAM_CHANNEL_SHIFT )

const uint8_t IRQ_SHIFT[8] = { 0U, 6U, 16U, 22U, 0U, 6U, 16U, 22U };
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
uC_DMA::uC_DMA( uint16_t dma_channel, uC_DMA_DIR_ENUM direction,
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
	uint8_t dma_request_num;

	dma_request_num = static_cast<uint8_t>( Get_DMA_Request_Num( dma_channel ) );
	dma_stream_id = static_cast<uint8_t>( Get_DMA_Stream_Num( dma_channel ) );
	m_dma = ( uC_Base::Self() )->Get_DMA_Ctrl( dma_stream_id );

	if ( ( m_dma != NULL ) && ( !Test_Bit( m_active_dma, dma_stream_id ) ) )
	{
		Set_Bit32( m_active_dma, dma_stream_id );

		if ( !uC_Base::Is_Periph_Clock_Enabled( &m_dma->periph_def ) )
		{
			// DMAMUX1 does not have separate reset bit in RCC like STM32WB55
			// By reseting DMA1 or DMA2, DMAMUX1 is getting reset.
			// Thus we need to take backup of MUX configured channels.
			uint32_t dma1_channel_list[uC_BASE_DMA_STREAM_MAX];
			uint32_t* channel_ptr = reinterpret_cast<uint32_t*>( DMAMUX1_Channel0 );

			for ( uint8_t channel = 0; channel < uC_BASE_DMA_STREAM_MAX; channel++ )
			{
				// Store channel information
				dma1_channel_list[channel] = *channel_ptr;
				channel_ptr++;
			}
			uC_Base::Reset_Periph( &m_dma->periph_def );
			uC_Base::Enable_Periph_Clock( &m_dma->periph_def );

			channel_ptr = reinterpret_cast<uint32_t*>( DMAMUX1_Channel0 );

			for ( uint8_t channel = 0; channel < uC_BASE_DMA_STREAM_MAX; channel++ )
			{
				// Restore channel information
				*channel_ptr = dma1_channel_list[channel];
				channel_ptr++;
			}
		}

		/**
		 *@remark Coding Standard Deviation:
		 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to
		 * underlying type for sub-integers
		 *@n Justification: Generic #define is used here, recasting in that #define would have
		 * adverse effects throughout.
		 */
		/*lint -e{1960} # MISRA Deviation */
		// Here request number represents a peripheral which will get linked with DMA mux channel
		m_dma->dmamux_channel_ctrl->CCR = dma_request_num & DMAMUX_CxCR_DMAREQ_ID;

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
			Set_Mask( m_dma->dma_channel_ctrl->CR, DMA_SxCR_DIR, DMA_SxCR_DIR );
		}
		else if ( direction == uC_DMA_DIR_FROM_MEMORY )
		{
			/**
			 *@remark Coding Standard Deviation:
			 *@n MISRA-C++[2008] Required Rule 5-0-10, Operators '~' and '<<' require recasting to
			 * underlying type for sub-integers
			 *@n Justification: Generic #define is used here, recasting in that #define would have
			 * adverse effects throughout.
			 */
			/*lint -e{1960} # MISRA Deviation */
			Set_Mask( m_dma->dma_channel_ctrl->CR, DMA_SxCR_DIR, DMA_SxCR_DIR_0 );
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
			Set_Mask( m_dma->dma_channel_ctrl->CR, DMA_SxCR_DIR, 0U );
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
				Set_Mask( m_dma->dma_channel_ctrl->CR,
						  ( DMA_SxCR_PSIZE | DMA_SxCR_MSIZE ),
						  ( DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0 ) );
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
				Set_Mask( m_dma->dma_channel_ctrl->CR,
						  ( DMA_SxCR_PSIZE | DMA_SxCR_MSIZE ),
						  ( DMA_SxCR_PSIZE_1 | DMA_SxCR_MSIZE_1 ) );
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
				Set_Mask( m_dma->dma_channel_ctrl->CR,
						  ( DMA_SxCR_PSIZE | DMA_SxCR_MSIZE ), 0U );
				break;
		}

		if ( mem_inc == uC_DMA_MEM_INC_TRUE )
		{
			m_dma->dma_channel_ctrl->CR |= DMA_SxCR_MINC;
			m_mem_inc = true;
		}
		else
		{
			m_dma->dma_channel_ctrl->CR &= ~DMA_SxCR_MINC;
			m_mem_inc = false;
		}
		if ( periph_inc == uC_DMA_PERIPH_INC_TRUE )
		{
			m_dma->dma_channel_ctrl->CR |= DMA_SxCR_PINC;
		}
		else
		{
			m_dma->dma_channel_ctrl->CR &= ~DMA_SxCR_PINC;
		}

		if ( circular == uC_DMA_CIRC_TRUE )
		{
			m_dma->dma_channel_ctrl->CR |= DMA_SxCR_CIRC;
		}
		else
		{
			m_dma->dma_channel_ctrl->CR &= ~DMA_SxCR_CIRC;
		}

		m_advanced_vect = reinterpret_cast<Advanced_Vect*>( nullptr );

		m_volatile_item.start = reinterpret_cast<uint8_t*>( nullptr );
		m_volatile_item.end = reinterpret_cast<uint8_t*>( nullptr );
		m_volatile_item.next = reinterpret_cast<volatile_item_t*>( nullptr );
		m_volatile_item.prev = reinterpret_cast<volatile_item_t*>( nullptr );
	}
	else
	{
		BF_ASSERT( false );
	}
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
	m_dma->dma_channel_ctrl->CR &= ~DMA_SxCR_EN;	/// Must disable the DMA to set it up.

	if ( ( m_mem_inc == true ) && ( mem_ptr != NULL_8b_DATA ) )
	{
		m_dma->dma_channel_ctrl->CR |= DMA_SxCR_MINC;
	}
	else
	{
		mem_ptr = reinterpret_cast<uint8_t*>( &m_dummy_data );
		m_dma->dma_channel_ctrl->CR &= ~DMA_SxCR_MINC;
	}
	m_total_item_count = item_count;
	m_dma->dma_channel_ctrl->NDTR = item_count;
	m_dma->dma_channel_ctrl->M0AR = reinterpret_cast<uint32_t>( mem_ptr );
	m_dma->dma_channel_ctrl->PAR = reinterpret_cast<uint32_t>( periph_ptr );

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
	if ( m_dma->dma_num < 4U )
	{
		m_dma->dma_ctrl->LIFCR = DMA_LISR_TCIF0 << IRQ_SHIFT[m_dma->dma_num];	// Clear the
																				// appropriate flag.
	}
	else
	{
		m_dma->dma_ctrl->HIFCR = DMA_LISR_TCIF0 << IRQ_SHIFT[m_dma->dma_num];	// Clear the
																				// appropriate flag.
	}
	m_dma->dma_channel_ctrl->CR |= DMA_SxCR_TCIE;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_DMA::Disable_End_Buff_Int( void ) const
{
	m_dma->dma_channel_ctrl->CR &= ~DMA_SxCR_TCIE;

	if ( m_dma->dma_num < 4U )
	{
		m_dma->dma_ctrl->LIFCR = DMA_LISR_TCIF0 << IRQ_SHIFT[m_dma->dma_num];	// Clear the
																				// appropriate flag.
	}
	else
	{
		m_dma->dma_ctrl->HIFCR = DMA_LISR_TCIF0 << IRQ_SHIFT[m_dma->dma_num];	// Clear the
																				// appropriate flag.
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_DMA::Enable( void )
{
	m_dma->dma_channel_ctrl->CR |= DMA_SxCR_EN;

	RAM_Diag::uC_RAM::Mark_As_Volatile( &m_volatile_item );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_DMA::Disable( void )
{
	m_dma->dma_channel_ctrl->CR &= ~DMA_SxCR_EN;
	m_dma->dma_channel_ctrl->CR &= ~DMA_SxCR_TCIE;

	if ( m_dma->dma_num < 4U )
	{
		m_dma->dma_ctrl->LIFCR = static_cast<uint32_t>
			( uC_DMA_ALL_STATUS_FLAGS_MASK ) << IRQ_SHIFT[m_dma->dma_num];
	}
	else
	{
		m_dma->dma_ctrl->HIFCR = static_cast<uint32_t>
			( uC_DMA_ALL_STATUS_FLAGS_MASK ) << IRQ_SHIFT[m_dma->dma_num];
	}

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

	if ( m_dma->dma_num < 4U )
	{
		busy = ( m_dma->dma_ctrl->LISR & ( DMA_LISR_TCIF0 << IRQ_SHIFT[m_dma->dma_num] ) ) == 0U;
	}
	else
	{
		busy = ( m_dma->dma_ctrl->HISR & ( DMA_LISR_TCIF0 << IRQ_SHIFT[m_dma->dma_num] ) ) == 0U;
	}
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

	temp_count = static_cast<uint16_t>( m_dma->dma_channel_ctrl->NDTR );

	// Removing this if-check. DMA_CCR1_DIR is non-zero, the if condition is always false.
	// if ( ( m_dma->dma_channel_ctrl->CR | DMA_SxCR_DIR ) == 0U )
	{
		temp_count = m_total_item_count - temp_count;
	}

	return ( temp_count );
}

uint16_t uC_DMA::Counts_Done( void ) const
{
	volatile uint16_t temp_count;

	temp_count = static_cast<uint16_t>( m_dma->dma_channel_ctrl->NDTR );

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
void uC_DMA::Deallocate_DMA_Stream( uint8_t dma_stream )
{
	uint8_t dma_stream_id;

	dma_stream_id = static_cast<uint8_t>( Get_DMA_Stream_Num( dma_stream ) );
	Clr_Bit( m_active_dma, dma_stream_id );
}

/* lint +e1924
   lint +e1960*/
