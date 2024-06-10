@example  OTP flash memory
@brief    The sample code to use the OTP class on STM32F40x,STM3241x,STM32F207xx, STM32F42xxx and STM32F43xxx.

@par Board Specific Settings  

@details  Please do the following setting on EasyMXPro Board to run the sample code successfully.
@n        1. STM32F407 Card is fitted on the board.


@par Pin Configurations 
@details Not applicable

@par Dependency
@details Not applicable

@par Sample Code Listing

~~~~{.cpp}

#include "uC_OTP.h"

/* structure for configuring of OTP memory region */
#ifndef OTP_RAM_SIMULATION
NV_Ctrls::uC_OTP::uc_otp_config_t OTP_CONFIG
{
	0x1FFF7820, // OTP memory address
	8U          // size of data in that OTP memory address
};
#endif

uint8_t Otp_test( void )
{
	uint8_t mac_reg[8] = "ABC";
	uint8_t recv_mac[8] = {0};
	uint8_t return_status = NV_Ctrl::DATA_ERROR;
#ifndef OTP_RAM_SIMULATION
	static NV_Ctrl* m_otp_ctrl = new NV_Ctrls::uC_OTP( &OTP_CONFIG, FLASH );
	
	return_status = m_otp_ctrl->Write( reinterpret_cast<uint8_t*>( mac_reg ),
									   OTP_CONFIG.otp_start_address, sizeof( mac_reg ) );
	return_status = m_otp_ctrl->Read( reinterpret_cast<uint8_t*>( recv_mac ),
									  OTP_CONFIG.otp_start_address, sizeof( mac_reg ) );
#else
	NV_Ctrls::uC_OTP::uc_otp_config_t OTP_CONFIG;
	uint8_t *ramptr = new uint8_t[64];
	const uint8_t *ptr = reinterpret_cast<const uint8_t *>(0x10000000); // specify the address from which RAM begins
	OTP_CONFIG.otp_start_address = 0x10000000 + (ramptr - ptr );
	OTP_CONFIG.size = sizeof( uint8_t[64] );
	static NV_Ctrl* m_otp_ctrl = new NV_Ctrls::uC_OTP( &OTP_CONFIG, FLASH );
	return_status = m_otp_ctrl->Write( reinterpret_cast<uint8_t*>( mac_reg ),
									  OTP_CONFIG.otp_start_address, sizeof( mac_reg ) );
	return_status = m_otp_ctrl->Read( reinterpret_cast<uint8_t*>( recv_mac ),
									 OTP_CONFIG.otp_start_address, sizeof( mac_reg ) );
#endif

	return ( return_status );
}