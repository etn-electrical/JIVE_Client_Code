/****************************************************************************
 *
 *                      Eaton Wiring Devices
 *
 ****************************************************************************
 * Description      : IoT Platform Smart Breaker 2.0
 *
 * File name        : i2c_comp.h
 *
 * Author           : Kothe, AARTI J
 *
 * Last Changed By  : $Kothe, AARTI J $
 * Revision         : $Revision: 1.0 $
 * Last Changed     : $Date: 02/09/2024
 *
 ****************************************************************************/
#ifndef I2C_COMP_H
#define I2C_COMP_H

#include "driver/i2c.h"
#include "eeram_comp.h"
/*******************************************************************************
* CONSTANTS & MACROS:
*******************************************************************************/
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          1000000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000
#define DEVICE_ADDRESS_CONTROL_REG 0x18

#define I2C_DEVICE_ADDRESS 0x50  // I2C device address
#define I2C_MAX_EERAM_BYTE_SIZE 0x1FF
#define I2C_STATUS_REG_ADD 0x0000
#define I2C_STATUS_REG_VAL 0x02
#define I2C_DEVICE_ADDRESS_CONTROL_REG 0x18

#define RESET_VALUE 0x00000000
/*******************************************************************************
* FUNCTIONS:
*******************************************************************************/
eeram_err_t I2C_Master_Init(void);

#endif /* I2C_COMP_H */
