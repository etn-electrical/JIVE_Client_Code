/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Breaker
 *
 * File name        : Device_IOT_DCI_Interface.cpp
 *
 * Author           : Ahmed El-Gayyar
 *
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 06/25/2021
 *
 ****************************************************************************/
 
#include "Includes.h"
#include "DCI_Data.h"
#include "StdLib_MV.h"
#include "Babelfish_Assert.h"

#include "Breaker.h"
#include "LED_Breaker.h"
#include "EventManager.h"
#include "KeyManager.h"
#include "DeviceGPIO.h"


#include "Includes.h"
#include "IOT_DCI_Config.h"
#include "IOT_DCI_Data.h"
#include "String_Sanitizer.hpp"
#include "String_Sanitize_Whitelists.h"
#include "StdLib_MV.h"
#include "Ram.h"

static const char *TAG = "IOT_DCI_Interface";
extern metroData_t g_cloudTrendingData;

DCI_Owner* m_PrimaryContactStatus;             //tag_value 10083391
DCI_Owner* m_SecondaryContactStatus;           //tag_value 112078
DCI_Owner* m_PathStatus;  						//teg_value 10228020
DCI_Owner* m_PrimaryState;  					//teg_value 10228021
DCI_Owner* m_SecondaryState;  					//teg_value 10228022
DCI_Owner* m_POWER_UP_STATE;                //tag_value 112079

DCI_Owner* m_NetworkSSID;	                //tag_value 112083
DCI_Owner* m_NetworkPassword;	            //tag_value 112084
DCI_Owner* m_WiFiModuleFirmwareVersion;	            //tag_value 112086
DCI_Owner* m_MacAddress;	                //tag_value 112094

DCI_Owner* m_ResetBreaker;	                //tag_value 112105

DCI_Owner* m_ProductName;	                //tag_value 10014222  //Device Name

DCI_Owner* m_ProgrammedPartNumber;         //tag_value 10034175 //Programmed new part number

DCI_Owner* m_BuildType;                     //tag_value 10034176 //Special build Name



DCI_Owner* m_ConnectionString;

DCI_Owner* m_DeviceUUID;

DCI_Owner* m_VoltageL1;						//tag_value 10083394 //Voltage Line 1

DCI_Owner* m_VoltageL2;						//tag_value 10083395 //Voltage Line 2

DCI_Owner* m_CurrentL1;						//tag_value 10083396 //Current Line 1

DCI_Owner* m_CurrentL2;						//tag_value 10083397 //Current Line 2

DCI_Owner* m_ActivePowerL1;					//tag_value 10134540 //ActivePower 1

DCI_Owner* m_ActivePowerL2;					//tag_value 10134543 //ActivePower 2

DCI_Owner* m_ReactivePowerL1;				//tag_value 10134541 //ReactivePower 1

DCI_Owner* m_ReactivePowerL2;				//tag_value 10134544 //ReactivePower 2

DCI_Owner* m_ApparentPowerL1;				//tag_value 10134542 //ApparentPower Line 1

DCI_Owner* m_ApparentPowerL2;				//tag_value 10134545 //ApparentPower Line 2

DCI_Owner* m_ActiveEnergyL1;				//tag_value 10134546 //ActiveEnergy Line 1

DCI_Owner* m_ReactiveEnergyL1;				//tag_value 10134547 //ReactiveEnergy Line 1

DCI_Owner* m_ApparentEnergyL1;				//tag_value 10134548 //ApparentEnergy Line 1

DCI_Owner* m_ActiveEnergyL2;				//tag_value 10134549 //ActiveEnergy Line 2

DCI_Owner* m_ReactiveEnergyL2;				//tag_value 10134550 //ReactiveEnergy Line 2

DCI_Owner* m_ApparentEnergyL2;				//tag_value 10134551 //ApparentEnergy Line 2

DCI_Owner* m_ReverseActiveEnergyL1;				//tag_value 10153974 //ReverseActiveEnergy Line 1

DCI_Owner* m_ReverseReactiveEnergyL1;				//tag_value10153975//ReverseReactiveEnergy Line 1

DCI_Owner* m_ReverseApparentEnergyL1;				//tag_value 10153976 //ReverseApparentEnergy Line 1

DCI_Owner* m_ReverseActiveEnergyL2;				//tag_value 10153977 //ReverseActiveEnergy Line 2

DCI_Owner* m_ReverseReactiveEnergyL2;				//tag_value 10153978 //ReverseReactiveEnergy Line 2

DCI_Owner* m_ReverseApparentEnergyL2;				//tag_value 10153979 //ReverseApparentEnergy Line 2

DCI_Owner* m_PowerFactorL1;					//tag_value 10083402 //PowerFactor Line 1

DCI_Owner* m_PowerFactorL2;					//tag_value 10083403 //PowerFactor Line 2

DCI_Owner* m_FrequencyL1;					//tag_value 10134552 //Frequency Line 1

DCI_Owner* m_FrequencyL2;					//tag_value 10134553 //Frequency Line 2

DCI_Owner* m_UnicastPrimaryUDPKey;  			//tag_value 10186907 UnicastPrimaryUDPKey
DCI_Owner* m_GetUnicastPrimaryUDPKey;			//tag_value 10186911 GetUnicastPrimaryUDPKey
DCI_Owner* m_BroadcastPrimaryUDPKey;			//tag_value 10186909 BroadcastPrimaryUDPKey
DCI_Owner* m_GetBroadcastPrimaryUDPKey;			//tag_value 10186910 GetBroadcastPrimaryUDPKey
DCI_Owner* m_UnicastSecondaryUDPKey;			//tag_value 10192325 UnicastSecondaryUDPKey
DCI_Owner* m_GetUnicastSecondaryUDPKey;			//tag_value 10192326 GetUnicastSecondaryUDPKey
DCI_Owner* m_BroadcastSecondaryUDPKey;			//tag_value 10192327 BroadcastSecondaryUDPKey
DCI_Owner* m_GetBroadcastSecondaryUDPKey;		//tag_value 10192328 GetBroadcastSecondaryUDPKey
DCI_Owner* m_ErrorLogData;						//tag_value 10083415 Error log Data


extern DRAM_ATTR Device_Info_str DeviceInfo;

