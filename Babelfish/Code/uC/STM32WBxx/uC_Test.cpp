/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "Includes.h"
#include "Micro_Task.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Prototypes
 *****************************************************************************************
 */

#include "uC_Watchdog.h"
#include "uC_Watchdog_HW.h"
#define uC_TEST_WATCHDOG_TIMEOUT_SET        1000
bool uC_TEST_Watchdog_Test_Init( void );


#include "uC_Reset.h"
bool uC_TEST_Soft_Reset_Test_Init( void );

#include "uC_IO_Config.h"
#include "App_IO_Config.h"
bool uC_TEST_IO_Config_Test_Init( void );

#include "uC_Input.h"
bool uC_TEST_Input_Test_Init( void );

void uC_TEST_Input_Test_Task( OS_TASK_PARAM param );

#include "uC_Interrupt.h"
bool uC_TEST_Interrupt_Test_Init( void );

#include "uC_AtoD.h"
bool uC_TEST_AtoD_Test_Init( void );

void uC_TEST_AtoD_Int_CBACK( void );


#include "CR_Tasker.h"
#include "CR_Queue.h"
#include "Timers_Lib.h"
CR_Tasker* my_cr_task0;
CR_Tasker* my_cr_task1;
CR_Tasker* my_cr_task2;
CR_Tasker* my_cr_task3;
CR_Tasker* my_cr_task4;
CR_Queue* my_cr_queue0;
CR_Queue* my_cr_queue1;
CR_Queue* my_cr_queue2;
CR_Queue* my_cr_queue3;
Timers* my_cr_timer0;
Timers* my_cr_timer1;
Timers* my_cr_timer2;
Timers* my_cr_timer3;
void uC_TEST_CR_Tasker_Task0( CR_Tasker* cr_task, CR_TASKER_PARAM param );

void uC_TEST_CR_Tasker_Task1( CR_Tasker* cr_task, CR_TASKER_PARAM param );

void uC_TEST_CR_Tasker_Task2( CR_Tasker* cr_task, CR_TASKER_PARAM param );

void uC_TEST_CR_Tasker_Task3( CR_Tasker* cr_task, CR_TASKER_PARAM param );

void uC_TEST_CR_Tasker_Task4( CR_Tasker* cr_task, CR_TASKER_PARAM param );

void uC_TEST_CR_Timer0_Func( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );

void uC_TEST_CR_Timer1_Func( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );

void uC_TEST_CR_Timer2_Func( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );

void uC_TEST_CR_Timer3_Func( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param );

bool uC_TEST_CR_Tasker_Init( void );

bool uC_TEST_uC_Delay( void );

#include "Output_Pos.h"
#include "Timers_Lib.h"
#include "uC_SPI.h"
#include "FRAM.h"
bool uC_TEST_FRAM_Init( void );

void uC_TEST_FRAM_Task( OS_TASK_PARAM param );

#include "uC_DMA.h"
#include "Stdlib_MV.h"
bool uC_TEST_DMA_Init( void );


#include "AT25DF.h"
#include "uC_SPI.h"
bool uC_TEST_AT25DF_Init( void );

void uC_TEST_AT25DF_Task( CR_Tasker* cr_task, CR_TASKER_PARAM param );


#include "uC_Flash.h"
bool uC_TEST_uC_Flash_Init( void );

void uC_TEST_Init_String( uint8_t* string, uint16_t length, uint8_t start_val = 0, uint8_t inc_val = 1 );

#include "uC_SPI.h"
bool uC_TEST_SPI_Test_Init( void );

