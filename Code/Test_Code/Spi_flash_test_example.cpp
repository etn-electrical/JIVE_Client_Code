/**
 **********************************************************************************************
 * @file			spi_flash_test_app_main.cpp
 * @brief			The following examples demonstrates the testing of Flash module.
 **********************************************************************************************
 */
#include "Spi_flash_test_example.h"
#include "Babelfish_Assert.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
const NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD ESP_SCRATCH_IMAGE_CONFIG =
{
	reinterpret_cast<uint32_t>( uC_Scratch_Area_Start_Address() ),
	reinterpret_cast<uint32_t>( uC_Scratch_Area_End_Address() ),
	reinterpret_cast<uint32_t>( uC_Scratch_Area_End_Address() ),
};

static const uint32_t DATA_LEN = 256U;							///< Test Data Length.
static const uint32_t ASYNC_WAIT_DELAY_VAL = 1U;				///< ASYNC Operation Wait Delay value.
static const uint8_t ENABLE_VAL = 1U;							///< Enable Value.
static const uint8_t DISABLE_VAL = 0U;							///< Disable Value.
static const uint32_t SECTOR_SIZE = SPI_FLASH_SEC_SIZE;			///< SPI Flash Sector Size.
uint32_t parallel_task_count = 0U;

static uint32_t async_operation_completion_flag;				///< ASYNC Operation completion flag.
static NV_Ctrl::nv_status_t async_task_nv_ctrl_result;			///< ASYNC Operation status result.

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Sync_Read_Write_Test( void )
{
	uint32_t write_data_buffer[DATA_LEN];
	uint8_t return_status = NV_Ctrl::DATA_ERROR;

	/*Fill a write data buffer with random bytes from hardware RNG*/
	esp_fill_random( write_data_buffer, DATA_LEN );
	uint32_t flash_addr = ESP_SCRATCH_IMAGE_CONFIG.start_address;
	uint32_t read_data_buffer[DATA_LEN] = { 0U };
	uint32_t data_size = DATA_LEN * sizeof( uint32_t );
	NV_Ctrl* esp_flash = nullptr;


	esp_flash =
		new NV_Ctrls::uC_Flash( reinterpret_cast<NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD const*>( &ESP_SCRATCH_IMAGE_CONFIG ),
								nullptr );

	if ( esp_flash != nullptr )
	{
		return_status =
			esp_flash->Write( reinterpret_cast<uint8_t*>( write_data_buffer ), flash_addr, data_size, false );
		if ( return_status == NV_Ctrl::SUCCESS )
		{
			return_status = esp_flash->Read( reinterpret_cast<uint8_t*>( read_data_buffer ), flash_addr, data_size,
											 false );
			if ( return_status == NV_Ctrl::SUCCESS )
			{
				if ( memcmp( write_data_buffer, read_data_buffer, DATA_LEN ) )
				{
					printf( "Data from Read( ) and Write( ) is not equal \r\n" );
				}
				else
				{
					printf( "Data from Read( ) and Write( ) is equal \r\n" );
				}
			}
			else
			{
				printf( "Failed to read data from flash memory, return_status = %d\r\n", return_status );
			}
		}
		else
		{
			printf( "Failed to write data to flash memory, return_status = %d\r\n", return_status );
		}

		if ( esp_flash->Erase_All() == NV_Ctrl::SUCCESS )
		{
			printf( "Sync Erase All Flash successfully \r\n" );
			memset( read_data_buffer, 0U, data_size );
			/* Read Data on flash after flash erase*/
			return_status = esp_flash->Read( reinterpret_cast<uint8_t*>( read_data_buffer ), flash_addr, data_size,
											 false );
			if ( return_status == NV_Ctrl::SUCCESS )
			{
				bool erase_flash_status = false;
				uint32_t data_to_compare = 0xFFFFFFFF;
				for ( uint32_t index = 0; index < DATA_LEN; index++ )
				{
					if ( read_data_buffer[index] != data_to_compare )
					{
						printf( "Sync Erase All Flash verification failed\r\n" );
						erase_flash_status = false;
						break;
					}
					else
					{
						erase_flash_status = true;
					}
				}

				if ( erase_flash_status )
				{
					printf( "Sync Erase All Flash verified successfully\r\n" );
				}
			}
			else/* Else part of Read Data on flash */
			{
				printf( "Failed to read data from flash memory while verification\r\n" );
			}
		}
		else
		{
			printf( "Error: Sync Erase All failed\r\n" );
		}
		delete esp_flash;
	}
}