void DCI_Update_ErroLogUpdate(const char * ErrorLog)
{

	m_ErrorLogData->Check_In((DCI_DATA_PASS_TD *) ErrorLog );
	//ets_printf("%s \n", ErrorLog);
	//format for now
	// Send To Cloud Breaker Turns ON 40 at 1677284584   Fri Feb 24 19:40:57 2023
}

void DCI_Update_VoltageL1()
{
	static float PrevVoltageL1 = 0;

	if (g_cloudTrendingData.rmsvoltage[0] != PrevVoltageL1)
	{
		PrevVoltageL1 = g_cloudTrendingData.rmsvoltage[0];

		m_VoltageL1->Check_In((DCI_DATA_PASS_TD *)&PrevVoltageL1);
	}
}


void DCI_Update_VoltageL2()
{
	static float PrevVoltageL2 = 0;

	if (g_cloudTrendingData.rmsvoltage[1] != PrevVoltageL2)
	{
		PrevVoltageL2 = g_cloudTrendingData.rmsvoltage[1];

		m_VoltageL2->Check_In((DCI_DATA_PASS_TD *)&PrevVoltageL2);
	}
}


void DCI_Update_CurrentL1()
{
	static float PrevCurrentL1 = 0;

	if (g_cloudTrendingData.rmscurrent[0] != PrevCurrentL1)
	{
		PrevCurrentL1 = g_cloudTrendingData.rmscurrent[0];

		m_CurrentL1->Check_In((DCI_DATA_PASS_TD *)&PrevCurrentL1);
	}
}


void DCI_Update_CurrentL2()
{
	static float PrevCurrentL2 = 0;

	if (g_cloudTrendingData.rmscurrent[1] != PrevCurrentL2)
	{
		PrevCurrentL2 = g_cloudTrendingData.rmscurrent[1];

		m_CurrentL2->Check_In((DCI_DATA_PASS_TD *)&PrevCurrentL2);
//	            ets_printf("CurrentL2 %f \n",PrevCurrentL2);
	}
}

void DCI_Update_ActivePowerL1()
{
	static float PrevActivePowerL1 = 0;

	if (g_cloudTrendingData.powerActive[0] != PrevActivePowerL1)
	{
			PrevActivePowerL1 = g_cloudTrendingData.powerActive[0];

			m_ActivePowerL1->Check_In((DCI_DATA_PASS_TD *)&PrevActivePowerL1);
//	            ets_printf("ActivePowerL1 %f \n",PrevActivePowerL1);
	}
}

void DCI_Update_ActivePowerL2()
{
	static float PrevActivePowerL2 = 0;

	if (g_cloudTrendingData.powerActive[1] != PrevActivePowerL2)
	{
		PrevActivePowerL2 = g_cloudTrendingData.powerActive[1];

		m_ActivePowerL2->Check_In((DCI_DATA_PASS_TD *)&PrevActivePowerL2);
	}
}


void DCI_Update_ReactivePowerL1()
{
	static float PrevReactivePowerL1 = 0;

	if (g_cloudTrendingData.powerReactive[0] != PrevReactivePowerL1)
	{
		PrevReactivePowerL1 = g_cloudTrendingData.powerReactive[0];

		m_ReactivePowerL1->Check_In((DCI_DATA_PASS_TD *)&PrevReactivePowerL1);
	}
}

void DCI_Update_ReactivePowerL2()
{
	static float PrevReactivePowerL2 = 0;

	if (g_cloudTrendingData.powerReactive[1] != PrevReactivePowerL2)
	{
		PrevReactivePowerL2 = g_cloudTrendingData.powerReactive[1];
		m_ReactivePowerL2->Check_In((DCI_DATA_PASS_TD *)&PrevReactivePowerL2);
	}
}

void DCI_Update_ApparentPowerL1()
{
	static float PrevApparentPowerL1 = 0;

	if (g_cloudTrendingData.powerApparent[0] != PrevApparentPowerL1)
	{
		PrevApparentPowerL1 = g_cloudTrendingData.powerApparent[0];

		m_ApparentPowerL1->Check_In((DCI_DATA_PASS_TD *)&PrevApparentPowerL1);
	}
}


void DCI_Update_ApparentPowerL2()
{
	static float PrevApparentPowerL2 = 0;

	if (g_cloudTrendingData.powerApparent[1] != PrevApparentPowerL2)
	{
		PrevApparentPowerL2 = g_cloudTrendingData.powerApparent[1];

		m_ApparentPowerL2->Check_In((DCI_DATA_PASS_TD *)&PrevApparentPowerL2);
	}
}


void DCI_Update_PowerFactorL1()
{
	static float PrevPowerFactorL1 = 0;

	if (g_cloudTrendingData.powerfactor[0] != PrevPowerFactorL1)
	{
		PrevPowerFactorL1 = g_cloudTrendingData.powerfactor[0];

		m_PowerFactorL1->Check_In((DCI_DATA_PASS_TD *)&PrevPowerFactorL1);

	}
}

void DCI_Update_PowerFactorL2()
{
	static float PrevPowerFactorL2 = 0;

	if (g_cloudTrendingData.powerfactor[1] != PrevPowerFactorL2)
	{
		PrevPowerFactorL2 = g_cloudTrendingData.powerfactor[1];

		m_PowerFactorL2->Check_In((DCI_DATA_PASS_TD *)&PrevPowerFactorL2);
	}
}


void DCI_Update_FrequencyL1()
{
	static float PrevFrequencyL1 = 0;

	if (g_cloudTrendingData.linefrequency[0] != PrevFrequencyL1)
	{
		PrevFrequencyL1 = g_cloudTrendingData.linefrequency[0];

		m_FrequencyL1->Check_In((DCI_DATA_PASS_TD *)&PrevFrequencyL1);
	}
}

void DCI_Update_FrequencyL2()
{
	static float PrevFrequencyL2 = 0;

	if (g_cloudTrendingData.linefrequency[1] != PrevFrequencyL2)
	{
		PrevFrequencyL2 = g_cloudTrendingData.linefrequency[1];

		m_FrequencyL2->Check_In((DCI_DATA_PASS_TD *)&PrevFrequencyL2);
	}
}


