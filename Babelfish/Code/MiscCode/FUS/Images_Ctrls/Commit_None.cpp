/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2019 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */

#include "includes.h"
#include "Commit_None.h"


namespace BF_FUS
{


/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
Commit_None::Commit_None( void )
	: m_commit( nullptr ),
	m_commit_time( 0U ),
	m_state( IDLE )
{
	// TODO Auto-generated constructor stub

}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Commit_None::Image_Handler( fw_state_op_t* commit )
{
	switch ( m_state )
	{
		case IDLE:
			m_commit = commit;
			m_commit_time = BF_Lib::Timers::Get_Time();
			m_state = COMMIT;
			break;

		case COMMIT:
			Commit_State_Handler();
			break;

		case RETRY:
			break;

		case WAIT:
			break;

		default:
			break;
	}
	m_commit->wait_time = DEFAULT_FUS_WAIT_TIME_MS;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Commit_None::Reset( void )
{
	if ( m_commit != nullptr )
	{
		m_commit->status = FUS_NO_ERROR;
	}
	m_state = IDLE;
}

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
void Commit_None::Commit_State_Handler( void )
{
	uint32_t cur_time = BF_Lib::Timers::Get_Time();

	m_commit->status = FUS_PENDING_CALL_BACK;
	if ( cur_time > m_commit_time + DEFAULT_FUS_WAIT_TIME_MS )
	{
		m_commit->status = FUS_COMMIT_DONE;
		m_state = IDLE;
	}
}

}
