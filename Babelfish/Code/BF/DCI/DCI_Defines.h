/**
 *****************************************************************************************
 *	@file
 *
 *	@brief
 *
 *	@details
 *
 *	@copyright 2014 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef DCI_DEFINES_H
#define DCI_DEFINES_H

// DCI = Data Center Interface
// DA = Data_Access

// TD = typedef
// ERR = error
// CB = Callback
// MSK = Mask
// ST = Struct
// DV = Data Value

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */

/*
 *****************************************************************************************
 *		Typedefs
 *****************************************************************************************
 */

/*
 *********************************************************
 *		Function Returns
 *********************************************************
 */
enum DCI_ERROR_TD
{
	DCI_ERR_NO_ERROR,
	DCI_ERR_INVALID_COMMAND,		///< Invalid command was requested.
	DCI_ERR_INVALID_DATA_ID,		///< Data ID does not exist.
	DCI_ERR_ACCESS_VIOLATION,		///< Target data does not exist.  Null ram ptr.
	DCI_ERR_EXCEEDS_RANGE,			///< The set value resulted in a range error.
	DCI_ERR_RAM_READ_ONLY,			///< This value is read only.  No write access is allowed.
	DCI_ERR_NV_NOT_AVAILABLE,		///< NV Value does not exist or is inaccessible.
	DCI_ERR_NV_ACCESS_FAILURE,		///< NV is Busy or Broken.
	DCI_ERR_NV_READ_ONLY,			///< The NV value is listed as read only.
	DCI_ERR_DEFAULT_NOT_AVAILABLE,	///< The Default is not available.
	DCI_ERR_DEFAULT_NOT_WRITABLE,	///< The Default value is not writable. (pertains to special access)
	DCI_ERR_RANGE_NOT_AVAILABLE,	///< The Range is not available.
	DCI_ERR_RANGE_NOT_WRITABLE,		///< The Range value is not writable. (pertains to special access)
	DCI_ERR_ENUM_NOT_AVAILABLE,		///< The Enum is not available.
	DCI_ERR_ENUM_NOT_WRITABLE,		///< The Enum value is not writable. (pertains to special access)
	DCI_ERR_INVALID_DATA_LENGTH,	///< The data length is invalid.
	DCI_ERR_DATA_LENGTH_NOT_WRITABLE,	///< The length value is not writable. (pertains to special access)
	DCI_ERR_VALUE_LOCKED,///< The data value has been locked and is not accessible right now.  Please come back later.
	DCI_ERR_DATA_INCORRECT,			///< The data received is incorrect
	DCI_ERR_TOTAL_ERRORS
};

/*
 *********************************************************
 *		Attribute Definitions
 *********************************************************
 */
typedef uint8_t dci_bool_t;
static const dci_bool_t DCI_BOOL_TRUE = 1U;
static const dci_bool_t DCI_BOOL_FALSE = 0U;

typedef uint16_t DCI_ID_TD;
typedef uint8_t DCI_LOCK_TYPE_TD;

typedef uint8_t DCI_RAM_DATA_TD;
typedef uint32_t DCI_NV_LOC_TD;
typedef DCI_RAM_DATA_TD DCI_DEF_DATA_TD;
typedef DCI_RAM_DATA_TD DCI_RANGE_DATA_TD;
typedef uint16_t DCI_LENGTH_TD;
typedef DCI_RAM_DATA_TD DCI_ENUM_LIST_TD;
typedef uint16_t DCI_ENUM_COUNT_TD;
typedef uint16_t DCI_ENUM_INDEX_TD;
typedef uint16_t DCI_CBACK_INDEX_TD;
typedef uint16_t DCI_RANGE_DATA_INDEX_TD;

// typedef uint8_t DCI_DATATYPE_TD;
enum DCI_DATATYPE_TD
{
	DCI_DTYPE_BOOL,
	DCI_DTYPE_UINT8,
	DCI_DTYPE_SINT8,
	DCI_DTYPE_UINT16,
	DCI_DTYPE_SINT16,
	DCI_DTYPE_UINT32,
	DCI_DTYPE_SINT32,
	DCI_DTYPE_UINT64,
	DCI_DTYPE_SINT64,
	DCI_DTYPE_FLOAT,
	DCI_DTYPE_LFLOAT,
	DCI_DTYPE_DFLOAT,
	DCI_DTYPE_BYTE,
	DCI_DTYPE_WORD,
	DCI_DTYPE_DWORD,
	DCI_DTYPE_STRING8,
	DCI_DTYPE_MAX_TYPES
};

