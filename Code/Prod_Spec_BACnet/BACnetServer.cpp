/**
 **********************************************************************************************
 * @file            BACnetServer.cpp  product specific initialization
 *
 * @brief           Product Specific initialization
 * @details         This file shall include the initialization routines specific to BACNET
 * component.
 *
 *
 * @copyright       2021 Eaton Corporation. All Rights Reserved.
 * @note            Eaton Corporation claims proprietary rights to the material disclosed
 *                  here on.  This technical information may not be reproduced or used
 *                  without direct written permission from Eaton Corporation.
 **********************************************************************************************
 */
/*
 *****************************************************************************************
 *		Include Files
 *****************************************************************************************
 */
#include "vsbhp.h"
#include "BACnetServer.h"
#include "BACnetStackInterface.h"
#include "BACnet_DCI.h"
#include "Change_Tracker.h"
#include "DevObject.h"
#include "BACnetDCIInterface.h"
#include "core.h"
#include "string.h"
#include "Prod_Spec_LTK_ESP32.h"
/*
 *****************************************************************************************
 *		Global Variables
 *****************************************************************************************
 */
/* Extern First */
extern bool_t isSoleMaster;
extern uint8_t protocolOperational;
extern uint8_t monitorTimeout;
extern uint8_t BACnetIPForeignDeviceEnabled;

// SA Bus Fault Code param is decided to be removed as discussed in DH1-61
// extern DCI_Owner* SABus_fault_code_owner;
extern bool_t Slave_SABus_CommTOut_flag;
extern uint8_t ubRS485Comm;
extern BACnet_DCI* BACnetDCIPtr;

bool_t BACnetServer::m_ip_enable = false;

Change_Tracker* BACnetServer::m_bacnetip_tracker = NULL;
static DCI_Owner* bac_fault_code_owner;
static DCI_Owner* bacnet_ip_comm_timeout_owner;
static DCI_Owner* bac_comm_timeout_owner;
static DCI_Owner* bac_protocol_status_owner;
static DCI_Owner* bacnet_ip_protocol_status_owner;
#ifdef BACNET_IP_SETUP
static DCI_Owner* enable_BACnetIP_owner;
static DCI_Owner* bacnet_ip_bbmd_port_number_address_owner;
static DCI_Owner* bacnet_ip_port_number_owner;
static DCI_Owner* bacnet_ip_bbmd_ip_address_owner;
static DCI_Owner* bacnet_ip_reg_internal_owner;
static DCI_Owner* bacnet_ip_fault_behavior_owner;
#endif
static DCI_Owner* bacnet_ip_foreign_device_owner;
static const uint8_t ForeignDeviceDisabled = 0;
// static const uint8_t ForeignDeviceEnabled = 1;

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */

extern void Init_BACnetDCIInterface( const BACNET_TARGET_INFO_ST_TD* bacnet_target_info );

int32_t Abort_Code_MSV( uint32_t objinst );

