/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPIO.C
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Handles IO packet parsing and formatting
**
*****************************************************************************
*/

#include "eipinc.h"

/*---------------------------------------------------------------------------
** ioParseIOPacket( )
**
** Received I/O data on Class0/1 connection
**---------------------------------------------------------------------------
*/
INT32  ioParseIOPacket( UINT32 lOriginatingAddress, SOCKET lOriginatingSocket, UINT8* pBuf, INT32 lBytesReceived)
{
	CPFHDR Hdr;
	UINT8 *pData, *pDataLocal;
	UINT8* pStartBuf;
	UINT16  iInDataSeqNbr = 0;
	CONNECTION* pConnection;
	BOOL bRunFlag  = TRUE,
		bMatchFound = FALSE,
		bMatchFoundOnce = FALSE,
		bForceNotify = FALSE,
		bNewData = FALSE;
	UINT8 bCOO, bROO;
	INT32 iLen, iLenLocal, iPacketSeqCountDiff, nPacketOffset = 0;
	ASSEMBLY* pAssembly;
	UINT32 lRunIdleHeader;
	UINT32 lTickCount = gdwTickCount;
#ifdef CONNECTION_STATS
	UINT32  lRecvDelay;
#endif
#ifdef EIP_LARGE_CONFIG_OPT
	INT32 iConnectionCount;
#endif

	pData = pBuf;

	while( nPacketOffset < lBytesReceived )
	{
		pStartBuf = pData;

		if (nPacketOffset + CPFHDR_SIZE > lBytesReceived)
		{
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, lOriginatingAddress, 0,
				"ioParseIOPacket doesn't have complete Common Packet Format header");
#ifdef CONNECTION_STATS
			gSystemStats.systemStats.iNumFailedRecvIOMessages++;
#endif
			return MALFORMED_PACKET_ERROR;
		}

		Hdr.iS_count = UINT16_GET(pData);
		pData += sizeof(UINT16);

		Hdr.iAs_type = UINT16_GET(pData);
		pData += sizeof(UINT16);

		Hdr.iAs_length = UINT16_GET(pData);
		pData += sizeof(UINT16);

		Hdr.aiAs_cid = UINT32_GET(pData);
		pData += sizeof(UINT32);

		Hdr.aiAs_seq = UINT32_GET(pData);
		pData += sizeof(UINT32);

		Hdr.iDs_type = UINT16_GET(pData);
		pData += sizeof(UINT16);

		Hdr.iDs_length = UINT16_GET(pData);
		pData += sizeof(UINT16);

		/* Check if header is valid */
		if ( Hdr.iS_count != 2 )
		{
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, lOriginatingAddress, 0, "ioParseIOPacket invalid iS_count");
#ifdef CONNECTION_STATS
			gSystemStats.systemStats.iNumFailedRecvIOMessages++;
#endif
			return MALFORMED_PACKET_ERROR;
		}

		if ( Hdr.iAs_type != CPF_TAG_ADR_SEQUENCED )
		{
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, lOriginatingAddress, 0, "ioParseIOPacket invalid iAs_type");
#ifdef CONNECTION_STATS
			gSystemStats.systemStats.iNumFailedRecvIOMessages++;
#endif
			return MALFORMED_PACKET_ERROR;
		}

		if ( Hdr.iAs_length != 8 )
		{
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, lOriginatingAddress, 0, "ioParseIOPacket invalid iAs_length");
#ifdef CONNECTION_STATS
			gSystemStats.systemStats.iNumFailedRecvIOMessages++;
#endif
			return MALFORMED_PACKET_ERROR;
		}

		if ( Hdr.iDs_type != CPF_TAG_PKT_CONNECTED )
		{
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, lOriginatingAddress, 0, "ioParseIOPacket invalid iDs_type");
#ifdef CONNECTION_STATS
			gSystemStats.systemStats.iNumFailedRecvIOMessages++;
