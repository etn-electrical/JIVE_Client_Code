Attribute VB_Name = "Modbus_HTML_PUBLIC"
'****************************************************************************************************************************
'
'   Generates the Modbus Register Documentation for *ALL* registers.
'   The resultant HTML file is NOT to be published to customers.
'
'****************************************************************************************************************************
Private HtmlDoc As TextStream

'****************************************************************************************************************************
'
'   Generate Modbus Register Documentation for *ALL* registers.
'   The resultant HTML file is NOT to be published to customers.
'
'****************************************************************************************************************************
Sub Modbus_HTML_PUBLIC_Documentation()
    Application.Calculation = xlCalculationManual
    Dim file_name As String
    file_name = Get_Modbus_Output_File_Pathname(".html")
    Set fs = CreateObject("Scripting.FileSystemObject")
    Set HtmlDoc = fs.CreateTextFile(file_name, True)
    Call Emit_HTML_Header
    Call Emit_HTML_Table_Header("Register", "Name", "Attributes", "Description")

    Dim desc_sheet_row As Long
    Dim reg_num As Long
    Dim coil_num As Long
    Dim modbus_sheet_row As Long
    Dim reg_num_str As String
    Dim attribute_str As String
    Dim short_descr As String
    Dim long_descr As String
    Dim dcid_name As String

    modbus_sheet_row = MODBUS_DATA_START_ROW
    While (Cells(modbus_sheet_row, MODBUS_DATA_DEFINE_COL).text <> Empty) And (Cells(modbus_sheet_row, MODBUS_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        dcid_name = Cells(modbus_sheet_row, MODBUS_DATA_DEFINE_COL).text
        desc_sheet_row = Find_DCID_Row(dcid_name)
        reg_num = Cells(modbus_sheet_row, MODBUS_DATA_REG_COL).value + 1
        coil_num = ((reg_num - 1) * 16) + 1

        If desc_sheet_row > 0 Then
            If Modbus_Is_UserDocIgnored_Checked(modbus_sheet_row) = False Then
                reg_num_str = CStr(reg_num)
                short_descr = DCID_Lookup_Short_Description(desc_sheet_row)
                attribute_str = Build_Html_Attribute_Description(modbus_sheet_row, desc_sheet_row)
                long_descr = Build_Html_Long_Description(desc_sheet_row, coil_num)
                Call Emit_HTML_Table_Row(reg_num_str, short_descr, attribute_str, long_descr)
            End If
        Else
            reg_num_str = CStr(reg_num)
            short_descr = dcid_name
            attribute_str = "Sorry, not found!"
            long_descr = "Sorry, not found!"
            Call Emit_HTML_Table_Row(reg_num_str, short_descr, attribute_str, long_descr)
        End If
        modbus_sheet_row = modbus_sheet_row + 1
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
    HtmlDoc.WriteLine ("C445 Modbus Registers")
    HtmlDoc.WriteLine ("</h1>")
    HtmlDoc.WriteLine ("Created = " & DateTime.Now & " local<br/>")
    Dim ident_str_array() As String
    ident_str_array = Modbus_Lookup_Identity_Strings()
    For index = 1 To UBound(ident_str_array) Step 1
        HtmlDoc.WriteLine (ident_str_array(index) & "<br/>")
    Next index
End Sub

'****************************************************************************************************************************
'
'   Emit HTML Table Header
'
'****************************************************************************************************************************
Private Sub Emit_HTML_Table_Header(reg_num_str As String, short_descr As String, attribute_str As String, long_descr As String)
    HtmlDoc.WriteLine ("<table class=""reference"" cellspacing=""0"" cellpadding=""2"" border=""5"" width=""100%"" style=""font-family:arial"">")
    HtmlDoc.WriteLine ("<tr>")
    HtmlDoc.WriteLine ("<th align=""left"" valign=""top"">" & reg_num_str & "</th>")
    HtmlDoc.WriteLine ("<th align=""left"" valign=""top"">" & short_descr & "</th>")
    HtmlDoc.WriteLine ("<th align=""left"" valign=""top"" width=""20%"">" & attribute_str & "</th>")
    HtmlDoc.WriteLine ("<th align=""left"" valign=""top"" width=""50%"">" & long_descr & "</th>")
    HtmlDoc.WriteLine ("</tr>")
End Sub

'****************************************************************************************************************************
'
'   Emit HTML Table Row
'
'****************************************************************************************************************************
Private Sub Emit_HTML_Table_Row(reg_num_str As String, short_descr As String, attribute_str As String, long_descr As String)
    HtmlDoc.WriteLine ("<tr>")
    HtmlDoc.WriteLine ("<td align=""left"" valign=""top"">" & reg_num_str & "</td>")
    HtmlDoc.WriteLine ("<td align=""left"" valign=""top"">" & short_descr & "</td>")
    HtmlDoc.WriteLine ("<td align=""left"" valign=""top"">" & attribute_str & "</td>")
    HtmlDoc.WriteLine ("<td align=""left"" valign=""top"">" & long_descr & "</td>")
    HtmlDoc.WriteLine ("</tr>")
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
Private Function Build_Html_Attribute_Description(modbus_sheet_row As Long, desc_sheet_row As Long) As String
    Dim attrib_str As String
    Dim result_str As String

    attrib_str = ""
    result_str = ""

    ' data type
    attrib_str = Mid(DCID_Lookup_Datatype(desc_sheet_row), 11)
    result_str = result_str & attrib_str & " "

    ' read-only
    If DCID_Is_Writable(desc_sheet_row) Then
        attrib_str = "RW"
    Else
        attrib_str = "RO"
    End If
    result_str = result_str & attrib_str & " "

    ' non-volatile
    If DCID_Is_NonVolatile(desc_sheet_row) Then
        attrib_str = "NV"
    Else
        attrib_str = ""
    End If
    result_str = result_str & attrib_str & "<br/>" & vbCrLf

    ' array
    If DCID_Is_Array(desc_sheet_row) Then
        attrib_str = "Array size: " & DCID_Lookup_Count(desc_sheet_row)
        result_str = result_str & attrib_str & "<br/>" & vbCrLf
    End If

    ' default value
    If DCID_Has_Default_Value(desc_sheet_row) Then
        attrib_str = "Default: " & DCID_Lookup_Default(desc_sheet_row)
        If DCID_Is_Default_Writeable(desc_sheet_row) Then
            attrib_str = attrib_str & " (RW)"
        End If
        result_str = result_str & attrib_str & "<br/>" & vbCrLf
    End If

    ' range
    If DCID_Has_Range_Checking(desc_sheet_row) Then
        attrib_str = "Range: " & DCID_Lookup_Min(desc_sheet_row) & " to " & DCID_Lookup_Max(desc_sheet_row)
        If DCID_Is_Range_Writeable(desc_sheet_row) Then
            attrib_str = attrib_str & " (RW)"
        End If
        result_str = result_str & attrib_str & "<br/>" & vbCrLf
    End If

    ' enumerated value
    If DCID_Is_Enum(desc_sheet_row) Then
        attrib_str = "Enum"
        result_str = result_str & attrib_str & "<br/>" & vbCrLf
    End If

    ' bit field
    If DCID_Is_Bitfield(desc_sheet_row) Then
        attrib_str = "Bitfield"
        result_str = result_str & attrib_str & "<br/>" & vbCrLf
    End If

    ' units
    If DCID_Has_Units(desc_sheet_row) Then
        attrib_str = "Units: " & DCID_Lookup_Units(desc_sheet_row)
        result_str = result_str & attrib_str & "<br/>" & vbCrLf
    End If

    Build_Html_Attribute_Description = result_str
End Function

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

    long_desc = vbCrLf & "<table class=""reference"" cellspacing=""0"" cellpadding=""2"" border=""1"" width=""100%"" style=""font-family:arial"">"
    long_desc = long_desc & "<tr>" & vbCrLf
    long_desc = long_desc & "<th width=""25%"">Bit</th>" & vbCrLf
    long_desc = long_desc & "<th width=""50%"">Description</th>" & vbCrLf
    long_desc = long_desc & "<th>Coil</th>" & vbCrLf
    long_desc = long_desc & "</tr>" & vbCrLf

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

            long_desc = long_desc & "<tr>" & vbCrLf
            long_desc = long_desc & "<td align=""center"">" & ParsedItems(0) & "</td>" & vbCrLf
            long_desc = long_desc & "<td>" & ParsedItems(2) & "</td>" & vbCrLf
            If coil <= 65535 Then
                long_desc = long_desc & "<td align=""center"">" & coil & "</td>" & vbCrLf
            Else
                long_desc = long_desc & "<td align=""center"">n/a</td>" & vbCrLf
            End If
            long_desc = long_desc & "</tr>" & vbCrLf

        End If
    Next index
    long_desc = long_desc & "</table>" & vbCrLf

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

    long_desc = vbCrLf & "<table class=""reference"" cellspacing=""0"" cellpadding=""2"" border=""1"" width=""100%"" style=""font-family:arial"">" & vbCrLf
    long_desc = long_desc & "<tr>" & vbCrLf
    long_desc = long_desc & "<th width=""25%"">Value</th>" & vbCrLf
    long_desc = long_desc & "<th>Description</th>" & vbCrLf
    long_desc = long_desc & "</tr>" & vbCrLf

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

            long_desc = long_desc & "<tr>" & vbCrLf
            long_desc = long_desc & "<td align=""center"">" & ParsedItems(0) & "</td>" & vbCrLf
            long_desc = long_desc & "<td align=""left"">" & ParsedItems(2) & "</td>" & vbCrLf
            long_desc = long_desc & "</tr>" & vbCrLf

        End If
    Next index
    long_desc = long_desc & "</table>" & vbCrLf

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

    long_desc = long_desc & "<br/>" & vbCrLf
    
    Dim extra As String
    extra = DCID_Lookup_Format_Description(desc_sheet_row)
    If Len(extra) > 0 Then
        long_desc = long_desc & extra & "<br/>" & vbCrLf
    End If
    
    extra = DCID_Lookup_Internal_Description(desc_sheet_row)
    If Len(extra) > 0 Then
        long_desc = long_desc & extra & "<br/>" & vbCrLf
    End If
    
    Build_Html_Long_Description = long_desc
End Function