typedef uint8_t DCI_DATATYPE_SIZE_TD;

static const DCI_DATATYPE_SIZE_TD DCI_DTYPE_BOOL_SIZE = 1U;
static const DCI_DATATYPE_SIZE_TD DCI_DTYPE_UINT8_SIZE = 1U;
static const DCI_DATATYPE_SIZE_TD DCI_DTYPE_SINT8_SIZE = 1U;
static const DCI_DATATYPE_SIZE_TD DCI_DTYPE_UINT16_SIZE = 2U;
static const DCI_DATATYPE_SIZE_TD DCI_DTYPE_SINT16_SIZE = 2U;
static const DCI_DATATYPE_SIZE_TD DCI_DTYPE_UINT32_SIZE = 4U;
static const DCI_DATATYPE_SIZE_TD DCI_DTYPE_SINT32_SIZE = 4U;
static const DCI_DATATYPE_SIZE_TD DCI_DTYPE_UINT64_SIZE = 8U;
static const DCI_DATATYPE_SIZE_TD DCI_DTYPE_SINT64_SIZE = 8U;
static const DCI_DATATYPE_SIZE_TD DCI_DTYPE_FLOAT_SIZE = 4U;
static const DCI_DATATYPE_SIZE_TD DCI_DTYPE_LFLOAT_SIZE = 10U;
static const DCI_DATATYPE_SIZE_TD DCI_DTYPE_DFLOAT_SIZE = 8U;
static const DCI_DATATYPE_SIZE_TD DCI_DTYPE_BYTE_SIZE = 1U;
static const DCI_DATATYPE_SIZE_TD DCI_DTYPE_WORD_SIZE = 2U;
static const DCI_DATATYPE_SIZE_TD DCI_DTYPE_DWORD_SIZE = 4U;
static const DCI_DATATYPE_SIZE_TD DCI_DTYPE_STRING8_SIZE = 1U;



/**
 * @brief The following represents a bitfield as well as the bits associated with it.
 * They represent the primary owner data type.
 */
typedef uint16_t DCI_OWNER_ATTRIB_TD;
static const uint8_t DCI_OWNER_ATTRIB_INFO_READ_ONLY = 0U;
static const uint8_t DCI_OWNER_ATTRIB_INFO_INIT_VAL = 1U;
static const uint8_t DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL = 2U;
static const uint8_t DCI_OWNER_ATTRIB_INFO_RW_DEFAULT_VAL = 3U;
static const uint8_t DCI_OWNER_ATTRIB_INFO_RANGE_VAL = 4U;
static const uint8_t DCI_OWNER_ATTRIB_INFO_RW_RANGE_VAL = 5U;
static const uint8_t DCI_OWNER_ATTRIB_INFO_ENUM_VAL = 6U;
static const uint8_t DCI_OWNER_ATTRIB_INFO_RW_ENUM_VAL = 7U;
static const uint8_t DCI_OWNER_ATTRIB_INFO_CALLBACK = 8U;
static const uint8_t DCI_OWNER_ATTRIB_INFO_RW_LENGTH = 9U;
static const uint8_t DCI_OWNER_ATTRIB_INFO_APP_PARAM = 10U;	///< This is a parameter associated with application
															///< specific functions not protocols or patrons.
static const uint8_t DCI_OWNER_ATTRIB_INFO_MAX = 11U;

static const DCI_OWNER_ATTRIB_TD DCI_OWNER_ATTRIB_INFO_READ_ONLY_MASK =
	static_cast<DCI_OWNER_ATTRIB_TD>( 1U << DCI_OWNER_ATTRIB_INFO_READ_ONLY );
static const DCI_OWNER_ATTRIB_TD DCI_OWNER_ATTRIB_INFO_INIT_VAL_MASK =
	static_cast<DCI_OWNER_ATTRIB_TD>( 1U << DCI_OWNER_ATTRIB_INFO_INIT_VAL );
