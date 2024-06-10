@example  httpd
@brief    The sample code for modbus tcp extended to include the setup for http.
@par Board Specific Settings  
@details  Please do the following setting on EasyMXPro Board to run successfully the sample code. 
@n        1. STM32Fx07 Card is fitted on the board. 
@n        

@par Pin Configurations 
@details   Not applicable



@par Dependency 
@details Not applicable 

@par Sample Code Listing 
  
~~~~{.cpp}

#include "Ram.h"
#include "Ram_Static.h"
#include "Modbus_Net.h"
#include "Modbus_TCP.h"
#include "httpd.h"

/*
 brief - An example code which shows how to initialize and use httpd services
*/
void Httpd_Example(void)
{
    modbus_tcp = new Modbus_Net( MODBUS_TCP_DEFAULT_ADDRESS, new Modbus_TCP(), 
                    &modbus_dci_data_target_info, 
                    new DCI_Owner( DCI_MODBUS_TCP_COMM_TIMEOUT_DCID ) ); 
    modbus_tcp->Enable_Port( MODBUS_TCP_TX_MODE ); 
    
    /* Initialize HTTP */ 
   httpd_init( &rest_dci_data_target_info, reinterpret_cast<DCI_Owner*>(NULL) ); //loTool kit

}

~~~~