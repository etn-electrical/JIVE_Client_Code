/*
 *****************************************************************************************
 *
 *		Copyright Eaton Corporation. All Rights Reserved.
 *		Creator: Mark A Verheyen
 *
 *		Description:
 *
 *
 *
 *****************************************************************************************
 */
#ifndef uC_Base_K60D10_H
   #define uC_Base_K60D10_H

#include "uC_Base.h"
#include "Defines.h"


/*
 **************************************************************************************************
 *  Constants
 **************************************************************************************************
 */
typedef enum BROWN_OUT_OPT_SELECT_EN// TODO: not sure if we need these
{
	BROWN_OUT_OPT_SEL_2p7,
	BROWN_OUT_OPT_SEL_2p4,
	BROWN_OUT_OPT_SEL_2p1,
	BROWN_OUT_OPT_SEL_1p8
} BROWN_OUT_SELECT_EN;
typedef enum WATCHDOG_OPT_SELECT_EN
{
	WATCHDOG_OPT_SEL_HW,
	WATCHDOG_OPT_SEL_SW
} WATCHDOG_OPT_SELECT_EN;


/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class uC_Base_K60D10 : public uC_Base
{
	public:
		uC_Base_K60D10( void );
		~uC_Base_K60D10( void );

		uC_BASE_WDOG_CTRL_STRUCT const* Get_WDOG_Ctrl( UINT8 block );

		uC_BASE_PIO_CTRL_STRUCT const* Get_PIO_Ctrl( UINT32 io_block );

		uC_BASE_TIMER_IO_STRUCT const* Get_Timer_Ctrl( UINT32 timer );

		uC_BASE_LP_TIMER_CTRL_STRUCT const* Get_LP_Timer_Ctrl( UINT32 timer );

		uC_BASE_USART_IO_STRUCT const* Get_USART_Ctrl( UINT32 port );

		uC_BASE_SPI_IO_STRUCT const* Get_SPI_Ctrl( UINT32 spi_port );

		// uC_BASE_I2C_IO_STRUCT const*		    Get_I2C_Ctrl( UINT32 i2c_port );
		uC_BASE_EXT_INT_IO_STRUCT const* Get_Ext_Int_Ctrl( uC_USER_IO_STRUCT const* pio_ctrl );

		// uC_BASE_TIMER_IO_STRUCT const*		Get_PWM_Ctrl( UINT32 pwm_block );
		// uC_BASE_ATOD_IO_STRUCT const*		    Get_AtoD_Ctrl( UINT32 atod_block );
		uC_BASE_DMA_IO_STRUCT const* Get_DMA_Ctrl( UINT32 dma_channel );

		uC_BASE_EMAC_IO_STRUCT const* Get_EMAC_Ctrl( UINT8 emac_block );

		UINT32 Get_PClock_Freq( UINT8 pclock_selection );								// TODO: not sure if we need
																						// this

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Get_Device_Sig( UINT8* data, UINT8 length = 12 )
		{
			// MB //UINT8* device_sig_ptr;  device_sig_ptr = (UINT8*)0x1FFF7A10;
			// for ( MUINT8 i = 0; ( ( i < length ) && ( i < 12 ) ); i++ ) { data[i] = device_sig_ptr[i]; }
		}

		// static BOOL Set_Option_Bytes( BROWN_OUT_OPT_SELECT_EN brown_out_sel = BROWN_OUT_OPT_SEL_2p7,
		// WATCHDOG_OPT_SELECT_EN watchdog_sel = WATCHDOG_OPT_SEL_SW );

		static const uC_BASE_PIO_CTRL_STRUCT m_pio_ctrls[uC_BASE_MAX_PIO_CTRLS];
		static const uC_BASE_TIMER_IO_STRUCT m_timer_io[uC_BASE_MAX_TIMER_CTRLS];
		static const uC_BASE_LP_TIMER_CTRL_STRUCT m_lp_timer_ctrls[uC_BASE_MAX_LP_TIMER_CTRLS];
		static const uC_BASE_USART_IO_STRUCT m_usart_io[uC_BASE_MAX_USART_PORTS];
		static const uC_BASE_SPI_IO_STRUCT m_spi_io[uC_BASE_SPI_MAX_CTRLS];
		// static const uC_BASE_I2C_IO_STRUCT		m_i2c_io[uC_BASE_I2C_MAX_CTRLS];
		static const uC_BASE_EXT_INT_IO_STRUCT m_ext_ints[uC_BASE_EXT_INT_MAX_PORTS];
		// static const uC_BASE_ATOD_IO_STRUCT		m_atod_io[uC_BASE_ATOD_MAX_CTRLS];
		static const uC_BASE_DMA_IO_STRUCT m_dma_io[uC_BASE_DMA_CHANNEL_MAX];
		static const uC_BASE_EMAC_IO_STRUCT m_emac_io[uC_BASE_EMAC_MAX_CTRLS];

		static const UINT32 m_pclock_freqs[uC_BASE_K60D10_MAX_PCLKS];

		/// I2C Interfaces.
		// Stores the function pointer of the objects static interrupt handler.
		// static uC_BASE_I2C_INT_OBJ_CBACK_FUNC* m_obj_func_i2c_dma_list_Tx[uC_BASE_I2C_MAX_CTRLS];
		// Stores the object pointer associated with the interrupt.
		// static uC_BASE_I2C_OBJ_PARAM_TD m_obj_i2c_dma_list_Tx[uC_BASE_I2C_MAX_CTRLS];

		// Stores the function pointer of the objects static interrupt handler.
		// static uC_BASE_I2C_INT_OBJ_CBACK_FUNC* m_obj_func_i2c_dma_list_Rx[uC_BASE_I2C_MAX_CTRLS];
		// Stores the object pointer associated with the interrupt.
		// static uC_BASE_I2C_OBJ_PARAM_TD m_obj_i2c_dma_list_Rx[uC_BASE_I2C_MAX_CTRLS];

		// Stores the function pointer of the objects static interrupt handler.
		// static uC_BASE_I2C_INT_OBJ_CBACK_FUNC* m_obj_func_i2c_list[uC_BASE_I2C_MAX_CTRLS];
		// Stores the object pointer associated with the interrupt.
		// static uC_BASE_I2C_OBJ_PARAM_TD m_obj_i2c_list[uC_BASE_I2C_MAX_CTRLS];


		// static void uC_BASE_I2C_DMA_INT1_Tx( void );
		// static void uC_BASE_I2C_DMA_INT2_Tx( void );
		// static void uC_BASE_I2C_DMA_INT3_Tx( void );

		// static void uC_BASE_I2C_DMA_INT1_Rx( void );
		// static void uC_BASE_I2C_DMA_INT2_Rx( void );
		// static void uC_BASE_I2C_DMA_INT3_Rx( void );


		// static void uC_BASE_I2C_INT1( void );
		// static void uC_BASE_I2C_INT2( void );
		// static void uC_BASE_I2C_INT3( void );
};


#endif