static const DCI_OWNER_ATTRIB_TD DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL_MASK =
	static_cast<DCI_OWNER_ATTRIB_TD>( 1U << DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL );
static const DCI_OWNER_ATTRIB_TD DCI_OWNER_ATTRIB_INFO_RW_DEFAULT_VAL_MASK =
	static_cast<DCI_OWNER_ATTRIB_TD>( 1U << DCI_OWNER_ATTRIB_INFO_RW_DEFAULT_VAL );
static const DCI_OWNER_ATTRIB_TD DCI_OWNER_ATTRIB_INFO_RANGE_VAL_MASK =
	static_cast<DCI_OWNER_ATTRIB_TD>( 1U << DCI_OWNER_ATTRIB_INFO_RANGE_VAL );
static const DCI_OWNER_ATTRIB_TD DCI_OWNER_ATTRIB_INFO_RW_RANGE_VAL_MASK =
	static_cast<DCI_OWNER_ATTRIB_TD>( 1U << DCI_OWNER_ATTRIB_INFO_RW_RANGE_VAL );
static const DCI_OWNER_ATTRIB_TD DCI_OWNER_ATTRIB_INFO_ENUM_VAL_MASK =
	static_cast<DCI_OWNER_ATTRIB_TD>( 1U << DCI_OWNER_ATTRIB_INFO_ENUM_VAL );
static const DCI_OWNER_ATTRIB_TD DCI_OWNER_ATTRIB_INFO_RW_ENUM_VAL_MASK =
	static_cast<DCI_OWNER_ATTRIB_TD>( 1U << DCI_OWNER_ATTRIB_INFO_RW_ENUM_VAL );
static const DCI_OWNER_ATTRIB_TD DCI_OWNER_ATTRIB_INFO_CALLBACK_MASK =
	static_cast<DCI_OWNER_ATTRIB_TD>( 1U << DCI_OWNER_ATTRIB_INFO_CALLBACK );
static const DCI_OWNER_ATTRIB_TD DCI_OWNER_ATTRIB_INFO_RW_LENGTH_MASK =
	static_cast<DCI_OWNER_ATTRIB_TD>( 1U << DCI_OWNER_ATTRIB_INFO_RW_LENGTH );
static const DCI_OWNER_ATTRIB_TD DCI_OWNER_ATTRIB_INFO_APP_PARAM_MASK =
	static_cast<DCI_OWNER_ATTRIB_TD>( 1U << DCI_OWNER_ATTRIB_INFO_APP_PARAM );	///< This is a parameter associated with
																				///< application specific functions not
																				///< protocols or patrons.

/**
 * @brief The following represents a bitfield as well as the bits associated with it.
 * They represent the patron access behaviors allowed.
 */
typedef uint16_t DCI_PATRON_ATTRIB_TD;
static const uint8_t DCI_PATRON_ATTRIB_RAM_WR_DATA = 0U;
static const uint8_t DCI_PATRON_ATTRIB_NVMEM_DATA = 1U;
static const uint8_t DCI_PATRON_ATTRIB_NVMEM_WR_DATA = 2U;
static const uint8_t DCI_PATRON_ATTRIB_DEFAULT_DATA = 3U;
static const uint8_t DCI_PATRON_ATTRIB_RANGE_DATA = 4U;
static const uint8_t DCI_PATRON_ATTRIB_ENUM_DATA = 5U;
static const uint8_t DCI_PATRON_ATTRIB_OWNER_CALLBACK = 6U;
static const uint8_t DCI_PATRON_MAX_ATTRIBS = 7U;

static const DCI_PATRON_ATTRIB_TD DCI_PATRON_ATTRIB_RAM_WR_DATA_MASK =
	static_cast<DCI_PATRON_ATTRIB_TD>( 1U << DCI_PATRON_ATTRIB_RAM_WR_DATA );
static const DCI_PATRON_ATTRIB_TD DCI_PATRON_ATTRIB_NVMEM_DATA_MASK =
	static_cast<DCI_PATRON_ATTRIB_TD>( 1U << DCI_PATRON_ATTRIB_NVMEM_DATA );
