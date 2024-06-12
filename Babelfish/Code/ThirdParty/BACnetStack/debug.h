/*---------------------------------------------------------------------------
	©2000-2005, PolarSoft® Inc., All Rights Reserved
	module:		IMREF.C
desc:		Debug library used solely in Linux
authors:	David M. Fisher, Jack R. Neyer
----------------------------------------------------------------------------*/ 
####COPYRIGHTEND####*/
#ifndef DEBUG_H
#define DEBUG_H

#include <time.h>
#include <syslog.h>

void debug_init(void);
void debug_printf(int level, char *fmt, ...);
void debug_error_printf(int line, char *file, char *fmt, ...);
void debug_set_level(int level);
int debug_get_level(void);
time_t debug_uptime(void);
void debug_dump_data(int level, unsigned char *dataBuff,
    unsigned int dataSize);
void sys_msg(int priority, char *fmt, ...);

#define error_printf(format, arg...) \
  debug_error_printf(__LINE__, __FILE__, format, ## arg)

#define debug_where(x) debug_print_where(x,__LINE__, __FILE__)

#define msg(format, arg...) debug_printf(5, format, ## arg)
#define debugmsg(format, arg...) debug_printf(9,format, ## arg)
#define errormsg error_printf

#endif
