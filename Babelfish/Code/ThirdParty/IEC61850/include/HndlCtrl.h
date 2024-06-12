/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 2001-2015 */
/*****************************************************************************/
/*                                                                           */
/* This file is the property of:                                             */
/*                                                                           */
/*                       Triangle MicroWorks, Inc.                           */
/*                      Raleigh, North Carolina USA                          */
/*                      <www.TriangleMicroWorks.com>                         */
/*                          (919) 870-6615                                   */
/*                    <support@trianglemicroworks.com>                       */
/*                                                                           */
/* This Source Code and the associated Documentation contain proprietary     */
/* information of Triangle MicroWorks, Inc. and may not be copied or         */
/* distributed in any form without the written permission of Triangle        */
/* MicroWorks, Inc.  Copies of the source code may be made only for backup   */
/* purposes.                                                                 */
/*                                                                           */
/* Your License agreement may limit the installation of this source code to  */
/* specific products.  Before installing this source code on a new           */
/* application, check your license agreement to ensure it allows use on the  */
/* product in question.  Contact Triangle MicroWorks for information about   */
/* extending the number of products that may use this source code library or */
/* obtaining the newest revision.                                            */
/*                                                                           */
/*****************************************************************************/


/** @HndlCtrl.h
 *
 * This file contains declarations for default Handler Functions
 *
 * for 61850 controls
 *
 * These functions must be implemented to interface with the process
 * Examples are provided in examples/common
 *
 */

#ifndef HNDLCTRL_H_
#define HNDLCTRL_H_

/**
 *  The 61850 Select service for 'normal' security uses a read of a name string
 * to do the selection, while the 'enhanced' security uses a write of the SBOw
 * data structure.  This handler implements the 'normal' security.
 **/
TMW_CLIB_API int MMSd_readSBOns(void * N, int size, void * parameter, void * E);

/**
 * NOTE: The write of SBOw is only allowed if using the enhanced security model!
 **/
TMW_CLIB_API void MMSd_writeSboControlCheck(void *N, void *D, int size, void *parameter, void *E);

/**
 *
 **/
TMW_CLIB_API void MMSd_writeOperControlCheck(void *N, void *D, int size, void *parameter, void *E);

/**
 *
 **/
TMW_CLIB_API void MMSd_writeCancelControlTest(void *N, void *D, int size, void *parameter, void *E);


/** The SBO object component requires special handling.  It should
 *  arm a timer for the object to which it is attached, enabling
 *  controls for a specific duration.  It then returns a string
 *  containing the object name.  This sample handler is set up to
 *  have its parameter pointing at the MMSd dictionary entry, which
 *  allows it access to its name as well as all of the other handler
 *  parameters.
 **/
TMW_CLIB_API int MMSd_readSBO(void *N, int size, void *parameter, void *E);




/**
 * These functions are in user code, but prototypes are needed because
 * they are called by the handlers  - examples/common/simulate.c provides examples.
 **/

/**
 *
 **/
TMW_CLIB_API void MMSd_writeSBO(void *N, void *D, int size, void *parameter, void *E);

/**
 *
 */
int MMSd_OperateControl(MMSd_OperControlCheckParams *pOperParams,
                        MMSd_ControlServiceParams *pServiceParams,
                        MMSd_context *pMmsContext);

/**
 * Select point, start sboTimeout, and change stSeld status value
 */
void MMSd_SelectControl(MMSd_SboControlCommon *pSelectCommon,
                        MMSd_context *pMmsContext);

/**
 *
 */
int MMSd_CancelOperateControl(MMSd_CancelControlTestParams *pCancelParams,
                              MMSd_context *pMmsContext);




#endif /* HNDLCTRL_H_ */
