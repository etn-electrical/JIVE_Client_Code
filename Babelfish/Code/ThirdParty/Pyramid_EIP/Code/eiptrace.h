/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPTRACE.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Outputting trace dump to the disk file or the debugger window
**
** To use trace functionality there should be two preprocessor definitions
** set: TRACE_OUTPUT enabling general trace support and either
** TRACE_FILE_OUTPUT to send it to the file or TRACE_DEBUGGER_OUTPUT
** to output to the debug window. 
** In case of the file dump the program will output to the debug%n.txt file
** in the et_ip directory on the c: drive (on Windows)
**
*****************************************************************************
*/

#ifndef EIPTRACE_H
#define EIPTRACE_H

#ifdef TRACE_OUTPUT

#define TRACE_BUFFER_SIZE				1024

extern LogMessageCallbackType* gfnLogMessage;

extern UINT8 gTraceFlag;
extern UINT16 gTraceTypeFlag;

extern void InitializeDump(void);			/* Prepare for the debug dump */
extern void FinishDump(void);				/* Cleanup after debug dump */
EXTERN void fDumpStr(UINT8 iSeverity, UINT16 iType, UINT32 lHostIpAddr, UINT32 lRemoteIpAddr, const char* lpszFormat, ...);				/* Variable number of parameters supported */
EXTERN void fDumpBuf(UINT8 iSeverity, UINT16 iType, UINT32 lHostIpAddr, UINT32 lRemoteIpAddr, char* pBuf, int nLen);					/* Output the buffer contents as a string of bytes */
extern void fDumpStrNoNewLine(UINT8 iSeverity, UINT16 iType, UINT32 lHostIpAddr, UINT32 lRemoteIpAddr, const char* lpszFormat, ...);	/* Variable number of parameters supported */

#define DumpStr0(severity, x, y, z, a)    fDumpStr(severity, x, y, z, a)
#define DumpStr1(severity, x, y, z, a, b)    fDumpStr(severity, x, y, z, a, b)
#define DumpStr2(severity, x, y, z, a, b, c)    fDumpStr(severity, x, y, z, a, b, c)
#define DumpStr3(severity, x, y, z, a, b, c, d)    fDumpStr(severity, x, y, z, a, b, c, d)
#define DumpStr4(severity, x, y, z, a, b, c, d, e)    fDumpStr(severity, x, y, z, a, b, c, d, e)
#define DumpStr5(severity, x, y, z, a, b, c, d, e, f)    fDumpStr(severity, x, y, z, a, b, c, d, e, f)
#define DumpStr6(severity, x, y, z, a, b, c, d, e, f, g)    fDumpStr(severity, x, y, z, a, b, c, d, e, f, g)
#define DumpStr7(severity, x, y, z, a, b, c, d, e, f, g, h)    fDumpStr(severity, x, y, z, a, b, c, d, e, f, g, h)
#define DumpStr8(severity, x, y, z, a, b, c, d, e, f, g, h, i)    fDumpStr(severity, x, y, z, a, b, c, d, e, f, g, h, i)
#define DumpStr9(severity, x, y, z, a, b, c, d, e, f, g, h, i, j)    fDumpStr(severity, x, y, z, a, b, c, d, e, f, g, h, i, j)
#define DumpStr10(severity, x, y, z, a, b, c, d, e, f, g, h, i, j, k)    fDumpStr(severity, x, y, z, a, b, c, d, e, f, g, h, i, j, k)
#define DumpStr11(severity, x, y, z, a, b, c, d, e, f, g, h, i, j, k, l)    fDumpStr(severity, x, y, z, a, b, c, d, e, f, g, h, i, j, k, l)
#define DumpBuf(severity, a, b, c, d, e)    fDumpBuf(severity, a, b, c, d, e)
#define DumpStr0NoNewLine(severity, x, y, z, a)    fDumpStrNoNewLine(severity, x, y, z, a)
#define DumpStr1NoNewLine(severity, x, y, z, a, b)    fDumpStrNoNewLine(severity, x, y, z, a, b)
#else /* If TRACE_OUTPUT is not defined replace Dump* statements with nothing */
#define DumpStr0(severity, x, y, z, a)
#define DumpStr1(severity, x, y, z, a, b)
#define DumpStr2(severity, x, y, z, a, b, c)
#define DumpStr3(severity, x, y, z, a, b, c, d)
#define DumpStr4(severity, x, y, z, a, b, c, d, e)
#define DumpStr5(severity, x, y, z, a, b, c, d, e, f)
#define DumpStr6(severity, x, y, z, a, b, c, d, e, f, g)
#define DumpStr7(severity, x, y, z, a, b, c, d, e, f, g, h)
#define DumpStr8(severity, x, y, z, a, b, c, d, e, f, g, h, i)
#define DumpStr9(severity, x, y, z, a, b, c, d, e, f, g, h, i, j)
#define DumpStr10(severity, x, y, z, a, b, c, d, e, f, g, h, i, j, k)
#define DumpStr11(severity, x, y, z, a, b, c, d, e, f, g, h, i, j, k, l)
#define DumpBuf(severity, a, b, c, d, e)
#define DumpStr0NoNewLine(severity, x, y, z, a)
#define DumpStr1NoNewLine(severity, x, y, z, a, b)
#endif

extern char traceGetTypeAsChar(UINT16 iType);

#endif /* #ifndef EIPTRACE_H */
