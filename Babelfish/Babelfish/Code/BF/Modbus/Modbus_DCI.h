/**
 *****************************************************************************************
 *  @file
 *	@brief It provides the interface between Modbus protocol and DCI database
 *
 *	@details Modbus protocol access the Modbus registers & the modbus registers are linked to DCI database
 *  @n DCI - Data store
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef MODBUS_DCI_H
#define MODBUS_DCI_H

#include "Modbus_DCI_Defines.h"
#include "DCI_Patron.h"

/// This structure defines the Modbus register to DCID lookup.
typedef struct DCI_MBUS_TO_DCID_LKUP_ST_TD
{
	MODBUS_REG_TD modbus_reg;
	DCI_LENGTH_TD offset;
	DCI_ID_TD dcid;
} DCI_MBUS_TO_DCID_LKUP_ST_TD;

/// This structure defines the DCID to Modbus Register lookup.
typedef struct DCI_MBUS_FROM_DCID_LKUP_ST_TD
{
	DCI_ID_TD dcid;
	DCI_LENGTH_TD length;
	MODBUS_REG_TD modbus_reg;
} DCI_MBUS_FROM_DCID_LKUP_ST_TD;

/// Defines the modbus target.  This is a container for the entire identity of a Modbus Interface.
typedef struct MODBUS_TARGET_INFO_ST_TD
{
	uint8_t total_mbus_objs;
	const DCI_ID_TD* mbus_obj_list;

	MODBUS_REG_TD total_registers;
	const DCI_MBUS_TO_DCID_LKUP_ST_TD* mbus_to_dcid;

	DCI_ID_TD total_dcids;
	const DCI_MBUS_FROM_DCID_LKUP_ST_TD* dcid_to_mbus;
} MODBUS_TARGET_INFO_ST_TD;

/**
 ****************************************************************************************
 * @brief This is a Modbus_DCI class
 * @details It provides an interface between Modbus protocol and DCI,
 * @n @b Usage: Access DCI registers as per request from Modbus protocol.
 * It provides public methods to
 * @n @b 1. Read Bits of Modbus coils
 * @n @b 2. Write Bits to modbus coils
 * @n @b 3. Read holding Registers of modbus
 * @n @b 4. Write holding Registers of modbus
 * @n @b 5. Verifying Register ranges
 * @n @b 6. Getting slave device identity of modbus
 * @n @b 7. Corresponding DCID for Modbus register
 ****************************************************************************************
 */
class Modbus_DCI
{
	public:

		/**
		 *  @brief Constructor
		 *  @details Creates a Modbus DCI
		 *  @param[in] modbus_dev_profile Defines the Modbus target,
		 *  This is a container for the entire identity of a Modbus Interface
		 *  @param[in] unique_id: A source ID for the patron access.
		 *  @param[in] correct_out_of_range_sets: Will cause the DCI Patron to fix any values
		 *  which are out of range.
		 *  @return this
		 */
		Modbus_DCI( const MODBUS_TARGET_INFO_ST_TD* modbus_dev_profile,
					DCI_SOURCE_ID_TD unique_id, bool correct_out_of_range_sets = true );

		/**
		 *  @brief Constructor
		 *  @details Creates a Modbus DCI
		 *  @param[in] modbus_dev_profile Defines the Modbus target,
		 *  This is a container for the entire identity of a Modbus Interface
		 *  @param[in] patron_interface: The interface to our patron and database.
		 *  @param[in] unique_id: A source ID for the patron access.
		 *  @return this
		 */
		Modbus_DCI( const MODBUS_TARGET_INFO_ST_TD* modbus_dev_profile,
					DCI_Patron* patron_interface, DCI_SOURCE_ID_TD unique_id );

		/**
		 *  @brief Destructor
		 *  @details This will get called when object of Modbus_DCI goes out of scope
		 *  @return None
		 */
		~Modbus_DCI( void ) {}

		/**
		 *  @brief Read bits of modbus Registers coils.
		 *  @details Read the bits of Modbus register coil while read coil modbus command,
		 *  in case any error found while accessing corresponding register then it shall return corresponding Modbus
		 * Error Codes.
		 *  @param[in] bit_get,  pointer to Multi-bit Get Frames structure, it has information of requested modbus
		 * frame.
		 *  @param[out] bit_get_resp, pointer to Multi-bit Get respond frames structure having current coil value of
		 * modbus register.
		 *  @return  Modbus Error Codes.
		 */
		uint8_t Read_Bits( MB_BIT_GET_STRUCT* bit_get, MB_BIT_GET_RESP_STRUCT* bit_get_resp );

