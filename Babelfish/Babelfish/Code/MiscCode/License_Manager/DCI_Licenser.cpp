/**
 *****************************************************************************************
 *	@file DCI_Licenser.cpp
 *
 *	@brief A module capable of doing license management based on token received.
 *
 *	@copyright 2021 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "Babelfish_Assert.h"
#include "DCI_Owner.h"
#include "DCI_Licenser.h"
#include "License_Parser.h"
#include "Licenser_Config.h"

namespace BF_Misc
{

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_Licenser::DCI_Licenser( const feature_info_t* feature_info_list, const Enabler::fid_t total_features,
							License_Parser* parser, Gate* gate_handle ) :
	m_feature_info_list( feature_info_list ),
	m_total_features( total_features ),
	m_parser( parser ),
	m_license_list( nullptr ),
	m_tracker_timer( nullptr ),
	m_gate_handle( gate_handle )
{
	BF_ASSERT( m_feature_info_list );

	DCI_Owner* dci_owner = nullptr;
	uint8_t token_buff[MAX_TOKEN_SIZE] = {0};
	DCI_LENGTH_TD token_length = 0;

	m_license_list = new License_Parser::license_data_t[m_total_features];
	BF_ASSERT( m_license_list );

	for ( Enabler::fid_t i = 0; i < m_total_features; i++ )
	{
		dci_owner = new DCI_Owner( m_feature_info_list[i].dcid );

		// Only ram value is allowed to be changed by patrons.
		dci_owner->Attach_Callback( &License_Update_Callback_Static,
									reinterpret_cast<DCI_CBACK_PARAM_TD>( this ),
									( DCI_ACCESS_SET_RAM_CMD_MSK | DCI_ACCESS_POST_SET_RAM_CMD_MSK ) );

		token_length = dci_owner->Get_Length();

		/* Error : Token length is greater than receive buffer size i.e. MAX_TOKEN_SIZE
		 * Please check Licenser_Config.h file for MAX_TOKEN_SIZE definition */
		BF_ASSERT( token_length <= MAX_TOKEN_SIZE );

		/* Let's fetch tokens from dci */
		dci_owner->Check_Out( token_buff );

		/* Verify token */
		if ( License_Parser::VALID == m_parser->Verify( token_buff, token_length ) )
		{
			/* Update license by parsing a token */
			if ( false == m_parser->Parse( token_buff,
										   token_length,
										   &m_license_list[i] ) )
			{
				// TODO:What to do if parsing get failed
				BF_ASSERT( false );
			}
		}
		else
		{
			/* Mark license flag for invalid token */
			m_license_list[i].flag = License_Parser::NOT_AVAILABLE;
		}
	}

	m_tracker_timer = new BF_Lib::Timers( License_Tracker_Callback_Static, this );
	BF_ASSERT( m_tracker_timer );

	m_tracker_timer->Start( LICENSE_TRACKING_TIMEOUT_MS, TRUE );
	// coverity[leaked_storage]
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_Licenser::~DCI_Licenser( void )
{
	delete[] m_license_list;
	delete m_tracker_timer;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
License_Parser::license_data_t* DCI_Licenser::Get_License( DCI_ID_TD dcid )
{
	License_Parser::license_data_t* license = nullptr;
	Enabler::fid_t index = 0;
	bool loop_break = false;

	while ( ( index < m_total_features ) && ( loop_break == false ) )
	{
		if ( dcid == m_feature_info_list[index].dcid )
		{
			license = &m_license_list[index];
			loop_break = true;
		}
		index++;
	}
	return ( license );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
DCI_CB_RET_TD DCI_Licenser::License_Update_Callback( DCI_ACCESS_ST_TD* access_struct )
{
	DCI_CB_RET_TD return_status = DCI_CBACK_RET_PROCESS_NORMALLY;
	BF_Misc::License_Parser::license_t token = nullptr;
	BF_Misc::License_Parser::license_length_t token_length = 0;

	switch ( access_struct->command )
	{
		case DCI_ACCESS_SET_RAM_CMD:
			token = access_struct->data_access.data;
			token_length = access_struct->data_access.length;

			if ( License_Parser::VALID != m_parser->Verify( token, token_length ) )
			{
				return_status = DCI_CBACK_RET_NO_FURTHER_PROCESSING;
			}
			break;

		case DCI_ACCESS_POST_SET_RAM_CMD:
			token = access_struct->data_access.data;
			token_length = access_struct->data_access.length;

			if ( false == m_parser->Parse( token,
										   token_length,
										   Get_License( access_struct->data_id ) ) )
			{
				// TODO: What to do if parsing failed
				BF_ASSERT( false );
			}
			break;

		default:
			break;
	}

	return ( return_status );
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Licenser::License_Tracker_Callback( void )
{
	/* Periodic call at rate of LICENSE_TRACKING_TIMEOUT_MS */
	Enabler::fid_t index = 0;
	License_Parser::license_data_t* license = nullptr;
	feature_info_t* feature = nullptr;


	/* In one iteration, FID's which have received new token will be tracked */
	while ( index < m_total_features )
	{
		license = &m_license_list[index];
		feature = const_cast<feature_info_t*>( &m_feature_info_list[index] );

		if ( license->flag == License_Parser::AVAILABLE )
		{
			/* Here each FID can have different license model */
			switch ( license->model_type )
			{
				case License_Parser::NONE:
					Model_None_Handler( feature, license );
					break;

				case License_Parser::FREE:
					Model_Free_Handler( feature, license );
					break;

				case License_Parser::EXPIRY:
					Model_Expiry_Handler( feature, license );
					break;

				case License_Parser::METERED:
					Model_Metered_Handler( feature, license );
					break;

				default:
					BF_ASSERT( false );	///< We should never reach here
					break;
			}
		}
		index++;
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Licenser::Model_None_Handler( feature_info_t* feature, License_Parser::license_data_t* license )
{
	/* As this is License type = NONE, We should ignore model field */

	if ( license->command == License_Parser::ALLOW )
	{
		if ( m_gate_handle->Is_Enabled( feature->feature_id ) != Gate::ENABLED )
		{
			m_gate_handle->Enable( feature->feature_id );
		}
	}
	else if ( license->command == License_Parser::DISALLOW )
	{
		if ( m_gate_handle->Is_Enabled( feature->feature_id ) != Gate::DISABLED )
		{
			m_gate_handle->Disable( feature->feature_id );
		}
	}
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Licenser::Model_Free_Handler( feature_info_t* feature,
									   License_Parser::license_data_t* license )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Licenser::Model_Expiry_Handler( feature_info_t* feature,
										 License_Parser::license_data_t* license )
{}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void DCI_Licenser::Model_Metered_Handler( feature_info_t* feature,
										  License_Parser::license_data_t* license )
{}

}	// End of namespace BF_Misc