/**
 **********************************************************************************************
 * @brief                     Function for BACnetserver
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
BACnetServer::BACnetServer( const BACNET_TARGET_INFO_ST_TD* bacnet_target_info, bool_t ip_enable )
{
	m_ip_enable = ip_enable;
	/*Before calling this, Init, Configure,Enable USART 4 port */
	if ( m_ip_enable == true )
	{
		bacnet_ip_comm_timeout_owner = new DCI_Owner( DCI_BACNET_IP_COMM_TIMEOUT_DCID );
		bacnet_ip_protocol_status_owner = new DCI_Owner( DCI_BACNET_IP_PROTOCOL_STATUS_DCID );
#ifdef BACNET_IP_SETUP
		enable_BACnetIP_owner = new DCI_Owner( DCI_ENABLE_BACNET_IP_DCID );
		bacnet_ip_bbmd_port_number_address_owner = new DCI_Owner( DCI_BACNET_IP_BBMD_PORT_DCID );
		bacnet_ip_port_number_owner = new DCI_Owner( DCI_BACNET_IP_UPD_PORT_NUM_DCID );
		bacnet_ip_bbmd_ip_address_owner = new DCI_Owner( DCI_BACNET_IP_BBMD_IP_DCID );
		bacnet_ip_reg_internal_owner = new DCI_Owner( DCI_BACNET_IP_REGISTRATION_INTERVAL_DCID );
		bacnet_ip_fault_behavior_owner = new DCI_Owner( DCI_BACNET_IP_FAULT_BEHAVIOR_DCID );
#endif
		bacnet_ip_foreign_device_owner = new DCI_Owner( DCI_BACNET_IP_FOREIGN_DEVICE_DCID );
	}
	bac_protocol_status_owner = new DCI_Owner( DCI_BACNET_MSTP_PROTOCOL_STATUS_DCID );
	bac_fault_code_owner = new DCI_Owner( DCI_BACNET_FAULT_CODE_DCID );
	bac_comm_timeout_owner = new DCI_Owner( DCI_BACNET_MSTP_COMM_TIMEOUT_DCID );

	Init_BACnetDCIInterface( bacnet_target_info );
	createBACnetObjects( bacnet_target_info, Abort_Code_MSV );
	createBACnetCOVList();
	m_BACnet_Server_Patron = new DCI_Patron( FALSE );
	m_BACnet_Server_source_id = DCI_SOURCE_IDS_Get();
	BF_Lib::Timers* timerForFrMain = new BF_Lib::Timers( BACnetfrMainStatic, NULL,
														 BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY,
														 "BACnet Timer For Main" );
	timerForFrMain->Start( FR_MAIN_UPDATE_INTERVAL, TRUE );

	BF_Lib::Timers* timerForFrWork = new BF_Lib::Timers( BACnetfrWorkStatic, NULL,
														 BF_Lib::Timers::TIMERS_DEFAULT_CR_PRIORITY,
														 "BACnet Timer For Work" );
	timerForFrWork->Start( FR_WORK_UPDATE_INTERVAL, TRUE );
	m_bacnetip_tracker = new Change_Tracker( &BACNet_IP_Callback_Static,
											 reinterpret_cast<Change_Tracker::cback_param_t>( this ), true );

}

