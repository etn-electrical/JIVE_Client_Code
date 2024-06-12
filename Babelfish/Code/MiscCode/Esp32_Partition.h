/**
 *****************************************************************************************
 *  @file  Esp32_Partition.h
 *	@brief Esp32_Partition class.
 *
 *	@details This class shall be responsible to provide partition information.
 *
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef ESP32_PARTITION_H
#define ESP32_PARTITION_H

#include "esp_partition.h"
#include "esp_spi_flash.h"
#include "esp_app_format.h"
#include "esp_err.h"
#include "Babelfish_Assert.h"
#include "esp_ota_ops.h"

class ESP32_Partition
{
	public:

		ESP32_Partition();

		~ESP32_Partition( void );

		/*
		 *****************************************************************************************
		 * @brief Return the next OTA app partition which should be written with a new firmware.
		 * Finds next partition round-robin, starting from the current running partition.
		 * @ param[in]	passive_ota	Pointer having partition structure information related to next
		 * ota partition.
		 * @ param[in]	start_from If set, treat this partition info as describing the current running partition. Can be
		 * NULL, in which case esp_ota_get_running_partition() is used to find the currently running partition.
		 * @ return status of requested operation
		 *****************************************************************************************
		 */
		static bool Get_Next_Update_Partition( esp_partition_t* passive_ota, const esp_partition_t* start_from );

		/*
		 *****************************************************************************************
		 * @brief Get partition info of currently running app.
		 * @ param[in]	current_ota	Pointer having partition structure information related to
		 * current ota partition.
		 * @ return status of requested operation
		 *****************************************************************************************
		 */
		static bool Get_Running_Partition( esp_partition_t* current_ota );

		/*
		 *****************************************************************************************
		 * @ brief Gives Configure OTA data for a new boot partition.
		 * @ param[in]	partition Pointer to info for partition containing app image to boot.
		 * @ return status of requested operation
		 *****************************************************************************************
		 */
		static bool Ota_Set_Boot_Partition( const esp_partition_t* partition );


		/*
		 *****************************************************************************************
		 * @ brief Gives NV Config partition details..
		 * @ param[in]	nv_config	Pointer pointing to pointer having partition structure information related to
		 * NV Config partition.
		 * @ return status of requested operation
		 *****************************************************************************************
		 */
		static bool Get_Partition_Details( esp_partition_t* partition, esp_partition_type_t type,
										   esp_partition_subtype_t subtype, const char* label );

		/*
		 ****************************************************************************************
		 * @brief                       Creates new partition in case partition is not available for given label
		 * @param[in]offset             Starting address
		 * @param[in]size               Partition size
		 * @param[in]label              Partition label
		 * @param[in]type               Type of the partition, data or app
		 * @param[in]subtype            Subtype of the partition
		 * @return esp_err_t            Status of the partition creation success or failure.
		 * @note
		 ****************************************************************************************
		 */
		static esp_err_t Create_Partition( size_t offset, size_t size, const char* label, esp_partition_type_t type,
										   esp_partition_subtype_t subtype );

};

#endif
