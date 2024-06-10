/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Breaker
 *
 * File name        : Errorlog.h
 *
 * Author           : Ahmed El-Gayyar
 *
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 02/06/2023
 *
 ****************************************************************************/

/****************************************************************************
 *                              INCLUDE FILES                               
 ****************************************************************************/





/****************************************************************************
 *                              General definitions                               
 ****************************************************************************/




#ifndef __ERROR_LOG__H
#define __ERROR_LOG__H

#include <stdio.h>
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "driver/timer.h"
#include "driver/periph_ctrl.h"
#include "hal/timer_hal.h"


#include <stdlib.h>

#include <time.h>
#include <sys/time.h>


#include "Breaker.h"





#ifdef _ERROR_LOG_DEBUG_
    #define ERROR_LOG_SIZE                    100
#else
    #define ERROR_LOG_SIZE                    100
#endif

#define MAX_ERROR_DESCRIPTION_SIZE        40

#define STRFTIME_BUF_SIZE                 64

#define STORED_DATA                     0x01
#define SEND_TO_CLOUD_DATA              0x02

#define EPOCH_TIME_Jan_1_2023           1672549200 //0x63B11350
#define INVALID_TIME_STAMP              0xFFFFFFFF

#define LOG_SPI_COMM_FAIL				0x75
#define LOG_I2C_COMM_FAIL				0x76
#define LOG_SPI_COMM_ERR				0x77
#define LOG_I2C_COMM_ERR				0x78

typedef struct
{
    uint8_t    ErrorId;
    char    ErrorDescription[MAX_ERROR_DESCRIPTION_SIZE];
}ErrLogDataInfo_str;



typedef struct
{
    uint8_t    ErrorLog;
    bool       CloudUpdated;
    uint32_t   EpochTime;
#ifdef _ERROR_LOG_DEBUG_
    uint32_t    EventLog;
#endif    
}ErrLog_str;


typedef struct
{
    ErrLog_str ErrLogArray[ERROR_LOG_SIZE];
    uint8_t    Index;  //last error log
    bool       LogIsFull; // to indicate we have ERROR_LOG_SIZE and now we are overwriting the old vlause
#ifdef _ERROR_LOG_DEBUG_
    uint32_t    GlobalEventLog;
#endif    
}ErrorLogStrArray_str;

typedef struct
{
    uint32_t   CounterTimeConnected;
    uint32_t   EpochTimeConnected;
    bool       DoneTimeUpdatingTheLog;
}EpochTimeConnect_str;

void InitiateErrLog(void);
void ErrLog(uint8_t log);

uint32_t GetErrLogTime(void);

void SendErrLogToCloud(uint8_t LogIndex);

void SaveErrLogToNVM(void);

void GetErrorDescription(uint8_t ErrorId, char* ErrorDescription);
void ConvertEpochTimeToReadableFormat( uint32_t EpocTime, char * TimeBuffer,uint8_t BuffSize);
void ErrorPreperationLog(uint8_t index);



#endif
