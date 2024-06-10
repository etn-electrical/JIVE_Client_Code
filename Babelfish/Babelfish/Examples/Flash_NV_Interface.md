\example  Flash_NV
\brief    An example code for Flash_NV
          The sample code explains use of Flash_NV to store DCI parameters in internal Flash.
          @brief  Flash_NV is a interface through which we can access data stored in
          Flash memory. Dynamically allocated RAM buffer holds the continous value and 
          Flash NV region will be updated with latest values present in buffer every wait state interval.
          Values gets read from flash NV to RAM buffer every code startup.
	  Add PROD_SPEC_Target_NV_Init() code in Prod_Spec_MEM.cpp to store DCI parameters in internal Flash.
//Example code below can be added in PROD_SPEC_APP_Init to verify DCI operations support DCI values stored in Flash
and retained after power cycle. Every power cycle the DCI gets incremented.
{uint8_t status=0U; 
DCI_Owner* m_owner_nv_fram_check= nullptr;
m_owner_nv_fram_check = new DCI_Owner( DCI_BLE_NV_LED_CTRL_DCID );        
m_owner_nv_fram_check->Check_Out(status);       
status++;       
m_owner_nv_fram_check->Check_In(status);       
m_owner_nv_fram_check->Check_In_Init(&status);
}
#include "Flash_NV.h"

static const uint16_t WAIT_TIME_MS = 10U;//WAIT state time set to 10 msec
const uint8_t nv_flash_flush_key = 0x01U;

//memory address can be taken using uC_Fw_Code_Range.cpp and specific .icf files like below or else direct values can be entered as shown below
//
//const NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD DCI_NV_FLASH_CONFIG =
//{
//	0x8055000,
//	0x8056000,
//	0x08056FFF,
//};
const NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD DCI_NV_FLASH_CONFIG =
{
	reinterpret_cast<uint32_t>( uC_DCI_NV_Start_Address() ),
	( reinterpret_cast<uint32_t>( uC_DCI_NV_Start_Address() ) +
	  reinterpret_cast<uint32_t>( uC_DCI_NV_End_Address() ) + 1U ) / 2,
	reinterpret_cast<uint32_t>( uC_DCI_NV_End_Address() ),
};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_NV_flash_Callback( NV_Ctrl::cback_param_t param, NV_Ctrl::nv_status_t result );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void PROD_SPEC_Target_NV_Init( void )
{
	uint8_t nv_index = 0;
	NV_Ctrl* flash_space_nv_ctrl = nullptr;

#if defined ( WEB_SERVER_SUPPORT ) || defined ( PX_GREEN_IOT_SUPPORT ) || defined ( BLE_UART_SETUP )
	/* NV Memory + NV_CRYPTO */
	static const uint8_t NV_CTRL_MAX_CTRLS = 2U;
#else
	/* NV Memory or Ram_Nv */
	static const uint8_t NV_CTRL_MAX_CTRLS = 1U;
#endif
	static NV_Ctrl* nv_ctrl_list[NV_CTRL_MAX_CTRLS];
	uint8_t nv_flush_keys[NV_CTRL_MAX_CTRLS] = { 0 };

	nv_ctrl = new NV_Ctrls::uC_Flash(
		reinterpret_cast<NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD const*>( &DCI_NV_FLASH_CONFIG ),
		flash_base );


	flash_space_nv_ctrl = new NV_Ctrls::Flash_NV( nv_ctrl, &DCI_NV_FLASH_CONFIG,WAIT_TIME_MS
												  ,PROD_SPEC_NV_flash_Callback );

	if ( flash_space_nv_ctrl != nullptr )
	{
		nv_working_status = true;
	}

	if ( nv_working_status == true )
	{
        // Storing the Flash_NV handle in nv_ctrl_list to support DCI values stored in Flash NV region.
		nv_ctrl_list[nv_index] = flash_space_nv_ctrl;
		nv_flush_keys[nv_index] = nv_flash_flush_key;
	}
	else
	{
		nv_ctrl = new NV_Ctrls::Ram_NV( RAM_NV_MEMORY_SIZE,
										NV_DATA_CTRL0_CHECKSUM_SPACE_INCLUDED );

		nv_ctrl_list[nv_index] = nv_ctrl;
		nv_flush_keys[nv_index] = nv_ram_flush_key;
	}

	/* NV_Crypto inititialization */
#if defined ( WEB_SERVER_SUPPORT ) || defined ( PX_GREEN_IOT_SUPPORT ) || defined ( BLE_UART_SETUP )

	Crypt* crypt_obj = new Crypt_Aes_CBC_Matrix( const_cast<uint8_t const*>( &crypt_aes_key[0] ),
												 Crypt::AES_128_KEY,
												 const_cast<uint8_t const*>( &crypt_aes_iv[0] ),
												 Crypt::AES_IV_SIZE_KEY );


	crypto_space_nv_ctrl = new NV_Ctrls::NV_Crypto( nv_ctrl_list[0], crypt_obj, &NV_CRYPTO_CFG,
													PROD_SPEC_NV_Crypto_Callback );

	nv_ctrl_list[nv_index + 1] = crypto_space_nv_ctrl;
	nv_flush_keys[nv_index + 1] = nv_crypto_flush_key;	///< Used same key for NV_Crypto block
#endif

#ifdef LOGGING_SUPPORT
	if ( nv_working_status == true )
	{
		/*Monitor more NV failure events when NV memory is detected in the initilization*/
		NV::NV_Mem::Init( &nv_ctrl_list[0], NV_CTRL_MAX_CTRLS, &nv_flush_keys[0],
						  Capture_NV_Mem_Operation_Failure );
	}
	else
	{
		NV::NV_Mem::Init( &nv_ctrl_list[0], NV_CTRL_MAX_CTRLS, &nv_flush_keys[0] );
	}
#else	// LOGGING_SUPPORT
	NV::NV_Mem::Init( &nv_ctrl_list[0], NV_CTRL_MAX_CTRLS, &nv_flush_keys[0] );

#endif

}
 **********************************************************************************************
 * @brief                     Product Specific NV Flash callback to notify error events
 * @param[in] param           Callback parameter
 * @param[in] result          Error type
 * @return[out] void          none
 * @n
 **********************************************************************************************
 */
void PROD_SPEC_NV_flash_Callback( NV_Ctrl::cback_param_t param, NV_Ctrl::nv_status_t result )
{
	/* Adapter should handle error conditions here */
}
/*