		/**
		 *  @brief Write to Modbus Registers coils.
		 *  @details Write the Modbus registers coils as per requested value, in case of wrong request it shall return
		 * Modbus Error Codes.
		 *  in case any error found while accessing corresponding register then it shall return corresponding Modbus
		 * Error Codes.
		 *  @param[in] bit_set,  pointer to Multi-bit Set Frames structure, it has information of requested modbus
		 * frame.
		 *  @param[out] bit_set_resp, pointer to Multi-bit Set respond frames structure having current value of modbus
		 * structure with updated values.
		 *  @return  Modbus Error Codes.
		 */

		uint8_t Write_Bits( MB_BIT_SET_STRUCT* bit_set, MB_BIT_SET_RESP_STRUCT* bit_set_resp );

		/**
		 *  @brief Read Modbus Registers.
		 *  @details Read the current value of Modbus registers while read holding register modbus command,
		 *  in case any error found while accessing corresponding register then it shall return corresponding Modbus
		 * Error Codes.
		 *  @param[in] reg_get,  pointer to Multi-Reg Get Frames structure, it has information of requested modbus
		 * frame.
		 *  @param[out] reg_get_resp, pointer to Multi-Reg Get respond frames structure having current value of modbus
		 * structure.
		 *  @return  Modbus Error Codes.
		 */
		uint8_t Read_Registers( MB_REG_GET_STRUCT* reg_get, MB_REG_GET_RESP_STRUCT* reg_get_resp );

		/**
		 *  @brief Write to Modbus Registers.
		 *  @details this function code is used to write holding registers in a remote device(slave device) as per
		 * requested value, in case of wrong request it shall return Modbus Error Codes.
		 *  in case any error found while accessing corresponding register then it shall return corresponding Modbus
		 * Error Codes.
		 *  @param[in] reg_set,  pointer to Multi-Reg Set Frames structure, it has information of requested modbus
		 * frame.
		 *  @param[out] reg_set_resp, pointer to Multi-Reg Set respond frames structure having current value of modbus
		 * structure with updated values.
		 *  @return  Modbus Error Codes.
		 */
		uint8_t Write_Registers( MB_REG_SET_STRUCT* reg_set, MB_REG_SET_RESP_STRUCT* reg_set_resp );

		/**
		 *  @brief Check register range.
		 *  @details this function Check the read range before we do a write,  in case of wrong request it shall return
		 * Modbus Error Codes.
		 *  @param[in] start_reg,  starting modbus register number.
		 *  @param[in] num_registers, number of modbus register from start_reg.
		 *  @retval  02U :(MB_ILLEGAL_DATA_ADDRESS_ERROR_CODE)in case data id does not exist.
		 *  @retval  00U :(MB_NO_ERROR_CODE) in case data id exist.
		 */
		uint8_t Check_Register_Range( uint16_t start_reg, uint16_t num_registers ) const;

		/**
		 *  @brief get device identity.
		 *  @details this function allow reading the identification and additional information relative to the physical
		 * and functional
		 *  description of a remote device only i.e. read the device identification(basic device, regular device,
		 * extended device),
		 *  in case of wrong request it shall return Modbus Error Codes.
		 *  @param[in] dev_id_req,  pointer to modbus device identity request structure, it has information of requested
		 * modbus frame.
		 *  @param[out] dev_id_resp, pointer to modbus device identity responde structure, it has information of
		 * responded modbus frame.
		 *  @return  02U (MB_ILLEGAL_DATA_ADDRESS_ERROR_CODE)in case data id does not exist else 00U(MB_NO_ERROR_CODE).
		 */
		uint8_t Get_Device_Identity( MB_DEV_ID_REQ_STRUCT* dev_id_req,
									 MB_DEV_ID_RESP_STRUCT* dev_id_resp );

		/**
		 *  @brief Modbus Registers to DCID Lookup.
		 *  @details Provides access to the DCID and offset associated with the passed in modbus register,
		 *  in case DCID does not exist then null pointer in return.
		 *  @param[in] modbus_reg,  Modbus register number
		 *  @return Pointer to structure having Modbus register, offset & corresponding DCID.
		 *          NULL in case DCID not found for Modbus registers.
		 */
		const DCI_MBUS_TO_DCID_LKUP_ST_TD* Find_MBus_DCID( MODBUS_REG_TD modbus_reg ) const;

		/**
		 * Provides access to the MODBUS Register associated with the passed in DCID.
		 * It is a struct which is returned.  Null means it was not found.
		   ///This function shall be deprecated.
		   const DCI_MBUS_FROM_DCID_LKUP_ST_TD* Find_MBus_Reg( DCI_ID_TD dcid ) const;
		 */

