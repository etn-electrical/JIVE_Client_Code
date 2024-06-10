/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef ETHERNETIP_DCI_DEFINES_H
   #define ETHERNETIP_DCI_DEFINES_H


/*
 *****************************************************************************************
 *****************************************************************************************
 *		CIP DCI Access Structures.
 *****************************************************************************************
 *****************************************************************************************
 */

// ***********************************************************
// **** Access Structures
// ***********************************************************
#if CIP_TO_DCI_CLASS_BYTE_SIZE == 1
typedef uint8_t CIP_CLASS_TD;
#else
typedef uint16_t CIP_CLASS_TD;
#endif
#if CIP_TO_DCI_INSTANCE_BYTE_SIZE == 1
typedef uint8_t CIP_INSTANCE_TD;
#else
typedef uint16_t CIP_INSTANCE_TD;
#endif
#if CIP_TO_DCI_ATTRIBUTE_BYTE_SIZE == 1
typedef uint8_t CIP_ATTRIB_TD;
#else
typedef uint16_t CIP_ATTRIB_TD;
#endif


// ***********************************************************
// **** Access Structures
// ***********************************************************
typedef struct CIA_ID_TD		// This is not endian safe. 	NOT_ENDIANSAFE
{
	CIP_CLASS_TD cip_class;
	CIP_INSTANCE_TD cip_instance;
	CIP_ATTRIB_TD cip_attrib;
} CIA_ID_TD;

typedef struct
{
	CIA_ID_TD cia;

	uint8_t* dest_data;
	uint16_t length;		// The length of the data retrieved.
} CIP_GET_STRUCT;

typedef struct
{
	CIA_ID_TD cia;

	uint8_t* src_data;
	uint16_t length;		// This will contain the length of data being sent in.
} CIP_SET_STRUCT;


#endif
