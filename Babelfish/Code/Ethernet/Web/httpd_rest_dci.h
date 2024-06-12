/**
 *****************************************************************************************
 * @file
 * @details See header file for module overview.
 * @copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef __HTTPD_REST_DCI_H__
#define __HTTPD_REST_DCI_H__

#include "DCI_Patron.h"
#include "DCI_Owner.h"
#include "INT_ASCII_Conversion.h"
#include "string.h"
#include "DCI_Defines.h"
#include "Format_Handler.h"
#include "DCI_Constants.h"

uint8_t Specified_Allowed_Origin_Type( void );

typedef uint8_t rest_pid8_t;
typedef uint16_t REST_PARAM_ID_TD;
typedef uint16_t rest_pid16_t;

static const rest_pid16_t TOTAL_NUM_OF_CUSTOM_ASSEMBLIES = 10U;
static const rest_pid8_t MAX_NUM_OF_PARAMS_IN_CUSTOM_ASSY = 10U;

// DCI data type - string and length
static const char* const DCI_Datatype_str[DCI_DTYPE_MAX_TYPES] =
{
	"BOOL",
	"UINT8",
	"SINT8",
	"UINT16",
	"SINT16",
	"UINT32",
	"SINT32",
	"UINT64",
	"SINT64",
	"FLOAT",
	"LFLOAT",
	"DFLOAT",
	"BYTE",
	"WORD",
	"DWORD",
	"STRING"
};

static const uint8_t DCI_Datatype_length[DCI_DTYPE_MAX_TYPES] =
{
	4U,
	5U,
	5U,
	6U,
	6U,
	6U,
	6U,
	6U,
	6U,
	5U,
	6U,
	6U,
	4U,
	4U,
	5U,
	6U
};

typedef struct REST_PREDEFINED_ASSY_TD
{
	uint16_t assy_id;
	uint16_t number_of_parameters;
	const char_t* assy_name;
	uint8_t assy_name_len;
	REST_PARAM_ID_TD const* pParams;
	uint8_t access_r;
	uint8_t access_w;
} REST_PREDEFINED_ASSY_TD;

typedef struct REST_CUSTOM_ASSY_TD
{
	uint16_t assy_id;
	const char_t* assy_name;
	uint8_t assy_name_len;
	DCI_ID_TD validBitMapDCID;
	DCI_ID_TD paramListDCID;
	uint8_t access_r;
	uint8_t access_w;
} REST_CUSTOM_ASSY_TD;

struct enum_desc_t
{
	enum_val_t enum_val;
	char_t const* description;
};

struct enum_desc_block_t
{
	uint16_t total_enums;
	enum_desc_t* enum_list;
};

struct bitfield_desc_t
{
	bitfield_val_t bitfield_val;
	char_t const* description;
};

struct bitfield_desc_block_t
{
	bitfield_val_t total_bitfields;
	bitfield_desc_t* bitfield_list;
};

typedef struct DCI_REST_TO_DCID_LKUP_ST_TD
{
	REST_PARAM_ID_TD param_id;
	DCI_ID_TD dcid;
#ifndef REMOVE_REST_PARAM_NAME_TEXT
	const char_t* pName;
#endif
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
	const char_t* pDescription;
#endif
	const char_t* pUnits;
	const bitfield_desc_block_t* pBitfieldDes;
	const enum_desc_block_t* pEnumDes;
	const formatter_struct_t* pFormatData;
#ifndef REMOVE_REST_PARAM_NAME_TEXT
	uint8_t lenName;
#endif
	uint8_t lenUnits;
#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT
	uint16_t lenDescription;
#endif
	uint8_t access_r;
	uint8_t access_w;
} DCI_REST_TO_DCID_LKUP_ST_TD;
/* #k: Ori- Changed due to MISFICATION:
   typedef struct
   {
    uint8_t access_r;
    uint8_t access_w;
   } DCI_REST_BASELINE_ACCESS_LEVELS;
 */
typedef struct
{
	uint16_t access_r;
	uint16_t access_w;
} DCI_REST_BASELINE_ACCESS_LEVELS;


typedef struct REST_TARGET_INFO_ST_TD
{
	uint16_t number_of_parameters;
	DCI_REST_TO_DCID_LKUP_ST_TD const* param_list;
	DCI_REST_BASELINE_ACCESS_LEVELS baseline_access;
	uint16_t number_of_predefined_assemblies;
	REST_PREDEFINED_ASSY_TD const* predefined_assembly_list;
	uint16_t number_of_custom_assemblies;
	uint8_t max_number_of_paramters_in_custom_assembly;
	REST_CUSTOM_ASSY_TD const* custom_assembly_list;
} REST_TARGET_INFO_ST_TD;

typedef struct AUTH_STRUCT_TD
{
	const char_t* role_name;
	uint8_t role_level;
	const char_t* role_desc;
} AUTH_STRUCT_TD;


typedef struct DCI_USER_MGMT_DB_TD
{
	DCI_ID_TD full_name;
	DCI_ID_TD user_name;
	DCI_ID_TD password;
	DCI_ID_TD role;
	DCI_ID_TD pwd_set_epoch_time;
	DCI_ID_TD last_login_epoch_time;
	DCI_ID_TD pwd_complexity;
	DCI_ID_TD pwd_aging;
} DCI_USER_MGMT_DB_TD;

