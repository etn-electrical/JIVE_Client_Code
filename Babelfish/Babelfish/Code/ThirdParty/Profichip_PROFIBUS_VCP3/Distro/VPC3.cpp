/**
*****************************************************************************************
*	@file VPC3.cpp
*	@details See header file VPC3.h for module overview.
*	@copyright 2012 Eaton Corporation. All Rights Reserved.
*
*****************************************************************************************
*/

#include <string.h>
#include "Includes.h"
#include "uC_SPI.h"
#include "VPC3.h"
#include "OS_Tasker.h"
#include "uC_Ext_Int.h"
#include "VPC3_Main.h"
//#include "APP_IO_CONFIG_PROFI_STM32F101.h" SSN commented
#include "VPC3_dp_inc.h" //SSN added




/*
*****************************************************************************************
*		Constants
*****************************************************************************************
*/
#define VPC3_SMC_CSR_MASK	AT91C_SMC2_NWS								\
  							| AT91C_SMC2_WSEN 							\
							  | AT91C_SMC2_TDF 							\
                 				| AT91C_SMC2_BAT 						\
								  | AT91C_SMC2_DBW 						\
									| AT91C_SMC2_DRP					\
									  | AT91C_SMC2_ACSS 				\
										| AT91C_SMC2_RWSETUP			\
										  | AT91C_SMC2_RWHOLD 



#define     WAIT_STATE_ENABLED                  ((unsigned int) 1 << 7)
#define     DATA_FLOAT_TIME                     ((unsigned int) 0 << 8) 
#define     BYTE_ACCESS_TYPE_8                  ((unsigned int) 0 << 12) 
#define     DATE_BUS_WIDTH_8                    AT91C_SMC2_DBW_8 
#define     STANDARD_READ_PROTOCOL              ((unsigned int) 0 << 15) 
#define     STANDARD_ADDRESS_TO_CHIP_SELECT     AT91C_SMC2_ACSS_STANDARD 
#define     HALF_CYCLE_RWSETUP                  ((unsigned int) 0 << 24) 
#define     HALF_CYCLE_RWHOLD                   ((unsigned int) 0 << 28)



#define     SMC_VPC3_WAIT_TIME_NS		270		// 2* desired wait state time in ns
											// VPC3 requires 135ns; (2 * 135 = 270ns)

#define VPC3_READ_STATUS_REGISTER_OPCODE		0x13
#define VPC3_STATUS_REGISTER_ADRESS                     0x0005    // 005H     status register b8..b15



/*
*****************************************************************************************
*		Variables
*****************************************************************************************
*/
uC_Ext_Int *VPC3_ext_int;

/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/

VPC3::VPC3(uC_SPI* spi_ctrl, uint8_t chip_select, VPC3_CHIP_CONFIG_TD const* chip_config,BF_Lib::Output* reset_ctrl,ProfiBus_Display*  profi_display)
{
        m_reset_ctrl = reset_ctrl;
        m_profi_display = profi_display;
        
        m_spi_ctrl = spi_ctrl;
        
        m_chip_cfg = chip_config;
        
        m_chip_select = chip_select;
        
        m_spi_ctrl->Configure_Chip_Select( uC_SPI_MODE_3, chip_select, chip_config->max_clock_freq,
                                                                                uC_SPI_BIT_COUNT_8, TRUE ); //SSN uC_SPI_MODE_0-> uC_SPI_MODE_3
}

/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/
VPC3::~VPC3()
{
}
/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/
bool VPC3::TestRam(uint32_t start_addr, uint32_t length_in_2_bytes)
{
  uint16_t i=0, j=0;
  bool error_not_found = TRUE;
  volatile uint16_t *addr = NULL;
  
  *((uint8_t *) (VPC3_RAM_START + 0xC)) = 0x81; // set 4K RAM mode
  
  addr = (uint16_t *) start_addr;
  j = 0;
  for(i = 0; i < length_in_2_bytes;)
    {
        *addr =j;
        i++;
        j++;
        addr++;
    }

    j = 0;
  addr = (uint16_t *) start_addr;
    for(i = 0; i < length_in_2_bytes;)
    {
        if(*addr!=j)
        {
          error_not_found = FALSE;
        }
        i++;
        j++;
        addr++;
    }

  return error_not_found;
}

