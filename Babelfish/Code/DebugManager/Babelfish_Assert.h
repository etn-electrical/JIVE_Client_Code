/**
 **********************************************************************************************
 * @file			Assert function decleration
 *
 * @brief			This file contain macro used for assert functionality
 * @details
 * @copyright       2019 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
#ifndef BABELFISH_ASSERT_H
#define BABELFISH_ASSERT_H

#include "Debug_Manager.h"
#include "stdlib.h"

/*
 * Assert functionality
 * To remove the coverity warning such as "Infinite while loop with no exit" and "Derefence after null check" related to
 * use of BF_ASSERT, redefinition of BF_ASSERT is used. The cov-emit command defines the __COVERITY__ preprocessor
 * symbol. Details can be found in Coverity Command Reference documentation.
 */

#ifndef __COVERITY__
#define BF_ASSERT( X ) if ( !( X ) ){ Debug_Manager::Debug_Assert( __LINE__, __func__ );}

#else
/* Coverity Analysis assumes that the killpath is missing if the program does not use the standard assert() function
 * and it reports the subsequent dereference as a FORWARD_NULL. exit(int) is an existing library function
 * which is modeled in coverity to identify killpath, The file <install_dir_sa>/library/generic/common/killpath.c lists
 * these types of functions that are currently modeled in the system. Using existing modeled function here for
 * BF_ASSERT(), hence no need to build the library and no change required in coverity setup.
 * This BF_ASSERT() generates below warning wherever BF_ASSERT(false) is called in a code, this is intentional.
 *  [Warning[Pe111]: statement is unreachable]
 */
#define BF_ASSERT( X ) if ( !( X ) ){ exit( 0U );}
#endif

#endif	// #ifndef BABELFISH_ASSERT_H
