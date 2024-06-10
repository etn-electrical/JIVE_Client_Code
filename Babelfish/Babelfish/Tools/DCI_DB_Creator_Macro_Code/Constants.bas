Attribute VB_Name = "Constants"
' Constants

'**************************
'**************************
'Param to DCI Construction sheet.
'**************************
'**************************

'***************************************************************
'DCI Tab Indexing.
Public Const DCI_DEFINES_START_ROW = 9
Public Const DCI_DESCRIPTORS_COL = 1
Public Const DCI_DEFINES_COL = 2
Public Const DCI_LENGTH_COL = 6

Public Const DATA_DESCRIPTION_COL = 1
Public Const DCI_DATATYPE_COL = 3
Public Const DATATYPE_SIZE = 4
Public Const DCI_ARRAY_COUNT_COL = 5
Public Const TOTAL_LENGTH_COL = 6
Public Const DATA_DEFAULT_COL = 7
Public Const DATA_MIN_COL = 8
Public Const DATA_MAX_COL = 9
Public Const DATA_ENUM_COL = 10
Public Const DATA_BITFIELD_COL = 11
Public Const DATA_ENUMLIST_COL = 26
Public Const MIN_DATATYPE_SIZE = 8

Public Const READ_ONLY_VAL_ATTRIB_COL = 27
Public Const INITIAL_VAL_ATTRIB_COL = 28
Public Const DEFAULT_VAL_ATTRIB_COL = 29
Public Const RW_DEFAULT_VAL_ATTRIB_COL = 30
Public Const RANGE_ATTRIB_COL = 31
Public Const RW_RANGE_ATTRIB_COL = 32
Public Const CALLBACK_ATTRIB_COL = 33
Public Const RW_LENGTH_ATTRIB_COL = 34
Public Const ENUM_ATTRIB_COL = 35
Public Const APP_ONLY_ATTRIB_COL = 36
Public Const RW_ENUM_ATTRIB_COL = 37

Public Const PATRON_RAM_VAL_WRITEABLE_COL = 53
Public Const PATRON_INIT_VAL_READ_ACCESS_COL = 54
Public Const PATRON_INIT_VAL_WRITEABLE_ACCESS_COL = 55
Public Const PATRON_DEFAULT_VAL_READ_ACCESS_COL = 56
Public Const PATRON_RANGE_VAL_ACCESS_COL = 57
Public Const PATRON_ENUM_VAL_ACCESS_COL = 58

Public Const DCI_SHORT_DESC_COL = 79
Public Const DCI_SHORT_DESC_LEN_COL = 80
Public Const DCI_LONG_DESC_COL = 81
Public Const DCI_LONG_DESC_LEN_COL = 82
Public Const DCI_UNITS_DESC_COL = 83
Public Const DCI_UNITS_DESC_LEN_COL = 84

Public Const DCI_FORMAT_DESC_COL = 87
Public Const DCI_INTERNAL_DESC_COL = 88

Public Const DCI_DESCRIPTORS_ERROR_CHECK_OUTPUT_COL = 105

Public Const DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR = 3
Public Const DCI_DESCRIPTORS_ERROR_CHECK_WARNING_COLOR = 6
Public Const DCI_DESCRIPTORS_ERROR_CHECK_ATTENTION_COLOR = 8

Public Const DCI_DESCRIPTORS_ERROR_CHECK_NV_MEM_SHEET_DELETE = 48
Public Const DCI_DESCRIPTORS_ERROR_CHECK_NV_MEM_SHEET_CHECKSUM_DELETE = 16

Public Const DCI_DESCRIPTORS_LENGTH_COL_COLOR = 36


'***************************************************************
'Datatype defines
Public Const DCI_DESCRIPTORS_DTYPE_BOOL_STRING = "DCI_DTYPE_BOOL"
Public Const DCI_DESCRIPTORS_DTYPE_SINT8_STRING = "DCI_DTYPE_SINT8"
Public Const DCI_DESCRIPTORS_DTYPE_SINT16_STRING = "DCI_DTYPE_SINT16"
Public Const DCI_DESCRIPTORS_DTYPE_SINT32_STRING = "DCI_DTYPE_SINT32"
Public Const DCI_DESCRIPTORS_DTYPE_SINT64_STRING = "DCI_DTYPE_SINT64"
Public Const DCI_DESCRIPTORS_DTYPE_UINT8_STRING = "DCI_DTYPE_UINT8"
Public Const DCI_DESCRIPTORS_DTYPE_UINT16_STRING = "DCI_DTYPE_UINT16"
Public Const DCI_DESCRIPTORS_DTYPE_UINT32_STRING = "DCI_DTYPE_UINT32"
Public Const DCI_DESCRIPTORS_DTYPE_UINT64_STRING = "DCI_DTYPE_UINT64"
Public Const DCI_DESCRIPTORS_DTYPE_FLOAT_STRING = "DCI_DTYPE_FLOAT"
Public Const DCI_DESCRIPTORS_DTYPE_DFLOAT_STRING = "DCI_DTYPE_DFLOAT"
Public Const DCI_DESCRIPTORS_DTYPE_BYTE_STRING = "DCI_DTYPE_BYTE"
Public Const DCI_DESCRIPTORS_DTYPE_WORD_STRING = "DCI_DTYPE_WORD"
Public Const DCI_DESCRIPTORS_DTYPE_DWORD_STRING = "DCI_DTYPE_DWORD"
Public Const DCI_DESCRIPTORS_DTYPE_STRING8_STRING = "DCI_DTYPE_STRING8"


