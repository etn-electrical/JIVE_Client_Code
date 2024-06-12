\example  AtoD
\brief    The sample code to initialize and read the Analog channels on STM32WB55 and STM32F207
 \par Board Specific Settings  

 \details On P-Nucleo-WB55 board, Connect analog input at PA2 (ADC channel 7). 
		  On EasyMXPRoV7 board, place jumper J8 at PA3 to select ADC channel 3. 
 
 \par Pin Configurations 
 \details Not Applicable

 \par Dependency
  \details Following DCID needs to be created in Tools\RTK DCI DB Creator.xlsm for storing ADC value
 \n 1. DCI_EXAMPLE_ADC_VAL_DCID
 
 \par Sample Code Listing
 
~~~~{.cpp}

#include "CR_Tasker.h"
#include "uC_AtoD.h"
#ifdef uC_STM32F207_USAGE
#include  "AtoD.h"
#endif

#ifdef uC_STM32F207_USAGE
#define EG_ADC_CHANNEL 3
#endif

#ifdef uC_STM32WB55_USAGE
#define EG_ADC_CHANNEL 7
#define INTERRUPT_MODE  //Comment this macro to run ADC in Polling mode
void AtoD_Int( uC_AtoD::cback_param_t m_cback_param );
#endif

#define ACTIVE_CHANNELS (1 << EG_ADC_CHANNEL)
void AtoD_Task( CR_Tasker* cr_task, CR_TASKER_PARAM param );

uC_AtoD* eg_atod;
DCI_Owner* adc_value_owner;

// init function
void AtoD_init( void )
{
#ifdef uC_STM32F207_USAGE
	BF_Misc::AtoD* atod;
	eg_atod = new uC_AtoD( uC_ATOD_16BIT_RESOLUTION, ACTIVE_CHANNELS, uC_BASE_ATOD_CTRL_1 );
	atod = new BF_Misc::AtoD(eg_atod, EG_ADC_CHANNEL);
	// enable AtoD conversion and start reading channel
	eg_atod->Start_Repeat_Scan();
	// create a CR task to periodically read ADC channel
	new CR_Tasker( AtoD_Task, reinterpret_cast<CR_TASKER_PARAM>( NULL ) );
#endif

#ifdef uC_STM32WB55_USAGE
	/* create uC_AtoD class instance */
	eg_atod = new uC_AtoD( uC_ATOD_12BIT_RESOLUTION, ACTIVE_CHANNELS, uC_BASE_ATOD_CTRL_1 );	// active

#ifdef INTERRUPT_MODE
	eg_atod->Attach_Callback( &AtoD_Int, NULL, uC_INT_HW_PRIORITY_2 );
	/* Enable single conversion mode */
	eg_atod->Start_Scan();
#else // Polling mode
	/* Create a CR task to periodically read ADC channel */
	new CR_Tasker( AtoD_Task, reinterpret_cast<CR_TASKER_PARAM>( NULL ) );
	/* Enable AtoD continuous conversion mode and start reading channel */
	eg_atod->Start_Repeat_Scan();
#endif //INTERRUPT_MODE
#endif //uC_STM32WB55_USAGE

	adc_value_owner = new DCI_Owner( DCI_EXAMPLE_ADC_VAL_DCID );
}

#ifdef uC_STM32WB55_USAGE
//Interrupt handler
void AtoD_Int( uC_AtoD::cback_param_t m_cback_param )
{
	static uint16_t adc_channel_temp = 0U;
	adc_channel_temp = eg_atod->Get_Channel( EG_ADC_CHANNEL );
	adc_value_owner->Check_In( &adc_channel_temp );
}
#endif

// ADC read task
void AtoD_Task( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	static uint16_t adc_channel_temp = 0U;

	CR_Tasker_Begin( cr_task );

	while ( true )
	{
#ifdef uC_STM32WB55_USAGE
		eg_atod->Start_Scan();
		while ( !( eg_atod->Scan_Done() ) )
		{
			CR_Tasker_Delay( cr_task, TIME_100_MS );
		}
#endif		
		adc_channel_temp = eg_atod->Get_Channel( EG_ADC_CHANNEL );
		adc_value_owner->Check_In( &adc_channel_temp );
		CR_Tasker_Delay( cr_task, TIME_100_MS );
	}

	CR_Tasker_End();
}

~~~~
