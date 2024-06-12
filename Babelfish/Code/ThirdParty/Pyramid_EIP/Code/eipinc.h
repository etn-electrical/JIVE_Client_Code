/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPINC.h
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Master include file used by all files in the EtherNet/IP library
**
*****************************************************************************
*/
//#include PLATFORM_HEADER
#include "EIP_if_C_Connector.h"

#ifndef EXTERN
	#ifdef __cplusplus
		#define EXTERN extern "C"
	#else
		#define EXTERN extern
	#endif
#endif

#include "EtIpApi.h"

#include "eippdu.h"
#include "eipclien.h"
#include "eiputil.h"
#include "eiprqst.h"
#include "eiprqgrp.h"
#ifdef ET_IP_SCANNER
#include "Scanner/eipscnr.h"
#endif
#include "eipcnct.h"
#include "eipcnlst.h"
#include "eipsessn.h"
#include "eiproutr.h"
#include "eipetlnk.h"
#ifdef EIP_QOS
#include "eipqos.h"
#endif
#include "eiptcpip.h"
#include "eipucmm.h"
#include "eipsckt.h"
#include "eipio.h"
#ifdef ET_IP_SCANNER
#include "Scanner/eipscmgr.h"
#endif
#include "eipcnmgr.h"
#include "eipasm.h"
#ifdef ET_IP_SCANNER
#include "Scanner/eipcfg.h"
#endif
#include "eipnotfy.h"
#include "eiptrace.h"
#include "eipdbg.h"
#include "eipid.h"
#include "eipport.h"
#include "eippccc.h"

#if defined EIP_FILE_OBJECT
#include "eipfile.h"
#endif

#ifdef ET_IP_MODBUS
#include "Modbus/mbsinc.h"
#include "Modbus/eipmbus.h"
#include "Modbus/eipmbscm.h"
#endif

#ifdef EIP_CIPSAFETY
#include "Safety/eipssup.h"
#include "Safety/eipsval.h"
#endif
