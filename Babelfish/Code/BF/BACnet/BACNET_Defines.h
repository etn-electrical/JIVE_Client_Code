/*
 *****************************************************************************************
 *
 *		Copyright Eaton Corporation. All Rights Reserved.
 *		Creator: Mark A Verheyen
 *
 *		Description:
 *			This controls the network interface.  Non Register associated things.
 *
 *
 *****************************************************************************************
 */
#ifndef BACNET_DEFINES_H
#define BACNET_DEFINES_H

// ***********************************************************
/// **** Modbus Typedefs.
typedef uint16_t BACNET_REG_TD;

// ***********************************************************
/// **** BACNET TX Modes.
typedef enum
{
	BACNET_MSTP_TX_MODE,
	BACNET_IP_TX_MODE
} BACNET_TX_MODES_EN;

// ***********************************************************
/// **** BACnet MS/TP Parity Codes (parity and stop bit combos)
enum
{
	BACNET_PARITY_EVEN_1_STOP_BIT,	// 0
	BACNET_PARITY_ODD_1_STOP_BIT,	// 1
	BACNET_PARITY_NONE_2_STOP_BIT,	// 2
	BACNET_PARITY_EVEN_2_STOP_BIT,	// 3
	BACNET_PARITY_ODD_2_STOP_BIT,	// 4
	BACNET_PARITY_NONE_1_STOP_BIT	// 5
};

// ***********************************************************
/// **** BACNET Parity Modes
enum
{
	BACNET_PARITY_EVEN,	// 0
	BACNET_PARITY_ODD,	// 1
	BACNET_PARITY_NONE	// 2
};

// ***********************************************************
/// **** BACNET Stop Bits
enum
{
	BACNET_1_STOP_BIT,
	BACNET_2_STOP_BIT
};

// ***********************************************************
/// **** Generic Frame Structure this is used by DCI and MSG.
typedef struct
{
	uint8_t* data;
	uint16_t data_length;
} BACNET_FRAME_STRUCT;

#endif
