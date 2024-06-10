/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_DMA_H
   #define uC_DMA_H

#include "uC_Interrupt.h"
#include "Advanced_Vect.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
typedef enum
{
	uC_DMA_DIR_FROM_PERIPH,
	uC_DMA_DIR_FROM_MEMORY
} uC_DMA_DIR_ENUM;

typedef enum
{
	uC_DMA_MEM_INC_FALSE,
	uC_DMA_MEM_INC_TRUE
} uC_DMA_MEM_INC_ENUM;

typedef enum
{
	uC_DMA_PERIPH_INC_FALSE,
	uC_DMA_PERIPH_INC_TRUE
} uC_DMA_PERIPH_INC_ENUM;

typedef enum
{
	uC_DMA_CIRC_FALSE,
	uC_DMA_CIRC_TRUE
} uC_DMA_CIRC_ENUM;

typedef enum
{
	uC_DMA_MEM2MEM_FALSE,
	uC_DMA_MEM2MEM_TRUE
} uC_DMA_MEM2MEM_ENUM;

#define uC_DMA_ALL_STATUS_FLAGS_MASK            0x3F

typedef struct volatile_item_t
{
	uint8_t* start;
	uint8_t* end;
	volatile_item_t* next;
	volatile_item_t* prev;
} volatile_item_t;

/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_DMA
{
	public:
		uC_DMA( uint8_t dma_channel, uC_DMA_DIR_ENUM direction = uC_DMA_DIR_FROM_PERIPH,
				uint8_t datatype_size = 1,
				uC_DMA_MEM_INC_ENUM mem_inc = uC_DMA_MEM_INC_TRUE,
				uC_DMA_PERIPH_INC_ENUM periph_inc = uC_DMA_PERIPH_INC_FALSE,
				uC_DMA_CIRC_ENUM circular = uC_DMA_CIRC_FALSE,
				uC_DMA_MEM2MEM_ENUM mem_to_mem = uC_DMA_MEM2MEM_FALSE );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~uC_DMA() {}

		bool Attach_Interrupt( INT_CALLBACK_FUNC* int_handler, uint16_t priority ) const;

		typedef void* cback_param_t;
		typedef void (* cback_func_t)( cback_param_t param );

		bool Attach_Callback( cback_func_t cback_func,
							  cback_param_t param, uint16_t priority );

		static uint8_t* const NULL_8b_DATA;
		static uint16_t* const NULL_16b_DATA;
		static uint32_t* const NULL_32b_DATA;
		void Set_Buff( uint8_t* mem_ptr, volatile void* periph_ptr, uint16_t length );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Set_Buff( uint16_t* mem_ptr, volatile void* periph_ptr, uint16_t length )
		{ Set_Buff( reinterpret_cast<uint8_t*>( mem_ptr ), periph_ptr, length ); }

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Set_Buff( uint32_t* mem_ptr, volatile void* periph_ptr, uint16_t length )
		{ Set_Buff( reinterpret_cast<uint8_t*>( mem_ptr ), periph_ptr, length ); }

		void Enable_End_Buff_Int( void ) const;

		void Disable_End_Buff_Int( void ) const;

		void Enable_Int( void ) const;

		void Disable_Int( void ) const;

		void Clear_Int( void ) const;

		static void Deallocate_DMA_Stream( uint8_t dma_channel );

		void Enable( void );

		void Disable( void );

		bool Busy( void ) const;

		uint16_t Counts_Remaining( void ) const;

		uint16_t Counts_Done( void ) const;

		inline bool Empty( void )   { return ( m_dma->dma_channel_ctrl->NDTR == 0 ); }

	private:
		uint32_t m_dummy_data;

		uC_BASE_DMA_IO_STRUCT const* m_dma;
		uint16_t m_total_item_count;	// Units of measure are items.
		uint8_t m_datatype_size;		// Units of measure are bytes.
		Advanced_Vect* m_advanced_vect;
		bool m_mem_inc;
		// This is a bitfield to keep track of which DMAs were already allocated.
		static uint32_t m_active_dma;
		volatile_item_t m_volatile_item;
};



#endif