/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/
void VPC3::ClearRam(uint32_t start_addr, uint32_t length_in_bytes)
{
  memset((void *) start_addr, 0, length_in_bytes);
}
/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/
void Profibus_ISR(void)
{
	/*SSN The below term AT91C_BASE_PIOC->PIO_ISR is to be replaced by
        Micro101 specific Port. Temporarily commented  */
        //if ( /*AT91C_BASE_PIOC->PIO_ISR &*/ (1 << 17) )
	//{
		//OS_Tasker::Regular_Int_Enter();
          VPC3_ext_int->Clear_Int(); //SSN added to clear the external interrupt
		VPC3_Isr(); 
		//OS_Tasker::Regular_Int_Exit();
	//}
	//uC_Interrupt::Signal_End_Of_Int();
}

/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/
void VPC3::SetInterrupt(void)
{ 
        
        VPC3_ext_int = new  uC_Ext_Int(&PROFI_VPC3_EXT_INT_IO_CTRL);
        
        VPC3_ext_int->Attach_IRQ(Profibus_ISR,EXT_INT_FALLING_EDGE_TRIG,uC_IO_IN_CFG_HIZ,uC_INT_HW_PRIORITY_7);
        ///VPC3_ext_int->Clear_Int(); //SSN not required
        
        ///VPC3_ext_int->Enable_Int(); SSN this has been called in EnableInterruptVPC3Channel1(); 
}

/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/
void VPC3::VPC3_Update_Profi_Display( uint_fast8_t profi_state )
{
    m_profi_display->Update_ProfiBus_Display( profi_state );
}



/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/
bool VPC3::Read_Status( uint8_t* status )
{
	bool success = FALSE;
        DisableInterruptVPC3Channel1();
	if ( m_spi_ctrl->Get_Semaphore( VPC3_SPI_SEMAPHORE_TIMEOUT ) )
	{
		m_spi_ctrl->Select_Chip( m_chip_select );

		m_spi_ctrl->TX_Byte(VPC3_READ_STATUS_REGISTER_OPCODE);
		while ( !m_spi_ctrl->TX_Shift_Empty() );

                m_spi_ctrl->TX_Byte(VPC3_STATUS_REGISTER_ADRESS>>8);
                while ( !m_spi_ctrl->TX_Shift_Empty() );
                
                m_spi_ctrl->TX_Byte(VPC3_STATUS_REGISTER_ADRESS);
                while ( !m_spi_ctrl->TX_Shift_Empty() );
                              
		*status = m_spi_ctrl->RX_Byte();

		success = TRUE;

		m_spi_ctrl->De_Select_Chip( m_chip_select );
		m_spi_ctrl->Release_Semaphore();
	}
        EnableInterruptVPC3Channel1();
	return ( success );
}

