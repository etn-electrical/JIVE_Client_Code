\example  Param_Lock 
\brief    An example code for Param_Lock
          The sample code explains use of different APIs of Param_Lock Class.
\par Board Specific Settings  

 \details Not Applicable 
 
 \par Pin Configurations 
 \details Not Applicable

 \par Dependency
 \details Not Applicable
 
 \par Sample Code Listing

~~~~{.cpp}

Param_Lock::dci_lock_block_t* param_struct;
BF::DCI_Lock* m_lock_instance = nullptr;
Param_Lock* param_lock_instance = nullptr;
DCI_Owner* m_lock_status ,m_lock_type


param_struct->lock_list[1]= DCI_LOCK_TEST_1_DCID; //example dcid
param_struct->lock_list[2]= DCI_LOCK_TEST_2_DCID;
param_struct->lock_list[3]= DCI_LOCK_TEST_3_DCID;
param_struct->list_length = 3U ;

m_lock_instance = new BF::DCI_Lock( BF::DCI_Lock::READ_WRITE );
param_lock_instance = new Param_Lock(param_struct, m_lock_instance); ///< create instance with only these arguments 
//if we dont want to give access to the patrons to lock or unlock the dcid's
//else if we want to give access to patron to lock or unlock certain dcid
//Create the DCID for storing Lock status and also the lock type 

m_lock_status = new DCI_Owner(DCI_LOCK_STATUS_DCID);
m_lock_type = new DCI_Owner(DCI_LOCK_TYPE_DCID);

param_lock_instance = new Param_Lock(param_struct, m_lock_instance, m_lock_status ,m_lock_type);

// Lock for Read access

param_lock_instance->Lock(BF::DCI_Lock::LOCK_READ_MASK);

if ( TRUE == param_lock_instance->Locked(BF::DCI_Lock::LOCK_READ_MASK) )
	{
		retVal = PASS;
	}
	else
	{
		retVal = FALSE;
	}

param_lock_instance->Lock(BF::DCI_Lock::LOCK_WRITE_MASK);

if ( TRUE == param_lock_instance->Locked(BF::DCI_Lock::LOCK_WRITE_MASK) )
	{
		retVal = PASS;
	}
	else
	{
		retVal = FALSE;
	}
param_lock_instance->Lock(BF::DCI_Lock::LOCK_ALL_MASK);

if ( TRUE == param_lock_instance->Locked(BF::DCI_Lock::LOCK_ALL_MASK) )
	{
		retVal = PASS;
	}
	else
	{
		retVal = FALSE;
	}
~~~~