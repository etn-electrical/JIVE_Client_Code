/* sfzutf-stack.c
 *
 * Description: SFZUTF stack size calculation.
 */

/*****************************************************************************
* Copyright (c) 2008-2016 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
*****************************************************************************/

#include "implementation_defs.h"
#include "sfzutf.h"

#ifdef STACK_MEASUREMENT
/* stack measurement globals. */
const void *initial_stack_pointer;
const void *current_stack_pointer;
size_t measured_stack_usage = 0;

#endif

#ifdef STACK_MEASUREMENT
/* Notice: the following code is GCC specific. */

void __cyg_profile_func_enter(void *, void *)
__attribute__ ((no_instrument_function));
void __cyg_profile_func_exit(void *, void *)
__attribute__ ((no_instrument_function));

void
__cyg_profile_func_enter(void *this, void *callsite)
{
    char a;

    /* Dont do anything if stack measurement is not in start mode. */
    if (!STACK_MEASUREMENT_ON)
    {
        return;
    }
    if (&a < (char *) current_stack_pointer)
    {
        current_stack_pointer = &a;
    }
}

void
__cyg_profile_func_exit(void *this, void *callsite)
{
    char a;

    /* Dont do anything if stack measurement is not in start mode. */
    if (!STACK_MEASUREMENT_ON)
    {
        return;
    }
    if (&a < (char *) current_stack_pointer)
    {
        current_stack_pointer = &a;
    }
}
#endif

/* end of file sfzutf-stack.c */