/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/
bool  VPC3::Write( VPC3_ADR wAddress, unsigned char bData )
{
            bool success = FALSE;
            DisableInterruptVPC3Channel1();
        if ( m_spi_ctrl->Get_Semaphore( VPC3_SPI_SEMAPHORE_TIMEOUT ) )
	{
            m_spi_ctrl->Select_Chip( m_chip_select );
            
            m_spi_ctrl->TX_Byte(0x12);   // send command "WRITE_BYTE"
	    while ( !m_spi_ctrl->TX_Shift_Empty() );
            
            m_spi_ctrl->TX_Byte(wAddress>>8);
            while ( !m_spi_ctrl->TX_Shift_Empty() );
            
            m_spi_ctrl->TX_Byte(wAddress);
            while ( !m_spi_ctrl->TX_Shift_Empty() );
            
            m_spi_ctrl->TX_Byte(bData);
            while ( !m_spi_ctrl->TX_Shift_Empty() );

            success = TRUE;

	    m_spi_ctrl->De_Select_Chip( m_chip_select );
	    m_spi_ctrl->Release_Semaphore();
        }
        EnableInterruptVPC3Channel1();
	return ( success );	
}
/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/
void VPC3::MemSet(VPC3_ADR wAddress, UBYTE bValue, UWORD wLength )
{
        UWORD i;
        /* #if DP_ISOCHRONOUS_MODE
        DisableInterruptVPC3Sync();   
        #endif//#if DP_ISOCHRONOUS_MODE*/
        
        DisableInterruptVPC3Channel1();
        if ( m_spi_ctrl->Get_Semaphore( VPC3_SPI_SEMAPHORE_TIMEOUT ) )
        {
                m_spi_ctrl->Select_Chip( m_chip_select );
                
                m_spi_ctrl->TX_Byte(0x02);   /// send command "WRITE_Array"
                while ( !m_spi_ctrl->TX_Shift_Empty() );
                
                m_spi_ctrl->TX_Byte(wAddress>>8);
                while ( !m_spi_ctrl->TX_Shift_Empty() ); /// send high byte of address
                
                m_spi_ctrl->TX_Byte(wAddress);          /// send low byte of address
                while ( !m_spi_ctrl->TX_Shift_Empty() );
                
                for( i = 0; i < bValue; i++ )
                {
                        m_spi_ctrl->TX_Byte(bValue);   /// data is send
                        while ( !m_spi_ctrl->TX_Shift_Empty() ); /// wait end of transmition
                
                }//for( i = 0; i < wLength; i++ )
                
                m_spi_ctrl->De_Select_Chip( m_chip_select );
                m_spi_ctrl->Release_Semaphore();
        }
        
        EnableInterruptVPC3Channel1();
        
        /*#if DP_ISOCHRONOUS_MODE
        EnableInterruptVPC3Sync();   
        #endif//#if DP_ISOCHRONOUS_MODE*/
}
/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/
unsigned char VPC3::MemCmp( VPC3_UNSIGNED8_PTR pToVpc3Memory1, VPC3_UNSIGNED8_PTR pToVpc3Memory2, UWORD wLength )
{
        UBYTE bRetValue;
        UWORD i;
        DisableInterruptVPC3Channel1();
        bRetValue = 0;
        for( i = 0; i < wLength; i++ )
        {
                if( Vpc3Read( (VPC3_ADR)pToVpc3Memory1++ ) != Vpc3Read( (VPC3_ADR)pToVpc3Memory2++ ) )
                {
                        bRetValue = 1;
                        break;
                }///if( Vpc3Read( (VPC3_ADR)pToVpc3Memory1++ ) != Vpc3Read( (VPC3_ADR)pToVpc3Memory2++ ) )
        }///for( i = 0; i < wLength; i++ )
        EnableInterruptVPC3Channel1();
        return bRetValue;
}//UBYTE Vpc3::MemCmp( VPC3_UNSIGNED8_PTR pToVpc3Memory1, VPC3_UNSIGNED8_PTR pToVpc3Memory2, UWORD wLength )

/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/
void VPC3::CopyToVpc3( VPC3_UNSIGNED8_PTR pToVpc3Memory, MEM_UNSIGNED8_PTR pLocalMemory, UWORD wLength )
{
        UWORD wAddress;
        UWORD i;
        
        /*#if DP_ISOCHRONOUS_MODE
        DisableInterruptVPC3Sync();   
        #endif//#if DP_ISOCHRONOUS_MODE*/
        
        DisableInterruptVPC3Channel1();
        
        if ( m_spi_ctrl->Get_Semaphore( VPC3_SPI_SEMAPHORE_TIMEOUT ) )
        {
                m_spi_ctrl->Select_Chip( m_chip_select );
                
                
                m_spi_ctrl->TX_Byte(0x02);   /// send command "WRITE_Array"
                while ( !m_spi_ctrl->TX_Shift_Empty() ); /// wait end of transmition
                
                wAddress = (VPC3_ADR)pToVpc3Memory;
                
                m_spi_ctrl->TX_Byte(wAddress>>8);  /// send high byte of address
                while ( !m_spi_ctrl->TX_Shift_Empty() ); /// wait end of transmition
                
                m_spi_ctrl->TX_Byte(wAddress);          /// send low byte of address
                while ( !m_spi_ctrl->TX_Shift_Empty() ); /// wait end of transmition
                
                for( i = 0; i < wLength; i++ )
                {
                        m_spi_ctrl->TX_Byte(*pLocalMemory++);  /// send high byte of address
                        while ( !m_spi_ctrl->TX_Shift_Empty() ); /// wait end of transmition
                }//for( i = 0; i < wLength; i++ )
                
                m_spi_ctrl->De_Select_Chip( m_chip_select );
                m_spi_ctrl->Release_Semaphore();
        }
        EnableInterruptVPC3Channel1();
        
        /*#if DP_ISOCHRONOUS_MODE
        EnableInterruptVPC3Sync();   
        #endif//#if DP_ISOCHRONOUS_MODE*/
         
}

