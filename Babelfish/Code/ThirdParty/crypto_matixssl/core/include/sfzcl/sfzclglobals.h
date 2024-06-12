/* sfzclglobals.h
 */

/*****************************************************************************
* Copyright (c) 2006-2016 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
*****************************************************************************/

#ifndef SFZCLGLOBALS_H
#define SFZCLGLOBALS_H

/* To use global variables in code you have to do following:

   old code                     new code

 ** Declaration of global variable **

   extern int foobar;           SFZCL_GLOBAL_DECLARE(int, foobar);
 ********#define foobar SFZCL_GLOBAL_USE(foobar)

 ** Definiation of global variable **
   int foobar;                  SFZCL_GLOBAL_DEFINE(int, foobar);

 ** Initialization of global variable (this must be inside the
 ** init function or similar, all global variables are initialized to
 ** zero at the beginning). If SFZCL_GLOBAL_INIT is not called then
 ** first use of variable might print out warning about use of
 ** uninitialized global variable (if warnings are enabled).
 ** Warning might also be printed out if the SFZCL_GLOBAL_INIT is called
 ** multiple times without calls to sfzcl_global_reset or
 ** sfzcl_global_uninit + init.

   int foobar = 1;              ** this is not allowed

   foobar = 1;                  SFZCL_GLOBAL_INIT(foobar,1);

 ** Using the global variable

   foobar = 1;                  foobar = 1; ** i.e no changes
   foobar = foobar++;           foobar = foobar++;

 */

#define SFZCL_GLOBAL_USE(var) sfzcl_global_ ## var
#define SFZCL_GLOBAL_DECLARE(type, var) extern type sfzcl_global_ ## var
#define SFZCL_GLOBAL_DEFINE(type, var) type sfzcl_global_ ## var
#define SFZCL_GLOBAL_INIT(var, value) (sfzcl_global_ ## var) = (value)

#endif                          /* SFZCLGLOBALS_H */
