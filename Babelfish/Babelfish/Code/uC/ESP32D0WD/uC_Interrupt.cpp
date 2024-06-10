/**
 *****************************************************************************************
 *	@file		uC_Interrupt.cpp
 *	@details	See header file for module overview.
 *	@copyright	2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include "uC_Interrupt.h"
#include "esp_intr_alloc.h"
#include <string.h>
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_ipc.h"
#include "freertos/task.h"
#include "freertos/xtensa_api.h"

/*
 *****************************************************************************************
 *		Extern function
 *****************************************************************************************
 */
extern void xt_unhandled_interrupt( void* arg );

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint16_t VECDESC_FL_RESERVED = ( 1 << 0 );
static const uint16_t VECDESC_FL_INIRAM = ( 1 << 1 );
static const uint16_t VECDESC_FL_SHARED = ( 1 << 2 );
static const uint16_t VECDESC_FL_NONSHARED = ( 1 << 3 );
static const uint8_t INT_MUX_DISABLED_INTNO = 6;
static const uint8_t ETS_INTERNAL_TIMER0_INTR_NO = 6;
static const uint8_t ETS_INTERNAL_TIMER1_INTR_NO = 15;
static const uint8_t ETS_INTERNAL_TIMER2_INTR_NO = 16;
static const uint8_t ETS_INTERNAL_SW0_INTR_NO = 7;
static const uint8_t ETS_INTERNAL_SW1_INTR_NO = 29;
static const uint8_t ETS_INTERNAL_PROFILING_INTR_NO = 11;
static const ptrdiff_t CACHE_START = 0x400C0000U;
static const ptrdiff_t CACHE_END = 0x50000000U;
static const uint8_t MAX_INTR_NUMBER = 32;
static const char* TAG = "intr_alloc";
/*
 *****************************************************************************************
 *		Static Variables
 *****************************************************************************************
 */
static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

// This bitmask has value 1 if the int32_t should be disabled when the flash is disabled.
static uint32_t non_iram_int_mask[portNUM_PROCESSORS];

