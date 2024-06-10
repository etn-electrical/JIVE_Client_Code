/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Breaker
 *
 * File name        : ErrorLog.c
 *
 * Author           : Ahmed El-Gayyar
 *
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 02/06/2023
 *
 ****************************************************************************/

#include "freertos/FreeRTOS.h"
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp_int_wdt.h"



#include "Breaker.h"
#include "Flash.h"
#include "ErrorLog.h"

#ifdef _ERROR_LOG_DEBUG_
    //#define _DEBUG_TIMER_GENERATING_LOG_
#endif    

extern DRAM_ATTR Device_Info_str DeviceInfo;
ErrorLogStrArray_str ErrLogStr;

EpochTimeConnect_str EpochTimeConnectStr;

static uint32_t ErrorLogOneSecondTimerCounter = 0;
static bool        JustPoweredUp = true;
static bool        SendLogToTheCloud = true;  //This is used when we Just Powered up and we need to send the savedlog to the cloud
static bool     PreviousWiFiDisConnectionState = true;  // make it opposite of DeviceInfo.WiFiDisconnected init state

char ErrorReportBuffer [(MAX_ERROR_DESCRIPTION_SIZE*2)+ STRFTIME_BUF_SIZE];

#ifdef  _ERROR_LOG_DEBUG_
#define MAX_ERROR_LOGS        59  //change if any new err log added into ErrLogDataInfo
#else
#define MAX_ERROR_LOGS        54  //change if any new err log added into ErrLogDataInfo
#endif
const ErrLogDataInfo_str ErrLogDataInfo[] = {   // protection trip codes
												//TODO: in PR review, need to remove from below unwanted codes
												// Please don't forget to change MAX_ERROR_LOGS when you add more error logs
                                                 { 0x00 , "NO_TRIP"},
												 { 0x01 , "NORMAL_STARTUP"},
                                                 { 0x02 , "SELF_TEST_SUCCESSFUL_TRIP"},
                                                 { 0x03 , "OUTPUT_STATUS_LOG_VIA_SPI"},
                                                 { 0x04 , "END_OF_FACTORY_ANCHOR"},
                                                 { 0x05 , "RECALL_RESET_ANCHOR"},
												 { 0x20 , "GENERAL_ARC_DETECTION_LOW_CURRENT_TRIP"},
												 { 0x21 , "GENERAL_ARC_DETECTION_HIGH_CURRENT_TRIP"},
												 { 0x22 , "DIMMER_ARC_DETECTION_LOW_CURRENT_TRIP"},
												 { 0x23 , "DIMMER_ARC_DETECTION_HIGH_CURRENT_TRIP"},
												 { 0x24 , "GROUND_FAULT_OVERCURRENT_TRIP"},
												 { 0x25 , "PARALLEL_ARC_FAULT_TRIP"},
												 { 0x26 , "SHORT_DELAY_FAULT_OVERCURRENT_TRIP"},
												 { 0x27 , "OVERVOLTAGE_TRIP"},
												 { 0x28 , "HRGF_COLDSTART_TRIP"},
												 { 0x29 , "HRGF_RUNNING_TRIP"},
												 { 0x2A , "GN_TRIP"},
												 { 0x2B , "OVERLOAD_TRIP"},
												 { 0x2C , "HIGH_GN_TRIP"},
												 { 0x3F , "PRIMARY_HAL_TRIP_OR_OFF"},
												 { 0x40 , "FAILED_STARTUP_RAM_TEST"},
												 { 0x41 , "FAILED_STARTUP_ROM_TEST"},
												 { 0x42 , "FAILED_STARTUP_PROCESSOR_TEST"},
												 { 0x43 , "DATA_OVERRUN_TRIP"},
												 { 0x44 , "INVALID_DATA_SEQUENCE_TRIP"},
												 { 0x45 , "INCORRECT_INTERRUPT_COUNT_TRIP"},
												 { 0x46 , "HF_SENSE_FAULT_TRIP"},
												 { 0x47 , "LINE_CURRENT_BIAS_ERROR_TRIP"},
												 { 0x48 , "GROUND_FAULT_CURRENT_BIAS_ERROR_TRIP"},
												 { 0x49 , "LOG_HF_MIN_DETECTOR_STUCK_ERROR_TRIP"},
												 { 0x4A , "FAILED_CONTINUOUS_RAM_TEST_TRIP"},
												 { 0x4B , "FAILED_CONTINUOUS_ROM_TEST_TRIP"},
												 { 0x4C , "FAILED_CONTINUOUS_PROCESSOR_TEST_TRIP"},
												 { 0x4D , "NONHANDLED_INTERRUPT_TRIP"},
												 { 0x4E , "FAILED_TRIP_ATTEMPT"},
												 { 0x4F , "FAILED_SELF_CHECK_GF_INPUT"},
												 { 0x50 , "FAILED_SELF_CHECK_OUTPUT"},
												 { 0x51 , "FAILED_SELF_CHECK_CT_DIRECTION"},
												 { 0x52 , "COMPLETE_LOSS_OF_ZCD_TRIP"},
												 { 0x53 , "FAILED_TRIP_CIRCUIT_AUTO_MONITOR"},
												 { 0x54 , "OVER_TEMP_EVENT_OCCURED"},
												 { 0x55 , "OVER_TEMP_TRIP_EVENT_OCCURED"},
												 { 0x56 , "COOLED_DOWN_EVENT"},
												 { 0x60 , "SECONDARY_SWITCH_FAULT"},
												 { 0x61 , "SECONDARY_SWITCH_STUCK_OPEN"},
												 { 0x62 , "SECONDARY_SWITCH_STUCK_CLOSED"},
												 { 0x63 , "SECONDARY_SWITCH_UNEXPECTED_OPEN"},
												 { 0x64 , "SECONDARY_SWITCH_UNEXPECTED_CLOSED"},
												 { 0x65 , "SECONDARY_SWITCH_UNEXPECTED_OVERLOAD"},
												 { 0x66 , "SECONDARY_SWITCH_DEFAULT_TO_CLOSED"},
												 { LOG_SPI_COMM_FAIL , "SPI Communication fail"},
												 { LOG_SPI_COMM_ERR , "SPI Communication error LOG"},
												 { LOG_I2C_COMM_FAIL , "I2C Communication fail"},
												 { LOG_I2C_COMM_ERR , "I2C Communication error LOG"},

												 // 128-255 USER_INITIATED_HW_FAULT_CODES various.

#ifdef  _ERROR_LOG_DEBUG_
                                                { LOG_BREAKER_TURN_ON , "Breaker Turns ON"},
                                                { LOG_BREAKER_TURN_OFF , "Breaker Turns OFF"},
                                                { LOG_BREAKER_PROVISIONING , "Breaker Starts Provisioning"},
                                                { LOG_BREAKER_RE_PROVISIONING , "Breaker RE_Starts Provisioning"},
                                                { LOG_BREAKER_TIMER_BASED_LOG , "Breaker TIME_BASE LOG"},

                                                

#endif
                                                // if you add another line, increase MAX_ERROR_LOGS

};

