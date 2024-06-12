/**
 *****************************************************************************************
 * @file REST_FUS_Interface_Json.h
 * @details REST-FUS interface file help to get the execute REST operation.
 * @copyright 2022 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#ifndef __REST_FUS_INTERFACE_JSON_H__
#define __REST_FUS_INTERFACE_JSON_H__

#include <stdint.h>
#include "uC_Code_Range.h"
#include "NV_Ctrl.h"
#include "User_Session.h"
#include "Fw_Store.h"
#include "Prod_Spec_FUS.h"
#include "Json_Packetizer.h"

void User_Firmware_Upgrade_Info_Json( Fw_Upgrade_User_Function_Decl_1 get_user_info_cb,
									  Fw_Upgrade_User_Function_Decl_2 set_user_info_cb );

/**
 * @brief This function is used to update the user session handler.
 * @return None.
 */
void Fw_Upgrade_Session_Init_Handler_Json( User_Session* session );

/**
 * @brief This function is used to get the information of the processor.
 * @details This function returns the information of the processor when /rs/fw is requested.
 * @param[in] parser : Pointer to the hams parser.
 * @param[in] data : Pointer to the data
 * @param[in] length : length of the data to be parsed
 * @return None.
 */
uint32_t Prod_Info_Spec_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

uint32_t Parse_Fw_Resources_Cb_Json( hams_parser* parser, const uint8_t* data, uint32_t length );

/**
 * @brief This function is used to delete the active firmware session.
 * @param[in] None
 * @return None.
 */
void Exit_firmware_Session_Json( void );

/**
 * @brief This function is used to get the session id from the FUS.
 * @param[in] None
 * @return None.
 */
uint32_t Firmware_Session_Id_Json( void );

/**
 * @brief This function is used to allow the memory space for the firmware upgrade .
 * @param[in] None
 * @return None.
 */
bool Allocate_Rest_FUS_Buffer_Json( void );

/**
 * @brief This function is used to free the memory space used for the firmware session .
 * @param[in] None
 * @return None.
 */
void Deallocate_Rest_FUS_Buffer_Json( void );

#endif
