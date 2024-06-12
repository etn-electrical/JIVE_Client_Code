/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include <string.h>

#include "Includes.h"
#include "INT_ASCII_Conversion.h"

#define U64A_GT_EQ_U64B( u32_lo_a, u32_hi_a, u32_lo_b, u32_hi_b ) \
	( ( ( u32_hi_a ) > ( u32_hi_b ) ) \
	  || ( ( u32_hi_a ) == ( u32_hi_b ) && ( u32_lo_a ) >= ( u32_lo_b ) ) )

#define U64A_MINUS_U64B( u32_lo_a, u32_hi_a, u32_lo_b, u32_hi_b ) \
	do \
	{ \
		( u32_hi_a ) = ( ( u32_hi_a ) - ( u32_hi_b ) ); \
		if ( ( u32_lo_a ) < ( u32_lo_b ) ) \
		{ \
			( u32_hi_a )--; \
		} \
		( u32_lo_a ) -= ( u32_lo_b ); \
	} \
	while ( 0 )

#define U64A_PLUS_U64B( u32_lo_a, u32_hi_a, u32_lo_b, u32_hi_b ) \
	do \
	{ \
		( u32_hi_a ) += ( u32_hi_b ); \
		if ( ( ( 0xFFFFFFFFUL ) - ( u32_lo_b ) ) < ( u32_lo_a ) ) \
		{ \
			( u32_hi_a )++; \
		} \
		( u32_lo_a ) += ( u32_lo_b ); \
	} \
	while ( 0 )

#define IS_DIGIT( c )     ( ( ( c ) >= 0x30U ) && ( ( c ) <= 0x39U ) )	/* '0' ..'9' */

