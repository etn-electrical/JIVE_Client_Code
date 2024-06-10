/**
 *****************************************************************************************
 *	@file		DCI_Ram_Owner_Example.h
 *
 *	@brief		The file contains the DCI Owner,Patron basic testing for RAM functionality
 *
 *****************************************************************************************
 */


#ifndef DCI_RAM_EG
#define DCI_RAM_EG

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief		Main Function to test basic DCI owner functions
 * @return Void	None
 */
void DCI_RAM_Test_App_Main();

/**
 * @brief		Function to test basic DCI owner Check IN & OUT Data variables functions
 */

void DCI_CheckIn_CheckOut_Data_Variables_Test( void );

/**
 * @brief		Function to test basic DCI owner retrive attribute
 * @return void	None
 */

void DCI_Test_Owner_Attributes( void );

/**
 * @brief		Function to test basic DCI owner Lock & Unlock
 * @return void	None
 */

void DCI_owner_Test_Lock_Unlock( void );

/**
 * @brief		Function to test basic DCI owner Default check In & Out
 * @return void	None
 */

void DCI_CheckIn_Default_CheckOut_Default_Test( void );

/**
 * @brief		Function to test basic DCI Patron access data length, data type
 * @return void	None
 */
void DCI_Patron_access_test( void );

#ifdef __cplusplus
}
#endif

#endif	// DCI_RAM_EG