void InitiateErrLog()
{
    uint8_t  ReadValue,i;

    memset (&ErrLogStr,0, sizeof (ErrLogStr));
    memset (&EpochTimeConnectStr,0, sizeof (EpochTimeConnectStr));

    ReadValue = ReadByteFromFlash(EEOFFSET_INIT_ERROR_LOG_LABEL);

    if (ReadValue == ERROR_LOG_INITIATED)
    {
        //Read ErroLog
        ReadArrayFromFlash(EEOFFSET_ERROR_LOG_LABEL, (char *)&ErrLogStr,sizeof(ErrLogStr));
#ifdef  _ERROR_LOG_DEBUG_    
        if (ErrLogStr.LogIsFull)
        {
          for (i=ErrLogStr.Index; i< ERROR_LOG_SIZE;i++)
            {
                if (ErrLogStr.ErrLogArray[i].EpochTime < EPOCH_TIME_Jan_1_2023)
                {
                    //That means we saved the log and we lost power and we gaind it back
                    // That means whatever value is saved is not valid any more so we will replace it
                    // We don't need to save it in NVM and we will wait till another case save it since it will not 
                    // make a difference and we don't know if there is more records like that.
                    ErrLogStr.ErrLogArray[i].EpochTime = INVALID_TIME_STAMP;

                }
                ErrorPreperationLog(i);
            }
        }    
        for (i=0; i<ErrLogStr.Index;i++)
        {
            if (ErrLogStr.ErrLogArray[i].EpochTime < EPOCH_TIME_Jan_1_2023)
            {
                //That means we saved the log and we lost power and we gaind it back
                // That means whatever value is saved is not valid any more so we will replace it
                // We don't need to save it in NVM and we will wait till another case save it since it will not 
                // make a difference and we don't know if there is more records like that.
                ErrLogStr.ErrLogArray[i].EpochTime = INVALID_TIME_STAMP;

            }
            ErrorPreperationLog(i);
        }
#endif        
        
    }
    else
    {
        //Initiating The Erro log
        WriteByteToFlash(EEOFFSET_INIT_ERROR_LOG_LABEL,ERROR_LOG_INITIATED );
          SaveErrLogToNVM(); // Ahmed Come back here to check why I am doing that
#ifdef  _ERROR_LOG_DEBUG_        
        ets_printf("NVM Log Error is empty\n");
#endif            
    }

}

