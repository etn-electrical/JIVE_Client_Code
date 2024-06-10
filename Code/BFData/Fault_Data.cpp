/*
*****************************************************************************************
*       $Workfile:
*
*       $Author:$
*       $Date:$
*       Copyright© 2001-2004, Eaton Corporation. All Rights Reserved.
*
*       $Header:$
*****************************************************************************************
*/
#include "Includes.h"
#include "Fault_Data.h"


/*
*****************************************************************************************
* Struct definition.
*****************************************************************************************
*/
const FAULTS_LIST_ITEM_STRUCT faults_list[FAULTS_LIST_TOTAL_INDS] =
{
    {    //Initialization Fault
        FAULT_INITIALIZATION_ERROR_FLTID,
        false        //The fault is not self clearing - A Latching FAULT
    },
    {    //NV Memory Fault
        FAULT_NV_MEMORY_ERROR_FLTID,
        false        //The fault is not self clearing - A Latching FAULT
    },
    {    //EtherNet/IP Address Retreat
        FAULT_EIP_ACD_ADDRESS_RETREAT_FLTID,
        true         //The fault is self clearing - WARNING
    },
};


