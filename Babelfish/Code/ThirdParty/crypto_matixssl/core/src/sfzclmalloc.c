/* sfzclmalloc.c
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

#include "implementation_defs.h"
#include "sfzclincludes.h"
#include "spal_memory.h"

void *
sfzcl_realloc(void *ptr, size_t old_size, size_t new_size)
{
    void *new_ptr = NULL;

    /* sfzcl_realloc API requires old_size to ensure caller of the API
       keeps track of the size, which is usually required for correct
       usage of the reallocated memory. However, this implementation of
       sfzcl_realloc does not validate the value. */
    PARAMETER_NOT_USED(old_size);

    if (ptr == NULL)
    {
        return SPAL_Memory_Alloc(new_size);
    }

    if (new_size == 0)
    {
        new_size = 1;
    }

    new_ptr = (void *) SPAL_Memory_ReAlloc(ptr, (size_t) new_size);

    return new_ptr;
}

void *
sfzcl_strdup(const void *p)
{
    const char *str;
    char *cp = NULL;

    if (p)
    {
        str = (const char *) p;
        if ((cp = SPAL_Memory_Alloc(c_strlen(str) + 1)) != NULL)
        {
            c_strcpy(cp, str);
        }
    }
    return (void *) cp;
}

void *
sfzcl_memdup(const void *p, size_t len)
{
    const char *str = (const char *) p;
    char *cp = NULL;

    /* argument validation */
    if (p == NULL)
    {
        return NULL;
    }

    if ((cp = SPAL_Memory_Alloc(len + 1)) != NULL)
    {
        c_memcpy(cp, str, (size_t) len);
        cp[len] = '\0';
    }

    return (void *) cp;
}

/* end of file sfzclmalloc.c */
