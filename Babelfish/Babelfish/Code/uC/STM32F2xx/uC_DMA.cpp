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
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-2-4, C-style casts and notation
 * casts shall not be used
 * @n Justification: The ST supplied includes are written for both C and C++ therefore C-style casts
 * are required for all headers used
 * @n in the HAL. The note is re-enabled at the end of this file
 */
/*lint -e1924 */

/**
 * @remark Coding Standard Deviation:
 * @n MISRA-C++[2008] Rule: 5-0-4
 * @n PCLint: Note 1924: Violates MISRA C++ 2008 Required Rule 5-0-4, Implicit integral conversions
 * shall not change the signedness of the underlying type
 * @n Justification: The ST supplied includes are needed for the HAL - The includes will be used as
 * is The note is re-enabled at the end of this file
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
#define DMA_STREAM_MASK             ( ( static_cast<uint32_t> \
										( 1U ) << DMA_STREAM_CHANNEL_SHIFT ) - 1U )
#define Get_DMA_Stream_Num( id )    ( ( id ) & DMA_STREAM_MASK )
#define Get_DMA_Channel_Num( id )   ( static_cast<uint32_t>( id ) >> DMA_STREAM_CHANNEL_SHIFT )

// Not referenced.
// const uint8_t IRQ_SHIFT[8] =          { 0U, 6U, 16U, 22U, 0U, 6U, 16U, 22U };
const uint8_t MAX_DMA_CHANNELS = 8;
static const uint32_t DMA_CLEAR_FLAGS[MAX_DMA_CHANNELS] =
{
	DMA_LIFCR_CTCIF0 | DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTEIF0 | DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CFEIF0,
	DMA_LIFCR_CTCIF1 | DMA_LIFCR_CHTIF1 | DMA_LIFCR_CTEIF1 | DMA_LIFCR_CDMEIF1 | DMA_LIFCR_CFEIF1,
	DMA_LIFCR_CTCIF2 | DMA_LIFCR_CHTIF2 | DMA_LIFCR_CTEIF2 | DMA_LIFCR_CDMEIF2 | DMA_LIFCR_CFEIF2,
	DMA_LIFCR_CTCIF3 | DMA_LIFCR_CHTIF3 | DMA_LIFCR_CTEIF3 | DMA_LIFCR_CDMEIF3 | DMA_LIFCR_CFEIF3,
	DMA_HIFCR_CTCIF4 | DMA_HIFCR_CHTIF4 | DMA_HIFCR_CTEIF4 | DMA_HIFCR_CDMEIF4 | DMA_HIFCR_CFEIF4,
	DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5,
	DMA_HIFCR_CTCIF6 | DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTEIF6 | DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CFEIF6,
	DMA_HIFCR_CTCIF7 | DMA_HIFCR_CHTIF7 | DMA_HIFCR_CTEIF7 | DMA_HIFCR_CDMEIF7 | DMA_HIFCR_CFEIF7
};

static const uint32_t DMA_TXC_FLAGS[MAX_DMA_CHANNELS] =
{
	DMA_LIFCR_CTCIF0,
	DMA_LIFCR_CTCIF1,
	DMA_LIFCR_CTCIF2,
	DMA_LIFCR_CTCIF3,
	DMA_HIFCR_CTCIF4,
	DMA_HIFCR_CTCIF5,
	DMA_HIFCR_CTCIF6,
	DMA_HIFCR_CTCIF7
};

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

	dma_stream_id = static_cast<uint8_t>( Get_DMA_Stream_Num( dma_channel ) );
	m_dma = ( uC_Base::Self() )->Get_DMA_Ctrl( dma_stream_id );

	BF_ASSERT( ( m_dma != NULL ) && ( !Test_Bit( m_active_dma, dma_stream_id ) ) );

	Set_Bit( m_active_dma, dma_stream_id );

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
	Set_Mask( m_dma->dma_channel_ctrl->CR, DMA_SxCR_CHSEL,
			  ( DMA_SxCR_CHSEL_0 * Get_DMA_Channel_Num( dma_channel ) ) );

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
		m_dma->dma_ctrl->LIFCR = DMA_TXC_FLAGS[m_dma->dma_num];	// DMA_LISR_TCIF0<<IRQ_SHIFT[m_dma->dma_num];	//Clear
																// the appropriate flag.
	}
	else
	{
		m_dma->dma_ctrl->HIFCR = DMA_TXC_FLAGS[m_dma->dma_num];	// DMA_LISR_TCIF0<<IRQ_SHIFT[m_dma->dma_num];	//Clear
																// the appropriate flag.
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
		m_dma->dma_ctrl->LIFCR = DMA_TXC_FLAGS[m_dma->dma_num];	// DMA_LISR_TCIF0<<IRQ_SHIFT[m_dma->dma_num];	//Clear
																// the appropriate flag.
	}
	else
	{
		m_dma->dma_ctrl->HIFCR = DMA_TXC_FLAGS[m_dma->dma_num];	// DMA_LISR_TCIF0<<IRQ_SHIFT[m_dma->dma_num];	//Clear
																// the appropriate flag.
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
		m_dma->dma_ctrl->LIFCR = DMA_CLEAR_FLAGS[m_dma->dma_num];	// static_cast<uint32_t>(uC_DMA_ALL_STATUS_FLAGS_MASK)<<IRQ_SHIFT[m_dma->dma_num];
	}
	else
	{
		m_dma->dma_ctrl->HIFCR = DMA_CLEAR_FLAGS[m_dma->dma_num];	// static_cast<uint32_t>(uC_DMA_ALL_STATUS_FLAGS_MASK)<<IRQ_SHIFT[m_dma->dma_num];
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
		if ( ( m_dma->dma_ctrl->LISR & DMA_TXC_FLAGS[m_dma->dma_num] ) == 0U )
		{
			busy = true;
		}
		else
		{
			busy = false;
		}

		// busy = ( m_dma->dma_ctrl->LISR & ( DMA_LISR_TCIF0<<IRQ_SHIFT[m_dma->dma_num] ) ) == 0U;
	}
	else
	{
		if ( ( m_dma->dma_ctrl->HISR & DMA_TXC_FLAGS[m_dma->dma_num] ) == 0U )
		{
			busy = true;
		}
		else
		{
			busy = false;
		}
		// busy = ( m_dma->dma_ctrl->HISR & ( DMA_LISR_TCIF0<<IRQ_SHIFT[m_dma->dma_num] ) ) == 0U;
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

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
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
void uC_DMA::Deallocate_DMA_Stream( uint8_t dma_channel )
{
	uint8_t dma_stream_id;

	dma_stream_id = static_cast<uint8_t>( Get_DMA_Stream_Num( dma_channel ) );
	Clr_Bit( m_active_dma, dma_stream_id );
}

/* lint +e1924
  lint +e1960*/
