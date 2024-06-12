/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "OS_Binary_Semaphore.h"
#include "OS_Mutex.h"

extern OS_Mutex* AppMtx, ConfirmMtx, mstpMtx, npoolMtx;

void semaCaptureWrap( OS_Mutex* mtx, OS_TICK_TD timeOut );

void semaReleaseWrap( OS_Mutex* mtx );
