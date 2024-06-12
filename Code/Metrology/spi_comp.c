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
 * Last Changed By  : $Author: tia $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 07/18/2022
 *
 ****************************************************************************/
/****************************************************************************
*                           	Include files
****************************************************************************/
#include "spi_comp.h"
/****************************************************************************
*                              Global variables
****************************************************************************/
static const char* TAG = "SPI_Interface";
spi_device_handle_t spi;
crc_union g_readCRC;
extern metro_waveform_burst_data g_metroWaveformData;

/****************************************************************************
*                    Local function Definitions
****************************************************************************/
/**
 * @brief      This function Send-receive 16 bit data
 * @param[in]  *data_snd, passes pointer to locate array to send data
 * @retval     int8_t err type
*/
uint8_t SPI_Comp_SendAndReceivePacket_16(uint8_t * data_snd, uint8_t * data_rcv)
{
	uint8_t ret = METRO_SUCCESS;
	DMA_ATTR static uint8_t tx_buffer[SPI_PACKET_16_BUF_SIZE];
	DMA_ATTR static uint8_t rx_buffer[SPI_PACKET_16_BUF_SIZE_CRC];

	memcpy(tx_buffer, data_snd, SPI_PACKET_16_BUF_SIZE);	//Load Tx buffer with 16 bit data

		//Create packet

	spi_transaction_t temp_trans = {
				.length = BYTE * SPI_PACKET_16_BUF_SIZE_CRC,
				.tx_buffer = tx_buffer,
				.rx_buffer = rx_buffer,
		};

		//Check SPI read success

	//Check SPI read success

	if ((ret = spi_device_polling_transmit(spi, &temp_trans)) != METRO_SUCCESS)
	{
		if ((ret = spi_device_polling_transmit(spi, &temp_trans)) != METRO_SUCCESS)
		{
			ret = METRO_SPI_ERR;
		}
	}

		//Load Rx buffer with 16 bit data

#ifdef METRO_SPI_CRC_EN
	if (ret != METRO_SPI_ERR)
	{
		memcpy(data_rcv, rx_buffer, SPI_PACKET_16_BUF_SIZE_CRC);
	}
#else
	if (ret != METRO_SPI_ERR)
	{
		memcpy(data_rcv, rx_buffer, SPI_PACKET_16_BUF_SIZE);
	}
#endif

	return ret;
}


/**
 * @brief      This function Send-receive 32 bit data
 * @param[in]  *data_snd, passes pointer to locate array to send data
 * @retval     int8_t err type
*/
uint8_t SPI_Comp_SendAndReceivePacket_32(uint8_t * data_snd, uint8_t * data_rcv)
{
	DMA_ATTR static uint8_t tx_buffer[SPI_PACKET_32_BUF_SIZE];
	DMA_ATTR static uint8_t rx_buffer[SPI_PACKET_32_BUF_SIZE_CRC];
	uint8_t ret = METRO_SUCCESS;

	memcpy(tx_buffer, data_snd, SPI_PACKET_32_BUF_SIZE);	//Load Tx buffer with 32 bit data

		//Create packet

	spi_transaction_t temp_trans = {
				.length = BYTE * SPI_PACKET_32_BUF_SIZE_CRC,
				.tx_buffer = tx_buffer,
				.rx_buffer = rx_buffer,
		};

	if ((ret = spi_device_polling_transmit(spi, &temp_trans)) != METRO_SUCCESS)
	{
		if ((ret = spi_device_polling_transmit(spi, &temp_trans)) != METRO_SUCCESS)
		{
			ret = METRO_SPI_ERR;
		}
	}

		//Load Rx buffer with 16 bit data

#ifdef METRO_SPI_CRC_EN
	if (ret != METRO_SPI_ERR)
	{
		memcpy(data_rcv, rx_buffer, SPI_PACKET_32_BUF_SIZE_CRC);
	}
#else
	if (ret != METRO_SPI_ERR)
	{
		memcpy(data_rcv, rx_buffer, SPI_PACKET_32_BUF_SIZE);
	}
#endif
	return ret;
}

