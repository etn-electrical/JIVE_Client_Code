/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPQOS.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** QoS object 
**
*****************************************************************************
*/

#ifndef EIPQOS_H
#define EIPQOS_H

#ifdef EIP_QOS

#define QOS_CLASS								0x48	/* QoS class identifier */
#define QOS_CLASS_REVISION						1		/* QoS class revision */

/* Class and instance attribute numbers */
#define QOS_CLASS_ATTR_REVISION					1
#define QOS_CLASS_ATTR_MAX_INSTANCE				2
#define QOS_CLASS_ATTR_NUM_INSTANCES			3
#define QOS_CLASS_ATTR_MAX_CLASS_ATTR			6
#define QOS_CLASS_ATTR_MAX_INST_ATTR			7


#define QOS_INST_ATTR_8021Q_TAG_ENABLE			1
#define QOS_INST_ATTR_DSCP_PTP_EVENT			2
#define QOS_INST_ATTR_DSCP_PTP_GENERAL			3
#define QOS_INST_ATTR_DSCP_URGENT				4
#define QOS_INST_ATTR_DSCP_SCHEDULED			5
#define QOS_INST_ATTR_DSCP_HIGH					6
#define QOS_INST_ATTR_DSCP_LOW					7
#define QOS_INST_ATTR_DSCP_EXPLICIT				8

/* Instance attribute structure */
typedef struct tagQOS_INST_ATTR
{
	UINT8 b8021QTagEnable;
	UINT8 bDSCPPTPEvent;
	UINT8 bDSCPPTPGeneral;
	UINT8 bDSCPUrgent;
	UINT8 bDSCPScheduled;
	UINT8 bDSCPHigh;
	UINT8 bDSCPLow;
	UINT8 bDSCPExplicit;
}
QOS_INST_ATTR;

#define QOS_DEFAULT_VALUE_DSCP_PTP_EVENT			59
#define QOS_DEFAULT_VALUE_DSCP_PTP_GENERAL			47
#define QOS_DEFAULT_VALUE_DSCP_URGENT				55
#define QOS_DEFAULT_VALUE_DSCP_SCHEDULED			47
#define QOS_DEFAULT_VALUE_DSCP_HIGH					43
#define QOS_DEFAULT_VALUE_DSCP_LOW					31
#define QOS_DEFAULT_VALUE_DSCP_EXPLICIT				27

#define QOS_8021Q_VALUE_PTP_EVENT					7
#define QOS_8021Q_VALUE_PTP_GENERAL					5
#define QOS_8021Q_VALUE_URGENT						6
#define QOS_8021Q_VALUE_SCHEDULED					5
#define QOS_8021Q_VALUE_HIGH						5
#define QOS_8021Q_VALUE_LOW							3
#define QOS_8021Q_VALUE_EXPLICIT					3



#define QOS_MAX_DSCP_VALUE							63
#define QOS_MASK_ALL_ATTRIBUTES						0x01FE

extern void qosInit(QOS_INST_ATTR* pQOSAttr);
extern void qosParseClassInstanceRqst( REQUEST* pRequest );

#endif /* #ifdef EIP_QOS */

#endif /* #ifndef EIPQOS_H */

