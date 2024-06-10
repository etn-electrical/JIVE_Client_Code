/*
 *****************************************************************************************
 *		$Workfile: Fram.cpp$
 *
 *		$Author: Mark A Verheyen$
 *		$Date: 4/17/2008 5:10:20 PM$
 *		Copyright锟�2001-2004, Eaton Corporation. All Rights Reserved.
 *
 *		$Header: Fram.cpp, 10, 4/17/2008 5:10:20 PM, Mark A Verheyen$
 *****************************************************************************************
 */
#include "IS25.h"
#include "StdLib_MV.h"
#include "Ram.h"
#include "uC_Watchdog.h"
#include "NV_Ctrl_Debug.h"
#include "Babelfish_Assert.h"

namespace NV_Ctrls
{

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define IS25_STATUS_BUSY_BIT                    0U


// The number of bits that the upper bits need to be shifted to get it into the
// right space in the write opcode.  This is only for the single byte address type.
#define UPPER_ADDRESS_BIT_SHIFT                 3U

#define MAX_NUMBER_OF_WRITES                    2U
#define MAX_NUMBER_OF_READS                     2U

#define Delay_Chip_Select_Change()              uC_Delay( 1 )

#define IS25_NV_CHECKSUM_LEN                    2U


#define IS25_EPAGE_ALIGNMENT_MASK               ~( m_chip_cfg->erase_page_size - 1 )
#define IS25_WPAGE_ALIGNMENT_MASK               ~( m_chip_cfg->write_page_size - 1 )
#define IS25_SCRATCH_EPAGE_ADDRESS              ( m_chip_cfg->end_address - m_chip_cfg->erase_page_size )

#define IS25_MAX_ERASE_TIME                     300U
#define IS25_MAX_WRITE_PAGE_TIME                1U

#define SPI_TIMEOUT_VAL                         200U
#define CONVERT_MS_TO_COUNT( millisecond )          ( millisecond * ( ( MASTER_CLOCK_FREQUENCY / 1000 ) / 15 ) )
#define ERASE_BLOCK_4K     0xD7					// erase 4K
#define ERASE_BLOCK_32K    0x52
#define ERASE_BLOCK_64K    0xD8

#define BLOCK_4K    0x1000
#define BLOCK_32K   0x8000
#define BLOCK_64K   0x10000

#define ERASE_SECTOR_MAX_TIME       300U	// ms
#define ERASE_BLOCK_32K_MAX_TIME    500U
#define ERASE_BLOCK_64K_MAX_TIME    1000U

#define IS25_EBLOCK_4K_ALIGNMENT_MASK           ~( BLOCK_4K - 1 )
#define IS25_EBLOCK_32K_ALIGNMENT_MASK          ~( BLOCK_32K - 1 )
#define IS25_EBLOCK_64K_ALIGNMENT_MASK          ~( BLOCK_64K - 1 )

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
bool_t IS25::isBusy = false;
bool_t IS25::m_use_os = false;

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
IS25::IS25( uC_QSPI* spi_ctrl, uint8_t chip_select, IS25_CHIP_CONFIG_TD const* chip_config, uint16_t buffer_size ) :
	m_recovery_address( 0U ),
	m_use_protection( false ),
	m_use_checksum( false ),
	m_check_sum( 0U ),
	m_total_length( 0U ),
	m_done_length( 0U ),
	m_data_ptr( nullptr ),
	m_dest_address( 0U ),
	m_data_length( 0U ),
	m_retry_times( 0U ),
	m_deal_length( 0U ),
	m_deal_data( nullptr ),
	m_deal_address( nullptr ),
	m_start_time( 0U )
{
	m_spi_ctrl = spi_ctrl;
	m_chip_cfg = chip_config;
	m_chip_select = chip_select;

	// m_spi_ctrl->Configure_Chip_Select(  );//this needs to update
	m_wpage_buff = ( uint8_t* )Ram::Allocate( chip_config->write_page_size );
	m_shadow_wpage_buff = ( uint8_t* )Ram::Allocate( chip_config->write_page_size );

	m_buffer_size = buffer_size;
	m_data_buffer = ( uint8_t* )Ram::Allocate( m_buffer_size );

	m_write_status = IS25_PREP_TO_WRITE;
	m_done_length = 0U;

	m_sub_status = IS25_ERASING_PAGE;
	m_sub_done_length = 0U;

	m_long_operate_status = IS25_OPERATE_CMD;

	m_erase_val = IS25_DEFAULT_ERASE_VAL;

	m_cr_queue = new CR_Queue();
	new CR_Tasker( IS25_CR_Callback_Static, this, CR_TASKER_IDLE_PRIORITY, "IS25" );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t IS25::Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length, bool_t use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::BUSY_ERROR;
	bool_t good_data;
	uint16_t read_count;

	if ( Get_Semaphore() )
	{
		if ( Check_Range( address, length, false, use_protection ) && ( length != 0 ) )
		{
			if ( m_spi_ctrl->Get_Semaphore( SPI_TIMEOUT_VAL ) )
			{
				read_count = 0U;
				do
				{
					read_count++;
					good_data = Reading( data, address, length, false );
				}while ( ( !good_data ) && ( read_count < MAX_NUMBER_OF_READS ) );

				if ( good_data != FALSE )
				{
					return_status = NV_Ctrl::SUCCESS;
				}
				else
				{
					return_status = NV_Ctrl::DATA_ERROR;
					NV_CTRL_PRINT( DBG_MSG_ERROR,
								   "Failed to read from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
								   address, length, use_protection, return_status );
				}

				m_spi_ctrl->Release_Semaphore();
			}
		}
		else
		{
			return_status = INVALID_ADDRESS;
			NV_CTRL_PRINT( DBG_MSG_ERROR,
						   "Failed to read from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
						   address, length, use_protection, return_status );
		}
		Release_Semaphore();
	}
	else
	{
		return_status = NV_Ctrl::BUSY_ERROR;
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to read from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
					   address, length, use_protection, return_status );
	}
	return ( return_status );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t IS25::Write( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length,  bool_t use_protection )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::SUCCESS;
	bool_t inter_write_result;

