Attribute VB_Name = "Modbus_Helpers"

'****************************************************************************************************************************
'
'   Tests whether the given cell in the Modbus sheet is checked (typically with 'x')
'
'****************************************************************************************************************************
Public Function Modbus_Is_Box_Checked(row As Long, column As Long) As Boolean
    check_box = Trim(Worksheets("Modbus").Range("A1").Cells(row, column).text)
    Modbus_Is_Box_Checked = UCase(check_box) <> ""
End Function

'****************************************************************************************************************************
'
'   Tests whether the User Documentation Ignored box is checked
'
'****************************************************************************************************************************
Public Function Modbus_Is_UserDocIgnored_Checked(row As Long) As Boolean
    check_box = Trim(Worksheets("Modbus").Range("A1").Cells(row, MODBUS_USER_DOC_IGNORE_COL).text)
    Modbus_Is_UserDocIgnored_Checked = UCase(check_box) <> ""
End Function

'****************************************************************************************************************************
'
'   Lookup DCI Code Define field value for given Modbus sheet row
'
'****************************************************************************************************************************
Public Function Modbus_Lookup_Code_Define(row As Long) As String
    Modbus_Lookup_Code_Define = Worksheets("Modbus").Range("A1").Cells(row, MODBUS_DATA_DEFINE_COL).text
End Function

'****************************************************************************************************************************
'
'   Lookup modbus register number field value for given Modbus sheet row
'
'****************************************************************************************************************************
Public Function Modbus_Lookup_Register(row As Long) As String
    Modbus_Lookup_Register = Worksheets("Modbus").Range("A1").Cells(row, MODBUS_DATA_REG_COL).text
End Function

'****************************************************************************************************************************
'
'   Lookup Modbus device identity strings
'   Note: returns an array of strings
'
'****************************************************************************************************************************
Public Function Modbus_Lookup_Identity_Strings() As String()
    Const IDENT_NAME_COL = 34
    Const IDENT_VALUE_COL = 35
    Const INDENT_FIRST_ROW = 1
    Const INDENT_LAST_ROW = 7
    Dim desc_sheet_row As Long
    Dim row As Long
    Dim index As Long
    Dim name As String
    Dim dcid_name As String
    Dim value As String
    Dim result(INDENT_LAST_ROW - INDENT_FIRST_ROW + 1) As String
    row = INDENT_FIRST_ROW
    index = 0
    While row <= INDENT_LAST_ROW
        name = Worksheets("Modbus").Range("A1").Cells(row, IDENT_NAME_COL).text
        dcid_name = Worksheets("Modbus").Range("A1").Cells(row, IDENT_VALUE_COL).text
        desc_sheet_row = Find_DCID_Row(dcid_name)
        If (desc_sheet_row > 0) Then
            value = Trim(DCID_Lookup_Default(desc_sheet_row))
            If value = "" Then
                value = dcid_name & " [default is empty]"
            End If
        Else
            value = dcid_name & " [symbol not found]"
        End If
        result(index) = name & " " & value
        row = row + 1
        index = index + 1
    Wend
    Modbus_Lookup_Identity_Strings = result
End Function

'****************************************************************************************************************************
'
'*  Gets the path for generated files from the
'*  Usage Notes worksheet
'*  (e.g. ..\Docs\Generated\)
'
'****************************************************************************************************************************
Public Function Get_Generated_Doc_Path() As String
    Get_Generated_Doc_Path = ActiveWorkbook.Path & "\" & Worksheets("Usage Notes").Range("A1").Cells(GENERATED_DOCS_PATH_ROW, GENERATED_DOCS_PATH_COL).text
End Function

'****************************************************************************************************************************
'
'*  Gets the filename for Modbus output files from the worksheet
'*  (e.g. Modbus_DCI_Data)
'
'****************************************************************************************************************************
Public Function Get_Modbus_Output_File_Pathname(extension As String, Optional suffix As String = "") As String
    Get_Modbus_Output_File_Pathname = Get_Generated_Doc_Path() & Cells(2, 2).value & suffix & extension
End Function

'****************************************************************************************************************************
'
'*  Gets modbus register count for given number of bytes
'
'****************************************************************************************************************************
Public Function BytesToRegisterCount(bytes As Long) As Long
    If ((bytes Mod 2) <> 0) Then
        bytes = bytes + 1
    End If
    BytesToRegisterCount = bytes / 2
End Function


