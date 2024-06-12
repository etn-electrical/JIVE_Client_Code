/*******************************************************************************
 *
 * Copyright (c) 2013, 2014, 2015 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *******************************************************************************/
#include "Includes.h"
#include <liblwm2m.h>
#include <stdlib.h>
#include <string.h>
//#include <stdio.h>
//#include <stdarg.h>
//#include <sys/time.h>
#include "Timers_Lib.h"
#include "Ram.h"

#ifndef LWM2M_MEMORY_TRACE

void * lwm2m_malloc(size_t s)
{
	return(Ram::Allocate(s));
}

void lwm2m_free(void * p)
{
	if (p != NULL )
	{
		Ram::De_Allocate(p);
        p = NULL;
	}
}

char * lwm2m_strdup(const char * str)
{
    return strdup(str);
}

#endif

int lwm2m_strncmp(const char * s1,
                     const char * s2,
                     size_t n)
{
    return strncmp(s1, s2, n);
}

time_t lwm2m_gettime(void)
{
	/* Divide by 1000 to go from milliseconds to seconds*/
	time_t sec = BF_Lib::Timers::Get_Time() / (1000);
	return sec;
}

void lwm2m_printf(const char * format, ...)
{
#if 0
	va_list ap;

    va_start(ap, format);

    vfprintf(stderr, format, ap);

    va_end(ap);
#endif
}

char * strndup (const char *s, size_t n)
{
  size_t len = strnlen (s, n);
  char * str= (char *) lwm2m_malloc (len + 1);
  if (str == NULL)
    return NULL;
  str[len] = '\0';
  return (char *) memcpy (str, s, len);
}
