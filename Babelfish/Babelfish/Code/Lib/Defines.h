/**
 *****************************************************************************************
 *	@file Defines.h
 *
 *	@brief This file contains the general typedefs along with some general data
 *       to be helpful to the project.
 *
 *	@details Also contains some ready to use general union types for unsigned types, signed types, floats,
 *       number and math helper defines, shift behavior helpers, the bit behavior definitions, etc.
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DEFINES_H
   #define DEFINES_H

#include "Device_Config.h"
#include "Etn_Types.h"
#include "Bit.hpp"

namespace BF_Lib
{

/*
 *****************************************************************************************
 *		Public Typedef's
 *****************************************************************************************
 */

/// Processor optimized typedefs.
	#if PROCESSOR_REGISTER_BASE_SIZE == 4
typedef uint32_t MBASE;
typedef int32_t MSBASE;
typedef uint32_t MBOOL;
typedef uint32_t MUINT8;
typedef int32_t MSINT8;
typedef uint32_t MUINT16;
typedef int32_t MSINT16;
typedef uint32_t MUINT32;
typedef int32_t MSINT32;
	#elif PROCESSOR_REGISTER_BASE_SIZE == 2
typedef uint16_t MBASE;
typedef int16_t MSBASE;
typedef uint16_t MBOOL;
typedef uint16_t MUINT8;
typedef int16_t MSINT8;
typedef uint16_t MUINT16;
typedef int16_t MSINT16;
typedef uint32_t MUINT32;
typedef int32_t MSINT32;
	#elif PROCESSOR_REGISTER_BASE_SIZE == 1
typedef uint8_t MBASE;
typedef int8_t MSBASE;
typedef uint8_t MBOOL;
typedef uint8_t MUINT8;
typedef int8_t MSINT8;
typedef uint16_t MUINT16;
typedef int16_t MSINT16;
typedef uint32_t MUINT32;
typedef int32_t MSINT32;
	#endif

/// This is to make the 32 bit unsigned data masked.
	#define USINT32_BIT_MASK        0xFFFFFFFF

/// This is to make the 16 bit unsigned data masked.
	#define USINT16_BIT_MASK        0xFFFF

/// This is to make the 8 bit unsigned data masked.
	#define USINT8_BIT_MASK         0xFF


/// keyword of C++11 standard
	#ifdef __cplusplus
// #if __cplusplus <= 201402L
		 #define nullptr                    0
// #endif
	#endif


/// This is used for the 32 bit float type data.
typedef float float32_t;

/// This is used for the 64 bit float type data.
typedef double float64_t;

/// This is used for the boolean data type.
typedef bool bool_t;

/*
 *****************************************************************************************
 *	This is the unsigned stuff.
 *****************************************************************************************
 */


/**
 * @brief Split unsigned 16 bit type union shares memory between one unsigned 16 bit unsigned integer or
 * two unsigned 8 bit data types, at a time.
 */
typedef union
{
	uint16_t u16;
	uint8_t u8[2];
} SPLIT_UINT16;

/**
 * @brief Split unsigned 32 bit type union shares memory between one unsigned 32 bit unsigned integer, or two unsigned
 * 16 bit data types, or four eight bit unsigned data types at a time.
 */
typedef union
{
	uint32_t u32;
	uint16_t u16[2];
	uint8_t u8[4];
} SPLIT_UINT32;

/**
 * @brief Split unsigned 64 bit type union shares memory between one unsigned 64 bit unsigned integer, or two unsigned
 * 32 bit data types, or four unsigned 16 bit data types or eight 8 bit unsigned data types at a time.
 */
typedef union
{
	uint64_t u64;
	uint32_t u32[2];
	uint16_t u16[4];
	uint8_t u8[8];
} SPLIT_UINT64;


/**
 * @brief Super data type union shares memory between one float value, or four boolean data types, four unsigned 8 bit
 * integer types,
 * or two 16 bit unsigned data types, or one unsigned 32 bit data type or eight 8 bit unsigned data types and similarly
 * for signed  numbers at a time.
 */
typedef union
{
	float float_dt;
	bool bool_dt[4];
	uint8_t uint8_dt[4];
	uint16_t uint16_dt[2];
	uint32_t uint32_dt;
	int8_t sint8_dt[4];
	int16_t sint16_dt[2];
	int32_t sint32_dt;
} SUPER_DATA_TYPE;