void DCI_Update_ActiveEnergyL1()
{
	static double PrevActiveEnergyL1 = 0;

	if (g_cloudTrendingData.energyActive[0] != PrevActiveEnergyL1)
	{
		PrevActiveEnergyL1 = g_cloudTrendingData.energyActive[0];

		m_ActiveEnergyL1->Check_In((DCI_DATA_PASS_TD *)&PrevActiveEnergyL1);
	}
}


void DCI_Update_ActiveEnergyL2()
{
	static double PrevActiveEnergyL2 = 0;

	if (g_cloudTrendingData.energyActive[1] != PrevActiveEnergyL2)
	{
		PrevActiveEnergyL2 = g_cloudTrendingData.energyActive[1];

		m_ActiveEnergyL2->Check_In((DCI_DATA_PASS_TD *)&PrevActiveEnergyL2);
	}
}

void DCI_Update_ReactiveEnergyL1()
{
	static double PrevReactiveEnergyL1 = 0;

	if (g_cloudTrendingData.energyReactive[0] != PrevReactiveEnergyL1)
	{
		PrevReactiveEnergyL1 = g_cloudTrendingData.energyReactive[0];

		m_ReactiveEnergyL1->Check_In((DCI_DATA_PASS_TD *)&PrevReactiveEnergyL1);
	}
}

void DCI_Update_ReactiveEnergyL2()
{
	static double PrevReactiveEnergyL2 = 0;

	if (g_cloudTrendingData.energyReactive[1] != PrevReactiveEnergyL2)
	{
		PrevReactiveEnergyL2 = g_cloudTrendingData.energyReactive[1];

		m_ReactiveEnergyL2->Check_In((DCI_DATA_PASS_TD *)&PrevReactiveEnergyL2);
	}
}

void DCI_Update_ApparentEnergyL1()
{
	static double PrevApparentEnergyL1 = 0;

	if (g_cloudTrendingData.energyApparent[0] != PrevApparentEnergyL1)
	{
		PrevApparentEnergyL1 = g_cloudTrendingData.energyApparent[0];

		m_ApparentEnergyL1->Check_In((DCI_DATA_PASS_TD *)&PrevApparentEnergyL1);
	}
}


void DCI_Update_ApparentEnergyL2()
{
	static double PrevApparentEnergyL2 = 0;

	if (g_cloudTrendingData.energyApparent[1] != PrevApparentEnergyL2)
	{
		PrevApparentEnergyL2 = g_cloudTrendingData.energyApparent[1];

		m_ApparentEnergyL2->Check_In((DCI_DATA_PASS_TD *)&PrevApparentEnergyL2);
	}
}

void DCI_Update_RevActiveEnergyL1()
{
	static double PrevRevActiveEnergyL1 = 0;

	if (g_cloudTrendingData.energyRevActive[0] != PrevRevActiveEnergyL1)
	{
		PrevRevActiveEnergyL1 = g_cloudTrendingData.energyRevActive[0];

		m_ReverseActiveEnergyL1->Check_In((DCI_DATA_PASS_TD *)&PrevRevActiveEnergyL1);
	}
}


void DCI_Update_RevActiveEnergyL2()
{
	static double PrevRevActiveEnergyL2 = 0;

	if (g_cloudTrendingData.energyRevActive[1] != PrevRevActiveEnergyL2)
	{
		PrevRevActiveEnergyL2 = g_cloudTrendingData.energyRevActive[1];

		m_ReverseActiveEnergyL2->Check_In((DCI_DATA_PASS_TD *)&PrevRevActiveEnergyL2);
	}
}

void DCI_Update_RevReactiveEnergyL1()
{
	static double PrevRevReactiveEnergyL1 = 0;

	if (g_cloudTrendingData.energyRevReactive[0] != PrevRevReactiveEnergyL1)
	{
		PrevRevReactiveEnergyL1 = g_cloudTrendingData.energyRevReactive[0];

		m_ReverseReactiveEnergyL1->Check_In((DCI_DATA_PASS_TD *)&PrevRevReactiveEnergyL1);
	}
}

void DCI_Update_RevReactiveEnergyL2()
{
	static double PrevRevReactiveEnergyL2 = 0;

	if (g_cloudTrendingData.energyRevReactive[1] != PrevRevReactiveEnergyL2)
	{
		PrevRevReactiveEnergyL2 = g_cloudTrendingData.energyRevReactive[1];

		m_ReverseReactiveEnergyL2->Check_In((DCI_DATA_PASS_TD *)&PrevRevReactiveEnergyL2);
	}
}

void DCI_Update_RevApparentEnergyL1()
{
	static double PrevRevApparentEnergyL1 = 0;

	if (g_cloudTrendingData.energyRevApparent[0] != PrevRevApparentEnergyL1)
	{
		PrevRevApparentEnergyL1 = g_cloudTrendingData.energyRevApparent[0];

		m_ReverseApparentEnergyL1->Check_In((DCI_DATA_PASS_TD *)&PrevRevApparentEnergyL1);
	}
}


void DCI_Update_RevApparentEnergyL2()
{
	static double PrevRevApparentEnergyL2 = 0;

	if (g_cloudTrendingData.energyRevApparent[1] != PrevRevApparentEnergyL2)
	{
		PrevRevApparentEnergyL2 = g_cloudTrendingData.energyRevApparent[1];

		m_ReverseApparentEnergyL2->Check_In((DCI_DATA_PASS_TD *)&PrevRevApparentEnergyL2);
	}
}

static void SecondaryContactStatusCallback_Static( void )
{
	bool LoadStatus;

	m_SecondaryContactStatus->Check_Out((DCI_DATA_PASS_TD *)&LoadStatus);
    ets_printf("m_SecondaryContactStatus %d\n",LoadStatus);

#if defined PREVENT_REQUESTING_CURRENT_STATE
    if (LoadStatus != DeviceInfo.SecondaryContactState)
#endif
    {
//        RF_ToggleBreakerState();
		if (LoadStatus)
		{
			TurnBreakerOn();
		}
		else
		{
			TurnBreakerOff();
		}
    }
}

static void SecondaryStateCallback_Static( void )
{
	uint8_t LoadStatus;

	m_SecondaryState->Check_Out((DCI_DATA_PASS_TD *)&LoadStatus);
    ets_printf("m_SecondaryState %d\n",LoadStatus);

    DeviceInfo.SecondaryContactState = (ss_status_t)LoadStatus;

#if defined PREVENT_REQUESTING_CURRENT_STATE
    if (LoadStatus != DeviceInfo.SecondaryContactState)
#endif
    {
//        RF_ToggleBreakerState();
        if (LoadStatus == SS_CLOSED)
        {
            TurnBreakerOn();
        }
        else if (LoadStatus == SS_OPEN)
        {
        	TurnBreakerOff();
        }
    }
}