static const DCI_PATRON_ATTRIB_TD DCI_PATRON_ATTRIB_NVMEM_WR_DATA_MASK =
	static_cast<DCI_PATRON_ATTRIB_TD>( 1U << DCI_PATRON_ATTRIB_NVMEM_WR_DATA );
static const DCI_PATRON_ATTRIB_TD DCI_PATRON_ATTRIB_DEFAULT_DATA_MASK =
	static_cast<DCI_PATRON_ATTRIB_TD>( 1U << DCI_PATRON_ATTRIB_DEFAULT_DATA );
static const DCI_PATRON_ATTRIB_TD DCI_PATRON_ATTRIB_RANGE_DATA_MASK =
	static_cast<DCI_PATRON_ATTRIB_TD>( 1U << DCI_PATRON_ATTRIB_RANGE_DATA );
static const DCI_PATRON_ATTRIB_TD DCI_PATRON_ATTRIB_ENUM_DATA_MASK =
	static_cast<DCI_PATRON_ATTRIB_TD>( 1U << DCI_PATRON_ATTRIB_ENUM_DATA );
static const DCI_PATRON_ATTRIB_TD DCI_PATRON_ATTRIB_OWNER_CALLBACK_MASK =
	static_cast<DCI_PATRON_ATTRIB_TD>( 1U << DCI_PATRON_ATTRIB_OWNER_CALLBACK );

/*
 *********************************************************
 *		Access Struct
 *********************************************************
 */

static const DCI_LENGTH_TD ACCESS_OFFSET_NONE = 0U;		// Suggests no offset is needed.
static const DCI_LENGTH_TD ACCESS_LENGTH_GET_ALL = 0U;	// Indicates that we want to work on the full length of a
														// parameter.
static const DCI_LENGTH_TD ACCESS_LENGTH_SET_ALL = 0U;	// Indicates that we want to work on the full length of a
														// parameter.

typedef uint8_t DCI_SOURCE_ID_TD;

enum DCI_ACCESS_CMD_TD
{
	DCI_ACCESS_GET_RAM_CMD,
	DCI_ACCESS_SET_RAM_CMD,
	DCI_ACCESS_GET_INIT_CMD,
	DCI_ACCESS_SET_INIT_CMD,
	DCI_ACCESS_GET_DEFAULT_CMD,
	DCI_ACCESS_SET_DEFAULT_CMD,
	DCI_ACCESS_GET_LENGTH_CMD,
	DCI_ACCESS_SET_LENGTH_CMD,
	DCI_ACCESS_GET_MIN_CMD,
	DCI_ACCESS_SET_MIN_CMD,
	DCI_ACCESS_GET_MAX_CMD,
	DCI_ACCESS_SET_MAX_CMD,
	DCI_ACCESS_GET_ENUM_LIST_LEN_CMD,	// This is the enum list length in bytes.
	DCI_ACCESS_SET_ENUM_LIST_LEN_CMD,	// This is the enum list length in bytes.
	DCI_ACCESS_GET_ENUM_CMD,// Any segment of the enum list can be returned using the access struct.
	DCI_ACCESS_SET_ENUM_CMD,// Any segment of the enum list can be returned using the access struct.
	DCI_ACCESS_GET_ATTRIB_SUPPORT_CMD,
	DCI_ACCESS_RAM_TO_INIT_CMD,
	DCI_ACCESS_INIT_TO_RAM_CMD,
	DCI_ACCESS_DEFAULT_TO_INIT_CMD,
	DCI_ACCESS_DEFAULT_TO_RAM_CMD,
	DCI_ACCESS_POST_SET_RAM_CMD,///< This command is passed in the callback after the set has happened.
	DCI_ACCESS_POST_SET_INIT_CMD,	///< This command is passed in the callback after the set has happened.
	DCI_ACCESS_CLEAR_RAM_CMD,
	DCI_ACCESS_UNDEFINED_COMMAND
};

typedef uint32_t DCI_CB_MSK_TD;

