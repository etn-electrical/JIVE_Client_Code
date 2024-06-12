/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
void OpenAmp_Init( void );

void Virtual_UART_Init0( void );

void Virtual_UART_Init1( void );

void Virtual_UART_RegisterCallback0( void );

void Virtual_UART_RegisterCallback1( void );

void Check_Message( void );

void Error_Handler( void );

#define RPMSG_REMOTE    VIRTIO_DEV_SLAVE
#define VIRTIO_DEV_SLAVE    1UL
#define MAX_BUFFER_SIZE RPMSG_BUFFER_SIZE