void ErrorLogOneSecondTimer()
{
    
    static uint8_t TimeStampCorrectionIndex = 0;
    static uint8_t NumberOfLogsToCorrect = 0;
    static uint8_t SendingToCloudCounter = 0;

    ErrorLogOneSecondTimerCounter++;
    
    //Capture the time of the connection and update the internal timer
    if (JustPoweredUp == true)
    {
        if (DeviceInfo.IoTConnected == true)
        {
            EpochTimeConnectStr.CounterTimeConnected = ErrorLogOneSecondTimerCounter;
            ErrorLogOneSecondTimerCounter = GetEpochTime();
            EpochTimeConnectStr.EpochTimeConnected = ErrorLogOneSecondTimerCounter;
            JustPoweredUp = false;
        }

    }

    // correct time stamp once we are connected
    if ( (DeviceInfo.IoTConnected == true) && (DeviceInfo.WiFiDisconnected == false) && (EpochTimeConnectStr.DoneTimeUpdatingTheLog == false) &&
            (EpochTimeConnectStr.EpochTimeConnected != INVALID_TIME_STAMP) ) 
    {
        if (ErrLogStr.LogIsFull == false)
        {
            //update only the existing logs
            NumberOfLogsToCorrect = ErrLogStr.Index;
        }
        else
        {
            //the Array is full so update everything
            NumberOfLogsToCorrect = ERROR_LOG_SIZE;
        }
        for (TimeStampCorrectionIndex=0;TimeStampCorrectionIndex<NumberOfLogsToCorrect;TimeStampCorrectionIndex++)
        {
            if (    (ErrLogStr.ErrLogArray[TimeStampCorrectionIndex].EpochTime < EPOCH_TIME_Jan_1_2023) 
                 && (ErrLogStr.ErrLogArray[TimeStampCorrectionIndex].CloudUpdated == false) // it should be false, but just in case if we lose connection while we are updating the time
               )
            {
                ErrLogStr.ErrLogArray[TimeStampCorrectionIndex].EpochTime =  EpochTimeConnectStr.EpochTimeConnected - EpochTimeConnectStr.CounterTimeConnected + ErrLogStr.ErrLogArray[TimeStampCorrectionIndex].EpochTime;
            }
        }
        EpochTimeConnectStr.DoneTimeUpdatingTheLog = true;

        if (ErrLogStr.LogIsFull == false)
        {
            //That means the first element in the array is the first error log (the earliest)
            // and the last element is "ErrLogStr.Index - 1" + 1 for zero index
            SendingToCloudCounter = 0;

        }
        else
        {
            //The earliest one will be what the index pointing to and increasing till MaxLog and then go sequential till "ErrLogStr.Index - 1" + 1
        	SendingToCloudCounter = ErrLogStr.Index-1;
        }

        SaveErrLogToNVM();
    }

    //Now we will send the log one by one to the cloud starting with the earliest till the latest one
    // And then send it based on when it happened
    if ( (EpochTimeConnectStr.DoneTimeUpdatingTheLog == true) && (SendLogToTheCloud == true))
    {
        SendErrLogToCloud(SendingToCloudCounter);
        SendingToCloudCounter++;
        if (SendingToCloudCounter >= ERROR_LOG_SIZE)
        {
            SendingToCloudCounter = 0;
        }
        else
        {
			//overflow ERROR_LOG_SIZE and check if the counter goes from old index to latest
			if (SendingToCloudCounter == ErrLogStr.Index)
			{
				//That means we are done updating the cloud
				SendLogToTheCloud = false;
				//Save the log in flash
				SaveErrLogToNVM();
			}
        }

    }

    if ( (DeviceInfo.IoTConnected == true) && (PreviousWiFiDisConnectionState != DeviceInfo.WiFiDisconnected) )
    {
        // That means we were provisinoed and either lost the connection or gained it back
        PreviousWiFiDisConnectionState = DeviceInfo.WiFiDisconnected;

        if (DeviceInfo.WiFiDisconnected == true)
        {
            //this means we lost connection, we need to reset EpochTimeConnectStr.DoneTimeUpdatingTheLog
            // So when we get connected, go through the log and update it
            EpochTimeConnectStr.DoneTimeUpdatingTheLog = false;
            SendLogToTheCloud = true;
#ifdef _ERROR_LOG_DEBUG_
            ets_printf(" We lost Wifi connection\n" );    
#endif                
        }
        else
        {
            //that means we connected back
#ifdef _ERROR_LOG_DEBUG_            
            ets_printf(" We got Wifi connection back\n" );    
#endif        
        }
    }

#ifdef _DEBUG_TIMER_GENERATING_LOG_
    #define TIME_INTERVAL_TO_SEND_LOG    9 //Every 10 seconds
    static uint8_t TimeEntervalCounter = TIME_INTERVAL_TO_SEND_LOG;
    if (TimeEntervalCounter != 0)
    {
        TimeEntervalCounter--;
    }
    else
    {
        TimeEntervalCounter = TIME_INTERVAL_TO_SEND_LOG;
        ErrLog(LOG_BREAKER_TIMER_BASED_LOG);
    }
#endif

}