void DCI_UpdateSecondaryContactStatus(void)
{
    static ss_status_t PrevDeviceState = SS_UNKNOWN;
    bool BreakerState;
    
	if (DeviceInfo.SecondaryContactState != PrevDeviceState)
	{
		PrevDeviceState = DeviceInfo.SecondaryContactState;
		if(   (DeviceInfo.SecondaryContactState == SS_OPEN)
			||(DeviceInfo.SecondaryContactState == SS_OPENING)
			||(DeviceInfo.SecondaryContactState == SS_STUCK_OPEN))
		{
			BreakerState = false;
		}
		else
		{
			BreakerState = true;
		}

		m_SecondaryContactStatus->Check_In((DCI_DATA_PASS_TD *)&BreakerState);
		//ets_printf("m_LOAD_STATUS Saved vlaue %d\n",BreakerState);

		//m_LOAD_STATUS->Check_Out((DCI_DATA_PASS_TD *)&BreakerState);
		//ets_printf("m_LOAD_STATUSRead Saved vlaue %d\n",BreakerState);
	}
}

void DCI_UpdateSecondaryState(void)
{
    static ss_status_t PrevDeviceState = SS_UNKNOWN;
    uint8_t BreakerState;

    //Because this is writable we may change this value and not realize it.
    //interesting enough if we check the dci it does not seem to call the callback function.
	if (DeviceInfo.SecondaryContactState != PrevDeviceState)
	{
		PrevDeviceState = DeviceInfo.SecondaryContactState;
		BreakerState = (uint8_t)PrevDeviceState;

		m_SecondaryState->Check_In((DCI_DATA_PASS_TD *)&BreakerState);
		//ets_printf("m_LOAD_STATUS Saved vlaue %d\n",BreakerState);

		//m_LOAD_STATUS->Check_Out((DCI_DATA_PASS_TD *)&BreakerState);
		//ets_printf("m_LOAD_STATUSRead Saved vlaue %d\n",BreakerState);
	}
}

void DCI_UpdatePrimaryContactStatus(void)
{
    static pswitch_status_t PrevDeviceState = SW_UNKNOWN;
    bool BreakerState;
    
	if (DeviceInfo.PrimaryContactState != PrevDeviceState)
	{
		PrevDeviceState = DeviceInfo.PrimaryContactState;
		if(   (DeviceInfo.PrimaryContactState == SW_OPEN)
		    ||(DeviceInfo.PrimaryContactState == SW_TRIP))
		{
			BreakerState = false;
		}
		else
		{
			BreakerState = true;
		}

		m_PrimaryContactStatus->Check_In((DCI_DATA_PASS_TD *)&BreakerState);
		//ets_printf("m_LOAD_STATUS Saved vlaue %d\n",BreakerState);

		//m_LOAD_STATUS->Check_Out((DCI_DATA_PASS_TD *)&BreakerState);
		//ets_printf("m_LOAD_STATUSRead Saved vlaue %d\n",BreakerState);
	}
}

void DCI_UpdatePrimaryState(void)
{
    static pswitch_status_t PrevDeviceState = SW_UNKNOWN;
    uint8_t BreakerState;

	if (DeviceInfo.PrimaryContactState != PrevDeviceState)
	{
		PrevDeviceState = DeviceInfo.PrimaryContactState;
		BreakerState = (uint8_t)PrevDeviceState;

		m_PrimaryState->Check_In((DCI_DATA_PASS_TD *)&BreakerState);
		//ets_printf("m_LOAD_STATUS Saved vlaue %d\n",BreakerState);

		//m_LOAD_STATUS->Check_Out((DCI_DATA_PASS_TD *)&BreakerState);
		//ets_printf("m_LOAD_STATUSRead Saved vlaue %d\n",BreakerState);
	}
}

void DCI_UpdatePathStatus(void)
{
    static fdbk_status_t PrevDeviceState = PATH_UNKNOWN;
    uint8_t BreakerState;

	if (DeviceInfo.PathStatus != PrevDeviceState)
	{
		PrevDeviceState = DeviceInfo.PathStatus;
		BreakerState = (uint8_t)PrevDeviceState;

		m_PathStatus->Check_In((DCI_DATA_PASS_TD *)&BreakerState);

		//ets_printf("m_LOAD_STATUS Saved vlaue %d\n",BreakerState);

		//m_LOAD_STATUS->Check_Out((DCI_DATA_PASS_TD *)&BreakerState);
		//ets_printf("m_LOAD_STATUSRead Saved vlaue %d\n",BreakerState);
	}
}

void DCI_Update_SSID()
{
	m_NetworkSSID->Check_In( DeviceInfo.Ssid );

#if 0    
    m_NetworkSSID->Check_Out(prod_list);
    ets_printf("ssid %s\n",prod_list);
#endif    

}

void DCI_Update_DevicePassWord()
{
	m_NetworkPassword->Check_In( DeviceInfo.PassWord );

#if 0    
    m_NetworkPassword->Check_Out(prod_list);
    ets_printf("PassWord %s\n",prod_list);
#endif    

}


void GetDevicePassWord(void)
{
    memset ((char *)&DeviceInfo.PassWord,0, sizeof (DeviceInfo.PassWord));
    m_NetworkPassword->Check_Out( DeviceInfo.PassWord );
    
}
void GetDeviceSsid(void)
{
    memset ((char *)&DeviceInfo.Ssid,0, sizeof (DeviceInfo.Ssid));
    m_NetworkSSID->Check_Out( DeviceInfo.Ssid );
}


void DCI_Update_DeviceMacAdd()
{
    uint8_t eth_mac[6];

    
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
    {       
        snprintf((char *)&DeviceInfo.MacAddress, sizeof (DeviceInfo.MacAddress), "%02X:%02X:%02X:%02X:%02X:%02X",
              eth_mac[0], eth_mac[1], eth_mac[2],eth_mac[3],eth_mac[4],eth_mac[5]);
    }

	m_MacAddress->Check_In( DeviceInfo.MacAddress );

#if 0    
    m_MacAddress->Check_Out(prod_list);
    ets_printf("MacAddress %s\n",prod_list);
#endif    

}

