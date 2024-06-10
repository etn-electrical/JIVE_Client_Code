/**
 *****************************************************************************************
 * @file    Esp32_Partition.cpp
 * @details See header file for module overview.
 * @copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "includes.h"
#include "Esp32_Partition.h"
#include "Breaker.h"

//Ahmed
extern DRAM_ATTR Device_Info_str DeviceInfo;


ESP32_Partition::ESP32_Partition( void )
{}

/*
 *
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool ESP32_Partition::Get_Next_Update_Partition( esp_partition_t* passive_ota,
												 const esp_partition_t* start_from )
{
	esp_partition_t default_ota = { };
	const esp_partition_t* next_ota = nullptr;
	bool next_is_result = false;
	bool ret = false;

	if ( start_from == nullptr )
	{
		start_from = &default_ota;
		ret = Get_Running_Partition( &default_ota );
	}
	else
	{
		ret = true;
		/* Verifies partition data */
		start_from = esp_partition_verify( start_from );
	}
	BF_ASSERT( start_from != nullptr );
	if ( ret == true )
	{
		/* Loops for going through different ESP32 OTA Partition starting from OTA 0 */
		for ( esp_partition_subtype_t sub_t = ESP_PARTITION_SUBTYPE_APP_OTA_0;
			  sub_t < ESP_PARTITION_SUBTYPE_APP_OTA_MAX;
			  ( sub_t = ( static_cast<esp_partition_subtype_t>( sub_t + 1 ) ) ) )
		{
			/* Finds the first partition based on one or more parameters */
			const esp_partition_t* partition_check = esp_partition_find_first( ESP_PARTITION_TYPE_APP, sub_t, nullptr );
			if ( partition_check == nullptr )
			{
				continue;
			}

			if ( next_ota == nullptr )
			{
				/* Default to first OTA partition we find,
				   will be used if nothing else matches */
				next_ota = partition_check;
			}

			if ( partition_check->address == start_from->address )
			{
				/* Next OTA partition is the one to use */
				next_is_result = true;
			}
			else if ( next_is_result )
			{
				next_ota = partition_check;
				break;
			}
		}

		*passive_ota = *next_ota;
		/* checking for nullptr in passive_ota will not be enough as members of the structure can also be NULL */
		if ( ( passive_ota != nullptr ) && ( passive_ota->address != 0U ) )
		{
			ret = true;
		}
		else
		{
			ret = false;
		}
	}
	return ( ret );
}

/*
 *
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool ESP32_Partition::Get_Running_Partition( esp_partition_t* current_ota )
{
	bool ret = false;
	/* Find the flash address of this exact function. By definition that is part
	   of the currently running firmware. Then find the enclosing partition. */
	size_t phys_offs = spi_flash_cache2phys( reinterpret_cast<void*>( Get_Running_Partition ) );
	/* Find partition based on one or more parameters. */
	esp_partition_iterator_t it = esp_partition_find( ESP_PARTITION_TYPE_APP,
													  ESP_PARTITION_SUBTYPE_ANY,
													  nullptr );

	while ( it != nullptr )
	{
		/* Get esp_partition_t structure for given partition */
		const esp_partition_t* partition_check_running = esp_partition_get( it );
		if ( ( partition_check_running->address <= phys_offs ) &&
			 ( partition_check_running->address + partition_check_running->size > phys_offs ) )
		{
			/* Releases partition iterator */
			esp_partition_iterator_release( it );
			*current_ota = *partition_check_running;
			break;
		}
		/* Move partition iterator to the next partition found */
		it = esp_partition_next( it );
	}
	/* checking for nullptr in current_ota will not be enough as members of the structure can also be NULL */
	if ( ( current_ota != nullptr ) && ( current_ota->address != 0U ) )
	{
		ret = true;
	}
	else
	{
		ret = false;
	}
	return ( ret );
}

/*
 *
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool ESP32_Partition::Ota_Set_Boot_Partition( const esp_partition_t* partition )
{
    //Ahmed
	bool ret = false;
    //Ahmed
    if (DeviceInfo.OTA_State >= OTA_START)
    {
        DeviceInfo.OTA_State = OTA_BOOT_WRITE;
    }

	esp_err_t err = esp_ota_set_boot_partition( partition );

	if ( err == ESP_OK )
	{

		ret = true;
	}

	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool ESP32_Partition::Get_Partition_Details( esp_partition_t* partition, esp_partition_type_t type,
											 esp_partition_subtype_t subtype, const char* label )
{
	bool ret = false;
	/* Find partition based on one or more parameters. */
	esp_partition_iterator_t it = esp_partition_find( type,
													  subtype, label );

	/* Get esp_partition_t structure for given partition */
	const esp_partition_t* part = esp_partition_get( it );

	/* Releases partition iterator */
	esp_partition_iterator_release( it );
	*partition = *part;

	/* checking for nullptr in partition will not be enough as members of the structure can also be NULL */
	if ( ( partition != nullptr ) && ( partition->address != 0U ) )
	{
		ret = true;
	}
	else
	{
		ret = false;
	}
	return ( ret );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t ESP32_Partition::Create_Partition( size_t offset, size_t size, const char* label, esp_partition_type_t type,
											 esp_partition_subtype_t subtype )
{
	/*
	 * Disadvantages:
	 * 1. offset ( starting address ) which we are using here, we don't have memory check for overlapping.
	 *    So, we need to take extra care here.
	 * 2. Once partition is created, it is not seen on terminal like other partitions in partition table.
	 *
	 */

	const esp_partition_t* fat_partition = nullptr;
	/*
	 * Set to ESP_ERR_INVALID_ARG.
	 * In case partition is already available then return the same
	 */
	esp_err_t err = ESP_ERR_INVALID_ARG;

	/*
	 * Find partition based on one or more parameters.
	 */
	esp_partition_iterator_t it = esp_partition_find( type, subtype, label );

	/*
	 * Partition is not available so create one
	 */
	if ( nullptr == it )
	{
		err = esp_partition_register_external( esp_flash_default_chip, offset, size, label, type,
											   subtype, &fat_partition );
	}

	return ( err );
}
