/**
 *****************************************************************************************
 *	@file uC_Base.cpp
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#include "uC_Base.h"
#include <stdio.h>

/*
 *****************************************************************************************
 *		Variables
 *****************************************************************************************
 */
uC_Base* uC_Base::m_myself = ( nullptr );

static portMUX_TYPE periph_spinlock = portMUX_INITIALIZER_UNLOCKED;
/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_Base::uC_Base( void )
{
	m_myself = this;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_PIO_CTRL_STRUCT const* uC_Base::Get_PIO_Ctrl( uint32_t io_block )
{
	uC_BASE_PIO_CTRL_STRUCT const* return_ctrl = nullptr;

	if ( io_block < uC_BASE_MAX_PIO_CTRLS )
	{
		return_ctrl = &m_pio_ctrls[io_block];
	}
	return ( return_ctrl );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uC_BASE_TIMER_IO_STRUCT const* uC_Base::Get_Timer_Ctrl( uint32_t timer )
{
	uC_BASE_TIMER_IO_STRUCT const* return_ctrl = nullptr;

	if ( timer < uC_BASE_MAX_TIMER_CTRLS )
	{
		return_ctrl = &m_timer_io[timer];
	}
	return ( return_ctrl );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
esp_err_t uC_Base::Periph_Module_Enable( periph_module_t periph )
{
	portENTER_CRITICAL( &periph_spinlock );
	DPORT_SET_PERI_REG_MASK( Get_Clk_En_Reg( periph ), Get_Clk_En_Mask( periph ) );
	DPORT_CLEAR_PERI_REG_MASK( Get_Rst_En_Reg( periph ), Get_Rst_En_Mask( periph ) );
	portEXIT_CRITICAL( &periph_spinlock );
	return ( ESP_OK );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Base::Get_Clk_En_Mask( periph_module_t periph )
{
	uint32_t clk_en_mask = PERIPH_DEFAULT_MASK;

	switch ( periph )
	{
		case PERIPH_UART0_MODULE:
			clk_en_mask = DPORT_UART_CLK_EN;
			break;

		case PERIPH_UART1_MODULE:
			clk_en_mask = DPORT_UART1_CLK_EN;
			break;

		case PERIPH_UART2_MODULE:
			clk_en_mask = DPORT_UART2_CLK_EN;
			break;

		case PERIPH_UHCI0_MODULE:
			clk_en_mask = DPORT_UHCI0_CLK_EN;
			break;

		case PERIPH_UHCI1_MODULE:
			clk_en_mask = DPORT_UHCI1_CLK_EN;
			break;

		case PERIPH_PWM0_MODULE:
			clk_en_mask = DPORT_PWM0_CLK_EN;
			break;

		case PERIPH_PWM1_MODULE:
			clk_en_mask = DPORT_PWM1_CLK_EN;
			break;
//Ahmed
#if CONFIG_IDF_TARGET_ESP32S3
		case PERIPH_PWM2_MODULE:
			clk_en_mask = DPORT_PWM2_CLK_EN;
			break;

		case PERIPH_PWM3_MODULE:
			clk_en_mask = DPORT_PWM3_CLK_EN;
			break;
#endif
		default:
			clk_en_mask = PERIPH_DEFAULT_MASK;
			break;
	}
	return ( clk_en_mask );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Base::Get_Rst_En_Mask( periph_module_t periph )
{
	uint32_t rst_en_mask = PERIPH_DEFAULT_MASK;

	switch ( periph )
	{
		case PERIPH_UART0_MODULE:
			rst_en_mask = DPORT_UART_RST;
			break;

		case PERIPH_UART1_MODULE:
			rst_en_mask = DPORT_UART1_RST;
			break;

		case PERIPH_UART2_MODULE:
			rst_en_mask = DPORT_UART2_RST;
			break;

		case PERIPH_UHCI0_MODULE:
			rst_en_mask = DPORT_UHCI0_RST;
			break;

		case PERIPH_UHCI1_MODULE:
			rst_en_mask = DPORT_UHCI1_RST;
			break;

		case PERIPH_PWM0_MODULE:
			rst_en_mask = DPORT_PWM0_RST;
			break;

		case PERIPH_PWM1_MODULE:
			rst_en_mask = DPORT_PWM1_RST;
			break;
			//Ahmed
			#if CONFIG_IDF_TARGET_ESP32S3
		case PERIPH_PWM2_MODULE:
			rst_en_mask = DPORT_PWM2_RST;
			break;

		case PERIPH_PWM3_MODULE:
			rst_en_mask = DPORT_PWM3_RST;
			break;
#endif
		default:
			rst_en_mask = PERIPH_DEFAULT_MASK;
			break;
	}
	return ( rst_en_mask );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Base::Get_Clk_En_Reg( periph_module_t periph )
{
	return ( DPORT_PERIP_CLK_EN_REG );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
uint32_t uC_Base::Get_Rst_En_Reg( periph_module_t periph )
{
	return ( DPORT_PERIP_RST_EN_REG );
}