/**
 * @brief      This function receive burst data
 * @param[in]  *data_snd, passes pointer to locate array to send data
 * @param[in]  total bytes, maximum no of bytes for burst read, uint8_t* address of buffer which hold received data
 * @retval     uint8_t error type
*/
uint8_t SPI_Comp_SendAndReceivePacket_burst(uint8_t * data_snd, uint16_t total_bytes, uint8_t *rx_buffer)
{
	uint8_t ret = METRO_SUCCESS;
	DMA_ATTR static uint8_t tx_buffer[BURST_READ_TX_BUF_SIZE];

	memcpy(tx_buffer, data_snd, BURST_READ_TX_BUF_SIZE);
	memset(rx_buffer, 0, total_bytes * sizeof(uint8_t));

	spi_transaction_t temp_trans = {
				.length = BYTE * total_bytes, // 32 bits
				.tx_buffer = tx_buffer,
				.rx_buffer = rx_buffer,
	};

	if ((ret = spi_device_polling_transmit(spi, &temp_trans)) != METRO_SUCCESS)
	{
		ret = METRO_SPI_ERR;
	}

	return ret;
}

/**
 * @brief      This function write 16 bit data to ADE9030 via SPI
 * @param[in]  uint16_t us_ADE_Addr : address of the register
 * @param[in]  uint16_t Reg_Data : data to write
 * @retval     int8_t Err type
*/
uint8_t Write_SPI_ADE9039_Reg_16(uint16_t us_ADE_Addr, uint16_t Reg_Data)
{
	uint16_t us_iAddress;
	us_iAddress = us_ADE_Addr;
	uint8_t data_snd[SPI_TX_BUF_SIZE];
	uint8_t data_rcv[SPI_RX_BUF_SIZE];
	uint8_t ret = METRO_SUCCESS;

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

	if ((ret = SPI_Comp_SendAndReceivePacket_16(data_snd, data_rcv)) != METRO_SUCCESS)
	{
		if ((ret = SPI_Comp_SendAndReceivePacket_16(data_snd, data_rcv)) != METRO_SUCCESS)
		{
			ret = METRO_SPI_ERR;
		}
	}

	return ret;
}

/**
 * @brief      This function write 32 bit data to ADE9030 via SPI
 * @param[in]  uint16_t us_ADE_Addr : address of the register
 * @param[in]  uint32_t Reg_Data : data to write
 * @retval     int8_t err type
*/
uint8_t Write_SPI_ADE9039_Reg_32(uint16_t us_ADE_Addr, uint32_t Reg_Data)
{
	uint16_t us_iAddress;
	uint8_t *rx_buffer;
	us_iAddress = us_ADE_Addr;
	uint8_t data_snd[SPI_TX_BUF_SIZE];
	uint8_t data_rcv[SPI_RX_BUF_SIZE];
	uint8_t ret = METRO_SUCCESS;

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

	if ((ret = SPI_Comp_SendAndReceivePacket_32(data_snd, data_rcv)) != METRO_SUCCESS)
	{
		if ((ret = SPI_Comp_SendAndReceivePacket_32(data_snd, data_rcv)) != METRO_SUCCESS)//remove
		{
			ret = METRO_SPI_ERR;
		}
	}

	return ret;
}


/**
 * @brief      This function read 16 bit data from ADE9030 via SPI
 * @param[in]  uint16_t us_ADE_Addr : address of the register
 * @retval     int8_t err type
*/
uint8_t Read_SPI_ADE9039_Reg_16(uint16_t  us_ADE_Addr, uint16_t* spi_read_data)
{
	uint16_t us_iAddress;
	uint16_t crc = 0;
	us_iAddress = us_ADE_Addr;
	uint8_t ret = METRO_SUCCESS;
	uint8_t data_snd[SPI_TX_BUF_SIZE];
	uint8_t data_rcv[SPI_RX_BUF_SIZE];
	spi_read_data_union spiReadData;

		//Separate address from SPI data

	us_iAddress = (us_iAddress << 4);
	us_iAddress = (us_iAddress | 0x08);

	data_snd[0] = (us_iAddress & 0xff00)>>8;
	data_snd[1] = us_iAddress & 0x00ff;
	data_snd[2] = 0;
	data_snd[3] = 0;
	data_snd[4] = 0;
	data_snd[5] = 0;
	data_rcv[0] = 0;
	data_rcv[1] = 0;
	data_rcv[2] = 0;
	data_rcv[3] = 0;
	data_rcv[4] = 0;
	data_rcv[5] = 0;

	if ((ret = SPI_Comp_SendAndReceivePacket_16(data_snd, data_rcv)) != METRO_SUCCESS)
	{
		if ((ret = SPI_Comp_SendAndReceivePacket_16(data_snd, data_rcv)) != METRO_SUCCESS)
		{
			ret = METRO_SPI_ERR;
		}
	}

	if (ret != METRO_SPI_ERR)
	{
		memcpy(spiReadData.temp, &data_rcv[SPI_PACKET_16_START_INDEX], SPI_DATA_BYTES_16);
		*spi_read_data = Metro_Reverse_bytes_16(spiReadData.pm_data_16);

#ifdef METRO_SPI_CRC_EN
		//CRC calculation

		memcpy(g_readCRC.data, &data_rcv[4], SPI_DATA_BYTES_16); //crc data copy
		g_readCRC.ad_crc = Metro_Reverse_bytes_16(g_readCRC.ad_crc);
		crc = CRC_Check(2); //no of bytes
#endif
	}

	return ret;
}