#include "uC_TEST_Timers.h"
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_Init( void )
{
	uC_TEST_Timers_Init();

	// volatile BOOL test_success = 0;
	//
	//
	// test_success |= uC_TEST_uC_Delay();
	//
	// Enable_Interrupts();

	// test_success |= uC_TEST_uC_Flash_Init();
	// test_success |= uC_TEST_AT25DF_Init();

	// test_success |= uC_TEST_DMA_Init();
	// test_success |= uC_TEST_FRAM_Init();


	// test_success |= uC_TEST_CR_Tasker_Init();

	////Module instantiation
	// new uC_Interrupt();
	// new uC_IO_Config();
	//
	// Enable_Interrupts();
	//
	// test_success |= uC_TEST_AtoD_Test_Init();
	// test_success |= uC_TEST_PWM_Test_Init();
	// test_success |= uC_TEST_SPI_Test_Init();
	// test_success |= uC_TEST_USART_Test_Init();
	// test_success |= uC_TEST_Timers_Test_Init();
	// test_success |= uC_TEST_Timers_Test_Init();

	// test_success |= uC_TEST_Interrupt_Test_Init();
	// test_success |= uC_TEST_IO_Config_Test_Init();
	// test_success |= uC_TEST_Watchdog_Test_Init();
	// test_success |= uC_TEST_Soft_Reset_Test_Init();
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************

 #define uC_FLASH_TEST_START_POINT			0x08020000	//Roughly at the midpoint of the main memory.
 #define uC_FLASH_TEST_LENGTH				0x10000
   const uC_FLASH_CHIP_CONFIG_TD ST1_FLASH_CFG =
   {
    0x800,		//page size
    uC_FLASH_TEST_START_POINT,			//Start Address
    ( uC_FLASH_TEST_START_POINT + ( uC_FLASH_TEST_LENGTH / 2 ) ),	//Mirror Start Address
    ( uC_FLASH_TEST_START_POINT + uC_FLASH_TEST_LENGTH )			//End of Memory
   };
 #define uC_FLASH_TEST_STRING_LEN			5
   BOOL uC_TEST_uC_Flash_Init( void )
   {
    volatile NV_Ctrl::nv_status_t return_status;
    uC_FLASH_CHIP_CONFIG_TD const* flash_cfg_ptr = &ST1_FLASH_CFG;

    uC_Flash* flash_ctrl;
    uint8_t test_flash_data[uC_FLASH_TEST_STRING_LEN];

    for ( uint8_t i = 0; i < uC_FLASH_TEST_STRING_LEN; i++ )
    {
        test_flash_data[i] = i + 1;
    }

    flash_ctrl = new uC_Flash( &ST1_FLASH_CFG, FLASH );

 #if 0
    return_status = flash_ctrl->Erase( flash_cfg_ptr->start_address, flash_cfg_ptr->page_size * 2, 0x55, false );
    return_status = flash_ctrl->Erase( flash_cfg_ptr->mirror_start_address, flash_cfg_ptr->page_size * 2, 0x55, false );

    return_status = flash_ctrl->Write( test_flash_data, flash_cfg_ptr->start_address + 4, 4 );
    return_status = flash_ctrl->Write( test_flash_data,
            flash_cfg_ptr->start_address + flash_cfg_ptr->page_size - 2, 4, true );
    return_status = flash_ctrl->Write( test_flash_data,
            flash_cfg_ptr->start_address + flash_cfg_ptr->page_size - 4, 4, true );
    return_status = flash_ctrl->Write( test_flash_data,
            flash_cfg_ptr->start_address + flash_cfg_ptr->page_size - 5, 4, true );
    return_status = flash_ctrl->Write( test_flash_data, flash_cfg_ptr->start_address + 3, 4, true );
    return_status = flash_ctrl->Write( test_flash_data,	flash_cfg_ptr->start_address + 1, 4, true );
    return_status = flash_ctrl->Write( test_flash_data,	flash_cfg_ptr->start_address + 4, 3, true );
    return_status = flash_ctrl->Write( test_flash_data,	flash_cfg_ptr->start_address + 4, 1, true );
    return_status = flash_ctrl->Write( test_flash_data,	flash_cfg_ptr->start_address + 3, 3, true );
    return_status = flash_ctrl->Write( test_flash_data,	flash_cfg_ptr->start_address + 3, 1, true );

    return_status = flash_ctrl->Erase( flash_cfg_ptr->start_address, 4, 0x55, false );
    return_status = flash_ctrl->Erase(
            flash_cfg_ptr->start_address + flash_cfg_ptr->page_size - 2, 4, 0x55, false );
    return_status = flash_ctrl->Erase(
            flash_cfg_ptr->start_address + flash_cfg_ptr->page_size - 4, 4, 0x55, false );
    return_status = flash_ctrl->Erase(
            flash_cfg_ptr->start_address + flash_cfg_ptr->page_size - 5, 4, 0x55, false );
    return_status = flash_ctrl->Erase(
            flash_cfg_ptr->start_address + ( flash_cfg_ptr->page_size/2 ) + ( flash_cfg_ptr->page_size - 2 ), 4, 0x55,
             *false );

    return_status = flash_ctrl->Write( test_flash_data, flash_cfg_ptr->start_address + 4, 4, false );
    return_status = flash_ctrl->Write( test_flash_data,
            flash_cfg_ptr->start_address + flash_cfg_ptr->page_size - 2, 4, false );
    return_status = flash_ctrl->Write( test_flash_data,
            flash_cfg_ptr->start_address + flash_cfg_ptr->page_size - 4, 4, false );
    return_status = flash_ctrl->Write( test_flash_data,
            flash_cfg_ptr->start_address + flash_cfg_ptr->page_size - 5, 4, false );
    return_status = flash_ctrl->Write( test_flash_data,
            flash_cfg_ptr->start_address + ( flash_cfg_ptr->page_size/2 ) + ( flash_cfg_ptr->page_size - 2 ), 4, false
             *);

    return_status = flash_ctrl->Erase( flash_cfg_ptr->start_address, 4, 0x55, true );
    return_status = flash_ctrl->Erase(
            flash_cfg_ptr->start_address + flash_cfg_ptr->page_size - 2, 4, 0x55, true );
    return_status = flash_ctrl->Erase(
            flash_cfg_ptr->start_address + flash_cfg_ptr->page_size - 4, 4, 0x55, true );
    return_status = flash_ctrl->Erase(
            flash_cfg_ptr->start_address + flash_cfg_ptr->page_size - 5, 4, 0x55, true );
    return_status = flash_ctrl->Erase(
            flash_cfg_ptr->start_address + ( flash_cfg_ptr->page_size/2 ) + ( flash_cfg_ptr->page_size - 2 ), 4, 0x55,
             *true );
 #endif

    return ( return_status );
   }
 */

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_SPI* at25df_spi_ctrl;
uint8_t at25df_test_string[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
AT25DF* at25_ctrl;
CR_Tasker* at25df_cr_tasker;
uC_USER_IO_STRUCT const* at25df_spi_cs[1] = { &FLASH_SPI_CS_IO_CTRL };
const AT25DF_CHIP_CONFIG_TD AT25DF041_CFG =
{
	3,			// Address length
	0x1000,		// erase page size
	0x100,		// write page size
	0,			// Start Address
	( 0x80000 / 2 ),	// Mirror Start Address
	0x80000,			// End of Memory
	18000000		// Max clock freq
};
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_TEST_AT25DF_Init( void )
{
	uint8_t device_id[4] = { 0, 1, 2, 3 };
	AT25DF_CHIP_CONFIG_TD const* chip_config = &AT25DF041_CFG;
	volatile NV_Ctrl::nv_status_t return_status;

	at25df_spi_ctrl = new uC_SPI( EXT_MEM_SPI_PORT, true, at25df_spi_cs, 1 );
	at25_ctrl = new AT25DF( at25df_spi_ctrl, 0, &AT25DF041_CFG );

	at25_ctrl->Read( at25df_test_string, 0, 8, false );

	at25df_cr_tasker = new CR_Tasker( uC_TEST_AT25DF_Task );

	at25_ctrl->Read_Device_ID( device_id );

	// while(1){};
#if 0
	return_status = at25_ctrl->Write( device_id, 4, 4 );
	return_status = at25_ctrl->Write( device_id, chip_config->erase_page_size - 2, 4, true );
	return_status = at25_ctrl->Write( device_id, chip_config->erase_page_size - 4, 4, true );
	return_status = at25_ctrl->Write( device_id, chip_config->erase_page_size - 5, 4, true );
	return_status =
		at25_ctrl->Write( device_id, ( chip_config->erase_page_size / 2 ) + ( chip_config->write_page_size - 2 ), 4,
						  true );
	return_status =
		at25_ctrl->Write( device_id, ( chip_config->erase_page_size / 2 ) + ( chip_config->write_page_size - 4 ), 4,
						  true );
	return_status =
		at25_ctrl->Write( device_id, ( chip_config->erase_page_size / 2 ) + ( chip_config->write_page_size - 5 ), 4,
						  true );

	return_status = at25_ctrl->Erase( chip_config->start_address, 4, 0x55, false );
	return_status = at25_ctrl->Erase( chip_config->erase_page_size - 2, 4, 0x55, false );
	return_status = at25_ctrl->Erase( chip_config->erase_page_size - 4, 4, 0x55, false );
	return_status = at25_ctrl->Erase( chip_config->erase_page_size - 5, 4, 0x55, false );
	return_status = at25_ctrl->Erase( ( chip_config->erase_page_size / 2 ) + ( chip_config->write_page_size - 2 ), 4,
									  0x55, false );
	return_status = at25_ctrl->Erase( ( chip_config->erase_page_size / 2 ) + ( chip_config->write_page_size - 4 ), 4,
									  0x55, false );
	return_status = at25_ctrl->Erase( ( chip_config->erase_page_size / 2 ) + ( chip_config->write_page_size - 5 ), 4,
									  0x55, false );

	return_status = at25_ctrl->Write( device_id, 4, 4, false );
	return_status = at25_ctrl->Write( device_id, chip_config->erase_page_size - 2, 4, false );
	return_status = at25_ctrl->Write( device_id, chip_config->erase_page_size - 4, 4, false );
	return_status = at25_ctrl->Write( device_id, chip_config->erase_page_size - 5, 4, false );
	return_status =
		at25_ctrl->Write( device_id, ( chip_config->erase_page_size / 2 ) + ( chip_config->write_page_size - 2 ), 4,
						  false );
	return_status =
		at25_ctrl->Write( device_id, ( chip_config->erase_page_size / 2 ) + ( chip_config->write_page_size - 4 ), 4,
						  false );
	return_status =
		at25_ctrl->Write( device_id, ( chip_config->erase_page_size / 2 ) + ( chip_config->write_page_size - 5 ), 4,
						  false );

	return_status = at25_ctrl->Erase( chip_config->start_address, 4, 0x55, true );
	return_status = at25_ctrl->Erase( chip_config->erase_page_size - 2, 4, 0x55, true );
	return_status = at25_ctrl->Erase( chip_config->erase_page_size - 4, 4, 0x55, true );
	return_status = at25_ctrl->Erase( chip_config->erase_page_size - 5, 4, 0x55, true );
	return_status = at25_ctrl->Erase( ( chip_config->erase_page_size / 2 ) + ( chip_config->write_page_size - 2 ), 4,
									  0x55, true );
	return_status = at25_ctrl->Erase( ( chip_config->erase_page_size / 2 ) + ( chip_config->write_page_size - 4 ), 4,
									  0x55, true );
	return_status = at25_ctrl->Erase( ( chip_config->erase_page_size / 2 ) + ( chip_config->write_page_size - 5 ), 4,
									  0x55, true );
#endif

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_AT25DF_Task( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	CR_Tasker_Begin( cr_task );



	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#define DMA_TEST_STR_LEN        16
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_TEST_DMA_Init( void )
{
	uC_DMA* dma_temp;
	uint8_t temp_string1[DMA_TEST_STR_LEN];
	uint8_t temp_string2[DMA_TEST_STR_LEN];

	for ( uint8_t i = 0; i < DMA_TEST_STR_LEN; i++ )
	{
		temp_string1[i] = i + 1;
		temp_string2[i] = 0;
	}

	dma_temp = new uC_DMA( uC_DMA1_STREAM_6, uC_DMA_DIR_FROM_MEMORY, 1,
						   uC_DMA_MEM_INC_TRUE, uC_DMA_PERIPH_INC_TRUE, uC_DMA_CIRC_FALSE,
						   uC_DMA_MEM2MEM_TRUE );

	for ( uint8_t i = 0; i < DMA_TEST_STR_LEN; i++ )
	{
		Debug_2_On();

		dma_temp->Set_Buff( temp_string1, temp_string2, i );
		dma_temp->Enable();
		while ( !dma_temp->Empty() )
		{}
		Debug_2_Off();

		for ( uint8_t j = 0; j < DMA_TEST_STR_LEN; j++ )
		{
			temp_string2[j] = 0;
		}

		// Debug_1_On();
		BF_Lib::Copy_String( temp_string1, temp_string2, i );
		// Debug_1_Off();

		for ( uint8_t j = 0; j < DMA_TEST_STR_LEN; j++ )
		{
			temp_string2[j] = 0;
		}
	}

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
FRAM* fram_ctrl;
Timers* timer_ctrl;
uC_SPI* spi_ctrl;
uC_USER_IO_STRUCT const* spi_cs[1] = { &FRAM_SPI_CS_IO_CTRL };
const FRAM_CHIP_CONFIG_TD FRAM_25L16_CFG =
{
	2,			// Address length
	0,			// Start Address
	( 0x400 / 2 ),		// Mirror Start Address
	0x400,			// End of Memory
	18000000		// Max clock freq
};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_TEST_FRAM_Init( void )
{
	spi_ctrl = new uC_SPI( EXT_MEM_SPI_PORT, true, spi_cs, 1 );
	fram_ctrl = new FRAM( spi_ctrl, FRAM_SPI_CS_NUM, &FRAM_25L16_CFG );

	timer_ctrl = new Timers( uC_TEST_FRAM_Task, NULL );
	timer_ctrl->Start( 1000, true );

	// Micro_Task::Add_Task( uC_TEST_FRAM_Task, NULL );
	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_FRAM_Task( OS_TASK_PARAM param )
{
	static const uint8_t test_fram_str_len = 4;
	static uint8_t temp_fram_src_string[test_fram_str_len] = { 0 };
	static uint8_t temp_fram_dest_string[test_fram_str_len];

	fram_ctrl->Write( temp_fram_src_string, 4, test_fram_str_len, true );
	fram_ctrl->Read( temp_fram_dest_string, 4, test_fram_str_len, true );

	temp_fram_src_string[0]++;
	for ( uint8_t i = 0; i < ( test_fram_str_len - 1 ); i++ )
	{
		temp_fram_src_string[i + 1] = temp_fram_src_string[i] + 1;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#define OS_IDLE_TASK_FALLBACK_STACK_SIZE        40
#define COROUTINE_OS_TASK_STACK_SIZE            150
static uint8_t yield_test = 0;
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_CR_Fallback_Func( OS_TASK_PARAM )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_TEST_CR_Tasker_Init( void )
{
	my_cr_task0 = new CR_Tasker( uC_TEST_CR_Tasker_Task0, NULL, CR_TASKER_PRIORITY_0 );
	my_cr_task1 = new CR_Tasker( uC_TEST_CR_Tasker_Task1, NULL, CR_TASKER_PRIORITY_0 );
	my_cr_task2 = new CR_Tasker( uC_TEST_CR_Tasker_Task2, NULL, CR_TASKER_PRIORITY_1 );
	my_cr_task3 = new CR_Tasker( uC_TEST_CR_Tasker_Task3, NULL, CR_TASKER_PRIORITY_2 );
	my_cr_task4 = new CR_Tasker( uC_TEST_CR_Tasker_Task4, NULL, CR_TASKER_PRIORITY_3 );
	my_cr_timer0 = new Timers( uC_TEST_CR_Timer0_Func, NULL );
	my_cr_timer1 = new Timers( uC_TEST_CR_Timer1_Func, NULL );
	my_cr_timer2 = new Timers( uC_TEST_CR_Timer2_Func, NULL );
	my_cr_timer3 = new Timers( uC_TEST_CR_Timer3_Func, NULL );
	my_cr_timer0->Start( 10, true );
	my_cr_timer1->Start( 11, false );
	my_cr_timer2->Start( 12, false );
	my_cr_timer3->Start( 20, false );

	my_cr_queue3 = new CR_Queue();

	// OS_Tasker::Create_Task( CR_Tasker::Scheduler, COROUTINE_OS_TASK_STACK_SIZE,
	// OS_TASK_PRIORITY_1, "Coroutine Task", NULL);
	// OS_Tasker::Create_Task( uC_TEST_CR_Fallback_Func, OS_IDLE_TASK_FALLBACK_STACK_SIZE,
	// OS_TASK_PRIORITY_LOW_IDLE_TASK, "Idle Task Fallback", NULL);
	//
	// OS_Tasker::Set_Tick_Hook( CR_Tasker::Tick_Event );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_CR_Tasker_Task0( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	volatile static uint8_t temp_this;
	volatile static CR_QUEUE_STATUS result;
	volatile static uint32_t queue_data;

	CR_Tasker_Begin( cr_task );

	for (;;)
	{
		temp_this++;

		CR_Tasker_Yield( cr_task );

		temp_this++;

		CR_Queue_Send( cr_task, my_cr_queue3, ( CR_TX_QUEUE_DATA )&queue_data, 10, result );
	}

	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_CR_Tasker_Task1( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	volatile static uint8_t temp_this;

	CR_Tasker_Begin( cr_task );

	for (;;)
	{
		temp_this++;
		CR_Tasker_Delay( cr_task, 5 );

		CR_Tasker_Yield_Until( cr_task, yield_test >= 5 );
		yield_test = 0;

		temp_this++;
		temp_this++;
	}
	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_CR_Tasker_Task2( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	volatile static uint8_t temp_this;

	CR_Tasker_Begin( cr_task );

	for (;;)
	{
		temp_this++;
		CR_Tasker_Delay( cr_task, 3 );
		temp_this++;
		temp_this++;
		CR_Tasker_Delay( cr_task, 3 );
		temp_this++;
		temp_this++;
		CR_Tasker_Delay( cr_task, 3 );
		temp_this++;
		temp_this++;
	}
	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_CR_Tasker_Task3( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	volatile static uint8_t temp_this;
	volatile static CR_QUEUE_STATUS result;
	volatile static uint32_t queue_data;

	CR_Tasker_Begin( cr_task );

	for (;;)
	{
		temp_this++;

		CR_Queue_Receive( cr_task, my_cr_queue3, ( CR_RX_QUEUE_DATA )&queue_data, 1000, result );

		temp_this++;
		temp_this++;

		CR_Tasker_Delay( cr_task, 3 );
		my_cr_task4->Resume();
		my_cr_task4->Resume();

		temp_this++;
		temp_this++;

		CR_Tasker_Delay( cr_task, 1000 );
	}

	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_CR_Tasker_Task4( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	volatile static uint8_t test = 0;

	Debug_0_On();

	CR_Tasker_Begin( cr_task );

	test++;

	cr_task->Suspend();

	test++;

	CR_Tasker_End();

	test++;

	Debug_0_Off();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_CR_Timer0_Func( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_CR_Timer1_Func( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{
	Debug_1_Toggle();
	my_cr_timer1->Start( 3, false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_CR_Timer2_Func( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{
	Debug_2_Toggle();

	my_cr_timer2->Start( 2, false );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_CR_Timer3_Func( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD param )
{
	static uint16_t temp = 1;

	Debug_3_Toggle();

	my_cr_task4->Resume();
	yield_test++;

	my_cr_timer3->Start( 50, false );

	my_cr_timer0->Start( temp, false );

	temp++;
	if ( temp > 50 )
	{
		temp = 0;
	}
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool uC_TEST_Interrupt_Test_Init( void )
{
	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_TEST_Watchdog_Test_Init( void )
{
	bool watchdog_reset_worked;
	uint32_t counter;
	uC_Watchdog* test_dog;

	test_dog = new uC_Watchdog();

	uC_Watchdog::Init( uC_TEST_WATCHDOG_TIMEOUT_SET );

	watchdog_reset_worked = uC_Watchdog_HW::Watchdog_Reset_Occurred();

	if ( watchdog_reset_worked == false )
	{
		counter = 0;
		while ( counter < ~0 )
		{
			uC_Watchdog::Handler();
			counter++;
			// Nop();
		}
	}
	else
	{
		counter = 0;
		while ( counter < ( ( ~( uint32_t )0 ) >> DIV_BY_1024 ) )
		{
			counter++;
			test_dog->Feed_Dog();
			uC_Watchdog::Handler();
		}
	}

	return ( watchdog_reset_worked );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_TEST_Soft_Reset_Test_Init( void )
{
	uint16_t trigger_ids;

	trigger_ids = uC_Reset::Get_Reset_Trigger();

	if ( !Test_Bit( trigger_ids, RESET_SOFTWARE_TRIGRD ) )
	{
		uC_Reset::Soft_Reset();
	}
	// if ( !Test_Bit( trigger_ids, RESET_WATCHDOG_TRIGRD ) )
	// {
	// uC_Reset::Watchdog_Reset();
	// }

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#define DEBUG_INPUT_0_IO_CTRL           PIO_NULL_IO_STRUCT
#define DEBUG_INPUT_1_IO_CTRL           PIO_NULL_IO_STRUCT
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_TEST_IO_Config_Test_Init( void )
{
	uC_IO_Config::Set_Pin_To_Output( &DEBUG_OUTPUT_0_IO_CTRL );
	uC_IO_Config::Set_Pin_To_Output( &DEBUG_OUTPUT_1_IO_CTRL );
	uC_IO_Config::Set_Pin_To_Output( &DEBUG_OUTPUT_2_IO_CTRL );
	uC_IO_Config::Set_Pin_To_Output( &DEBUG_OUTPUT_3_IO_CTRL );
	uC_IO_Config::Set_Pin_To_Input( &DEBUG_INPUT_0_IO_CTRL );

	for ( uint_fast32_t i = 0; i < 100; i++ )
	{
		uC_Delay( 20000 );
		if ( uC_IO_Config::Get_Out( &DEBUG_OUTPUT_0_IO_CTRL ) )
		{
			uC_IO_Config::Clr_Out( &DEBUG_OUTPUT_0_IO_CTRL );
			uC_IO_Config::Clr_Out( &DEBUG_OUTPUT_1_IO_CTRL );
			uC_IO_Config::Clr_Out( &DEBUG_OUTPUT_2_IO_CTRL );
			uC_IO_Config::Clr_Out( &DEBUG_OUTPUT_3_IO_CTRL );
		}
		else
		{
			uC_IO_Config::Set_Out( &DEBUG_OUTPUT_0_IO_CTRL );
			uC_IO_Config::Set_Out( &DEBUG_OUTPUT_1_IO_CTRL );
			uC_IO_Config::Set_Out( &DEBUG_OUTPUT_2_IO_CTRL );
			uC_IO_Config::Set_Out( &DEBUG_OUTPUT_3_IO_CTRL );
		}
	}

	uC_IO_Config::Set_Out( &DEBUG_OUTPUT_1_IO_CTRL );
	uC_IO_Config::Set_Out( &DEBUG_OUTPUT_3_IO_CTRL );

	uint_fast32_t j = 0;

	while ( j < 5 )
	{
		if ( !uC_IO_Config::Get_In( &DEBUG_INPUT_0_IO_CTRL ) &&
			 !uC_IO_Config::Get_Out( &DEBUG_OUTPUT_0_IO_CTRL ) )
		{
			uC_Delay( 1000 );		// A little debounce
			uC_IO_Config::Set_Out( &DEBUG_OUTPUT_0_IO_CTRL );
			uC_IO_Config::Set_Out( &DEBUG_OUTPUT_2_IO_CTRL );
			j++;
		}
		else if ( uC_IO_Config::Get_In( &DEBUG_INPUT_0_IO_CTRL ) )
		{
			uC_IO_Config::Clr_Out( &DEBUG_OUTPUT_0_IO_CTRL );
			uC_IO_Config::Clr_Out( &DEBUG_OUTPUT_2_IO_CTRL );
		}
	}
	uC_IO_Config::Clr_Out( &DEBUG_OUTPUT_1_IO_CTRL );
	uC_IO_Config::Clr_Out( &DEBUG_OUTPUT_3_IO_CTRL );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_TEST_Input_Test_Init( void )
{
	new uC_Input();

	uC_Input::Config_Input_Pin( &DEBUG_INPUT_1_IO_CTRL, uC_IO_IN_CFG_HIZ );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_Input_Test_Task( OS_TASK_PARAM param )
{
	static uint16_t input_test_count = 0;

	if ( ( uC_Input::Get_Input( &DEBUG_INPUT_1_IO_CTRL ) &&
		   Is_Number_Even( input_test_count ) ) ||
		 ( !uC_Input::Get_Input( &DEBUG_INPUT_1_IO_CTRL ) &&
		   !Is_Number_Even( input_test_count ) ) )
	{
		input_test_count++;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_AtoD* atod_ctrl;
const uint32_t atod_active_channels = ( ( 1 << 10 ) | ( 1 << 16 ) | ( 1 << 17 ) );
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_TEST_AtoD_Test_Init( void )
{
	atod_ctrl = new uC_AtoD( uC_ATOD_16BIT_RESOLUTION, atod_active_channels );
	atod_ctrl->Start_Repeat_Scan();

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_AtoD_Int_CBACK( void )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
#define NUM_uC_TEST_SPI_CS_CTRLS        1
#define uC_TEST_SPI_CS_ID               0
uC_USER_IO_STRUCT const* uC_TEST_SPI_CS_CTRLS[NUM_uC_TEST_SPI_CS_CTRLS] =
{ &FRAM_SPI_CS_IO_CTRL };
uC_SPI* test_spi_ctrl;
#define SPI_TEST_STRING_LEN 8
uint8_t spi_test_string[SPI_TEST_STRING_LEN];
uint8_t spi_test_rx_string[SPI_TEST_STRING_LEN];
uint8_t spi_test_index = 0;
void uC_TEST_SPI_Int_Handler( void );

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_TEST_SPI_Test_Init( void )
{
	uC_TEST_Init_String( spi_test_string, SPI_TEST_STRING_LEN );
	uC_TEST_Init_String( spi_test_rx_string, SPI_TEST_STRING_LEN );

	test_spi_ctrl = new uC_SPI( EXT_MEM_SPI_PORT, true, uC_TEST_SPI_CS_CTRLS,
								NUM_uC_TEST_SPI_CS_CTRLS, &EXT_MEM_SPI_PIO, true );

	test_spi_ctrl->Configure_Chip_Select( uC_SPI_MODE_0, uC_TEST_SPI_CS_ID, 500000 );

	test_spi_ctrl->Select_Chip( uC_TEST_SPI_CS_ID );

	for ( uint32_t i = 0; i < 10000; i++ )
	{
		test_spi_ctrl->TX_Byte( i );
	}

	test_spi_ctrl->Set_TX_DMA( spi_test_string, SPI_TEST_STRING_LEN );
	test_spi_ctrl->Enable_TX_DMA();
	while ( test_spi_ctrl->TX_DMA_Busy() )
	{}
	test_spi_ctrl->Disable_TX_DMA();

	test_spi_ctrl->Set_RX_DMA( spi_test_rx_string, SPI_TEST_STRING_LEN );
	test_spi_ctrl->Enable_RX_DMA();
	// test_spi_ctrl->Set_TX_DMA( spi_test_string, SPI_TEST_STRING_LEN );
	while ( !test_spi_ctrl->TX_Shift_Empty() )
	{}
	test_spi_ctrl->Disable_RX_DMA();

	test_spi_ctrl->Set_XChange_DMA( spi_test_string, spi_test_rx_string, SPI_TEST_STRING_LEN );
	test_spi_ctrl->Enable_XChange_DMA();
	while ( !test_spi_ctrl->TX_Shift_Empty() )
	{}

	test_spi_ctrl->Set_TX_DMA( spi_test_string, SPI_TEST_STRING_LEN );
	test_spi_ctrl->Enable_TX_DMA();
	while ( test_spi_ctrl->TX_DMA_Busy() )
	{}
	test_spi_ctrl->Disable_TX_DMA();


	test_spi_ctrl->De_Select_Chip( uC_TEST_SPI_CS_ID );

	return ( true );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_Init_String( uint8_t* string, uint16_t length, uint8_t start_val, uint8_t inc_val )
{
	for ( uint16_t i = 0; i < length; i++ )
	{
		string[i] = start_val;
		start_val += inc_val;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_TEST_RAM_Testing( void )
{
#if 0
	Ram_Hybrid* temp_ram;
	#define MALLOC_STRING1_LEN      1
	static volatile uint8_t* string1;
	#define MALLOC_STRING2_LEN      2
	static volatile uint8_t* string2;
	#define MALLOC_STRING3_LEN      3
	static volatile uint8_t* string3;
	#define MALLOC_STRING4_LEN      4
	static volatile uint8_t* string4;
	#define MALLOC_STRING16_LEN     16
	static volatile uint8_t* string16;
	#define MALLOC_STRING18_LEN     18
	static volatile uint8_t* string18;

	temp_ram = new Ram_Hybrid( sizeof( uint8_t ) );

	string1 = ( volatile uint8_t* )temp_ram->Allocate( 1, true, 0x11 );
	string2 = ( volatile uint8_t* )temp_ram->Allocate( 2, true, 0x22 );
	string3 = ( volatile uint8_t* )temp_ram->Allocate( 3, true, 0x33 );
	string4 = ( volatile uint8_t* )temp_ram->Allocate( 4, true, 0x44 );
	string16 = ( volatile uint8_t* )temp_ram->Allocate( 16, true, 0x16 );
	string18 = ( volatile uint8_t* )temp_ram->Allocate( 18, true, 0x18 );

	temp_ram->Enable_Dynamic_Mem();

	string1 = ( volatile uint8_t* )temp_ram->Allocate( 1, true, 0x11 );
	string2 = ( volatile uint8_t* )temp_ram->Allocate( 2, true, 0x22 );
	string3 = ( volatile uint8_t* )temp_ram->Allocate( 3, true, 0x33 );
	string4 = ( volatile uint8_t* )temp_ram->Allocate( 4, true, 0x44 );
	string16 = ( volatile uint8_t* )temp_ram->Allocate( 16, true, 0x16 );
	string18 = ( volatile uint8_t* )temp_ram->Allocate( 18, true, 0x16 );



	string1 = ( volatile uint8_t* )malloc( MALLOC_STRING1_LEN );
	Copy_Val_To_String( ( uint8_t* )string1, 0x11, MALLOC_STRING1_LEN );

	string2 = ( volatile uint8_t* )malloc( MALLOC_STRING2_LEN );
	Copy_Val_To_String( ( uint8_t* )string2, 0x22, MALLOC_STRING2_LEN );

	string3 = ( volatile uint8_t* )malloc( MALLOC_STRING3_LEN );
	Copy_Val_To_String( ( uint8_t* )string3, 0x33, MALLOC_STRING3_LEN );

	string4 = ( volatile uint8_t* )malloc( MALLOC_STRING4_LEN );
	Copy_Val_To_String( ( uint8_t* )string4, 0x44, MALLOC_STRING4_LEN );

	string16 = ( volatile uint8_t* )malloc( MALLOC_STRING16_LEN );
	Copy_Val_To_String( ( uint8_t* )string16, 0xA0, MALLOC_STRING16_LEN );

	string18 = ( volatile uint8_t* )malloc( MALLOC_STRING18_LEN );
	Copy_Val_To_String( ( uint8_t* )string18, 0xA0, MALLOC_STRING18_LEN );
	string18 = ( volatile uint8_t* )realloc( ( uint8_t* )string18, MALLOC_STRING18_LEN - 14 );
	free( ( void* )string3 );

	string18 = ( volatile uint8_t* )malloc( 30 );
	Copy_Val_To_String( ( uint8_t* )string18, 0xA0, 30 );
	string18 = ( volatile uint8_t* )realloc( ( uint8_t* )string18, 10 );

	string2 = string18;
	string2 = ( volatile uint8_t* )malloc( MALLOC_STRING2_LEN );
	Copy_Val_To_String( ( uint8_t* )string2, 0x55, MALLOC_STRING2_LEN );

	free( ( void* )string2 );

	string4 = ( volatile uint8_t* )malloc( MALLOC_STRING4_LEN );
	Copy_Val_To_String( ( uint8_t* )string4, 0x66, MALLOC_STRING4_LEN );

	free( ( void* )string1 );

	string4 = ( volatile uint8_t* )malloc( MALLOC_STRING4_LEN );
	Copy_Val_To_String( ( uint8_t* )string4, 0x77, MALLOC_STRING4_LEN );

	string1 = ( volatile uint8_t* )malloc( MALLOC_STRING1_LEN );
	Copy_Val_To_String( ( uint8_t* )string1, 0x12, MALLOC_STRING1_LEN );
	string1 = ( volatile uint8_t* )malloc( MALLOC_STRING1_LEN );
	Copy_Val_To_String( ( uint8_t* )string1, 0x13, MALLOC_STRING1_LEN );
	string1 = ( volatile uint8_t* )malloc( MALLOC_STRING1_LEN );

	string1 = ( volatile uint8_t* )realloc( ( uint8_t* )string1, MALLOC_STRING1_LEN - 4 );

	Copy_Val_To_String( ( uint8_t* )string1, 0x14, MALLOC_STRING1_LEN );
	string1 = ( volatile uint8_t* )malloc( MALLOC_STRING1_LEN );
	Copy_Val_To_String( ( uint8_t* )string1, 0x15, MALLOC_STRING1_LEN );



	while ( true )
	{}

#endif
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
   //The code below is serious bunk.

 #include "uC_DMA.h"
 #include "uC_USART.h"
 #include "Output_Pos.h"
 #include "Timers_Lib.h"
   BOOL uC_TEST_DMA_Que_Init( void );
   void uC_TEST_DMA_Que_Task( OS_TASK_PARAM param );


   uC_USART* usart_ctrl;
   Output_Pos* output_ctrl;
   const uint16_t DATA_BUFF_LEN = 16;
   uint8_t tx_buff[DATA_BUFF_LEN] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
   uint8_t rx_buff[DATA_BUFF_LEN];
   BOOL uC_TEST_DMA_Que_Init( void )
   {
    Timers* timer_ctrl;

    usart_ctrl = new uC_USART( RS485_MODBUS_USART_PORT );

    usart_ctrl->Configure( uC_USART_CHAR_SIZE_8BIT, uC_USART_RS485_MODE,
                            uC_USART_PARITY_EVEN, uC_USART_1_STOP_BIT );

    usart_ctrl->Set_RX_DMA( rx_buff, DATA_BUFF_LEN );
    usart_ctrl->Enable_RX_DMA();
    usart_ctrl->Enable_RX();

    usart_ctrl->Set_TX_DMA( tx_buff, DATA_BUFF_LEN );
    usart_ctrl->Enable_TX_DMA();

    usart_ctrl->Set_Baud_Rate( 19200 );
    usart_ctrl->Enable_TX();

    Micro_Task::Add_Task( uC_TEST_DMA_Que_Task, NULL );
   //	timer_ctrl = new Timers( uC_TEST_DMA_Que_Task, NULL );
   //	timer_ctrl->Start( 1, true );
    output_ctrl = new Output_Pos( &RS485_MODBUS_TXEN_IO_CTRL );
    output_ctrl->On();

    return ( true );
   }
   void uC_TEST_DMA_Que_Task( OS_TASK_PARAM param )
   {
    static uint16_t ctr = 0;
    static uint16_t missed = 0;
    static uint16_t ctr2 = 0;

    if ( ctr != usart_ctrl->Get_DMARX_Data_Length() )
    {
        ctr = usart_ctrl->Get_DMARX_Data_Length();
        ctr2++;
    }
    else
    {
        missed++;
    }

    if ( usart_ctrl->Get_TX_Data_Length() == 0 )
    {
        for ( uint8_t i = DATA_BUFF_LEN; i > 1; i-- )
        {
            tx_buff[i] = tx_buff[i-1] + 1;
        }

        usart_ctrl->Set_RX_DMA( rx_buff, DATA_BUFF_LEN );
        usart_ctrl->Enable_RX_DMA();
        ctr = usart_ctrl->Get_DMARX_Data_Length();
        usart_ctrl->Set_TX_DMA( tx_buff, DATA_BUFF_LEN );
        usart_ctrl->Enable_TX_DMA();
    }
   }
 */