void DCI_Update_FirmWarVer(void)
{
    uint64_t ver = MY_FIRMWARE_VERSION;

	m_WiFiModuleFirmwareVersion->Check_In( (DCI_DATA_PASS_TD *)&(ver) );

#if 0    
	uint64_t tempver = MY_FIRMWARE_VERSION;
    m_FirmwareVersion->Check_Out((DCI_DATA_PASS_TD *)&tempver);
    ets_printf("FirmWarever %x\n",tempver);
#endif    

}


static void BreakerResetCallback_Static( void )
{
	uint32_t RestValue;
    
    m_ResetBreaker->Check_Out((DCI_DATA_PASS_TD *)&RestValue);
    
    
    //ets_printf("ResetValue %d\n",RestValue);
    
    if (RestValue == 0xFF)
    {
        //Manfacutre Reset
        ResetDevice(MANUFACTURING_RESET, true);
    }
    else
    {
        if (RestValue == 0x7F)
        {
            //Provisining reset
            ResetDevice(PROVISIONING_RESET, true);
        }
    }
}

static void BreakerNameCallback_Static( void )
{
	
    
    memset (&DeviceInfo.DeviceName,0, sizeof (DeviceInfo.DeviceName));
    m_ProductName->Check_Out((DCI_DATA_PASS_TD *)&DeviceInfo.DeviceName);
    DeviceInfo.DeviceNameChanged = true;
    ets_printf("Breaker Name Read  %s\n",DeviceInfo.DeviceName);
    
}

static void UnicastPrimaryUDPKeyCallback_Static( void )
{
	unsigned char UnicastPrimaryUDPKey[CRYPTO_HASH_LENGTH];

	m_UnicastPrimaryUDPKey->Check_Out((DCI_DATA_PASS_TD *)UnicastPrimaryUDPKey);
    ets_printf("m_UnicastPrimaryUDPKey %d\n",UnicastPrimaryUDPKey);


    if(memcmp(UnicastPrimaryUDPKey, DeviceInfo.BroadcastSecondaryUDPKey, CRYPTO_HASH_LENGTH) != 0 )
    {
        memcpy(DeviceInfo.BroadcastSecondaryUDPKey, UnicastPrimaryUDPKey, CRYPTO_HASH_LENGTH);
    }

}


static void UnicastSecondaryUDPKeyCallback_Static( void )
{
    unsigned char UnicastSecondaryUDPKey[CRYPTO_HASH_LENGTH];

	m_UnicastSecondaryUDPKey->Check_Out((DCI_DATA_PASS_TD *)UnicastSecondaryUDPKey);
    ets_printf("m_UnicastSecondaryUDPKey %d\n",UnicastSecondaryUDPKey);


    if(memcmp(UnicastSecondaryUDPKey, DeviceInfo.BroadcastSecondaryUDPKey, CRYPTO_HASH_LENGTH) != 0 )
    {
        memcpy(DeviceInfo.BroadcastSecondaryUDPKey, UnicastSecondaryUDPKey, CRYPTO_HASH_LENGTH);
    }

}


static void BroadcastPrimaryUDPKeyCallback_Static( void )
{
    unsigned char BroadcastPrimaryUDPKey[CRYPTO_HASH_LENGTH];

	m_BroadcastPrimaryUDPKey->Check_Out((DCI_DATA_PASS_TD *)BroadcastPrimaryUDPKey);
    ets_printf("m_BroadcastPrimaryUDPKey %d\n",BroadcastPrimaryUDPKey);


    if(memcmp(BroadcastPrimaryUDPKey, DeviceInfo.BroadcastSecondaryUDPKey, CRYPTO_HASH_LENGTH) != 0 )
    {
        memcpy(DeviceInfo.BroadcastSecondaryUDPKey, BroadcastPrimaryUDPKey, CRYPTO_HASH_LENGTH);
    }

}


static void BroadcastSecondaryUDPKeyCallback_Static( void )
{
	unsigned char BroadcastSecondaryUDPKey[CRYPTO_HASH_LENGTH];

	m_BroadcastSecondaryUDPKey->Check_Out((DCI_DATA_PASS_TD *)BroadcastSecondaryUDPKey);
    ets_printf("m_BroadcastSecondaryUDPKey %d\n",BroadcastSecondaryUDPKey);


    if(memcmp(BroadcastSecondaryUDPKey, DeviceInfo.BroadcastSecondaryUDPKey, CRYPTO_HASH_LENGTH) != 0 )
    {
        memcpy(DeviceInfo.BroadcastSecondaryUDPKey, BroadcastSecondaryUDPKey, CRYPTO_HASH_LENGTH);
    }

}


void DCI_UpdateBuildType(void)
{
    m_BuildType->Check_In( DeviceInfo.BuildType );
}

void DCI_UpdateNewProgrammedPartNumber(void)
{
    m_ProgrammedPartNumber->Check_In( DeviceInfo.NewProgramedPartNumber );
}