/**
 * @brief      This function read 32 bit data from ADE9030 via SPI
 * @param[in]  uint16_t us_ADE_Addr : address of the register
 * @retval     int8_t err type
*/
uint8_t Read_SPI_ADE9039_Reg_32(uint16_t  us_ADE_Addr, uint32_t* spi_read_data)
{
	uint16_t us_iAddress;
	uint16_t crc = 0;
	uint8_t data_snd[SPI_TX_BUF_SIZE];
	uint8_t data_rcv[SPI_RX_BUF_SIZE];
	us_iAddress = us_ADE_Addr;
	uint8_t ret = METRO_SUCCESS;
	spi_read_data_union spiReadData;

		//Separate address from SPI data

	us_iAddress = (us_iAddress << 4);
	us_iAddress = (us_iAddress | 0x08);

	data_snd[0] = (us_iAddress & 0xff00)>>8;
	data_snd[1] = us_iAddress & 0x00ff;
	data_snd[2] = 0;
	data_snd[3] = 0;
	data_snd[4] = 0;
	data_snd[5] = 0;
	data_rcv[0] = 0;
	data_rcv[1] = 0;
	data_rcv[2] = 0;
	data_rcv[3] = 0;
	data_rcv[4] = 0;
	data_rcv[5] = 0;

	if ((ret = SPI_Comp_SendAndReceivePacket_32(data_snd, data_rcv)) != METRO_SUCCESS)
	{
		if ((ret = SPI_Comp_SendAndReceivePacket_32(data_snd, data_rcv)) != METRO_SUCCESS)
		{
			ret = METRO_SPI_ERR;
		}
	}

	if (ret != METRO_SPI_ERR)
	{
		memcpy(spiReadData.temp, &data_rcv[SPI_PACKET_16_START_INDEX], SPI_PACKET_16_BUF_SIZE); //reg data copy
		*spi_read_data = Metro_Reverse_bytes_32(spiReadData.pm_data_32);

#ifdef METRO_SPI_CRC_EN
	memcpy(g_readCRC.data, &data_rcv[SPI_PACKET_16_BUF_SIZE_CRC], SPI_DATA_BYTES_16); //crc data copy
	read_crc.ad_crc = Metro_Reverse_bytes_16(read_crc.ad_crc);
	crc = CRC_Check(4);
#endif
	}

	return ret;
}


/**
 * @brief      This function read burst from ADE9030 via SPI from starting address
 * @param[in]  uint16_t us_ADE_Addr : address of the register
 * @retval     uint8_t error type
*/
uint8_t Burst_Read_SPI_ADE9039(uint16_t  us_ADE_Addr /*uint16_t *Reg_Data */,uint16_t total_bytes)
{
	uint8_t ret = METRO_SUCCESS;
	uint16_t us_iAddress;
	uint8_t *rx_buffer;
	uint8_t data_snd[SPI_TX_BUF_SIZE];
	uint8_t data_rcv[SPI_RX_BUF_SIZE];
	us_iAddress = us_ADE_Addr;

	rx_buffer = (uint8_t*)malloc(total_bytes * sizeof(uint8_t));

		//Separate address from SPI data

	us_iAddress = (us_iAddress << 4);
	us_iAddress = (us_iAddress | 0x08);

	data_snd[0] = (us_iAddress & 0xff00)>>8;
	data_snd[1] = us_iAddress & 0x00ff;
	data_snd[2] = 0;
	data_snd[3] = 0;
	data_snd[4] = 0;
	data_snd[5] = 0;

	if ((ret = SPI_Comp_SendAndReceivePacket_burst(data_snd, total_bytes, rx_buffer)) != METRO_SUCCESS)
	{
		if ((ret = SPI_Comp_SendAndReceivePacket_burst(data_snd, total_bytes, rx_buffer)) != METRO_SUCCESS)
		{
			ret = METRO_SPI_ERR;
		}
	}
	memcpy(g_metroWaveformData.spi_read_data, rx_buffer, total_bytes);
#if 0
	for(uint16_t x = 0; x< MAX_BURST_READ_DATA_BYTES; x++)
	{
		printf("%x   ", Metro_Waveform_data.spi_read_data[x]);
	}
#endif
	free(rx_buffer);
	rx_buffer = NULL;

	return ret;
}


