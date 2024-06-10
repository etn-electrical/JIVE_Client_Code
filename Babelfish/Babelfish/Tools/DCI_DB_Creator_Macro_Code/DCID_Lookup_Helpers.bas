Attribute VB_Name = "DCID_Lookup_Helpers"
'****************************************************************************************************************************
'
'   Lookup engineering description field value for given DCID row
'
'****************************************************************************************************************************
Public Function DCID_Lookup_Engrg_Description(row As Long) As String
    DCID_Lookup_Engrg_Description = Trim(Worksheets("DCI Descriptors").Range("A1").Cells(row, DATA_DESCRIPTION_COL).text)
End Function


'****************************************************************************************************************************
'
'   Lookup DCID symbol (code define) field value for given DCID row
'
'****************************************************************************************************************************
Public Function DCID_Lookup_DCID_Symbol(row As Long) As String
    DCID_Lookup_DCID_Symbol = Trim(Worksheets("DCI Descriptors").Range("A1").Cells(row, DCI_DEFINES_COL).text)
End Function


'****************************************************************************************************************************
'
'   Lookup datatype field value for given DCID row
'
'****************************************************************************************************************************
Public Function DCID_Lookup_Datatype(row As Long) As String
    DCID_Lookup_Datatype = Trim(Worksheets("DCI Descriptors").Range("A1").Cells(row, DCI_DATATYPE_COL).text)
End Function


'****************************************************************************************************************************
'
'   Lookup size of each element field value for given DCID row
'
'****************************************************************************************************************************
Public Function DCID_Lookup_Size(row As Long) As String
    DCID_Lookup_Size = Trim()
End Function


'****************************************************************************************************************************
'
'   Lookup count of array elements field value for given DCID row
'
'****************************************************************************************************************************
Public Function DCID_Lookup_Count(row As Long) As String
    countstr = Trim(Worksheets("DCI Descriptors").Range("A1").Cells(row, DCI_ARRAY_COUNT_COL).text)
    If Len(countstr) = 0 Then
        countstr = "1"
    End If
    DCID_Lookup_Count = countstr
End Function


'****************************************************************************************************************************
'
'   Lookup total length field value for given DCID row
'
'****************************************************************************************************************************
Public Function DCID_Lookup_Length(row As Long) As String
    DCID_Lookup_Length = Trim(Worksheets("DCI Descriptors").Range("A1").Cells(row, TOTAL_LENGTH_COL).text)
End Function


'****************************************************************************************************************************
'
'   Lookup factory default field value for given DCID row
'
'****************************************************************************************************************************
Public Function DCID_Lookup_Default(row As Long) As String
    DCID_Lookup_Default = Trim(Worksheets("DCI Descriptors").Range("A1").Cells(row, DATA_DEFAULT_COL).text)
End Function


'****************************************************************************************************************************
'
'   Lookup minimum limit field value for given DCID row
'
'****************************************************************************************************************************
Public Function DCID_Lookup_Min(row As Long) As String
    DCID_Lookup_Min = Trim(Worksheets("DCI Descriptors").Range("A1").Cells(row, DATA_MIN_COL).text)
End Function


'****************************************************************************************************************************
'
'   Lookup maximum limit field value for given DCID row
'
'****************************************************************************************************************************
Public Function DCID_Lookup_Max(row As Long) As String
    DCID_Lookup_Max = Trim(Worksheets("DCI Descriptors").Range("A1").Cells(row, DATA_MAX_COL).text)
End Function


'****************************************************************************************************************************
'
'   Lookup enum definitions field value for given DCID row
'
'****************************************************************************************************************************
Public Function DCID_Lookup_Enum(row As Long) As String
    DCID_Lookup_Enum = Trim(Worksheets("DCI Descriptors").Range("A1").Cells(row, DATA_ENUMLIST_COL).text)
End Function


'****************************************************************************************************************************
'
'   Lookup bitfield definitions field value for given DCID row
'
'****************************************************************************************************************************
Public Function DCID_Lookup_Bitfield(row As Long) As String
    DCID_Lookup_Bitfield = Trim(Worksheets("DCI Descriptors").Range("A1").Cells(row, DATA_BITFIELD_COL).text)
End Function


'****************************************************************************************************************************
'
'   Lookup short description field value for given DCID row
'
'****************************************************************************************************************************
Public Function DCID_Lookup_Short_Description(row As Long) As String
    descr = Trim(Worksheets("DCI Descriptors").Range("A1").Cells(row, DCI_SHORT_DESC_COL).text)
    If Len(descr) = 0 Then
        descr = DCID_Lookup_Engrg_Description(row)
    End If
    DCID_Lookup_Short_Description = descr
End Function


'****************************************************************************************************************************
'
'   Lookup long description field value for given DCID row
'
'****************************************************************************************************************************
Public Function DCID_Lookup_Long_Description(row As Long) As String
    DCID_Lookup_Long_Description = Trim(Worksheets("DCI Descriptors").Range("A1").Cells(row, DCI_LONG_DESC_COL).text)
End Function


'****************************************************************************************************************************
'
'   Lookup units of measure field value for given DCID row
'
'****************************************************************************************************************************
Public Function DCID_Lookup_Units(row As Long) As String
    DCID_Lookup_Units = Trim(Worksheets("DCI Descriptors").Range("A1").Cells(row, DCI_UNITS_DESC_COL).text)
End Function


'****************************************************************************************************************************
'
'   Lookup format description field value for given DCID row
'
'****************************************************************************************************************************
Public Function DCID_Lookup_Format_Description(row As Long) As String
    DCID_Lookup_Format_Description = Trim(Worksheets("DCI Descriptors").Range("A1").Cells(row, DCI_FORMAT_DESC_COL).text)
End Function

'****************************************************************************************************************************
'
'   Lookup long description field value for given DCID row
'
'****************************************************************************************************************************
Public Function DCID_Lookup_Internal_Description(row As Long) As String
    DCID_Lookup_Internal_Description = Trim(Worksheets("DCI Descriptors").Range("A1").Cells(row, DCI_INTERNAL_DESC_COL).text)
End Function




