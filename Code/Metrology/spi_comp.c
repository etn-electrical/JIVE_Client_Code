/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Smart Breaker 2.0
 *
 * File name        : spi_comp.c
 *
 * Author           : Kothe, AARTI J
 *
 * Last Changed By  : $Kothe, AARTI J $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 02/09/2024
 *
 ****************************************************************************/
/****************************************************************************
*                           	Include files
****************************************************************************/
#include "spi_comp.h"
/****************************************************************************
*                              Global variables
****************************************************************************/
spi_device_handle_t spi;
crc_union g_readCRC;
extern metro_waveform_burst_data g_metroWaveformData;

/****************************************************************************
*                    Local function Definitions
****************************************************************************/
/**
 * @brief      This function Send-receive 16 bit data
 * @param[in]  *data_snd, passes pointer to locate array to send data
 * @retval     metro_err_t err type
*/
metro_err_t SPI_Comp_SendAndReceivePacket_16(uint8_t * data_snd, uint8_t * data_rcv)
{
	metro_err_t ret = METRO_SUCCESS;
	DMA_ATTR static uint8_t tx_buffer[SPI_PACKET16_BUF_SIZE];
	DMA_ATTR static uint8_t rx_buffer[SPI_PACKET16_BUF_SIZE_CRC];

	memcpy(tx_buffer, data_snd, SPI_PACKET16_BUF_SIZE);	//Load Tx buffer with 16 bit data

		//Create packet

	spi_transaction_t temp_trans = {
				.length = BYTE * SPI_PACKET16_BUF_SIZE_CRC,
				.tx_buffer = tx_buffer,
				.rx_buffer = rx_buffer,
		};

		//Check SPI read success

	//Check SPI read success

	if (spi_device_polling_transmit(spi, &temp_trans) != ESP_OK)
	{
		if (spi_device_polling_transmit(spi, &temp_trans) != ESP_OK)
		{
			ret = METRO_SPI_ERR;	//SPI ERROR
		}
	}
	else
	{

		//Load Rx buffer with 16 bit data

#ifdef METRO_SPI_CRC_EN
		memcpy(data_rcv, rx_buffer, SPI_PACKET16_BUF_SIZE_CRC);
#else
		memcpy(data_rcv, rx_buffer, SPI_PACKET16_BUF_SIZE);
#endif
	}

	return ret;
}


/**
 * @brief      This function Send-receive 32 bit data
 * @param[in]  *data_snd, passes pointer to locate array to send data
 * @retval     metro_err_t err type
*/
metro_err_t SPI_Comp_SendAndReceivePacket_32(uint8_t * data_snd, uint8_t * data_rcv)
{
	metro_err_t ret = METRO_SUCCESS;
	DMA_ATTR static uint8_t tx_buffer[SPI_PACKET32_BUF_SIZE];
	DMA_ATTR static uint8_t rx_buffer[SPI_PACKET32_BUF_SIZE_CRC];

	memcpy(tx_buffer, data_snd, SPI_PACKET32_BUF_SIZE);	//Load Tx buffer with 32 bit data

		//Create packet

	spi_transaction_t temp_trans = {
				.length = BYTE * SPI_PACKET32_BUF_SIZE_CRC,
				.tx_buffer = tx_buffer,
				.rx_buffer = rx_buffer,
		};

	if (spi_device_polling_transmit(spi, &temp_trans) != ESP_OK)
	{
		if (spi_device_polling_transmit(spi, &temp_trans) != ESP_OK)
		{
			ret = METRO_SPI_ERR;	//SPI ERROR
		}
	}
	else
	{

		//Load Rx buffer with 16 bit data

#ifdef METRO_SPI_CRC_EN
		memcpy(data_rcv, rx_buffer, SPI_PACKET32_BUF_SIZE_CRC);
#else
		memcpy(data_rcv, rx_buffer, SPI_PACKET32_BUF_SIZE);
#endif
	}

	return ret;
}

/**
 * @brief      This function receive burst data
 * @param[in]  *data_snd, passes pointer to locate array to send data
 * @param[in]  total bytes, maximum no of bytes for burst read, uint8_t* address of buffer which hold received data
 * @retval     metro_err_t error type
*/
metro_err_t SPI_Comp_SendAndReceivePacket_burst(uint8_t * data_snd, uint16_t total_bytes, uint8_t *rx_buffer)
{
	metro_err_t ret = METRO_SUCCESS;
	DMA_ATTR static uint8_t tx_buffer[BURST_READ_TX_BUF_SIZE];

	memcpy(tx_buffer, data_snd, BURST_READ_TX_BUF_SIZE);
	memset(rx_buffer, 0, total_bytes * sizeof(uint8_t));

	spi_transaction_t temp_trans = {
				.length = BYTE * total_bytes, // 32 bits
				.tx_buffer = tx_buffer,
				.rx_buffer = rx_buffer,
	};

	if ((spi_device_polling_transmit(spi, &temp_trans)) != ESP_OK)
	{
		ret = METRO_SPI_ERR;	//SPI ERROR
	}

	return ret;
}

