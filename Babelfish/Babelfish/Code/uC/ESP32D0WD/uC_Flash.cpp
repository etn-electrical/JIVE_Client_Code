/**
 *****************************************************************************************
 * @file		uC_Flash.cpp
 * @details		See header file for module overview.
 * @copyright	2019 Eaton Corporation All Rights Reserved.
 *****************************************************************************************
 */
#include "uC_Flash.h"
//Ahmed
#include "Breaker.h"


extern DRAM_ATTR Device_Info_str DeviceInfo;


static const uint32_t DISABLE_VALUE = 0U;			///< Disable value.

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
namespace NV_Ctrls
{

NV_Ctrl::semaphore_status_t uC_Flash::m_semaphore = NV_Ctrl::RELEASE;

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const spi_flash_guard_funcs_t* s_flash_guard_ops;				///< structure constant flash guard ops
static const uint32_t SECTOR_SIZE = 4096U;								///< FLash Sector Size.
static const uint32_t PAGE_SIZE = 256U;									///< Flash Page Size.
static const uint32_t SECTOR_ERASE_TIME_BASELINE_MS = 40U;				///< Sector Erase Time in ms.
																		///< (Observation in range 28ms to 35ms)
static const uint32_t SECTOR_WRITE_TIME_BASELINE_MS = 20U;				///< Sector Write Time in ms.
																		///< (Observation in range 10ms to 15ms)
static const uint32_t SECTOR_READ_TIME_BASELINE_MS = 5U;				///< Sector Read Time in ms.
																		///< (Observation in range 1ms to 3ms)
static const uint8_t CHECKSUM_LENGTH = 2U;								///< Checksum data length.

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Flash::uC_Flash( uC_FLASH_CHIP_CONFIG_TD const* chip_config, FLASH_TypeDef* flash_ctrl, bool synchronus_erase,
					cback_func_t cback_func, cback_param_t param, bool erase_on_boundary ) :
	NV_Ctrl(),
	m_chip_cfg( chip_config ),
	m_flash_ctrl( flash_ctrl ),
	m_wait_time( 0U ),
	m_erase_val( DISABLE_VALUE ),
	m_synchronus( synchronus_erase ),
	m_erase_on_boundary( erase_on_boundary ),
	m_state( NV_Ctrl::IDLE ),
	m_status( NV_Ctrl::SUCCESS ),
	m_cr_task( reinterpret_cast<CR_Tasker*>( nullptr ) )
{
	m_flash_busy = false;
	spi_flash_init();

	/* Reset the asynchronous write/read/erase service structure */
	m_asynch_data = { 0U };

	/* Attach call-back function and parameter for asynchronous service completion */
	m_asynch_data.param = param;
	m_asynch_data.cback = cback_func;

	/* Create the CR Task for asynchronous write/read/erase service handler */
	m_cr_task = new CR_Tasker( &Flash_Async_Task_Static,
							   reinterpret_cast<CR_TASKER_PARAM>( this ), CR_TASKER_IDLE_PRIORITY,
							   "uC_Flash_Routine" );

	m_cr_task->Suspend();
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Flash::~uC_Flash( void )
{
	if ( m_cr_task != nullptr )
	{
		delete m_cr_task;
		m_cr_task = nullptr;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Read( uint8_t* data, uint32_t address, uint32_t length, cback_func_t cback,
									 cback_param_t param )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;

	/* Check asynchronous write/read/erase service handler is free or busy */
	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		/* Check address range */
		if ( Check_Range( address, length ) && ( length != 0U ) )
		{
			m_semaphore = NV_Ctrl::ACQUIRE;
			m_asynch_data.data = const_cast<uint8_t*>( data );
			m_asynch_data.address = address;
			m_asynch_data.length = length;
			m_asynch_data.cback = cback;
			m_asynch_data.param = param;
			m_wait_time = Read_Time( const_cast<uint8_t*>( data ), address, length );
			m_state = NV_Ctrl::READ;
			return_status = NV_Ctrl::PENDING_CALL_BACK;
			m_cr_task->Resume();
		}
		else
		{
			return_status = NV_Ctrl::INVALID_ADDRESS;
		}
	}
	else
	{
		return_status = NV_Ctrl::BUSY_ERROR;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Write( const uint8_t* data, uint32_t address, uint32_t length, cback_func_t cback,
									  cback_param_t param )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;

	/* Check asynchronous write/read/erase service handler is free or busy */
	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		/* Check address range */
		if ( Check_Range( address, length ) && ( length != 0U ) )
		{
			m_semaphore = NV_Ctrl::ACQUIRE;
			m_asynch_data.data = const_cast<uint8_t*>( data );
			m_asynch_data.address = address;
			m_asynch_data.length = length;
			m_asynch_data.cback = cback;
			m_asynch_data.param = param;
			m_wait_time = Write_Time( const_cast<uint8_t*>( data ), address, length );
			m_state = NV_Ctrl::WRITE;
			return_status = NV_Ctrl::PENDING_CALL_BACK;
			m_cr_task->Resume();
		}
		else
		{
			return_status = NV_Ctrl::INVALID_ADDRESS;
		}
	}
	else
	{
		return_status = NV_Ctrl::BUSY_ERROR;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Erase( uint32_t address, uint32_t length, cback_func_t cback, cback_param_t param )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;

	/* Check asynchronous write/read/erase service handler is free or busy */
	if ( m_semaphore == NV_Ctrl::RELEASE )
	{
		/* Check address range */
		if ( Check_Range( address, length ) && ( length != 0U ) )
		{
			m_semaphore = NV_Ctrl::ACQUIRE;
			m_asynch_data.data = nullptr;
			m_asynch_data.address = address;
			m_asynch_data.length = length;
			m_asynch_data.cback = cback;
			m_asynch_data.param = param;
			m_state = NV_Ctrl::ERASE;
			m_wait_time = Erase_Time( address, length );
			return_status = NV_Ctrl::PENDING_CALL_BACK;
			m_cr_task->Resume();
		}
		else
		{
			return_status = NV_Ctrl::INVALID_ADDRESS;
		}
	}
	else
	{
		return_status = NV_Ctrl::BUSY_ERROR;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Erase_All( cback_func_t cback, cback_param_t param )
{
	uint32_t size = m_chip_cfg->end_address - m_chip_cfg->start_address + 1U;

    //Ahmed
    ets_printf("Erease All Flash \n");
	return ( Erase( m_chip_cfg->start_address, size, cback, param ) );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
									 bool use_protection )
{
	nv_status_t status = INVALID_ADDRESS;

	if ( Check_Range( address, length ) && ( length != 0U ) )
	{
		status = Read_Now( data, address, length );
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Write( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,
									  bool use_protection )
{
	nv_status_t status = INVALID_ADDRESS;
	uint32_t page_num = DISABLE_VALUE;

	if ( Check_Range( address, length ) && ( length != 0U ) )
	{
		if ( m_erase_on_boundary == true )
		{
			page_num = ( address % SECTOR_SIZE ) / PAGE_SIZE;
			Erase_Page( page_num + ENABLE_VAL );
		}

		status = Write_Now( data, address, length );
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
											  NV_CTRL_LENGTH_TD length )
{
	nv_status_t status = SUCCESS;

	status = Read( ( uint8_t* )checksum_dest, address + length, CHECKSUM_LENGTH );

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Erase( uint32_t address, NV_CTRL_LENGTH_TD length, uint8_t erase_data,
									  bool protected_data )
{
	nv_status_t status = INVALID_ADDRESS;

	if ( Check_Range( address, length ) && ( length != 0U ) )
	{
		status = Erase_Now( address, length );
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Flash::Erase_Page( uint32_t address )
{
	bool ret = true;
	esp_err_t status = ESP_OK;

	status = spi_flash_erase_range( address * PAGE_SIZE, PAGE_SIZE );

	if ( status != ESP_OK )
	{
		ret = false;
	}
	else
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
uC_FLASH_STATUS_EN uC_Flash::Get_Status( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const
{
	bool ret = true;
	uC_FLASH_STATUS_EN status = uC_FLASH_COMPLETE;

	ret = Check_Range( address, length );

	if ( ret == true )
	{
		status = uC_FLASH_COMPLETE;
	}
	else
	{
		status = uC_FLASH_BUSY;
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Erase_All( void )
{
	uint32_t size = m_chip_cfg->end_address - m_chip_cfg->start_address + 1U;
	nv_status_t status;

	status = Erase( m_chip_cfg->start_address, size, uC_FLASH_DEFAULT_ERASE_VAL, false );

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Unlock_Flash( void )
{
	if ( s_flash_guard_ops && s_flash_guard_ops->op_unlock )
	{
		s_flash_guard_ops->op_unlock();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Lock_Flash( void )
{
	if ( s_flash_guard_ops && s_flash_guard_ops->op_lock )
	{
		s_flash_guard_ops->op_lock();
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Flash::Read_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const
{
	uint32_t total_sector_num = ( length / SECTOR_SIZE ) + ( ( ( length % SECTOR_SIZE ) == 0 ) ? 0 : 1 );
	uint32_t read_time = total_sector_num * SECTOR_READ_TIME_BASELINE_MS;

	return ( read_time );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Flash::Write_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
							   bool include_erase_time ) const
{
	uint32_t total_sector_num = ( length / SECTOR_SIZE ) + ( ( ( length % SECTOR_SIZE ) == 0 ) ? 0 : 1 );
	uint32_t write_time = total_sector_num * SECTOR_WRITE_TIME_BASELINE_MS;

	return ( write_time );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Flash::Erase_Time( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length )
{
	uint32_t total_sector_num = ( length / SECTOR_SIZE ) + ( ( ( length % SECTOR_SIZE ) == 0 ) ? 0 : 1 );
	uint32_t erase_time = total_sector_num * SECTOR_ERASE_TIME_BASELINE_MS;

	return ( erase_time );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Flash_Async_Task_Static( CR_Tasker* cr_task, CR_TASKER_PARAM param )
{
	( ( uC_Flash* )param )->Flash_Async_Task( cr_task );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Flash_Async_Task( CR_Tasker* cr_task )
{
	switch ( m_state )
	{
		case IDLE:
			m_cr_task->Suspend();
			break;

		case ERASE:
			asynch_erase_handler();
			break;

		case READ:
			asynch_read_handler();
			break;

		case WRITE:
			asynch_write_handler();
			break;

		case COMPLETE:
			m_asynch_data.cback( m_asynch_data.param, m_status );
			m_state = IDLE;
			break;

		case ERROR:
			m_asynch_data.cback( m_asynch_data.param, m_status );
			m_state = IDLE;
			break;

		default:
			break;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Attach_Callback( cback_func_t func_callback, cback_param_t func_param )
{
	m_asynch_data.param = func_param;
	m_asynch_data.cback = func_callback;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_FLASH_STATUS_EN uC_Flash::Erase_Sector( uint32_t sec_no )
{
	esp_err_t ret = ESP_OK;
	uC_FLASH_STATUS_EN status = uC_FLASH_COMPLETE;

	ret = spi_flash_erase_sector( sec_no );

	if ( ret == ESP_OK )
	{
		status = uC_FLASH_COMPLETE;
	}
	else
	{
		status = uC_FLASH_TIMEOUT;
	}

	return ( status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool uC_Flash::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length ) const
{
	bool result = false;
	NV_CTRL_ADDRESS_TD end_address = m_chip_cfg->end_address;

	if ( ( address >= m_chip_cfg->start_address ) && ( ( address + length ) <= ( end_address + 1U ) ) )
	{
		result = true;
	}
	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::Get_Memory_Info( mem_range_info_t* mem_range_info )
{
	mem_range_info->start_address = m_chip_cfg->start_address;
	mem_range_info->mirror_start_address = m_chip_cfg->mirror_start_address;
	mem_range_info->end_address = m_chip_cfg->end_address;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Erase_Now( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length )
{
	NV_Ctrl::nv_status_t result = NV_Ctrl::SUCCESS;
	esp_err_t ret;
	uint32_t erase_data_size = 0U;
    //Ahmed
    if (DeviceInfo.OTA_State >= OTA_START)
    {
        DeviceInfo.OTA_State = OTA_MEM_EREASE;
    }
	/* Round up test data length size with multiply of sectors size (4096).
	 * if ((n)x4096)  < test_data_length <= ((n+1)x4096) then erase_data_size = ((n+1)x4096).
	 * if test_data_length = 1024 bytes then erase_data_size = 4096 x 1 = 4096 bytes.
	 * if test_data_length = 8191 bytes then erase_data_size = 4096 x 2 = 8192 bytes.
	 * if test_data_length = 8193 bytes then erase_data_size = 4096 x 3 = 12288 bytes.
	 */
	erase_data_size = ( ( length - 1 ) / SECTOR_SIZE + 1 ) * SECTOR_SIZE;
    //Ahmed
    if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
    {
        ets_printf("Erease Address 0x%x  Size %d\n", address, erase_data_size);    
    }
	ret = spi_flash_erase_range( address, erase_data_size );
    //ets_printf("Erease Done\n");
	if ( ret != ESP_OK )
	{
		result = Interpret_SPI_Flash_Error( ret );
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Write_Now( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length )
{
	NV_Ctrl::nv_status_t result = NV_Ctrl::SUCCESS;
	esp_err_t ret;
    //Ahmed
    if (DeviceInfo.OTA_State >= OTA_START)
    {
        DeviceInfo.OTA_State = OTA_MEM_WRITE;
    }
    if (DeviceInfo.DeviceInFactoryMode != DEVICE_IS_IN_FACTORY_MODE)
    {
        ets_printf("Write Address 0x%x  Size %d\n", address, length);    
    }
	ret = spi_flash_write( address, data, length );

	if ( ret != ESP_OK )
	{
		result = Interpret_SPI_Flash_Error( ret );
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Read_Now( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length )
{
	NV_Ctrl::nv_status_t result = NV_Ctrl::SUCCESS;
	esp_err_t ret;

	ret = spi_flash_read( address, data, length );

	if ( ret != ESP_OK )
	{
		result = Interpret_SPI_Flash_Error( ret );
	}

	return ( result );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::asynch_erase_handler( void )
{
	NV_Ctrl::nv_status_t erase_status = NV_Ctrl::SUCCESS;
	NV_CTRL_LENGTH_TD length = 0U;

	if ( m_semaphore == NV_Ctrl::ACQUIRE )
	{
		length = ( m_asynch_data.length > SECTOR_SIZE ) ? SECTOR_SIZE : m_asynch_data.length;

		erase_status = Erase_Now( m_asynch_data.address, length );
		if ( erase_status != NV_Ctrl::SUCCESS )
		{
			m_state = NV_Ctrl::ERROR;
			m_status = erase_status;
			uC_Flash::m_semaphore = NV_Ctrl::RELEASE;
		}
		else
		{
			m_asynch_data.address += length;
			m_asynch_data.length -= length;
			if ( m_asynch_data.length == 0 )
			{
				m_state = NV_Ctrl::COMPLETE;
				m_status = NV_Ctrl::SUCCESS;
				m_semaphore = NV_Ctrl::RELEASE;
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::asynch_write_handler( void )
{
	NV_Ctrl::nv_status_t write_status = NV_Ctrl::SUCCESS;
	NV_CTRL_LENGTH_TD length = 0U;

	if ( m_semaphore == NV_Ctrl::ACQUIRE )
	{
		length = ( m_asynch_data.length > PAGE_SIZE ) ? PAGE_SIZE : m_asynch_data.length;

		write_status = Write_Now( m_asynch_data.data, m_asynch_data.address, length );
		if ( write_status != NV_Ctrl::SUCCESS )
		{
			m_state = NV_Ctrl::ERROR;
			m_status = write_status;
			uC_Flash::m_semaphore = NV_Ctrl::RELEASE;
		}
		else
		{
			m_asynch_data.data += length;
			m_asynch_data.address += length;
			m_asynch_data.length -= length;
			if ( m_asynch_data.length == 0 )
			{
				m_state = NV_Ctrl::COMPLETE;
				m_status = NV_Ctrl::SUCCESS;
				m_semaphore = NV_Ctrl::RELEASE;
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_Flash::asynch_read_handler( void )
{
	NV_Ctrl::nv_status_t read_status = NV_Ctrl::SUCCESS;
	NV_CTRL_LENGTH_TD length = 0U;

	if ( m_semaphore == NV_Ctrl::ACQUIRE )
	{
		length = ( m_asynch_data.length > PAGE_SIZE ) ? PAGE_SIZE : m_asynch_data.length;

		read_status = Read_Now( m_asynch_data.data, m_asynch_data.address, length );
		if ( read_status != NV_Ctrl::SUCCESS )
		{
			m_state = NV_Ctrl::ERROR;
			m_status = read_status;
			uC_Flash::m_semaphore = NV_Ctrl::RELEASE;
		}
		else
		{
			m_asynch_data.data += length;
			m_asynch_data.address += length;
			m_asynch_data.length -= length;
			if ( m_asynch_data.length == 0 )
			{
				m_state = NV_Ctrl::COMPLETE;
				m_status = NV_Ctrl::SUCCESS;
				m_semaphore = NV_Ctrl::RELEASE;
			}
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t uC_Flash::Interpret_SPI_Flash_Error( esp_err_t spi_flash_error )
{
	NV_Ctrl::nv_status_t return_nv_ctrl_status;

	switch ( spi_flash_error )
	{
		case ESP_OK:
			return_nv_ctrl_status = NV_Ctrl::SUCCESS;
			break;

		case ESP_ERR_TIMEOUT:
			return_nv_ctrl_status = NV_Ctrl::BUSY_ERROR;
			break;

		case ESP_ERR_NO_MEM:
		case ESP_ERR_INVALID_ARG:
		case ESP_ERR_INVALID_SIZE:
		case ESP_ERR_NOT_FOUND:
			return_nv_ctrl_status = NV_Ctrl::INVALID_ADDRESS;
			break;

		case ESP_ERR_NOT_SUPPORTED:
			return_nv_ctrl_status = NV_Ctrl::OPERATION_NOT_SUPPORTED;
			break;

		case ESP_FAIL:
		default:
			return_nv_ctrl_status = DATA_ERROR;
			break;
	}

	return ( return_nv_ctrl_status );
}

}	// Namespace End
