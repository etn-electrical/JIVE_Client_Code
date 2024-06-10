/**
 *****************************************************************************************
 *   @file
 *
 *   @brief
 *
 *   @details
 *
 *   @version
 *   C++ Style Guide Version 1.0
 *
 *   @copyright 2013 Eaton Corporation. All Rights Reserved.
 *****************************************************************************************
 */

#ifndef EIP_Device_Identity_H_
#define EIP_Device_Identity_H_

#include "Includes.h"
#include "Stdlib_MV.h"
#include "DCI_Defines.h"
#include "Delta_Com_Identity.h"
#include "DCI_Owner.h"
#include "Services.h"

/**
 * @brief Encapsulates handling of device identity parameters that behave in common fashion
 * across all LTK devices.
 */
class EIP_Device_Identity
{
	public:
		/**
		 * @brief Configuration structure (typically constants in flash rom).
		 */

		struct device_config_t
		{
			DCI_ID_TD m_product_code_dcid;
			DCI_ID_TD m_product_subcode_dcid;
			DCI_ID_TD m_firmware_revision_dcid;
			DCI_ID_TD m_hardware_revision_dcid;
			DCI_ID_TD m_serial_number_dcid;
		};

		/**
		 * @brief Constructs an instance of the class.
		 */
		EIP_Device_Identity( device_config_t const* config );

		/**
		 * @brief Destructor (for MISRA purposes only).
		 */
		virtual ~EIP_Device_Identity();

		/**
		 * @brief Restore identity parameters after wiping NV memory.
		 */
		virtual void Wipe_NV_Preserve_Identity( void );

		/**
		 * @brief Gets the product code.
		 */
		uint16_t Get_Product_Code( void ) const;

		/**
		 * @brief Gets the product subcode.
		 */
		uint16_t Get_Product_Subcode( void ) const;

		/**
		 * @brief Gets the firmware revision.
		 */
		uint32_t Get_Firmware_Revision( void ) const;

		/**
		 * @brief Gets the hardware revision.
		 */
		uint16_t Get_Hardware_Revision( void ) const;

		/**
		 * @brief Gets the serial numner.
		 */
		uint32_t Get_Serial_Number( void ) const;

	protected:
		static DCI_CB_RET_TD Write_Protect_Callback_Static( DCI_CBACK_PARAM_TD handle,
															DCI_ACCESS_ST_TD* access_struct );

	private:
		device_config_t const* m_config;
		DCI_Owner* m_product_code_owner;
		DCI_Owner* m_product_subcode_owner;
		DCI_Owner* m_firmware_revision_owner;
		DCI_Owner* m_hardware_revision_owner;
		DCI_Owner* m_product_serial_number_owner;

		DCI_CB_RET_TD Write_Protect_Callback( DCI_ACCESS_ST_TD* access_struct );

		virtual DCI_Owner* Get_DCI_Owner( DCI_ID_TD dcid );

		/**
		 * @brief Privatized Copy Constructor.
		 */
		EIP_Device_Identity( const EIP_Device_Identity& object );

		/**
		 * @brief Privatized Copy Assignment Operator.
		 */
		EIP_Device_Identity & operator =( const EIP_Device_Identity& object );

};

#endif