static const DCI_CB_MSK_TD DCI_ACCESS_GET_RAM_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_GET_RAM_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_SET_RAM_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_SET_RAM_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_GET_INIT_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_GET_INIT_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_SET_INIT_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_SET_INIT_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_GET_DEFAULT_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_GET_DEFAULT_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_SET_DEFAULT_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_SET_DEFAULT_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_GET_LENGTH_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_GET_LENGTH_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_SET_LENGTH_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_SET_LENGTH_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_GET_MIN_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_GET_MIN_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_SET_MIN_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_SET_MIN_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_GET_MAX_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_GET_MAX_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_SET_MAX_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_SET_MAX_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_GET_ENUM_LIST_LEN_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_GET_ENUM_LIST_LEN_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_SET_ENUM_LIST_LEN_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_SET_ENUM_LIST_LEN_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_GET_ENUM_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_GET_ENUM_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_SET_ENUM_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_SET_ENUM_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_GET_ATTRIB_SUPPORT_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_GET_ATTRIB_SUPPORT_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_RAM_TO_INIT_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_RAM_TO_INIT_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_INIT_TO_RAM_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_INIT_TO_RAM_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_DEFAULT_TO_INIT_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_DEFAULT_TO_INIT_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_DEFAULT_TO_RAM_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_DEFAULT_TO_RAM_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_POST_SET_RAM_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_POST_SET_RAM_CMD );
static const DCI_CB_MSK_TD DCI_ACCESS_POST_SET_INIT_CMD_MSK =
	static_cast<DCI_CB_MSK_TD>( 1U << DCI_ACCESS_POST_SET_INIT_CMD );

typedef void DCI_DATA_PASS_TD;

typedef struct DCI_ACCESS_DATA_TD
{
	DCI_DATA_PASS_TD* data;			///< Dest or Source data string ptr
	DCI_LENGTH_TD offset;			///< Byte offset
	DCI_LENGTH_TD length;			///< Byte length
} DCI_ACCESS_DATA_TD;

typedef struct DCI_ACCESS_ST_TD
{
	DCI_SOURCE_ID_TD source_id;		///< Source ID
	DCI_ID_TD data_id;		///< DCI ID
	DCI_ACCESS_CMD_TD command;		///< Access Command defined by DCI_ACCESS_DATA_TD

	DCI_ACCESS_DATA_TD data_access;		///< Access data defined by DCI_ACCESS_DATA_TD
} DCI_ACCESS_ST_TD;

/*
 *****************************************************************************************
 * See header file for function definition.
 *****************************************************************************************
 */
inline static DCI_CB_MSK_TD DCI_Access_Cmd_Mask( DCI_ACCESS_CMD_TD cmd )
{
	DCI_CB_MSK_TD cmd_mask = 1;

	return ( static_cast<DCI_CB_MSK_TD>( cmd_mask << static_cast<uint8_t>( cmd ) ) );
}

/*
 *********************************************************
 *		Callbacks
 *********************************************************
 */
typedef uint16_t DCI_CB_RET_TD;

static const DCI_CB_RET_TD DCI_CBACK_RET_PROCESS_NORMALLY = 0U;
static const DCI_CB_RET_TD DCI_CBACK_RET_NO_FURTHER_PROCESSING = 1U;
static const DCI_CB_RET_TD DCI_CBACK_RET_NO_FURTHER_PROC_VAL_CHANGE = 2U;
static const DCI_CB_RET_TD DCI_CBACK_RET_NO_FURTHER_PROC_NO_VAL_CHANGE = 3U;
static const DCI_CB_RET_TD DCI_CBACK_RET_ERROR_ENCODED =
	static_cast<DCI_CB_RET_TD>( 1U << ( ( sizeof( DCI_CB_RET_TD ) * 8U ) - 1U ) );

typedef void* DCI_CBACK_PARAM_TD;
typedef DCI_CB_RET_TD DCI_CBACK_FUNC_TD ( DCI_CBACK_PARAM_TD param,
										  DCI_ACCESS_ST_TD* access_struct );


/**
 * @brief Shall take the standard DCI error and combine it with the Callback Error encoded
 * return.
 * @description A DCI Owner callback can respond to the Patron interface with information on
 * how to proceed after the callback.  These returns generally include the following behaviors:
 * @li Do no further processing on the access.
 * @li Process normally.
 * @li Return the indicated error code.
 * @n@b In the case where an error must be returned the owner must encode the error in the
 * response.  This function shall avoid the need for the owner to manually encode the error in the
 * response parameter.
 * @param error: The DCI Error to encode and return to the Patron Access Function.
 * @return The combined DCI_CBACK_RET_ERROR_ENCODED and DCI error code.
 */
