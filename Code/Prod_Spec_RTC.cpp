/**
 *****************************************************************************************
 * @file	    Prod_Spec_RTC.cpp

 * @details     This file contains test application for Internal RTC through interface class.
 * @copyright   2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Includes.h"
#include "string.h"
#include "stdio.h"
#include "TimeCommon.h"
#include "Rtc_If.h"
#include "POSIXTimeUtil.h"
#include "Prod_Spec_RTC.h"
#include "LWIPService.h"
#include "OS_Mutex.h"
#include "Prod_Spec_LTK_ESP32.h"
#include "esp_sntp.h"
#include "sntp_if.h"
#include "Babelfish_Assert.h"

//Ahmed
#include "Breaker.h"
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint16_t UPDATE_DCID_FROM_RTC_TIMEOUT_MSEC = 1000U;
static DCI_Owner* device_up_time_owner = nullptr;

/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
DCI_Owner* m_owner_unix_epoch_time = nullptr;
DCI_Owner* m_owner_unix_epoch_64_bit_time = nullptr;
DCI_Owner* m_owner_time_offset = nullptr;
DCI_Owner* m_owner_date_format = nullptr;
DCI_Owner* m_owner_time_format = nullptr;
DCI_Owner* m_owner_time = nullptr;
DCI_Owner* m_owner_date = nullptr;

OS_Mutex* Mutex_Rtc = nullptr;


/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */
/**
 **********************************************************************************************
 * @brief                          This function updates the rtc unix time dcid in every 1 sec
 * @param[in] param                parameter we get from callback
 * @return[out] void               none
 * @n
 **********************************************************************************************
 */
void Update_RTC_DCID_Event( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );

/**
 **********************************************************************************************
 * @brief                             static function for Update_RTC callback
 * @param[in] cr_task                 CR_Tasker handle
 * @param[in] access_struct           DCI parameter information
 * @return[out] DCI_CB_RET_TD         status
 **********************************************************************************************
 */
