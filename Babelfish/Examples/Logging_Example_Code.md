@example  Logging example
@brief    Sample code to use logging feature

@par Pin Configurations 
@details   not applicable

@par Dependency
@details Not applicable

@par Sample Code Listing
 
~~~~{.cpp}

#ifdef LOGGING_SUPPORT

#include "Log_DCI.h"

#ifdef LOGGING_ON_RAM
#include "Log_Mem_RAM.h"
#endif

#if defined ( LOGGING_ON_INT_FLASH ) || defined ( LOGGING_ON_EXT_FLASH )
#include "Log_Mem_Flash.h"
#endif

#include "Modbus_Logging.h"

void Logging_Init( void );

/* Main dcid array used for logging. Whenever logging is triggered
* values of the dcid's in this array will be logged in the memory 	
*/
const DCI_ID_TD log_dcid_array[] =
{
	DCI_UNIX_EPOCH_TIME_DCID,
	DCI_LOG_TEST_1_DCID,
	DCI_SIMPLE_APP_CONTROL_WORD_DCID	  
};

/*	DCIDs which would be watched for any change and corresponding we can 
* trigger change based log
*/
const DCI_ID_TD log_dcid_watch_array[] =
{
	DCI_LOG_TEST_1_DCID,
	DCI_SIMPLE_APP_CONTROL_WORD_DCID
};

DCI_Owner * log_test_1;
DCI_Owner * change_based_log;
DCI_Owner* manual_log_trigger;
static void Logging_Manual_Trigger_Test_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param );
void Logging_Manual_Trigger_Test( void );

static void Logging_Interval_Trigger_Test_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param );

BF_Misc::Log_DCI* log_dci_handler;

static const uint8_t DATA_SERIES_LOG_ID = 0U;
/*Assign log ids here for other types (like alarm, audit) and increase TOTAL log id count*/
static const uint8_t TOTAL_LOG_ID = 1U;

BF_Misc::Log_Mem* log_mem_handle_array[TOTAL_LOG_ID]={nullptr};
DCI_Owner* interval_logging;

#ifdef LOGGING_SUPPORT

#ifdef LOGGING_ON_RAM
static const uint16_t LOG_RAM_BLOCK_SIZE = 4000U;
static uint8_t log_ram_block[LOG_RAM_BLOCK_SIZE];
#endif

void Configure_Flash_For_Logging( void );


/*
 * brief                    Example Code for starting Logging
 * note                     The below function does the following
 *                          1. creating owners for DCIDs
 *                          2. RAM or FLASH will be initialized according to defined Memory type(RAM or FLASH)
 *							3. Initializing Modbus to read the logs 
 *                          4. Initializing log dci handler which needs log dcid array, log mem handle, log change trigger(type of change log to be 
 *								executed) and the log watch list(used for change based logging )
 *                          5. If the DCID DCI_CHANGE_BASED_LOGGING_DCID has any value other than ONLY_MANUAL_TRIGGER start change function will
 *								start change based logging.
 *                          6. Interval based logging will be done according to the value of DCID DCI_LOG_INTERVAL_TIME_MS_DCID. This DCID contains the 
 *								interval time for interval logging in milliseconds.
 */
void Logging_Init( void )
{
	uint16_t dcid_count;
	uint16_t dcid_watch_array_count = 0U;
	uint8_t log_change_trigger = 0U;

	log_test_1 = new DCI_Owner( DCI_LOG_TEST_1_DCID );

	manual_log_trigger = new DCI_Owner( DCI_MANUAL_LOG_TRIGGER_DCID );

	change_based_log = new DCI_Owner( DCI_CHANGE_BASED_LOGGING_DCID );

	dcid_count = sizeof ( log_dcid_array ) / sizeof ( DCI_ID_TD );
	dcid_watch_array_count = sizeof ( log_dcid_watch_array ) / sizeof ( DCI_ID_TD );

#ifdef LOGGING_ON_RAM
	log_mem_handle_array[DATA_SERIES_LOG_ID] = new BF_Misc::Log_Mem_RAM( log_ram_block,
																		 LOG_RAM_BLOCK_SIZE );

#endif

#if defined ( LOGGING_ON_INT_FLASH ) || defined ( LOGGING_ON_EXT_FLASH )
	Configure_Flash_For_Logging();
#endif

#ifdef MODBUS_UART_SETUP
	new BF_Mbus::Modbus_Logging( modbus_serial_net, log_mem_handle_array, TOTAL_LOG_ID );

#endif

	change_based_log->Check_Out( log_change_trigger );
	log_dci_handler = new BF_Misc::Log_DCI( log_dcid_array, dcid_count,
											log_mem_handle_array[DATA_SERIES_LOG_ID],
											static_cast < BF_Misc::Log_DCI::log_change_trigger_t > ( log_change_trigger ),
											log_dcid_watch_array, dcid_watch_array_count );

	new CR_Tasker( Logging_Manual_Trigger_Test_Static, NULL );

	if ( log_change_trigger != BF_Misc::Log_DCI::ONLY_MANUAL_TRIGGER )
	{
		log_dci_handler->Start_Change();
	}
	/* Interval Based Log Trigger */
	interval_logging = new DCI_Owner( DCI_LOG_INTERVAL_TIME_MS_DCID );

	new CR_Tasker( Logging_Interval_Trigger_Test_Static, NULL );

}

#if defined ( LOGGING_ON_INT_FLASH ) || defined ( LOGGING_ON_EXT_FLASH )

#if defined ( uC_STM32F437_USAGE )

static const uint8_t MAX_SECTOR = 3U;

/*
 * brief                    Array of sector map
 * note                     Available sector map of internal bank2-flash for the controller being used(given example for STM32F437)
 */