/**
 * @brief						Call-back function for Asynchronous uC Flash Operation.
 * @n							It will Call from uC Flash for asynchronous operation from Handler
 * @n							when asynchronous operation perform.
 * @param[in] param:			Call back parameter. A void pointer to an object passed as an argument to the call-back
 * @n							function.
 * @param[in] result:			NV Ctrl result from asynchronous uC flash operation.
 * @return void					None
 */
static void Flash_Test_Callback( NV_Ctrl::cback_param_t param, NV_Ctrl::nv_status_t result )
{
	uint32_t* async_operation_completion_flag_param = nullptr;

	async_operation_completion_flag_param = reinterpret_cast<uint32_t*>( param );
	*async_operation_completion_flag_param = ENABLE_VAL;
	async_task_nv_ctrl_result = result;
}

/**
 * @brief						Wait for the Flash Operation in Asynchronous mode
 * @param[in] void:				None
 * @return bool					returns Asynchronous operation status
 */
bool Wait_Flash_Operation()
{
	bool return_value = false;

	while ( async_operation_completion_flag != ENABLE_VAL )
	{
		parallel_task_count++;
		vTaskDelay( ASYNC_WAIT_DELAY_VAL / portTICK_RATE_MS );
	}

	if ( NV_Ctrl::SUCCESS == async_task_nv_ctrl_result )
	{
		return_value = true;
	}

	return ( return_value );
}

/**
 * @brief						Erase All(Partition) function for Asynchronous uC Flash Operation.
 * @param[in] Esp_uC_Flash:		NV_Ctrls Class Pointer.
 * @return bool					return true for success and false for fail.
 */
static bool Async_Erase_All_Flash( NV_Ctrls::uC_Flash* esp_uc_flash )
{
	NV_Ctrl::nv_status_t nv_return_status = NV_Ctrl::SUCCESS;

	parallel_task_count = DISABLE_VAL;
	bool return_flag = false;

	/* Erase All Flash Operation in Asynchronous mode */
	async_operation_completion_flag = DISABLE_VAL;

	printf( "Erase All Flash : Waiting for Asynchronous Erase All Operation...\r\n" );
	nv_return_status = esp_uc_flash->Erase_All( Flash_Test_Callback,
												( NV_Ctrl::cback_param_t ) &async_operation_completion_flag );
	if ( nv_return_status == NV_Ctrl::PENDING_CALL_BACK )
	{
		/* Wait for the Erase Flash Operation in Asynchronous mode */
		if ( Wait_Flash_Operation() )
		{
			printf( "Asynchronous Erase Operation completed (parallel_task_count=%d)\r\n", parallel_task_count );
			return_flag = true;
		}
	}
	else
	{
		printf( "Erase All failed with return status %d\r\n", nv_return_status );
	}

	return ( return_flag );
}

/**
 * @brief						Erase Data function for Asynchronous uC Flash Operation.
 * @param[in] Esp_uC_Flash:		NV_Ctrls Class Pointer.
 * @param[in] flash_addr:		Flash memory address.
 * @param[in] data_length:		Data size.
 * @return bool					return true for success and false for fail.
 */
static bool Async_Erase_Flash( NV_Ctrls::uC_Flash* esp_uc_flash, uint32_t flash_addr, uint32_t data_length )
{
	NV_Ctrl::nv_status_t nv_return_status = NV_Ctrl::SUCCESS;

	parallel_task_count = DISABLE_VAL;
	bool return_flag = false;

	/* Erase Flash Operation in Asynchronous mode */
	async_operation_completion_flag = DISABLE_VAL;

	printf( "Erase Flash (data size = %d) : Waiting for Asynchronous Erase Operation...\r\n", data_length );
	nv_return_status = esp_uc_flash->Erase( flash_addr, data_length, Flash_Test_Callback,
											( NV_Ctrl::cback_param_t ) &async_operation_completion_flag );
	if ( nv_return_status == NV_Ctrl::PENDING_CALL_BACK )
	{
		/* Wait for the Erase Flash Operation in Asynchronous mode */
		if ( Wait_Flash_Operation() )
		{
			printf( "Asynchronous Erase Operation completed (parallel_task_count=%d)\r\n", parallel_task_count );
			return_flag = true;
		}
	}
	else
	{
		printf( "Asynchronous Erase failed with return status %d\r\n", nv_return_status );
	}

	return ( return_flag );
}

/**
 * @brief						Write Data function for Asynchronous uC Flash Operation.
 * @param[in] Esp_uC_Flash:		NV_Ctrls Class Pointer.
 * @param[in] flash_addr:		Flash memory address.
 * @param[in] data:				Data pointer.
 * @param[in] data_length:		Data size.
 * @return bool					return true for success and false for fail.
 */