'***************************************************************
'DCOM Server/Virtual Sheet File Indexing.
Public Const PARAMxDCI_SRVR_DATA_START_ROW = 9
Public Const PARAMxDCI_SRVR_DCI_DESCRIPT_COL = 1
Public Const PARAMxDCI_SRVR_DCI_DEFINE_COL = 2
Public Const PARAMxDCI_SRVR_DCID_COL = 3
Public Const PARAMxDCI_SRVR_SUBSCRIPTION_COL = 4
Public Const PARAMxDCI_SRVR_OWNER_PATRON_COL = 5
Public Const PARAMxDCI_SRVR_PARAM_DESCRIPT_COL = 6
Public Const PARAMxDCI_SRVR_PARAM_DEFINE_COL = 7
Public Const PARAMxDCI_SRVR_PARAM_ID_COL = 8
Public Const PARAMxDCI_SRVR_DCID_SORT_KEY_COL = 9
Public Const PARAMxDCI_SRVR_MAX_COL = 10
Public Const PARAMxDCI_SRVR_ERROR_COL = 11
Public Const PARAMxDCI_SRVR_WARN_COL = 12
Public Const PARAMxDCI_SRVR_MAX_ROW = 50000

Public Const PARAMxDCI_VIRT_DATA_START_ROW = 9
Public Const PARAMxDCI_VIRT_PARAM_DESCRIPT_COL = 1
Public Const PARAMxDCI_VIRT_PARAM_DEFINE_COL = 2
Public Const PARAMxDCI_VIRT_PARAM_ID_COL = 3
Public Const PARAMxDCI_VIRT_SUBSCRIPTION_COL = 4
Public Const PARAMxDCI_VIRT_OWNER_PATRON_COL = 5
Public Const PARAMxDCI_VIRT_SOUCE_OF_TRUTH_COL = 6
Public Const PARAMxDCI_VIRT_DCI_DESCRIPT_COL = 7
Public Const PARAMxDCI_VIRT_DCI_DEFINE_COL = 8
Public Const PARAMxDCI_VIRT_DCID_COL = 9
Public Const PARAMxDCI_VIRT_PARAM_ID_SORT_KEY_COL = 10
Public Const PARAMxDCI_VIRT_MAX_COL = 11
Public Const PARAMxDCI_VIRT_ERROR_COL = 13
Public Const PARAMxDCI_VIRT_WARN_COL = 14
Public Const PARAMxDCI_VIRT_MAX_ROW = 50000

'Public Const PARAMxDCI_MISSING_DCID_DESCRIPT_COL = 1
'Public Const PARAMxDCI_MISSING_DCID_COL = 2
Public Const PARAMxDCI_UNUSED_COL = 52

'***************************************************************
'NV Memory Tab Indexing.
Public Const NV_MEM_DATA_START_ROW = 7
Public Const NV_DEFINES_COL = 1
Public Const NV_ADDRESS_COL = 2
Public Const NV_MEMORY_COL_OFFSET = 5   ' The distance between multiple NV memories.
Public Const NV_MEMORY_DESCRIPTOR_ROW = 6 'The place where the memory is described
Public Const NV_MEMORY_DESCRIPTOR_COL_START = 1 ' The place where the descriptors start
Public Const NV_MEMORY_MAX_MEMORIES = 4 'If we go past this number we end up in the missing def column.
Public Const NV_MEMORY_MAX_COLUMN = (NV_MEMORY_MAX_MEMORIES * NV_MEMORY_COL_OFFSET) + NV_MEMORY_COL_OFFSET
Public Const NV_MEMORY_LAST_POSSIBLE_ADDRESS_ROW = 32768

Public Const NV_MISSING_START_ROW = 7
Public Const NV_MISSING_COL = 27

Public Const NV_MEM_CHECKSUM_STRING = "CHECKSUM"
Public Const NV_MEM_DEFINE_END_STRING_PART = "NV_DATA_END_NVCTRL"
Public Const NV_MEM_DEFINE_START_STRING_PART = "NV_DATA_START_NVCTRL"
Public Const NV_CRYPTO_META_DATA_END_STRING_PART = "NV_CRYPTO_META_DATA_END_NVCTRL"
Public Const NV_MEM_DCI_COVERED_PARAM_COLOR = 8
Public Const NV_MEM_DCI_COVERED_CHECKSUM_COLOR = 3
Public Const NV_MEM_NON_DCI_COVERED_PARAM_COLOR = 4
Public Const NV_MEM_NON_DCI_COVERED_CHECKSUM_COLOR = 3
Public Const NV_MEM_DCI_LENGTH_MISMATCH_CHECKSUM_COLOR = 6



'***************************************************************
'Parameter ID Tab Indexing.
Public Const PID_DEFINES_START_ROW = 9
Public Const PID_DESCRIPTORS_COL = 1
Public Const PID_DEFINES_COL = 2
Public Const PID_ID_COL = 3
Public Const PID_LENGTH_COL = 11

'***************************************************************
' Documentation path row and column from Usage Notes sheet.
Public Const GENERATED_DOCS_PATH_ROW = 26
Public Const GENERATED_DOCS_PATH_COL = 2
Public Const GENERATED_DOCS_DATA_NAMESPACE_ROW = 25
Public Const GENERATED_DOCS_DATA_NAMESPACE_COL = 2
Public Const FLOAT_PRECISION_DISPLAY_ROW = 28
Public Const FLOAT_PRECISION_DISPLAY_COL = 2



