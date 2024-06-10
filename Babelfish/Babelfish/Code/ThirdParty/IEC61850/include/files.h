//*****************************************************************************/
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

/*
 *
 *  This file contains
 *  Declarations for MMS File Services.
 *
 *  This file should not require modification.
 */

#ifndef _FILES_H
#define _FILES_H

#ifdef __cplusplus
extern "C" {
#endif

/* MMS File Services definitions  ----------------------------------
 --- The following definitions are used to interface with
 --- MMS file functions.
 -----------------------------------------------------------------
 */
#define MMSd_file_other0                    0
#define MMSd_file_filename_ambiguous1       1
#define MMSd_file_file_busy2                2
#define MMSd_file_name_syntax_error3        3
#define MMSd_file_content_invalid4          4
#define MMSd_file_position_invalid5         5
#define MMSd_file_file_access_denied6       6
#define MMSd_file_file_non_existent7        7
#define MMSd_file_duplicate_filename8       8
#define MMSd_file_insufficient_space9       9

int MMSd_fileServiceInit(void);
TMW_CLIB_API int MMSd_fileOpenServiceRequest(MMSd_context *cntxt, unsigned long pos, unsigned long *frsmID, MMSd_FileAttributes *attribs, MMSd_errorFlag *errcode);

TMW_CLIB_API int MMSd_fileReadServiceRequest(MMSd_context *cntxt, unsigned long frsmID, unsigned char *ptr, unsigned int length, int *moreFollows, MMSd_errorFlag *errcode);

TMW_CLIB_API int MMSd_fileCloseServiceRequest(MMSd_context *cntxt, unsigned long frsmID, MMSd_errorFlag *errcode);

TMW_CLIB_API int MMSd_fileDeleteServiceRequest(MMSd_context *cntxt, MMSd_errorFlag *errcode);

TMW_CLIB_API int MMSd_fileDirectoryServiceRequest(MMSd_context *cntxt, MMSd_errorFlag *errcode);

void MMSd_fileReadServiceResponse(MMSd_context *cntxt, MMSd_descriptor *fileData, MMSd_descriptor *moreFollows);

void MMSd_fileOpenServiceResponse(MMSd_context *cntxt, MMSd_descriptor *frsmID);

void MMSd_fileCloseServiceResponse(MMSd_context *cntxt);

void MMSd_fileDeleteServiceResponse(MMSd_context *cntxt);

void MMSd_fileDirectoryServiceResponse(MMSd_context *cntxt, MMSd_descriptor *moreFollows);

TMW_CLIB_API void MMSd_obtainFileServiceRequest(MMSd_context *cntxt, int sourceIndex, int destIndex, int *errcode);

void MMSd_obtainFileServiceResponse(MMSd_context *cntxt);

int MMSd_haveActiveObtainFileServices(MMSd_Connection *pMmsConnection);

void MMSd_initializeConnectionFiles(MMSd_Connection *pMmsConnection);

void MMSd_cleanupConnectionFiles(MMSd_Connection *pMmsConnection);

#if defined(OBTAIN_FILE_REMOVED)
#define MMSD_MAX_OBTAIN_FILE_NAME               1024
#define MMSD_MAX_OBTAIN_FILE_TRANSFERS            10
#define MMSD_OBTAIN_FILE_OPEN_BUFFER_SIZE       1024
#define MMSD_OBTAIN_FILE_READ_BUFFER_SIZE       1024
#define MMSD_OBTAIN_FILE_CLOSE_BUFFER_SIZE      1024

typedef enum {MMSd_ObtainFile_Idle,
              MMSd_ObtainFile_OpenRequest,
              MMSd_ObtainFile_OpenRequestPending,
              MMSd_ObtainFile_ReadRequest,
              MMSd_ObtainFile_ReadRequestPending,
              MMSd_ObtainFile_CloseRequest,
              MMSd_ObtainFile_CloseRequestPending,
              MMSd_ObtainFile_CloseComplete,
              MMSd_ObtainFile_SendError,
              MMSd_ObtainFile_SendConfirmedError
             } MMSd_ObtainFileState;

typedef enum {MMSd_obtainFileNoError,
              MMSd_obtainFileSourceError,
              MMSd_obtainFileDestError,
              MMSd_obtainFileConnectionError,
              MMSd_obtainFileStatusError,
              MMSd_obtainFileEncodeError,
              MMSd_obtainFileDecodeError,
              MMSd_obtainFileFlowControlled,
              MMSd_obtainFileSendError
             } MMSd_ObtainFileError;

typedef struct MMSd_ObtainFileStatus {
    char sourceFileName[ MMSD_MAX_OBTAIN_FILE_NAME + 1 ];
    char destFileName[ MMSD_MAX_OBTAIN_FILE_NAME + 1 ];
    FILE *pDestFile;
    MMSd_ObtainFileState nState;
    unsigned long originalInvokeId;
    unsigned long invokeId;
    long remoteFd;
    MMSd_time timeStamp;
    int obtainFileError;
    int obtainFileAdditionalInfo;
} MMSd_ObtainFileStatus;
#endif

int MMSd_obtainFileOpenFileResponse(MMSd_context *pMmsContext, MMSd_descriptor *frsmID);

int MMSd_obtainFileReadFileResponse(MMSd_context *pMmsContext, MMSd_descriptor *fileData, MMSd_descriptor *moreFollows);

int MMSd_obtainFileCloseFileResponse(MMSd_context *pMmsContext);

int MMSd_obtainFileConfirmedErrorRspService(MMSd_context *_CNTX);

TMW_CLIB_API void MMSd_obtainFileServiceStart(MMSd_Connection *pMmsConnection, unsigned long originaInvokeId, char *sourceFileName, char *destName);

void MMSd_obtainFileService(MMSd_Connection *pMmsConnection);

#ifdef __cplusplus
}
;
#endif

#endif          /* _FILES_H */