void ErrLog(uint8_t log)
{
    if (ErrLogStr.Index >= ERROR_LOG_SIZE )
    {
        ErrLogStr.Index = 0;
        ErrLogStr.LogIsFull = true; 
    }
        
    ErrLogStr.ErrLogArray[ErrLogStr.Index].ErrorLog = log;
    ErrLogStr.ErrLogArray[ErrLogStr.Index].EpochTime = GetErrLogTime();
    ErrLogStr.ErrLogArray[ErrLogStr.Index].CloudUpdated = false; //We may have the buffer full and we are overwritting, so we need to reset that flag for every entery
#ifdef _ERROR_LOG_DEBUG_
    ErrLogStr.ErrLogArray[ErrLogStr.Index].EventLog=ErrLogStr.GlobalEventLog;
#endif

    if ((DeviceInfo.IoTConnected == true) && (DeviceInfo.WiFiDisconnected == false))
    {    
        SendErrLogToCloud(ErrLogStr.Index);
    }
    else
    {
#ifdef _ERROR_LOG_DEBUG_
        ErrorPreperationLog(ErrLogStr.Index);
#endif
    }
    ErrLogStr.Index++;

#ifdef _ERROR_LOG_DEBUG_
    ErrLogStr.GlobalEventLog++;
#endif
    SaveErrLogToNVM(); 
    
}

uint32_t GetErrLogTime(void)
{
    uint32_t ReturnTimeValue;

    if (DeviceInfo.IoTConnected == true)
    {
        ReturnTimeValue =  GetEpochTime();
        ErrorLogOneSecondTimerCounter = ReturnTimeValue;
    }
    else
    {
        ReturnTimeValue = ErrorLogOneSecondTimerCounter;
    }
    return ReturnTimeValue;
}

void SendErrLogToCloud(uint8_t LogIndex)
{
    // we need to modify that code to send in Json format, but for now I'll send it pure text
    // The format for now will be the error explanation and the time stamp 
    // " NORMAL STARTUP at 123456789"
    

    if (( (DeviceInfo.IoTConnected == true) && (DeviceInfo.WiFiDisconnected == false) )
    		&& (ErrLogStr.ErrLogArray[LogIndex].CloudUpdated == false))
    {
        ErrorPreperationLog(LogIndex);
        DCI_Update_ErroLogUpdate(&ErrorReportBuffer);
        ErrLogStr.ErrLogArray[LogIndex].CloudUpdated = true;
    }

}

void SaveErrLogToNVM(void)
{
    WriteArrayToFlash(EEOFFSET_ERROR_LOG_LABEL,(char *)&ErrLogStr,sizeof(ErrLogStr));
}

void GetErrorDescription(uint8_t ErrorId, char* ErrorDescription)
{
    uint8_t i;
    // Please check if  MAX_ERROR_LOGS matches the number of error logs
    for (i=0; i < MAX_ERROR_LOGS; i++)
    {
        if (ErrorId == ErrLogDataInfo[i].ErrorId)
        {
        	memcpy(ErrorDescription, ErrLogDataInfo[i].ErrorDescription, sizeof(ErrLogDataInfo[i].ErrorDescription));
        	break;
        }
    }
    return;
}