static DCI_CB_RET_TD Update_RTC_CB( DCI_CBACK_PARAM_TD cback_param,
									DCI_ACCESS_ST_TD* access_struct );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Prod_Spec_RTC_Init( void )
{
#if defined USE_ONBOARD_RTC
	// Currently supported only for webserver enabled devices(2xx, 4xx)
	// Disable Rtc_If instance creration we don't have continuous power to RTC, Testharness testcase
	// may start failing in that case.
	// coverity[leaked_storage]
	new Rtc_If();
#endif
	BF_Lib::Timers* update_rtc_dcid_timer;

	// coverity[leaked_storage]
	new Time_Conversion::POSIXTimeUtil();

	m_owner_unix_epoch_time = new DCI_Owner( DCI_UNIX_EPOCH_TIME_DCID );
	m_owner_unix_epoch_64_bit_time = new DCI_Owner( DCI_UNIX_EPOCH_TIME_64_BIT_DCID );
	m_owner_time_offset = new DCI_Owner( DCI_TIME_OFFSET_DCID );
	m_owner_date_format = new DCI_Owner( DCI_DATE_FORMAT_DCID );
	m_owner_time_format = new DCI_Owner( DCI_TIME_FORMAT_DCID );
	m_owner_time = new DCI_Owner( DCI_RTC_TIME_DCID );
	m_owner_date = new DCI_Owner( DCI_RTC_DATE_DCID );
	device_up_time_owner = new DCI_Owner( DCI_DEVICE_UP_TIME_SEC_DCID );


	m_owner_unix_epoch_time->Attach_Callback( &Update_RTC_CB, NULL,
											  ( DCI_ACCESS_SET_RAM_CMD_MSK |
												DCI_ACCESS_GET_RAM_CMD_MSK ) );


	m_owner_unix_epoch_64_bit_time->Attach_Callback( &Update_RTC_CB, NULL,
													 ( DCI_ACCESS_SET_RAM_CMD_MSK |
													   DCI_ACCESS_GET_RAM_CMD_MSK ) );

	update_rtc_dcid_timer = new BF_Lib::Timers( Update_RTC_DCID_Event, 0 );
	update_rtc_dcid_timer->Start( UPDATE_DCID_FROM_RTC_TIMEOUT_MSEC, TRUE );

	Mutex_Rtc = new OS_Mutex();

#ifdef SNTP_SETUP
	/*Pass handle of SNTP_If object to LWIPService*/
	Sntp_If_Init( new SNTP_If( &Update_Epoch_Time ) );
#endif	// SNTP_SETUP

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
static DCI_CB_RET_TD Update_RTC_CB( DCI_CBACK_PARAM_TD cback_param,
									DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_PROCESS_NORMALLY;
	uint32_t* epoch_time_ptr = nullptr;
	Time_Internal utc_date_time = { 0, 0, 0, 0, 1, 1, 0 };

	/* check if command is valid */
	if ( access_struct->command == DCI_ACCESS_SET_RAM_CMD )
	{
		if ( Mutex_Rtc->Acquire( OS_MUTEX_INDEFINITE_TIMEOUT ) )
		{
			epoch_time_ptr = reinterpret_cast<uint32_t*>( access_struct->data_access.data );
			Time_Conversion::POSIXTimeUtil::Get_UTC_Time_From_POSIX_Time(
				epoch_time_ptr[0], &utc_date_time );

			utc_date_time.microsecond = epoch_time_ptr[1];

#ifdef USE_ONBOARD_RTC
			Rtc_If::Set_Date_Time( utc_date_time );
#else
			// Update the time of the external RTC
#endif
			Mutex_Rtc->Release();
		}
		else
		{
			return_status = DCI_CBack_Encode_Error( DCI_ERR_VALUE_LOCKED );
		}
	}
	else if ( access_struct->command == DCI_ACCESS_GET_RAM_CMD )
	{
		/* Update DCID with RTC time */
		Update_RTC_DCID_Event( nullptr );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Update_RTC_DCID_Event( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{
	uint32_t epoch_time_temp[2] = {0}, temp = 0;
	date_time_format_t curr_date_time;
	Time_Internal utc_date_time = { 0, 0, 0, 0, 1, 1, 0 };
	uint32_t device_up_time_sec = 0U;

	device_up_time_owner->Check_Out( device_up_time_sec );
	device_up_time_sec += 1;
	device_up_time_owner->Check_In( device_up_time_sec );

#ifdef USE_ONBOARD_RTC
	Rtc_If::Get_Date_Time( utc_date_time );
#else
	// utc_date_time = fetch current TIME from external RTC
#endif
	curr_date_time.p.date = utc_date_time.day;
	curr_date_time.p.month = utc_date_time.month;
	curr_date_time.p.year = utc_date_time.year + 2000U;

	m_owner_date->Check_In( &curr_date_time );

	curr_date_time.date_time = 0U;
	curr_date_time.q.hh = utc_date_time.hour;
	curr_date_time.q.mm = utc_date_time.minute;
	curr_date_time.q.ss = utc_date_time.second;

	m_owner_time->Check_In( &curr_date_time );

	Time_Conversion::POSIXTimeUtil::Get_POSIX_Time_From_UTC_Time( &utc_date_time,
																  &temp );

	epoch_time_temp[0] = temp;
	epoch_time_temp[1] = utc_date_time.microsecond;
	m_owner_unix_epoch_time->Check_In( &epoch_time_temp[0] );
	m_owner_unix_epoch_64_bit_time->Check_In( &epoch_time_temp );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Update_Epoch_Time( SNTP_If::cback_param_t param, SNTP_If::cback_event_t event,
						uint32_t* epoch_time, uint32_t* epoch_time_usec )
{
	Time_Internal utc_date_time = { 0, 0, 0, 0, 1, 1, 0 };

	BF_Lib::Unused<SNTP_If::cback_param_t>::Okay( param );

	switch ( event )
	{
		case SNTP_If::NEW_TIME_RECEIVED:
			if ( Mutex_Rtc->Acquire( OS_MUTEX_INDEFINITE_TIMEOUT ) )
			{
				Time_Conversion::POSIXTimeUtil::Get_UTC_Time_From_POSIX_Time(
					*epoch_time, &utc_date_time );

				utc_date_time.microsecond = *epoch_time_usec;

#ifdef USE_ONBOARD_RTC
				Rtc_If::Set_Date_Time( utc_date_time );
#else
				// Update the time of the external RTC
#endif
				Mutex_Rtc->Release();
			}
			break;

		case SNTP_If::RETRIEVE_TIME:
#ifdef USE_ONBOARD_RTC
			Rtc_If::Get_Date_Time( utc_date_time );
#else
			// get the time from external RTC
#endif
			Time_Conversion::POSIXTimeUtil::Get_POSIX_Time_From_UTC_Time( &utc_date_time,
																		  epoch_time );
			*epoch_time_usec = utc_date_time.microsecond;
			break;

		default:
			// Do nothing because we don't have anything to do.
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t Get_Epoch_Time( void )
{
	uint32_t epoc_time_temp = 0U;

	m_owner_unix_epoch_time->Check_Out( epoc_time_temp );

	return ( epoc_time_temp );
}

#if 0	///< Not getting used anywhere. Keeping it for further reference
/**
 **********************************************************************************************
 * @brief                       This fuction will provide current epoch time.
 * @param[in] void              none
 * @return[out] uint32_t        epoch time
 * @n
 **********************************************************************************************
 */
bool Get_Date_Time( uint32_t epoch_time, uint32_t* date, uint32_t* time )
{
	bool return_val = false;
	date_time_format_t curr_date_time;
	Time_Internal utc_date_time = { 0, 0, 0, 0, 1, 1, 0 };

	if ( Time_Conversion::POSIXTimeUtil::Get_UTC_Time_From_POSIX_Time(
			 epoch_time, &utc_date_time ) )
	{

		curr_date_time.p.date = utc_date_time.day;
		curr_date_time.p.month = utc_date_time.month;
		curr_date_time.p.year = utc_date_time.year + 2000U;
		memcpy( date, &curr_date_time, sizeof( &date ) );

		curr_date_time.date_time = 0U;
		curr_date_time.q.hh = utc_date_time.hour;
		curr_date_time.q.mm = utc_date_time.minute;
		curr_date_time.q.ss = utc_date_time.second;
		memcpy( time, &curr_date_time, sizeof( &time ) );

		return_val = true;
	}

	return ( return_val );
}

#endif

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void SNTP_Init( void )
{
	LWIP_Service_TCPIP_Thread_CallBack( nullptr );
}

//Ahmed
uint32_t GetEpochTime( void )
{
	return Get_Epoch_Time();
}
