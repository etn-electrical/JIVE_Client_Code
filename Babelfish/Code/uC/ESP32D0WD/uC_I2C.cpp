/**
 *****************************************************************************************
 *	@file		uC_I2C.cpp
 *	@details	See header file for module overview.
 *	@copyright	2018 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "uC_I2C.h"
#include "uC_IO_Config.h"
#include "driver/periph_ctrl.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
static const uint8_t I2C_IO_INIT_LEVEL = 1;
static const uint8_t ENABLE_VALUE = 1;
static const uint8_t DISABLE_VALUE = 0;
static const uint8_t MAX_I2C_BUFF_LENGTH = 100;
static const uint8_t I2C_EVT_QUEUE_LEN = 1;
static const uint8_t I2C_CMD_EVT_ALIVE = 0;
static const uint8_t I2C_CMD_EVT_DONE = 1;
static const uint8_t I2C_MASTER_TOUT_CNUM_DEFAULT = 8;
static const uint8_t HALFCYCLE_DIV_VALUE = 2;
static const uint32_t TICKS_CAL_VALUE = 1000;
static const uint8_t ESP_SLAVE_ADDR = 0x28;
static const uint8_t ACK_CHECK_DIS = 0x0;		///< I2C master will not check ack from slave
static const char* I2C_TAG = "i2c";

/*
 *****************************************************************************************
 *		Static variables
 *****************************************************************************************
 */