/**
 * @brief      This function write 16 bit data to ADE9030 via SPI
 * @param[in]  uint16_t us_ADE_Addr : address of the register
 * @param[in]  uint16_t Reg_Data : data to write
 * @retval     metro_err_t Err type
*/
metro_err_t SPI_Write_ADE9039_Reg_16(uint16_t us_ADE_Addr, uint16_t Reg_Data)
{
	metro_err_t ret = METRO_SUCCESS;
	uint16_t us_iAddress;
	us_iAddress = us_ADE_Addr;
	uint8_t data_snd[SPI_TX_BUF_SIZE];
	uint8_t data_rcv[SPI_RX_BUF_SIZE];

	memset(data_rcv, 0, sizeof(data_rcv));

		//Separate address from SPI data

	us_iAddress = (us_iAddress << 4);
	us_iAddress = (us_iAddress & 0xFFF7);

		//Separate data from SPI data

	data_snd[0] = (us_iAddress & 0xff00)>>8;
	data_snd[1] = us_iAddress & 0x00ff;
	data_snd[2] = (Reg_Data >> 8) & 0xff;
	data_snd[3] =  Reg_Data & 0x00ff;
	data_snd[4] = 0;
	data_snd[5] = 0;

	if ((SPI_Comp_SendAndReceivePacket_16(data_snd, data_rcv)) != ESP_OK)
	{
		if ((SPI_Comp_SendAndReceivePacket_16(data_snd, data_rcv)) != ESP_OK)
		{
			ret = METRO_SPI_ERR;	//SPI ERROR
		}
	}

	return ret;
}

/**
 * @brief      This function write 32 bit data to ADE9030 via SPI
 * @param[in]  uint16_t us_ADE_Addr : address of the register
 * @param[in]  uint32_t Reg_Data : data to write
 * @retval     metro_err_t err type
*/
metro_err_t SPI_Write_ADE9039_Reg_32(uint16_t us_ADE_Addr, uint32_t Reg_Data)
{
	metro_err_t ret = METRO_SUCCESS;
	uint16_t us_iAddress;
	us_iAddress = us_ADE_Addr;
	uint8_t data_snd[SPI_TX_BUF_SIZE];
	uint8_t data_rcv[SPI_RX_BUF_SIZE];

	memset(data_rcv, 0, sizeof(data_rcv));

		//Separate address from SPI data

	us_iAddress = (us_iAddress << 4);
	us_iAddress = (us_iAddress & 0xFFF7);

		//Separate address from SPI data

	data_snd[0] = (us_iAddress & 0xff00)>>8;
	data_snd[1] = us_iAddress & 0x00ff;
	data_snd[2] = (Reg_Data >> 24) & 0xff;
	data_snd[3] = (Reg_Data >> 16) & 0x00ff;
	data_snd[4] = (Reg_Data >> 8) & 0x0000ff;
	data_snd[5] = Reg_Data & 0x000000ff;

	if ((SPI_Comp_SendAndReceivePacket_32(data_snd, data_rcv)) != ESP_OK)
	{
		if ((SPI_Comp_SendAndReceivePacket_32(data_snd, data_rcv)) != ESP_OK)
		{
			ret = METRO_SPI_ERR;	//SPI ERROR
		}
	}

	return ret;
}


/**
 * @brief      This function read 16 bit data from ADE9030 via SPI
 * @param[in]  uint16_t us_ADE_Addr : address of the register
 * @retval     metro_err_t err type
*/
metro_err_t SPI_Read_ADE9039_Reg_16(uint16_t  us_ADE_Addr, uint16_t* spi_read_data)
{
	metro_err_t ret = METRO_SUCCESS;
	uint16_t us_iAddress;
	us_iAddress = us_ADE_Addr;
	uint8_t data_snd[SPI_TX_BUF_SIZE];
	uint8_t data_rcv[SPI_RX_BUF_SIZE];
	spi_read_data_union spiReadData;

	memset(data_rcv, 0 , sizeof(data_rcv));
	memset(data_snd, 0 , sizeof(data_snd));

		//Separate address from SPI data

	us_iAddress = (us_iAddress << 4);
	us_iAddress = (us_iAddress | 0x08);

	data_snd[0] = (us_iAddress & 0xff00)>>8;
	data_snd[1] = us_iAddress & 0x00ff;

	if ((SPI_Comp_SendAndReceivePacket_16(data_snd, data_rcv)) != ESP_OK)
	{
		if ((SPI_Comp_SendAndReceivePacket_16(data_snd, data_rcv)) != ESP_OK)
		{
			ret = METRO_SPI_ERR;	//SPI ERROR
		}
	}
	else
	{
		memcpy(spiReadData.temp, &data_rcv[SPI_DATA_START_INDEX], SPI_DATA16_SIZE);
		*spi_read_data = Metro_Reverse_bytes_16(spiReadData.pm_data_16);

#ifdef METRO_SPI_CRC_EN
		//CRC calculation

		memcpy(g_readCRC.data, &data_rcv[SPI_DATA16_CRC_START_INDEX], SPI_CRC_SIZE); //crc data copy
		g_readCRC.ad_crc = Metro_Reverse_bytes_16(g_readCRC.ad_crc);
		if (SPI_CRC_Check(SPI_DATA16_SIZE, data_rcv) != ESP_OK)
		{
			ret = METRO_SPI_ERR;
			printf("CRC error\n");
		}
#endif
	}

	return ret;
}