/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/
void VPC3::CopyFromVpc3( MEM_UNSIGNED8_PTR pLocalMemory, VPC3_UNSIGNED8_PTR pToVpc3Memory, UWORD wLength )
{
        UWORD wAddress;
        UWORD i;
        
        /*#if DP_ISOCHRONOUS_MODE
        DisableInterruptVPC3Sync();   
        #endif//#if DP_ISOCHRONOUS_MODE*/
        
        DisableInterruptVPC3Channel1();
        if ( m_spi_ctrl->Get_Semaphore( VPC3_SPI_SEMAPHORE_TIMEOUT ) )
        {
                m_spi_ctrl->Select_Chip( m_chip_select );
                
                m_spi_ctrl->TX_Byte(0x03);   /// send command "READ_Array"
                while ( !m_spi_ctrl->TX_Shift_Empty() ); /// wait end of transmition
                
                wAddress = (VPC3_ADR)pToVpc3Memory;
                
                m_spi_ctrl->TX_Byte(wAddress>>8);  /// send high byte of address
                while ( !m_spi_ctrl->TX_Shift_Empty() ); /// wait end of transmition
                
                m_spi_ctrl->TX_Byte(wAddress);          /// send low byte of address
                while ( !m_spi_ctrl->TX_Shift_Empty() ); /// wait end of transmition
                
                for( i = 0; i < wLength; i ++ )
                {
                pLocalMemory[ i ] = m_spi_ctrl->RX_Byte();     /// data is send to generate SCK signal
                }//for( i = 0; i < wLength; i ++ )
                
                m_spi_ctrl->De_Select_Chip( m_chip_select );
                m_spi_ctrl->Release_Semaphore();
        }
        EnableInterruptVPC3Channel1();
        
        /*#if DP_ISOCHRONOUS_MODE
        EnableInterruptVPC3Sync();   
        #endif//#if DP_ISOCHRONOUS_MODE*/
}///void VPC3::CopyFromVpc3( MEM_UNSIGNED8_PTR pLocalMemory, VPC3_UNSIGNED8_PTR pToVpc3Memory, UWORD wLength )


/*
*****************************************************************************************
* See header file for function definition.
*****************************************************************************************
*/
unsigned char VPC3::Read( VPC3_ADR wAddress )
{
        unsigned char bData;
        DisableInterruptVPC3Channel1();
        //uC_Delay( 50 );
        if ( m_spi_ctrl->Get_Semaphore( VPC3_SPI_SEMAPHORE_TIMEOUT ) )
        {
                m_spi_ctrl->Select_Chip( m_chip_select );
                
                m_spi_ctrl->TX_Byte(VPC3_READ_STATUS_REGISTER_OPCODE);
                while ( !m_spi_ctrl->TX_Shift_Empty() );
                
                m_spi_ctrl->TX_Byte(wAddress>>8);
                while ( !m_spi_ctrl->TX_Shift_Empty() );
                
                m_spi_ctrl->TX_Byte(wAddress);
                while ( !m_spi_ctrl->TX_Shift_Empty() );
                              
                bData = m_spi_ctrl->RX_Byte();
                
                m_spi_ctrl->De_Select_Chip( m_chip_select );
                m_spi_ctrl->Release_Semaphore();
        }
        //uC_Delay( 50 );
        EnableInterruptVPC3Channel1();
        return bData;
}
UINT8 VPC3::getProfiCommStatus( uint8_t profi_state )
{
    static UINT8 failTimeCount = 0;
    static UINT8 status = 0;

    profi_state = profi_state & 0xF0 ;

    switch( (profi_state ) )
    {
      case DATA_EX:
            failTimeCount =0;
            status = 0;
            break;
      case WAIT_PRM:
      case WAIT_CFG:
      case DP_ERROR:
      default:
            failTimeCount++;
            if( failTimeCount>10 )      /* 5ms*10 = 50 milisecond timeout */
            {
                failTimeCount =0;
                status = 1;
            }
        break;
    }
  return(status);
}