/**
 * @brief      This function initialize the spi
 * @retval     int8_t error type
*/
uint8_t SPI_Init()
{
	uint8_t ret = METRO_SUCCESS;

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
		.mode = SPI_DMA_MODE,		//SPI DMA mode
		.spics_io_num = BOARD_PIN_CS,		//Assign CS pin
		.queue_size = SPI_QUEUE_SIZE,
		.input_delay_ns = SPI_INPUT_DELAY_MS,

	#if 0
		.command_bits = SPI_CMD_BITS,
		.address_bits = SPI_ADDRESS_BITS,
		.dummy_bits = SPI_DUMMY_BITS,
		.input_delay_ns = SPI_INPUT_DELAY,
		.pre_cb = AD_En_CS,
		.post_cb = AD_Ds_CS,
	#endif
	};

		// SPI_DMA_ENABLED

	if ((ret = spi_bus_initialize(AD_SPI_HOST, &buscfg, SPI_DMA_MODE)) != METRO_SUCCESS)
	{
		if ((ret = spi_bus_initialize(AD_SPI_HOST, &buscfg, SPI_DMA_MODE)) != METRO_SUCCESS)
		{
			ret = METRO_SPI_ERR;
		}
	}

	if (ret != METRO_SPI_ERR)
	{
		//Attach the AD to the SPI bus

		if ((ret = spi_bus_add_device(AD_SPI_HOST, &devcfg, &spi)) != METRO_SUCCESS)
		{
			if ((ret = spi_bus_add_device(AD_SPI_HOST, &devcfg, &spi)) != METRO_SUCCESS)
			{
				ret = METRO_SPI_ERR;
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
void AD_En_CS(spi_transaction_t *t)
{
	gpio_set_level(BOARD_PIN_CS, 1);
}


/**
 * @brief      This function set chip select to high for high to low transaction
 * @param[in]  spi_transaction_t *t , passes pointer to locate spi transaction structure
 * @retval     None
*/
void AD_Ds_CS(spi_transaction_t *t)
{
	gpio_set_level(BOARD_PIN_CS, 0);
}


#ifdef METRO_SPI_CRC_EN
/**
 * @brief      This function verifies 16 bit CRC of esp32 and ad chipset
 * @param[in]  total no of bytes for which CRC need to calculate
 * @retval     uint16_t calculated CRC
*/
uint16_t CRC_Check(uint8_t total_bytes)
{
	uint16_t esp32_crc = 0;
	esp32_crc = CRC_CCITT16_Cal32(total_bytes);
	if(g_readCRC.ad_crc != esp32_crc)
	{
		ESP_LOGI(TAG,"data corrupted\n");
		return 0;
	}
	else
	{
		ESP_LOGI(TAG,"data receive success\n");
		return esp32_crc;
	}
}


/**
 * @brief      This function calculates 16 bit CRC using CCITT16 algorithm
 * @param[in]  total no of bytes for which CRC need to calculate
 * @retval     uint16_t calculated CRC
*/
uint16_t CRC_CCITT16_Cal32(uint8_t total_bytes)
{
	uint16_t  crc = 0xffff;//0x1d0f;
	uint8_t i = 0, j = 0;
	uint8_t data_rcv[SPI_RX_BUF_SIZE];

		//unsigned short CRC = CRC_PRESET;

	for (i = 2; i < (total_bytes + 2); i++)
	{
		unsigned int xr = data_rcv[i] << 8;
		crc = crc ^ xr;

		for (j = 0; j < 8; j++)
		{
			if (crc & 0x8000)
			{
				crc = (crc << 1);
				crc = crc ^ CRC_POLYNOM;
			}
			else
			{
				crc = crc << 1;
			}
		} //end for
	}//end for
	crc = crc & 0xFFFF;
	return crc;

}

#endif