#endif
			return MALFORMED_PACKET_ERROR;
		}

		if (nPacketOffset + (pData-pStartBuf) + Hdr.iDs_length > lBytesReceived)
		{
			DumpStr0(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, lOriginatingAddress, 0, "ioParseIOPacket doesn't have complete packet");
#ifdef CONNECTION_STATS
			gSystemStats.systemStats.iNumFailedRecvIOMessages++;
#endif
			return MALFORMED_PACKET_ERROR;
		}

		bMatchFoundOnce = FALSE;
#ifdef EIP_LARGE_CONFIG_OPT
		iConnectionCount = 0;
#else
		pConnection = gConnections;
#endif

		do
		{
			iLen = Hdr.iDs_length;

#ifdef EIP_LARGE_CONFIG_OPT
			bMatchFound = connListFindEntryInCnxnIdTable(lOriginatingAddress, Hdr.aiAs_cid, lOriginatingSocket, &pConnection, iConnectionCount);
#else
			bMatchFound = FALSE;
			for (; pConnection < gpnConnections; pConnection++ ) 
			{
				if ( lOriginatingAddress == pConnection->cfg.lIPAddress && 
					 Hdr.aiAs_cid == pConnection->lConsumingCID && 
					 connectionIsIOConnection(pConnection)) /* If it's a listen only incoming connection disregard the input */
				{
					bMatchFound = TRUE;
					break;
				}
			}
#endif

			if ((pConnection != NULL) && (pConnection < gpnConnections) &&
				/* With multiple network cards, the data may come in multiple times 
				   This ensures that a packet only gets processed once and that there isn't a 
				   debug error message for each duplicate message */
				(lOriginatingSocket == pConnection->lClass1Socket))
			{

				/* Make sure packets come in sequence */
				if ( pConnection->lInAddrSeqNbr != 0 )
				{
					iPacketSeqCountDiff = (INT32)(Hdr.aiAs_seq-((pConnection->lInAddrSeqNbr+1)));
					if (iPacketSeqCountDiff != 0)
					{
						if (iPacketSeqCountDiff < 0)
						{
							/*"old" packets have come in, ignore them */
							DumpStr1(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress, 
									"old (missed?) packet sequence count %u", Hdr.aiAs_seq);
#ifdef CONNECTION_STATS
							gSystemStats.systemStats.iNumFailedRecvIOMessages++;
#endif
							/* skip out on the rest of the processing and continue through packet data */
#ifdef EIP_LARGE_CONFIG_OPT
							iConnectionCount++;
#else
							pConnection++;
#endif
							continue;
						}
#ifdef CONNECTION_STATS
						pConnection->lLostPackets += iPacketSeqCountDiff;
#endif
					}
				}
				pConnection->lInAddrSeqNbr = Hdr.aiAs_seq;

				if (pConnection->cfg.bTransportClass == Class1)
				{
					/* Grab the data sequence count */
					iInDataSeqNbr = UINT16_GET( pData );


					pDataLocal = pData + DATA_SEQUENCE_COUNT_SIZE;
					iLenLocal = iLen - DATA_SEQUENCE_COUNT_SIZE;
				}
				else
				{
					pDataLocal = pData;
					iLenLocal = iLen;
				}


				if ( pConnection->cfg.bInputRunProgramHeader )
				{
					lRunIdleHeader = UINT32_GET(pDataLocal);
					pDataLocal += sizeof(UINT32);

					bRunFlag = (lRunIdleHeader & IO_RUN_IDLE_MASK) ? TRUE : FALSE;
					bCOO = (UINT8)((lRunIdleHeader & IO_COO_MASK) >> IO_COO_BSHIFT);
					bROO = (UINT8)((lRunIdleHeader & IO_ROO_MASK) >> IO_ROO_BSHIFT);
					iLenLocal -= sizeof(UINT32);

 					if ((!pConnection->cfg.bOriginator) &&
						(pConnection->cfg.bRedundantOwner))
					{
						if (bROO != pConnection->cfg.bReadyOwnershipOutputs)
						{
							notifyEvent(NM_INCOMING_CONNECTION_ROO_VALUE_CHANGED, pConnection->cfg.nInstance, bROO);
							pConnection->cfg.bReadyOwnershipOutputs = (UINT8)bROO;
						}

						if (bCOO != pConnection->cfg.bClaimOutputOwnership)
						{
							if ((bCOO == 1) && (pConnection->cfg.bClaimOutputOwnership == 0))
							{
								/* COO value reset for this connection, it becomes the new master */
								pConnection->cfg.bClaimOutputOwnership = bCOO;
								connectionSetNewRedundantMaster(pConnection);
								/* Timestamp the transition */
								pConnection->lRedundantMasterTimestamp = lTickCount;
							}
							else if ((bCOO == 0) && (pConnection->cfg.bClaimOutputOwnership == 1))
							{
								/* Connection no longer wants to be the master, find a new one */
								pConnection->cfg.bClaimOutputOwnership = bCOO;
								if (pConnection->bIsRedundantMaster)
									connectionFindNewRedundantMaster(pConnection);
							}
						}
						/* Connection was master based on ROO, but now no longer wants to be the master, find a new one */
						else if ((pConnection->bIsRedundantMaster) &&
								(pConnection->cfg.bReadyOwnershipOutputs == 0) &&
								(pConnection->cfg.bClaimOutputOwnership == 0))
						{
							connectionFindNewRedundantMaster(pConnection);
						}

					}

					if (((!pConnection->cfg.bRedundantOwner) ||
						(pConnection->bIsRedundantMaster)) &&
						(bRunFlag != pConnection->bRunIdleFlag))
					{
						notifyEvent(NM_INCOMING_CONNECTION_RUN_IDLE_FLAG_CHANGED, pConnection->cfg.nInstance, bRunFlag );
						pConnection->bRunIdleFlag = (UINT8)bRunFlag;
					}
				}

				/* Check if member data changed and notify about individual members being updated */
				if (!pConnection->cfg.bOriginator)
				{
					if ((!pConnection->cfg.bRedundantOwner) ||
						(pConnection->bIsRedundantMaster))
					{
#ifdef ET_IP_MODBUS
						/* Handle regular CIP connections */
						if ((pConnection->cfg.modbusCfg.iModbusConsumeSize == 0) &&
							(pConnection->cfg.modbusCfg.iModbusProduceSize == 0))
						{
#endif
#if defined ET_IP_SCANNER && !defined (EIP_NO_CCO)
							if (assemblyIsScannerInputInstance(pConnection->cfg.iTargetConsumingConnPoint))
							{

								if (pConnection->cfg.bClass1VariableConsumer)
								{
									/* Variable class1 connection, assign new size */
									if (iLenLocal < 0)
									{
										DumpStr2(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress, 
											"ioParseIOPacket incoming packet error - data length of %d when expected was %d bytes",
											iLenLocal, pConnection->cfg.iInputScannerSize);
#ifdef CONNECTION_STATS
										pConnection->iRcvdPacketError++;
										gSystemStats.systemStats.iNumFailedRecvIOMessages++;
#endif
										return nPacketOffset;
									}

									/* Variable class1 connection, assign new size */
									if (pConnection->cfg.iCurrentConsumingSize != iLenLocal)
									{
										pConnection->cfg.iCurrentConsumingSize = (UINT16)iLenLocal;
										bForceNotify = TRUE;
									}
								}
								else if (iLenLocal != pConnection->cfg.iInputScannerSize)
								{
									DumpStr2(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress, 
										"ioParseIOPacket incoming packet error - data length of %d when expected was %d bytes",
										iLenLocal, pConnection->cfg.iInputScannerSize);
#ifdef CONNECTION_STATS
										pConnection->iRcvdPacketError++;
										gSystemStats.systemStats.iNumFailedRecvIOMessages++;
#endif
									return nPacketOffset;
								}

								if (bRunFlag)
								{
									if (pConnection->cfg.bTransportClass == Class1)
									{
										if ((pConnection->iInDataSeqNbr != iInDataSeqNbr) ||
											(pConnection->bReceivedIOPacket == FALSE))
										{
											pConnection->bReceivedIOPacket = TRUE;
											pConnection->iInDataSeqNbr = iInDataSeqNbr;
											bNewData = TRUE;
										}
									}
									else if (pConnection->cfg.bTransportClass == Class0)
									{
										/* No data sequence count, so data is always considered new */
										bNewData = TRUE;
									}

#ifdef EIP_APP_MEMCMP
									bForceNotify = TRUE;
#endif
									if (bNewData)
									{
										/* Check if the data changed */
										if ((pConnection->cfg.iInputScannerSize > 0) &&
											(iLenLocal > 0) &&
											(!assemblyNtfyScannerInputChange(pConnection, pDataLocal, (UINT16)iLenLocal, bForceNotify)))
										{
											/* If data is the same, but seq count changed - notify as required */
											notifyEvent( NM_CONNECTION_NEW_INPUT_SCANNER_DATA, pConnection->cfg.nInstance, 0 );
										}
									}
								}
							}
							else
#endif
							if (connectionIsDataConnPoint(pConnection->cfg.iTargetConsumingConnPoint))
							{
								pAssembly = assemblyGetByInstance(pConnection->cfg.iTargetConsumingConnPoint);
								if ( pAssembly == NULL )
								{
									DumpStr2(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
										"ioParseIOPacket Internal logic error - no assembly for Instance %d connection Instance %d",
										pConnection->cfg.iTargetConsumingConnPoint, pConnection->cfg.nInstance);
									return nPacketOffset;
								}
								
								if (pConnection->cfg.bClass1VariableConsumer)
								{
									if (iLenLocal < 0)
									{
										DumpStr2(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress,
											"ioParseIOPacket incoming packet error - data length of %d when expected was %d bytes",
											iLenLocal, pAssembly->iSize);
										return nPacketOffset;
									}

									/* Variable class1 connection, assign new size */
									if (pConnection->cfg.iCurrentConsumingSize != iLenLocal)
									{
										pConnection->cfg.iCurrentConsumingSize = (UINT16)iLenLocal;
										bForceNotify = TRUE;
									}
								}
								else if (iLenLocal != pAssembly->iSize)
								{
									DumpStr2(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress, 
										"ioParseIOPacket incoming packet error - data length of %d when expected was %d bytes",
										iLenLocal, pAssembly->iSize);
#ifdef CONNECTION_STATS
									pConnection->iRcvdPacketError++;
									gSystemStats.systemStats.iNumFailedRecvIOMessages++;
#endif
									return nPacketOffset;
								}

								if ( bRunFlag )
								{

									if (pConnection->cfg.bTransportClass == Class1)
									{
										if ((pConnection->iInDataSeqNbr != iInDataSeqNbr) ||
											(pConnection->bReceivedIOPacket == FALSE))
										{
											pConnection->bReceivedIOPacket = TRUE;
											pConnection->iInDataSeqNbr = iInDataSeqNbr;
											bNewData = TRUE;
										}
									}
									else if (pConnection->cfg.bTransportClass == Class0)
									{
										/* No data sequence count, so data is always considered new */
										bNewData = TRUE;
									}

#ifdef EIP_APP_MEMCMP
									bForceNotify = TRUE;
#endif

									if (bNewData)
									{
										/* Check if member or instance data as a whole has been changed and notify if needed */
										if ( assemblyNotifyInstanceChange(pConnection->cfg.iTargetConsumingConnPoint, pDataLocal, (UINT16)iLenLocal, bForceNotify) )
										{
											/* Store data in assembly object */
											assemblyNotifyMembersChange(pConnection->cfg.iTargetConsumingConnPoint, INVALID_MEMBER, pDataLocal, (UINT16)iLenLocal );
											assemblySetInstanceData(pConnection->cfg.iTargetConsumingConnPoint, pDataLocal, (UINT16)iLenLocal );
										}
										else
										{
											/* If data is the same, but seq count changed - notify as required */
											notifyEvent( NM_ASSEMBLY_NEW_INSTANCE_DATA, pConnection->cfg.iTargetConsumingConnPoint, 0 );
										}
									}
								}
							}
#ifdef ET_IP_MODBUS
						}
						else
						{
							if ( eipmbsIsDataConnPoint(pConnection->cfg.iTargetConsumingConnPoint) )
							{
								/* We have a CIP-Modbus I/O packet */
								eipmbsParseIOPacket(pConnection, pDataLocal, (UINT16)iLenLocal);
							}
						}
#endif /* ET_IP_MODBUS */
					}
				}
#ifdef ET_IP_SCANNER
				else
				{
					if (pConnection->cfg.bClass1VariableConsumer)
					{
						/* Variable class1 connection, assign new size */
						if (pConnection->cfg.iCurrentConsumingSize != iLenLocal)
						{
							pConnection->cfg.iCurrentConsumingSize = (UINT16)iLenLocal;
							bForceNotify = TRUE;
						}
					}


					if (pConnection->cfg.bTransportClass == Class1)
					{
						if ((pConnection->iInDataSeqNbr != iInDataSeqNbr) ||
							(pConnection->bReceivedIOPacket == FALSE))
						{
							pConnection->bReceivedIOPacket = TRUE;
							pConnection->iInDataSeqNbr = iInDataSeqNbr;
							bNewData = TRUE;
						}
					}
					else if (pConnection->cfg.bTransportClass == Class0)
					{
						/* No data sequence count, so data is always considered new */
						bNewData = TRUE;
					}

#ifdef EIP_APP_MEMCMP
					bForceNotify = TRUE;
#endif

					if (bNewData)
						assemblyNtfyScannerInputChange(pConnection, pDataLocal, (UINT16)iLenLocal, bForceNotify);
				}
#endif
				/* Reset timeout ticks */
				connectionResetConsumingTicks( pConnection );

#ifdef CONNECTION_STATS
				if ( pConnection->bCollectingStats )
				{
					if (pConnection->lLastRcvdTick )
					{
						if ( lTickCount >= pConnection->lLastRcvdTick )
							lRecvDelay = lTickCount - pConnection->lLastRcvdTick;
						else
							lRecvDelay = (0xffffffff - pConnection->lLastRcvdTick) + lTickCount;

						if ( lRecvDelay >= pConnection->cfg.lConsumingDataRate )
							lRecvDelay -= pConnection->cfg.lConsumingDataRate;
						else
							lRecvDelay = 0;

						if ( lRecvDelay > pConnection->lMaxRcvdDelay )
							pConnection->lMaxRcvdDelay = lRecvDelay;
					}

					pConnection->lLastRcvdTick = lTickCount;

					if ( !pConnection->lStartRcvdTick )
						pConnection->lStartRcvdTick = lTickCount;

					pConnection->lTotalRcvdPackets++;
					pConnection->lRcvdPPSCount++;
					pConnection->lTotalRcvdBytes += iLen;
				}

				gSystemStats.systemStats.iNumSuccessfulRecvIOMessages++;
#endif /* CONNECTION_STATS */
			}

			if ( !bMatchFound )
			{
				if (bMatchFoundOnce == FALSE)
				{
					DumpStr0(TRACE_LEVEL_WARNING, TRACE_TYPE_CONNECTION, lOriginatingAddress, 0,
							 "ioParseIOPacket could not match connection IP address / Consuming Id combination");
#ifdef CONNECTION_STATS
					gSystemStats.systemStats.iNumFailedRecvIOMessages++;
#endif
					continue;
				}
			}
			else
			{
				bMatchFoundOnce = TRUE;
			}

#ifdef EIP_LARGE_CONFIG_OPT
			iConnectionCount++;
#else
			pConnection++;
#endif
		} while (bMatchFound == TRUE);

		pData += iLen;
		nPacketOffset = (INT32)(pData - pStartBuf);

		if ( nPacketOffset == 0 )
			return 0;
	}

	return nPacketOffset;
}

