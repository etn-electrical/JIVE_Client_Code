/**
 *****************************************************************************************
 *  @file IEC61850 stack integration defines file
 *	@brief It provides the interface between user application and the IEC61850 protocol stack with DCI database
 *
 *	@copyright 2020 Eaton Corporation. All Rights Reserved.
 *
 *****************************************************************************************
 */
#ifndef IEC61850_DEFINES_H
   #define IEC61850_DEFINES_H

/*
 *****************************************************************************************
 *		Constants
 *****************************************************************************************
 */
#define MAX_RAM_ALLOCATION      256		///< Max ram size allocation


/*
 *****************************************************************************************
 *		Defines
 *****************************************************************************************
 */
// #define GOOSE_DCI_ENABLE ///< Goose configuration setting used in DCI Sheet >
// #define SAV_DCI_ENABLE	///< Sav configuration setting used in DCI Sheet >

/*
 *********************************************************
 *		Typedefs
 *********************************************************
 */
typedef uint8_t IEC61850_bool_t;
typedef uint8_t IEC61850_DATATYPE_SIZE_TD;

/*
 *********************************************************
 *		static variables
 *********************************************************
 */
static const IEC61850_bool_t IEC61850_BOOL_TRUE = 1U;
static const IEC61850_bool_t IEC61850_BOOL_FALSE = 0U;
static const IEC61850_DATATYPE_SIZE_TD IEC61850_DTYPE_BOOL_SIZE = 1U;
static const IEC61850_DATATYPE_SIZE_TD IEC61850_DTYPE_UINT8_SIZE = 1U;
static const IEC61850_DATATYPE_SIZE_TD IEC61850_DTYPE_SINT8_SIZE = 1U;
static const IEC61850_DATATYPE_SIZE_TD IEC61850_DTYPE_UINT16_SIZE = 2U;
static const IEC61850_DATATYPE_SIZE_TD IEC61850_DTYPE_SINT16_SIZE = 2U;
static const IEC61850_DATATYPE_SIZE_TD IEC61850_DTYPE_UINT32_SIZE = 4U;
static const IEC61850_DATATYPE_SIZE_TD IEC61850_DTYPE_SINT32_SIZE = 4U;
static const IEC61850_DATATYPE_SIZE_TD IEC61850_DTYPE_UINT64_SIZE = 8U;
static const IEC61850_DATATYPE_SIZE_TD IEC61850_DTYPE_SINT64_SIZE = 8U;
static const IEC61850_DATATYPE_SIZE_TD IEC61850_DTYPE_FLOAT_SIZE = 4U;
static const IEC61850_DATATYPE_SIZE_TD IEC61850_DTYPE_LFLOAT_SIZE = 10U;
static const IEC61850_DATATYPE_SIZE_TD IEC61850_DTYPE_DFLOAT_SIZE = 8U;
static const IEC61850_DATATYPE_SIZE_TD IEC61850_DTYPE_BYTE_SIZE = 1U;
static const IEC61850_DATATYPE_SIZE_TD IEC61850_DTYPE_WORD_SIZE = 2U;
static const IEC61850_DATATYPE_SIZE_TD IEC61850_DTYPE_DWORD_SIZE = 4U;
static const IEC61850_DATATYPE_SIZE_TD IEC61850_DTYPE_STRING8_SIZE = 1U;

/**
 * @brief GOOSE error types details
 */
enum iec61850_error_t
{
	IEC61850_ERR_NO_ERROR,
	IEC61850_ERR_MTYPE_INVALID,			///< message type is invalid.
	IEC61850_ERR_MEMORY_ALLOC,			///< Error in memory allocation
	IEC61850_ERR_DCI_ERR,				///< Error in DCI access
	IEC61850_ERR_TOTAL_ERRORS
};

/**
 * @brief GOOSE tag types details
 */
enum iec61850_tagtype_t
{
	TAG_TYPE_BOOL,
	TAG_TYPE_UINT8,
	TAG_TYPE_SINT8,
	TAG_TYPE_UINT16,
	TAG_TYPE_SINT16,
	TAG_TYPE_UINT32,
	TAG_TYPE_SINT32,
	TAG_TYPE_UINT64,
	TAG_TYPE_SINT64,
	TAG_TYPE_FLOAT,
	TAG_TYPE_LFLOAT,
	TAG_TYPE_DFLOAT,
	TAG_TYPE_BYTE,
	TAG_TYPE_WORD,
	TAG_TYPE_DWORD,
	TAG_TYPE_STRING8,
	TAG_TYPE_Q,
	TAG_TYPE_T,
	IEC61850_TAG_TYPE_MAX
};

/**
 * @brief GOOSE message types details
 */
enum iec61850_message_type_t
{
	IEC61850_MTYPE_PUBLISH,
	IEC61850_MTYPE_SUBSCRIBE,
	IEC61850_MTYPE_MAX_TYPES
};

/**
 * @brief GOOSE message types details
 */
enum iec61850_goose_state_t
{
	GOOSE_STATE_INIT,
	GOOSE_STATE_STOP,
	GOOSE_STATE_START,
	GOOSE_STATE_RUN
};

/**
 * @brief SAV message types details
 */
enum iec61850_sav_state_t
{
	SAV_STATE_INIT,
	SAV_STATE_STOP,
	SAV_STATE_START,
	SAV_STATE_RUN
};

#endif