const U32_PAIR_TD
INT_ASCII_Conversion::comp_table[sizeof( UINT64_MAX_STRING ) - 1U][9U] =
{
	{
		{ 1UL, 0UL },
		{ 2UL, 0UL },
		{ 3UL, 0UL },
		{ 4UL, 0UL },
		{ 5UL, 0UL },
		{ 6UL, 0UL },
		{ 7UL, 0UL },
		{ 8UL, 0UL },
		{ 9UL, 0UL },
	},
	{
		{ 10UL, 0UL },
		{ 20UL, 0UL },
		{ 30UL, 0UL },
		{ 40UL, 0UL },
		{ 50UL, 0UL },
		{ 60UL, 0UL },
		{ 70UL, 0UL },
		{ 80UL, 0UL },
		{ 90UL, 0UL },
	},
	{
		{ 100UL, 0UL },
		{ 200UL, 0UL },
		{ 300UL, 0UL },
		{ 400UL, 0UL },
		{ 500UL, 0UL },
		{ 600UL, 0UL },
		{ 700UL, 0UL },
		{ 800UL, 0UL },
		{ 900UL, 0UL },
	},
	{
		{ 1000UL, 0UL },
		{ 2000UL, 0UL },
		{ 3000UL, 0UL },
		{ 4000UL, 0UL },
		{ 5000UL, 0UL },
		{ 6000UL, 0UL },
		{ 7000UL, 0UL },
		{ 8000UL, 0UL },
		{ 9000UL, 0UL },
	},
	{
		{ 10000UL, 0UL },
		{ 20000UL, 0UL },
		{ 30000UL, 0UL },
		{ 40000UL, 0UL },
		{ 50000UL, 0UL },
		{ 60000UL, 0UL },
		{ 70000UL, 0UL },
		{ 80000UL, 0UL },
		{ 90000UL, 0UL },
	},
	{
		{ 100000UL, 0UL },
		{ 200000UL, 0UL },
		{ 300000UL, 0UL },
		{ 400000UL, 0UL },
		{ 500000UL, 0UL },
		{ 600000UL, 0UL },
		{ 700000UL, 0UL },
		{ 800000UL, 0UL },
		{ 900000UL, 0UL },
	},
	{
		{ 1000000UL, 0UL },
		{ 2000000UL, 0UL },
		{ 3000000UL, 0UL },
		{ 4000000UL, 0UL },
		{ 5000000UL, 0UL },
		{ 6000000UL, 0UL },
		{ 7000000UL, 0UL },
		{ 8000000UL, 0UL },
		{ 9000000UL, 0UL },
	},
	{
		{ 10000000UL, 0UL },
		{ 20000000UL, 0UL },
		{ 30000000UL, 0UL },
		{ 40000000UL, 0UL },
		{ 50000000UL, 0UL },
		{ 60000000UL, 0UL },
		{ 70000000UL, 0UL },
		{ 80000000UL, 0UL },
		{ 90000000UL, 0UL },
	},
	{
		{ 100000000UL, 0UL },
		{ 200000000UL, 0UL },
		{ 300000000UL, 0UL },
		{ 400000000UL, 0UL },
		{ 500000000UL, 0UL },
		{ 600000000UL, 0UL },
		{ 700000000UL, 0UL },
		{ 800000000UL, 0UL },
		{ 900000000UL, 0UL },
	},
	{
		{ 1000000000UL, 0UL },
		{ 2000000000UL, 0UL },
		{ 3000000000UL, 0UL },
		{ 4000000000UL, 0UL },
		{ 705032704UL, 1UL },
		{ 1705032704UL, 1UL },
		{ 2705032704UL, 1UL },
		{ 3705032704UL, 1UL },
		{ 410065408UL, 2UL },
	},
	{
		{ 1410065408UL, 2UL },
		{ 2820130816UL, 4UL },
		{ 4230196224UL, 6UL },
		{ 1345294336UL, 9UL },
		{ 2755359744UL, 11UL },
		{ 4165425152UL, 13UL },
		{ 1280523264UL, 16UL },
		{ 2690588672UL, 18UL },
		{ 4100654080UL, 20UL },
	},
	{
		{ 1215752192UL, 23UL },
		{ 2431504384UL, 46UL },
		{ 3647256576UL, 69UL },
		{ 568041472UL, 93UL },
		{ 1783793664UL, 116UL },
		{ 2999545856UL, 139UL },
		{ 4215298048UL, 162UL },
		{ 1136082944UL, 186UL },
		{ 2351835136UL, 209UL },
	},
	{
		{ 3567587328UL, 232UL },
		{ 2840207360UL, 465UL },
		{ 2112827392UL, 698UL },
		{ 1385447424UL, 931UL },
		{ 658067456UL, 1164UL },
		{ 4225654784UL, 1396UL },
		{ 3498274816UL, 1629UL },
		{ 2770894848UL, 1862UL },
		{ 2043514880UL, 2095UL },
	},
	{
		{ 1316134912UL, 2328UL },
		{ 2632269824UL, 4656UL },
		{ 3948404736UL, 6984UL },
		{ 969572352UL, 9313UL },
		{ 2285707264UL, 11641UL },
		{ 3601842176UL, 13969UL },
		{ 623009792UL, 16298UL },
		{ 1939144704UL, 18626UL },
		{ 3255279616UL, 20954UL },
	},
	{
		{ 276447232UL, 23283UL },
		{ 552894464UL, 46566UL },
		{ 829341696UL, 69849UL },
		{ 1105788928UL, 93132UL },
		{ 1382236160UL, 116415UL },
		{ 1658683392UL, 139698UL },
		{ 1935130624UL, 162981UL },
		{ 2211577856UL, 186264UL },
		{ 2488025088UL, 209547UL },
	},
	{
		{ 2764472320UL, 232830UL },
		{ 1233977344UL, 465661UL },
		{ 3998449664UL, 698491UL },
		{ 2467954688UL, 931322UL },
		{ 937459712UL, 1164153UL },
		{ 3701932032UL, 1396983UL },
		{ 2171437056UL, 1629814UL },
		{ 640942080UL, 1862645UL },
		{ 3405414400UL, 2095475UL },
	},
	{
		{ 1874919424UL, 2328306UL },
		{ 3749838848UL, 4656612UL },
		{ 1329790976UL, 6984919UL },
		{ 3204710400UL, 9313225UL },
		{ 784662528UL, 11641532UL },
		{ 2659581952UL, 13969838UL },
		{ 239534080UL, 16298145UL },
		{ 2114453504UL, 18626451UL },
		{ 3989372928UL, 20954757UL },
	},
	{
		{ 1569325056UL, 23283064UL },
		{ 3138650112UL, 46566128UL },
		{ 413007872UL, 69849193UL },
		{ 1982332928UL, 93132257UL },
		{ 3551657984UL, 116415321UL },
		{ 826015744UL, 139698386UL },
		{ 2395340800UL, 162981450UL },
		{ 3964665856UL, 186264514UL },
		{ 1239023616UL, 209547579UL },
	},
	{
		{ 2808348672UL, 232830643UL },
		{ 1321730048UL, 465661287UL },
		{ 4130078720UL, 698491930UL },
		{ 2643460096UL, 931322574UL },
		{ 1156841472UL, 1164153218UL },
		{ 3965190144UL, 1396983861UL },
		{ 2478571520UL, 1629814505UL },
		{ 991952896UL, 1862645149UL },
		{ 3800301568UL, 2095475792UL },
	},
	{
		{ 2313682944UL, 2328306436UL },
		{ 0UL, 0UL },
		{ 0UL, 0UL },
		{ 0UL, 0UL },
		{ 0UL, 0UL },
		{ 0UL, 0UL },
		{ 0UL, 0UL },
		{ 0UL, 0UL },
		{ 0UL, 0UL },
	},
};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::uint32_to_str( uint32_t u32, uint8_t* p_max_u32_str )
{
	uint32_t tmp32 = 0U;
	uint8_t num_of_digits = 0U;
	uint8_t i;
	uint8_t tmpChar = 0U;

	memset( p_max_u32_str, 0, sizeof( UINT32_MAX_STRING ) );
	do
	{
		tmp32 = ( u32 ) / ( 10U );
		p_max_u32_str[num_of_digits] =
			static_cast<uint8_t>( ( ( u32 - ( tmp32 * 10U ) ) + 0x30U ) );	/*..'0' */
		num_of_digits++;
		u32 = tmp32;
	} while ( u32 != 0U );

	// reverse
	for ( i = 0U; i < ( num_of_digits / 2U ); i++ )
	{
		tmpChar = p_max_u32_str[i];
		p_max_u32_str[i] = p_max_u32_str[( num_of_digits - 1U ) - i];
		p_max_u32_str[( num_of_digits - 1U ) - i] = tmpChar;
	}

	return ( num_of_digits );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::sint32_to_str( int32_t s32, uint8_t* p_max_u32_str_plus_1 )
{
	uint8_t return_val = 0U;

	// just to make sure the last byte is 0, may not be necessary
	p_max_u32_str_plus_1[sizeof( UINT32_MAX_STRING )] = 0U;

	// extreme case: mininum value for SINT32 don't have SINT32 negative
	if ( ( ( -SINT32_MAX ) == ( s32 + 1 ) ) )
	{
		memcpy( p_max_u32_str_plus_1, SINT32_MIN_STRING, sizeof( SINT32_MIN_STRING ) );
		return_val = static_cast<uint8_t>( sizeof( SINT32_MIN_STRING ) - 1U );
	}
	else
	{

		p_max_u32_str_plus_1[sizeof( UINT32_MAX_STRING )] = 0U;
		if ( s32 < 0 )
		{
			p_max_u32_str_plus_1[0] = '-';
			return_val = static_cast<uint8_t>( sizeof ( '-' )
											   + uint32_to_str( static_cast<uint32_t>( -s32 ),
																p_max_u32_str_plus_1 + sizeof ( '-' ) ) );
		}
		else
		{
			p_max_u32_str_plus_1[sizeof( UINT32_MAX_STRING )] = 0U;
			return_val = uint32_to_str( ( uint32_t )s32, p_max_u32_str_plus_1 );
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::uint64_to_str( uint64_t u64, uint8_t* p_max_u64_str )
{
	uint8_t loc = 0U;
	uint8_t return_val = 0U;
	int8_t i;
	uint8_t most_significant_digit_reached = 0;
	BF_Lib::SPLIT_UINT64 u64_data;

	u64_data.u64 = u64;
	memset( p_max_u64_str, 0, sizeof( UINT64_MAX_STRING ) );
	// 0 needs special handling
	if ( 0 == u64 )
	{
		p_max_u64_str[0] = '0';
		return_val = 1;
	}
	else
	{

		// COMPARISON CHART
		// +------------   >=1?
		// +->N:-------- 0
		// +->Y:-----------------------------   >=5?
		// +->N:----------------  >=3?
		// |  +->N:---------  >=2?
		// |  |  +->N:--  1
		// |  |  +->Y:------  2
		// |  +->Y:---------------    >=4?
		// |     +->N:--------    3
		// |     +->Y:-------------   4
		// +->Y:-------------------------------   >=7?
		// +->N:------------------------   >=6?
		// |  +->N:-----------------   5
		// |  +->Y:---------------------   6
		// +->Y:--------------------------------   >=8?
		// +->N:------------------------    7
		// +->Y:---------------------------------   >=9?
		// +->N:--------------------------   8
		// +->Y:------------------------------   9

		// compare with 1E19
		if ( U64A_GT_EQ_U64B( u64_data.u32[0], u64_data.u32[1], comp_table[19][0].lo, comp_table[19][0].hi ) )
		{
			U64A_MINUS_U64B( u64_data.u32[0], u64_data.u32[1], comp_table[19][0].lo,
							 comp_table[19][0].hi );
			most_significant_digit_reached = 1;
			p_max_u64_str[loc++] = '1';
		}

		for ( i = 18; i >= 0; i-- )
		{
			// first compare with the 1Ei number
			if ( !U64A_GT_EQ_U64B( u64_data.u32[0], u64_data.u32[1], comp_table[i][0].lo, comp_table[i][0].hi ) )
			{
				if ( most_significant_digit_reached )
				{
					p_max_u64_str[loc++] = '0';
				}
			}
			else
			{
				most_significant_digit_reached = 1;
				// compare with 5Ei
				if ( !U64A_GT_EQ_U64B( u64_data.u32[0], u64_data.u32[1], comp_table[i][4].lo, comp_table[i][4].hi ) )
				{
					// 1Ei <= number < 5Ei
					// compare with 3Ei
					if ( !U64A_GT_EQ_U64B( u64_data.u32[0], u64_data.u32[1], comp_table[i][2].lo,
										   comp_table[i][2].hi ) )
					{
						// 1Ei <= number < 3Ei
						// compare with 2Ei
						if ( !U64A_GT_EQ_U64B( u64_data.u32[0], u64_data.u32[1], comp_table[i][1].lo,
											   comp_table[i][1].hi ) )
						{
							// 1Ei <= number < 2Ei, this number is '1'
							U64A_MINUS_U64B( u64_data.u32[0], u64_data.u32[1], comp_table[i][0].lo,
											 comp_table[i][0].hi );
							p_max_u64_str[loc++] = '1';
						}
						else
						{
							// 2Ei <= number < 3Ei, this number is '2'
							U64A_MINUS_U64B( u64_data.u32[0], u64_data.u32[1], comp_table[i][1].lo,
											 comp_table[i][1].hi );
							p_max_u64_str[loc++] = '2';
						}
					}
					else
					{
						// 3Ei <= number < 5Ei
						// compare with 4Ei
						if ( !U64A_GT_EQ_U64B( u64_data.u32[0], u64_data.u32[1], comp_table[i][3].lo,
											   comp_table[i][3].hi ) )
						{
							// 3Ei <= number < 4Ei, this number is '3'
							U64A_MINUS_U64B( u64_data.u32[0], u64_data.u32[1], comp_table[i][2].lo,
											 comp_table[i][2].hi );
							p_max_u64_str[loc++] = '3';
						}
						else
						{
							// 4Ei <= number < 5Ei, this number is '4'
							U64A_MINUS_U64B( u64_data.u32[0], u64_data.u32[1], comp_table[i][3].lo,
											 comp_table[i][3].hi );
							p_max_u64_str[loc++] = '4';
						}
					}
				}
				else
				{
					// 5Ei <= number < 10Ei
					// compare with 7Ei
					if ( !U64A_GT_EQ_U64B( u64_data.u32[0], u64_data.u32[1], comp_table[i][6].lo,
										   comp_table[i][6].hi ) )
					{
						// 5Ei <= number < 7Ei
						// compare with 6Ei
						if ( !U64A_GT_EQ_U64B( u64_data.u32[0], u64_data.u32[1], comp_table[i][5].lo,
											   comp_table[i][5].hi ) )
						{
							// 5Ei <= number < 6Ei, this number is '5'
							U64A_MINUS_U64B( u64_data.u32[0], u64_data.u32[1], comp_table[i][4].lo,
											 comp_table[i][4].hi );
							p_max_u64_str[loc++] = '5';
						}
						else
						{
							// 6Ei <= number < 7Ei, this number is '6'
							U64A_MINUS_U64B( u64_data.u32[0], u64_data.u32[1], comp_table[i][5].lo,
											 comp_table[i][5].hi );
							p_max_u64_str[loc++] = '6';
						}
					}
					else
					{
						// 7Ei <= number < 10Ei
						// compare with 8Ei
						if ( !U64A_GT_EQ_U64B( u64_data.u32[0], u64_data.u32[1], comp_table[i][7].lo,
											   comp_table[i][7].hi ) )
						{
							// 7Ei <= number < 8Ei, this number is '7'
							U64A_MINUS_U64B( u64_data.u32[0], u64_data.u32[1], comp_table[i][6].lo,
											 comp_table[i][6].hi );
							p_max_u64_str[loc++] = '7';
						}
						else
						{
							// 8Ei <= number < 10Ei
							// compare with 9Ei
							if ( !U64A_GT_EQ_U64B( u64_data.u32[0], u64_data.u32[1], comp_table[i][8].lo,
												   comp_table[i][8].hi ) )
							{
								// 8Ei <= number < 9Ei, this number is '8'
								U64A_MINUS_U64B( u64_data.u32[0], u64_data.u32[1],
												 comp_table[i][7].lo,
												 comp_table[i][7].hi );
								p_max_u64_str[loc++] = '8';
							}
							else
							{
								// 9Ei <= number < 10Ei, this number is '9'
								U64A_MINUS_U64B( u64_data.u32[0], u64_data.u32[1],
												 comp_table[i][8].lo,
												 comp_table[i][8].hi );
								p_max_u64_str[loc++] = '9';
							}
						}
					}
				}
			}
		}
		return_val = loc;
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::sint64_to_str( int64_t s64, uint8_t* p_max_u64_str_plus_1 )
{
	uint8_t return_val = 0;

	// just to make sure the last byte is 0, may not be necessary
	p_max_u64_str_plus_1[sizeof( UINT64_MAX_STRING )] = 0;

	if ( s64 < 0 )
	{
		p_max_u64_str_plus_1[0] = '-';
		if ( ( s64 + 1 ) == -( ( int64_t )SINT64_MAX ) )
		{
			return_val = uint64_to_str( SINT64_MAX + 1, p_max_u64_str_plus_1 + 1 ) + 1;
		}
		else
		{
			return_val = uint64_to_str( ( ( uint64_t )( -s64 ) ), p_max_u64_str_plus_1 + 1 ) + 1;
		}
	}
	else
	{
		return_val = uint64_to_str( ( ( uint64_t )( s64 ) ), p_max_u64_str_plus_1 );
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::str_to_uint64( const uint8_t* str, uint32_t first_digit,
											 uint32_t last_digit, uint64_t* pRes, bool_t leading0Allowed )
{
	uint8_t return_val = 0U;
	bool_t loop_break = false;
	BF_Lib::SPLIT_UINT64 temp =
	{ 0 };
	uint_fast32_t i = 0U;
	uint16_t num_of_digits = 0U;

	// sanity checks
	if ( first_digit > last_digit )
	{
		return_val = ERR_PARSING_INPUT_ERROR;
	}
	else
	{
		for ( i = first_digit; i <= last_digit; i++ )
		{
			if ( !IS_DIGIT( str[i] ) )
			{
				return_val = ERR_PARSING_NON_DIGIT;
				loop_break = true;
			}
		}
		if ( false == loop_break )
		{
			if ( leading0Allowed )
			{
				// skipping leading zeros, until a non-0 shows up or it's the last digit
				while ( ( first_digit < last_digit ) && ( '0' == str[first_digit] ) )
				{
					first_digit++;
				}
			}
			else
			{
				if ( ( '0' == str[first_digit] ) && ( first_digit < last_digit ) )
				{
					return_val = ERR_PARSING_LEADING_ZERO;
					loop_break = true;
				}
			}
			if ( false == loop_break )
			{
				if ( first_digit + sizeof( UINT64_MAX_STRING ) - 1 <= last_digit )
				{
					return_val = ERR_PARSING_BEYOND_UINT64;	// more than max # of digits for UINT64
				}
				else
				{
					// check against the max possible uint64 value
					if ( first_digit + sizeof( UINT64_MAX_STRING ) - 2 == last_digit )
					{
						bool_t exit_loop = false;
						for ( i = first_digit;
							  ( ( i <= last_digit ) && ( false == exit_loop ) &&
								( false == loop_break ) ); i++ )
						{
							if ( str[i] > UINT64_MAX_STRING[i - first_digit] )
							{
								return_val = ERR_PARSING_BEYOND_UINT64;
								loop_break = true;
							}
							if ( str[i] < UINT64_MAX_STRING[i - first_digit] )
							{
								exit_loop = true;
							}
						}
					}
					if ( false == loop_break )
					{

						num_of_digits = last_digit - first_digit + 1U;
						temp.u64 = 0U;
						for ( i = first_digit; i <= last_digit; i++ )
						{
							if ( 0x30U != str[i] )	/* 0 */
							{

								// temp += comp_table[num_of_digits-(i-first_digit)-1][str[i]-'0'-1]
								U64A_PLUS_U64B( temp.u32[0], temp.u32[1],
												comp_table[num_of_digits - ( i - first_digit ) - 1][str[i] - '0' - 1].lo,
												comp_table[num_of_digits - ( i - first_digit ) - 1][str[i] - '0' -
																									1].hi );
							}
						}
						*pRes = temp.u64;
						return_val = static_cast<uint8_t>( ERR_PARSING_OK );
					}
				}
			}
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::str_to_sint64( const uint8_t* str, uint32_t first_digit,
											 uint32_t last_digit, int64_t* pRes, bool_t leading0Allowed )
{
	uint64_t temp_u64 = 0U;
	uint8_t return_val;
	uint8_t is_negative = 0U;
	uint8_t err;

	if ( 0x2BU == str[first_digit] )
	{
		first_digit++;
	}
	else if ( 0x2DU == str[first_digit] )
	{
		first_digit++;
		is_negative = 1U;
	}
	else
	{
		// MISRA Suppress
	}
	err = str_to_uint64( str, first_digit, last_digit, &temp_u64, leading0Allowed );
	if ( ( static_cast<uint8_t>( ERR_PARSING_OK ) ) != err )
	{
		if ( ( static_cast<uint8_t>( ERR_PARSING_BEYOND_UINT64 ) ) == err )
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_SINT64 );
		}
		else
		{
			return_val = err;
		}
	}
	else
	{
		if ( ( SINT64_MAX + 1U ) < temp_u64 )
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_SINT64 );
		}
		else
		{
			if ( ( SINT64_MAX + 1U ) == temp_u64 )
			{
				if ( 0U == is_negative )
				{
					return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_SINT64 );
				}
				else
				{
					// is negative if reached here
					*pRes = ( -( static_cast<int64_t>( SINT64_MAX ) ) ) - 1;
					return_val = static_cast<uint8_t>( ERR_PARSING_OK );
				}
			}
			else
			{
				// temp_u64 >= MAX_SINT16
				if ( 0U != is_negative )
				{
					*pRes = -( static_cast<int64_t>( temp_u64 ) );
				}
				else
				{
					*pRes = static_cast<int64_t>( temp_u64 );
				}
				return_val = static_cast<uint8_t>( ERR_PARSING_OK );
			}
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::str_to_uint32( const uint8_t* str, uint32_t first_digit,
											 uint32_t last_digit, uint32_t* pRes, bool_t leading0Allowed )
{
	uint64_t tmpU64 = 0U;
	uint8_t return_val;
	uint8_t err;

	err = str_to_uint64( str, first_digit, last_digit, &tmpU64, leading0Allowed );

	if ( ( static_cast<uint8_t>( ERR_PARSING_OK ) ) != err )
	{
		if ( ( static_cast<uint8_t>( ERR_PARSING_BEYOND_UINT64 ) ) == err )
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_UINT32 );
		}
		else
		{
			return_val = err;
		}
	}
	else
	{
		if ( tmpU64 > ( static_cast<uint64_t>( UINT32_MAX ) ) )
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_UINT32 );
		}
		else
		{
			*pRes = static_cast<uint32_t>( tmpU64 );
			return_val = static_cast<uint8_t>( ERR_PARSING_OK );
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::str_to_sint32( const uint8_t* str, uint32_t first_digit,
											 uint32_t last_digit, int32_t* pRes, bool_t leading0Allowed )
{
	uint8_t return_val;
	int64_t tmpS64 = 0;
	uint8_t err;

	err = str_to_sint64( str, first_digit, last_digit, &tmpS64, leading0Allowed );

	if ( ( static_cast<uint8_t>( ERR_PARSING_OK ) ) != err )
	{
		if ( ( static_cast<uint8_t>( ERR_PARSING_BEYOND_SINT64 ) ) == err )
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_SINT32 );
		}
		else
		{
			return_val = err;
		}
	}
	else
	{

		if ( ( tmpS64 > ( static_cast<int64_t>( SINT32_MAX ) ) ) ||
			 ( tmpS64 < ( ( -( static_cast<int64_t>( SINT32_MAX ) ) ) - 1 ) ) )
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_SINT32 );
		}
		else
		{
			*pRes = static_cast<int32_t>( tmpS64 );
			return_val = static_cast<uint8_t>( ERR_PARSING_OK );
		}

	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::str_to_uint16( const uint8_t* str, uint32_t first_digit,
											 uint32_t last_digit, uint16_t* pRes, bool_t leading0Allowed )
{
	uint8_t return_val = 0U;
	bool_t loop_break = false;
	uint32_t tmpU32 = 0U;
	uint32_t i = 0U;

	// sanity checks
	if ( first_digit > last_digit )
	{
		return_val = static_cast<uint8_t>( ERR_PARSING_INPUT_ERROR );
	}
	else
	{
		for ( i = first_digit; ( ( i <= last_digit ) && ( false == loop_break ) ); i++ )
		{
			if ( !IS_DIGIT( str[i] ) )
			{
				return_val = static_cast<uint8_t>( ERR_PARSING_NON_DIGIT );
				loop_break = true;
			}
		}
		if ( false == loop_break )
		{
			if ( leading0Allowed )
			{
				// skipping leading zeros, until a non-0 shows up or it's the last digit
				while ( ( first_digit < last_digit ) && ( 0x30U == str[first_digit] ) )	/* '0' */
				{
					first_digit++;
				}
			}
			else
			{
				if ( ( 0x30U == str[first_digit] ) && ( first_digit < last_digit ) )/* '0' */
				{
					return_val = static_cast<uint8_t>( ERR_PARSING_LEADING_ZERO );
					loop_break = true;
				}
			}
			if ( false == loop_break )
			{
				if ( ( ( first_digit + sizeof( UINT16_MAX_STRING ) ) - 1U ) <= last_digit )
				{
					return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_UINT16 );	// more than max # of digits for
																					// UINT64
				}
				else
				{
					// check against the max possible uint64 value
					for ( i = first_digit; i <= last_digit; i++ )
					{
						tmpU32 *= 10U;
						tmpU32 += str[i] - 0x30U;	/* '0' */
					}
					if ( tmpU32 > static_cast<uint16_t>( UINT16_MAX ) )
					{
						return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_UINT16 );
					}
					else
					{
						*pRes = static_cast<uint16_t>( tmpU32 );
						return_val = static_cast<uint8_t>( ERR_PARSING_OK );
					}
				}
			}
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::str_to_sint16( const uint8_t* str, uint32_t first_digit,
											 uint32_t last_digit, int16_t* pRes, bool_t leading0Allowed )
{
	uint8_t return_val = 0U;
	uint16_t tmpU16 = 0U;
	uint8_t err;
	uint8_t is_negative = 0U;

	if ( 0x2BU == str[first_digit] )/* '+' */
	{
		first_digit++;
	}
	else if ( 0x2DU == str[first_digit] )	/* '-' */
	{
		first_digit++;
		is_negative = 1U;
	}
	else
	{
		// MISRA Suppress
	}

	err = str_to_uint16( str, first_digit, last_digit, &tmpU16, leading0Allowed );

	if ( ( static_cast<uint8_t>( ERR_PARSING_OK ) ) != err )
	{
		if ( ( static_cast<uint8_t>( ERR_PARSING_BEYOND_UINT16 ) ) == err )
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_SINT16 );
		}
		else
		{
			return_val = err;
		}
	}
	else
	{

		if ( ( SINT16_MAX + 1U ) < tmpU16 )
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_SINT16 );
		}
		else
		{
			if ( ( SINT16_MAX + 1U ) == tmpU16 )
			{
				if ( 0U == is_negative )
				{
					return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_SINT16 );
				}
				else
				{
					// is negative if reached here
					*pRes = ( -( static_cast<int16_t>( SINT16_MAX ) ) ) - 1;
					return_val = static_cast<uint8_t>( ERR_PARSING_OK );
				}
			}
			else
			{
				// temp_u64 >= MAX_SINT16
				if ( 0U != is_negative )
				{
					*pRes = -( static_cast<int16_t>( tmpU16 ) );
				}
				else
				{
					*pRes = static_cast<int16_t>( tmpU16 );
				}
				return_val = static_cast<uint8_t>( ERR_PARSING_OK );
			}
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::str_to_uint8( const uint8_t* str, uint32_t first_digit,
											uint32_t last_digit, uint8_t* pRes, bool_t leading0Allowed )
{
	uint8_t return_val = 0U;
	uint16_t tmpU16 = 0U;
	uint8_t err;

	err = str_to_uint16( str, first_digit, last_digit, &tmpU16, leading0Allowed );

	if ( ( static_cast<uint8_t>( ERR_PARSING_OK ) ) != err )
	{
		if ( ( static_cast<uint8_t>( ERR_PARSING_BEYOND_UINT16 ) ) == err )
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_UINT8 );
		}
		else
		{
			return_val = err;
		}
	}
	else
	{
		if ( tmpU16 > ( static_cast<uint8_t>( UINT8_MAX ) ) )
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_UINT8 );
		}
		else
		{
			*pRes = static_cast<uint8_t>( tmpU16 );
			return_val = static_cast<uint8_t>( ERR_PARSING_OK );
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::str_to_bool( const uint8_t* str, uint32_t first_digit,
										   uint32_t last_digit, uint8_t* pRes, bool_t leading0Allowed )
{
	uint8_t return_val = 0U;
	uint16_t tmpU16 = 0U;
	uint8_t err;

	err = str_to_uint16( str, first_digit, last_digit, &tmpU16, leading0Allowed );

	if ( ( static_cast<uint8_t>( ERR_PARSING_OK ) ) != err )
	{
		if ( ( static_cast<uint8_t>( ERR_PARSING_BEYOND_UINT16 ) ) == err )
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_UINT8 );
		}
		else
		{
			return_val = err;
		}
	}
	else
	{
		if ( tmpU16 > 1U )
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_BOOL );
		}
		else
		{
			*pRes = static_cast<uint8_t>( tmpU16 );
			return_val = static_cast<uint8_t>( ERR_PARSING_OK );
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::str_to_sint8( const uint8_t* str, uint32_t first_digit,
											uint32_t last_digit, int8_t* pRes, bool_t leading0Allowed )
{
	uint8_t return_val;
	int16_t tmpS16 = 0;
	uint8_t err;

	err = str_to_sint16( str, first_digit, last_digit, &tmpS16, leading0Allowed );

	if ( ( static_cast<uint8_t>( ERR_PARSING_OK ) ) != err )
	{
		if ( ( static_cast<uint8_t>( ERR_PARSING_BEYOND_SINT16 ) ) == err )
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_SINT8 );
		}
		else
		{
			return_val = err;
		}
	}
	else
	{

		if ( ( tmpS16 > ( static_cast<int16_t>( SINT8_MAX ) ) ) ||
			 ( tmpS16 < ( -( SINT8_MAX + 1 ) ) ) )
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_SINT8 );
		}
		else
		{
			*pRes = static_cast<int8_t>( tmpS16 );
			return_val = static_cast<uint8_t>( ERR_PARSING_OK );
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::str_hex_to_8Bytes( const uint8_t* str, uint32_t first_digit,
												 uint32_t last_digit, uint64_t* pRes, bool_t hasPrefix0x,
												 bool_t leading0Allowed )
{
	uint8_t return_val = static_cast<uint8_t>( ERR_PARSING_OK );
	bool_t loop_break = false;
	uint64_t tmp8Bytes = 0U;
	uint8_t tmpHexValue = 0U;

	if ( hasPrefix0x )
	{
		if ( ( first_digit + 2U ) > ( last_digit ) )
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_INPUT_ERROR );
			loop_break = true;
		}
		else if ( 0x30U != str[first_digit] )	/* '0' */
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_INPUT_ERROR );
			loop_break = true;
		}
		else if ( ( 0x78U != str[first_digit + 1U] ) && ( 0x58U != str[first_digit + 1U] ) )/* 'x'..'X' */
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_INPUT_ERROR );
			loop_break = true;
		}
		else
		{
			// MISRA Suppress
		}
		if ( false == loop_break )
		{
			first_digit += 2U;
		}
	}
	if ( false == loop_break )
	{
		if ( first_digit > last_digit )
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_INPUT_ERROR );
		}
		else if ( ( !leading0Allowed ) && ( 0x30U == str[first_digit] ) )	/* '0' */
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_LEADING_ZERO );
		}
		else
		{
			// skipping the leading zeros
			while ( ( first_digit < last_digit ) && ( 0x30U == str[first_digit] ) )	/* '0' */
			{
				first_digit++;
			}
			if ( ( first_digit + ( sizeof( uint64_t ) << 1U ) ) < ( last_digit + 1U ) )
			{
				return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_8BYTES );
			}
			else
			{
				for ( uint32_t i = first_digit;
					  ( ( i <= last_digit ) && ( false == loop_break ) );
					  i++ )
				{
					if ( ( 0x30U <= str[i] ) && ( 0x39U >= str[i] ) )	/* 0..9 */
					{
						tmpHexValue = str[i] - 0x30U;
					}
					else if ( ( 0x41U <= str[i] ) && ( 0x46U >= str[i] ) )	/* A...F */
					{
						tmpHexValue = ( str[i] - 0x41U ) + 0x0AU;
					}
					else if ( ( 0x61U <= str[i] ) && ( 0x66U >= str[i] ) )	/* a...f */
					{
						tmpHexValue = ( str[i] - 0x61U ) + 0x0AU;
					}
					else
					{
						return_val = static_cast<uint8_t>( ERR_PARSING_NON_DIGIT );
						loop_break = true;
					}
					if ( false == loop_break )
					{
						tmp8Bytes <<= 4U;
						tmp8Bytes |= tmpHexValue;
					}
				}
				if ( false == loop_break )
				{
					*pRes = tmp8Bytes;
				}
			}
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::str_hex_to_byte( const uint8_t* str, uint32_t first_digit,
											   uint32_t last_digit, uint8_t* pRes, bool_t hasPrefix0x,
											   bool_t leading0Allowed )
{
	uint8_t return_val = 0U;
	uint64_t tmpU64 = 0U;
	uint8_t tmpByte = 0U;
	uint8_t err;

	*pRes = 0U;
	err = str_hex_to_8Bytes( str, first_digit, last_digit, &tmpU64, hasPrefix0x,
							 leading0Allowed );
	if ( ( static_cast<uint8_t>( ERR_PARSING_BEYOND_8BYTES ) ) == err )
	{
		return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_BYTE );
	}
	else
	{
		if ( ( static_cast<uint8_t>( ERR_PARSING_OK ) ) == err )
		{
			if ( ( static_cast<uint64_t>( UINT8_MAX ) ) < tmpU64 )
			{
				return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_BYTE );
			}
			else
			{
				tmpByte = static_cast<uint8_t>( tmpU64 );
				*pRes = tmpByte;
				return_val = static_cast<uint8_t>( ERR_PARSING_OK );
			}
		}
		else
		{
			return_val = err;
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::str_hex_to_bool( const uint8_t* str, uint32_t first_digit,
											   uint32_t last_digit, uint8_t* pRes, bool_t hasPrefix0x,
											   bool_t leading0Allowed )
{
	uint8_t return_val = 0U;
	uint8_t err;
	uint8_t tmpByte = 0U;

	err = str_hex_to_byte( str, first_digit, last_digit, &tmpByte, hasPrefix0x,
						   leading0Allowed );
	if ( ( static_cast<uint8_t>( ERR_PARSING_BEYOND_BYTE ) ) == err )
	{
		return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_BOOL );
	}
	else
	{
		if ( ( static_cast<uint8_t>( ERR_PARSING_OK ) ) == err )
		{
			if ( 1U < tmpByte )
			{
				return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_BOOL );
			}
			else
			{
				*pRes = tmpByte;
				return_val = static_cast<uint8_t>( ERR_PARSING_OK );
			}
		}
		else
		{
			return_val = err;
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::str_hex_to_word( const uint8_t* str, uint32_t first_digit,
											   uint32_t last_digit, uint16_t* pRes, bool_t hasPrefix0x,
											   bool_t leading0Allowed )
{
	uint8_t return_val = 0U;
	uint64_t tmpU64 = 0U;
	uint16_t tmpWord = 0U;
	uint8_t err;

	*pRes = 0U;
	err = str_hex_to_8Bytes( str, first_digit, last_digit, &tmpU64, hasPrefix0x,
							 leading0Allowed );
	if ( ( static_cast<uint8_t>( ERR_PARSING_BEYOND_8BYTES ) ) == err )
	{
		return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_WORD );
	}
	else
	{
		if ( ( static_cast<uint8_t>( ERR_PARSING_OK ) ) == err )
		{
			if ( ( static_cast<uint64_t>( UINT16_MAX ) ) < tmpU64 )
			{
				return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_WORD );
			}
			else
			{
				tmpWord = static_cast<uint16_t>( tmpU64 );
				*pRes = tmpWord;
				return_val = static_cast<uint8_t>( ERR_PARSING_OK );
			}
		}
		else
		{
			return_val = err;
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::str_hex_to_dword( const uint8_t* str, uint32_t first_digit,
												uint32_t last_digit, uint32_t* pRes, bool_t hasPrefix0x,
												bool_t leading0Allowed )
{
	uint8_t return_val = 0U;
	uint64_t tmpU64 = 0U;
	uint32_t tmpDWord = 0U;
	uint8_t err;

	*pRes = 0U;
	err = str_hex_to_8Bytes( str, first_digit, last_digit, &tmpU64, hasPrefix0x,
							 leading0Allowed );
	if ( ( static_cast<uint8_t>( ERR_PARSING_BEYOND_8BYTES ) ) == err )
	{
		return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_DWORD );
	}
	else
	{
		if ( ( static_cast<uint8_t>( ERR_PARSING_OK ) ) == err )
		{
			if ( UINT32_MAX < tmpU64 )
			{
				return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_DWORD );
			}
			else
			{
				tmpDWord = static_cast<uint32_t>( tmpU64 );
				*pRes = tmpDWord;
				return_val = static_cast<uint8_t>( ERR_PARSING_OK );
			}
		}
		else
		{
			return_val = err;
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::str_hex_to_float( const uint8_t* str, uint32_t first_digit,
												uint32_t last_digit, uint32_t* pRes, bool_t hasPrefix0x,
												bool_t leading0Allowed )
{
	uint8_t return_val = 0U;
	uint64_t tmpU64 = 0U;
	uint32_t tmpFloat = 0U;
	uint8_t err;

	*pRes = 0U;
	err = str_hex_to_8Bytes( str, first_digit, last_digit, &tmpU64, hasPrefix0x,
							 leading0Allowed );
	if ( ( static_cast<uint8_t>( ERR_PARSING_BEYOND_8BYTES ) ) == err )
	{
		return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_FLOAT );
	}
	else
	{
		if ( ( static_cast<uint8_t>( ERR_PARSING_OK ) ) == err )
		{
			if ( UINT32_MAX < tmpU64 )
			{
				return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_FLOAT );
			}
			else
			{
				tmpFloat = static_cast<uint32_t>( tmpU64 );
				*pRes = tmpFloat;
				return_val = static_cast<uint8_t>( ERR_PARSING_OK );
			}
		}
		else
		{
			return_val = err;
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::str_hex_to_dfloat( const uint8_t* str, uint32_t first_digit,
												 uint32_t last_digit, uint64_t* pRes, bool_t hasPrefix0x,
												 bool_t leading0Allowed )
{
	uint64_t tmpU64 = 0U;
	uint8_t return_val = 0U;
	uint8_t err;

	*pRes = 0U;
	err = str_hex_to_8Bytes( str, first_digit, last_digit, &tmpU64, hasPrefix0x,
							 leading0Allowed );
	if ( ( static_cast<uint8_t>( ERR_PARSING_BEYOND_8BYTES ) ) == err )
	{
		return_val = static_cast<uint8_t>( ERR_PARSING_BEYOND_DFLOAT );
	}
	else
	{
		if ( ( static_cast<uint8_t>( ERR_PARSING_OK ) ) == err )
		{
			*pRes = tmpU64;
			return_val = static_cast<uint8_t>( ERR_PARSING_OK );
		}
		else
		{
			return_val = err;
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint8_t INT_ASCII_Conversion::str_hex_to_Bytes( const uint8_t* str, uint32_t first_digit,
												uint32_t last_digit, uint8_t* pRes )
{
	uint8_t return_val = static_cast<uint8_t>( ERR_PARSING_OK );
	bool_t loop_break = false;
	uint8_t tmpHexValue = 0U;

	if ( false == loop_break )
	{
		if ( first_digit > last_digit )
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_INPUT_ERROR );
		}
		if ( ( last_digit - first_digit ) & 0x01 )
		{
			for ( uint32_t i = first_digit;
				  ( ( i <= last_digit ) && ( false == loop_break ) );
				  i++ )
			{
				if ( ( 0x30U <= str[i] ) && ( 0x39U >= str[i] ) )	/* 0..9 */
				{
					tmpHexValue |= str[i] - 0x30U;
				}
				else if ( ( 0x41U <= str[i] ) && ( 0x46U >= str[i] ) )	/* A...F */
				{
					tmpHexValue |= ( str[i] - 0x41U ) + 0x0AU;
				}
				else if ( ( 0x61U <= str[i] ) && ( 0x66U >= str[i] ) )	/* a...f */
				{
					tmpHexValue |= ( str[i] - 0x61U ) + 0x0AU;
				}
				else
				{
					return_val = static_cast<uint8_t>( ERR_PARSING_NON_DIGIT );
					loop_break = true;
				}
				if ( false == loop_break )
				{
					if ( i & 0x01 )
					{
						*pRes = tmpHexValue;
						pRes++;
						tmpHexValue = 0U;
					}
					else
					{
						tmpHexValue <<= 4U;
					}
				}
			}
			if ( false == loop_break )
			{
				*pRes = tmpHexValue;
			}
		}
		else
		{
			return_val = static_cast<uint8_t>( ERR_PARSING_INPUT_ERROR );
		}
	}
	return ( return_val );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */

bool INT_ASCII_Conversion::Is_String_Numeric( const uint8_t* source_str, uint32_t length )
{
	uint_fast16_t temp_len = length;
	bool return_status = true;

	while ( ( temp_len > 0U ) && ( *source_str != nullptr ) && return_status )
	{
		temp_len--;
		if ( IS_DIGIT( *source_str ) )
		{
			source_str++;
		}
		else
		{
			return_status = false;
		}
	}
	return ( return_status );
}
