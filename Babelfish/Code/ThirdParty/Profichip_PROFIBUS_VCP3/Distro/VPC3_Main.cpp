/**
*****************************************************************************************
*	@file
*	@details See header file for module overview.
*       @Author Shafiq Navodia
*	@Date 23/2/2012
*	@copyright 2012 Eaton Corporation. All Rights Reserved.
*       @Header VPC3_Main.h
*****************************************************************************************
*/

#include "Includes.h"
#include "uC_SPI.h"
#include "VPC3.h"
#include "VPC3_Main.h"
#include "Output_Pos.h"
#include "Output_Neg.h"
#include "NV_Address.h"

bool uC_TEST_USART_Test_Init2( void );
bool uC_TEST_IO_Config_Test_Init2( void );     //#k: New for RedBall PCB Test
bool uC_TEST_AtoD_Test_Init2( void );
bool uC_TEST_FRAM_Init2( void );      //#k: New for RedBall PCB testing

/*
*****************************************************************************************
*		Static Variables
*****************************************************************************************
*/



uC_USER_IO_STRUCT const* const EXT_VPC3_CS_CTRLS[NUM_EXT_ASIC_CS_CTRLS] =
{&VPC3_SPI_CS_IO_CTRL };

const VPC3_CHIP_CONFIG_TD VPC3_CFG =
{
		2U,			//Address length
		NV_DATA_START_NVCTRL0_NVADD,	//Start Address
		NV_DATA_CTRL0_RANGE_SIZE,		//Mirror Start Address
		NV_DATA_CTRL0_RANGE_SIZE * 2U,	//End of Memory
		5000000U		//Max clock freq
};

VPC3*  VPC3_ctrl;
uC_SPI* spi_ctrl_VPC3;
ProfiBus_Display* profi_display_ctrl;
extern void TEST_FRAM(void);
extern void TEST_FRAM_Init_FM25L04(void);
extern DP_ERROR_CODE uC_Test_VPC3_Memory( void );
/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/
void EnableInterruptVPC3Channel1(void)
{
        VPC3_ext_int->Enable_Int();
}
/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/
void DisableInterruptVPC3Channel1(void)
{
        ///SSN  to be used in serial mode; This is added for testing purpose
        VPC3_ext_int->Disable_Int();
}
/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/
void SetResetVPC3Channel1( void )
{
        ///VPC3_RESET__;
        ///NOP__();
        VPC3_ctrl->Reset_On();
        VPC3_ctrl->Delay(20U);
        VPC3_ctrl->Delay(20U);
  
}//void SetResetVPC3Channel1( void )
/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/
void ClrResetVPC3Channel1(void)
{
        /// VPC3_CLEAR_RESET__;
        VPC3_ctrl->Reset_Off();
        VPC3_ctrl->Delay(20U);
}


void Init_LED_Interface(void)
{
	  profi_display_ctrl = new ProfiBus_Display( new BF_Misc::LED( new BF_Lib::Output_Pos( &PROFI_LED_SF_IO_CTRL ) ),
	                              new BF_Misc::LED( new BF_Lib::Output_Pos( &PROFI_LED_BF_IO_CTRL ) ),
	                              new BF_Misc::LED( new BF_Lib::Output_Pos( &PROFI_LED_Active_IO_CTRL ) ) );
}
/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/

void init_hardware( void )
{
        ///Here SPI is initialized
        spi_ctrl_VPC3 = new uC_SPI( EXT_MEM_SPI_PORT, TRUE, EXT_VPC3_CS_CTRLS,
                                NUM_EXT_ASIC_CS_CTRLS, &EXT_MEM_SPI_PIO, TRUE );
        /*in this the chipselect is configured and then there is a
        *basic check to read the status register for the VPC3+S, in this function low
        *level spi driver m_spi_ctrl->TX_Byte and m_spi_ctrl->RX_Byte are directly
        *called.
        */
        VPC3_ctrl = new VPC3( spi_ctrl_VPC3, VPC3_SPI_CS_NUM, &VPC3_CFG,new BF_Lib::Output_Pos( &PROFI_VPC3_RESET_IO_CTRL ), profi_display_ctrl );
        VPC3_ctrl->SetInterrupt(); //Set the VPC3 EXt INT Pin    
            
               
        /// Perform VPC3 reset for at least 300ns time
        /// Set reset hardware pin
        ///pVpc3Ctrl->Reset_On();
        ///pVpc3Ctrl->Delay(20);
        SetResetVPC3Channel1();    
        ///Reset is cleared later on in the code 
}
/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/
void VPC3_main( uint8_t  Slave_Addr )
{
        #ifdef Hyperterminal_Test_Enable        
        //TEST_FRAM_Init_FM25L04();
        //TEST_FRAM();
        uC_TEST_IO_Config_Test_Init2();
        uC_TEST_USART_Test_Init2();
        uC_TEST_AtoD_Test_Init2();
        uC_TEST_FRAM_Init2();
        #endif//Hyperterminal_Test_Enable
        init_hardware(); /// need to reset here
        #ifdef Hyperterminal_Test_Enable
        uC_Test_VPC3_Memory();
        #endif//Hyperterminal_Test_Enable
        Enable_Interrupts(); ///SSN Added 
        ProfibusInit(Slave_Addr);
}
/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/
void Swap16( UWORD MEM_PTR ptr_temp )
{
        ///Motorola <--> PC Format
        
        *ptr_temp = ( (*ptr_temp>>8) | (*ptr_temp<<8) );    // 1 2  <-->  2 1
}//void Swap16( UWORD MEM_PTR w )