#if 0  
don't use since it creates time shifting
void ConvertEpochTimeToReadableFormat( uint32_t EpocTime, char * TimerBuffer, uint8_t BuffSize)
{
    
    time_t now = 0;
    struct tm timeinfo;
    struct timeval nowset;
    int rc = 0;


    memset( &timeinfo, 0, sizeof( timeinfo ) );
    nowset.tv_sec = EpocTime;

    rc = settimeofday( &nowset, NULL );

    //setenv( "TZ", "IST-05:30", 1 );    ///< Set timezone to Indian Standard Time.
    setenv( "TZ", "IST+05", 1 );    ///< Set timezone to EST Standard Time.
    tzset();


    time( &now );        ///< fetching epoch time second.
    localtime_r( &now, &timeinfo );
    strftime( TimerBuffer, BuffSize, "%c", &timeinfo );
    

    //printf( "Formatted date & time : %s\n", TimerBuffer );

}
#endif

void ErrorPreperationLog(uint8_t index)
{
    
    uint8_t LogID;
    char buff[MAX_ERROR_DESCRIPTION_SIZE] = "UNKNOWN ERROR";
    uint32_t    TimeStamp;
    uint32_t    event;
    uint32_t    errorlog;
    //char strftime_buf[STRFTIME_BUF_SIZE];

    //memset( strftime_buf, 0, STRFTIME_BUF_SIZE );
    memset( ErrorReportBuffer, 0, sizeof (ErrorReportBuffer) );

    LogID = ErrLogStr.ErrLogArray[index].ErrorLog;
    TimeStamp = ErrLogStr.ErrLogArray[index].EpochTime;
    errorlog = ErrLogStr.ErrLogArray[index].ErrorLog;
#ifdef _ERROR_LOG_DEBUG_
    event = ErrLogStr.ErrLogArray[index].EventLog;
#endif
    GetErrorDescription(LogID, buff);
    if ((DeviceInfo.IoTConnected == true) && (DeviceInfo.WiFiDisconnected == false))
    {
        //ConvertEpochTimeToReadableFormat(TimeStamp,strftime_buf,STRFTIME_BUF_SIZE);
#ifdef _ERROR_LOG_DEBUG_
        //sprintf(ErrorReportBuffer," Send To Cloud %s %u at %u   %s", buff,event,TimeStamp , strftime_buf);
        //sprintf(ErrorReportBuffer," Send To Cloud %s %u at %u ", buff,event,TimeStamp );
    	sprintf(ErrorReportBuffer,"%u,%x,%u,%s", event, errorlog, TimeStamp, buff );
#else
        //sprintf(ErrorReportBuffer," Send To Cloud %s  at %u  ", buff,TimeStamp );
    	sprintf(ErrorReportBuffer,"%x,%u,%s", errorlog, TimeStamp, buff);
#endif
    }
    else
    {
#ifdef _ERROR_LOG_DEBUG_
        if (TimeStamp == INVALID_TIME_STAMP)
        {
            sprintf(ErrorReportBuffer," Write to NVM %s %u,%x  With Invalid Time Stamp", buff,event,errorlog);
        }
        else
        {
            sprintf(ErrorReportBuffer," Write to NVM %s %u,%x at %u", buff,event,errorlog,TimeStamp);
        }
        
#else
        sprintf(ErrorReportBuffer," Write to NVM %u,%s at %u",errorlog, buff,TimeStamp);
#endif
    }
#ifdef _ERROR_LOG_DEBUG_
	ets_printf("%s \n", ErrorReportBuffer);
#endif
}



//Test_RTC_App_Main
// https://www.oryx-embedded.com/doc/date__time_8c_source.html to conver from EpochTime to a readable format :)


#if 0
#define ERROR_LOG_SIZE    100

typedef struct
{
    uint8_t    ErrorLog;
    uint32_t   EpochTime;
    bool       CloudUpdated; 
}ErrLog_str;


typedef struct
{
    ErrLog_str ErrLogArray[ERROR_LOG_SIZE];
    uint8_t    Index;  //last error log
    bool       LogIsFull; // to indicate we have ERROR_LOG_SIZE and now we are overwriting the old vlause
}ErrorLogStrArray_str;

typedef struct
{
    uint32_t   CounterTimeConnected;
    uint32_t   EpochTimeConnected;
    bool       DoneUpdatingTheLog;
}EpochTimeConnect_str;

#endif
void get_TripLog(uint8_t *sblcp_trip_log)
{

	memcpy(sblcp_trip_log, ErrLogStr.ErrLogArray, sizeof(ErrLogStr.ErrLogArray));
	sblcp_trip_log[sizeof(ErrLogStr.ErrLogArray)] = ErrLogStr.Index - 1;
}
