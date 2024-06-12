/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
/**
 *****************************************************************************************
 *	@file VPC3.h
 *
 *	@brief This file gives access to the VPC3 function.
 *
 *	@details It gives lower level access functions
 *
 *	@note
 *	Further suggestions:
 *       Creator: Shafiq Navodia
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2012 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef VPC3_H
   #define VPC3_H

#include "Output.h"
#include "VPC3_platform.h"
// #include "VPC3_dp_inc.h"
#include "LED.h"
#include "DCI_Patron.h"
#include "ProfiBus_Display.h"
#include "uC_SPI.h"

void vpc3_isr( void );

void VPC3_main( void );

/*
 *****************************************************************************************
 *		Typedefs
 *****************************************************************************************
 */
typedef struct VPC3_CHIP_CONFIG_TD
{
	uint8_t address_length;
	uint32_t start_address;
	uint32_t mirror_start_address;
	uint32_t end_address;
	uint32_t max_clock_freq;
} VPC3_CHIP_CONFIG_TD;
/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

#define VPC3_RAM_START              ( 0x10000000 )
#define VPC3_SEMAPHORE_TIMEOUT          2000
#define VPC3_SPI_SEMAPHORE_TIMEOUT      2000
/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
/**
 ****************************************************************************************
 * @brief This VPC3 class will give access to the chip and its dual port memory.
 *
 * @details
 *
 * @note
 *
 *
 ****************************************************************************************
 */
class VPC3
{

	public:

		/**
		 *  @brief Constructor initializes the SPI signals.
		 *  @param The pointer of the SPI control signals.
		 *  @param The chipselect signal passed.
		 *  @param The chip configuration passed.
		 *  @param The control of the reset signal to th VPC3
		 *  @param The control of the Profibus Leds.
		 *  @return void.
		 */
		VPC3( uC_SPI* spi_ctrl, uint8_t chip_select, VPC3_CHIP_CONFIG_TD const* chip_config, BF_Lib::Output* reset_ctrl,
			  ProfiBus_Display* profi_display_ctrl );

		/**
		 *  @brief Destructor of the VPC3.
		 *  @return void.
		 */
		~VPC3();

		/**
		 *  @brief This function updates the Profibus Leds.
		 *  @param The state of the Profibus Disply is passed.
		 *  @return void.
		 */
		void VPC3_Update_Profi_Display( uint_fast8_t profi_display );

		/**
		 *  @brief Tests the dual port Ram of VPC3.
		 *  @param The start address of the Ram.
		 *  @param The length in Bytes.
		 *  @return void.
		 */

		bool TestRam( uint32_t start_addr, uint32_t length_in_2_bytes );

		/**
		 *  @brief Clears the dual port Ram of VPC3.
		 *  @param The start address of the Ram.
		 *  @param The length in Bytes.
		 *  @return void.
		 */
		void ClearRam( uint32_t start_addr, uint32_t length_in_bytes );

		void SetInterrupt( void );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Reset_On()
		{
			m_reset_ctrl->On();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Reset_Off()
		{
			m_reset_ctrl->Off();
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Delay( uint8_t count_in_50ns )
		{
			while ( count_in_50ns-- )
			{
				uC_Delay( 50 );
			}
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline void Reset( uint16_t count1_in_50ns, uint16_t count2_in_50ns )
		{
			Reset_On();
			Delay( count1_in_50ns );
			Reset_Off();
			Delay( count2_in_50ns );
		}

		bool Write_Status( uint8_t status );

		bool Read_Status( uint8_t* status );

		bool Write( VPC3_ADR wAddress, unsigned char bData );

		unsigned char Read( VPC3_ADR wAddress );

		void MemSet( VPC3_ADR wAddress, UBYTE bValue, UWORD wLength );

		unsigned char MemCmp( VPC3_UNSIGNED8_PTR pToVpc3Memory1, VPC3_UNSIGNED8_PTR pToVpc3Memory2, UWORD wLength );

		void CopyToVpc3( VPC3_UNSIGNED8_PTR pToVpc3Memory, MEM_UNSIGNED8_PTR pLocalMemory, UWORD wLength );

		void CopyFromVpc3( MEM_UNSIGNED8_PTR pLocalMemory, VPC3_UNSIGNED8_PTR pToVpc3Memory, UWORD wLength );

		UINT8 getProfiCommStatus( uint8_t profi_state );

	private:
		BF_Lib::Output* m_reset_ctrl;
		ProfiBus_Display* m_profi_display;
		DCI_Patron* m_dci_access;
		DCI_Patron* m_dci_access_output;
		uint16_t Calculate_nws( void );	// (SDT) 6/30/09

		uC_SPI* m_spi_ctrl;
		uint8_t m_chip_select;
		VPC3_CHIP_CONFIG_TD const* m_chip_cfg;

};


#endif	// #ifndef VPC3_H

