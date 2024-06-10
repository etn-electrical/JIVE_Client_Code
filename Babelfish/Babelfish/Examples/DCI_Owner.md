\example  DCI_Owner 
\brief    An example code for DCI Owner
          The sample code explains use of different APIs of DCI_Owner Class.
\par Board Specific Settings  

 \details Not Applicable 
 
 \par Pin Configurations 
 \details Not Applicable

 \par Dependency
 \details Not Applicable
 
 \par Sample Code Listing

 ~~~~{.cpp}
 void DCIInit( void )
{
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

///< Locking DCID for Write access 

	test_Lk->Lock(BF::DCI_Lock::LOCK_WRITE_MASK);

	if ( TRUE == test_Lk->Is_Locked(BF::DCI_Lock::LOCK_WRITE_MASK) )
	{
		retVal = PASS;
	}
	else
	{
		retVal = FALSE;
	}

      ///< Locking DCID for READ access 
      
      	test_Lk->Lock(BF::DCI_Lock::LOCK_READ_MASK);

	if ( TRUE == test_Lk->Is_Locked(BF::DCI_Lock::LOCK_READ_MASK) )
	{
		retVal = PASS;
	}
	else
	{
		retVal = FALSE;
	}
              ///< Locking DCID for both read and write access 
      
      	test_Lk->Lock(BF::DCI_Lock::LOCK_ALL_MASK);

	if ( TRUE == test_Lk->Is_Locked(BF::DCI_Lock::LOCK_ALL_MASK) )
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

#endif
~~~~