/*---------------------------------------------------------------------------
** ioSendIOPacket( )
**
** Produce on I/O connection
**---------------------------------------------------------------------------
*/
INT32 ioSendIOPacket( CONNECTION* pConnection, CONNECTION* pMulticastProducer )
{
	UINT8*  pData;
	UINT16  iLen, iDataLen;
	ASSEMBLY* pAssembly = NULL;
#ifdef CONNECTION_STATS
	UINT32  lSendDelay;
	UINT32  lTickCount = gdwTickCount;
#endif /* CONNECTION_STATS */

	/* If it's disconnected or this is a listen only incoming connection, do not send anything */
	if ( pConnection->lConnectionState != ConnectionEstablished )
		return ERROR_STATUS;

#ifdef CONNECTION_STATS
	if ( pConnection->bCollectingStats )
	{
		if (pConnection->lStartSendTick != 0 )
		{
			if (pConnection->lLastSendTick )
			{
				if ( lTickCount >= pConnection->lLastSendTick )
					lSendDelay = lTickCount - pConnection->lLastSendTick;
				else
					lSendDelay = (0xffffffff - pConnection->lLastSendTick) + lTickCount;

				if ( lSendDelay >= pConnection->cfg.lProducingDataRate )
					lSendDelay -= pConnection->cfg.lProducingDataRate;
				else
					lSendDelay = 0;

				if ( lSendDelay > pConnection->lMaxSendDelay )
					pConnection->lMaxSendDelay = lSendDelay;
			}
			pConnection->lLastSendTick = lTickCount;
		}

		if ( !pConnection->lStartSendTick )
			pConnection->lStartSendTick = lTickCount;

		pConnection->lTotalSendPackets++;
		pConnection->lSendPPSCount++;
	}
#endif /* CONNECTION_STATS */

	/* Update outgoing sequence count.  This is done for all connections, so connections that 
	   share connection points will keep in sync with the sequence count */
	pConnection->lOutAddrSeqNbr++;

	/* Only produce once for all multicast connections producing for the same connection point */
	if ( pMulticastProducer != NULL )
		return 0;

#ifdef ET_IP_SCANNER
	if ((!pConnection->cfg.bOriginator)
#ifndef EIP_NO_CCO
		&& (!assemblyIsScannerOutputInstance(pConnection->cfg.iTargetProducingConnPoint))
#endif
		)
	{
#endif
		if ( pConnection->cfg.iTargetProducingConnPoint != gHeartbeatConnPoint &&
			 pConnection->cfg.iTargetProducingConnPoint != gListenOnlyConnPoint )
		{
			pAssembly = assemblyGetByInstance(pConnection->cfg.iTargetProducingConnPoint);
			if ( pAssembly == NULL )
			{
				DumpStr2(TRACE_LEVEL_ERROR, TRACE_TYPE_CONNECTION, pConnection->cfg.lHostIPAddr, pConnection->cfg.lIPAddress, 
					"ioSendIOPacket Internal logic error - no assembly for Instance %d connection Instance %d",
					pConnection->cfg.iTargetProducingConnPoint, pConnection->cfg.nInstance );
				return ERROR_STATUS;
			}

			iDataLen = (UINT16)((pConnection->cfg.bClass1VariableProducer) ? pConnection->cfg.iCurrentProducingSize : pAssembly->iSize);
		}
		else
		{
			iDataLen = 0;
		}
#ifdef ET_IP_SCANNER
	}
	else
	{
		iDataLen = (UINT16)((pConnection->cfg.bClass1VariableProducer) ? pConnection->cfg.iCurrentProducingSize : pConnection->cfg.iOutputScannerSize);
	}
#endif

	/* If Run/Program header is used add 4 bytes to the length */
	iLen = (UINT16)(pConnection->cfg.bOutputRunProgramHeader ? (iDataLen + sizeof(UINT32)) : iDataLen);

	pData = ioFormatIOPacketHeader(pConnection, iLen, gmsgBuf);

#ifdef ET_IP_SCANNER
	if (pConnection->cfg.bOriginator)
	{
		if ( iDataLen )
		{
			assemblyGetScannerOutputData(
#ifdef EIP_LARGE_MEMORY
					pConnection->cfg.nOutputScannerTable,
#else
					1,
#endif
					pData, pConnection->cfg.nOutputScannerOffset, iDataLen);
		}
	}
#ifndef EIP_NO_CCO
	else if (assemblyIsScannerOutputInstance(pConnection->cfg.iTargetProducingConnPoint))
	{
		if ( pConnection->cfg.iOutputScannerSize )
			assemblyGetScannerOutputData(pConnection->cfg.iTargetProducingConnPoint, pData, pConnection->cfg.nOutputScannerOffset, iDataLen);
	}
#endif /* EIP_NO_CCO */
	else 
#endif /* ET_IP_SCANNER */
		if ( pConnection->cfg.iTargetProducingConnPoint != gHeartbeatConnPoint &&
			 pConnection->cfg.iTargetProducingConnPoint != gListenOnlyConnPoint )
		{
			/* Get data from assembly object */
			assemblyGetInstanceData(pConnection->cfg.iTargetProducingConnPoint, pData, iDataLen);
		}

	if (pConnection->cfg.bTransportClass == Class1)
		iLen += DATA_SEQUENCE_COUNT_SIZE;

#ifdef CONNECTION_STATS
	if ( pConnection->bCollectingStats )
	{
		pConnection->lTotalSendBytes += iLen;
	}
#endif

	/* Produce data */
	return socketClass1Send( pConnection, (CPFHDR_SIZE + iLen) );
}