// static portMUX_TYPE i2c_spinlock[I2C_NUM_MAX] = {portMUX_INITIALIZER_UNLOCKED, portMUX_INITIALIZER_UNLOCKED};
static DRAM_ATTR i2c_dev_t* const I2C[I2C_NUM_MAX] = { &I2C0, &I2C1 };
static i2c_obj_t* p_i2c_obj[I2C_NUM_MAX] = {0};

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::uC_I2C( uint8_t channel, uint16_t device_address, i2c_mode_t speed_mode,
				address_type_t addr_mode, bool_t synchronus_trnasfer, bool_t enable_dma,
				duty_type_t fastmode_duty_cycle, uC_BASE_I2C_IO_PORT_STRUCT const* io_port )
{
	m_i2c_speed_type = speed_mode;
	m_f_duty_cycle = fastmode_duty_cycle;
	m_channel_number = channel;
	m_chip_address = device_address;
	m_addressing_mode = addr_mode;
	m_dma_transfer = false;
	m_synchronous_type = synchronus_trnasfer;
	m_transfer_status = NO_ERROR_INIT;
	m_io_ctrl = io_port;
	m_config = static_cast<uC_BASE_I2C_IO_STRUCT*>( nullptr );
	Init_I2C( m_channel_number, m_i2c_speed_type, m_f_duty_cycle );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Init_I2C( uint8_t i2c_channel, i2c_mode_t i2c_mode,
					   duty_type_t fmode_duty_cycle )
{
	int cycle;
	int half_cycle;
	esp_err_t ret;
	i2c_port_t i2c_num = ( i2c_port_t )i2c_channel;

	m_config = &( uC_Base::m_i2c_io[i2c_num] );

	Check_I2C( i2c_num < I2C_NUM_MAX, "i2c number error", ESP_ERR_INVALID_ARG );
	Check_I2C( m_config != NULL, "i2c null address error", ESP_ERR_INVALID_ARG );
	ret = Set_I2C_Pins( i2c_num );

	if ( ret != ESP_OK )
	{
		return;
	}
	// Reset the I2C hardware in case there is a soft reboot.
	Disable_I2C( i2c_num );
	Enable_I2C( i2c_num );
	I2C[i2c_num]->ctr.rx_lsb_first = I2C_DATA_MODE_MSB_FIRST;	///< set rx data msb first
	I2C[i2c_num]->ctr.tx_lsb_first = I2C_DATA_MODE_MSB_FIRST;	///< set tx data msb first
	I2C[i2c_num]->ctr.ms_mode = I2C_MODE_MASTER;				///< mode for master or slave
	I2C[i2c_num]->ctr.sda_force_out = ENABLE_VALUE;				///< set open-drain output mode
	I2C[i2c_num]->ctr.scl_force_out = ENABLE_VALUE;				///< set open-drain output mode
	I2C[i2c_num]->ctr.sample_scl_level = DISABLE_VALUE;			///< sample at high level of clock

	// I2C Master mode initialization
	I2C[i2c_num]->fifo_conf.nonfifo_en = DISABLE_VALUE;
	cycle = ( I2C_APB_CLK_FREQ / m_config->clk_speed );
	half_cycle = cycle / HALFCYCLE_DIV_VALUE;
	I2C[i2c_num]->timeout.tout = cycle * I2C_MASTER_TOUT_CNUM_DEFAULT;

	// set timing for data
	I2C[i2c_num]->sda_hold.time = half_cycle / HALFCYCLE_DIV_VALUE;
	I2C[i2c_num]->sda_sample.time = half_cycle / HALFCYCLE_DIV_VALUE;
	I2C[i2c_num]->scl_low_period.period = half_cycle;
	I2C[i2c_num]->scl_high_period.period = half_cycle;

	// set timing for start signal
	I2C[i2c_num]->scl_start_hold.time = half_cycle;
	I2C[i2c_num]->scl_rstart_setup.time = half_cycle;

	// set timing for stop signal
	I2C[i2c_num]->scl_stop_hold.time = half_cycle;
	I2C[i2c_num]->scl_stop_setup.time = half_cycle;

	I2C_Driver_Install( i2c_num, DISABLE_VALUE,
						DISABLE_VALUE, DISABLE_VALUE );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Write( uint16_t address, uint16_t length, uint8_t* datasource,
								   i2c_status_t* write_status, i2c_cmd_handle_t cmd )
{
	return_type error = NO_ERROR;
	esp_err_t ret;
	i2c_port_t i2c_num = ( i2c_port_t )m_channel_number;

	m_chip_address = address;
	Generate_Start( cmd );
	Send_Data( ( m_chip_address << 1 ) | I2C_MASTER_WRITE, ACK_CHECK_EN, cmd );

	Check_I2C( ( datasource != NULL ), "i2c null address error", ESP_ERR_INVALID_ARG );
	Check_I2C( cmd != NULL, "i2c command link error", ESP_ERR_INVALID_ARG );

	i2c_master_write( cmd, datasource, length, ACK_CHECK_EN );
	Set_Stop( cmd );

	ret = i2c_master_cmd_begin( i2c_num, cmd, TICKS_CAL_VALUE / portTICK_RATE_MS );
	i2c_cmd_link_delete( cmd );
	if ( ret == ESP_OK )
	{
		error = NO_ERROR;
	}
	else
	{
		error = I2C_ERROR;
	}
	return ( error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Read( uint16_t address, uint16_t length, uint8_t* datasource,
								  i2c_status_t* read_status, i2c_cmd_handle_t cmd )
{
	return_type error = NO_ERROR;
	esp_err_t ret;
	i2c_port_t i2c_num = ( i2c_port_t ) m_channel_number;

	m_chip_address = address;

	if ( length == 0 )
	{
		return ( NO_ERROR );
	}
	Generate_Start( cmd );
	Send_Data( ( m_chip_address << 1 ) | I2C_MASTER_READ, ACK_CHECK_EN, cmd );

	if ( length > 1 )
	{
		length = length - ENABLE_VALUE;
		Check_I2C( length > 0, "i2c data read length error", ESP_ERR_INVALID_ARG );
		Check_I2C( ( datasource != NULL ), "i2c null address error", ESP_ERR_INVALID_ARG );
		Check_I2C( cmd != NULL, "i2c command link error", ESP_ERR_INVALID_ARG );
		i2c_master_read( cmd, datasource, length, I2C_MASTER_ACK );
	}

	error = Receive_Data( datasource, I2C_MASTER_NACK, cmd );
	Set_Stop( cmd );

	ret = i2c_master_cmd_begin( i2c_num, cmd, TICKS_CAL_VALUE / portTICK_RATE_MS );
	i2c_cmd_link_delete( cmd );
	if ( ret == ESP_OK )
	{
		error = NO_ERROR;
	}
	else
	{
		error = I2C_ERROR;
	}
	return ( error );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Send_Device_Address( uint16_t devaddr,
												 uint8_t read_write )
{
	return_type addrs_ack_status = NO_ERROR;

	m_chip_address = ESP_SLAVE_ADDR;

	return ( addrs_ack_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Generate_Start( i2c_cmd_handle_t cmd_handle )
{
	esp_err_t error;

	Check_I2C( cmd_handle != NULL, "i2c command link error", ESP_ERR_INVALID_ARG );
	error = i2c_master_start( cmd_handle );
	if ( error == ESP_OK )
	{
		return ( NO_ERROR );
	}
	else
	{
		return ( I2C_ERROR );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Set_Stop( i2c_cmd_handle_t cmd_handle )
{
	esp_err_t error;

	Check_I2C( cmd_handle != NULL, "i2c command link error", ESP_ERR_INVALID_ARG );
	error = i2c_master_stop( cmd_handle );
	if ( error == ESP_OK )
	{
		return ( NO_ERROR );
	}
	else
	{
		return ( I2C_ERROR );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Send_Data( uint8_t data, bool ack_en, i2c_cmd_handle_t cmd_handle )
{
	esp_err_t error;

	Check_I2C( cmd_handle != NULL, "i2c command link error", ESP_ERR_INVALID_ARG );
	error = i2c_master_write_byte( cmd_handle, data, ack_en );
	if ( error == ESP_OK )
	{
		return ( NO_ERROR );
	}
	else
	{
		return ( I2C_ERROR );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::return_type uC_I2C::Receive_Data( uint8_t* data, i2c_ack_type_t ack, i2c_cmd_handle_t cmd_handle ) const
{
	esp_err_t error;

	Check_I2C( ( data != NULL ), "i2c null address error", ESP_ERR_INVALID_ARG );
	Check_I2C( cmd_handle != NULL, "i2c command link error", ESP_ERR_INVALID_ARG );
	Check_I2C( ack < I2C_MASTER_ACK_MAX, "i2c ack type error", ESP_ERR_INVALID_ARG );
	error = i2c_master_read_byte( cmd_handle, data, I2C_MASTER_NACK );
	if ( error == ESP_OK )
	{
		return ( NO_ERROR );
	}
	else
	{
		return ( I2C_ERROR );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Enable_I2C( i2c_port_t i2c_num )
{
	if ( i2c_num == I2C_NUM_0 )
	{
		periph_module_enable( PERIPH_I2C0_MODULE );
	}
	else if ( i2c_num == I2C_NUM_1 )
	{
		periph_module_enable( PERIPH_I2C1_MODULE );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_I2C::i2c_status_t uC_I2C::I2C_Transfer_Status( void )
{
	return ( m_transfer_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Attach_Callback( cback_func_t cback_func, cback_param_t param )
{
	interrupt_config_t* interrupt_config = reinterpret_cast<interrupt_config_t*>( param );
	i2c_port_t i2c_num = ( i2c_port_t ) m_channel_number;
	uint8_t int_priority = DISABLE_VALUE;

	Check_I2C( i2c_num < I2C_NUM_MAX, "i2c number error", ESP_ERR_INVALID_ARG );
	Check_I2C( interrupt_config->handler != NULL, "i2c null address error", ESP_ERR_INVALID_ARG );

	interrupt_config->intrstatusreg = DISABLE_VALUE;
	interrupt_config->intrstatusmask = DISABLE_VALUE;

	switch ( i2c_num )
	{
		case I2C_NUM_1:
			interrupt_config->source = ETS_I2C_EXT1_INTR_SOURCE;
			break;

		case I2C_NUM_0:
		default:
			interrupt_config->source = ETS_I2C_EXT0_INTR_SOURCE;
			break;
	}
	uC_Interrupt::Set_Vector( reinterpret_cast<INT_CALLBACK_FUNC*>( interrupt_config ), MAX_IRQTYPE,
							  int_priority );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::I2C_ISR( void* arg )
{
	i2c_obj_t* p_i2c = ( i2c_obj_t* ) arg;
	int32_t i2c_num = p_i2c->i2c_num;
	uint32_t status = I2C[i2c_num]->int_status.val;
	int32_t idx = 0;
	int32_t evt;

	portBASE_TYPE HPTaskAwoken = pdFALSE;

	while ( status != 0 )
	{
		status = I2C[i2c_num]->int_status.val;
		if ( status & I2C_TX_SEND_EMPTY_INT_ST_M )
		{
			I2C[i2c_num]->int_clr.tx_send_empty = ENABLE_VALUE;
		}
		else if ( status & I2C_RX_REC_FULL_INT_ST_M )
		{
			I2C[i2c_num]->int_clr.rx_rec_full = ENABLE_VALUE;
		}
		else if ( status & I2C_ACK_ERR_INT_ST_M )
		{
			I2C[i2c_num]->int_ena.ack_err = DISABLE_VALUE;
			I2C[i2c_num]->int_clr.ack_err = ENABLE_VALUE;
			if ( p_i2c->mode == I2C_MODE_MASTER )
			{
				p_i2c_obj[i2c_num]->status = uC_I2C::I2C_STATUS_ACK_ERROR;
				I2C[i2c_num]->int_clr.ack_err = ENABLE_VALUE;
				// get error ack value from slave device, stop the commands
				uC_I2C::I2C_ISR_Static( nullptr, i2c_num );
			}
		}
		else if ( status & I2C_TRANS_START_INT_ST_M )
		{
			I2C[i2c_num]->int_clr.trans_start = ENABLE_VALUE;
		}
		else if ( status & I2C_TIME_OUT_INT_ST_M )
		{
			I2C[i2c_num]->int_ena.time_out = DISABLE_VALUE;
			I2C[i2c_num]->int_clr.time_out = ENABLE_VALUE;
			p_i2c_obj[i2c_num]->status = uC_I2C::I2C_STATUS_TIMEOUT;
			uC_I2C::I2C_ISR_Static( nullptr, i2c_num );
		}
		else if ( status & I2C_TRANS_COMPLETE_INT_ST_M )
		{
			I2C[i2c_num]->int_clr.trans_complete = ENABLE_VALUE;
			if ( p_i2c->mode == I2C_MODE_SLAVE )
			{
				int rx_fifo_cnt = I2C[i2c_num]->status_reg.rx_fifo_cnt;
				for ( idx = 0; idx < rx_fifo_cnt; idx++ )
				{
					p_i2c->data_buf[idx] = I2C[i2c_num]->fifo_data.data;
				}
				xRingbufferSendFromISR( p_i2c->rx_ring_buf, p_i2c->data_buf, rx_fifo_cnt, &HPTaskAwoken );
				I2C[i2c_num]->int_clr.rx_fifo_full = ENABLE_VALUE;
			}
			else
			{
				// add check for unexcepted situations caused by noise.
				if ( ( p_i2c->status != uC_I2C::I2C_STATUS_ACK_ERROR ) && ( p_i2c->status != uC_I2C::I2C_STATUS_IDLE ) )
				{
					uC_I2C::I2C_ISR_Static( nullptr, i2c_num );
				}
			}
		}
		else if ( status & I2C_MASTER_TRAN_COMP_INT_ST_M )
		{
			I2C[i2c_num]->int_clr.master_tran_comp = ENABLE_VALUE;
		}
		else if ( status & I2C_ARBITRATION_LOST_INT_ST_M )
		{
			I2C[i2c_num]->int_clr.arbitration_lost = ENABLE_VALUE;
			p_i2c_obj[i2c_num]->status = uC_I2C::I2C_STATUS_TIMEOUT;
			uC_I2C::I2C_ISR_Static( nullptr, i2c_num );
		}
		else if ( status & I2C_SLAVE_TRAN_COMP_INT_ST_M )
		{
			I2C[i2c_num]->int_clr.slave_tran_comp = ENABLE_VALUE;
		}
		else if ( status & I2C_END_DETECT_INT_ST_M )
		{
			I2C[i2c_num]->int_ena.end_detect = DISABLE_VALUE;
			I2C[i2c_num]->int_clr.end_detect = ENABLE_VALUE;
			uC_I2C::I2C_ISR_Static( nullptr, i2c_num );
		}
		else if ( status & I2C_RXFIFO_OVF_INT_ST_M )
		{
			I2C[i2c_num]->int_clr.rx_fifo_ovf = ENABLE_VALUE;
		}
		else if ( status & I2C_TXFIFO_EMPTY_INT_ST_M )
		{
			int32_t tx_fifo_rem = SOC_I2C_FIFO_LEN - I2C[i2c_num]->status_reg.tx_fifo_cnt;
			size_t size = DISABLE_VALUE;
			uint8_t* data = ( uint8_t* ) xRingbufferReceiveUpToFromISR( p_i2c->tx_ring_buf, &size, tx_fifo_rem );
			if ( data )
			{
				for ( idx = 0; idx < size; idx++ )
				{
					WRITE_PERI_REG( I2C_DATA_APB_REG( i2c_num ), data[idx] );
				}
				vRingbufferReturnItemFromISR( p_i2c->tx_ring_buf, data, &HPTaskAwoken );
				I2C[i2c_num]->int_ena.tx_fifo_empty = ENABLE_VALUE;
				I2C[i2c_num]->int_clr.tx_fifo_empty = ENABLE_VALUE;
			}
			else
			{
				I2C[i2c_num]->int_ena.tx_fifo_empty = DISABLE_VALUE;
				I2C[i2c_num]->int_clr.tx_fifo_empty = ENABLE_VALUE;
			}
		}
		else if ( status & I2C_RXFIFO_FULL_INT_ST_M )
		{
			int32_t rx_fifo_cnt = I2C[i2c_num]->status_reg.rx_fifo_cnt;
			for ( idx = 0; idx < rx_fifo_cnt; idx++ )
			{
				p_i2c->data_buf[idx] = I2C[i2c_num]->fifo_data.data;
			}
			xRingbufferSendFromISR( p_i2c->rx_ring_buf, p_i2c->data_buf, rx_fifo_cnt, &HPTaskAwoken );
			I2C[i2c_num]->int_clr.rx_fifo_full = ENABLE_VALUE;
		}
		else
		{
			I2C[i2c_num]->int_clr.val = status;
		}
	}
	if ( p_i2c->mode == I2C_MODE_MASTER )
	{
		evt = I2C_CMD_EVT_ALIVE;
		xQueueSendFromISR( p_i2c->cmd_evt_queue, &evt, &HPTaskAwoken );
	}
	// We only need to check here if there is a high-priority task needs to be switched.
	if ( HPTaskAwoken == pdTRUE )
	{
		portYIELD_FROM_ISR();
	}

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::I2C_ISR_Static( void* object, int32_t i2c_num )
{
	i2c_obj_t* p_i2c = p_i2c_obj[i2c_num];
	portBASE_TYPE HPTaskAwoken = pdFALSE;
	int32_t evt;

	if ( p_i2c->status == I2C_STATUS_DONE )
	{
		return;
	}
	else if ( ( p_i2c->status == I2C_STATUS_ACK_ERROR ) ||
			  ( p_i2c->status == I2C_STATUS_TIMEOUT ) )
	{
		I2C[i2c_num]->int_ena.end_detect = DISABLE_VALUE;
		I2C[i2c_num]->int_clr.end_detect = ENABLE_VALUE;
		if ( p_i2c->status == I2C_STATUS_TIMEOUT )
		{
			I2C[i2c_num]->int_clr.time_out = ENABLE_VALUE;
			I2C[i2c_num]->int_ena.val = DISABLE_VALUE;
		}
		evt = I2C_CMD_EVT_DONE;
		xQueueOverwriteFromISR( p_i2c->cmd_evt_queue, &evt, &HPTaskAwoken );
		if ( HPTaskAwoken == pdTRUE )
		{
			portYIELD_FROM_ISR();
		}
		return;
	}
	else if ( ( p_i2c->cmd_link.head != NULL ) && ( p_i2c->status == I2C_STATUS_READ ) )
	{
		i2c_cmd_t* cmd = &p_i2c->cmd_link.head->cmd;
		while ( p_i2c->rx_cnt-- > 0 )
		{
			*cmd->data++ = READ_PERI_REG( I2C_DATA_APB_REG( i2c_num ) );
		}
		if ( cmd->byte_num > 0 )
		{
			p_i2c->rx_fifo_remain = SOC_I2C_FIFO_LEN;
			p_i2c->cmd_idx = DISABLE_VALUE;
		}
		else
		{
			p_i2c->cmd_link.head = p_i2c->cmd_link.head->next;
		}
	}
	if ( p_i2c->cmd_link.head == NULL )
	{
		p_i2c->cmd_link.cur = NULL;
		evt = I2C_CMD_EVT_DONE;
		xQueueOverwriteFromISR( p_i2c->cmd_evt_queue, &evt, &HPTaskAwoken );
		if ( HPTaskAwoken == pdTRUE )
		{
			portYIELD_FROM_ISR();
		}
		// Return to the IDLE status after cmd_eve_done signal were send out.
		p_i2c->status = uC_I2C::I2C_STATUS_IDLE;
		return;
	}
	while ( p_i2c->cmd_link.head )
	{
		i2c_cmd_t* cmd = &p_i2c->cmd_link.head->cmd;
		I2C[i2c_num]->command[p_i2c->cmd_idx].val = DISABLE_VALUE;
		I2C[i2c_num]->command[p_i2c->cmd_idx].ack_en = cmd->ack_en;
		I2C[i2c_num]->command[p_i2c->cmd_idx].ack_exp = cmd->ack_exp;
		I2C[i2c_num]->command[p_i2c->cmd_idx].ack_val = cmd->ack_val;
		I2C[i2c_num]->command[p_i2c->cmd_idx].byte_num = cmd->byte_num;
		I2C[i2c_num]->command[p_i2c->cmd_idx].op_code = cmd->op_code;
		if ( cmd->op_code == I2C_CMD_WRITE )
		{
			uint32_t wr_filled = DISABLE_VALUE;
			if ( cmd->data )
			{
				while ( p_i2c->tx_fifo_remain > 0 && cmd->byte_num > 0 )
				{
					WRITE_PERI_REG( I2C_DATA_APB_REG( i2c_num ), *cmd->data++ );
					p_i2c->tx_fifo_remain--;
					cmd->byte_num--;
					wr_filled++;
				}
			}
			else
			{
				WRITE_PERI_REG( I2C_DATA_APB_REG( i2c_num ), cmd->byte_cmd );
				p_i2c->tx_fifo_remain--;
				cmd->byte_num--;
				wr_filled++;
			}
			I2C[i2c_num]->command[p_i2c->cmd_idx].byte_num = wr_filled;
			I2C[i2c_num]->command[p_i2c->cmd_idx + 1].val = DISABLE_VALUE;
			I2C[i2c_num]->command[p_i2c->cmd_idx + 1].op_code = I2C_CMD_END;
			p_i2c->tx_fifo_remain = SOC_I2C_FIFO_LEN;
			p_i2c->cmd_idx = DISABLE_VALUE;
			if ( cmd->byte_num > 0 )
			{}
			else
			{
				p_i2c->cmd_link.head = p_i2c->cmd_link.head->next;
			}
			p_i2c->status = I2C_STATUS_WRITE;
			break;
		}
		else if ( cmd->op_code == I2C_CMD_READ )
		{
			p_i2c->rx_cnt = cmd->byte_num > p_i2c->rx_fifo_remain ? p_i2c->rx_fifo_remain : cmd->byte_num;
			cmd->byte_num -= p_i2c->rx_cnt;
			I2C[i2c_num]->command[p_i2c->cmd_idx].byte_num = p_i2c->rx_cnt;
			I2C[i2c_num]->command[p_i2c->cmd_idx].ack_val = cmd->ack_val;
			I2C[i2c_num]->command[p_i2c->cmd_idx + 1].val = DISABLE_VALUE;
			I2C[i2c_num]->command[p_i2c->cmd_idx + 1].op_code = I2C_CMD_END;
			p_i2c->status = I2C_STATUS_READ;
			break;
		}
		else
		{}
		p_i2c->cmd_idx++;
		p_i2c->cmd_link.head = p_i2c->cmd_link.head->next;
		if ( ( p_i2c->cmd_link.head == NULL ) || ( p_i2c->cmd_idx >= 15 ) )
		{
			p_i2c->tx_fifo_remain = SOC_I2C_FIFO_LEN;
			p_i2c->cmd_idx = DISABLE_VALUE;
			break;
		}
	}
	I2C[i2c_num]->int_clr.end_detect = ENABLE_VALUE;
	I2C[i2c_num]->int_ena.end_detect = ENABLE_VALUE;
	I2C[i2c_num]->ctr.trans_start = DISABLE_VALUE;
	I2C[i2c_num]->ctr.trans_start = ENABLE_VALUE;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_I2C::Check_I2C( bool validation_result, const char* str, esp_err_t error_type )
{
	if ( !( validation_result ) )
	{
		ESP_LOGE( I2C_TAG, "%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str );      \
		return ( error_type );
	}
	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_I2C::Set_I2C_Pins( i2c_port_t i2c_num )
{
	int32_t sda_in_sig, sda_out_sig, scl_in_sig, scl_out_sig;
	uC_PERIPH_IO_STRUCT temp_PERIPH_IO_STRUCT;

	Check_I2C( ( ( m_config->sda_io_num < 0 ) || ( ( uC_IO_Config::GPIO_Output_Validation( m_config->sda_io_num ) ) ) ),
			   "sda gpio number error", ESP_ERR_INVALID_ARG );
	Check_I2C( m_config->scl_io_num < 0 ||
			   ( uC_IO_Config::GPIO_Output_Validation( m_config->scl_io_num ) ) ||
			   ( uC_IO_Config::GPIO_Validation( m_config->scl_io_num ) && m_config->mode == I2C_MODE_SLAVE ),
			   "scl gpio number error", ESP_ERR_INVALID_ARG );
	Check_I2C( m_config->sda_io_num < 0 ||
			   ( m_config->sda_pullup_en == GPIO_PULLUP_ENABLE &&
				 uC_IO_Config::GPIO_Output_Validation(
					 m_config->sda_io_num ) ) ||
			   m_config->sda_pullup_en == GPIO_PULLUP_DISABLE, "this i2c pin does not support internal pull-up",
			   ESP_ERR_INVALID_ARG );
	Check_I2C( m_config->scl_io_num < 0 ||
			   ( m_config->scl_pullup_en == GPIO_PULLUP_ENABLE &&
				 uC_IO_Config::GPIO_Output_Validation(
					 m_config->scl_io_num ) ) ||
			   m_config->scl_pullup_en == GPIO_PULLUP_DISABLE, "this i2c pin does not support internal pull-up",
			   ESP_ERR_INVALID_ARG );

	switch ( i2c_num )
	{
		case I2C_NUM_1:
			sda_out_sig = I2CEXT1_SDA_OUT_IDX;
			sda_in_sig = I2CEXT1_SDA_IN_IDX;
			scl_out_sig = I2CEXT1_SCL_OUT_IDX;
			scl_in_sig = I2CEXT1_SCL_IN_IDX;
			break;

		case I2C_NUM_0:
		default:
			sda_out_sig = I2CEXT0_SDA_OUT_IDX;
			sda_in_sig = I2CEXT0_SDA_IN_IDX;
			scl_out_sig = I2CEXT0_SCL_OUT_IDX;
			scl_in_sig = I2CEXT0_SCL_IN_IDX;
			break;
	}

	if ( m_config->sda_io_num >= 0 )
	{
		temp_PERIPH_IO_STRUCT.io_num = m_config->sda_io_num;
		uC_IO_Config::Set_GPIO_Level( m_config->sda_io_num, I2C_IO_INIT_LEVEL );
		PIN_FUNC_SELECT( GPIO_PIN_MUX_REG[m_config->sda_io_num], PIN_FUNC_GPIO );
		uC_IO_Config::Set_GPIO_Direction( m_config->sda_io_num, GPIO_MODE_INPUT_OUTPUT_OD );
		if ( m_config->sda_pullup_en == GPIO_PULLUP_ENABLE )
		{
			uC_IO_Config::Enable_Periph_Input_Pin( &temp_PERIPH_IO_STRUCT, uC_IO_IN_CFG_PULLUP );
		}
		else
		{
			uC_IO_Config::Disable_Periph_Pin_Set_To_Input( &temp_PERIPH_IO_STRUCT, uC_IO_IN_CFG_HIZ );
		}
		gpio_matrix_out( m_config->sda_io_num, sda_out_sig, false, false );
		gpio_matrix_in( m_config->sda_io_num, sda_in_sig, false );
	}

	if ( m_config->scl_io_num >= 0 )
	{
		temp_PERIPH_IO_STRUCT.io_num = m_config->scl_io_num;
		uC_IO_Config::Set_GPIO_Level( m_config->scl_io_num, I2C_IO_INIT_LEVEL );
		PIN_FUNC_SELECT( GPIO_PIN_MUX_REG[m_config->scl_io_num], PIN_FUNC_GPIO );
		uC_IO_Config::Set_GPIO_Direction( m_config->scl_io_num, GPIO_MODE_INPUT_OUTPUT_OD );
		gpio_matrix_out( m_config->scl_io_num, scl_out_sig, DISABLE_VALUE, DISABLE_VALUE );

		if ( m_config->scl_pullup_en == GPIO_PULLUP_ENABLE )
		{
			uC_IO_Config::Enable_Periph_Input_Pin( &temp_PERIPH_IO_STRUCT, uC_IO_IN_CFG_PULLUP );
		}
		else
		{
			uC_IO_Config::Disable_Periph_Pin_Set_To_Input( &temp_PERIPH_IO_STRUCT, uC_IO_IN_CFG_HIZ );
		}
		gpio_matrix_in( m_config->scl_io_num, scl_in_sig, DISABLE_VALUE );
	}
	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void uC_I2C::Disable_I2C( i2c_port_t i2c_num )
{
	if ( i2c_num == I2C_NUM_0 )
	{
		periph_module_disable( PERIPH_I2C0_MODULE );
	}
	else if ( i2c_num == I2C_NUM_1 )
	{
		periph_module_disable( PERIPH_I2C1_MODULE );
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_I2C::I2C_Driver_Install( i2c_port_t i2c_num, size_t slv_rx_buf_len, size_t slv_tx_buf_len,
									  int32_t intr_alloc_flags )
{
	return ( i2c_driver_install( i2c_num, I2C_MODE_MASTER, slv_rx_buf_len,
								 slv_tx_buf_len, intr_alloc_flags ) );
}
