Attribute VB_Name = "DCID_Test_Helpers"
'****************************************************************************************************************************
'
'   Test whether given DCID has given cell checked (typically with "x")
'
'****************************************************************************************************************************
Public Function DCID_Is_Box_Checked(row As Long, column As Long) As Boolean
    check_box = Trim(Worksheets("DCI Descriptors").Range("A1").Cells(row, column).text)
    DCID_Is_Box_Checked = UCase(check_box) <> ""
End Function

'****************************************************************************************************************************
'
'   Test whether given DCID is a scalar (i.e. not an array)
'
'****************************************************************************************************************************
Public Function DCID_Is_Scalar(row As Long) As Boolean
    DCID_Is_Scalar = Not DCID_Is_Array(row)
End Function

'****************************************************************************************************************************
'
'   Test whether given DCID is an array
'
'****************************************************************************************************************************
Public Function DCID_Is_Array(row As Long) As Boolean
    DCID_Is_Array = Val(DCID_Lookup_Count(row)) > 1
End Function

'****************************************************************************************************************************
'
'   Test whether given DCID has a default value
'   NOTE: This is not correct - there is more to it than just this check box
'
'****************************************************************************************************************************
Public Function DCID_Has_Default_Value(row As Long) As Boolean
    DCID_Has_Default_Value = DCID_Is_Box_Checked(row, DEFAULT_VAL_ATTRIB_COL)
End Function

'****************************************************************************************************************************
'
'   Test whether given DCID has a non-volatile initial value
'
'****************************************************************************************************************************
Public Function DCID_Has_Init_Value(row As Long) As Boolean
    DCID_Has_Init_Value = DCID_Is_Box_Checked(row, INITIAL_VAL_ATTRIB_COL)
End Function

'****************************************************************************************************************************
'
'   Test whether given DCID has range-checking enabled
'
'****************************************************************************************************************************
Public Function DCID_Has_Range_Checking(row As Long) As Boolean
    DCID_Has_Range_Checking = DCID_Is_Box_Checked(row, RANGE_ATTRIB_COL)
End Function

'****************************************************************************************************************************
'
'   Test whether given DCID is an enumeration
'
'****************************************************************************************************************************
Public Function DCID_Is_Enum(row As Long) As Boolean
    result = False
    data_type = DCID_Lookup_Datatype(row)
    If ((data_type = "DCI_DTYPE_UINT8") Or (data_type = "DCI_DTYPE_UINT16") _
            Or (data_type = "DCI_DTYPE_UINT32") Or (data_type = "DCI_DTYPE_SINT8") _
            Or (data_type = "DCI_DTYPE_SINT16") Or (data_type = "DCI_DTYPE_SINT32")) Then
        enumlist = DCID_Lookup_Enum(row)
        If Len(enumlist) > 0 Then
            result = True
        End If
    End If
    DCID_Is_Enum = result
End Function

'****************************************************************************************************************************
'
'   Test whether given DCID is a bitfield
'
'****************************************************************************************************************************
Public Function DCID_Is_Bitfield(row As Long) As Boolean
    result = False
    data_type = DCID_Lookup_Datatype(row)
    If ((data_type = "DCI_DTYPE_BYTE") Or (data_type = "DCI_DTYPE_WORD") _
            Or (data_type = "DCI_DTYPE_DWORD")) Then
        result = True
    End If
    DCID_Is_Bitfield = result
End Function

'****************************************************************************************************************************
'
'   Test whether given DCID has a bitfield definition
'
'****************************************************************************************************************************
Public Function DCID_Has_Bitfield_Definition(row As Long) As Boolean
    result = False
    If DCID_Is_Bitfield(row) Then
        bitlist = DCID_Lookup_Bitfield(row)
        If Len(bitlist) > 0 Then
            result = True
        End If
    End If
    DCID_Has_Bitfield_Definition = result
End Function

'****************************************************************************************************************************
'
'   Test whether given DCID is "normal" (i.e. not enum or bitfield)
'
'****************************************************************************************************************************
Public Function DCID_Is_Normal(row As Long) As Boolean
    DCID_Is_Normal = Not ((DCID_Is_Enum(row) Or DCID_Is_Bitfield(row)))
End Function

'****************************************************************************************************************************
'
'   Test whether given DCID is writeable
'
'****************************************************************************************************************************
Public Function DCID_Is_Writable(row As Long) As Boolean
    DCID_Is_Writable = DCID_Is_Box_Checked(row, PATRON_RAM_VAL_WRITEABLE_COL)
End Function


'****************************************************************************************************************************
'
'   Test whether given DCID is stored in non-volatile memory
'
'****************************************************************************************************************************
Public Function DCID_Is_NonVolatile(row As Long) As Boolean
    DCID_Is_NonVolatile = DCID_Is_Box_Checked(row, PATRON_INIT_VAL_WRITEABLE_ACCESS_COL)
End Function


'****************************************************************************************************************************
'
'   Test whether given DCID has unpublished checkbox checked
'
'****************************************************************************************************************************
Public Function DCID_Is_Unpublished(row As Long) As Boolean
    DCID_Is_Unpublished = Not DCID_Is_Box_Checked(row, DCI_UNPUBLISHED_CHECKBOX_COL)
End Function


'****************************************************************************************************************************
'
'   Test whether given DCID has units
'
'****************************************************************************************************************************
Public Function DCID_Has_Units(row As Long) As Boolean
    units_str = DCID_Lookup_Units(row)
    DCID_Has_Units = Len(units_str) > 0
End Function


'****************************************************************************************************************************
'
'   Test whether given DCID has Backup Memory checkbox checked
'
'****************************************************************************************************************************
Public Function DCID_Is_Default_Writeable(row As Long) As Boolean
    DCID_Is_Default_Writeable = DCID_Is_Box_Checked(row, RW_DEFAULT_VAL_ATTRIB_COL)
End Function


'****************************************************************************************************************************
'
'   Test whether given DCID has Backup Memory checkbox checked
'
'****************************************************************************************************************************
Public Function DCID_Is_Range_Writeable(row As Long) As Boolean
    DCID_Is_Range_Writeable = DCID_Is_Box_Checked(row, RW_RANGE_ATTRIB_COL)
End Function





