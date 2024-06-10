/****************************************************************************
*****************************************************************************
**
** File Name
** ---------
**
** EIPPORT.H
**
** COPYRIGHT (c) 2001-2016 Pyramid Solutions, Inc.
**
*****************************************************************************
*****************************************************************************
**
** Description
** -----------
**
** Port object 
**
*****************************************************************************
*/

#ifndef EIPPORT_H
#define EIPPORT_H

#ifndef EIP_NO_CIP_ROUTING

#define PORT_CLASS						0xF4	/* Port object class identifier */
#define PORT_CLASS_REVISION				2		/* Port object class revision */

#define TCP_IP_PORT_TYPE				4
#define TCP_IP_MODBUS_TCP_PORT_TYPE		201
#define TCP_IP_BACKPLANE_PORT_TYPE		1
#define TCP_IP_STARTING_PORT_NUMBER		2
#define TCP_IP_BACKPLANE_PORT_NUMBER	1
#define MODBUS_SL_PORT_TYPE				202
#define MODBUS_SL_STARTING_PORT_NUMBER	8

#define PORT_PORT_NAME					"Ethernet/IP Port"
#define PORT_BACKPLANE_PORT_NAME		"Backplane"
#define PORT_MODBUS_TCP_PORT_NAME		"Modbus/TCP Port"
#define PORT_MODBUS_SL_PORT_NAME		"ModbusSL Port"


/* Class and instance attribute numbers */
#define PORT_CLASS_ATTR_REVISION				1
#define PORT_CLASS_ATTR_MAX_INSTANCE			2
#define PORT_CLASS_ATTR_NUM_INSTANCES			3
#define PORT_CLASS_ATTR_MAX_CLASS_ATTR			6
#define PORT_CLASS_ATTR_MAX_INST_ATTR			7
#define PORT_CLASS_ATTR_ENTRY_PORT				8
#define PORT_CLASS_ATTR_ALL_PORTS				9

#define PORT_INST_ATTR_PORT_TYPE				1
#define PORT_INST_ATTR_PORT_NUMBER				2
#define PORT_INST_ATTR_PORT_OBJECT				3
#define PORT_INST_ATTR_PORT_NAME				4
#define PORT_INST_ATTR_PORT_NODE_ADDRESS		7
#define PORT_INST_ATTR_PORT_ROUTE_CAPABILITY	10
#define PORT_INST_ATTR_ASSOC_COMMUNICATION_OBJECTS	11

#define PORT_ROUTE_INCOMING_UCMM_SUPPORT		0x0001
#define PORT_ROUTE_OUTGOING_UCMM_SUPPORT		0x0002
#define PORT_ROUTE_INCOMING_IO_SUPPORT			0x0004
#define PORT_ROUTE_OUTGOING_IO_SUPPORT			0x0008
#define PORT_ROUTE_INCOMING_CLASS3_SUPPORT		0x0010
#define PORT_ROUTE_OUTGOING_CLASS3_SUPPORT		0x0020
#define PORT_ROUTE_CIP_SAFETY_SUPPORT			0x0040

extern void portParseClassInstanceRequest( REQUEST* pRequest );

#endif /* #ifndef EIP_NO_CIP_ROUTING */

#endif /* #ifndef EIPPORT_H */