	if ( Get_Semaphore() )
	{
		if ( m_spi_ctrl->Get_Semaphore( SPI_TIMEOUT_VAL ) )
		{
			if ( ( m_use_os != FALSE ) && ( data != &m_erase_val ) )
			{
				if ( length > m_buffer_size )
				{
					return_status = NV_Ctrl::DATA_ERROR;
					NV_CTRL_PRINT( DBG_MSG_ERROR,
								   "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
								   address, length, use_protection, return_status );
				}
			}

			if ( return_status == NV_Ctrl::SUCCESS )
			{
				if ( Check_Range( address, length, false, use_protection ) )
				{
					inter_write_result = Internal_Write( data, address, length, false, use_protection );

					if ( inter_write_result == false )
					{
						return_status = NV_Ctrl::DATA_ERROR;
						NV_CTRL_PRINT( DBG_MSG_ERROR,
									   "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
									   address, length, use_protection, return_status );
					}
					else
					{
						return_status = NV_Ctrl::SUCCESS;
					}
				}
				else
				{
					return_status = INVALID_ADDRESS;
					NV_CTRL_PRINT( DBG_MSG_ERROR,
								   "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
								   address, length, use_protection, return_status );
				}
			}

			if ( return_status != NV_Ctrl::SUCCESS )
			{
				m_spi_ctrl->Release_Semaphore();
			}
		}
		else
		{
			return_status = NV_Ctrl::BUSY_ERROR;
			NV_CTRL_PRINT( DBG_MSG_ERROR,
						   "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
						   address, length, use_protection, return_status );
		}

		if ( ( return_status != NV_Ctrl::SUCCESS ) ||
			 ( ( return_status == NV_Ctrl::SUCCESS ) && ( m_use_os == false ) ) )
		{
			Release_Semaphore();
		}
	}
	else
	{
		return_status = NV_Ctrl::BUSY_ERROR;
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to write from Address: %X, Length: %u, Protection: %d, ErrorStatus: %d",
					   address, length, use_protection, return_status );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t IS25::Erase( uint32_t address, NV_CTRL_LENGTH_TD length, uint8_t erase_data,
								  bool_t use_protection )
{
	m_erase_val = erase_data;

	return ( Write( &m_erase_val, address, length, use_protection ) );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t IS25::Erase_All( void )
{
	return ( Erase( m_chip_cfg->start_address,
					( m_chip_cfg->end_address - m_chip_cfg->start_address ),
					IS25_DEFAULT_ERASE_VAL, false ) );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t IS25::Erase_Operation( uint8_t erase_cmd, uint32_t address )
{
	NV_Ctrl::nv_status_t return_status = NV_Ctrl::SUCCESS;
	bool_t erase_result;

	if ( Get_Semaphore() )
	{
		if ( m_spi_ctrl->Get_Semaphore( SPI_TIMEOUT_VAL ) )
		{
			if ( ( erase_cmd != ERASE_BLOCK_4K ) && ( erase_cmd != ERASE_BLOCK_32K ) &&
				 ( erase_cmd != ERASE_BLOCK_64K ) )
			{
				return_status = NV_Ctrl::DATA_ERROR;
				NV_CTRL_PRINT( DBG_MSG_ERROR,
							   "Failed to erase from Address: %X, EraseCommand: %X, ErrorStatus: %d",
							   address, erase_cmd, return_status );
			}

			if ( return_status == NV_Ctrl::SUCCESS )
			{
				uint8_t status = 0U;
				uC_Watchdog::Force_Feed_Dog();

				m_spi_ctrl->Select_Chip( m_chip_select );

				m_spi_ctrl->QSPI_WriteEnable();

				m_spi_ctrl->De_Select_Chip( m_chip_select );

				// An edge has to be sent to the Chip select pin on a write instruction.
				// Nops are there to give at least a 5Mhz pulse width.  Get it?  If not think about it.
				Delay_Chip_Select_Change();

				m_spi_ctrl->Select_Chip( m_chip_select );

				uint16_t max_time;
				if ( erase_cmd == ERASE_BLOCK_4K )
				{
					max_time = ERASE_SECTOR_MAX_TIME;
					address = address & IS25_EPAGE_ALIGNMENT_MASK;
				}
				else if ( erase_cmd == ERASE_BLOCK_32K )
				{
					max_time = ERASE_BLOCK_32K_MAX_TIME;
					address = address & IS25_EBLOCK_32K_ALIGNMENT_MASK;
				}
				else
				{
					max_time = ERASE_BLOCK_64K_MAX_TIME;
					address = address & IS25_EBLOCK_64K_ALIGNMENT_MASK;
				}
				m_spi_ctrl->Erase_Block( erase_cmd, address );
				m_spi_ctrl->De_Select_Chip( m_chip_select );

				TIMERS_TIME_TD begin_time = BF_Lib::Timers::Get_Time();

				while ( true )
				{
					uC_Delay( 50 );

					if ( BF_Lib::Timers::Expired( begin_time, max_time ) )
					{
						erase_result = false;
						break;
					}

					Read_Status( &status );
					if ( Test_Bit( status, IS25_STATUS_BUSY_BIT ) == false )
					{
						erase_result = true;
						break;
					}
					uC_Watchdog::Force_Feed_Dog();
				}
				if ( erase_result == false )
				{
					return_status = NV_Ctrl::DATA_ERROR;
					NV_CTRL_PRINT( DBG_MSG_ERROR,
								   "Failed to erase from Address: %X, EraseCommand: %X, ErrorStatus: %d",
								   address, erase_cmd, return_status );
				}
				else
				{
					return_status = NV_Ctrl::SUCCESS;
				}
			}
			m_spi_ctrl->Release_Semaphore();
		}
		else
		{
			return_status = NV_Ctrl::BUSY_ERROR;
			NV_CTRL_PRINT( DBG_MSG_ERROR,
						   "Failed to erase from Address: %X, EraseCommand: %X, ErrorStatus: %d",
						   address, erase_cmd, return_status );
		}

		Release_Semaphore();
	}
	else
	{
		return_status = NV_Ctrl::BUSY_ERROR;
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to erase from Address: %X, EraseCommand: %X, ErrorStatus: %d",
					   address, erase_cmd, return_status );
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t IS25::Erase_External_Serial_Flash( uint32_t start_address, uint32_t length )
{
	NV_Ctrl::nv_status_t result = NV_Ctrl::SUCCESS;

	// Check the start address is at the beginning of sector or not.
	if ( start_address % BLOCK_4K == 0 )
	{
		while ( ( length >= BLOCK_4K ) && ( result == NV_Ctrl::SUCCESS ) )
		{
			if ( ( length >= BLOCK_64K ) && ( start_address == ( start_address & IS25_EBLOCK_64K_ALIGNMENT_MASK ) ) )
			{
				result = Erase_Operation( ERASE_BLOCK_64K, start_address );
				start_address += BLOCK_64K;
				length -= BLOCK_64K;
			}
			else if ( ( length >= BLOCK_32K ) &&
					  ( start_address == ( start_address & IS25_EBLOCK_32K_ALIGNMENT_MASK ) ) )
			{
				result = Erase_Operation( ERASE_BLOCK_32K, start_address );
				start_address += BLOCK_32K;
				length -= BLOCK_32K;
			}
			else if ( ( length >= BLOCK_4K ) && ( start_address == ( start_address & IS25_EBLOCK_4K_ALIGNMENT_MASK ) ) )
			{
				result = Erase_Operation( ERASE_BLOCK_4K, start_address );
				start_address += BLOCK_4K;
				length -= BLOCK_4K;
			}
		}
	}
	else
	{
		result = NV_Ctrl::OPERATION_NOT_SUPPORTED;
		NV_CTRL_PRINT( DBG_MSG_ERROR,
					   "Failed to Erase from Address: %X, ErrorStatus: %d",
					   start_address, result );
	}

	return ( result );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
NV_Ctrl::nv_status_t IS25::Read_Checksum( uint16_t* checksum_dest, NV_CTRL_ADDRESS_TD address,
										  NV_CTRL_LENGTH_TD length )
{
	return ( Read( ( uint8_t* )checksum_dest, address + length, IS25_NV_CHECKSUM_LEN, false ) );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool_t IS25::Write_Status( uint8_t status )
{
	bool_t success = false;

	m_spi_ctrl->Select_Chip( m_chip_select );

	m_spi_ctrl->QSPI_WriteEnable();

	m_spi_ctrl->De_Select_Chip( m_chip_select );

	// An edge has to be sent to the Chip select pin on a write instruction.
	// Nops are there to give at least a 5Mhz pulse width.  Get it?  If not think about it.
	Delay_Chip_Select_Change();

	m_spi_ctrl->Select_Chip( m_chip_select );

	m_spi_ctrl->QSPI_WriteStatusRegister( status );

	success = true;

	m_spi_ctrl->De_Select_Chip( m_chip_select );

	return ( success );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool_t IS25::Read_Status( uint8_t* status )
{
	bool_t success = false;

	m_spi_ctrl->Select_Chip( m_chip_select );

	*status = m_spi_ctrl->QSPI_ReadStatusRegister();

	success = true;

	m_spi_ctrl->De_Select_Chip( m_chip_select );

	return ( success );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void IS25::Set_Fault( void )
{
	// Fault_Manager::Set(FC_SERIAL_FLASH_FAULT);
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void IS25::Clr_Fault( void )
{
	// Fault_Manager::Clr(FC_SERIAL_FLASH_FAULT);
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool_t IS25::Is_Ready( void )
{
	/*bool_t result;
	   uint8_t status = 0U;
	   m_spi_ctrl->Select_Chip( m_chip_select );
	   Push_TGINT( );
	   Read_Status( &status );
	   if( Test_Bit( status, IS25_STATUS_BUSY_BIT ) == false )
	   {
	    result = true;
	   }
	   else
	   {
	    result = false;
	   }
	   Pop_TGINT( );
	   m_spi_ctrl->De_Select_Chip( m_chip_select );

	   return result;*/
	return ( !isBusy );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void IS25::IS25_CR_Callback( CR_Tasker* cr_tasker )
{
	RESULT_STATE_ST result_status_pair;
	CR_QUEUE_STATUS result;

	CR_Tasker_Begin( cr_tasker );

	while ( TRUE )
	{
		CR_Queue_Receive( cr_tasker, m_cr_queue, NULL, OS_TASKER_INDEFINITE_TIMEOUT, result );
		if ( result == CR_QUEUE_PASS )
		{
			m_retry_times = 0U;

			do
			{
				do
				{
					result_status_pair = Write_Now();
					if ( result_status_pair.result == IS25_PENDING )
					{
						CR_Tasker_Yield( cr_tasker );
					}

				}while( result_status_pair.status != IS25_FINISH );
				m_retry_times++;
			}while( ( result_status_pair.result != IS25_TRUE ) && ( m_retry_times < MAX_NUMBER_OF_WRITES ) );

			m_spi_ctrl->Release_Semaphore();
			Release_Semaphore();
		}
	}

	CR_Tasker_End();
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool_t IS25::Internal_Write( uint8_t* data, uint32_t dest_address, NV_CTRL_LENGTH_TD length, bool_t use_checksum,
							 bool_t use_protection )
{
	bool_t result;
	NV_CTRL_LENGTH_TD length_index;
	RESULT_STATE_ST result_status_pair;

	if ( ( m_use_os == false ) || ( data == &m_erase_val ) )
	{
		m_data_ptr = data;
	}
	else
	{
		BF_Lib::Copy_String( m_data_buffer, data, length );
		m_data_ptr = m_data_buffer;
	}

	m_dest_address = dest_address;
	m_data_length = length;

	if ( use_checksum != FALSE )
	{
		m_use_checksum = TRUE;

		m_check_sum = 0U;

		if ( m_data_ptr != &m_erase_val )
		{
			for ( length_index = 0U; length_index < m_data_length; length_index++ )
			{
				m_check_sum += m_data_ptr[length_index];
			}
		}
		else
		{
			m_check_sum = m_erase_val * m_data_length;
		}

		m_total_length = m_data_length + IS25_NV_CHECKSUM_LEN;
	}
	else
	{
		m_use_checksum = false;
		m_total_length = m_data_length;
	}

	m_use_protection = use_protection;

	if ( m_use_os != FALSE )
	{
		result = true;
		m_cr_queue->Post_From_ISR( false );
	}
	else
	{
		m_retry_times = 0U;
		do
		{
			do
			{
				result_status_pair = Write_Now();
			}while( result_status_pair.status != IS25_FINISH );
			m_retry_times++;
		}while( ( result_status_pair.result != IS25_TRUE ) && ( m_retry_times < MAX_NUMBER_OF_WRITES ) );

		result = result_status_pair.result;
	}

	return ( result );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
RESULT_STATE_ST IS25::Write_Now( void )
{
	RESULT_STATE_ST main_result_pair = { IS25_FALSE, IS25_PREP_TO_WRITE };
	RESULT_STATE_ST sub_result_pair;
	DATA_ANALYSE_RESULT_EN analyse_result;
	bool_t check_result;

	uC_Watchdog::Force_Feed_Dog();

	switch ( m_write_status )
	{
		case IS25_PREP_TO_WRITE:
			if ( m_done_length == m_total_length )
			{
				main_result_pair.result = IS25_TRUE;
				m_write_status = IS25_CHECKING_DATA;
			}
			else
			{
				m_deal_length = Get_Deal_Length( m_dest_address, m_total_length, m_done_length, IS25_WPAGE );
				m_deal_data = Get_Deal_Data( m_done_length );
				m_deal_address = m_dest_address + m_done_length;
				analyse_result = Analyze_Data( m_deal_data, m_deal_address, m_deal_length );
				switch ( analyse_result )
				{
					case SAME_VAL:
						m_done_length += m_deal_length;
						m_write_status = IS25_PREP_TO_WRITE;
						break;

					case ERASE_VAL:
						Condition_Copy( m_wpage_buff, m_deal_data, m_deal_length );
						m_write_status = IS25_WRITTING_PAGE;
						break;

					case NO_MEANING_VAL:
						m_write_status = IS25_PREP_SAVE_DEST_PAGE;
						break;
				}
			}
			main_result_pair.result = IS25_TRUE;
			break;

		case IS25_WRITTING_PAGE:
			main_result_pair.result = Writting( m_wpage_buff, m_deal_address, m_deal_length );
			if ( main_result_pair.result == IS25_TRUE )
			{
				if ( m_use_protection == FALSE )
				{
					m_done_length += m_deal_length;
					m_write_status = IS25_PREP_TO_WRITE;
				}
				else
				{
					m_recovery_address = Get_Recovery_Address( m_deal_address, TRUE );
					m_write_status = IS25_WRITTING_PROTECTION;
				}
			}
			break;

		case IS25_WRITTING_PROTECTION:
			main_result_pair.result = Writting( m_wpage_buff, m_recovery_address, m_deal_length );
			if ( main_result_pair.result == IS25_TRUE )
			{
				m_done_length += m_deal_length;
				m_write_status = IS25_PREP_TO_WRITE;
			}
			break;

		case IS25_PREP_SAVE_DEST_PAGE:
			m_recovery_address = Get_Recovery_Address( m_deal_address, m_use_protection );
			m_deal_length = Get_Deal_Length( m_deal_address, m_total_length, m_done_length, IS25_EPAGE );
			m_write_status = IS25_SAVING_DEST_PAGE;
			main_result_pair.result = IS25_TRUE;
			break;

		case IS25_SAVING_DEST_PAGE:
			sub_result_pair = Save_Page( m_recovery_address & IS25_EPAGE_ALIGNMENT_MASK,
										 m_deal_address & IS25_EPAGE_ALIGNMENT_MASK, TRUE );
			main_result_pair.result = sub_result_pair.result;
			if ( ( sub_result_pair.status == IS25_FINISH ) && ( main_result_pair.result == IS25_TRUE ) )
			{
				m_write_status = IS25_ERASING_PAGE;
			}
			break;

		case IS25_ERASING_PAGE:
			main_result_pair.result = Erase_Page( m_deal_address & IS25_EPAGE_ALIGNMENT_MASK );
			if ( main_result_pair.result == IS25_TRUE )
			{
				m_write_status = IS25_RECOVERYING_DEST_PAGE;
			}
			break;

		case IS25_RECOVERYING_DEST_PAGE:
			sub_result_pair = Save_Page( m_deal_address & IS25_EPAGE_ALIGNMENT_MASK,
										 m_recovery_address & IS25_EPAGE_ALIGNMENT_MASK, FALSE );
			main_result_pair.result = sub_result_pair.result;
			if ( ( sub_result_pair.status == IS25_FINISH ) && ( main_result_pair.result == IS25_TRUE ) )
			{
				m_done_length += m_deal_length;
				m_write_status = IS25_PREP_TO_WRITE;
			}
			break;

		case IS25_CHECKING_DATA:
			check_result = Check_Data( m_data_ptr, m_dest_address, m_data_length, m_use_checksum );
			if ( ( check_result != FALSE ) && ( m_use_protection != FALSE ) )
			{
				m_recovery_address = Get_Recovery_Address( m_dest_address, TRUE );
				check_result = Check_Data( m_data_ptr, m_recovery_address, m_data_length, m_use_checksum );
			}


			if ( check_result == FALSE )
			{
				main_result_pair.result = IS25_FALSE;
			}
			else
			{
				main_result_pair.result = IS25_TRUE;
			}

			if ( main_result_pair.result == IS25_TRUE )
			{
				// Fault_Manager::Clr(FC_SERIAL_FLASH_FAULT);
			}

			m_write_status = IS25_FINISH;
			break;

		case IS25_FINISH:
			break;
	}

	if ( main_result_pair.result == IS25_FALSE )
	{
		// Fault_Manager::Set(FC_SERIAL_FLASH_FAULT);
		m_write_status = IS25_FINISH;
	}

	main_result_pair.status = m_write_status;

	if ( m_write_status == IS25_FINISH )
	{
		m_done_length = 0U;
		m_write_status = IS25_PREP_TO_WRITE;
	}

	return ( main_result_pair );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
DATA_ANALYSE_RESULT_EN IS25::Analyze_Data( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length )
{
	DATA_ANALYSE_RESULT_EN result = NO_MEANING_VAL;

	m_spi_ctrl->Read_Data( m_wpage_buff, address, length );

	if ( data != &m_erase_val )
	{
		if ( Compare_Data( m_wpage_buff, data, length ) )
		{
			result = SAME_VAL;
		}
		if ( Compare_Data( m_wpage_buff, m_erase_val, length ) )
		{
			result = ERASE_VAL;
		}
	}
	else
	{
		if ( Compare_Data( m_wpage_buff, m_erase_val, length ) )
		{
			result = SAME_VAL;
		}
	}

	return ( result );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uint32_t IS25::Get_Recovery_Address( uint32_t dest_address, bool_t use_protection )
{
	uint32_t recovery_address;

	if ( use_protection == FALSE )
	{
		recovery_address = ( dest_address & ~IS25_EPAGE_ALIGNMENT_MASK ) + IS25_SCRATCH_EPAGE_ADDRESS;
	}
	else
	{
		recovery_address = ( dest_address - m_chip_cfg->start_address ) + m_chip_cfg->mirror_start_address;
	}

	return ( recovery_address );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
NV_CTRL_LENGTH_TD IS25::Get_Deal_Length( uint32_t address, NV_CTRL_LENGTH_TD total_length,
										 NV_CTRL_LENGTH_TD done_length, IS25_PAGE_TYPE_EN page_type )
{
	NV_CTRL_LENGTH_TD deal_length;
	uint32_t page_alignment_mask;
	uint32_t page_size;

	switch ( page_type )
	{
		case IS25_EPAGE:
			page_alignment_mask = IS25_EPAGE_ALIGNMENT_MASK;
			page_size = m_chip_cfg->erase_page_size;
			break;

		case IS25_WPAGE:
			page_alignment_mask = IS25_WPAGE_ALIGNMENT_MASK;
			page_size = m_chip_cfg->write_page_size;
			break;

		default:
			BF_ASSERT( false );
			break;
	}

	deal_length = ( address + done_length ) & ~page_alignment_mask;
	deal_length = page_size - deal_length;

	if ( deal_length > ( total_length - done_length ) )
	{
		deal_length = total_length - done_length;
	}

	return ( deal_length );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uint8_t* IS25::Get_Deal_Data( NV_CTRL_LENGTH_TD done_length )
{
	uint8_t* deal_data_ptr;
	NV_CTRL_LENGTH_TD deal_length;
	NV_CTRL_LENGTH_TD data_copy_length;
	NV_CTRL_LENGTH_TD checksum_copy_length;

	deal_length = Get_Deal_Length( m_dest_address, m_total_length, done_length, IS25_WPAGE );

	if ( ( done_length + deal_length ) > m_data_length )
	{
		data_copy_length = 0U;

		if ( done_length < m_data_length )
		{
			data_copy_length = m_data_length - done_length;
			Condition_Copy( m_shadow_wpage_buff, m_data_ptr + done_length, m_data_length - done_length );
		}

		checksum_copy_length = done_length + deal_length - m_data_length;

		if ( checksum_copy_length > 0 )
		{
			if ( data_copy_length == 0 )
			{
				deal_data_ptr = ( uint8_t* )&m_check_sum + IS25_NV_CHECKSUM_LEN - checksum_copy_length;
			}
			else
			{
				deal_data_ptr = ( uint8_t* )&m_check_sum;
			}

			Condition_Copy( m_shadow_wpage_buff + data_copy_length, deal_data_ptr, checksum_copy_length );
		}
		deal_data_ptr = m_shadow_wpage_buff;
	}
	else
	{
		if ( m_data_ptr != &m_erase_val )
		{
			deal_data_ptr = m_data_ptr + done_length;
		}
		else
		{
			deal_data_ptr = &m_erase_val;
		}
	}

	return ( deal_data_ptr );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void IS25::Condition_Copy( uint8_t* dest_address, uint8_t* source_address, NV_CTRL_LENGTH_TD length )
{
	if ( source_address != &m_erase_val )
	{
		BF_Lib::Copy_String( dest_address, source_address, length );
	}
	else
	{
		for ( NV_CTRL_LENGTH_TD i = 0U; i < length; i++ )
		{
			dest_address[i] = m_erase_val;
		}
	}
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
RESULT_STATE_ST IS25::Save_Page( uint32_t dest_page, uint32_t source_page, bool_t use_ram_val )
{
	RESULT_STATE_ST result_pair = { IS25_FALSE, IS25_PREP_TO_WRITE };
	NV_CTRL_LENGTH_TD read_length;
	NV_CTRL_LENGTH_TD ram_length;
	NV_CTRL_LENGTH_TD count;

	switch ( m_sub_status )
	{
		case IS25_ERASING_PAGE:
			result_pair.result = Erase_Page( dest_page );
			if ( result_pair.result == IS25_TRUE )
			{
				m_sub_status = IS25_PREP_TO_WRITE;
			}
			break;

		case IS25_PREP_TO_WRITE:
			if ( use_ram_val == FALSE )
			{
				m_spi_ctrl->Read_Data( m_wpage_buff, source_page + m_sub_done_length, m_chip_cfg->write_page_size );
			}
			else
			{
				read_length = 0U;
				ram_length = 0U;
				count = 0U;
				while ( count < m_chip_cfg->write_page_size )
				{
					while ( ( ( source_page + m_sub_done_length + count + read_length ) < m_dest_address ) || \
							( ( source_page + m_sub_done_length + count + read_length ) >=
							  ( m_dest_address + m_total_length ) ) )
					{
						if ( ( read_length + count ) < m_chip_cfg->write_page_size )
						{
							read_length++;
						}
						else
						{
							break;
						}
					}

					if ( read_length > 0 )
					{
						m_spi_ctrl->Read_Data( m_wpage_buff + count, source_page + m_sub_done_length + count,
											   read_length );
					}

					count += read_length;
					read_length = 0U;

					while ( ( ( source_page + m_sub_done_length + count + ram_length ) >= m_dest_address ) && \
							( ( source_page + m_sub_done_length + count + ram_length ) <
							  ( m_dest_address + m_data_length ) ) )
					{
						if ( ( ram_length + count ) < m_chip_cfg->write_page_size )
						{
							ram_length++;
						}
						else
						{
							break;
						}
					}

					if ( ram_length > 0 )
					{
						if ( m_data_ptr != &m_erase_val )
						{
							Condition_Copy( m_wpage_buff + count,
											m_data_ptr + source_page + m_sub_done_length + count - m_dest_address,
											ram_length );
						}
						else
						{
							Condition_Copy( m_wpage_buff + count, m_data_ptr, ram_length );
						}
					}

					count += ram_length;
					ram_length = 0U;

					while ( ( ( source_page + m_sub_done_length + count + ram_length ) >=
							  ( m_dest_address + m_data_length ) ) && \
							( source_page + m_sub_done_length + count + ram_length ) <
							( m_dest_address + m_total_length ) )
					{
						if ( ( ram_length + count ) < m_chip_cfg->write_page_size )
						{
							ram_length++;
						}
						else
						{
							break;
						}
					}

					if ( ram_length > 0 )
					{
						Condition_Copy( m_wpage_buff + count,
										( uint8_t* )&m_check_sum + ( source_page + m_sub_done_length + count ) -
										( m_dest_address + m_data_length ), ram_length );
					}

					count += ram_length;
					ram_length = 0U;
				}
			}

			m_sub_status = IS25_WRITTING_PAGE;
			result_pair.result = IS25_TRUE;
			break;

		case IS25_WRITTING_PAGE:
			result_pair.result = Writting( m_wpage_buff, dest_page + m_sub_done_length, m_chip_cfg->write_page_size );

			if ( result_pair.result == IS25_TRUE )
			{
				m_spi_ctrl->Read_Data( m_wpage_buff, dest_page + m_sub_done_length, m_chip_cfg->write_page_size );
				m_sub_status = IS25_PREP_TO_WRITE;
				m_sub_done_length += m_chip_cfg->write_page_size;
			}

			if ( m_sub_done_length == m_chip_cfg->erase_page_size )
			{
				m_sub_status = IS25_FINISH;
			}
			break;
	}

	if ( result_pair.result == IS25_FALSE )
	{
		m_sub_status = IS25_FINISH;
	}

	result_pair.status = m_sub_status;

	if ( m_sub_status == IS25_FINISH )
	{
		m_sub_done_length = 0U;
		m_sub_status = IS25_ERASING_PAGE;
	}

	return ( result_pair );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool_t IS25::Compare_Data( uint8_t* source_data, uint8_t* dest_data, NV_CTRL_LENGTH_TD length )
{
	bool_t result;
	uint32_t index;

	for ( index = 0U; index < length; index++ )
	{
		if ( source_data[index] != dest_data[index] )
		{
			break;
		}
	}

	if ( index != length )
	{
		result = FALSE;
	}
	else
	{
		result = TRUE;
	}

	return ( result );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool_t IS25::Compare_Data( uint8_t* source_data, uint8_t value, NV_CTRL_LENGTH_TD length )
{
	bool_t result;
	uint32_t index;

	for ( index = 0U; index < length; index++ )
	{
		if ( source_data[index] != value )
		{
			break;
		}
	}

	if ( index != length )
	{
		result = FALSE;
	}
	else
	{
		result = TRUE;
	}

	return ( result );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
IS25_RESULT_EN IS25::Check_Chip_Status( TIMERS_TIME_TD duration )
{
	IS25_RESULT_EN result = IS25_PENDING;
	uint8_t status = 0U;
	uint16_t count = 0U;

	if ( m_use_os == FALSE )
	{
		count = CONVERT_MS_TO_COUNT( duration );
		do
		{
			Read_Status( &status );
			count--;
		}while ( Test_Bit( status, IS25_STATUS_BUSY_BIT ) && ( count != 0 ) );

		if ( Test_Bit( status, IS25_STATUS_BUSY_BIT ) == FALSE )
		{
			result = IS25_TRUE;
		}
		else
		{
			result = IS25_FALSE;
		}
	}
	else
	{
		if ( BF_Lib::Timers::Expired( m_start_time, IS25_MAX_ERASE_TIME ) )
		{
			result = IS25_FALSE;
		}

		Read_Status( &status );
		if ( Test_Bit( status, IS25_STATUS_BUSY_BIT ) == FALSE )
		{
			result = IS25_TRUE;
		}
	}

	return ( result );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
IS25_RESULT_EN IS25::Writting( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length )
{
	IS25_RESULT_EN result;

	uC_Watchdog::Force_Feed_Dog();

	switch ( m_long_operate_status )
	{
		case IS25_OPERATE_CMD:
			m_spi_ctrl->Select_Chip( m_chip_select );

			m_spi_ctrl->QSPI_WriteEnable();

			m_spi_ctrl->De_Select_Chip( m_chip_select );

			// An edge has to be sent to the Chip select pin on a write instruction.
			// Nops are there to give at least a 5Mhz pulse width.
			Delay_Chip_Select_Change();

			m_spi_ctrl->Select_Chip( m_chip_select );

			m_spi_ctrl->Write_Data( data, address, length );
			m_spi_ctrl->De_Select_Chip( m_chip_select );

			m_start_time = BF_Lib::Timers::Get_Time();
			result = IS25_PENDING;
			m_long_operate_status = IS25_WAITTING;
			break;

		case IS25_WAITTING:
			result = Check_Chip_Status( IS25_MAX_WRITE_PAGE_TIME );
			break;
	}

	if ( result != IS25_PENDING )
	{
		m_long_operate_status = IS25_OPERATE_CMD;
	}

	Deal_IS25_Result( result );

	return ( result );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool_t IS25::Reading( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length, bool_t use_checksum )
{
	bool_t good_data = true;

	uC_Watchdog::Force_Feed_Dog();

	m_spi_ctrl->Select_Chip( m_chip_select );

	if ( use_checksum == TRUE )
	{
		length += 2U;
	}

	m_spi_ctrl->Read_Data( data, address, length );

	m_spi_ctrl->De_Select_Chip( m_chip_select );

	return ( good_data );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool_t IS25::Check_Data( uint8_t* check_data, uint32_t address, uint32_t length, bool_t use_checksum )
{
	bool_t good_data = TRUE;
	uint8_t read_data;
	uint8_t data[0xFF] = { 0U };
	uint32_t count = 0U;
	BF_Lib::SPLIT_UINT16 checksum;

	m_spi_ctrl->Select_Chip( m_chip_select );
	m_spi_ctrl->Read_Data( data, address, length );

	checksum.u16 = 0U;
	while ( ( length-- ) && ( good_data != FALSE ) )
	{
		read_data = data[count++];

		checksum.u16 = read_data + checksum.u16;

		if ( *check_data != read_data )
		{
			good_data = FALSE;
		}
		if ( check_data != &m_erase_val )
		{
			check_data++;
		}
	}

	if ( ( use_checksum == TRUE ) && ( good_data == TRUE ) )
	{
		read_data = data[count++];
		if ( read_data != checksum.u8[0] )
		{
			good_data = FALSE;
		}

		read_data = data[count++];
		if ( read_data != checksum.u8[1] )
		{
			good_data = FALSE;
		}
	}

	m_spi_ctrl->De_Select_Chip( m_chip_select );

	return ( good_data );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void IS25::Send_Data_Command( uint8_t command, uint32_t address )
{
	/*BF_Lib::SPLIT_uint32_t temp_address;

	   temp_address.u32 = address;

	   switch ( m_chip_cfg->address_length )
	   {
	    case 1:
	        m_spi_ctrl->TX_Byte( command | ( temp_address.u8[1]<<UPPER_ADDRESS_BIT_SHIFT ) );
	        while ( !m_spi_ctrl->TX_Data_Reg_Empty() ){};
	        m_spi_ctrl->TX_Byte( temp_address.u8[0] );
	        while ( !m_spi_ctrl->TX_Data_Reg_Empty() ){};
	        break;

	    default:
	        m_spi_ctrl->TX_Byte( command );
	        while ( !m_spi_ctrl->TX_Data_Reg_Empty() ){};
	        for ( uint32_t i = m_chip_cfg->address_length; i > 0U; i-- )
	        {
	            m_spi_ctrl->TX_Byte( temp_address.u8[i - 1] );
	            while ( !m_spi_ctrl->TX_Data_Reg_Empty() ){};
	        }
	        break;
	   }*/
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool_t IS25::Check_Range( NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length, bool_t use_checksum, bool_t protection )
{
	bool_t range_good = FALSE;
	NV_CTRL_ADDRESS_TD end_address;

	if ( use_checksum != FALSE )
	{
		length += IS25_NV_CHECKSUM_LEN;
	}

	if ( protection == FALSE )
	{
		end_address = m_chip_cfg->end_address;
	}
	else
	{
		end_address = m_chip_cfg->mirror_start_address;
	}

	if ( ( address >= m_chip_cfg->start_address ) && ( ( address + length ) <= end_address ) )
	{
		range_good = TRUE;
	}

	return ( range_good );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void IS25::Quick_Read( uint8_t* data, uint32_t address, NV_CTRL_LENGTH_TD length )
{
	uC_Watchdog::Force_Feed_Dog();

	m_spi_ctrl->Select_Chip( m_chip_select );

	m_spi_ctrl->Read_Data( data, address, length );

	m_spi_ctrl->De_Select_Chip( m_chip_select );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
IS25_RESULT_EN IS25::Erase_Page( uint32_t address )
{
	IS25_RESULT_EN result;

	uC_Watchdog::Force_Feed_Dog();

	switch ( m_long_operate_status )
	{
		case IS25_OPERATE_CMD:
			m_spi_ctrl->Select_Chip( m_chip_select );

			m_spi_ctrl->QSPI_WriteEnable();

			m_spi_ctrl->De_Select_Chip( m_chip_select );

			// An edge has to be sent to the Chip select pin on a write instruction.
			// Nops are there to give at least a 5Mhz pulse width.  Get it?  If not think about it.
			Delay_Chip_Select_Change();

			m_spi_ctrl->Select_Chip( m_chip_select );

			m_spi_ctrl->Erase_Sector( address );
			m_spi_ctrl->De_Select_Chip( m_chip_select );

			m_start_time = BF_Lib::Timers::Get_Time();
			result = IS25_PENDING;
			m_long_operate_status = IS25_WAITTING;
			break;

		case IS25_WAITTING:
			result = Check_Chip_Status( IS25_MAX_ERASE_TIME );
			break;
	}

	if ( result != IS25_PENDING )
	{
		m_long_operate_status = IS25_OPERATE_CMD;
	}

	Deal_IS25_Result( result );

	return ( result );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void IS25::Deal_IS25_Result( IS25_RESULT_EN result )
{
	if ( result == IS25_FALSE )
	{
		// Fault_Manager::Set( FC_SERIAL_FLASH_FAULT );
	}
	else if ( result == IS25_TRUE )
	{
		// Fault_Manager::Clr(FC_SERIAL_FLASH_FAULT);
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
bool_t IS25::Change_Mode( uint8_t mode )
{
	bool_t success = false;

	m_spi_ctrl->Select_Chip( m_chip_select );

	m_spi_ctrl->Change_QSPI_Mode( mode );

	success = true;

	m_spi_ctrl->De_Select_Chip( m_chip_select );

	return ( success );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
uint32_t IS25::Write_Time( uint8_t* data, NV_CTRL_ADDRESS_TD address, NV_CTRL_LENGTH_TD length,
						   bool_t include_erase_time ) const
{
	uint32_t write_time;
	uint32_t test_address;

	// We always change at least a page.
	write_time = m_chip_cfg->page_write_time;
	test_address = m_chip_cfg->start_address + m_chip_cfg->erase_page_size;
	while ( ( address <= test_address ) && ( ( address + length ) > test_address ) )
	{
		write_time += m_chip_cfg->page_erase_time + m_chip_cfg->page_write_time;
		test_address += m_chip_cfg->erase_page_size;
	}

	// Because we write to a scratch space when we are writing to serial flash.
	write_time = write_time * 2U;		// because every write requires a write to buffer space and back again.

	return ( write_time );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
bool_t IS25::Get_Semaphore( void )
{
	bool_t result;

	Push_TGINT();
	if ( isBusy == true )
	{
		result = false;
	}
	else
	{
		isBusy = true;
		result = true;
	}
	Pop_TGINT();

	return ( result );
}

/*
 *****************************************************************************************
 * ----- Function Header -----------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 * ---------------------------------------------------------------------------------------
 *****************************************************************************************
 */
void IS25::Release_Semaphore( void )
{
	Push_TGINT();
	isBusy = false;
	Pop_TGINT();
}

}
