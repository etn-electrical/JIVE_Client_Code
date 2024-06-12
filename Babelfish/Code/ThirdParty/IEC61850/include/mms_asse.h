//*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 2001-2015 */
/*****************************************************************************/
/*                                                                           */
/* This file is the property of:                                             */
/*                                                                           */
/*                       Triangle MicroWorks, Inc.                           */
/*                      Raleigh, North Carolina USA                          */
/*                      <www.TriangleMicroWorks.com>                         */
/*                          (919) 870-6615                                   */
/*                    <support@trianglemicroworks.com>                       */
/*                                                                           */
/* This Source Code and the associated Documentation contain proprietary     */
/* information of Triangle MicroWorks, Inc. and may not be copied or         */
/* distributed in any form without the written permission of Triangle        */
/* MicroWorks, Inc.  Copies of the source code may be made only for backup   */
/* purposes.                                                                 */
/*                                                                           */
/* Your License agreement may limit the installation of this source code to  */
/* specific products.  Before installing this source code on a new           */
/* application, check your license agreement to ensure it allows use on the  */
/* product in question.  Contact Triangle MicroWorks for information about   */
/* extending the number of products that may use this source code library or */
/* obtaining the newest revision.                                            */
/*                                                                           */
/*****************************************************************************/

/*
 *
 *  This file contains
 *  Declarations for ASSERT.
 *
 *  This file should not require modification.
 */

#ifndef _MMS_ASSE_H
#define _MMS_ASSE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef    allowPrintf
    #define allowPrintf    0
    #ifndef NULL
        #define NULL    ((void *) 0)
    #endif
#endif


#if !(allowPrintf)

#ifdef  __IAR_SYSTEMS_ICC
#pragma warnings=off
#endif

#define MMSd_assert( _condition_, _string_ )
#define YELL( _string_ )
#define YELL1( _string_, _parameter1_ )
#define YELL2( _string_, _parameter1_, _parameter2_ )

#ifdef  __IAR_SYSTEMS_ICC
#pragma warnings=default
#endif

#else

#if   terminateAssert
#define MMSd_assert( _condition_, _string_ ) \
        if (!(_condition_)) { \
            printf( "ASSERTION FAILURE at %s: %s \n",   \
                    moduleName_String, _string_ ); \
            abort(); \
            return; \
        }
#else
#define MMSd_assert( _condition_, _string_ ) \
        if (!(_condition_)) { \
            printf( "ASSERTION FAILURE at %s: %s \n",   \
                    moduleName_String, _string_ ); \
            return; \
        }
#endif      /* terminateAssert */


#define YELL( _string_ ) \
    printf( "%s: --==>> %s \n", moduleName_String, _string_ )

#define YELL1( _string_, _parameter1_ ) \
    { printf("%s: ", moduleName_String ); \
      printf( _string_, _parameter1_ );  \
      printf( "\n" ); \
    }

#define YELL2( _string_, _parameter1_, _parameter2_ ) \
    { printf("%s: ", moduleName_String ); \
      printf( _string_, _parameter1_, _parameter2_ ); \
      printf( "\n" ); \
    }

#endif      /* !(allowPrintf) */


#ifdef __cplusplus
};
#endif

#endif /* _MMS_ASSE_H */