static const BF_Misc::Log_Mem_Flash::memory_block_t INTERNAL_LOG_FLASH_PAGE[MAX_SECTOR] =
{
	//{  0x08100000U,  0x08103FFFU,  0x4000U, 4U},          //16K  //Bank 2 Sector 16
	//{  0x08104000U,  0x08107FFFU,  0x4000U, 5U},          //16K //Bank 2 Sector 17
	//{  0x08108000U,  0x0810BFFFU,  0x4000U, 6U},          //16K //Bank 2 Sector 18
	//{  0x0810C000U,  0x0810FFFFU,  0x4000U, 6U},          //16K //Bank 2 Sector 19
	//{  0x08110000U,  0x0811FFFFU,  0x10000U, 7U},          //64K //Bank 2 Sector 20	
	//{  0x08120000U,  0x0813FFFFU,  0x20000U, 7U},          //128K //Bank 2 Sector 21
	//{  0x08140000U,  0x0815FFFFU,  0x20000U, 7U},          //128K //Bank 2 Sector 22
	//{  0x08160000U,  0x0817FFFFU,  0x20000U, 7U},          //128K //Bank 2 Sector 23
	//{  0x08180000U,  0x0819FFFFU,  0x20000U, XU},           //128K  //Bank 2 Sector 24
	{  0x081A0000U,  0x081BFFFFU,  0x20000U, 4000U, 0U},           //128K  //Bank 2 Sector 25
	{  0x081C0000U,  0x081DFFFFU,  0x20000U, 4000U, 1U},           //128K  //Bank 2 Sector 26
	{  0x081E0000U,  0x081FFFFFU,  0x20000U, 4000U, 2U},           //128K  //Bank 2 Sector 27
	//{  0x00000000U,  0x00000000U,  0x00000U, 0xFFFFU}
};
/*	Note-
	1. Device should have atleast 2 flash sectors for each logging id.
	2. The memory sectors should be sequential.
*/

/*
 * brief                    Chip configuration defined for time series logging.
 * note                     For different logging we need to have configuration defined. Using Bank2's sector 13,14 and 15 for 
 * 							time series logging on flash in this example
 */
static const NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD STM32_TIME_SERIES_LOG_FLASH_CFG =
{
	INTERNAL_LOG_FLASH_PAGE[0U].sector_start_addr, 		/*start_address - Bank 2 Sector 13*/
	INTERNAL_LOG_FLASH_PAGE[0U].sector_start_addr, 		/*mirror_start_address*/
	INTERNAL_LOG_FLASH_PAGE[MAX_SECTOR - 1U].sector_end_addr,  	/*end_address*/
};

#endif /*uC_STM32F437_USAGE*/

/*
 * brief                    Flash handler
 * note                     Flash handler for the configurations mentioned above. Will be passed to log mem flash to do read 
 * 							, write and erase operations on flash.
 */
static NV_Ctrl* nv_ctrl_log_flash;
void dummy_call_back(NV_Ctrl::cback_param_t func_param, NV_Ctrl::nv_status_t result);
/*
 * brief                    Flash based logging initialization
 * note                     Here we will be initializing the Flash memory logging.
 * 							1. create a flash memory handle using flash configuration.
 * 							2. initialize log mem flash by passing flash handle, start addrs of the flash,
 * 							allocated size in flash, log page size, max number of sectors and we can enable 
 * 							protections of data on flash by passing use_protection = true which will enable Checksum
 * 							so logged data
 */
void Configure_Flash_For_Logging( void )
{
  	bool_t synchronous_erase = false;
	bool_t use_protection = false;


  	nv_ctrl_flash_data_series = new NV_Ctrls::uC_Flash( &STM32_TIME_SERIES_LOG_FLASH_CFG, FLASH,
										   synchronous_erase,dummy_call_back );

	log_mem_handle_array[DATA_SERIES_LOG_ID] = new BF_Misc::Log_Mem_Flash( nv_ctrl_flash_data_series,
																		   INTERNAL_LOG_FLASH_PAGE,
																		   MAX_SECTOR,
																		   use_protection );

}
void dummy_call_back(NV_Ctrl::cback_param_t func_param, NV_Ctrl::nv_status_t result)
{
	
}
#endif /* ( LOGGING_ON_INT_FLASH ) || ( LOGGING_ON_EXT_FLASH )*/

/*
 * brief                    Manual log CR Task
 * note                     we will be monitoring the manual log dcid to trigger the manual log when the dcid is changed
 */
static void Logging_Manual_Trigger_Test_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	bool_t trigger_log_request;
    manual_log_trigger->Check_Out( &trigger_log_request );
	if ( trigger_log_request == TRUE )
	{
		trigger_log_request = 0U;
		manual_log_trigger->Check_In( trigger_log_request );
		log_dci_handler->Trigger();
	}
}

/*
 * brief                    Interval based logging CR Task
 * note                     Monitoring the interval logging dcid to change the interval logging time if we change the interval logging time.
 */
static void Logging_Interval_Trigger_Test_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	uint32_t interval_value = 0U;
	static uint32_t last_interval_value = 0;

	interval_logging->Check_Out( interval_value );

	if( last_interval_value != interval_value )
	{
		if ( interval_value > 0U )
		{
			/* Start interval based logging */
			log_dci_handler->Start_Interval( interval_value );
		}
		else
		{
			/* Stop interval based logging */
			log_dci_handler->Stop_Interval();
		}
	}
	last_interval_value = interval_value;
}
/*If only one flash memory is used, a common erase finish callback function can be used */

#endif /* ( LOGGING_SUPPORT )*/