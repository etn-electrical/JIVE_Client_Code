/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Smart Breaker 2.0
 *
 * File name        : i2c_comp.c
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
#include <stdio.h>
#include "esp_log.h"
#include "i2c_comp.h"
#include "driver/gpio.h"
#include "../DeviceGPIO.h"
#include <math.h>

/****************************************************************************
 *                    Local function Definitions
 ****************************************************************************/
/**
 * @brief      This function is designed to initialize i2c as master
 * @param[in]  None
 * @retval     eeram_err_t i2c error
*/
eeram_err_t I2C_Master_Init(void)
{
	eeram_err_t ret = EERAM_SUCCESS;
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = BOARD_PIN_I2C_SDA,
        .scl_io_num = BOARD_PIN_I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    if (i2c_param_config(i2c_master_port, &conf) != ESP_OK)
    {
        if (i2c_param_config(i2c_master_port, &conf) != ESP_OK)
        {
        	ret = EERAM_I2C_ERR;	//I2C ERROR
        }
    }
    else
    {
        if (i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0) != ESP_OK)
        {
            if (i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0) != ESP_OK)
            {
            	ret = EERAM_I2C_ERR;	//I2C ERROR
            }
        }
    }

    return ret;
}



