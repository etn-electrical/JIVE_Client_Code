/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"

namespace BF_Lib
{
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

#ifdef BIT_MASK_SETN_CLRN_TESTN_ENABLE

#if BIT_MASK_SETN_CLRN_TESTN_ENABLE == USE_8BIT_MASK_FOR_SETN_CLRN_TESTN
static flash uint8_t bit_to_mask[8] =
{
	0x01U, 0x02U, 0x04U, 0x08U,
	0x10U, 0x20U, 0x40U, 0x80U
};
#elif BIT_MASK_SETN_CLRN_TESTN_ENABLE == USE_16BIT_MASK_FOR_SETN_CLRN_TESTN
static flash uint16_t bit_to_mask[16] =
{
	0x0001U, 0x0002U, 0x0004U, 0x0008U,
	0x0010U, 0x0020U, 0x0040U, 0x0080U,
	0x0100U, 0x0200U, 0x0400U, 0x0800U,
	0x1000U, 0x2000U, 0x4000U, 0x8000U
};
#elif BIT_MASK_SETN_CLRN_TESTN_ENABLE == USE_32BIT_MASK_FOR_SETN_CLRN_TESTN
static flash uint32_t bit_to_mask[32] =
{
	0x00000001U, 0x00000002U, 0x00000004U, 0x00000008U,
	0x00000010U, 0x00000020U, 0x00000040U, 0x00000080U,
	0x00000100U, 0x00000200U, 0x00000400U, 0x00000800U,
	0x00001000U, 0x00002000U, 0x00004000U, 0x00008000U,
	0x00010000U, 0x00020000U, 0x00040000U, 0x00080000U,
	0x00100000U, 0x00200000U, 0x00400000U, 0x00800000U,
	0x01000000U, 0x02000000U, 0x04000000U, 0x08000000U,
	0x10000000U, 0x20000000U, 0x40000000U, 0x80000000U
};
#endif

#endif

}
