/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPTRACE.C
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
** to output to th edebug window. 
** In case of the file dump the program will output to the debug%n.txt file
** in the et_ip directory (or root directory for WinCE) on the c: drive.
**
*****************************************************************************
*/

#include "eipinc.h"

#ifdef TRACE_OUTPUT

LogMessageCallbackType* gfnLogMessage = NULL;		/* Client application message function */

UINT8 gTraceFlag;
UINT16 gTraceTypeFlag;

#if defined(TRACE_FILE_OUTPUT)

/*---------------------------------------------------------------------------
** InitializeDump( )
**
** Prepare for the dump. If the target is the file, then make sure that the
** output directory exists and determine the file name we are using for output.
** We are writing to the files debug0.txt, debug1.txt, and so on. So if
** the last file created is debug 75.txt we are going to create a new
** debug76.txt and open it with the write privileges.
**---------------------------------------------------------------------------
*/
void InitializeDump(void)
{

	/* Create the file and open it for writing */
	platformInitLogFile();
}

/*---------------------------------------------------------------------------
** FinishDump( )
**
** Cleanup after dump.
**---------------------------------------------------------------------------
*/

void FinishDump(void)
{
	platformCloseLogFile();
}


#endif /* #ifdef TRACE_FILE_OUTPUT */

/*---------------------------------------------------------------------------
** fDumpStr( )
**
** Dumps the string to the file or the debugger window. Accepts variable
** number of arguments. Arguments are passed using printf convention:
** %d in the format string indicates decimal integer argument, %x - hex
** integer, %s - a string, and so on.
**---------------------------------------------------------------------------
*/
void fDumpStr(UINT8 iSeverity, UINT16 iType, UINT32 lHostIpAddr, UINT32 lRemoteIpAddr, const char* lpszFormat, ...)
{
	static char szBuf[TRACE_BUFFER_SIZE];
	va_list args;

	/* Build the output string using printf convention */
	va_start(args, lpszFormat);
	vsprintf(szBuf, lpszFormat, args);
	va_end(args);

	if ((iSeverity & gTraceFlag) && (iType & gTraceTypeFlag))
	{
		if (gfnLogMessage != NULL)
			gfnLogMessage(iSeverity, iType, lHostIpAddr, lRemoteIpAddr, szBuf, (UINT16)(strlen(szBuf)+1));

#if defined(TRACE_FILE_OUTPUT) || defined (TRACE_DEBUGGER_OUTPUT)
		platformWriteLog(iSeverity, iType, lHostIpAddr, lRemoteIpAddr, szBuf, TRUE);
#endif
	}

#ifdef TRACE_SOCKET_OUTPUT
	traceSocketWriteLogString(iSeverity, iType, lHostIpAddr, lRemoteIpAddr, szBuf);
#endif

}

/*---------------------------------------------------------------------------
** fDumpStrNoNewLine( )
**
** Dumps the string to the file or the debugger window. Accepts variable
** number of arguments. Arguments are passed using printf convention:
** %d in the format string indicates decimal integer argument, %x - hex
** integer, %s - a string, and so on.
**---------------------------------------------------------------------------
*/
void fDumpStrNoNewLine(UINT8 iSeverity, UINT16 iType, UINT32 lHostIpAddr, UINT32 lRemoteIpAddr, const char* lpszFormat, ...)
{
	char szBuf[TRACE_BUFFER_SIZE];
	va_list args;

	/* Build the output string using printf convention */
	va_start(args, lpszFormat);
	vsprintf(szBuf, lpszFormat, args);
	va_end(args);

	if ((iSeverity & gTraceFlag) && (iType & gTraceTypeFlag))
	{
		if (gfnLogMessage != NULL)
			gfnLogMessage(iSeverity, iType, lHostIpAddr, lRemoteIpAddr, szBuf, (UINT16)(strlen(szBuf)+1));

#if defined(TRACE_FILE_OUTPUT) || defined (TRACE_DEBUGGER_OUTPUT)
		platformWriteLog(iSeverity, iType, lHostIpAddr, lRemoteIpAddr, szBuf, FALSE);
#endif
	}

#ifdef TRACE_SOCKET_OUTPUT
	traceSocketWriteLogString(iSeverity, iType, lHostIpAddr, lRemoteIpAddr, szBuf);
#endif
}

/*---------------------------------------------------------------------------
** fDumpBuf( )
**
** Dumps the buffer contents to the file or the debugger window.
** Output the buffer as a string of bytes presented in the hexadecimal
** format and separated by a space.
**---------------------------------------------------------------------------
*/
void fDumpBuf(UINT8 iSeverity, UINT16 iType, UINT32 lSrcIp, UINT32 lDstIp, char* pBuf, int nLen)
{
	static char szBuffer[MAX_PACKET_SIZE*3];	/* Each byte takes 2 chars plus spaces in between bytes */
	char szTemp[16];
	INT32 i;
	INT32 nByte;
	char *c = szBuffer;

#ifdef TRACE_SOCKET_OUTPUT
	traceSocketWriteLogData(iSeverity, iType, lSrcIp, lDstIp, pBuf, nLen);
#endif /* TRACE_SOCKET_OUTPUT*/ 

	if ( nLen <= 0 || nLen > MAX_PACKET_SIZE || (!((iSeverity & gTraceFlag) && (iType & gTraceTypeFlag))))
		return;

	for( i = 0; i < nLen; i++ )
	{
		nByte = (INT32)(UINT8)pBuf[i];		/* Take each byte one by one */
		sprintf(szTemp, "%02x ", nByte);	/* Convert it to the Hex string presentation */
		*c++ = szTemp[0];					/* Incorporate in the big string */
		*c++ = szTemp[1];
		*c++ = ' ';
	}

	*c++ = 0;

	if (gfnLogMessage != NULL)
		gfnLogMessage(iSeverity, iType, lSrcIp, lDstIp, szBuffer, (UINT16)(strlen(szBuffer)+1));

#if defined(TRACE_FILE_OUTPUT) || defined (TRACE_DEBUGGER_OUTPUT)
	platformWriteLog(iSeverity, iType, lSrcIp, lDstIp, szBuffer, FALSE);
#endif /* #ifdef TRACE_FILE_OUTPUT || TRACE_DEBUGGER_OUTPUT */
}

char traceGetTypeAsChar(UINT16 iType)
{
	char cType = '?';

	switch(iType)
	{
	case  TRACE_TYPE_TCP_IN:
		cType = '<';
		break;
	case TRACE_TYPE_TCP_OUT:
		cType = '>';
		break;
	case TRACE_TYPE_UDP_IN:
		cType = '+';
		break;
	case TRACE_TYPE_UDP_OUT:
		cType = '-';
		break;
	case TRACE_TYPE_BCAST_IN:
		cType = '[';
		break;
	case TRACE_TYPE_BCAST_OUT:
		cType = ']';
		break;
	case TRACE_TYPE_MSG:
		cType = '*';
		break;
	case TRACE_TYPE_CONNECTION:
		cType = 'C';
		break;
	case TRACE_TYPE_SESSION:
		cType = 'S';
		break;
	case TRACE_TYPE_EXPLICIT:
		cType = 'E';
		break;
	case TRACE_TYPE_SECURITY:
		cType = 'X';
		break;
	}

	return cType;
}

#endif /* #ifdef TRACE_OUTPUT */
