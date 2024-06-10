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
#ifndef ETHERNETIP_DCI_H
	#define ETHERNETIP_DCI_H

#include "EtherNetIP_DCI_Defines.h"
#include "DCI_Patron.h"
#include "DCI_Owner.h"

/*
 *****************************************************************************************
 *		Control Defines.
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Typedefs
 *****************************************************************************************
 */
enum
{
	EIP_SPECIAL_USE_DATATYPE_LEN,	// The alternate length is typically in bytes.  This tells the code to use the
									// datatype as the alternate length.
	EIP_SPECIAL_BITWISE_TO_BOOL,	// Convert a bit (typically at an offset) to a BOOL value.
	EIP_SPECIAL_SHORT_STRING_DTYPE,	// The data associated with this DCID is a string which needs to be front loaded
									// with the string length.
	EIP_SPECIAL_MAX
};

typedef enum CIP_INSTANCE_0_EN_TD
{
	CIP_INST0_INVALID_ATTRIB,
	CIP_INST0_REV,
	CIP_INST0_MAX_INST,
	CIP_INST0_NUM_INST,
	CIP_INST0_ENUM_MAX
} CIP_INSTANCE_0_EN_TD;

// This structure defines the CIP Attribute Structure.
typedef struct CIP_ATTRIB_ST_TD
{
	CIP_ATTRIB_TD cip_attrib;
	DCI_ID_TD dcid;
	uint8_t offset;
	uint8_t alt_length;
	uint8_t special;
} CIP_ATTRIB_ST_TD;


// This structure defines the CIP Instance Structure.
typedef struct CIP_INSTANCE_ST_TD
{
	CIP_INSTANCE_TD instance;
	CIP_ATTRIB_ST_TD const* attrib_list;
} CIP_INSTANCE_ST_TD;


// This structure defines the CIP Class Structure.
typedef struct CIP_CLASS_ST_TD
{
	CIP_CLASS_TD cip_class;
	CIP_INSTANCE_ST_TD const* instances_list;
	uint16_t revision;
	uint16_t num_instances;	// Num instances is the same as max instances presently.  Unless my understanding is wrong.
	uint16_t num_attributes;
} CIP_CLASS_ST_TD;

#define CIP_ASM_NO_BYTE_OFFSET                      ( ( uint8_t )( ~0 ) )	// Value does not have offset.
#define CIP_ASM_HEAD_OF_LIST                        ( CIP_ASM_NO_BYTE_OFFSET - 1 )

// The following
typedef enum CIP_ASM_TYPE
{
	CIP_ASM_TYPE_STANDALONE,
	CIP_ASM_TYPE_LIST_HEAD,
	CIP_ASM_TYPE_LIST_MEMBER,
	CIP_ASM_TYPE_DYNAMIC_HEAD
} CIP_ASM_TYPE;
typedef struct CIP_ASM_STRUCT_TD
{
	DCI_ID_TD dci_id;
	uint8_t dci_id_offset;	// This variable keeps track of what DCID index you are on for multiple assemblies that use
							// a single DCID.
	CIP_INSTANCE_TD instance;
	bool produced;
	CIP_ASM_TYPE type;
	uint8_t asm_offset;
	DCI_ID_TD const* dcid_list;
	uint8_t dcid_list_len;
} CIP_ASM_STRUCT_TD;


// Defines the target.  This is a container for the entire Interface.
typedef struct EIP_TARGET_INFO_ST_TD
{
	uint16_t identity_dev_type;

	uint16_t total_classes;
	CIP_CLASS_ST_TD const* class_list;

	// The first word of this array is the number of classes.
	uint16_t const* msg_rtr_attrib1;

	uint16_t total_assemblies;
	uint16_t asm_max_data_len;
	CIP_ASM_STRUCT_TD const* asm_list;
} EIP_TARGET_INFO_ST_TD;

typedef struct CIP_STRUCT_PACK
{
	CIP_CLASS_ST_TD const* class_struct;
	CIP_INSTANCE_ST_TD const* instance_struct;
	CIP_ATTRIB_ST_TD const* attrib_struct;
} CIP_STRUCT_PACK;



/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class EtherNetIP_DCI
{
	public:
		EtherNetIP_DCI( EIP_TARGET_INFO_ST_TD const* eip_dev_profile,
						DCI_SOURCE_ID_TD unique_id );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~EtherNetIP_DCI( void ) {}

		uint8_t Get( CIP_GET_STRUCT* req_struct );

		uint8_t Set( CIP_SET_STRUCT* req_struct );

		uint8_t Get_Length( CIA_ID_TD* cia, uint16_t* length ) const;

		uint16_t Class_Count( void )             { return ( m_dev_profile->msg_rtr_attrib1[0] ); }

		uint16_t const* Class_List( void )      { return ( &m_dev_profile->msg_rtr_attrib1[1] ); }

	private:
		/**
		 * Provides access to the DCID and offset associated with the passed in register.
		 * It is a struct which is returned.  Null means it was not found.
		 */
		bool Find_Def( CIP_STRUCT_PACK* struct_pack, CIA_ID_TD* cia ) const;

		bool Find_Class( CIP_STRUCT_PACK* struct_pack, CIP_CLASS_TD cip_class ) const;

		bool Find_Inst( CIP_STRUCT_PACK* struct_pack, CIP_INSTANCE_TD cip_instance ) const;

		bool Find_Attrib( CIP_STRUCT_PACK* struct_pack, CIP_ATTRIB_TD cip_attrib ) const;

		uint8_t Get_Inst0( CIP_GET_STRUCT* req_struct ) const;

		uint8_t Get_DCI_Data( CIP_GET_STRUCT* req_struct );

		DCI_SOURCE_ID_TD m_source_id;
		DCI_Patron* m_dci_access;

		EIP_TARGET_INFO_ST_TD const* m_dev_profile;

		uint8_t Interpret_DCI_Error( DCI_ERROR_TD dci_error ) const;

		uint8_t Interpret_Missing_CIA( CIP_STRUCT_PACK* cip_struct ) const;

		uint8_t DCI_Check_Register_Range( uint16_t start_reg, uint16_t num_registers );

		void Create_MB_Reg_Data( uint8_t* data, uint_fast16_t len );

		void Extract_MB_Reg_Data( uint8_t* data, uint_fast16_t len );

};

#endif