static bool Async_Write_Flash( NV_Ctrls::uC_Flash* esp_uc_flash, uint32_t flash_addr, uint8_t* data,
							   uint32_t data_length )
{
	NV_Ctrl::nv_status_t nv_return_status = NV_Ctrl::SUCCESS;

	parallel_task_count = DISABLE_VAL;
	bool return_flag = false;

	/* Write Flash Operation in Asynchronous mode */
	async_operation_completion_flag = DISABLE_VAL;

	printf( "Write Flash (data size = %d) : Waiting for Asynchronous Write Operation...\r\n", data_length );
	nv_return_status = esp_uc_flash->Write( reinterpret_cast<uint8_t*>( data ), flash_addr, data_length,
											Flash_Test_Callback,
											( NV_Ctrl::cback_param_t ) &async_operation_completion_flag );
	if ( nv_return_status == NV_Ctrl::PENDING_CALL_BACK )
	{
		/* Wait for the Erase Flash Operation in Asynchronous mode */
		if ( Wait_Flash_Operation() )
		{
			printf( "Asynchronous Write Operation completed (parallel_task_count=%d)\r\n", parallel_task_count );
			return_flag = true;
		}
	}
	else
	{
		printf( "Asynchronous Write operation failed with return status %d\r\n", nv_return_status );
	}

	return ( return_flag );
}

/**
 * @brief						Read Data function for Asynchronous uC Flash Operation.
 * @param[in] Esp_uC_Flash:		NV_Ctrls Class Pointer.
 * @param[in] flash_addr:		Flash memory address.
 * @param[in] data:				Data pointer.
 * @param[in] data_length:		Data size.
 * @return bool					return true for success and false for fail.
 */
static bool Async_Read_Flash( NV_Ctrls::uC_Flash* Esp_uC_Flash, uint32_t flash_addr, uint8_t* data,
							  uint32_t data_length )
{
	NV_Ctrl::nv_status_t nv_return_status = NV_Ctrl::SUCCESS;

	parallel_task_count = DISABLE_VAL;
	bool return_flag = false;

	/* Read Flash Operation in Asynchronous mode */
	async_operation_completion_flag = DISABLE_VAL;

	printf( "Read Flash (data size = %d) : Waiting for Asynchronous Read Operation...\r\n", data_length );
	nv_return_status = Esp_uC_Flash->Read( reinterpret_cast<uint8_t*>( data ), flash_addr, data_length,
										   Flash_Test_Callback,
										   ( NV_Ctrl::cback_param_t ) &async_operation_completion_flag );
	if ( nv_return_status == NV_Ctrl::PENDING_CALL_BACK )
	{
		/* Wait for the Erase Flash Operation in Asynchronous mode */
		if ( Wait_Flash_Operation() )
		{
			printf( "Asynchronous Read Operation completed (parallel_task_count=%d)\r\n", parallel_task_count );
			return_flag = true;
		}
	}
	else
	{
		printf( "Asynchronous read operation failed with return status %d\r\n", nv_return_status );
	}

	return ( return_flag );
}

/**
 * @brief						Asynchronous uC Flash Test.
 * @param[in] data_length:		Test data size.
 * @return void					None
 */