/*---------------------------------------------------------------------------
** ioFormatIOPacketHeader( )
**
** Format the header for a class1 data packet
**---------------------------------------------------------------------------
*/
UINT8* ioFormatIOPacketHeader(CONNECTION* pConnection, UINT16 iLen, UINT8* pDataBuffer)
{
	UINT8*  pData = pDataBuffer;
	UINT32 lHeaderValue = 0;

	/* Populate the message header */
	UINT16_SET( pData, 2 ); /* number of objects in the header */
	pData += sizeof(UINT16);

	UINT16_SET( pData, CPF_TAG_ADR_SEQUENCED );
	pData += sizeof(UINT16);

	UINT16_SET( pData, 8 ); /* size of the addr header */
	pData += sizeof(UINT16);

	UINT32_SET( pData, pConnection->lProducingCID );
	pData += sizeof(UINT32);

	UINT32_SET( pData, pConnection->lOutAddrSeqNbr );
	pData += sizeof(UINT32);

	UINT16_SET( pData, CPF_TAG_PKT_CONNECTED );
	pData += sizeof(UINT16);

	if (pConnection->cfg.bTransportClass == Class1)
	{
		UINT16_SET( pData, (iLen + DATA_SEQUENCE_COUNT_SIZE));
		pData += sizeof(UINT16);

		UINT16_SET( pData, pConnection->iOutDataSeqNbr );
		pData += sizeof(UINT16);
	}
	else
	{
		UINT16_SET(pData, iLen);
		pData += sizeof(UINT16);
	}

	if ( pConnection->cfg.bOutputRunProgramHeader )
	{
		lHeaderValue |= (gbRunMode ? 1 : 0);
#ifdef ET_IP_SCANNER
		if ((pConnection->cfg.bOriginator) && 
			(pConnection->cfg.bRedundantOwner))
		{
			lHeaderValue |= ((pConnection->cfg.bClaimOutputOwnership << IO_COO_BSHIFT) & IO_COO_MASK); 
			lHeaderValue |= ((pConnection->cfg.bReadyOwnershipOutputs << IO_ROO_BSHIFT) & IO_ROO_MASK);
		}
#endif

		UINT32_SET( pData, lHeaderValue);
		pData += sizeof(UINT32);
	}

	return pData;
}