inline DCI_CB_RET_TD DCI_CBack_Encode_Error( DCI_ERROR_TD error )
{
	DCI_CB_RET_TD wrapped_error = DCI_CBACK_RET_ERROR_ENCODED;

	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule: 5-2-7, 5-2-8, 5-2-9
	 * @n PCLint: Note 930: Cast from enum to unsigned short.
	 * @n Justification: DCI Defines will guarantee that the enum will not exceed the callback
	 * max size.
	 */
	/*lint -e{930} */
	wrapped_error |= static_cast<DCI_CB_RET_TD>( error );

	return ( wrapped_error );
}

/**
 * @brief Shall take a callback error where a DCI error is encoded and decode it back to a
 * DCI error.
 * @description A DCI Owner callback can respond to the interface with information on
 * how to proceed after the callback.  These returns generally include the following behaviors:
 * @li Do no further processing on the access.
 * @li Process normally.
 * @li Return the indicated error code.
 * @n@b In the case where an error must be returned the owner must encode the error in the
 * response.  This function shall avoid the need for the owner to manually encode the error in the
 * response parameter.
 * @param cback_error: The callback error with the encoded error.
 * @return The DCI error which was encoded.
 */
inline DCI_ERROR_TD DCI_CBack_Decode_Error( DCI_CB_RET_TD cback_error )
{
	DCI_ERROR_TD dci_error;

	/**
	 * @remark Coding Standard Deviation:
	 * @n MISRA-C++[2008] Rule: 5-2-7, 5-2-8, 5-2-9
	 * @n PCLint: Note 930: Cast from enum to unsigned short.
	 * @n Justification: Type safe is important.  To maintain that barrier and avoid
	 * common static casts we provide this service.
	 */
	/*lint -e{930} */
	dci_error = static_cast<DCI_ERROR_TD>( cback_error &
										   static_cast<DCI_CB_RET_TD>( ~DCI_CBACK_RET_ERROR_ENCODED ) );

	return ( dci_error );
}

/*
 *********************************************************
 *		Data Block Define
 *********************************************************
 */

typedef struct DCI_CBACK_BLOCK_TD
{
	DCI_CBACK_FUNC_TD* cback;
	DCI_CBACK_PARAM_TD cback_param;
	DCI_CB_MSK_TD cback_mask;			// The mask uses the bit shifted DCI Access commands.
} DCI_CBACK_BLOCK_TD;

typedef struct DCI_ENUM_BLOCK
{
	DCI_ENUM_LIST_TD const* enum_list;
} DCI_ENUM_BLOCK;

typedef struct DCI_RANGE_BLOCK
{
	DCI_RANGE_DATA_TD const* min;
	DCI_RANGE_DATA_TD const* max;
	DCI_ENUM_INDEX_TD enum_block_index;
} DCI_RANGE_BLOCK;

typedef struct DCI_DEFAULT_BLOCK
{
	DCI_DEF_DATA_TD const* def;
} DCI_DEFAULT_BLOCK;

typedef struct DCI_DATA_BLOCK_TD
{
	DCI_NV_LOC_TD nv_loc;
	DCI_RAM_DATA_TD* ram_ptr;			// ptr to ram;
	DCI_LENGTH_TD const* length_ptr;
	DCI_DEF_DATA_TD const* default_val;
	DCI_ID_TD dcid;
	DCI_OWNER_ATTRIB_TD owner_attrib_info;
	DCI_PATRON_ATTRIB_TD patron_attrib_info;
	DCI_CBACK_INDEX_TD cback_block_index;
	DCI_RANGE_DATA_INDEX_TD range_block_index;
	DCI_DATATYPE_TD datatype;
} DCI_DATA_BLOCK_TD;

static const DCI_ID_TD DCI_ID_UNDEFINED =
	static_cast<DCI_ID_TD>( ~static_cast<DCI_ID_TD>( 0U ) );

#endif
