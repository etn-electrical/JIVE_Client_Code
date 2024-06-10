#ifndef __DCI_ENUMS
#define __DCI_ENUMS

#include "Includes.h"

typedef BF_Lib::bool_t dci_enum_bool_t;
typedef uint8_t dci_enum_uint8_t;
typedef int8_t dci_enum_int8_t;
typedef uint16_t dci_enum_uint16_t;
typedef int16_t dci_enum_int16_t;
typedef uint32_t dci_enum_uint32_t;
typedef uint64_t dci_enum_uint64_t;
typedef int32_t dci_enum_int32_t;
typedef int64_t dci_enum_int64_t;
typedef float32_t dci_enum_float32_t;
typedef float64_t dci_enum_float64_t;


	//Enums for DCID DCI_FW_UPGRADE_MODE
	class DCI_FW_UPGRADE_MODE_ENUM {
		public: 
		static const dci_enum_uint8_t UNRESTRICTED_FW_UPGRADE = 0U;// Upgrade to any version
		static const dci_enum_uint8_t RESTRICTED_FW_UPGRADE = 1U;// Upgrade to same or higher version
		static const dci_enum_uint8_t NO_FW_UPGRADE = 2U;// Upgrade not allowed
		static const dci_enum_uint8_t MAX_ENUM  = 2U;
		static const uint32_t TOTAL_ENUMS  = 3U;
	};

	//Enums for DCID DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS
	class DCI_IP_ADR_ALLO_CNTRL_EIP_OR_DIP_STATUS_ENUM {
		public: 
		static const dci_enum_uint8_t DIP_SWITCH = 1U;// DIP Switch
		static const dci_enum_uint8_t ETHERNET = 2U;// Ethernet
		static const dci_enum_uint8_t MAX_ENUM  = 2U;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_IP_ADDRESS_ALLOCATION_METHOD
	class DCI_IP_ADDRESS_ALLOCATION_METHOD_ENUM {
		public: 
		static const dci_enum_uint8_t RESTORE_IP = 0U;// Statically Hardcoded
		static const dci_enum_uint8_t SET_DHCP = 1U;// DHCP Allocated
		static const dci_enum_uint8_t SET_NV_MEMORY = 2U;// Taken From NV
		static const dci_enum_uint8_t DIP_SWITCH_SETTING = 3U;// DIP Switch Configured
		static const dci_enum_uint8_t MAX_ENUM  = 3U;
		static const uint32_t TOTAL_ENUMS  = 4U;
	};

	//Enums for DCID DCI_PHY1_LINK_SPEED_SELECT
	class DCI_PHY1_LINK_SPEED_SELECT_ENUM {
		public: 
		static const dci_enum_uint16_t SPEED_10_MBPS = 10U;// 10 Mbps speed
		static const dci_enum_uint16_t SPEED_100_MBPS = 100U;// 100 Mbps speed
		static const dci_enum_uint16_t MAX_ENUM  = 100U;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_PHY2_LINK_SPEED_SELECT
	class DCI_PHY2_LINK_SPEED_SELECT_ENUM {
		public: 
		static const dci_enum_uint16_t SPEED_10_MBPS = 10U;// 10 Mbps speed
		static const dci_enum_uint16_t SPEED_100_MBPS = 100U;// 100 Mbps speed
		static const dci_enum_uint16_t MAX_ENUM  = 100U;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL
	class DCI_WEB_ACCESS_PASSWORD_WAIVER_LEVEL_ENUM {
		public: 
		static const dci_enum_uint8_t NO_ROLE = 0U;// This can be accessd without authentication
		static const dci_enum_uint8_t VIEWER = 7U;// This is a Role having readonly access
		static const dci_enum_uint8_t OPERATOR = 15U;// This is an Operator Role having Product control privileges
		static const dci_enum_uint8_t ENGINEER = 31U;// This is an Engineer Role having Product configuration privileges
		static const dci_enum_uint8_t ADMIN = 99U;// This is an Administrator having all privileges
		static const dci_enum_uint8_t MAX_ENUM  = 99U;
		static const uint32_t TOTAL_ENUMS  = 5U;
	};

	//Enums for DCID DCI_EXAMPLE_MODBUS_SERIAL_PARITY
	class DCI_EXAMPLE_MODBUS_SERIAL_PARITY_ENUM {
		public: 
		static const dci_enum_uint16_t EVEN_PARITY = 0U;// Even Parity
		static const dci_enum_uint16_t ODD_PARITY = 1U;// Odd Parity
		static const dci_enum_uint16_t NONE_PARITY = 2U;// None Parity
		static const dci_enum_uint16_t MAX_ENUM  = 2U;
		static const uint32_t TOTAL_ENUMS  = 3U;
	};

	//Enums for DCID DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT
	class DCI_EXAMPLE_MODBUS_SERIAL_STOPBIT_ENUM {
		public: 
		static const dci_enum_uint16_t ONE_STOP_BIT = 0U;// One Stop Bit
		static const dci_enum_uint16_t TWO_STOP_BIT = 1U;// Two Stop Bit
		static const dci_enum_uint16_t MAX_ENUM  = 1U;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_HTTP_ENABLE
	class DCI_HTTP_ENABLE_ENUM {
		public: 
		static const dci_enum_bool_t HTTP_DISABLE = 0;// Disable
		static const dci_enum_bool_t HTTP_ENABLE = 1;// Enable
		static const dci_enum_bool_t MAX_ENUM  = 1;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_DEVICE_PARAMETERIZATION_ENABLE
	class DCI_DEVICE_PARAMETERIZATION_ENABLE_ENUM {
		public: 
		static const dci_enum_bool_t DISABLE_PARAM_DOWNLOAD = 0;// Ignores parameters in parameterization message
		static const dci_enum_bool_t ENABLE_PARAM_DOWNLOAD = 1;// Download configuration in parameterization message to C445
		static const dci_enum_bool_t MAX_ENUM  = 1;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_PRODUCT_ACTIVE_FAULT
	class DCI_PRODUCT_ACTIVE_FAULT_ENUM {
		public: 
		static const dci_enum_uint16_t NO_FAULTS = 0U;// No Faults
		static const dci_enum_uint16_t PROTECTION_UNDER_VOLTAGE = 1U;// Under voltage
		static const dci_enum_uint16_t PROTECTION_OVER_VOLTAGE = 2U;// Over voltage
		static const dci_enum_uint16_t PROTECTION_HRG = 3U;// External GF
		static const dci_enum_uint16_t PROTECTION_RESIDUAL_GF = 4U;// Residual GF
		static const dci_enum_uint16_t PROTECTION_I_PHASE_LOSS = 5U;// Current phase loss
		static const dci_enum_uint16_t PROTECTION_I_UNBALANCE = 6U;// Current unbalance
		static const dci_enum_uint16_t PROTECTION_INSTANTANEOUS_OVER_CURRENT = 7U;// Instantaneous over current
		static const dci_enum_uint16_t PROTECTION_JAM = 8U;// Jam
		static const dci_enum_uint16_t PROTECTION_PF_DEVIATION = 9U;// PF Deviation
		static const dci_enum_uint16_t PROTECTION_V_PHASE_LOSS = 10U;// Voltage phase loss
		static const dci_enum_uint16_t PROTECTION_V_UNBALANCE = 11U;// Voltage unbalance
		static const dci_enum_uint16_t PROTECTION_FREQUENCY_DEVIATION_FAST = 12U;// Frequency deviation fast
		static const dci_enum_uint16_t PROTECTION_FREQUENCY_DEVIATION_SLOW = 13U;// Frequency deviation slow
		static const dci_enum_uint16_t PROTECTION_UNDER_CURRENT = 14U;// Under current
		static const dci_enum_uint16_t PROTECTION_HIGH_POWER = 15U;// High power
		static const dci_enum_uint16_t PROTECTION_LOW_POWER = 16U;// Low power
		static const dci_enum_uint16_t PROTECTION_CONTACTOR_FAILURE = 17U;// Contactor failure
		static const dci_enum_uint16_t PROTECTION_STARTS_PER_HOUR_LIMIT = 18U;// Starts limit exceeded
		static const dci_enum_uint16_t PROTECTION_OVERLOAD = 19U;// Overload
		static const dci_enum_uint16_t PROTECTION_STALL = 20U;// Stall
		static const dci_enum_uint16_t PROTECTION_PHASE_ROTATION = 21U;// Phase rotation mismatch
		static const dci_enum_uint16_t PROTECTION_PTC = 22U;// PTC - See PTC State for details
		static const dci_enum_uint16_t PROTECTION_UNDER_VOLTAGE_RESTART = 23U;// Under voltage restart
		static const dci_enum_uint16_t MM_FAULT = 24U;// Measurement Module fault
		static const dci_enum_uint16_t FIELDBUS_COMMLOSS = 25U;// Communication loss on active fieldbus
		static const dci_enum_uint16_t MM_COMM_FAULT = 26U;// Measurement Module not available or communication loss with the module
		static const dci_enum_uint16_t CUI_COMM_FAULT = 27U;// User Interface not available or communication loss with the module
		static const dci_enum_uint16_t CONTROL_TEST_TRIP = 28U;// Test trip was triggered
		static const dci_enum_uint16_t OPTCOM_COMM_FAULT = 29U;// Option Card not available or communication loss with the module
		static const dci_enum_uint16_t RTC_OPTION_NV_MEM_FAIL = 30U;// RTC / Backup Memory Option Board NV memory fail
		static const dci_enum_uint16_t CUI_MISMATCH = 31U;// Currently connected User Interface does not match with what was connected before
		static const dci_enum_uint16_t MM_MISMATCH = 32U;// Currently connected Measurement Module does not match with what was connected before
		static const dci_enum_uint16_t COMM_CARD_MISMATCH = 33U;// Currently connected Option Card does not match with what was connected before
		static const dci_enum_uint16_t MM_FIRMWARE_INCOMPATIBLE = 34U;// Measurement Module firmware is incompatible
		static const dci_enum_uint16_t BUI_FIRMWARE_INCOMPATIBLE = 35U;// User Interface firmware is incompatible
		static const dci_enum_uint16_t EDC_FIRMWARE_INCOMPATIBLE = 36U;// Ethernet Option Card firmware is incompatible
		static const dci_enum_uint16_t PDC_FIRMWARE_INCOMPATIBLE = 37U;// Profi Option Card firmware is incompatible
		static const dci_enum_uint16_t INT_PWRBRD_COMMLOSS = 500U;// Internal - communication loss with Power Supply Board
		static const dci_enum_uint16_t INT_PWRBRD_NOT_ANSWERING = 501U;// Internal - Power Supply Board is not responding to SPI
		static const dci_enum_uint16_t INT_BCM_NV_MEM_READ_INVALID_CHECKSUM = 502U;// Internal - Checksums in NV memory (FRAM) didn't match during read (neither pair)
		static const dci_enum_uint16_t INT_BCM_NV_MEM_WRITE_INVALID_CHECKSUM = 503U;// Internal - Checksums in NV memory (FRAM) didn't match during write (neither pair)
		static const dci_enum_uint16_t INT_BACKMEM_RTC_MISSING = 504U;// Internal - RTC / Backup Memory Option Card is missing
		static const dci_enum_uint16_t INT_BACKMEM_RTC_MISMATCH = 505U;// Internal - RTC / Backup Memory Option Card does not match actual
		static const dci_enum_uint16_t INT_BACKMEM_RTC_NV_FAULT = 506U;// Internal - RTC / Backup Memory Option Card has NV Fault.
		static const dci_enum_uint16_t INT_BCM_SERIAL_FLASH_BAD = 507U;// Internal - serial flash memory fault (Attempt Factory Reset first.  Return to manufacturer if not cleared)
		static const dci_enum_uint16_t INT_MAPPING_ERROR = 508U;// Internal - logic mapping error (Attempt factory reset)
		static const dci_enum_uint16_t INT_UI_NVMEM_FAULT = 509U;// Internal - UI NV memory error
		static const dci_enum_uint16_t INT_OPTION_CARD_NVMEM_FAULT = 510U;// Internal - Option card NV memory error
		static const dci_enum_uint16_t MAX_ENUM  = 510U;
		static const uint32_t TOTAL_ENUMS  = 49U;
	};

	//Enums for DCID DCI_BACNET_IP_UPD_PORT_NUM
	class DCI_BACNET_IP_UPD_PORT_NUM_ENUM {
		public: 
		static const dci_enum_uint16_t BAC0 = 47808U;// Backnet 0
		static const dci_enum_uint16_t BAC1 = 47809U;// Backnet 1
		static const dci_enum_uint16_t BAC2 = 47810U;// Backnet 2
		static const dci_enum_uint16_t BAC3 = 47811U;// Backnet 3
		static const dci_enum_uint16_t BAC4 = 47812U;// Backnet 4
		static const dci_enum_uint16_t BAC5 = 47813U;// Backnet 5
		static const dci_enum_uint16_t BAC6 = 47814U;// Backnet 6
		static const dci_enum_uint16_t BAC7 = 47815U;// Backnet 7
		static const dci_enum_uint16_t BAC8 = 47816U;// Backnet 8
		static const dci_enum_uint16_t BAC9 = 47817U;// Backnet 9
		static const dci_enum_uint16_t BACA = 47818U;// Backnet 10
		static const dci_enum_uint16_t BACB = 47819U;// Backnet 11
		static const dci_enum_uint16_t BACC = 47820U;// Backnet 12
		static const dci_enum_uint16_t BACD = 47821U;// Backnet 13
		static const dci_enum_uint16_t BACE = 47822U;// Backnet 14
		static const dci_enum_uint16_t BACF = 47823U;// Backnet 15
		static const dci_enum_uint16_t MAX_ENUM  = 47823U;
		static const uint32_t TOTAL_ENUMS  = 16U;
	};

	//Enums for DCID DCI_BACNET_IP_FOREIGN_DEVICE
	class DCI_BACNET_IP_FOREIGN_DEVICE_ENUM {
		public: 
		static const dci_enum_uint8_t DISABLED = 0U;// Disabled
		static const dci_enum_uint8_t ENABLED = 1U;// Enabled
		static const dci_enum_uint8_t MAX_ENUM  = 1U;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_BACNET_IP_BBMD_PORT
	class DCI_BACNET_IP_BBMD_PORT_ENUM {
		public: 
		static const dci_enum_uint16_t BAC0 = 47808U;// Bacnet 0
		static const dci_enum_uint16_t BAC1 = 47809U;// Bacnet 1
		static const dci_enum_uint16_t BAC2 = 47810U;// Bacnet 2
		static const dci_enum_uint16_t BAC3 = 47811U;// Bacnet 3
		static const dci_enum_uint16_t BAC4 = 47812U;// Bacnet 4
		static const dci_enum_uint16_t BAC5 = 47813U;// Bacnet 5
		static const dci_enum_uint16_t BAC6 = 47814U;// Bacnet 6
		static const dci_enum_uint16_t BAC7 = 47815U;// Bacnet 7
		static const dci_enum_uint16_t BAC8 = 47816U;// Bacnet 8
		static const dci_enum_uint16_t BAC9 = 47817U;// Bacnet 9
		static const dci_enum_uint16_t BACA = 47818U;// Bacnet 10
		static const dci_enum_uint16_t BACB = 47819U;// Bacnet 11
		static const dci_enum_uint16_t BACC = 47820U;// Bacnet 12
		static const dci_enum_uint16_t BACD = 47821U;// Bacnet 13
		static const dci_enum_uint16_t BACE = 47822U;// Bacnet 14
		static const dci_enum_uint16_t BACF = 47823U;// Bacnet 15
		static const dci_enum_uint16_t MAX_ENUM  = 47823U;
		static const uint32_t TOTAL_ENUMS  = 16U;
	};

	//Enums for DCID DCI_BACNET_IP_PROTOCOL_STATUS
	class DCI_BACNET_IP_PROTOCOL_STATUS_ENUM {
		public: 
		static const dci_enum_uint8_t STOPPED = 0U;// Stopped
		static const dci_enum_uint8_t OPERATIONAL = 1U;// Operational
		static const dci_enum_uint8_t FAULTED = 2U;// Faulted
		static const dci_enum_uint8_t MAX_ENUM  = 2U;
		static const uint32_t TOTAL_ENUMS  = 3U;
	};

	//Enums for DCID DCI_BACNET_IP_FAULT_BEHAVIOR
	class DCI_BACNET_IP_FAULT_BEHAVIOR_ENUM {
		public: 
		static const dci_enum_uint8_t FIELDBUS_CONTROL = 0U;// in Fieldbus Control
		static const dci_enum_uint8_t ALL_CONTROL = 1U;// in All Control
		static const dci_enum_uint8_t MAX_ENUM  = 1U;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_ENABLE_BACNET_IP
	class DCI_ENABLE_BACNET_IP_ENUM {
		public: 
		static const dci_enum_uint8_t DISABLED = 0U;// Disabled
		static const dci_enum_uint8_t ENABLED = 1U;// Enabled
		static const dci_enum_uint8_t MAX_ENUM  = 1U;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_BACNET_MSTP_BAUD
	class DCI_BACNET_MSTP_BAUD_ENUM {
		public: 
		static const dci_enum_uint32_t BAUDRATE_9600 = 0U;// Baudrate 9600
		static const dci_enum_uint32_t BAUDRATE_19200 = 1U;// Baudrate 19200
		static const dci_enum_uint32_t BAUDRATE_38400 = 2U;// Baudrate 38400
		static const dci_enum_uint32_t BAUDRATE_76800 = 3U;// Baudrate 76800
		static const dci_enum_uint32_t BAUDRATE_115200 = 4U;// Baudrate 115200
		static const dci_enum_uint32_t MAX_ENUM  = 4U;
		static const uint32_t TOTAL_ENUMS  = 5U;
	};

	//Enums for DCID DCI_BACNET_MSTP_PARITY
	class DCI_BACNET_MSTP_PARITY_ENUM {
		public: 
		static const dci_enum_uint16_t EVEN_PARITY = 0U;// Even Parity
		static const dci_enum_uint16_t ODD_PARITY = 1U;// Odd Parity
		static const dci_enum_uint16_t NONE_PARITY = 2U;// None Parity
		static const dci_enum_uint16_t MAX_ENUM  = 2U;
		static const uint32_t TOTAL_ENUMS  = 3U;
	};

	//Enums for DCID DCI_BACNET_MSTP_STOPBIT
	class DCI_BACNET_MSTP_STOPBIT_ENUM {
		public: 
		static const dci_enum_uint16_t ONE_STOP_BIT = 0U;// One Stop Bit
		static const dci_enum_uint16_t TWO_STOP_BIT = 1U;// Two Stop Bit
		static const dci_enum_uint16_t MAX_ENUM  = 1U;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_BACNET_MSTP_PROTOCOL_STATUS
	class DCI_BACNET_MSTP_PROTOCOL_STATUS_ENUM {
		public: 
		static const dci_enum_uint8_t STOPPED = 0U;// Stopped
		static const dci_enum_uint8_t OPERATIONAL = 1U;// Operational
		static const dci_enum_uint8_t FAULTED = 2U;// Faulted
		static const dci_enum_uint8_t MAX_ENUM  = 2U;
		static const uint32_t TOTAL_ENUMS  = 3U;
	};

	//Enums for DCID DCI_BACNET_FAULT_CODE
	class DCI_BACNET_FAULT_CODE_ENUM {
		public: 
		static const dci_enum_uint8_t NONE = 0U;// None
		static const dci_enum_uint8_t SOLE_MASTER = 1U;// Sole Master
		static const dci_enum_uint8_t DUPLICATE_MAC_ID = 2U;// Duplicate MAC ID
		static const dci_enum_uint8_t BAUD_RATE_FAULT = 3U;// Baud rate fault
		static const dci_enum_uint8_t MAX_ENUM  = 3U;
		static const uint32_t TOTAL_ENUMS  = 4U;
	};

	//Enums for DCID DCI_TIME_OFFSET
	class DCI_TIME_OFFSET_ENUM {
		public: 
		static const dci_enum_int16_t GMT_PLUS_660 = 660;// GMT-11:00
		static const dci_enum_int16_t GMT_PLUS_600 = 600;// GMT-10:00
		static const dci_enum_int16_t GMT_PLUS_540 = 540;// GMT-09:00
		static const dci_enum_int16_t GMT_PLUS_480 = 480;// GMT-08:00
		static const dci_enum_int16_t GMT_PLUS_420 = 420;// GMT-07:00
		static const dci_enum_int16_t GMT_PLUS_360 = 360;// GMT-06:00
		static const dci_enum_int16_t GMT_PLUS_300 = 300;// GMT-05:00
		static const dci_enum_int16_t GMT_PLUS_270 = 270;// GMT-04:30
		static const dci_enum_int16_t GMT_PLUS_240 = 240;// GMT-04:00
		static const dci_enum_int16_t GMT_PLUS_210 = 210;// GMT-03:30
		static const dci_enum_int16_t GMT_PLUS_180 = 180;// GMT-03:00
		static const dci_enum_int16_t GMT_PLUS_120 = 120;// GMT-02:00
		static const dci_enum_int16_t GMT_PLUS_60 = 60;// GMT-01:00
		static const dci_enum_int16_t GMT_UTC = 0;// GMT/UTC
		static const dci_enum_int16_t GMT_NEG_60 = -60;// GMT+01:00
		static const dci_enum_int16_t GMT_NEG_120 = -120;// GMT+02:00
		static const dci_enum_int16_t GMT_NEG_180 = -180;// GMT+03:00
		static const dci_enum_int16_t GMT_NEG_210 = -210;// GMT+03:30
		static const dci_enum_int16_t GMT_NEG_240 = -240;// GMT+04:00
		static const dci_enum_int16_t GMT_NEG_300 = -300;// GMT+05:00
		static const dci_enum_int16_t GMT_NEG_330 = -330;// GMT+05:30
		static const dci_enum_int16_t GMT_NEG_345 = -345;// GMT+05:45
		static const dci_enum_int16_t GMT_NEG_360 = -360;// GMT+06:00
		static const dci_enum_int16_t GMT_NEG_390 = -390;// GMT+06:30
		static const dci_enum_int16_t GMT_NEG_420 = -420;// GMT+07:00
		static const dci_enum_int16_t GMT_NEG_480 = -480;// GMT+08:00
		static const dci_enum_int16_t GMT_NEG_540 = -540;// GMT+09:00
		static const dci_enum_int16_t GMT_NEG_570 = -570;// GMT+09:30
		static const dci_enum_int16_t GMT_NEG_600 = -600;// GMT+10:00
		static const dci_enum_int16_t GMT_NEG_660 = -660;// GMT+11:00
		static const dci_enum_int16_t GMT_NEG_720 = -720;// GMT+12:00
		static const dci_enum_int16_t GMT_NEG_780 = -780;// GMT+13:00
		static const dci_enum_int16_t MAX_ENUM  = 660;
		static const uint32_t TOTAL_ENUMS  = 32U;
	};

	//Enums for DCID DCI_DATE_FORMAT
	class DCI_DATE_FORMAT_ENUM {
		public: 
		static const dci_enum_uint8_t DD_MM_YYYY = 0U;// mm/dd/yyyy
		static const dci_enum_uint8_t MM_DD_YYYY = 1U;// dd/mm/yyyy
		static const dci_enum_uint8_t DDMMYYYY = 2U;// yyyy-mm-dd
		static const dci_enum_uint8_t YYYY_MM_DD = 3U;// dd mm yyyy
		static const dci_enum_uint8_t MAX_ENUM  = 3U;
		static const uint32_t TOTAL_ENUMS  = 4U;
	};

	//Enums for DCID DCI_TIME_FORMAT
	class DCI_TIME_FORMAT_ENUM {
		public: 
		static const dci_enum_uint8_t TWELVE_HOURS_FORMAT = 0U;// 12Hrs (AM/PM)
		static const dci_enum_uint8_t TWENTYFOUR_HOURS_FORMAT = 1U;// 24Hrs
		static const dci_enum_uint8_t MAX_ENUM  = 1U;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_REST_RESET_COMMAND
	class DCI_REST_RESET_COMMAND_ENUM {
		public: 
		static const dci_enum_uint8_t REBOOT_DEVICE = 1U;// Reboot Device
		static const dci_enum_uint8_t FACTORY_RESET = 2U;// Factory Reset
		static const dci_enum_uint8_t MAX_ENUM  = 2U;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_DEVICE_CERT_VALID
	class DCI_DEVICE_CERT_VALID_ENUM {
		public: 
		static const dci_enum_uint8_t CERT_INVALID = 0U;// Certificate Invalid
		static const dci_enum_uint8_t CERT_VALID = 1U;// Certificate Valid
		static const dci_enum_uint8_t MAX_ENUM  = 1U;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_DEVICE_CERT_CONTROL
	class DCI_DEVICE_CERT_CONTROL_ENUM {
		public: 
		static const dci_enum_uint8_t CERT_DO_NOT_GENERATE = 0U;// Certificate Generation Disable. If disabled certificate will NOT generate after factory reset.
		static const dci_enum_uint8_t CERT_GENERATE = 1U;// Certificate Generation Enable. If enabled certificate will generate after factory reset.
		static const dci_enum_uint8_t CERT_GENERATE_IP_CHANGE = 2U;// Certificate Generation Enable on IP change. If enabled certificate will generate on boot-up if IP is changed.
		static const dci_enum_uint8_t MAX_ENUM  = 2U;
		static const uint32_t TOTAL_ENUMS  = 3U;
	};

	//Enums for DCID DCI_SNTP_SERVICE_ENABLE
	class DCI_SNTP_SERVICE_ENABLE_ENUM {
		public: 
		static const dci_enum_uint8_t DISABLE_SNTP_SERVICE = 0U;// Disable
		static const dci_enum_uint8_t ENABLE_SNTP_SERVICE_MANUAL = 1U;// Manual Server Entry
		static const dci_enum_uint8_t MAX_ENUM  = 1U;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_SNTP_ACTIVE_SERVER
	class DCI_SNTP_ACTIVE_SERVER_ENUM {
		public: 
		static const dci_enum_uint8_t UNCONNECTED = 0U;// Unconnected
		static const dci_enum_uint8_t CONNECTED_SERVER_1 = 1U;// Connected_To_Server_1
		static const dci_enum_uint8_t CONNECTED_SERVER_2 = 2U;// Connected_To_Server_2
		static const dci_enum_uint8_t CONNECTED_SERVER_3 = 3U;// Connected_To_Server_3
		static const dci_enum_uint8_t MAX_ENUM  = 3U;
		static const uint32_t TOTAL_ENUMS  = 4U;
	};

	//Enums for DCID DCI_IOT_UPDATE_RATE
	class DCI_IOT_UPDATE_RATE_ENUM {
		public: 
		static const dci_enum_uint8_t RATE_5_SEC = 5U;// 5 seconds
		static const dci_enum_uint8_t RATE_10_SEC = 10U;// 10 seconds
		static const dci_enum_uint8_t RATE_15_SEC = 15U;// 15 seconds
		static const dci_enum_uint8_t RATE_20_SEC = 20U;// 20 seconds
		static const dci_enum_uint8_t RATE_25_SEC = 25U;// 25 seconds
		static const dci_enum_uint8_t RATE_30_SEC = 30U;// 30 seconds
		static const dci_enum_uint8_t MAX_ENUM  = 30U;
		static const uint32_t TOTAL_ENUMS  = 6U;
	};

	//Enums for DCID DCI_IOT_CADENCE_UPDATE_RATE
	class DCI_IOT_CADENCE_UPDATE_RATE_ENUM {
		public: 
		static const dci_enum_uint8_t RATE_5_SEC = 5U;// 5 seconds
		static const dci_enum_uint8_t RATE_10_SEC = 10U;// 10 seconds
		static const dci_enum_uint8_t RATE_15_SEC = 15U;// 15 seconds
		static const dci_enum_uint8_t RATE_20_SEC = 20U;// 20 seconds
		static const dci_enum_uint8_t RATE_25_SEC = 25U;// 25 seconds
		static const dci_enum_uint8_t RATE_30_SEC = 30U;// 30 seconds
		static const dci_enum_uint8_t MAX_ENUM  = 30U;
		static const uint32_t TOTAL_ENUMS  = 6U;
	};

	//Enums for DCID DCI_GROUP0_CADENCE
	class DCI_GROUP0_CADENCE_ENUM {
		public: 
		static const dci_enum_int32_t CADENCE_STOP_PUBLISHING = -1;// Stop publishing
		static const dci_enum_int32_t CADENCE_IMMEDIATE = 0;// Immediate publishing
		static const dci_enum_int32_t CADENCE_1_SEC = 1000;// Cadence 1s
		static const dci_enum_int32_t CADENCE_5_SEC = 5000;// Cadence 5s
		static const dci_enum_int32_t CADENCE_10_SEC = 10000;// Cadence 10s
		static const dci_enum_int32_t CADENCE_15_SEC = 15000;// Cadence 15s
		static const dci_enum_int32_t CADENCE_20_SEC = 20000;// Cadence 20s
		static const dci_enum_int32_t MAX_ENUM  = 20000;
		static const uint32_t TOTAL_ENUMS  = 7U;
	};

	//Enums for DCID DCI_GROUP1_CADENCE
	class DCI_GROUP1_CADENCE_ENUM {
		public: 
		static const dci_enum_int32_t CADENCE_STOP_PUBLISHING = -1;// Stop publishing
		static const dci_enum_int32_t CADENCE_10_SEC = 10000;// Cadence_10s
		static const dci_enum_int32_t CADENCE_60_SEC = 60000;// Cadence_60s
		static const dci_enum_int32_t CADENCE_120_SEC = 120000;// Cadence_120s
		static const dci_enum_int32_t CADENCE_300_SEC = 300000;// Cadence_300s
		static const dci_enum_int32_t CADENCE_500_SEC = 500000;// Cadence_500s
		static const dci_enum_int32_t CADENCE_600_SEC = 600000;// Cadence_600s
		static const dci_enum_int32_t MAX_ENUM  = 600000;
		static const uint32_t TOTAL_ENUMS  = 7U;
	};

	//Enums for DCID DCI_GROUP2_CADENCE
	class DCI_GROUP2_CADENCE_ENUM {
		public: 
		static const dci_enum_int32_t CADENCE_STOP_PUBLISHING = -1;// Stop publishing
		static const dci_enum_int32_t CADENCE_IMMEDIATE = 0;// Immediate publishing
		static const dci_enum_int32_t CADENCE_1_SEC = 1000;// Cadence 1s
		static const dci_enum_int32_t CADENCE_5_SEC = 5000;// Cadence 5s
		static const dci_enum_int32_t CADENCE_10_SEC = 10000;// Cadence 10s
		static const dci_enum_int32_t CADENCE_15_SEC = 15000;// Cadence 15s
		static const dci_enum_int32_t CADENCE_20_SEC = 20000;// Cadence 20s
		static const dci_enum_int32_t MAX_ENUM  = 20000;
		static const uint32_t TOTAL_ENUMS  = 7U;
	};

	//Enums for DCID DCI_GROUP3_CADENCE
	class DCI_GROUP3_CADENCE_ENUM {
		public: 
		static const dci_enum_int32_t CADENCE_STOP_PUBLISHING = -1;// Stop publishing
		static const dci_enum_int32_t CADENCE_IMMEDIATE = 0;// Immediate publishing
		static const dci_enum_int32_t CADENCE_1_SEC = 1000;// Cadence 1s
		static const dci_enum_int32_t CADENCE_5_SEC = 5000;// Cadence 5s
		static const dci_enum_int32_t CADENCE_10_SEC = 10000;// Cadence 10s
		static const dci_enum_int32_t CADENCE_15_SEC = 15000;// Cadence 15s
		static const dci_enum_int32_t CADENCE_20_SEC = 20000;// Cadence 20s
		static const dci_enum_int32_t MAX_ENUM  = 20000;
		static const uint32_t TOTAL_ENUMS  = 7U;
	};

	//Enums for DCID DCI_IOT_CONN_STAT_REASON
	class DCI_IOT_CONN_STAT_REASON_ENUM {
		public: 
		static const dci_enum_uint8_t IOT_INITIALIZING = 0U;// IoT initial state (default)
		static const dci_enum_uint8_t IOT_CLOUD_CONNECTED = 1U;// Cloud connected
		static const dci_enum_uint8_t IOT_DISABLED = 2U;// IoT disabled
		static const dci_enum_uint8_t IOT_CLOCK_INCORRECT = 3U;// IoT clock not correctly set
		static const dci_enum_uint8_t IOT_RECONNECTING = 4U;// Reconnecting
		static const dci_enum_uint8_t IOT_FAILED_TO_OPEN_CONN = 5U;// Failed to open connection
		static const dci_enum_uint8_t IOT_TOO_MANY_LOST_MESSAGES = 6U;// Too many lost messages
		static const dci_enum_uint8_t IOT_INVALID_CONN_STRING = 7U;// Invalid connection string
		static const dci_enum_uint8_t IOT_INVALID_CONFIG = 8U;// Invalid configuration
		static const dci_enum_uint8_t IOT_DAILY_LIMIT_REACHED = 9U;// Daily cloud message limit reached
		static const dci_enum_uint8_t IOT_EXPIRED_SAS_TOKEN = 10U;// SAS token expired
		static const dci_enum_uint8_t IOT_DEVICE_DISABLED = 11U;// Device disabled by user on IoT hub
		static const dci_enum_uint8_t IOT_RETRY_EXPIRED = 12U;// Retry expired
		static const dci_enum_uint8_t IOT_NO_NETWORK = 13U;// No network
		static const dci_enum_uint8_t IOT_COMMUNICATION_ERROR = 14U;// Communication error
		static const dci_enum_uint8_t IOT_UNKNOWN_ERROR = 15U;// Unknown error
		static const dci_enum_uint8_t IOT_TLS_INIT_FAILED = 16U;// TLS initialization failure
		static const dci_enum_uint8_t IOT_NETWORK_LINK_DOWN = 17U;// Ethernet/network interface link down
		static const dci_enum_uint8_t MAX_ENUM  = 17U;
		static const uint32_t TOTAL_ENUMS  = 18U;
	};

	//Enums for DCID DCI_CHANGE_BASED_LOGGING
	class DCI_CHANGE_BASED_LOGGING_ENUM {
		public: 
		static const dci_enum_uint8_t ONLY_MANUAL_TRIGGER = 0U;// manual trigger
		static const dci_enum_uint8_t LOG_ON_ANY_CHANGE = 1U;// log on any change
		static const dci_enum_uint8_t LOG_ON_ALL_CHANGE = 2U;// log on all change
		static const dci_enum_uint8_t LOG_ON_ALL_WRITE = 3U;// log on all write
		static const dci_enum_uint8_t MAX_ENUM  = 3U;
		static const uint32_t TOTAL_ENUMS  = 4U;
	};

	//Enums for DCID DCI_BLE_TEST_SETTING_F
	class DCI_BLE_TEST_SETTING_F_ENUM {
		public: 
		static const dci_enum_uint8_t OPTION1 = 0U;// option 1
		static const dci_enum_uint8_t OPTION2 = 1U;// option 2
		static const dci_enum_uint8_t OPTION3 = 2U;// option 3
		static const dci_enum_uint8_t OPTION4 = 3U;// option 4
		static const dci_enum_uint8_t MAX_ENUM  = 3U;
		static const uint32_t TOTAL_ENUMS  = 4U;
	};

	//Enums for DCID DCI_LOGGING_EVENT_CODE
	class DCI_LOGGING_EVENT_CODE_ENUM {
		public: 
		static const dci_enum_uint8_t NO_EVENT = 0U;// No Event
		static const dci_enum_uint8_t DEVICE_RESTART = 1U;// Device Restart
		static const dci_enum_uint8_t FACTORY_RESET = 2U;// Factory Reset
		static const dci_enum_uint8_t IMAGE_UPGRADE_STARTED = 3U;// Image upgrade started
		static const dci_enum_uint8_t IMAGE_UPGRADE_COMPLETE = 4U;// Image upgrade complete
		static const dci_enum_uint8_t CHECKSUM_FAILURE = 5U;// Checksum failure
		static const dci_enum_uint8_t SIGNATURE_VERIFICATION_FAILURE = 6U;// Signature verification failure
		static const dci_enum_uint8_t VERSION_ROLLBACK_FAILURE = 7U;// Version rollback failure
		static const dci_enum_uint8_t NV_DATA_RESTORE_FAIL = 8U;// NV data restore fail
		static const dci_enum_uint8_t NV_PARAM_WRITE_FAIL = 9U;// NV param write fail
		static const dci_enum_uint8_t NV_PARAM_ERASE_FAIL = 10U;// NV parameter erase fail
		static const dci_enum_uint8_t NV_PARAM_ERASE_ALL_FAIL = 11U;// NV erase all parameters fail
		static const dci_enum_uint8_t SELF_PASSWORD_CHANGE = 12U;// Self user password change
		static const dci_enum_uint8_t OTHER_USER_PASSWORD_CHANGE = 13U;// Other user password change
		static const dci_enum_uint8_t USER_CREATED = 14U;// User created
		static const dci_enum_uint8_t USER_DELETED = 15U;// User deleted
		static const dci_enum_uint8_t ADMIN_LOGGED_EVENT = 16U;// Admin user logged event
		static const dci_enum_uint8_t USER_LOG_IN = 17U;// User logged in event
		static const dci_enum_uint8_t USER_LOG_OUT = 18U;// User logout
		static const dci_enum_uint8_t USER_LOCKED = 19U;// User Locked event
		static const dci_enum_uint8_t RESET_DEFAULT_ADMIN_USER = 20U;// Reset default admin user
		static const dci_enum_uint8_t RESET_ALL_ACCOUNT_USERS = 21U;// Reset all user accounts
		static const dci_enum_uint8_t DATA_LOG_MEM_CLEAR = 22U;// Data Log Memory Clear
		static const dci_enum_uint8_t EVENT_LOG_MEM_CLEAR = 23U;// Event Log Memory Clear
		static const dci_enum_uint8_t AUDIT_POWER_LOG_MEM_CLEAR = 24U;// Audit-Power Log Memory Clear
		static const dci_enum_uint8_t AUDIT_FW_UPDATE_LOG_MEM_CLEAR = 25U;// Audit-Fw-Update Log Memory Clear
		static const dci_enum_uint8_t AUDIT_USER_LOG_MEM_CLEAR = 26U;// Audit-User Log Memory Clear
		static const dci_enum_uint8_t AUDIT_CONFIG_LOG_MEM_CLEAR = 27U;// Audit-Config Log Memory Clear
		static const dci_enum_uint8_t IOT_FUS_IMAGE_UPGRADE_STARTED = 28U;// Image upgrade started over IOT
		static const dci_enum_uint8_t IOT_FUS_IMAGE_UPGRADE_COMPLETED = 29U;// Image upgrade completed over IOT
		static const dci_enum_uint8_t IOT_FUS_IMAGE_UPGRADE_ABORTED = 30U;// Image upgrade aborted/cancelled over IOT
		static const dci_enum_uint8_t IOT_FUS_FAILED_DUE_TO_BAD_PARAM = 31U;// Image upgrade failed over IOT due to bad parameter
		static const dci_enum_uint8_t IOT_FUS_CHECKSUM_FAILURE = 32U;// Checksum failure over IOT
		static const dci_enum_uint8_t IOT_FUS_SIGNATURE_VERIFICATION_FAILURE = 33U;// Signature verification failure over IOT
		static const dci_enum_uint8_t IOT_FUS_VERSION_ROLLBACK_FAILURE = 34U;// Version rollback failure over IOT
		static const dci_enum_uint8_t IOT_FUS_REJECTED_BY_PRODUCT = 35U;// IOT FUS request rejected on product side
		static const dci_enum_uint8_t TEST_EVENT_101 = 101U;// Test event-101
		static const dci_enum_uint8_t TEST_EVENT_102 = 102U;// Test event-102
		static const dci_enum_uint8_t MAX_ENUM  = 102U;
		static const uint32_t TOTAL_ENUMS  = 38U;
	};

	//Enums for DCID DCI_DEVICE_STATE
	class DCI_DEVICE_STATE_ENUM {
		public: 
		static const dci_enum_uint8_t OPERATIONAL = 0U;// Operational
		static const dci_enum_uint8_t FAULT = 1U;// Fault
		static const dci_enum_uint8_t MAX_ENUM  = 1U;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_NV_MEM_ID
	class DCI_NV_MEM_ID_ENUM {
		public: 
		static const dci_enum_uint8_t FRAM = 0U;// FRAM
		static const dci_enum_uint8_t UNDEFINED = 1U;// Undefined
		static const dci_enum_uint8_t MAX_ENUM  = 1U;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_NV_FAIL_OPERATION
	class DCI_NV_FAIL_OPERATION_ENUM {
		public: 
		static const dci_enum_uint8_t INITIALIZATION = 0U;// Initilization
		static const dci_enum_uint8_t READ_OP = 1U;// Read Parameter
		static const dci_enum_uint8_t READ_CHECKSUM_OP = 2U;// Read Checksum
		static const dci_enum_uint8_t WRITE_OP = 3U;// Write Parameter
		static const dci_enum_uint8_t ERASE_OP = 4U;// Erase Parameter
		static const dci_enum_uint8_t ERASE_ALL_OP = 5U;// Erase All Parameters
		static const dci_enum_uint8_t MAX_ENUM  = 5U;
		static const uint32_t TOTAL_ENUMS  = 6U;
	};

	//Enums for DCID DCI_LOG_PROC_IMAGE_ID
	class DCI_LOG_PROC_IMAGE_ID_ENUM {
		public: 
		static const dci_enum_uint8_t PROC_ID_0_APPLICATION = 0U;// Processor Main and Image APPLICATION
		static const dci_enum_uint8_t PROC_ID_0_WWW = 1U;// Processor Main and Image WEB
		static const dci_enum_uint8_t PROC_ID_0_LANGUAGE = 2U;// Processor Main and Image LANGUAGE
		static const dci_enum_uint8_t MAX_ENUM  = 2U;
		static const uint32_t TOTAL_ENUMS  = 3U;
	};

	//Enums for DCID DCI_MODBUS_TCP_ENABLE
	class DCI_MODBUS_TCP_ENABLE_ENUM {
		public: 
		static const dci_enum_bool_t MODBUS_TCP_DISABLE = 0;// Disable
		static const dci_enum_bool_t MODBUS_TCP_ENABLE = 1;// Enable
		static const dci_enum_bool_t MAX_ENUM  = 1;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_BACNET_IP_ENABLE
	class DCI_BACNET_IP_ENABLE_ENUM {
		public: 
		static const dci_enum_bool_t BACNET_IP_DISABLE = 0;// Disable
		static const dci_enum_bool_t BACNET_IP_ENABLE = 1;// Enable
		static const dci_enum_bool_t MAX_ENUM  = 1;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_LOG_PORT_NUMBER
	class DCI_LOG_PORT_NUMBER_ENUM {
		public: 
		static const dci_enum_uint8_t PORT_UNKNOWN = 0U;// Unknown Port Value
		static const dci_enum_uint8_t PORT_HTTP = 1U;// HTTP Port
		static const dci_enum_uint8_t PORT_HTTPS = 2U;// HTTPS Port
		static const dci_enum_uint8_t PORT_MODBUS_TCP = 3U;// MODBUS TCP Port
		static const dci_enum_uint8_t PORT_BACNET = 4U;// BACNET Port
		static const dci_enum_uint8_t PORT_BLE = 5U;// BLE Port
		static const dci_enum_uint8_t PORT_ETHERNET_IP_TCP = 6U;// Ethernet IP over TCP Port
		static const dci_enum_uint8_t PORT_ETHERNET_IP_UDP = 7U;// Ethernet IP over UDP Port
		static const dci_enum_uint8_t MAX_ENUM  = 7U;
		static const uint32_t TOTAL_ENUMS  = 8U;
	};

	//Enums for DCID DCI_BLE_USER1_ROLE
	class DCI_BLE_USER1_ROLE_ENUM {
		public: 
		static const dci_enum_uint8_t OBSERVER_ROLE = 0U;// observer_role
		static const dci_enum_uint8_t OPERATOR_ROLE = 1U;// operator_role
		static const dci_enum_uint8_t ENGINEER_ROLE = 2U;// engineer_role
		static const dci_enum_uint8_t ADMIN_ROLE = 3U;// admin_role
		static const dci_enum_uint8_t MAX_ENUM  = 3U;
		static const uint32_t TOTAL_ENUMS  = 4U;
	};

	//Enums for DCID DCI_BLE_USER2_ROLE
	class DCI_BLE_USER2_ROLE_ENUM {
		public: 
		static const dci_enum_uint8_t OBSERVER_ROLE = 0U;// observer_role
		static const dci_enum_uint8_t OPERATOR_ROLE = 1U;// operator_role
		static const dci_enum_uint8_t ENGINEER_ROLE = 2U;// engineer_role
		static const dci_enum_uint8_t ADMIN_ROLE = 3U;// adminr_role
		static const dci_enum_uint8_t MAX_ENUM  = 3U;
		static const uint32_t TOTAL_ENUMS  = 4U;
	};

	//Enums for DCID DCI_BLE_USER3_ROLE
	class DCI_BLE_USER3_ROLE_ENUM {
		public: 
		static const dci_enum_uint8_t OBSERVER_ROLE = 0U;// observer_role
		static const dci_enum_uint8_t OPERATOR_ROLE = 1U;// operator_role
		static const dci_enum_uint8_t ENGINEER_ROLE = 2U;// engineer_role
		static const dci_enum_uint8_t ADMIN_ROLE = 3U;// admin_role
		static const dci_enum_uint8_t MAX_ENUM  = 3U;
		static const uint32_t TOTAL_ENUMS  = 4U;
	};

	//Enums for DCID DCI_BLE_USER4_ROLE
	class DCI_BLE_USER4_ROLE_ENUM {
		public: 
		static const dci_enum_uint8_t OBSERVER_ROLE = 0U;// observer_role
		static const dci_enum_uint8_t OPERATOR_ROLE = 1U;// operator_role
		static const dci_enum_uint8_t ENGINEER_ROLE = 2U;// engineer_role
		static const dci_enum_uint8_t ADMIN_ROLE = 3U;// admin_role
		static const dci_enum_uint8_t MAX_ENUM  = 3U;
		static const uint32_t TOTAL_ENUMS  = 4U;
	};

	//Enums for DCID DCI_RESET_SOURCE
	class DCI_RESET_SOURCE_ENUM {
		public: 
		static const dci_enum_uint32_t NONE = 0U;// NONE
		static const dci_enum_uint32_t HARD_FAULT = 1U;// HARD_FAULT
		static const dci_enum_uint32_t WATCHDOG = 2U;// WATCHDOG
		static const dci_enum_uint32_t EXCEPTION = 3U;// EXCEPTION
		static const dci_enum_uint32_t MAX_ENUM  = 3U;
		static const uint32_t TOTAL_ENUMS  = 4U;
	};

	//Enums for DCID DCI_CORS_TYPE
	class DCI_CORS_TYPE_ENUM {
		public: 
		static const dci_enum_uint8_t ALLOW_ALL_DOMAIN = 0U;// (*) - allows all Origin
		static const dci_enum_uint8_t ALLOW_ACTIVE_IP = 1U;// (Active device IP) - allows Origin with device IP
		static const dci_enum_uint8_t MAX_ENUM  = 1U;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_DEVICE_PWD_SETTING
	class DCI_DEVICE_PWD_SETTING_ENUM {
		public: 
		static const dci_enum_uint8_t DCI_COMMON_PWD_CHANGE = 0U;// Device Common Password Change
		static const dci_enum_uint8_t DCI_UNIQUE_PWD_CHANGE = 1U;// Device Unique Password Change
		static const dci_enum_uint8_t DCI_UNIQUE_PWD_NO_CHANGE = 2U;// Device Unique Password No Change
		static const dci_enum_uint8_t MAX_ENUM  = 2U;
		static const uint32_t TOTAL_ENUMS  = 3U;
	};

	//Enums for DCID DCI_LANG_PREF_SETTING
	class DCI_LANG_PREF_SETTING_ENUM {
		public: 
		static const dci_enum_uint8_t USE_DEVICE_STORED_LANG_SETTING = 0U;// Use Device stored language setting
		static const dci_enum_uint8_t USE_DEVICE_STORED_LANG_SETTING_PER_USER = 1U;// Use Device stored language setting per user
		static const dci_enum_uint8_t USE_BROWSER_STORED_LANG_SETTING = 2U;// Use browser stored language setting
		static const dci_enum_uint8_t USE_BROWSER_LOCAL_LANG_SETTING = 3U;// Use browser local language setting
		static const dci_enum_uint8_t MAX_ENUM  = 3U;
		static const uint32_t TOTAL_ENUMS  = 4U;
	};

	//Enums for DCID DCI_PTP_ENABLE
	class DCI_PTP_ENABLE_ENUM {
		public: 
		static const dci_enum_bool_t PTP_DISABLE = 0;// PTP disable
		static const dci_enum_bool_t PTP_ENABLE = 1;// PTP enable
		static const dci_enum_bool_t MAX_ENUM  = 1;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};

	//Enums for DCID DCI_CLIENT_AP_SECURITY
	class DCI_CLIENT_AP_SECURITY_ENUM {
		public: 
		static const dci_enum_int32_t WIFI_STA_SECURITY_OPEN = 0;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_WEP_PSK = 1;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_WEP_SHARED = 2;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_WPA_TKIP_PSK = 3;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_WPA_AES_PSK = 4;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_WPA_MIXED_PSK = 5;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_WPA2_AES_PSK = 6;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_WPA2_TKIP_PSK = 7;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_WPA2_MIXED_PSK = 8;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_WPA2_FBT_PSK = 9;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_WPA_TKIP_ENT = 10;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_WPA_AES_ENT = 11;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_WPA_MIXED_ENT = 12;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_WPA2_TKIP_ENT = 13;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_WPA2_AES_ENT = 14;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_WPA2_MIXED_ENT = 15;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_WPA2_FBT_ENT = 16;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_IBSS_OPEN = 17;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_WPS_OPEN = 18;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_WPS_SECURE = 19;// 
		static const dci_enum_int32_t WIFI_STA_SECURITY_FORCE_32_BIT = 20;// 
		static const dci_enum_int32_t MAX_ENUM  = 20;
		static const uint32_t TOTAL_ENUMS  = 21U;
	};

	//Enums for DCID DCI_CLIENT_AP_BAND
	class DCI_CLIENT_AP_BAND_ENUM {
		public: 
		static const dci_enum_uint8_t WIFI_STA_802_11_BAND_5GHZ = 0U;// 5GHz  support
		static const dci_enum_uint8_t WIFI_STA_802_11_BAND_2_4GHZ = 1U;// 2.4 GHz  support
		static const dci_enum_uint8_t MAX_ENUM  = 1U;
		static const uint32_t TOTAL_ENUMS  = 2U;
	};
#endif
