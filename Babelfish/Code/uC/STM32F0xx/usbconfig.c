/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
uC_IO_Config::uC_IO_Config( void )
{
	uC_BASE_PIO_CTRL_STRUCT const* temp_pio;
	uC_USER_IO_STRUCT temp_user_io;
	uC_IO_INPUT_CFG_EN input_cfg;

	temp_pio = ( uC_Base::Self() )->Get_PIO_Ctrl( uC_BASE_PIO_A );
	temp_user_io.reg_ctrl = temp_pio->reg_ctrl;
	temp_user_io.port_index = uC_BASE_PIO_A;

	uC_Base::Reset_Periph( &temp_pio->periph_def );
	uC_Base::Enable_Periph_Clock( &temp_pio->periph_def );

	// for ( uint8_t j = 0U; j < 16U; j++ )
	{
		uint8_t j = 10;
		if ( Test_Bit( *temp_pio->set_input, j ) )
		{
			temp_user_io.pio_num = j;
			if ( Test_Bit( *temp_pio->input_pullup, j ) )
			{
				input_cfg = uC_IO_IN_CFG_PULLUP;
			}
			if ( Test_Bit( *temp_pio->input_pulldown, j ) )
			{
				input_cfg = uC_IO_IN_CFG_PULLDOWN;
			}
			else
			{
				input_cfg = uC_IO_IN_CFG_HIZ;
			}
			Set_Pin_To_Input( &temp_user_io, input_cfg );
		}
		else if ( Test_Bit( *temp_pio->set_output, j ) )
		{
			temp_user_io.pio_num = j;
			Set_Pin_To_Output( &temp_user_io, Test_Bit( *temp_pio->output_init, j ) );
		}
		else
		{
			// do nothing
		}
	}
}