void BreakerDCIInit( void )
{
	m_ErrorLogData = new DCI_Owner (  DCI_IOT_ErrorLogData_DCID );

	m_VoltageL1 = new DCI_Owner (  DCI_IOT_VoltageL1_DCID );
	m_VoltageL2 = new DCI_Owner (  DCI_IOT_VoltageL2_DCID );
	
	m_CurrentL1 = new DCI_Owner (  DCI_IOT_CurrentL1_DCID );
	m_CurrentL2 = new DCI_Owner (  DCI_IOT_CurrentL2_DCID );

	m_ActivePowerL1 = new DCI_Owner (  DCI_IOT_ActivePowerL1_DCID );
	m_ActivePowerL2 = new DCI_Owner (  DCI_IOT_ActivePowerL2_DCID );

	m_ReactivePowerL1 = new DCI_Owner (  DCI_IOT_ReactivePowerL1_DCID );
	m_ReactivePowerL2 = new DCI_Owner (  DCI_IOT_ReactivePowerL2_DCID );

	m_ApparentPowerL1 = new DCI_Owner (  DCI_IOT_ApparentPowerL1_DCID );
	m_ApparentPowerL2 = new DCI_Owner (  DCI_IOT_ApparentPowerL2_DCID );

	m_ActiveEnergyL1 = new DCI_Owner (  DCI_IOT_ActiveEnergyL1_DCID );
	m_ActiveEnergyL2 = new DCI_Owner (  DCI_IOT_ActiveEnergyL2_DCID );

	m_ReactiveEnergyL1 = new DCI_Owner (  DCI_IOT_ReactiveEnergyL1_DCID );
	m_ReactiveEnergyL2 = new DCI_Owner (  DCI_IOT_ReactiveEnergyL2_DCID );

	m_ApparentEnergyL1 = new DCI_Owner (  DCI_IOT_ApparentEnergyL1_DCID );
	m_ApparentEnergyL2 = new DCI_Owner (  DCI_IOT_ApparentEnergyL2_DCID );

	m_ReverseActiveEnergyL1 = new DCI_Owner (  DCI_IOT_ReverseActiveEnergyL1_DCID );
	m_ReverseActiveEnergyL2 = new DCI_Owner (  DCI_IOT_ReverseActiveEnergyL2_DCID );

	m_ReverseReactiveEnergyL1 = new DCI_Owner (  DCI_IOT_ReverseReactiveEnergyL1_DCID );
	m_ReverseReactiveEnergyL2 = new DCI_Owner (  DCI_IOT_ReverseReactiveEnergyL2_DCID );

	m_ReverseApparentEnergyL1 = new DCI_Owner (  DCI_IOT_ReverseApparentEnergyL1_DCID );
	m_ReverseApparentEnergyL2 = new DCI_Owner (  DCI_IOT_ReverseApparentEnergyL2_DCID );

	m_PowerFactorL1 = new DCI_Owner (  DCI_IOT_PowerFactorL1_DCID );
	m_PowerFactorL2 = new DCI_Owner (  DCI_IOT_PowerFactorL2_DCID );

	m_FrequencyL1 = new DCI_Owner (  DCI_IOT_FrequencyL1_DCID );
	m_FrequencyL2 = new DCI_Owner (  DCI_IOT_FrequencyL2_DCID );

	m_UnicastPrimaryUDPKey        = new DCI_Owner (  DCI_IOT_UNICAST_PRIMARY_UDP_KEY_DCID );
	m_GetUnicastPrimaryUDPKey     = new DCI_Owner (  DCI_IOT_GET_UNICAST_UDP_KEY_DCID );
	m_BroadcastPrimaryUDPKey      = new DCI_Owner (  DCI_IOT_BROADCAST_UDP_PRIMARY_KEY_DCID );
	m_GetBroadcastPrimaryUDPKey   = new DCI_Owner (  DCI_IOT_GET_BROADCAST_UDP_KEY_DCID );
	m_UnicastSecondaryUDPKey      = new DCI_Owner (  DCI_IOT_UNICAST_SECONDARY_UDP_KEY_DCID );
	m_GetUnicastSecondaryUDPKey   = new DCI_Owner (  DCI_IOT_GET_UNICAST_SECONDARY_UDP_KEY_DCID );
	m_BroadcastSecondaryUDPKey    = new DCI_Owner (  DCI_IOT_BROADCAST_SECONDARY_UDP_KEY_DCID );
	m_GetBroadcastSecondaryUDPKey = new DCI_Owner (  DCI_IOT_GET_BROADCAST_SECONDARY_UDP_KEY_DCID );


   	m_PrimaryContactStatus	    = new DCI_Owner (  DCI_IOT_PRIMARY_CONTACT_STATUS_DCID  );
    m_SecondaryContactStatus	= new DCI_Owner (  DCI_IOT_SECONDARY_CONTACT_STATUS_DCID  );

    m_PathStatus				= new DCI_Owner (  DCI_IOT_PATH_STATUS_DCID  );
    m_PrimaryState			    = new DCI_Owner (  DCI_IOT_PRIMARY_STATE_DCID  );
    m_SecondaryState			= new DCI_Owner (  DCI_IOT_SECONDARY_STATE_DCID  );
    
    m_NetworkSSID = new DCI_Owner( DCI_IOT_NetworkSSID_DCID);
    m_NetworkPassword = new DCI_Owner( DCI_IOT_NetworkPassword_DCID);
    m_WiFiModuleFirmwareVersion = new DCI_Owner( DCI_IOT_FirmwareVersion_DCID);
    m_MacAddress = new DCI_Owner( DCI_IOT_MacAddress_DCID);
 
    m_ResetBreaker = new DCI_Owner( DCI_IOT_ResetBreaker_DCID);
    
    m_ProductName = new DCI_Owner( DCI_PRODUCT_NAME_DCID);
    m_ProductName->Check_In((DCI_DATA_PASS_TD *)&DeviceInfo.DeviceName);

    m_ConnectionString = new DCI_Owner( DCI_IOT_CONN_STRING_DCID);
    
    m_DeviceUUID = new DCI_Owner( DCI_IOT_DEVICE_GUID_DCID);
    
    m_ProgrammedPartNumber = new DCI_Owner( DCI_NEW_PROG_PART_NUMBER_DCID);
    
    m_BuildType = new DCI_Owner( DCI_BUILD_TYPE_DCID);
    
	m_SecondaryContactStatus->Attach_Callback( (DCI_CBACK_FUNC_TD *)&SecondaryContactStatusCallback_Static,
    		NULL,
            static_cast<DCI_CB_MSK_TD>(DCI_ACCESS_POST_SET_RAM_CMD_MSK));//DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_POST_SET_RAM_CMD_MSK

	m_SecondaryState->Attach_Callback( (DCI_CBACK_FUNC_TD *)&SecondaryStateCallback_Static,
    		NULL,
            static_cast<DCI_CB_MSK_TD>(DCI_ACCESS_POST_SET_RAM_CMD_MSK));//DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_POST_SET_RAM_CMD_MSK

    m_ResetBreaker->Attach_Callback( (DCI_CBACK_FUNC_TD *)&BreakerResetCallback_Static,
    		NULL,
            static_cast<DCI_CB_MSK_TD>(DCI_ACCESS_POST_SET_RAM_CMD_MSK));//DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_POST_SET_RAM_CMD_MSK


    m_ProductName->Attach_Callback( (DCI_CBACK_FUNC_TD *)&BreakerNameCallback_Static,
    		NULL,
            static_cast<DCI_CB_MSK_TD>(DCI_ACCESS_POST_SET_RAM_CMD_MSK));//DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_POST_SET_RAM_CMD_MSK

    m_UnicastPrimaryUDPKey->Attach_Callback( (DCI_CBACK_FUNC_TD *)&UnicastPrimaryUDPKeyCallback_Static,
    		NULL,
            static_cast<DCI_CB_MSK_TD>(DCI_ACCESS_POST_SET_RAM_CMD_MSK));//DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_POST_SET_RAM_CMD_MSK
    m_UnicastSecondaryUDPKey->Attach_Callback( (DCI_CBACK_FUNC_TD *)&UnicastSecondaryUDPKeyCallback_Static,
    		NULL,
            static_cast<DCI_CB_MSK_TD>(DCI_ACCESS_POST_SET_RAM_CMD_MSK));//DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_POST_SET_RAM_CMD_MSK
    m_BroadcastPrimaryUDPKey->Attach_Callback( (DCI_CBACK_FUNC_TD *)&BroadcastPrimaryUDPKeyCallback_Static,
    		NULL,
            static_cast<DCI_CB_MSK_TD>(DCI_ACCESS_POST_SET_RAM_CMD_MSK));//DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_POST_SET_RAM_CMD_MSK
    m_BroadcastSecondaryUDPKey->Attach_Callback( (DCI_CBACK_FUNC_TD *)&BroadcastSecondaryUDPKeyCallback_Static,
    		NULL,
            static_cast<DCI_CB_MSK_TD>(DCI_ACCESS_POST_SET_RAM_CMD_MSK));//DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_POST_SET_RAM_CMD_MSK


}


