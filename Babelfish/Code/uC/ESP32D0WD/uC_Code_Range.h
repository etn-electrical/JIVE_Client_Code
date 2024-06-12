/**
 *****************************************************************************************
 *	@file uC_Code_Range.h
 *
 *	@brief Functions prototype for returning partition table offset.
 *
 *	@details These function are providing the address(Offset) for various partition in ESP32
 *
 *	@version C++ Style Guide Version 1.0
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef uC_CODE_RANGE_H
#define uC_CODE_RANGE_H

/**
 * @brief						Provides DCI NV start address
 * @param[in]					void
 * @return nv_status_t			status of requested operation.
 */
uint8_t* uC_DCI_NV_Start_Address( void );

/**
 * @brief						Provides DCI NV end address
 * @param[in]					void
 * @return nv_status_t			status of requested operation.
 */
uint8_t* uC_DCI_NV_End_Address( void );

/**
 * @brief						Provides running partition start address for FUS.
 * @param[in]					void
 * @return nv_status_t			status of requested operation.
 */
uint8_t* uC_App_Fw_Start_Address( void );

/**
 * @brief						Provides running partition end address for FUS.
 * @param[in]					void
 * @return nv_status_t			status of requested operation.
 */
uint8_t* uC_App_Fw_End_Address( void );

/**
 * @brief                       Provides web image start address for FUS from created partition.
 * @param[in]                   void
 * @return nv_status_t          status of requested operation.
 */
uint8_t* uC_Web_Fw_Start_Address( void );

/**
 * @brief                       Provides web image end address for FUS from created partition.
 * @param[in]                   void
 * @return nv_status_t          status of requested operation.
 */
uint8_t* uC_Web_Fw_End_Address( void );

/**
 * @brief                       Creates new partition in case partition is not available for given label
 * @param[in]offset             Starting address
 * @param[in]size               Partition size
 * @param[in]label              Partition label
 * @return esp_err_t            Status of the partition creation success or failure.
 * @note                        We have used web_image as a label just for reference,
 *                              we need to add new partition label as per our need in else condition
 *                              similar to web_image label
 */
esp_err_t uC_Create_Partition( size_t offset, size_t size, const char* label );

#endif	// #ifndef uC_CODE_RANGE_H