/**
 * @brief      This function read 32 bit data from ADE9030 via SPI
 * @param[in]  uint16_t us_ADE_Addr : address of the register
 * @retval     metro_err_t err type
*/
metro_err_t SPI_Read_ADE9039_Reg_32(uint16_t  us_ADE_Addr, uint32_t* spi_read_data)
{
	metro_err_t ret = METRO_SUCCESS;
	uint16_t us_iAddress;
	uint8_t data_snd[SPI_TX_BUF_SIZE];
	uint8_t data_rcv[SPI_RX_BUF_SIZE];
	us_iAddress = us_ADE_Addr;
	spi_read_data_union spiReadData;

	memset(data_rcv, 0 , sizeof(data_rcv));
	memset(data_snd, 0 , sizeof(data_snd));

		//Separate address from SPI data

	us_iAddress = (us_iAddress << 4);
	us_iAddress = (us_iAddress | 0x08);

	data_snd[0] = (us_iAddress & 0xff00)>>8;
	data_snd[1] = us_iAddress & 0x00ff;

	if (SPI_Comp_SendAndReceivePacket_32(data_snd, data_rcv) != ESP_OK)
	{
		if (SPI_Comp_SendAndReceivePacket_32(data_snd, data_rcv) != ESP_OK)
		{
			ret = METRO_SPI_ERR;	//SPI ERROR
		}
	}
	else
	{
		memcpy(spiReadData.temp, &data_rcv[SPI_DATA_START_INDEX], SPI_DATA32_SIZE); //reg data copy
		*spi_read_data = Metro_Reverse_bytes_32(spiReadData.pm_data_32);

#ifdef METRO_SPI_CRC_EN

		memcpy(g_readCRC.data, &data_rcv[SPI_DATA32_CRC_START_INDEX], SPI_CRC_SIZE); //crc data copy
		g_readCRC.ad_crc = Metro_Reverse_bytes_16(g_readCRC.ad_crc);
		if(SPI_CRC_Check(SPI_DATA32_SIZE, data_rcv) != ESP_OK) // Assuming 2 bytes
		{
			ret = METRO_SPI_ERR;
		}
#endif
	}

	return ret;
}


/**
 * @brief      This function read burst from ADE9030 via SPI from starting address
 * @param[in]  uint16_t us_ADE_Addr : address of the register
 * @retval     metro_err_t error type
*/
metro_err_t SPI_Burst_Read(uint16_t  us_ADE_Addr)
{
	metro_err_t ret = METRO_SUCCESS;
	uint16_t us_iAddress;

	uint8_t rx_buffer[MAX_BURST_READ_DATA_BYTES] = {0};
	uint8_t data_snd[SPI_TX_BUF_SIZE];
	uint8_t data_rcv[SPI_RX_BUF_SIZE];
	us_iAddress = us_ADE_Addr;

	memset(data_rcv, 0, sizeof(data_rcv));

		//Separate address from SPI data

	us_iAddress = (us_iAddress << 4);
	us_iAddress = (us_iAddress | 0x08);

	data_snd[0] = (us_iAddress & 0xff00)>>8;
	data_snd[1] = us_iAddress & 0x00ff;
	data_snd[2] = 0;
	data_snd[3] = 0;
	data_snd[4] = 0;
	data_snd[5] = 0;

	if (SPI_Comp_SendAndReceivePacket_burst(data_snd, MAX_BURST_READ_DATA_BYTES, rx_buffer) != ESP_OK)
	{
		if (SPI_Comp_SendAndReceivePacket_burst(data_snd, MAX_BURST_READ_DATA_BYTES, rx_buffer) != ESP_OK)
		{
			ret = METRO_SPI_ERR;	//SPI ERROR
		}
	}
	else
	{
		memcpy(g_metroWaveformData.spi_read_data, rx_buffer, MAX_BURST_READ_DATA_BYTES);
	}

	return ret;

}


