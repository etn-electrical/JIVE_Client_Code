/**
 *****************************************************************************************
 *	@file            AtoD.cpp implementation file  for ADC functionality.
 *  @brief           This file shall wrap all the functions required for ADC functionality
 *	@details         See header file for module overview.
 *	@copyright       2014 Eaton Corporation. All Rights Reserved.
 *	@note            Eaton Corporation claims proprietary rights to the material disclosed
 *                   here in.  This technical information should not be reproduced or used
 *                   without direct written permission from Eaton Corporation.
 *****************************************************************************************
 */
#include "Includes.h"
#include "AtoD.h"
namespace BF_Misc
{

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
AtoD::AtoD( uC_AtoD* uc_atod_ctrl, uint8_t channel ) :
	m_channel( 0U ),
	m_uc_atod_ctrl( reinterpret_cast<uC_AtoD*>( nullptr ) )
{
	m_channel = channel;
	m_uc_atod_ctrl = uc_atod_ctrl;
}

}
