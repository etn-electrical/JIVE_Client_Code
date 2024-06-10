/**
*****************************************************************************************
*	@file VPC3_Main.h
*
*	@brief This file has all the VPC3 specific APIs.
*
*	@details The Profichip stack calls these APIs to read or write into the VPC3.
*
*	@note
*       This file contains the function  VPC3_main() which calls the Profibus stack in 
*       a while(1) loop.
*	@version
*	C++ Style Guide Version 1.0
*
*	@copyright 2012 Eaton Corporation. All Rights Reserved.
*
*****************************************************************************************
*/


#ifndef VPC3_Main_H
   #define VPC3_Main_H

#include "VPC3_platform.h"
#include "uC_Ext_Int.h"

#define NUM_EXT_ASIC_CS_CTRLS		1U



extern uC_Ext_Int *VPC3_ext_int;

/**
* @brief Initializes the registers of VPC3 chip.
* @details Initializes the registers of VPC3 chip.
* @param void
* @return void.
*/
void ProfibusInit( uint8_t Slave_Addr ); ///Added SSN

/**
* @brief This is the stack function to be called by the tasker.
* @details CPU should call this function every 600 usec.
* @param void
* @return void.
*/
void ProfibusMain(void); ///Added SSN

/**
* @brief Will write a single byte into the memory of VPC3 chip.
* @details This will write into the memory address passed as an arguement.
* @param 16 bit memory location.
* @param Single byte data which is to be written.
* @return True.
*/
//void Vpc3Write( VPC3_ADR wAddress, UBYTE bData );

/**
* @brief Will Read a single byte from the memory location of VPC3 chip.
* @details This will read from the memory address passed as an arguement.
* @param 16 bit memory location.
* @return Byte which is read from the memory location.
*/
//UBYTE Vpc3Read( VPC3_ADR wAddress );

/**
* @brief Will write a single byte into the subsequent memory locations of VPC3 chip.
* @details This will write starting from memory address passed as an arguement upto wLength count.
* @param 16 bit memory location starting address.
* @param Single value data which is to be written into multiple locations.
* @param wLength locations to be written.
* @return True.
*/
//void Vpc3MemSet( VPC3_ADR wAddress, UBYTE bValue, UWORD wLength );

/**
* @brief Will do bytewise comparison of two different memory blocks of VPC3 chip.
* @details This will compare the block starting from memory address pToVpc3Memory1 and pToVpc3Memory2
* and if not equal will return 1.
* @param 16 bit memory location starting address of the first block.
* @param 16 bit memory location starting address of the second block.
* @param Size of both blocks
* @return 1-> Memory blocks are different 0->Memory blocks are same.
*/
//UBYTE Vpc3MemCmp( VPC3_UNSIGNED8_PTR pToVpc3Memory1, VPC3_UNSIGNED8_PTR pToVpc3Memory2, UWORD wLength );

/**
* @brief Will copy bytes from the controller memory to VPC3 memory.
* @details this will copy wLenght bytes from the controller memory location to VPC3 location.
* @param 16 bit memory location starting address of the VPC3.
* @param memory address of the array in the controller 
* @param number of bytes to be copied
* @return void.
*/
//void CopyToVpc3( VPC3_UNSIGNED8_PTR pToVpc3Memory, MEM_UNSIGNED8_PTR pLocalMemory, UWORD wLength );

/**
* @brief Will copy bytes from VPC3 memory to the controller memory.
* @details this will copy wLenght bytes from VPC3 location to the controller memory location.
* @param 16 bit memory location starting address of the VPC3.
* @param memory address of the array in the controller 
* @param number of bytes to be copied
* @return void.
*/
//void CopyFromVpc3( MEM_UNSIGNED8_PTR pLocalMemory, VPC3_UNSIGNED8_PTR pToVpc3Memory, UWORD wLength );

/**
* @brief enables the external interrupts of the controller for VPC3
* @details enables the external interrupts of the controller for VPC3
* @param void.
* @return void.
*/
void EnableInterruptVPC3Channel1();

/**
* @brief disables the external interrupts of the controller for VPC3
* @details disables the external interrupts of the controller for VPC3
* @param void.
* @return void.
*/
void DisableInterruptVPC3Channel1();

/**
* @brief this will intialize SPI, VPC3 and will run Profibus stack in a while (l) loop.
* @return void.
*/
void VPC3_main( uint8_t  Slave_Addr );

/**
* @brief does the swapping of motorolla to PC format.
* @param pointer to the word to be swapped
* @return void.
*/
void Swap16( UWORD MEM_PTR ptr_temp );

/**
* @brief initializes the SPI and GPIOs.
* @param void.
* @return void.
*/
void init_hardware( void );

void Init_LED_Interface(void);
#endif //VPC3_Main_H
