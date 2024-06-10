/**
 *****************************************************************************************
 *	@file
 *	@details See header file for module overview.
 *	@copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Image_Header.h"

namespace BF_FUS
{

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Image_Header::Image_Header( void )
	: m_comp_num( 0 )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Image_Header::Read_Header( uint8_t* buff, uint16_t* buff_size, NV_Ctrl* nv_ctrl )
{
	return ( FUS_NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Image_Header::Magic_Endian( uint8_t* buff_size, uint16_t* buff_length, NV_Ctrl* nv_ctrl )
{
	return ( FUS_NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Image_Header::Msg_For_Boot_Nvadd( uint8_t* buff_size, uint16_t* buff_length, NV_Ctrl* nv_ctrl )
{
	return ( FUS_NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Image_Header::Header_Format_Version( uint8_t* buff_size, uint16_t* buff_length, NV_Ctrl* nv_ctrl )
{
	return ( FUS_NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Image_Header::Image_Id( uint8_t* buff_size, uint16_t* buff_length, NV_Ctrl* nv_ctrl )
{
	return ( FUS_NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Image_Header::Version_Info( uint8_t* buff_size, uint16_t* buff_length, NV_Ctrl* nv_ctrl )
{
	return ( FUS_NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Image_Header::Product_Code( uint8_t* buff_size, uint16_t* buff_length, NV_Ctrl* nv_ctrl )
{
	return ( FUS_NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Image_Header::Compatibility_Num( uint8_t* buff_size, uint16_t* buff_length, NV_Ctrl* nv_ctrl )
{
	return ( FUS_NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Image_Header::Crc( uint8_t* buff_size, uint16_t* buff_length, NV_Ctrl* nv_ctrl )
{
	return ( FUS_NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Image_Header::Update_Compatibility_Num( uint16_t comp_num )
{
	m_comp_num = comp_num;
	return ( FUS_NO_ERROR );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
fus_op_status_t Image_Header::Get_Compatibility_Num( uint16_t* comp_num )
{
	*comp_num = m_comp_num;
	return ( FUS_NO_ERROR );
}

}