/*
 *****************************************************************************************
 *	This is the signed stuff.
 *****************************************************************************************
 */

/**
 * @brief Split signed 16 bit type union shares memory between one signed 16 bit unsigned integer or
 * two signed 8 bit data types, or one unsigned 16 bit unsigned integer or two unsigned 8 bit data types at a time.
 */
typedef union
{
	int16_t s16;
	int8_t s8[2];
	uint16_t u16;
	uint8_t u8[2];
} SPLIT_SINT16;

/**
 * @brief Split signed 32 bit type union shares memory between one signed 32 bit unsigned integer, or two signed
 * 16 bit data types, or four eight bit signed data types at a time.
 */
typedef union
{
	int32_t s32;
	int16_t s16[2];
	int8_t s8[4];
} SPLIT_SINT32;

/**
 * @brief Split signed 64 bit type union shares memory between one signed 64 bit unsigned integer, or two signed
 * 32 bit data types, or four signed 16 bit data types or eight 8 bit signed data types at a time.
 */
typedef union
{
	int64_t s64;
	int32_t s32[2];
	int16_t s16[4];
	int8_t s8[8];
} SPLIT_SINT64;


/*
 *****************************************************************************************
 *	This is the Float stuff.
 *****************************************************************************************
 */
/**
 * @brief Split float 32 bit type union shares memory between one float 32 bit value, or two unsigned
 * 16 bit data types, or four eight bit unsigned data types at a time.
 */
typedef union
{
	float f32;
	uint16_t u16[2];
	uint8_t u8[4];
} SPLIT_FLOAT32;

/**
 * @brief Split float 64 bit type union shares memory between one 64 bit float, or two unsigned
 * 32 bit data types, or four unsigned 16 bit data types or eight 8 bit unsigned data types at a time.
 */
typedef union
{
	float f64;
	uint32_t u32[2];
	uint16_t u16[4];
	uint8_t u8[8];
} SPLIT_FLOAT64;




/*
 *****************************************************************************************
 *		Bit behavior.
 *****************************************************************************************
 */
	#ifndef Set_Bit

/**
 * @brief The following bit control functions compile to a single instruction assuming that
 * the bit is a constant and not a variable.
 */

		#define Set_Bit( a, b )                ( ( a ) |= ( ( 1U ) << ( b ) ) )		/// bit control compiles to single
																					/// instruction
		#define Set_Bit16( a, b )              ( ( a ) |= ( static_cast<uint16_t>( 1U ) << ( b ) ) )	/// bit control
																										/// compiles to
																										/// single
																										/// instruction
		#define Set_Bit32( a, b )              ( ( a ) |= ( static_cast<uint32_t>( 1U ) << ( b ) ) )	/// bit control
																										/// compiles to
																										/// single
																										/// instruction
		#define Clr_Bit( a, b )                ( ( a ) &= ( ~( 1U << ( b ) ) ) )
		#define Clr_Bit8( a, \
						  b )               ( ( a ) &= \
												  ( ~static_cast<uint8_t>( static_cast<uint8_t>( 1U ) << ( b ) ) ) )
		#define Clr_Bit16( a, \
						   b )              ( ( a ) &= \
												  ( ~static_cast<uint16_t>( static_cast<uint16_t>( 1U ) << ( b ) ) ) )
		#define Clr_Bit32( a, b )              ( ( a ) &= ( ~( static_cast<uint32_t>( 1U ) << ( b ) ) ) )
		#define Test_Bit( a, \
						  b )               ( ( static_cast<uint32_t>( a ) & \
												( static_cast<uint32_t>( 1U ) << ( b ) ) ) != 0U )
		#define Toggle_Bit( a, b )             ( ( a ) ^= ( 1U << ( b ) ) )

/// Copies one bit from c.d to a.b
		#define Copy_Bit( a, b, c, d )   if ( ( c ) & ( 1U << ( d ) ) ){ a |= ( 1U << ( b ) ); \
} else { ( a ) &= ( ~( 1U << ( b ) ) ); }