class httpd_rest_dci
{
	public:

		static void init_httpd_rest_dci();

		static const DCI_REST_TO_DCID_LKUP_ST_TD* findIndexREST( uint16_t param_id );

		static const DCI_REST_TO_DCID_LKUP_ST_TD* Find_Index_Rest_From_Dcid( DCI_ID_TD dcid );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static void setRestStruct( REST_TARGET_INFO_ST_TD const* rest_struct )
		{
			m_rest_struct = rest_struct;
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static REST_TARGET_INFO_ST_TD const* getRestStruct()
		{
			return ( m_rest_struct );
		}

		static uint32_t getParamAllSize( uint8_t req_access );

		static uint32_t getAssyAllSize( uint8_t req_access );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static uint16_t getNumberOfParameters()
		{
			return ( m_rest_struct->number_of_parameters );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static uint16_t getNumberofPredefinedAssemblies()
		{
			return ( m_rest_struct->number_of_predefined_assemblies );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static uint16_t getNumberofCustomAssemblies()
		{
			return ( m_rest_struct->number_of_custom_assemblies );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static uint16_t getTotalNumberOfAssemblies()
		{
			return ( m_rest_struct->number_of_predefined_assemblies + m_rest_struct->number_of_custom_assemblies );
		}

		static uint32_t getParamExtraSize( uint16_t index );

		static bool includedInParamAll( uint16_t index, uint8_t req_access );

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static uint16_t getParamId( uint16_t index )
		{
			return ( m_rest_struct->param_list[index].param_id );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static uint8_t getParamNameLen( uint16_t index )
		{
#ifdef REMOVE_REST_PARAM_NAME_TEXT
			return ( 0 );

#else
			return ( m_rest_struct->param_list[index].lenName );

#endif
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static const char_t* getParamName( uint16_t index )
		{
#ifdef REMOVE_REST_PARAM_NAME_TEXT
			return ( "" );

#else
			return ( m_rest_struct->param_list[index].pName );

#endif
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static uint16_t getPredefinedAssyNumber()
		{
			return ( m_rest_struct->number_of_predefined_assemblies );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static uint16_t getCustomAssyNumber()
		{
			return ( m_rest_struct->number_of_custom_assemblies );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static const REST_PREDEFINED_ASSY_TD* getPredefinedAssy( uint16_t index )
		{
			return ( &( m_rest_struct->predefined_assembly_list[index] ) );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static const REST_CUSTOM_ASSY_TD* getCustomAssy( uint16_t index )
		{
			return ( &( m_rest_struct->custom_assembly_list[index] ) );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static bool includedInPredefinedAssyAll( uint16_t index, uint8_t req_access )
		{
			if ( req_access < m_rest_struct->predefined_assembly_list[index].access_r )
			{
				return ( false );
			}
			return ( true );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static bool includedInCustomAssyAll( uint16_t index, uint8_t req_access )
		{
			if ( req_access < m_rest_struct->custom_assembly_list[index].access_r )
			{
				return ( false );
			}
			return ( true );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static uint32_t getPredefinedAssyExtraSize( uint16_t index )
		{
			return ( ( INT_ASCII_Conversion::UINT16_Number_Digits( m_rest_struct->predefined_assembly_list[index].
																   assy_id ) << 1 )
					 + m_rest_struct->predefined_assembly_list[index].assy_name_len
					 + STRLEN( "0" ) );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static uint32_t getCustomAssyExtraSize( uint16_t index )
		{
			return ( ( INT_ASCII_Conversion::UINT16_Number_Digits( m_rest_struct->custom_assembly_list[index].assy_id )
					   << 1 )
					 + m_rest_struct->custom_assembly_list[index].assy_name_len
					 + STRLEN( "0" ) );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static uint16_t getPredefinedAssyId( uint16_t index )
		{
			return ( m_rest_struct->predefined_assembly_list[index].assy_id );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static uint16_t getCustomAssyId( uint16_t index )
		{
			return ( m_rest_struct->custom_assembly_list[index].assy_id );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static const uint8_t* getPredefinedAssyName( uint16_t index )
		{
			return ( reinterpret_cast<uint8_t const*>( m_rest_struct->predefined_assembly_list[index].assy_name ) );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static const uint8_t* getCustomAssyName( uint16_t index )
		{
			return ( reinterpret_cast<uint8_t const*>( m_rest_struct->custom_assembly_list[index].assy_name ) );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static uint8_t getPredefinedAssyNameLen( uint16_t index )
		{
			return ( m_rest_struct->predefined_assembly_list[index].assy_name_len );
		}

		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		inline static uint8_t getCustomAssyNameLen( uint16_t index )
		{
			return ( m_rest_struct->custom_assembly_list[index].assy_name_len );
		}

	private:

		static REST_TARGET_INFO_ST_TD const* m_rest_struct;
};

extern const DCI_REST_TO_DCID_LKUP_ST_TD base_dci_rest_data_rest_to_dcid[];

#endif	/* __HTTPD_REST_DCI_H__ */
