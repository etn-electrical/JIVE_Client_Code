Attribute VB_Name = "Modbus_HTML_UNPUB_Builder"
'****************************************************************************************************************************
'
'   Generates the Modbus Register Documentation for *ALL* registers.
'   The resultant HTML file is NOT to be published to customers.
'
'****************************************************************************************************************************
Private HtmlDoc As TextStream
Private Const HTML_COL_WIDTH_REG = """10%"""
Private Const HTML_COL_WIDTH_RW_INFO = """10%"""
Private Const HTML_COL_WIDTH_NAME = """20%"""
Private Const HTML_COL_WIDTH_DESC = """60%"""

Private Const HTML_COL_ALIGN_REG = """center"""
Private Const HTML_COL_ALIGN_RW_INFO = """center"""
Private Const HTML_COL_ALIGN_NAME = """left"""
Private Const HTML_COL_ALIGN_DESC = """left"""

Private Const HTML_COL_VALIGN_REG = """top"""
Private Const HTML_COL_VALIGN_RW_INFO = """top"""
Private Const HTML_COL_VALIGN_NAME = """top"""
Private Const HTML_COL_VALIGN_DESC = """top"""

'****************************************************************************************************************************
'
'   Generate Modbus Register Documentation for *ALL* registers.
'   The resultant HTML file is NOT to be published to customers.
'
'****************************************************************************************************************************
Sub Create_Modbus_HTML_UNPUB_Documentation()
    Application.Calculation = xlCalculationManual
    Dim file_name As String
    file_name = Get_Modbus_Output_File_Pathname(".html", "_UNPUB")
    Set fs = CreateObject("Scripting.FileSystemObject")
    Set HtmlDoc = fs.CreateTextFile(file_name, True)
    Call Emit_HTML_Header
    Call Emit_HTML_Table_Header("Register", "Access", "Default", "Range", "Units", "Name", "Description")
    
    Dim desc_sheet_row As Long
    Dim reg_num As Long
    Dim coil_num As Long
    Dim counter As Long
    Dim reg_num_str As String
    Dim access_str As String
    Dim default_str As String
    Dim range_str As String
    Dim units_str As String
    Dim short_descr As String
    Dim long_descr As String
    Dim dcid_name As String
    
    counter = MODBUS_DATA_START_ROW
    While (Cells(counter, MODBUS_DATA_DEFINE_COL).text <> Empty) And (Cells(counter, MODBUS_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        dcid_name = Cells(counter, MODBUS_DATA_DEFINE_COL).text
        desc_sheet_row = Find_DCID_Row(dcid_name)
        reg_num = Cells(counter, MODBUS_DATA_REG_COL).value + 1
        coil_num = ((reg_num - 1) * 16) + 1

        If desc_sheet_row > 0 Then
            If Modbus_Is_UserDocIgnored_Checked(counter) = False Then
                reg_num_str = ""
            Else
                reg_num_str = " *"
            End If
            reg_num_str = CStr(reg_num) & reg_num_str
            If DCID_Is_Writable(desc_sheet_row) Then
                access_str = "R/W"
            Else
                access_str = "R"
            End If
            If DCID_Has_Default_Value(desc_sheet_row) Then
                default_str = DCID_Lookup_Default(desc_sheet_row)
            Else
                default_str = ""
            End If
            If DCID_Has_Range_Checking(desc_sheet_row) Then
                range_str = DCID_Lookup_Min(desc_sheet_row) & " .. " & DCID_Lookup_Max(desc_sheet_row)
            Else
                range_str = ""
            End If
            
            units_str = Trim(DCID_Lookup_Units(desc_sheet_row))
            
            short_descr = DCID_Lookup_Short_Description(desc_sheet_row) & "<br/>( " & dcid_name & " )"
                long_descr = Build_Html_Long_Description(desc_sheet_row, coil_num)
            Call Emit_HTML_Table_Row(reg_num_str, access_str, default_str, range_str, units_str, short_descr, long_descr)
        Else
            reg_num_str = CStr(reg_num)
            access_str = "*ERROR*"
            default_str = ""
            range_str = ""
            units_str = ""
            short_descr = dcid_name
            long_descr = "Sorry, not found!"
            Call Emit_HTML_Table_Row(reg_num_str, access_str, default_str, range_str, units_str, short_descr, long_descr)
        End If
        
        counter = counter + 1
    Wend

    Call Emit_HTML_Table_Footer
    Call Emit_HTML_Footer
    HtmlDoc.Close
    Application.Calculation = xlCalculationAutomatic
End Sub

'****************************************************************************************************************************
'
'   Emit HTML Header
'
'****************************************************************************************************************************
Private Sub Emit_HTML_Header()
    HtmlDoc.WriteLine ("<!DOCTYPE html>")
    HtmlDoc.WriteLine ("<html>")
    HtmlDoc.WriteLine ("<body>")
    HtmlDoc.WriteLine ("<h1 style=" & HTML_TEXT_STYLE & ">")
    HtmlDoc.WriteLine ("C445 Modbus Registers UNPUBLISHED")
    HtmlDoc.WriteLine ("</h1>")
    HtmlDoc.WriteLine ("This file contains information that is NOT to be shared with customers.")
    HtmlDoc.WriteLine ("Created = " & DateTime.Now & " local<br/>")
    Dim ident_str_array() As String
    ident_str_array = Modbus_Lookup_Identity_Strings()
    For index = 1 To UBound(ident_str_array) Step 1
        HtmlDoc.WriteLine (ident_str_array(index) & "<br/>")
    Next index
    HtmlDoc.WriteLine ("* indicates registers that are unpublished.")
End Sub

'****************************************************************************************************************************
'
'   Emit HTML Table Header
'
'****************************************************************************************************************************
Private Sub Emit_HTML_Table_Header(reg_num_str As String, access_str As String, default_str As String, range_str As String, _
        units_str As String, short_descr As String, long_descr As String)
    HtmlDoc.WriteLine ("<table class=""reference"" cellspacing=""0"" cellpadding=""2"" border=""5"" width=""100%"" style=""font-family:arial"">")
    HtmlDoc.WriteLine ("    <tr>")
    HtmlDoc.WriteLine ("        <th align=" & HTML_COL_ALIGN_REG & " valign=" & HTML_COL_VALIGN_REG & ">" & reg_num_str & "</th>")
    HtmlDoc.WriteLine ("        <th align=" & HTML_COL_ALIGN_RW_INFO & " valign=" & HTML_COL_VALIGN_RW_INFO & ">" & access_str & "</th>")
    HtmlDoc.WriteLine ("        <th align=" & HTML_COL_ALIGN_RW_INFO & " valign=" & HTML_COL_VALIGN_RW_INFO & ">" & default_str & "</th>")
    HtmlDoc.WriteLine ("        <th align=" & HTML_COL_ALIGN_RW_INFO & " valign=" & HTML_COL_VALIGN_RW_INFO & ">" & range_str & "</th>")
    HtmlDoc.WriteLine ("        <th align=" & HTML_COL_ALIGN_RW_INFO & " valign=" & HTML_COL_VALIGN_RW_INFO & ">" & units_str & "</th>")
    HtmlDoc.WriteLine ("        <th width=" & HTML_COL_WIDTH_NAME & " align=" & HTML_COL_ALIGN_NAME & " valign=" & HTML_COL_VALIGN_NAME & ">" & short_descr & "</th>")
    HtmlDoc.WriteLine ("        <th width=" & HTML_COL_WIDTH_DESC & " align=" & HTML_COL_ALIGN_DESC & " valign=" & HTML_COL_VALIGN_DESC & ">" & long_descr & "</th>")
    HtmlDoc.WriteLine ("    </tr>")
End Sub

'****************************************************************************************************************************
'
'   Emit HTML Table Row
'
'****************************************************************************************************************************
Private Sub Emit_HTML_Table_Row(reg_num_str As String, access_str As String, default_str As String, range_str As String, _
        units_str As String, short_descr As String, long_descr As String)
    HtmlDoc.WriteLine ("    <tr>")
    HtmlDoc.WriteLine ("        <td align=" & HTML_COL_ALIGN_REG & " valign=" & HTML_COL_VALIGN_REG & ">" & reg_num_str & "</td>")
    HtmlDoc.WriteLine ("        <td align=" & HTML_COL_ALIGN_RW_INFO & " valign=" & HTML_COL_VALIGN_RW_INFO & ">" & access_str & "</td>")
    HtmlDoc.WriteLine ("        <td align=" & HTML_COL_ALIGN_RW_INFO & " valign=" & HTML_COL_VALIGN_RW_INFO & ">" & default_str & "</td>")
    HtmlDoc.WriteLine ("        <td align=" & HTML_COL_ALIGN_RW_INFO & " valign=" & HTML_COL_VALIGN_RW_INFO & ">" & range_str & "</td>")
    HtmlDoc.WriteLine ("        <td align=" & HTML_COL_ALIGN_RW_INFO & " valign=" & HTML_COL_VALIGN_RW_INFO & ">" & units_str & "</td>")
    HtmlDoc.WriteLine ("        <td align=" & HTML_COL_ALIGN_NAME & " valign=" & HTML_COL_VALIGN_NAME & ">" & short_descr & "</td>")
    HtmlDoc.WriteLine ("        <td align=" & HTML_COL_ALIGN_DESC & " valign=" & HTML_COL_VALIGN_DESC & ">" & long_descr & "</td>")
    HtmlDoc.WriteLine ("    </tr>")
End Sub

'****************************************************************************************************************************
'
'   Emit HTML Table Footer
'
'****************************************************************************************************************************
Private Sub Emit_HTML_Table_Footer()
    HtmlDoc.WriteLine ("</table>")
End Sub

'****************************************************************************************************************************
'
'   Emit HTML Footer
'
'****************************************************************************************************************************
Private Sub Emit_HTML_Footer()
    HtmlDoc.WriteLine ("</body>")
    HtmlDoc.WriteLine ("</html>")
End Sub

'****************************************************************************************************************************
'
'   Build bitfield description string
'
'****************************************************************************************************************************
Private Function Build_Html_Bitfield_Description(desc_sheet_row As Long, first_coil_num As Long) As String
    Dim coil As Long
    Dim long_desc As String
    Dim index As Long
    Dim ListItems() As String
    Dim ParsedItems() As String
    Dim TotalEnums As Long
    Dim bitfieldlist As String
    
    'long_desc = "<br/>Bit# = Description (Coil#)"
    long_desc = "<table class=""reference"" cellspacing=""0"" cellpadding=""2"" border=""1"" width=""100%"" style=""font-family:arial"">"
    long_desc = long_desc & "<tr>"
    long_desc = long_desc & "<th width=""25%"">Bit</th>"
    long_desc = long_desc & "<th width=""50%"">Description</th>"
    long_desc = long_desc & "<th>Coil</th>"
    long_desc = long_desc & "</tr>"
    
    bitfieldlist = DCID_Lookup_Bitfield(desc_sheet_row)
    ListItems = Split(bitfieldlist, ITEM_DELIMITER)
    TotalEnums = CInt(ListItems(0))
    For index = 1 To UBound(ListItems) Step 1
        If "" <> Trim(ListItems(index)) Then
            ParsedItems = Split(ListItems(index), SUB_ITEM_DELIMITER)
            'Remove line feeds
            ParsedItems(0) = Replace(ParsedItems(0), vbLf, "")
            ParsedItems(1) = Replace(ParsedItems(1), vbLf, "")
            ParsedItems(2) = Replace(ParsedItems(2), vbLf, "")
            coil = first_coil_num + CInt(ParsedItems(0))
            
            long_desc = long_desc & "<tr>"
            long_desc = long_desc & "<td align=""center"">" & ParsedItems(0) & "</td>"
            long_desc = long_desc & "<td>" & ParsedItems(2) & "</td>"
            long_desc = long_desc & "<td align=""center"">" & coil & "</td>"
            long_desc = long_desc & "</tr>"
            
            'long_desc = long_desc & "<br/>" & ParsedItems(0) & " = " & ParsedItems(2) & " (" & coil & ")"
        End If
    Next index
    long_desc = long_desc & "</table>"
    
    Build_Html_Bitfield_Description = long_desc
End Function

'****************************************************************************************************************************
'
'   Build enum description string
'
'****************************************************************************************************************************
Private Function Build_Html_Enum_Description(desc_sheet_row As Long) As String
    Dim long_desc As String
    Dim index As Long
    Dim ListItems() As String
    Dim ParsedItems() As String
    Dim TotalEnums As Long
    
    'long_desc = "<br/>Value = Description "
    long_desc = "<table class=""reference"" cellspacing=""0"" cellpadding=""2"" border=""1"" width=""100%"" style=""font-family:arial"">"
    long_desc = long_desc & "<tr>"
    long_desc = long_desc & "<th width=""25%"">Value</th>"
    long_desc = long_desc & "<th>Description</th>"
    long_desc = long_desc & "</tr>"
    
    enumlist = DCID_Lookup_Enum(desc_sheet_row)
    ListItems = Split(enumlist, ITEM_DELIMITER)
    TotalEnums = CInt(ListItems(0))
    For index = 1 To UBound(ListItems) Step 1
        If "" <> Trim(ListItems(index)) Then
            ParsedItems = Split(ListItems(index), SUB_ITEM_DELIMITER)
            'Remove line feeds
            ParsedItems(0) = Replace(ParsedItems(0), vbLf, "")
            ParsedItems(1) = Replace(ParsedItems(1), vbLf, "")
            ParsedItems(2) = Replace(ParsedItems(2), vbLf, "")
            
            long_desc = long_desc & "<tr>"
            long_desc = long_desc & "<td align=""center"">" & ParsedItems(0) & "</td>"
            long_desc = long_desc & "<td align=""left"">" & ParsedItems(2) & "</td>"
            long_desc = long_desc & "</tr>"
            
            'long_desc = long_desc & "<br/>" & ParsedItems(0) & " = " & ParsedItems(2)
        End If
    Next index
    long_desc = long_desc & "</table>"

    Build_Html_Enum_Description = long_desc
End Function

'****************************************************************************************************************************
'
'   Build array description string
'
'****************************************************************************************************************************
Private Function Build_Html_Array_Description(desc_sheet_row As Long) As String
    Dim long_desc As String
    Dim bytes As Long
    Dim registers As Long
    bytes = CInt(DCID_Lookup_Length(desc_sheet_row))
    registers = BytesToRegisterCount(bytes)
    long_desc = "<br/>Array of " & registers & " registers"
    Build_Html_Array_Description = long_desc
End Function

'****************************************************************************************************************************
'
'   Build long description string
'
'****************************************************************************************************************************
Private Function Build_Html_Long_Description(desc_sheet_row As Long, first_coil_num As Long) As String
    Dim long_desc As String
    long_desc = DCID_Lookup_Long_Description(desc_sheet_row)
    
    If (DCID_Has_Bitfield_Definition(desc_sheet_row)) Then
        long_desc = long_desc & Build_Html_Bitfield_Description(desc_sheet_row, first_coil_num)
    ElseIf (DCID_Is_Enum(desc_sheet_row)) Then
        long_desc = long_desc & Build_Html_Enum_Description(desc_sheet_row)
    ElseIf (DCID_Is_Array(desc_sheet_row)) Then
        long_desc = long_desc & Build_Html_Array_Description(desc_sheet_row)
    End If
    
    Build_Html_Long_Description = long_desc
End Function
