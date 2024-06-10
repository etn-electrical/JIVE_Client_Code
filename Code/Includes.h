/*
 *****************************************************************************************
 *
 *		Copyright Eaton Corporation. All Rights Reserved.
 *		Creator: Mark A Verheyen
 *
 *		Description:
 *
 *
 *
 *****************************************************************************************
 */
#ifndef INCLUDES_H
#define INCLUDES_H

// #include <intrinsics.h>
#include <stddef.h>
#define __STDC_LIMIT_MACROS		// This will force the C++ compiler to use the STDINT limits.
#include <stdint.h>
#include <string.h>

#ifndef BOOTLOADER_BUILD
#include "Defines.h"
#include "Exception.h"
#endif
#include "App_IO_Config.h"

#endif