		/**
		 *  @brief set register with requested data.
		 *  @details this function code is used to write modbus registers as per requested value, in case of wrong
		 * request it shall return DCI Error Codes.
		 *  in case any error found while accessing corresponding register then it shall return corresponding DCI error
		 * code.
		 *  A back door access method without having to create the structures above.  These are not
		 *   passed in from the protocol side but accessed more from the application side.
		 *  @param[in] start_reg,  starting modbus register from onward registers need to access in this function.
		 *  @param[in] num_registers, number of modbus register from start_reg.
		 *  @param[in] src_data, pointer to data which is suppose to be written on modbus registers.
		 *  @return  DCI Error Codes.
		 */
		DCI_ERROR_TD Set_Reg_Data( uint16_t start_reg, uint16_t num_registers, uint8_t* src_data,
								   uint8_t attribute = MB_ATTRIB_NORMAL_ACCESS );

		/**
		 *  @brief Get register value.
		 *  @details this function code is used to read  modbus holding registers.
		 *  in case any error found while accessing corresponding register then it shall return corresponding DCI error
		 * code.
		 *  A back door access method without having to create the structures above.  These are not
		 *  passed in from the protocol side but accessed more from the application side.
		 *  @param[in] start_reg,  starting modbus register from onward registers need to access in this function.
		 *  @param[in] num_registers, number of modbus register from start_reg.
		 *  @param[out] dest_data, data pointer which shall have value of modbus holding registers.
		 *  @return  DCI Error Codes.
		 */
		DCI_ERROR_TD Get_Reg_Data( uint16_t start_reg, uint16_t num_registers, uint8_t* dest_data,
								   uint8_t attribute = MB_ATTRIB_NORMAL_ACCESS );

		/**
		 *  @brief getting length of DCID.
		 *  @details this function code provide data length associated with DCID.
		 *  @param[in] dcid. DCID number
		 *  @return  Length of DCID.
		 */
		DCI_LENGTH_TD Get_Length( DCI_ID_TD dcid ) const;

	private:

		/**
		 * @brief Privatized Copy Constructor and Copy Assignment Operator.
		 * @details Copy Constructor and Copy Assignment Operator
		 */
		Modbus_DCI( const Modbus_DCI& object );
		Modbus_DCI & operator =( const Modbus_DCI& object );

		static const uint8_t MODBUS_DCI_REGISTER_BYTE_SIZE = 2U;
		static const uint8_t MB_DEV_ID_CONFORMITY_LEVEL_BASIC = 0x01U;
		static const uint8_t MB_DEV_ID_CONFORMITY_LEVEL_REGULAR = 0x02U;
		static const uint8_t MB_DEV_ID_CONFORMITY_LEVEL_EXTENDED = 0x03U;
		static const uint8_t MB_DEV_ID_CONFORMITY_LEVEL_BASIC_SINGLE = 0x81U;
		static const uint8_t MB_DEV_ID_CONFORMITY_LEVEL_REGULAR_SINGLE = 0x82U;
		static const uint8_t MB_DEV_ID_CONFORMITY_LEVEL_EXTENDED_SINGLE = 0x83U;
		static const uint8_t MODBUS_LENGTH_ATTRIB_RESP_LEN = 2U;
		static const DCI_MBUS_TO_DCID_LKUP_ST_TD* const LOOKUP_REG_TO_DCI_NOT_FOUND;
		static const DCI_MBUS_FROM_DCID_LKUP_ST_TD* const LOOKUP_DCI_TO_REG_NOT_FOUND;

		DCI_SOURCE_ID_TD m_source_id;
		DCI_Patron* m_dci_access;

		const MODBUS_TARGET_INFO_ST_TD* m_dev_profile;

		uint8_t Interpret_DCI_Error( DCI_ERROR_TD dci_error ) const;

		DCI_ERROR_TD DCI_Check_Register_Range( uint16_t start_reg, uint16_t num_registers ) const;

		void Create_MB_Reg_Data( uint8_t* data, uint_fast16_t len ) const;

		DCI_ERROR_TD Extract_MB_Reg_Data( uint8_t* data, uint_fast16_t len ) const;

		/**
		 * Provides singular DCI data access.  Used primarily for Get Device Identity.
		 * \param dcid is the id you want to retrieve.
		 * \param length is passed is the actual data length.
		 * \param dest_data is where the data should be put.
		 * \param max_buffer_length is the limit on the target data buffer.
		 * \return A modbus error code.
		 */
		uint8_t Get_DCI_Data( DCI_ID_TD dcid, DCI_LENGTH_TD* length, uint8_t* dest_data,
							  DCI_LENGTH_TD max_buffer_length );

};

#endif