/// Copies the Compliment one bit from c.d to a.b
		#define NCopy_Bit( a, b, c, d )  if ( ( c ) & ( 1U << ( d ) ) ){ ( a ) &= ( ~( 1U << ( b ) ) ); \
} else { a |= ( 1U << ( b ) ); }
		#define Bool_To_Bit( a, b, bool_val )   if ( bool_val == FALSE )    \
	{ Clr_Bit( a, b ); }    \
	else                    \
	{ Set_Bit( a, b ); }

/// Mask functions
		#define Set_Mask( dest, mask, new_data )        dest = ( dest & ~( mask ) ) | ( ( mask ) & ( new_data ) )
		#define Clr_Mask( dest, mask )                  dest &= ~( mask )

/// Array bit functions.  This only works with uint8_t's
		#if 0	// DEPRECATE:  Old C code.  The below should not be used and should be deprecated.  If this
				// functionality is necessary then a special inline should be used.
			#define Bit_Array_Byte_Num( bit )           ( ( bit ) >> 3 )		/// Zero based array byte index.
			#define Set_Array_Bit( data, \
								   bit )      ( ( reinterpret_cast<uint8_t*>( data ) )[( bit ) >> \
																					   3U] |= \
													( 1U << ( ( bit ) & 0x07U ) ) )
			#define Clr_Array_Bit( data, \
								   bit )      ( ( reinterpret_cast<uint8_t*>( data ) )[( bit ) >> \
																					   3U] &= \
													static_cast<uint8_t>( ~static_cast<uint8_t>( 1U << \
																								 ( ( bit ) & \
																								   0x07U ) ) ) )
			#define Toggle_Array_Bit( data, \
									  bit )   ( ( ( uint8_t* )data )[( bit ) >> 3U] ^= ( 1U << ( ( bit ) & 0x07U ) ) )
			#define Test_Array_Bit( data, \
									bit )     ( ( ( reinterpret_cast<uint8_t*>( data ) )[( bit ) >> 3U] & \
												  ( ~static_cast<uint8_t>( 1U << ( ( bit ) & 0x07U ) ) ) ) != 0U )
///< Copies one bit from c.d to a.b
			#define Copy_Array_Bit( a, b, c, d ) if ( Test_Array_Bit( c, d ) )   \
	{ Set_Array_Bit( a, b ); }  \
	else                        \
	{ Clr_Array_Bit( a, b ); }
///< Copies the Compliment one bit from c.d to a.b
			#define NCopy_Array_Bit( a, b, c, d )    if ( Test_Array_Bit( c, d ) )   \
	{ Clr_Array_Bit( a, b ); }  \
	else                        \
	{ Set_Array_Bit( a, b ); }
// Sets a bit based on a bool value pushed in.
			#define BOOL_Array_Bit( a, b, bool_val )    if ( bool_val == FALSE )    \
	{ Clr_Array_Bit( a, b ); }  \
	else                        \
	{ Set_Array_Bit( a, b ); }
		#endif
	#endif

	#ifdef BIT_MASK_SETN_CLRN_TESTN_ENABLE
		#if BIT_MASK_SETN_CLRN_TESTN_ENABLE == USE_8BIT_MASK_FOR_SETN_CLRN_TESTN
extern flash uint8_t bit_to_mask[8];
		#elif BIT_MASK_SETN_CLRN_TESTN_ENABLE == USE_16BIT_MASK_FOR_SETN_CLRN_TESTN
extern flash uint16_t bit_to_mask[16];
		#elif BIT_MASK_SETN_CLRN_TESTN_ENABLE == USE_32BIT_MASK_FOR_SETN_CLRN_TESTN
extern flash uint32_t bit_to_mask[32];
		#endif

		#define Set_BitN( data, bit )   ( ( data ) |= bit_to_mask[( bit )] )
		#define Clr_BitN( data, bit )   ( ( data ) &= ( ~bit_to_mask[( bit )] ) )
		#define Toggle_BitN( data, bit )    ( ( data ) ^= ( bit_to_mask[( bit )] ) )
		#define Test_BitN( data, bit )  ( ( ( data ) & bit_to_mask[( bit )] ) != 0U )
		#define Copy_BitN( dest, dest_b, src, src_b )        \
	if ( Test_BitN( src, src_b ) ){ Set_BitN( dest, dest_b ); } else { Clr_BitN( dest, dest_b ); }
		#define NCopy_BitN( dest, dest_b, src, src_b )       \
	if ( Test_BitN( src, src_b ) ){ Clr_BitN( dest, dest_b ); } else { Set_BitN( dest, dest_b ); }
	#endif