static void Async_Read_Write_Test( uint32_t test_data_length )
{
	NV_Ctrls::uC_Flash* esp_uc_flash = nullptr;
	uint8_t* write_data_buffer = nullptr;
	uint8_t* read_data_buffer = nullptr;
	uint32_t erase_data_size;
	/* Flash address for uC Flash operation read/write/erase. */
	uint32_t flash_addr = ESP_SCRATCH_IMAGE_CONFIG.start_address;

	printf( "Async_Read_Write_Test ( Test data length size = %d )\r\n", test_data_length );

	/* Allocate the memory for write data buffer */
	write_data_buffer = new uint8_t[test_data_length];
	/* Allocate the memory for read data buffer */
	read_data_buffer = new uint8_t[test_data_length];
	/*Fill a write data buffer with random bytes from hardware RNG*/
	esp_fill_random( write_data_buffer, test_data_length );

	/* Clear a read data buffer */
	memset( read_data_buffer, 0U, test_data_length );

	/* Create the uC_flash object using NV_Ctrl class */
	esp_uc_flash = new NV_Ctrls::uC_Flash(
		reinterpret_cast<NV_Ctrls::uC_FLASH_CHIP_CONFIG_TD const*>( &ESP_SCRATCH_IMAGE_CONFIG ), nullptr );
	if ( esp_uc_flash == nullptr )
	{
		printf( "Error: Fail to create the uC_flash object\r\n" );
	}

	if ( ( esp_uc_flash != nullptr ) && ( write_data_buffer != nullptr ) && ( read_data_buffer != nullptr ) )
	{
		printf( "\r\nASYNC UC FLASH TEST 1: Erase->Write->Read->Compare.\r\n" );

		/* Round up test data length size with multiply of sectors size (4096).
		 * if (0x4096)  < test_data_length <= (1x4096) then erase_data_size = (1x4096).
		 * if (1x4096)  < test_data_length <= (2x4096) then erase_data_size = (2x4096).
		 * if (2x4096)  < test_data_length <= (3x4096) then erase_data_size = (3x4096).
		 * if (3x4096)  < test_data_length <= (4x4096) then erase_data_size = (4x4096).
		 *                                  .
		 *                                  .
		 *                                  .
		 * if ((n)x4096)  < test_data_length <= ((n+1)x4096) then erase_data_size = ((n+1)x4096).
		 *
		 * e.g.
		 * if test_data_length = 1024 bytes then erase_data_size = 4096 x 1 = 4096 bytes.
		 * if test_data_length = 4097 bytes then erase_data_size = 4096 x 2 = 8192 bytes.
		 * if test_data_length = 8191 bytes then erase_data_size = 4096 x 2 = 8192 bytes.
		 * if test_data_length = 8193 bytes then erase_data_size = 4096 x 3 = 12288 bytes.
		 * if test_data_length = 20520 bytes then erase_data_size = 4096 x 6 = 24576 bytes.
		 */
		erase_data_size = ( ( test_data_length - 1 ) / SECTOR_SIZE + 1 ) * SECTOR_SIZE;
		printf( "erase_data_size=%d for test_data_length=%d\r\n", erase_data_size, test_data_length );

		/* Erase Data on flash */
		if ( Async_Erase_Flash( esp_uc_flash, flash_addr, erase_data_size ) )
		{
			/* Write Data on flash */
			if ( Async_Write_Flash( esp_uc_flash, flash_addr, write_data_buffer, test_data_length ) )
			{
				/* Read Data on flash */
				if ( Async_Read_Flash( esp_uc_flash, flash_addr, read_data_buffer, test_data_length ) )
				{
					/* Read Data with Write Data. */
					if ( memcmp( write_data_buffer, read_data_buffer, test_data_length ) )
					{
						printf( "Data from Read( ) and Write( ) is not equal \r\n" );
					}
					else
					{
						printf( "Data from Read( ) and Write( ) is equal \r\n" );
					}
				}
				else/* Else part of Read Data on flash */
				{
					printf( "Error: Async_Read_Flash failed\r\n" );
				}
			}
			else/* Else part of Write Data on flash */
			{
				printf( "Error: Async_Write_Flash failed\r\n" );
			}
		}
		else/* Else part of Erase Data on flash */
		{
			printf( "Error: Async_Erase_Flash failed\r\n" );
		}

		printf( "\r\nASYNC UC FLASH TEST 2: Erase_All->Read->Print.\r\n" );

		/* Erase All on flash */
		if ( Async_Erase_All_Flash( esp_uc_flash ) )
		{
			printf( "Erase All Flash successfully \r\n" );
			memset( read_data_buffer, 0U, test_data_length );
			/* Read Data on flash after flash erase*/
			if ( Async_Read_Flash( esp_uc_flash, flash_addr, read_data_buffer, test_data_length ) )
			{
				bool erase_flash_status = false;
				uint8_t data_to_compare = 0xFF;
				for ( uint32_t index = 0; index < test_data_length; index++ )
				{
					if ( read_data_buffer[index] != data_to_compare )
					{
						printf( "Async Erase All Flash verification failed\r\n" );
						erase_flash_status = false;
						break;
					}
					else
					{
						erase_flash_status = true;
					}
				}
				if ( erase_flash_status )
				{
					printf( "Async Erase All Flash verified successfully\r\n" );
				}
			}
			else/* Else part of Read Data on flash */
			{
				printf( "Failed to read data from flash memory while verification\r\n" );
			}
		}
		else
		{
			printf( "Error: Async_Erase_All_Flash failed\r\n" );
		}

		printf( "Async_Read_Write_Test over\r\n" );

		/* Delete the uC_flash object of NV_Ctrl class */
		delete esp_uc_flash;
		/* Deallocate the memory for write data buffer */
		delete[] write_data_buffer;
		/* Deallocate the memory for read data buffer */
		delete[] read_data_buffer;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Spi_Flash_Test_App_Main()
{
	/* Synchronous Flash Test */
	Sync_Read_Write_Test();

	/* Asynchronous Flash Test */
	Async_Read_Write_Test( DATA_LEN );
}
