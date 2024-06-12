/*
 *****************************************************************************************
 *
 *		$Author:$
 *		$Date$
 *		Copyright @Eaton Corporation. All Rights Reserved.
 *		Creator: Alvin Zhao
 *
 *		Description:
 *		$Header:$
 *
 *
 *****************************************************************************************
 */
#ifndef BINARY_SEARCH_H
#define BINARY_SEARCH_H
#include "DCI_Defines.h"
#include "Modbus_DCI.h"

/*****************************************************************************************
* Function Declaration
*****************************************************************************************/
extern DCI_ID_TD Find_DCID_From_Modbus_ID( uint16_t modbus_reg );

extern uint16_t Find_Modbus_ID_From_DCID( DCI_ID_TD dcid );

extern const DCI_MBUS_TO_DCID_LKUP_ST_TD* Find_From_Modbus_ID( uint16_t modbus_reg );

extern const DCI_MBUS_FROM_DCID_LKUP_ST_TD* Find_From_DCID( DCI_ID_TD dci_id );

#endif