// Linked list of vector descriptions, sorted by cpu.intno value
static vector_desc_t* vector_desc_head = NULL;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Interrupt::Set_Vector( INT_CALLBACK_FUNC* vector_callback, IRQn_Type vector_number,
							   uint16_t priority )
{
	intr_handle_data_t* ret = NULL;
	int32_t force = -1;
	uint32_t cpu;
	int32_t intr;
	interrupt_config_t* interrupt_config = reinterpret_cast<interrupt_config_t*>( vector_callback );

	ESP_EARLY_LOGV( TAG, "esp_intr_alloc_intrstatus (cpu %d): checking args", xPortGetCoreID() );

	/*Shared interrupts should be level-triggered.
	   You can't set an handler / arg for a non-C-callable interrupt.
	   Shared ints should have handler and non-processor-local source
	   Statusreg should have a mask */
	if ( ( ( interrupt_config->priority & ESP_INTR_FLAG_SHARED ) &&
		   ( interrupt_config->priority & ESP_INTR_FLAG_EDGE ) ) ||
		 ( ( interrupt_config->priority & ESP_INTR_FLAG_HIGH ) && ( interrupt_config->handler ) ) ||
		 ( ( interrupt_config->priority & ESP_INTR_FLAG_SHARED ) &&
		   ( !interrupt_config->handler || ( interrupt_config->source < 0 ) ) ) ||
		 ( interrupt_config->intrstatusreg && !interrupt_config->intrstatusmask ) )
	{
		return ( false );
	}

	// If the ISR is marked to be IRAM-resident, the handler must not be in the cached region
	if ( ( interrupt_config->priority & ESP_INTR_FLAG_IRAM ) &&
		 ( ( ptrdiff_t ) interrupt_config->handler >= CACHE_START ) &&
		 ( ( ptrdiff_t ) interrupt_config->handler < CACHE_END ) )
	{
		return ( false );
	}

	// Default to prio 1 for shared interrupts. Default to prio 1, 2 or 3 for non-shared interrupts.
	if ( ( interrupt_config->priority & ESP_INTR_FLAG_LEVELMASK ) == 0 )
	{
		if ( interrupt_config->priority & ESP_INTR_FLAG_SHARED )
		{
			interrupt_config->priority |= ESP_INTR_FLAG_LEVEL1;
		}
		else
		{
			interrupt_config->priority |= ESP_INTR_FLAG_LOWMED;
		}
	}
	ESP_EARLY_LOGV( TAG, "esp_intr_alloc_intrstatus (cpu %d): Args okay. Resulting priority 0x%X",
					xPortGetCoreID(), interrupt_config->priority );

	/*Check 'special' interrupt sources. These are tied to one specific interrupt, so we
	   have to force get_free_int to only look at that.*/
	switch ( interrupt_config->source )
	{
		case ETS_INTERNAL_TIMER0_INTR_SOURCE:
			force = ETS_INTERNAL_TIMER0_INTR_NO;
			break;

		case ETS_INTERNAL_TIMER1_INTR_SOURCE:
			force = ETS_INTERNAL_TIMER1_INTR_NO;
			break;

		case ETS_INTERNAL_TIMER2_INTR_SOURCE:
			force = ETS_INTERNAL_TIMER2_INTR_NO;
			break;

		case ETS_INTERNAL_SW0_INTR_SOURCE:
			force = ETS_INTERNAL_SW0_INTR_NO;
			break;

		case ETS_INTERNAL_SW1_INTR_SOURCE:
			force = ETS_INTERNAL_SW1_INTR_NO;
			break;

		case ETS_INTERNAL_PROFILING_INTR_SOURCE:
			force = ETS_INTERNAL_PROFILING_INTR_NO;
			break;

		default:
			break;
	}

	// Allocate a return handle. If we end up not needing it, we'll free it later on.
	ret = static_cast<intr_handle_data_t*>
		( heap_caps_malloc( sizeof( intr_handle_data_t ), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT ) );

	if ( ret == NULL )
	{
		return ( false );
	}

	portENTER_CRITICAL( &spinlock );
	cpu = xPortGetCoreID();

	// See if we can find an interrupt that matches the priority.
	intr = get_available_int( interrupt_config->priority, cpu, force, interrupt_config->source );
	if ( intr == -1 )
	{
		// None found. Bail out.
		portEXIT_CRITICAL( &spinlock );
		free( ret );
		return ( false );
	}

	// Get an int vector desc for int.
	vector_desc_t* vd = get_desc_for_int( intr, cpu );

	if ( vd == NULL )
	{
		portEXIT_CRITICAL( &spinlock );
		free( ret );
		return ( false );
	}

	// Allocate that int!
	if ( ( interrupt_config->priority & ESP_INTR_FLAG_SHARED ) != 0 )
	{
		// Populate vector entry and add to linked list.
		shared_vector_desc_t* sh_vec = static_cast<shared_vector_desc_t*>
			( malloc( sizeof( shared_vector_desc_t ) ) );
		if ( sh_vec == NULL )
		{
			portEXIT_CRITICAL( &spinlock );
			free( ret );
			return ( false );
		}
		memset( sh_vec, 0, sizeof( shared_vector_desc_t ) );
		sh_vec->statusreg = ( uint32_t* )interrupt_config->intrstatusreg;
		sh_vec->statusmask = interrupt_config->intrstatusmask;
		sh_vec->isr = interrupt_config->handler;
		sh_vec->arg = interrupt_config->arg;
		sh_vec->next = vd->shared_vec_info;
		sh_vec->source = interrupt_config->source;
		sh_vec->disabled = 0;
		vd->shared_vec_info = sh_vec;
		vd->flags |= VECDESC_FL_SHARED;
		// (Re-)set shared isr handler to new value.
		xt_set_interrupt_handler( intr, shared_intr_isr, vd );
	}
	else
	{
		// Mark as unusable for other interrupt sources. This is ours now!
		vd->flags = VECDESC_FL_NONSHARED;
		if ( interrupt_config->handler != NULL )
		{
#if CONFIG_SYSVIEW_ENABLE
			non_shared_isr_arg_t* ns_isr_arg = malloc( sizeof( non_shared_isr_arg_t ) );
			if ( !ns_isr_arg )
			{
				portEXIT_CRITICAL( &spinlock );
				free( ret );
				return ( false );
			}
			ns_isr_arg->isr = interrupt_config->handler;
			ns_isr_arg->isr_arg = interrupt_config->arg;
			ns_isr_arg->source = interrupt_config->source;
			xt_set_interrupt_handler( intr, non_shared_intr_isr, ns_isr_arg );
#else
			xt_set_interrupt_handler( intr, interrupt_config->handler, interrupt_config->arg );
#endif
		}
		if ( ( interrupt_config->priority & ESP_INTR_FLAG_EDGE ) != 0 )
		{
			xthal_set_intclear( 1 << intr );
		}
		vd->source = interrupt_config->source;
	}
	if ( ( interrupt_config->priority & ESP_INTR_FLAG_IRAM ) != 0 )
	{
		vd->flags |= VECDESC_FL_INIRAM;
		non_iram_int_mask[cpu] &= ~( 1 << intr );
	}
	else
	{
		vd->flags &= ~VECDESC_FL_INIRAM;
		non_iram_int_mask[cpu] |= ( 1 << intr );
	}
	if ( interrupt_config->source >= 0 )
	{
		intr_matrix_set( cpu, interrupt_config->source, intr );
	}

	// Fill return handle data.
	ret->vector_desc = vd;
	ret->shared_vector_desc = vd->shared_vec_info;

	// Enable int at CPU-level;
	ESP_INTR_ENABLE( intr );

	/*If interrupt has to be started disabled, do that now; ints won't be enabled for real until the end
	   of the critical section*/
	if ( ( interrupt_config->priority & ESP_INTR_FLAG_INTRDISABLED ) != 0 )
	{
		esp_intr_disable( ret );
	}
	portEXIT_CRITICAL( &spinlock );

	// Fill return handle if needed, otherwise free handle.
	if ( interrupt_config->ret_handle != NULL )
	{
		*interrupt_config->ret_handle = ret;
	}
	else
	{
		free( ret );
	}
	ESP_EARLY_LOGD( TAG, "Connected src %d to int %d (cpu %d)", interrupt_config->source, intr, cpu );
	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Interrupt::Clear_Vector( intr_handle_t handle )
{
	bool free_shared_vector = false;
//Ahmed
    #if !defined(CONFIG_FREERTOS_UNICORE) || defined(CONFIG_APPTRACE_GCOV_ENABLE)
        // Assign this routine to the core where this interrupt is allocated on.
        if ( handle->vector_desc->cpu != xPortGetCoreID() )
        {
            esp_ipc_call_blocking( handle->vector_desc->cpu, &esp_intr_free_cb, ( void* )handle );
        }
    #endif
	portENTER_CRITICAL( &spinlock );
	esp_intr_disable( handle );
	if ( ( handle->vector_desc->flags & VECDESC_FL_SHARED ) != 0 )
	{
		// Find and kill the shared int
		shared_vector_desc_t* svd = handle->vector_desc->shared_vec_info;
		shared_vector_desc_t* prevsvd = NULL;
		assert( svd );	///< Should be something in there for a shared int.
		while ( svd != NULL )
		{
			if ( svd == handle->shared_vector_desc )
			{
				// Found it. Now kill it.
				if ( prevsvd )
				{
					prevsvd->next = svd->next;
				}
				else
				{
					handle->vector_desc->shared_vec_info = svd->next;
				}
				free( svd );
				break;
			}
			prevsvd = svd;
			svd = svd->next;
		}
		// If nothing left, disable interrupt.
		if ( handle->vector_desc->shared_vec_info == NULL )
		{
			free_shared_vector = true;
		}
		ESP_LOGV( TAG, "esp_intr_free: Deleting shared int: %s. Shared int is %s",
				  svd ? "not found or last one" : "deleted", free_shared_vector ? "empty now." : "still in use" );
	}

	if ( ( handle->vector_desc->flags & VECDESC_FL_NONSHARED ) || free_shared_vector )
	{
		ESP_LOGV( TAG, "esp_intr_free: Disabling int, killing handler" );
#if CONFIG_SYSVIEW_ENABLE
		if ( !free_shared_vector )
		{
			void* isr_arg = xt_get_interrupt_handler_arg( handle->vector_desc->intno );
			if ( isr_arg )
			{
				free( isr_arg );
			}
		}
#endif
		// Reset to normal handler
		xt_set_interrupt_handler( handle->vector_desc->intno, xt_unhandled_interrupt,
								  ( void* )( ( int )handle->vector_desc->intno ) );
		// Theoretically, we could free the vector_desc... not sure if that's worth the few bytes of memory
		// we save.(We can also not use the same exit path for empty shared ints anymore if we delete
		// the desc.) For now, just mark it as free.
		handle->vector_desc->flags &= !( VECDESC_FL_NONSHARED | VECDESC_FL_RESERVED );
		// Also kill non_iram mask bit.
		non_iram_int_mask[handle->vector_desc->cpu] &= ~( 1 << ( handle->vector_desc->intno ) );
	}
	portEXIT_CRITICAL( &spinlock );
	free( handle );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Interrupt::Is_Vector_Empty( intr_handle_t handle )
{
	if ( handle->vector_desc->shared_vec_info == NULL )
	{
		return ( true );
	}
	else
	{
		return ( false );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Interrupt::Enable_Int( IRQn_Type vector_number, intr_handle_t handle )
{
	if ( !handle )
	{
		return ( false );
	}
	portENTER_CRITICAL( &spinlock );
	int source;

	if ( handle->shared_vector_desc != NULL )
	{
		handle->shared_vector_desc->disabled = 0;
		source = handle->shared_vector_desc->source;
	}
	else
	{
		source = handle->vector_desc->source;
	}
	if ( source >= 0 )
	{
		// Disabled using int matrix; re-connect to enable
		intr_matrix_set( handle->vector_desc->cpu, source, handle->vector_desc->intno );
	}
	else
	{
		// Re-enable using cpu int ena reg
		if ( handle->vector_desc->cpu != xPortGetCoreID() )
		{
			return ( false );	// Can only enable these ints on this cpu
		}
		ESP_INTR_ENABLE( handle->vector_desc->intno );
	}
	portEXIT_CRITICAL( &spinlock );
	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Interrupt::Disable_Int( IRQn_Type vector_number, intr_handle_t handle )
{
	if ( handle == NULL )
	{
		return ( false );
	}
	portENTER_CRITICAL( &spinlock );
	int source;
	bool disabled = true;

	if ( handle->shared_vector_desc != NULL )
	{
		handle->shared_vector_desc->disabled = 1;
		source = handle->shared_vector_desc->source;

		shared_vector_desc_t* svd = handle->vector_desc->shared_vec_info;
		assert( svd != NULL );
		while ( svd )
		{
			if ( ( svd->source == source ) && ( svd->disabled == 0 ) )
			{
				disabled = false;
				break;
			}
			svd = svd->next;
		}
	}
	else
	{
		source = handle->vector_desc->source;
	}

	if ( source >= 0 )
	{
		if ( disabled == true )
		{
			// Disable using int matrix
			intr_matrix_set( handle->vector_desc->cpu, source, INT_MUX_DISABLED_INTNO );
		}
	}
	else
	{
		// Disable using per-cpu regs
		if ( handle->vector_desc->cpu != xPortGetCoreID() )
		{
			portEXIT_CRITICAL( &spinlock );
			return ( false );	// Can only enable these ints on this cpu
		}
		ESP_INTR_DISABLE( handle->vector_desc->intno );
	}
	portEXIT_CRITICAL( &spinlock );
	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Interrupt::Clear_Int( IRQn_Type vector_number, intr_handler_t handler, int intr, void* arg, uint32_t cpu,
							  int flags )
{
	vector_desc_t* vd = get_desc_for_int( intr, cpu );

	vd->flags = VECDESC_FL_NONSHARED;
	if ( handler )
	{
#if CONFIG_SYSVIEW_ENABLE
		non_shared_isr_arg_t* ns_isr_arg = malloc( sizeof( non_shared_isr_arg_t ) );
		if ( !ns_isr_arg )
		{
			portEXIT_CRITICAL( &spinlock );
			free( ret );
			return;
		}
		ns_isr_arg->isr = handler;
		ns_isr_arg->isr_arg = arg;
		ns_isr_arg->source = source;
		xt_set_interrupt_handler( intr, non_shared_intr_isr, ns_isr_arg );
#else
		xt_set_interrupt_handler( intr, handler, arg );
#endif
	}
	if ( ( flags & ESP_INTR_FLAG_EDGE ) != 0 )
	{
		xthal_set_intclear( 1 << intr );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Interrupt::insert_vector_desc( vector_desc_t* to_insert )
{
	vector_desc_t* vd = vector_desc_head;
	vector_desc_t* prev = NULL;

	while ( vd != NULL )
	{
		if ( vd->cpu > to_insert->cpu )
		{
			break;
		}
		if ( ( vd->cpu == to_insert->cpu ) && ( vd->intno >= to_insert->intno ) )
		{
			break;
		}
		prev = vd;
		vd = vd->next;
	}
	if ( ( vector_desc_head == NULL ) || ( prev == NULL ) )
	{
		// First item
		to_insert->next = vd;
		vector_desc_head = to_insert;
	}
	else
	{
		prev->next = to_insert;
		to_insert->next = vd;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
vector_desc_t* uC_Interrupt::find_desc_for_int( int intno, uint32_t cpu )
{
	vector_desc_t* vd = vector_desc_head;

	while ( vd != NULL )
	{
		if ( ( vd->cpu == cpu ) && ( vd->intno == intno ) )
		{
			break;
		}
		vd = vd->next;
	}
	return ( vd );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
vector_desc_t* uC_Interrupt::get_desc_for_int( int intno, uint32_t cpu )
{
	vector_desc_t* vd = find_desc_for_int( intno, cpu );

	if ( vd == NULL )
	{
		vector_desc_t* newvd = static_cast<vector_desc_t*>
			( heap_caps_malloc( sizeof( vector_desc_t ), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT ) );
		if ( newvd == NULL )
		{
			return ( NULL );
		}
		memset( newvd, 0, sizeof( vector_desc_t ) );
		newvd->intno = intno;
		newvd->cpu = cpu;
		insert_vector_desc( newvd );
		return ( newvd );
	}
	else
	{
		return ( vd );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
vector_desc_t* uC_Interrupt::find_desc_for_source( int source, uint32_t cpu )
{
	vector_desc_t* vd = vector_desc_head;

	while ( vd != NULL )
	{
		if ( !( vd->flags & VECDESC_FL_SHARED ) )
		{
			if ( ( vd->source == source ) && ( cpu == vd->cpu ) )
			{
				break;
			}
		}
		else if ( vd->cpu == cpu )
		{
			// check only shared vds for the correct cpu, otherwise skip
			bool found = false;
			shared_vector_desc_t* svd = vd->shared_vec_info;
			assert( svd != NULL );
			while ( svd )
			{
				if ( svd->source == source )
				{
					found = true;
					break;
				}
				svd = svd->next;
			}
			if ( found != 0 )
			{
				break;
			}
		}
		vd = vd->next;
	}
	return ( vd );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Interrupt::int_has_handler( int intr, uint32_t cpu )
{
	return ( _xt_interrupt_table[intr * portNUM_PROCESSORS + cpu].handler != xt_unhandled_interrupt );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Interrupt::is_vect_desc_usable( vector_desc_t* vd, int flags, uint32_t cpu, int force )
{
	// Check if interrupt is not reserved by design
	int x = vd->intno;

	if ( int_desc[x].cpuflags[cpu] == INTDESC_RESVD )
	{
		ALCHLOG( "....Unusable: reserved" );
		return ( false );
	}
	if ( ( int_desc[x].cpuflags[cpu] == INTDESC_SPECIAL ) && ( force == -1 ) )
	{
		ALCHLOG( "....Unusable: special-purpose int" );
		return ( false );
	}
	// Check if the interrupt level is acceptable
	if ( ( flags & ( 1 << int_desc[x].level ) ) == 0 )
	{
		ALCHLOG( "....Unusable: incompatible level" );
		return ( false );
	}
	// check if edge/level type matches what we want
	if ( ( ( flags & ESP_INTR_FLAG_EDGE ) && ( int_desc[x].type == INTTP_LEVEL ) ) ||
		 ( ( ( !( flags & ESP_INTR_FLAG_EDGE ) ) && ( int_desc[x].type == INTTP_EDGE ) ) ) )
	{
		ALCHLOG( "....Unusable: incompatible trigger type" );
		return ( false );
	}
	// check if interrupt is reserved at runtime
	if ( vd->flags & VECDESC_FL_RESERVED )
	{
		ALCHLOG( "....Unusable: reserved at runtime." );
		return ( false );
	}

	// Ints can't be both shared and non-shared.
	assert( !( ( vd->flags & VECDESC_FL_SHARED ) && ( vd->flags & VECDESC_FL_NONSHARED ) ) );
	// check if interrupt already is in use by a non-shared interrupt
	if ( ( vd->flags & VECDESC_FL_NONSHARED ) != 0 )
	{
		ALCHLOG( "....Unusable: already in (non-shared) use." );
		return ( false );
	}
	// check shared interrupt flags
	if ( ( vd->flags & VECDESC_FL_SHARED ) != 0 )
	{
		if ( ( flags & ESP_INTR_FLAG_SHARED ) != 0 )
		{
			bool in_iram_flag = ( ( flags & ESP_INTR_FLAG_IRAM ) != 0 );
			bool desc_in_iram_flag = ( ( vd->flags & VECDESC_FL_INIRAM ) != 0 );
			// Bail out if int is shared, but iram property doesn't match what we want.
			if ( ( vd->flags & VECDESC_FL_SHARED ) && ( desc_in_iram_flag != in_iram_flag ) )
			{
				ALCHLOG( "....Unusable: shared but iram prop doesn't match" );
				return ( false );
			}
		}
		else
		{
			// We need an unshared IRQ; can't use shared ones; bail out if this is shared.
			ALCHLOG( "...Unusable: int is shared, we need non-shared." );
			return ( false );
		}
	}
	else if ( int_has_handler( x, cpu ) )
	{
		// Check if interrupt already is allocated by xt_set_interrupt_handler
		ALCHLOG( "....Unusable: already allocated" );
		return ( false );
	}

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
int uC_Interrupt::get_available_int( int flags, uint32_t cpu, int force, int source )
{
	int x;
	int best = -1;
	int bestLevel = 9;
	int bestSharedCt = INT_MAX;

	// Default vector desc, for vectors not in the linked list
	vector_desc_t empty_vect_desc;

	memset( &empty_vect_desc, 0, sizeof( vector_desc_t ) );

	// Level defaults to any low/med interrupt
	if ( !( flags & ESP_INTR_FLAG_LEVELMASK ) )
	{
		flags |= ESP_INTR_FLAG_LOWMED;
	}

	ALCHLOG( "get_available_int: try to find existing. Cpu: %d, Source: %d", cpu, source );
	vector_desc_t* vd = find_desc_for_source( source, cpu );

	if ( vd )
	{
		// if existing vd found, don't need to search any more.
		ALCHLOG( "get_avalible_int: existing vd found. intno: %d", vd->intno );
		if ( ( force != -1 ) && ( force != vd->intno ) )
		{
			ALCHLOG( "get_avalible_int: intr forced but not matach existing. existing intno: %d, force: %d", vd->intno,
					 force );
		}
		else if ( !is_vect_desc_usable( vd, flags, cpu, force ) )
		{
			ALCHLOG( "get_avalible_int: existing vd invalid." );
		}
		else
		{
			best = vd->intno;
		}
		return ( best );
	}
	if ( force != -1 )
	{
		ALCHLOG( "get_available_int: try to find force. Cpu: %d, Source: %d, Force: %d", cpu, source, force );
		// if force assigned, don't need to search any more.
		vd = find_desc_for_int( force, cpu );
		if ( vd == NULL )
		{
			// if existing vd not found, just check the default state for the intr.
			empty_vect_desc.intno = force;
			vd = &empty_vect_desc;
		}
		if ( is_vect_desc_usable( vd, flags, cpu, force ) )
		{
			best = vd->intno;
		}
		else
		{
			ALCHLOG( "get_avalible_int: forced vd invalid." );
		}
		return ( best );
	}

	ALCHLOG( "get_free_int: start looking. Current cpu: %d", cpu );
	// No allocated handlers as well as forced intr, iterate over the 32 possible interrupts
	for ( x = 0; x < MAX_INTR_NUMBER; x++ )
	{
		// Grab the vector_desc for this vector.
		vd = find_desc_for_int( x, cpu );
		if ( vd == NULL )
		{
			empty_vect_desc.intno = x;
			vd = &empty_vect_desc;
		}

		ALCHLOG( "Int %d reserved %d level %d %s hasIsr %d",
				 x, int_desc[x].cpuflags[cpu] == INTDESC_RESVD, int_desc[x].level,
				 int_desc[x].type == INTTP_LEVEL ? "LEVEL" : "EDGE", int_has_handler( x, cpu ) );

		if ( is_vect_desc_usable( vd, flags, cpu, force ) )
		{
			if ( ( flags & ESP_INTR_FLAG_SHARED ) != 0 )
			{
				// We're allocating a shared int.

				// See if int already is used as a shared interrupt.
				if ( ( vd->flags & VECDESC_FL_SHARED ) != 0 )
				{
					// We can use this already-marked-as-shared interrupt. Count the already attached isrs in order to
					// see
					// how useful it is.
					int no = 0;
					shared_vector_desc_t* svdesc = vd->shared_vec_info;
					while ( svdesc != NULL )
					{
						no++;
						svdesc = svdesc->next;
					}
					if ( ( no < bestSharedCt ) || ( bestLevel > int_desc[x].level ) )
					{
						// Seems like this shared vector is both okay and has the least amount of ISRs already attached
						// to
						// it.
						best = x;
						bestSharedCt = no;
						bestLevel = int_desc[x].level;
						ALCHLOG( "...int %d more usable as a shared int: has %d existing vectors", x, no );
					}
					else
					{
						ALCHLOG( "...worse than int %d", best );
					}
				}
				else
				{
					if ( best == -1 )
					{
						// We haven't found a feasible shared interrupt yet. This one is still free and usable, even if
						// not marked as shared.
						// Remember it in case we don't find any other shared interrupt that qualifies.
						if ( bestLevel > int_desc[x].level )
						{
							best = x;
							bestLevel = int_desc[x].level;
							ALCHLOG( "...int %d usable as a new shared int", x );
						}
					}
					else
					{
						ALCHLOG( "...already have a shared int" );
					}
				}
			}
			else
			{
				// Seems this interrupt is feasible. Select it and break out of the loop; no need to search further.
				if ( bestLevel > int_desc[x].level )
				{
					best = x;
					bestLevel = int_desc[x].level;
				}
				else
				{
					ALCHLOG( "...worse than int %d", best );
				}
			}
		}
	}
	ALCHLOG( "get_available_int: using int %d", best );

	// Okay, by now we have looked at all potential interrupts and hopefully have selected the best one in best.
	return ( best );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Interrupt::shared_intr_isr( void* arg )
{
	vector_desc_t* vd = ( vector_desc_t* )arg;
	shared_vector_desc_t* sh_vec = vd->shared_vec_info;

	portENTER_CRITICAL( &spinlock );
	while ( sh_vec )
	{
		if ( !sh_vec->disabled )
		{
			if ( ( sh_vec->statusreg == NULL ) || ( *sh_vec->statusreg & sh_vec->statusmask ) )
			{
#if CONFIG_SYSVIEW_ENABLE
				traceISR_ENTER( sh_vec->source + ETS_INTERNAL_INTR_SOURCE_OFF );
#endif
				sh_vec->isr( sh_vec->arg );
#if CONFIG_SYSVIEW_ENABLE
				// check if we will return to scheduler or to interrupted task after ISR
				if ( !port_switch_flag[xPortGetCoreID()] )
				{
					traceISR_EXIT();
				}
#endif
			}
		}
		sh_vec = sh_vec->next;
	}
	portEXIT_CRITICAL( &spinlock );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Interrupt::esp_intr_free_cb( void* arg )
{
	( void )Clear_Vector( ( intr_handle_t )arg );
}

#ifdef __cplusplus
}
#endif
