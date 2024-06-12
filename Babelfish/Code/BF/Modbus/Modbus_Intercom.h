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
#ifndef MODBUS_INTERCOM_H
	#define MODBUS_INTERCOM_H

#include "Modbus_Master.h"
#include "Intercom.h"
#include "Device_Identity.h"

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
// This is the number of times to retry a request.  If set to zero there is no re-request of the data.
// If set to 3, there will be a total of 4 requests sent the initial one then 3 retries (assuming there are issues).
#define MODBUS_INTERCOM_MAX_MSG_RETRIES     3

/// The MODBUS_ATOMIBLOCK is only used by the modbus intercom.  It is not passed outside.
/// It is used by Modbus_Intercom to extract the appropriate number of registers for one block.
typedef struct MODBUS_ATOMIBLOCK
{
	uint16_t start_reg;
	uint16_t num_registers;
} MODBUS_ATOMIBLOCK;

typedef struct MODBUS_INTERCOM_TARGET_INFO_ST
{
	const MODBUS_TARGET_INFO_ST_TD* modbus_target_def;

	const MODBUS_ATOMIBLOCK* mbus_atomiblock;
	MODBUS_REG_TD change_que_reg;
	uint16_t inter_message_delay;

	const INTERCOM_ATOMIBLOCK_SPEC* intercom_atomiblock;
} MODBUS_INTERCOM_TARGET_INFO_ST;


/*
 **************************************************************************************************
 *  Class Declaration
   --------------------------------------------------------------------------------------------------
 */
class Modbus_Intercom : public Intercom
{
	public:
		/**
		 * Constructor - needs definition.
		 * \param slave_address is the target devices slave address.
		 * \param modbus_target_def is the target devices intercom definition file.  essentially
		 * 		the behaviors and lookup tables associated with the specific device.
		 * \param inter_message_delay is a delay that is added before every message.
		 * 		It is in milliseconds and if set to zero there is no delay added.
		 * \param device_identity_handler is a handle to a storehouse of device identity information.
		 * 		It is also responsible for maintaining the presently connected device.
		 * \return a pointer to the new intercom object.
		 */
		Modbus_Intercom( uint8_t slave_address, Modbus_Master* modbus_master,
						 const MODBUS_INTERCOM_TARGET_INFO_ST* modbus_target_def,
						 Device_Identity* dev_identity_handler = NULL );
		/*
		 *****************************************************************************************
		 * See header file for function definition.
		 *****************************************************************************************
		 */
		~Modbus_Intercom( void ) {}


		/**
		 * This function will attempt to connect to the device.
		 * If success is returned then the correct device is connected.  This function has the option of
		 * never returning if it does not find a device.  Because this function may never return
		 * this function must be called in an RTOS task and not a task idle type loop.
		 * \param timeout_ms is the time allowed to search.  A time of 0 means never return.
		 * \return an indication of whether the connect succeeded.
		 */
		bool Connect( BF_Lib::Timers::TIMERS_TIME_TD timeout_ms );

		/**
		 * This function will cause the intercom to forget which device it was
		 * previously married to.  Essentially wiping the end product specific behavior away.
		 */
		void Disconnect( void );

		/**
		 * Get data will use a modbus read multiple register command to extract the single DCID.
		 * data is the destination for the data.
		 */
		INTERCOM_ERROR_ENUM Get_Data( DCI_ID_TD dcid, uint8_t* data,
									  INTERCOM_ATTRIB_ENUM attribute = INTERCOM_ATTRIB_RAM_VAL );

		/**
		 * Uses the write multiple holding register function to set register values.  This will set
		 * a single DCID value using the data pointed to by data.
		 */
		INTERCOM_ERROR_ENUM Set_Data( DCI_ID_TD dcid, uint8_t* data,
									  INTERCOM_ATTRIB_ENUM attribute = INTERCOM_ATTRIB_RAM_AND_INIT_VAL );

		/**
		 * Uses the read multiple write multiple modbus command.  Gets one DCID and sets one DCID.
		 * the data pointers point at the various pieces of data to set/get.
		 */
		INTERCOM_ERROR_ENUM GetSet_Data( DCI_ID_TD get_dcid, uint8_t* get_data,
										 DCI_ID_TD set_dcid, uint8_t* set_data,
										 INTERCOM_ATTRIB_ENUM get_attribute = INTERCOM_ATTRIB_RAM_VAL,
										 INTERCOM_ATTRIB_ENUM set_attribute = INTERCOM_ATTRIB_RAM_AND_INIT_VAL );

		/**
		 * This function will cause the end device to provide an indication when the
		 * passed DCID has changed.  The end device will then watch this value for
		 * changes.
		 */
		INTERCOM_ERROR_ENUM Set_DCID_Monitor( DCI_ID_TD dcid_to_monitor );

		/**
		 * This function provides the ability to query the end device for a
		 * a value that has changed and is ready to be fetched.
		 */
		INTERCOM_ERROR_ENUM Get_Changed_DCID( DCI_ID_TD* changed_dcid );

		/**
		 * Provides a system command level interface to the intercom.
		 * \param command is the desired command to pass down.
		 * \param data is only required if the system command requires data.
		 * \return the modbus error if any.
		 */
		INTERCOM_ERROR_ENUM System_Cmd( INTRCM_SYSCMD_ENUM command, uint8_t* data );

		/**
		 * This function provides the requester with a specification block defining any
		 * atomic retrieval functionality.  If an intercom service is capable of providing
		 * atomic data it should fill in an atomiblock spec defining what is available in
		 * the block.
		 */
		void Get_Atomiblock_Spec( const INTERCOM_ATOMIBLOCK_SPEC** atomiblock_spec );

		/**
		 * This function will load the requested atomic block into dest data.
		 * The block counter indicates which atomic block is being referenced.
		 * The requester will expect the dest data to contain all the data associated
		 * with the block.
		 * Data is suspect if there is an error returned.
		 */
		INTERCOM_ERROR_ENUM Get_Atomiblock( uint8_t block_counter, uint8_t* dest_data );

	private:
		INTERCOM_ERROR_ENUM Interpret_Modbus_Error( uint8_t modbus_error );

		uint8_t Get_Modbus_Attribute( INTERCOM_ATTRIB_ENUM intercom_attrib );

		Device_Identity* m_dev_identity_handler;

		Modbus_Master* m_mb_master;
		Modbus_DCI* m_modbus_dci;
		uint8_t m_slave_address;
		const MODBUS_ATOMIBLOCK* m_mbus_atomiblock_list;
		const INTERCOM_ATOMIBLOCK_SPEC* m_intercom_atomiblock;
		MODBUS_REG_TD m_monitor_reg;
		uint16_t m_inter_message_delay;								// in milliseconds.
};


#endif