/*
 *****************************************************************************************
 *		Number and math helper defines.
 *****************************************************************************************
 */

/**
 * @brief Works with Numbers and tells if the number passed in is odd or even.
 */
	#define Is_Number_Odd( a )      BF_Lib::Bit::Test( a, 0U )
	#define Is_Number_Even( a )     !BF_Lib::Bit::Test( a, 0U )

	#ifndef NULL
		#ifdef __cplusplus
			#define NULL    0
		#else
			#define NULL    ( ( void* )0 )
		#endif
	#endif

/**
 * @brief Used for boolean type data.
 */
	#define TRUE    true
	#define FALSE   false
// #define TRUE    (1==1)
// #define FALSE   !TRUE

	#define PASS    true


// LTK-4767: TODO: This check should be removed once proper path has been added in ESP32 project
// Enabling this for ESP32 is giving error in ESP32 platform code
#ifndef ESP32_SETUP
	#define FAIL    false
#endif
/// Shift divide defines
	#define DIV_BY_2        1U
	#define DIV_BY_4        2U
	#define DIV_BY_8        3U
	#define DIV_BY_16       4U
	#define DIV_BY_32       5U
	#define DIV_BY_64       6U
	#define DIV_BY_128      7U
	#define DIV_BY_256      8U
	#define DIV_BY_512      9U
	#define DIV_BY_1024     10U

// Provides a quick rounded shift.
// 5>>DIV_BY_8 = 1
// 3>>DIV_BY_8 = 0
// 16>>DIV_BY_8 = 2
// 14>>DIV_BY_8 = 2
	#define Round_Shift_Div( value, shift )     ( ( value + ( 1 << ( shift - 1U ) ) ) >> shift )

// Provides a rouding for division.
	#define Round_Div( dividend, divisor )      ( ( dividend + ( divisor >> DIV_BY_2 ) ) / divisor )

/// Shift multiply defines
	#define MULT_BY_2       1U
	#define MULT_BY_4       2U
	#define MULT_BY_8       3U
	#define MULT_BY_16      4U
	#define MULT_BY_32      5U
	#define MULT_BY_64      6U
	#define MULT_BY_128     7U
	#define MULT_BY_256     8U
	#define MULT_BY_512     9U
	#define MULT_BY_1024    10U

	#define LO_BYTE( w )          ( ( w ) & 0xff )
	#define HI_BYTE( w )          ( ( ( w ) >> 8U ) & 0xff )


typedef void CALLBACK_FUNC( void );

typedef void CLASS_CALLBACK_FUNC( void* passed_ptr );
typedef void* CLASS_CALLBACK_ID;

	#define Null_Explicit_Messaging_Function()          ( ( void* )0 )

/// This is to check and make sure a func pointer is not zero before calling it.
	#define Execute_Callback_Func( func )   if ( func != NULL ){ ( *func )( ); }

/**
 * @brief A template to help alleviate Lint errors around unused variables.
 *
 * @description It is a good practice to indicate that a passed in value is unused.  It
 * is common to indicate this to LINT by either using the variable in some pointless fashion
 * or using a LINT exception.  This template can be used to quickly eliminate this and
 * indicate to the reader the unused nature of the value.
 * <br>
 * Example:
 * void Func( uint8_t unused_param )
 * {
 * 		BF_Lib::Unused<uint8_t>::Verified( unused_param );
 * }
 */
template<typename T>
class Unused
{
	public:

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Okay( T unused_variable )
		{
			/**
			 * @remark Coding Standard Deviation:
			 * @n MISRA-C++[2008] Rule 0-1-11: Symbol 'unused_variable' not referenced
			 * @n PCLint:
			 * @n Justification: In some cases we are not using a variable.  Rather than create
			 * confusing code where we assign the param to itself which could imply an error or
			 * peppering the code with lint exceptions we mark a variable as being unused using this
			 * template which clearly indicates our intention.
			 */
			/*lint -e{715}*/
		}

};

}

#endif
