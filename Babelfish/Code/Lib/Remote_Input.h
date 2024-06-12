/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@version
 *	C++ Style Guide Version 1.0
 *
 *	@copyright 2013 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef REMOTE_INPUT_H_
#define REMOTE_INPUT_H_

#include "Includes.h"
#include "Input.h"

class Remote_Input : public BF_Lib::Input
{
	public:

		Remote_Input( bool initial_state = false );
		virtual ~Remote_Input();

		bool Get( void );

		bool Get_Raw( void );

		void Set( bool state );

	private:
		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 * definitions made private to disallow usage.
		 */
		Remote_Input( const Remote_Input& object );
		Remote_Input & operator =( const Remote_Input& object );

		bool m_state;

};

#endif	/* REMOTE_INPUT_H_ */
