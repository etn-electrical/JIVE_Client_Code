/*
*****************************************************************************************
*       $Workfile:$
*
*       $Author:$
*       $Date:$
*       Copyright © Eaton Corporation. All Rights Reserved.
*       Creator: Mark A Verheyen
*
*       Description:
*
*
*       Code Inspection Date: 
*
*       $Header:$
*****************************************************************************************
*/
#ifndef FAULT_ID_LIST_H
  #define FAULT_ID_LIST_H


/*
*****************************************************************************************
 * Fault Index's to be passed into the fault manager.
*****************************************************************************************
*/
#define FAULT_INITIALIZATION_ERROR_FLT    0U
#define FAULT_NV_MEMORY_ERROR_FLT    1U
#define FAULT_EIP_ACD_ADDRESS_RETREAT_FLT    2U
#define FAULTS_LIST_TOTAL_INDS        (0x0003U)

/*
*****************************************************************************************
 * The following are the FAULT ID's.  They are intended only for outside viewing.
 * Interally we use the _FLT's.
*****************************************************************************************
*/
#define FAULT_INITIALIZATION_ERROR_FLTID    0x0001U
#define FAULT_NV_MEMORY_ERROR_FLTID    0x0002U
#define FAULT_EIP_ACD_ADDRESS_RETREAT_FLTID    0x8003U

/*
*****************************************************************************************
* Faults Struct and Typedefs
*****************************************************************************************
*/

/*
*****************************************************************************************
*       More Constants
*****************************************************************************************
*/
///There is a distinction beteween IND and ID.
///IND is the internal representation.  Internal to the application.  ID is the external user
///fault code.  The ID is the ID presented to the user identifying the fault or warning.
///the BIT is the bitfield which is used to find the faults.
typedef uint8_t       FAULTS_IND_TD;
typedef uint16_t      FAULTS_ID_TD;
typedef uint8_t       FAULTS_BIT_TD;
#define FAULTS_Word_Select( a )         ( (a)>> ( (sizeof(FAULTS_BIT_TD)/2U) + 3U ) )
#define FAULTS_Bit_Mask( a )            ( 1U<<( (a) & ( ( sizeof(FAULTS_BIT_TD) * 8U ) - 1U ) ) )
#define FAULTS_TOTAL_BIT_WORDS          ( 1U ) // FAULTS_Word_Select( FAULTS_LIST_TOTAL_INDS ) is always zero


typedef struct FAULTS_LIST_ITEM_STRUCT
{
    FAULTS_ID_TD    fault_id;
    BOOL            self_clear;
} FAULTS_LIST_ITEM_STRUCT;


/// The fault list extern.
extern const FAULTS_LIST_ITEM_STRUCT faults_list[FAULTS_LIST_TOTAL_INDS];


#endif
