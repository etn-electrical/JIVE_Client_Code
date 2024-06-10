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
#ifndef PROFIBUS_PROD_CONS_H
	#define PROFIBUS_PROD_CONS_H

#include "Change_Tracker.h"
#include "DCI.h"
#include "DCI_Owner.h"
#include "Bit_List.h"
#include "ProfiBus_DCI.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define MAX_PROD_MODULES            28
#define MAX_CONS_MODULES            4
#define MAX_PROD_SIZE               71
#define MAX_CONS_SIZE               6
#define MAX_TOTAL_MODULES           ( MAX_PROD_MODULES + MAX_CONS_MODULES )
#define MAX_DATA_LEN                ( MAX_PROD_SIZE + MAX_CONS_SIZE )


#define SPECIAL_FORMAT_CFG_LENGTH       4

#define MAX_CK_CFG_LEN              ( MAX_TOTAL_MODULES * SPECIAL_FORMAT_CFG_LENGTH )
#define IO_MOD_START_ID             DCI_RUN_TIME_PROFIBUS_MOD
#define IO_MOD_END_ID               DCI_EMPTY_PAD_BYTE_PROFIBUS_MOD

enum
{
	SF_HEADER_BYTE,
	SF_LENGTH_BYTE,
	SF_MSB_PROFI_ID_BYTE,
	SF_LSB_PROFI_ID_BYTE
};

#define     CONSISTENCY_WHOLE_LENGTH        ( ( uint8_t )0x80 )

enum
{
	CFG_OK,
	CFG_NOT_SUPPORTED_ERROR,
	CFG_MAX_IO_MODULE_ERROR,
	CFG_MAX_IO_SIZE_ERROR,
	CFG_IO_ASSEMBLY_SET_LENGTH_ERROR
};


enum
{
	OUTPUT,
	INPUT
};

enum
{
	EVEN,
	ODD
};

/*
 *****************************************************************************************
 *		Structures
 *****************************************************************************************
 */
typedef struct
{
	PROFI_ID_TD profi_id;
	DCI_LENGTH_TD data_size;
	uint8_t mod_type;
} MODULE_STRUCT;


/*
 *****************************************************************************************
 *		ProfiBus_Prod_Cons   Class
 *****************************************************************************************
 */

class ProfiBus_Prod_Cons
{
	public:
		ProfiBus_Prod_Cons( DCI_ID_TD prod_list_id, DCI_ID_TD cons_list_id,
							DCI_ID_TD prod_data_id, ProfiBus_DCI* profibus_dci_ctrl );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~ProfiBus_Prod_Cons( void ) {}



		uint8_t Check_New_Config_Data( uint8_t* cfg_buffer_ptr, uint8_t* rd_cfg_buffer_ptr,
									   uint8_t cfg_data_len, uint8_t rd_cfg_data_len );

		uint8_t Build_IO_Assemblies( uint8_t* cfg_ptr, uint8_t cfg_len );

		void Start_Prod_Scan( BF_Lib::Timers::TIMERS_TIME_TD refresh_rate );

		void Stop_Prod_Scan();

		void Refresh_Prod();

		DCI_ERROR_TD Copy_Prod_Data( uint8_t* dest_data, DCI_LENGTH_TD max_buffer_length );

		DCI_ERROR_TD Copy_Cons_Data( uint8_t* src_data );

		void Clear_MI_Run( void );

		uint8_t Get_Module_Data( uint8_t* cfg_ptr, MODULE_STRUCT* module_ptr );

		bool Check_Module_Data_Length( MODULE_STRUCT* module_ptr );

	private:
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		static void Refresh_Prod_Static( BF_Lib::Timers::TIMERS_CBACK_PARAM_TD handle )
		{ ( ( ProfiBus_Prod_Cons* )handle )->Refresh_Prod(); }


		uint8_t* m_prod_data_ptr;
		Timers* m_prod_timer;
		DCI_Owner* m_prod_list_owner;
		DCI_Owner* m_cons_list_owner;
		DCI_Owner* m_prod_data_owner;
		DCI_Owner* m_empty_pad_byte_owner;
		uint8_t m_total_prod_modules;
		uint8_t m_total_cons_modules;
		bool m_prod_data_change;

		ProfiBus_DCI* m_profibus_dci_ctrl;
};

#endif	/*PROFIBUS_PROD_CONS_H_*/
