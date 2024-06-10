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
#ifndef DEVICE_DEFINES_H
   #define DEVICE_DEFINES_H


/*
 *****************************************************************************************
 *		Register handling size
 *****************************************************************************************
 */
// This is the byte size that the microcontroller uses most efficiently.
// For an AVR this value would be 1.  For a 32 bit ARM it would be 4.  For a 16bit DSP it would be 2.
#define PROCESSOR_REGISTER_BASE_SIZE            4U

// Will increase the size to meet micro alignment requirements.
#define Mem_Aligned_Size( mem_size )    \
	( ( mem_size + ( PROCESSOR_REGISTER_BASE_SIZE - 1U ) ) & \
	  ~static_cast<uint32_t>( PROCESSOR_REGISTER_BASE_SIZE - 1U ) )

/*
 *****************************************************************************************
 *		Flash Memory Max Read Speed
 *****************************************************************************************
 */
#define MAX_FLASH_READ_SPEED            30000000U		// In MHz

/*
 *****************************************************************************************
 *		Bit mask helper for variable bit mask.
 *****************************************************************************************
 */
// The following provide an optimized set,clr,tst bit functions.
// These functions use a bitmask instead of requiring a shift.
// This facilitates instances where the code needs a variable bit location.
// ie: Set_BitN( byte, bit_i ) where i is a variable not a constant.
// If you don't define BIT_MASK_SETN_CLRN_TESTN_ENABLE the code will not be included in defines.c.
#define USE_8BIT_MASK_FOR_SETN_CLRN_TESTN       0
#define USE_16BIT_MASK_FOR_SETN_CLRN_TESTN      1
#define USE_32BIT_MASK_FOR_SETN_CLRN_TESTN      2
// #define BIT_MASK_SETN_CLRN_TESTN_ENABLE		USE_16BIT_MASK_FOR_SETN_CLRN_TESTN


/*
 *****************************************************************************************
 *		Public Typedef's
 *****************************************************************************************
 */

// Legacy datatype definitions.
typedef bool BOOL;
typedef unsigned char UINT8;
typedef signed char SINT8;
typedef unsigned short UINT16;
typedef signed short SINT16;
typedef unsigned int UINT32;
typedef signed int SINT32;
typedef unsigned long long UINT64;
typedef signed long long SINT64;
typedef float FLOAT;
typedef double DFLOAT;


// New datatype definitions.
typedef double float64_t;


#define MAX_UINT8 0xFF
#define MAX_UINT16 0xFFFF
#define MAX_UINT32 0xFFFFFFFF

#define MAX_INT8 0x7F
#define MAX_INT16 0x7FFF
#define MAX_INT32 0x7FFFFFFF

/// This contains the default pointer type to the memory locations of
/// the peripherals.  This is the type we will use to define the pointer
/// to the base memory location of the peripheral.
typedef unsigned long ULBASE;


/*
 *****************************************************************************************
 *	Interrupt Compatibility Stuff.
 *****************************************************************************************
 */
// #define __interrupt 			__irq __arm		//Because ARM compiler uses a different descriptor
// #define __fast_interrupt 		__fiq __arm		//Because ARM compiler uses a different descriptor

/*
 *****************************************************************************************
 *	Flash const override.
 *****************************************************************************************
 */
#define flash                   const
#define ROM_CONST               const

/*
 *****************************************************************************************
 *	These are designed to store the interrupt bit so you can disable and return the
 *	interrupts to the original state.
 *****************************************************************************************
 */
typedef uint32_t GINT_TDEF;
// void Push_Int( GINT_TDEF& int_bit_temp );
#define Push_GINT( temp )               Push_Int( temp )
// void Pop_Int( GINT_TDEF int_bit_temp );
#define Pop_GINT( temp )                Pop_Int( temp )

#ifdef __cplusplus
inline void Push_Int( GINT_TDEF& int_bit_temp ) { int_bit_temp = __get_PRIMASK();__set_PRIMASK( 1 ); }

inline void Pop_Int( GINT_TDEF int_bit_temp )   { __set_PRIMASK( int_bit_temp ); }

#endif
/*
 *****************************************************************************************
 *	The following are the same as the above except these create the temp variable on
 *	the stack to make it a little cleaner.
 *****************************************************************************************
 */
#define Push_TGINT()    GINT_TDEF stack_int_bit;Push_Int( stack_int_bit )
#define Pop_TGINT()     Pop_Int( stack_int_bit )
#define Push_TGINTS()   Push_Int( stack_int_bit )


/*
 *****************************************************************************************
 *	Interrupt control functions.  Processor and compiler specific.
 *****************************************************************************************
 */
#define Disable_Interrupts()        __disable_interrupt()
#define Enable_Interrupts()         __enable_interrupt()
#ifdef DEBUG
	#define Debug_Break()               __asm( "BKPT #0\n" )
#else
	#define Debug_Break()
#endif

/*
 *****************************************************************************************
 *	Nop define
 *****************************************************************************************
 */
#define Nop()           __no_operation()	// _NOP()

/*
 *****************************************************************************************
 *	A very basic microsecond delay.
 *	Update: This delay has been calibrated to be accurate to +/- 2% within the range
 *	of 1us to 200us (with a divisor below of 10) and may be accurate outside of that
 *	range.  This applies to the Debug build configuration that has no compiler optimization
 *	as well as the With_Uberloader build configuration that has high compiler optimization.
 *	C9903612
 *****************************************************************************************
 */
#define uC_Delay( microseconds )            \
	for ( volatile uint_fast16_t ijklmno =      \
			  ( microseconds * ( ( MASTER_CLOCK_FREQUENCY / 1000000U ) / 10U ) ); \
		  ijklmno > 0U; ijklmno-- ){}

/*
 *****************************************************************************************
 *		Bit behavior.
 *****************************************************************************************
 */
#define RAM_BASE       0x20000000
#define RAM_BB_BASE    0x22000000

#define Var_ResetBit_BB( VarAddr, BitNumber )    \
	( *( __IO uint32_t* ) ( RAM_BB_BASE | ( ( VarAddr - RAM_BASE ) << 5 ) | ( ( BitNumber ) << 2 ) ) = 0 )
#define Var_SetBit_BB( VarAddr, BitNumber )       \
	( *( __IO uint32_t* ) ( RAM_BB_BASE | ( ( VarAddr - RAM_BASE ) << 5 ) | ( ( BitNumber ) << 2 ) ) = 1 )
#define Var_GetBit_BB( VarAddr, BitNumber )       \
	( *( __IO uint32_t* ) ( RAM_BB_BASE | ( ( VarAddr - RAM_BASE ) << 5 ) | ( ( BitNumber ) << 2 ) ) )

#endif