/**
 **********************************************************************************************
 * @brief                     Function for BACnetfrMain
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void BACnetServer::BACnetfrMain()
{
	uint8_t protoStatus;
	uint16_t commTimeout = 0;
	uint16_t commTimeoutIP = 0;

	static uint16_t expectedNumberOfHits;
	static uint32_t actualNumofHits;

	enum
	{
		FaultBehaviorInFBControl = 0,
		FaultBehaviorInAllControl,
	};

	enum
	{
		Stopped = 0,
		Operational,
		Faulted
	};

	enum
	{
		None = 0,
		Sole_Master
	};

	// #define FaultBehaviorInFBControl 0

#define FIELDBUS_WARNING_COAST 4

	// enable_BACnetIP_owner->Check_Out(bacnet_ip_enable);
	if ( m_ip_enable == true )
	{
		m_bacnetip_tracker->Track_ID( DCI_BACNET_IP_FOREIGN_DEVICE_DCID );
		bacnet_ip_foreign_device_owner->Check_Out( BACnetIPForeignDeviceEnabled );
		bacnet_ip_comm_timeout_owner->Check_Out( commTimeoutIP );
		if ( protocolOperational )
		{
			if ( commTimeoutIP && !monitorTimeout )	// commTimeout is non zero then only report fault else just ignore
			{
				expectedNumberOfHits = commTimeoutIP / FR_MAIN_UPDATE_INTERVAL;	// we are supposed to come he every 10
																				// mSec
				if ( actualNumofHits++ >= expectedNumberOfHits )// if timeout is given generate fault only after timeout
																// is reached
				{
					// DCI_ubBACnetProtocolStatus 0=Stopped;1=Operational;2=Faulted;
					protoStatus = Faulted;
					bacnet_ip_protocol_status_owner->Check_In( protoStatus );
					// 0=None;1=Sole Master;
					// protoStatus = 1;
					// bac_fault_code_owner->Check_In(protoStatus);
					if ( 4294967295 == actualNumofHits )// in case of uint32_t overflow restart
					{
						actualNumofHits = expectedNumberOfHits;
					}
				}
			}
			else// We are Good
			{
				expectedNumberOfHits = 0;
				actualNumofHits = 0;
				monitorTimeout = 0;
				// 0=Stopped;1=Operational;2=Faulted;
				protoStatus = Operational;
				bacnet_ip_protocol_status_owner->Check_In( protoStatus );
				// 0=None;1=Sole Master;
				// protoStatus = 0;
				// bac_fault_code_owner->Check_In(protoStatus);
				// Fault_Manager:: Clr(FC_FIELDBUS_BACNETIP_FAULT);
			}
		}
	}
	else if ( 2U == ubRS485Comm )	// Nishchal: 2U is SA Bus
	{
		if ( true == Slave_SABus_CommTOut_flag )
		{	// Nishchal: SA Bus Min Comm Timeout value is 3 Sec, as Read Drive Block Data req comes every 3 sec.
			// SABus_comm_timeout_owner->Check_Out(commTimeout);
			if ( commTimeout )
			{
				// Misc_Func_Unit::Set_Fault_Stop_Mode(FC_FIELDBUS_SABUS_FAULT, COMM_STOP_MODE);
				expectedNumberOfHits = commTimeout / FR_MAIN_UPDATE_INTERVAL;	// we are supposed to come he every 10
																				// mSec
				if ( actualNumofHits++ >= expectedNumberOfHits )// if timeout is given generate fault only after timeout
																// is reached
				{
					// DCI_ubBACnetProtocolStatus 0=Stopped;1=Operational;2=Faulted;
					protoStatus = Faulted;
					// SABus_protocol_status_owner->Check_In(protoStatus);

					// SA Bus Fault Code param is decided to be removed as discussed in DH1-61
					// 0=None;1=Sole Master;
					// protoStatus = Sole_Master;

					if ( 4294967295 == actualNumofHits )// in case of uint32_t overflow restart
					{
						actualNumofHits = expectedNumberOfHits;
					}
				}
			}
		}
		else
		{}
	}
	else if ( isSoleMaster )
	{
		bac_comm_timeout_owner->Check_Out( commTimeout );
		// Misc_Func_Unit::Set_Fault_Stop_Mode(FC_FIELDBUS_MSTP_FAULT, COMM_STOP_MODE);
		if ( commTimeout )	// commTimeout is non zero then only report fault else just ignore
		{
			expectedNumberOfHits = commTimeout / FR_MAIN_UPDATE_INTERVAL;	// we are supposed to come he every 10 mSec
			if ( actualNumofHits++ >= expectedNumberOfHits )// if timeout is given generate fault only after timeout is
															// reached
			{
				// DCI_ubBACnetProtocolStatus 0=Stopped;1=Operational;2=Faulted;
				protoStatus = Faulted;
				bac_protocol_status_owner->Check_In( protoStatus );
				// 0=None;1=Sole Master;
				protoStatus = Sole_Master;
				bac_fault_code_owner->Check_In( protoStatus );

				if ( 4294967295 == actualNumofHits )// in case of uint32_t overflow restart
				{
					actualNumofHits = expectedNumberOfHits;
				}
			}
		}
	}
	else// We are Good
	{
		expectedNumberOfHits = 0;
		actualNumofHits = 0;
		// 0=Stopped;1=Operational;2=Faulted;
		protoStatus = Operational;
		bac_protocol_status_owner->Check_In( protoStatus );
		// 0=None;1=Sole Master;
		protoStatus = None;
		bac_fault_code_owner->Check_In( protoStatus );
		// Fault_Manager:: Clr(FC_FIELDBUS_MSTP_FAULT);
	}

	frMain();
}

/**
 **********************************************************************************************
 * @brief                     Function for BACnetFrWork
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void BACnetServer::BACnetFrWork()
{
	frWork( FR_WORK_UPDATE_INTERVAL );
}

/**
 **********************************************************************************************
 * @brief                     Function for BACNet_IP_Callback
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void BACnetServer::BACNet_IP_Callback( DCI_ID_TD dci_id )
{
	// uint8_t result;
	if ( dci_id == DCI_BACNET_IP_FOREIGN_DEVICE_DCID )
	{
		{
			bacnet_ip_foreign_device_owner->Check_Out( BACnetIPForeignDeviceEnabled );
			// if( result == DCI_ERR_NO_ERROR )
			{
				if ( BACnetIPForeignDeviceEnabled == ForeignDeviceDisabled )
				{
					BACnetDCIPtr->Read_Param( DCI_BACNET_IP_BBMD_IP_DCID,
											  ( uint8_t* )&BACnetIPForeignDeviceEnabled, DCI_ACCESS_GET_RAM_CMD );
				}
			}
		}
	}
}

/**
 **********************************************************************************************
 * @brief                     Function for Abort_Code_MSV
 * @param[in] void            none
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
int32_t Abort_Code_MSV( uint32_t objinst )
{
	return ( 0 );
}