/**
 * @brief      This function initialize the spi
 * @retval     metro_err_t error type
*/
metro_err_t SPI_Init(void)
{
	metro_err_t ret = METRO_SUCCESS;

	//SPI bus configuration setting

	spi_bus_config_t buscfg={
		.miso_io_num = BOARD_PIN_MISO,		//Assign MISO pin
		.mosi_io_num = BOARD_PIN_MOSI,		//Assign MOSI pin
		.sclk_io_num = BOARD_PIN_SCLK,		//Assign SCLK pin
		.quadwp_io_num = SIGNAL_NOT_USED,		//Write protected signal - Not used
		.quadhd_io_num = SIGNAL_NOT_USED,		//Hold signal - Not used
		.max_transfer_sz = 10,
	};

		//Configure SPI for slave interfacing

	spi_device_interface_config_t devcfg={
		.clock_speed_hz = SPI_CLK_FREQ,		//Clock out at 10 MHz
		.mode = SPI_DMA_CH_AUTO,		//SPI DMA mode
		.spics_io_num = BOARD_PIN_CS,		//Assign CS pin
		.queue_size = SPI_QUEUE_SIZE,
		.input_delay_ns = SPI_INPUT_DELAY_MS,

	#if 0
		.command_bits = SPI_CMD_BITS,
		.address_bits = SPI_ADDRESS_BITS,
		.dummy_bits = SPI_DUMMY_BITS,
		.input_delay_ns = SPI_INPUT_DELAY,
		.pre_cb = SPI_AD_En_CS,
		.post_cb = AD_Ds_CS,
	#endif
	};

		// SPI_DMA_ENABLED

	if (spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_CH_AUTO) != ESP_OK)
	{
		if (spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_CH_AUTO) != ESP_OK)
		{
			ret = METRO_SPI_ERR;	//SPI ERROR
		}
	}
	else
	{

			//Attach the AD to the SPI bus

		if (spi_bus_add_device(HSPI_HOST, &devcfg, &spi) != ESP_OK)
		{
			if (spi_bus_add_device(HSPI_HOST, &devcfg, &spi) != ESP_OK)
			{
				ret = METRO_SPI_ERR;	//SPI ERROR
			}
		}
	}

	return ret;
}


/**
 * @brief      This function set chip select to low for high to low transaction
 * @param[in]  spi_transaction_t *t , passes pointer to locate spi transaction structure
 * @retval     None
*/
void SPI_AD_En_CS(spi_transaction_t *t)
{
	gpio_set_level(BOARD_PIN_CS, 1);
}


/**
 * @brief      This function set chip select to high for high to low transaction
 * @param[in]  spi_transaction_t *t , passes pointer to locate spi transaction structure
 * @retval     None
*/
void SPI_AD_Ds_CS(spi_transaction_t *t)
{
	gpio_set_level(BOARD_PIN_CS, 0);
}


#ifdef METRO_SPI_CRC_EN
/**
 * @brief      This function verifies 16 bit CRC of esp32 and ad chipset
 * @param[in]  total no of bytes for which CRC need to calculate, uint8_t data_rcv[SPI_RX_BUF_SIZE]
 * @retval     uint16_t calculated CRC
*/
metro_err_t SPI_CRC_Check(uint8_t total_bytes, uint8_t data_rcv[SPI_RX_BUF_SIZE])
{
	metro_err_t ret = METRO_SUCCESS;
	uint16_t esp32_crc = 0;

	esp32_crc = SPI_CRC_CCITT16_Cal32(data_rcv, total_bytes);

	if(g_readCRC.ad_crc != esp32_crc)
	{
		ret = METRO_SPI_ERR;
		printf("SPI data Corrupted\n");
	}

	return ret;
}


/**
 * @brief      This function calculates 16 bit CRC using CCITT16 algorithm
 * @param[in]  total no of bytes for which CRC need to calculate
 * @retval     uint16_t calculated CRC
*/
uint16_t SPI_CRC_CCITT16_Cal32(uint8_t data_rcv[SPI_RX_BUF_SIZE], uint8_t total_bytes)
{
	uint16_t  calculated_CRC = 0xffff;
	uint8_t i = 0, j = 0;

		//unsigned short CRC = CRC_PRESET;

	for (i = 2; i < (total_bytes + 2); i++)
	{
		unsigned int xr = data_rcv[i] << 8;
		calculated_CRC = calculated_CRC ^ xr;

		for (j = 0; j < 8; j++)
		{
			if (calculated_CRC & 0x8000)
			{
				calculated_CRC = (calculated_CRC << 1);
				calculated_CRC = calculated_CRC ^ CRC_POLYNOM;
			}
			else
			{
				calculated_CRC = calculated_CRC << 1;
			}
		} //end for
	}//end for
	calculated_CRC = calculated_CRC & 0xFFFF;

	return calculated_CRC;
}

#endif