void SetConnectStringInfo()
{
 
     uint16_t size;

    memset ((char *)&DeviceInfo.HostName,0, sizeof (DeviceInfo.HostName)); 
    memset ((char *)&DeviceInfo.DeviceId,0, sizeof (DeviceInfo.DeviceId));
    memset ((char *)&DeviceInfo.SharedAccessKey,0, sizeof (DeviceInfo.SharedAccessKey));
    
    
    size = strlen (DeviceInfo.ConnectionString);
    if (size<100)
    {
    	//invalid connection string, reset
    	ESP_LOGI(TAG, "Invalid connection string, reset");
        ResetDevice(MANUFACTURING_RESET, true);
        return;
    }
    ParseConnectionString();
    m_ConnectionString->Check_In((DCI_DATA_PASS_TD *)DeviceInfo.ConnectionString);
    m_ConnectionString->Check_In_Init((DCI_DATA_PASS_TD *)DeviceInfo.ConnectionString);
    
    memset ((char *)&DeviceInfo.ConnectionString,0, sizeof (DeviceInfo.ConnectionString));
    m_ConnectionString->Check_Out_Init((DCI_DATA_PASS_TD *)DeviceInfo.ConnectionString);
   
    
    m_DeviceUUID->Check_In((DCI_DATA_PASS_TD *)DeviceInfo.DeviceId);
    m_DeviceUUID->Check_In_Init((DCI_DATA_PASS_TD *)DeviceInfo.DeviceId);
    
    
    DeviceInfo.ConnectionStringSaved = true;
}

void GetConnectStringInfo()
{
 
    uint16_t size;
    
    DeviceInfo.DidWeCheckTheConnectionString = true;

    memset ((char *)&DeviceInfo.ConnectionString,0, sizeof (DeviceInfo.ConnectionString));    
    memset ((char *)&DeviceInfo.HostName,0, sizeof (DeviceInfo.HostName)); 
    memset ((char *)&DeviceInfo.DeviceId,0, sizeof (DeviceInfo.DeviceId));
    memset ((char *)&DeviceInfo.SharedAccessKey,0, sizeof (DeviceInfo.SharedAccessKey));
    
    //m_ConnectionString->Check_Out_Init((DCI_DATA_PASS_TD *)DeviceInfo.ConnectionString);
    m_ConnectionString->Check_Out((DCI_DATA_PASS_TD *)DeviceInfo.ConnectionString);
    
    size = strlen (DeviceInfo.ConnectionString);
    
    if (size<100)
    {
        DeviceInfo.ConnectionStringSaved = false;
        if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
        {        
            ets_printf("NO Connection string SAVED\n");
        }
        return;
    }
    
    
    ParseConnectionString();
    
    DeviceInfo.ConnectionStringSaved = true;

}

void ParseConnectionString(void)
{
    char *p;
    uint8_t TextLocation1,TextLocation2;
    
    p= strchr((char *)&DeviceInfo.ConnectionString,';');
    TextLocation1 =  p-DeviceInfo.ConnectionString + 1;
    memcpy ((char *)&DeviceInfo.HostName, (char *)&DeviceInfo.ConnectionString +sizeof ("HostName=")-1,TextLocation1-sizeof ("HostName="));


    p= strchr((char *)&DeviceInfo.ConnectionString+TextLocation1,';');
    TextLocation2 =  p-DeviceInfo.ConnectionString + 1;
    memcpy ((char *)&DeviceInfo.DeviceId, (char *)&DeviceInfo.ConnectionString+sizeof ("DeviceId=")-1 + TextLocation1,TextLocation2-TextLocation1-sizeof ("DeviceId="));
    

    p = strstr((char *)&DeviceInfo.ConnectionString,"SharedAccessKey=")+sizeof("SharedAccessKey=")-1;


    strcpy ((char *)&DeviceInfo.SharedAccessKey,p);
    
    if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
    {    
    	ets_printf( "ConnectionString: %.*s \n",  DeviceInfo.ConnectionString);
    }
    

}

