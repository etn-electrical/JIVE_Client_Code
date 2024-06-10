/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPID.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Identity object
**
*****************************************************************************
*/

#ifndef EIPID_H
#define EIPID_H


#define ID_CLASS						1			/* Identity class identifier */
#define ID_CLASS_REVISION				1			/* Identity class revision */

#define ID_VENDOR_ID					68			/* Scanner vendor ID */
#define ID_PRODUCT_TYPE					12			/* Scanner product type */

#define ID_MAJOR_REVISION				4			/* Scanner major revision */
#define ID_MINOR_REVISION				8			/* Scanner minor revision */
#define ID_SERIAL_NUMBER				0x504d4153	/* Scanner serial number */

#ifdef ET_IP_SCANNER

#define ID_PRODUCT_CODE					14			/* Scanner product code */

#define ID_PRODUCT_NAME					"EtherNetIP Master Stack Library"	/* Scanner product name */
#define ID_PRODUCT_NAME_SIZE			32			/* Scanner product name length including terminating 0 */

#else /* #ifdef ET_IP_SCANNER */

#define ID_PRODUCT_CODE					15			/* Adapter product code */

#define ID_PRODUCT_NAME					"EtherNetIP Adapter"		/* Adapter product name */
#define ID_PRODUCT_NAME_SIZE			19									/* Adapter product name length including terminating 0 */

#endif /*#ifdef ET_IP_SCANNER */

/* Class and instance attribute numbers */
#define ID_CLASS_ATTR_REVISION			1
#define ID_CLASS_ATTR_MAX_INSTANCE		2
#define ID_CLASS_ATTR_NUM_INSTANCES		3
#define ID_CLASS_ATTR_OPT_ATTR_LIST		4
#define ID_CLASS_ATTR_MAX_CLASS_ATTR	6
#define ID_CLASS_ATTR_MAX_INST_ATTR		7

#define ID_ATTR_VENDOR_ID				1
#define ID_ATTR_DEVICE_TYPE				2
#define ID_ATTR_PRODUCT_CODE			3
#define ID_ATTR_REVISION				4
#define ID_ATTR_STATUS					5
#define ID_ATTR_SERIAL_NBR				6
#define ID_ATTR_PRODUCT_NAME			7
#ifdef EIP_ID_USER_ATTRIBUTES
#define ID_ATTR_USER_NAME				15
#define ID_ATTR_USER_DESCRIPTION		16
#define ID_ATTR_USER_LOCATION			17
#endif
#define ID_ATTR_MODBUS_IDENTITY			18
#define ID_ATTR_PROTECTION_MODE			19

/* Number of optional attributes implemented as defined by the CIP spec */
#if defined (EIP_ID_USER_ATTRIBUTES)
#define ID_ATTR_OPTIONAL_COUNT			3
#endif



/*---------------------------------------------------------------------------
** General status bits.
**---------------------------------------------------------------------------
*/
#define ID_STATUS_OWNED					0x0001
#define ID_STATUS_CONFIGURED			0x0004

/*---------------------------------------------------------------------------
** Device state bits.
**---------------------------------------------------------------------------
*/
#define ID_STATUS_SELF_TESTING			0x0000
#define ID_STATUS_NVS_UPDATE			0x0010
#define ID_STATUS_COMM_FAULT			0x0020
#define ID_STATUS_NOT_CONNECTED			0x0030
#define ID_STATUS_NVS_BAD_CONFIG		0x0040
#define ID_STATUS_MAJOR_FAULT			0x0050
#define ID_STATUS_CONNECTED_RUN_MODE	0x0060
#define ID_STATUS_CONNECTED_IDLE_MODE	0x0070


/*---------------------------------------------------------------------------
** Fault bits.
**---------------------------------------------------------------------------
*/
#define ID_STATUS_MINOR_RECOVERABLE_FAULT	0x0100
#define ID_STATUS_MINOR_UNRECOVERABLE_FAULT	0x0200
#define ID_STATUS_MAJOR_RECOVERABLE_FAULT	0x0400
#define ID_STATUS_MAJOR_UNRECOVERABLE_FAULT	0x0800

/*---------------------------------------------------------------------------
** Protection mode bits.
**---------------------------------------------------------------------------
*/
#define ID_PROTECTION_MODE_IMPLICIT			0x01
#define ID_PROTECTION_MODE_EXPLICIT			0x08

#define ID_INSTANCE_ATTRIBUTE_SIZE			14

extern EtIPIdentityInfo gIDInfo[MAX_IDENTITY_INSTANCE];

extern void idInit(void);
extern void idParseClassInstanceRequest( REQUEST* pRequest );
extern BOOL idCheckElectronicKey(EtIPDeviceID* pDeviceID, UINT8* pbGeneralError, UINT16* piExtendedError);

#endif /* #ifndef EIPID_H */
