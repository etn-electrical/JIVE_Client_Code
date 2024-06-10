/**
This is an example for SPI transfer through DMA

Convert this file to .cpp and add a .h file with function
void Test_SPI_DMA(bool b_dmaTranferInterrupt) defined in it.

This file is master in SPI communication.

A slave can be programmed to transmit data on SPI after
 receiving string from this master

This has been tested for STM32F303
**/

#include "Includes.h"
#include "uC_SPI.h"
#include "App_IO_Config.h"
#include "Prod_Spec_SPI_DMA.h"		// for exported function void Test_SPI_DMA(bool b_dmaTranferInterrupt);
#include "StdLib_MV.h"


uC_USER_IO_STRUCT const* const EXAMPLE_SPI_CS_CTRLS[EXAMPLE_SPI_CS_NUM] =
{ &EXAMPLE_SPI_CS_IO_CTRL };

#define _100_CHARACTERS_  "*0123456789*abcdefghijklmnopqrstuvwxyz*0123456789*0123456789*ABCDEFGHIJKLMNOPQRSTUVWXYZ*0123456789*E"

uint8_t aTxBuffer[] = {_100_CHARACTERS_};
/* Size of buffer */
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
#define BUFFERSIZE                       (COUNTOF(aTxBuffer) - 1)
/* Buffer used for reception */
uint8_t aRxBuffer[BUFFERSIZE];


static uC_SPI* st_spi_handle = NULL;

/**
*	this is a Receive callback function which defines what action user wants
*	after an interrupt is raised after receive complete data of desired length
*	@param handle:	
*			This will be passed while resistering this callback function
**/
void my_RX_DMA_callback(uC_SPI::cback_param_t handle)
{
  if(handle != NULL)
  {
        ((uC_SPI*)handle)->Disable_XChange_DMA();
  }
  
  if(BF_Lib::String_Compare((uint8_t*)aTxBuffer, (uint8_t*)aRxBuffer, BUFFERSIZE))
  {
	  // Processing Success in match as per User's need         
  }
  else
  {
	  // Processing Failure in match as per User's need   
  }
}

/**
*	this is a Transmit callback function which defines what action user wants
*	after an interrupt is raised after Tranmission of complete data of desired length
*	@param handle:	
*			This will be passed while resistering this callback function
**/
void my_TX_DMA_callback(uC_SPI::cback_param_t handle)
{
	//function just for illustration
	//User can add its own code 
}


/*********************** Test_SPI_DMA***********************/
//Test_SPI_DMA is an example function
//			
//	    ///////use this in PROD_SPEC_Main_Init() for example//////////
//      #include "Prod_Spec_SPI_DMA.h"
//	    Test_SPI_DMA( bool b_dmaTranferInterrupt);
//	    ///////use this in PROD_SPEC_Main_Init/////////////////////
/*********************** Test_SPI_DMA***********************/
void Test_SPI_DMA(bool b_dmaTranferInterrupt)
{
	st_spi_handle = new uC_SPI
						(
                          	EXAMPLE_SPI_PORT, 				/** SPI peripheral# **/
						  	true, 							/** Master mode **/
						  	&EXAMPLE_SPI_CS_CTRLS[0], 		/** CS GPIOs **/
						  	1U, 							/** Total CS **/
						  	&EXAMPLE_SPI_PIO, 				/** MOSI MISO CLK GPIOs **/
						  	TRUE 							/** DMA need **/
						);
  

	st_spi_handle->Configure_Chip_Select
						(
						 	static_cast <uint8_t>( uC_SPI_MODE_0 ), /** CPHA_CPOL_MODE **/
                            0,                          			/**EXAMPLE_SPI_CS_NUM CS number **/	
                            1000000U,                   			/** 1 MhZ from Cube code SPI_BR **/
                            uC_SPI_BIT_COUNT_8,         			/** 8 bit data transfer **/
                            false                       			/**manual CS false as cube code has NSS implemented**/
						);
	
	//if( st_spi_handle->Get_Semaphore( SPI_SEMAPHORE_TIMEOUT ) ) //
	//{
	st_spi_handle->Select_Chip( 0 ); 
	
	st_spi_handle->Set_XChange_DMA
						(
						 	aTxBuffer,
							aRxBuffer,
							static_cast <uint16_t>( BUFFERSIZE ) 
						);
	
	if(b_dmaTranferInterrupt)	//for DMA with interuupt
	{ 
		st_spi_handle->Set_Rx_DMA_Callback( &my_RX_DMA_callback, st_spi_handle,  uC_INT_HW_PRIORITY_6);
		st_spi_handle->Set_Tx_DMA_Callback( &my_TX_DMA_callback, st_spi_handle,  uC_INT_HW_PRIORITY_6);
		st_spi_handle->Enable_XChange_DMA_Interrupt();
		st_spi_handle->Enable_XChange_DMA();
	}
	else	//for DMA without interuupt
	{
		st_spi_handle->Enable_XChange_DMA();
		
		while ( st_spi_handle->XChange_DMA_Busy() )
		{}     //!spi_ctrlspi_ctrl->TX_Shift_Empty() ){};
		
		st_spi_handle->Disable_XChange_DMA();
		
		if(BF_Lib::String_Compare((uint8_t*)aTxBuffer, (uint8_t*)aRxBuffer, BUFFERSIZE))
		{
			// Processing Success in match as per User's need         
		}
		else
		{
			// Processing Failure in match as per User's need   
		}
	}
	
	//} // end if( st_spi_handle->Get_Semaphore( SPI_SEMAPHORE_TIMEOUT ) )
}