void ResetConnectStringInfo()
{
    memset ((char *)&DeviceInfo.ConnectionString,0, sizeof (DeviceInfo.ConnectionString));    
    memset ((char *)&DeviceInfo.HostName,0, sizeof (DeviceInfo.HostName));
    memset ((char *)&DeviceInfo.DeviceId,0, sizeof (DeviceInfo.DeviceId));
    memset ((char *)&DeviceInfo.SharedAccessKey,0, sizeof (DeviceInfo.SharedAccessKey));    
    //WriteStringToFlash(EEOFFSET_CONNECTION_STRING_LABEL,DeviceInfo.ConnectionString,sizeof(DeviceInfo.ConnectionString));
    memset ((char *)&DeviceInfo.Ssid,0, sizeof (DeviceInfo.Ssid));
    memset ((char *)&DeviceInfo.PassWord,0, sizeof (DeviceInfo.PassWord));
    //WriteStringToFlash(EEOFFSET_SSID_LABEL,DeviceInfo.Ssid,sizeof(DeviceInfo.Ssid));
    //WriteStringToFlash(EEOFFSET_PASWORD_LABEL,DeviceInfo.PassWord,sizeof(DeviceInfo.PassWord));
}

    
    
#if 0

    //
	new DCI_Owner( DCI_PWR_BRD_HARDWARE_ID_VAL_DCID );

	//

	DCI_Owner*          m_prod_data_owner;
	DCI_ID_TD           prod_data_id;
	uint8_t             m_dummy_owner_data;

	m_prod_data_owner = new DCI_Owner( prod_data_id, &m_dummy_owner_data );

	//

	DCI_ID_TD address_id;
	DCI_Owner* m_address_owner;
	uint8_t new_address;
	uint8_t temp_state;

	m_address_owner = new DCI_Owner( address_id );
	m_address_owner->Check_Out( new_address );

	temp_state = 3;
	m_address_owner->Check_In( temp_state );

	//

	DCI_Owner *test_Owner_Check_In_Init = new DCI_Owner( DCI_TEST_ATTRI_0100000000_000000_DCID );

	uint8_t actualVal;
	uint8_t testVal[1] = { 1 };

	test_Owner_Check_In_Init->Check_Out( &actualVal );
	test_Owner_Check_In_Init->Check_In_Init( testVal );
	test_Owner_Check_In_Init->Check_Out_Init( &actualVal );

	//

	BF_Lib::bool_t retVal;

	DCI_Owner* test_Lk = new DCI_Owner( DCI_TEST_LOCK_DCID );

	test_Lk->Lock();

	if ( TRUE == test_Lk->Is_Locked() )
	{
		retVal = PASS;
	}
	else
	{
		retVal = FALSE;
	}

	//

	BF_Lib::bool_t retVal;

	DCI_Owner* test_Lk = new DCI_Owner( DCI_TEST_LOCK_DCID );

	test_Lk->Unlock();

	if ( FALSE == test_Lk->Is_Locked() )
	{
		retVal = PASS;
	}
	else
	{
		retVal = FALSE;
	}

	//

	BF_Lib::bool_t retVal;

	DCI_Owner* test_get_owner_attr = new DCI_Owner( DCI_TEST_GET_OWNER_ATTRIBUTE_DCID );

	DCI_OWNER_ATTRIB_TD retrivedAttributes;
	DCI_OWNER_ATTRIB_TD actualDCIDAttributes = 0x01FE; // 0b0000000111111110

	retrivedAttributes = test_get_owner_attr->Get_Owner_Attrib();

	if ( retrivedAttributes == actualDCIDAttributes )
	{
		retVal = PASS;
	}
	else
	{
		retVal = FALSE;
	}

	//

	DCI_ID_TD prod_list_id = 10;

	DCI_Owner* m_prod_list_owner = new DCI_Owner( prod_list_id );

	DCI_LENGTH_TD       index = 0;
	PROFI_ID_TD         prod_list_mod;

	m_prod_list_owner->Check_Out_Index( &prod_list_mod, index );
	m_prod_list_owner->Check_In_Index( &prod_list_mod, index );

	//

	DCI_ID_TD count_crc_id = 20;
	uint16_t m_num_crc_errors = 0;

	DCI_Owner* m_count_of_crc_failures = new DCI_Owner( count_crc_id );

	m_num_crc_errors++;

	if(m_count_of_crc_failures != nullptr)
	{
		m_count_of_crc_failures->Check_In(m_num_crc_errors);
		m_count_of_crc_failures->Load_Current_To_Init();
	}

	//

	DCI_ID_TD prod_list_id = 10;
	DCI_ID_TD prod_list[10];

	DCI_Owner* m_prod_list_owner = new DCI_Owner( prod_list_id );

	length = 2;

	if ( m_prod_list_owner->Check_In_Length( &length ) )
	{
		id_ptr = &prod_list[0];
		m_prod_list_owner->Check_In( id_ptr );
	}

	//

	DCI_ID_TD dci_id = 25;
	DCI_LENGTH_TD temp_asm_len = 0;

	owner = new DCI_Owner( dci_id, DCI_OWNER_ALLOC_NO_MEM );

	temp_asm_len++;

	owner->Check_In_Length( temp_asm_len );

	//

	#define DCI_MULTICAST_ENABLE_DCID            111
	#define DCI_BROADCAST_ENABLE_DCID            112

	UINT32 broadcast_settings = ETH_BroadcastFramesReception_Enable;
	UINT32 multicast_settings = ETH_MulticastFramesFilter_None;

	DCI_Owner* broadcast_owner = new DCI_Owner( DCI_MULTICAST_ENABLE_DCID );
	DCI_Owner* multicast_owner = new DCI_Owner( DCI_BROADCAST_ENABLE_DCID );

	broadcast_owner->Check_Out_Init( (UINT8*) &broadcast_settings);
	multicast_owner->Check_Out_Init( (UINT8*) &multicast_settings);

	//

	#define DCI_PREVIOUS_IP_SETTINGS_IN_USE_STATUS_DCID            30

	previous_IP_settings_in_use_owner = new DCI_Owner( DCI_PREVIOUS_IP_SETTINGS_IN_USE_STATUS_DCID );

	previous_ip_settings = FALSE;

	previous_IP_settings_in_use_owner->Check_In(previous_ip_settings);
	previous_IP_settings_in_use_owner->Check_In_Init((uint8_t*)&previous_ip_settings);

	//

	static void Prod_Data_Callback_Static( void )
	{
		//Do Nothing;
	}

	m_prod_data_owner->Attach_Callback( Prod_Data_Callback_Static,
	reinterpret_cast<DCI_CBACK_PARAM_TD>(this),
	static_cast<DCI_CB_MSK_TD>(DCI_ACCESS_GET_RAM_CMD_MSK) );
}


#endif //END_DOXYGEN_EXAMPLE_BLOCK


    
