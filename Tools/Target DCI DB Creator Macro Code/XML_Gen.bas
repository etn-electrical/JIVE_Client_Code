Attribute VB_Name = "XML_Gen"


'*************************************************************
'*************************************************************
'*** XML Parameter Mapping Creation Macro
'*************************************************************
'*************************************************************

Sub XML_Parameter_Mapping_Create()
    Const description_column As Integer = 1
    Const dci_id_column As Integer = 2
    Const xml_id_column As Integer = 3
    Const xml_type_column As Integer = 4
    Const include_column As Integer = 5
    Const datatype_column As Integer = 5
    Const start_row As Integer = 9
    Const file_path_cell As String = "B1"
    Const file_name_cell As String = "B2"
    Const struct_datatype_cell As String = "B3"
    Const parameter_list_cell As String = "B4"
    Const parameter_list_length_cell As String = "B5"
    Const diagnostic_list_cell As String = "B6"
    Const diagnostic_list_length_cell As String = "B7"
    
    Dim param_row As Integer
    Dim include_row As Integer
    Dim param_count As Integer
    Dim diag_count As Integer
    Dim write_count As Integer
    
    Dim success_flag As Boolean
    Dim file_text As String
    
    
    Application.Calculation = xlCalculationManual

    param_row = start_row
    param_count = 0
    diag_count = 0
    success_flag = True
    While (Cells(param_row, dci_id_column) <> "" And Trim(Cells(param_row, dci_id_column)) <> "BEGIN_IGNORED_DATA")
        
    ' Clear previous warning colors
    ActiveSheet.Range(Cells(param_row, description_column), Cells(param_row, xml_type_column)).Interior.ColorIndex = xlNone
    
    ' Verify no duplicate DCI label entries
        Cells(param_row, dci_id_column).Select
        
        match_row = Application.Match(ActiveCell.Value, ActiveSheet.Columns(dci_id_column), 0)
        
        If (IsError(match_row)) Then
            ActiveSheet.Cells(match_row, dci_id_column).Interior.ColorIndex = 36
            ActiveSheet.Cells(match_row, dci_id_column).Interior.Pattern = xlSolid
            ActiveCell.Interior.ColorIndex = 36
            ActiveCell.Interior.Pattern = xlSolid
            success_flag = False
        Else
            If (match_row <> param_row) Then
                ActiveSheet.Cells(match_row, dci_id_column).Interior.ColorIndex = 36
                ActiveSheet.Cells(match_row, dci_id_column).Interior.Pattern = xlSolid
                ActiveCell.Interior.ColorIndex = 36
                ActiveCell.Interior.Pattern = xlSolid
                success_flag = False
            End If
        End If
                        
    ' Verify correct parameter designations
        Cells(param_row, xml_type_column).Select
        
        If ("P" <> Trim(ActiveCell.Value) And "D" <> Trim(ActiveCell.Value)) Then
            ActiveCell.Interior.ColorIndex = 38
            ActiveCell.Interior.Pattern = xlSolid
            success_flag = False
        Else
            If ("P" = Trim(ActiveCell.Value)) Then
                param_count = param_count + 1
            Else
                diag_count = diag_count + 1
            End If
        End If
        
    ' Verify DCI parameter exists
        Cells(param_row, dci_id_column).Select
        
        match_row = Application.Match(ActiveCell.Value, Worksheets("DCI Descriptors").Columns(2), 0)
        
        If Not IsError(Index) Then
            Cells(param_row, description_column) = Worksheets("DCI Descriptors").Cells(match_row, 1)
            'Cells(param_row, datatype_column) = Worksheets("DCI Descriptors").Cells(match_row, 3)
        Else
            ActiveCell.Interior.ColorIndex = 38
            ActiveCell.Interior.Pattern = xlSolid
            Cells(param_row, description_column) = ""
            Cells(param_row, datatype_column) = ""
            success_flag = False
        End If
        
    ' Verify XML parameter number is valid
        Cells(param_row, xml_id_column).Select
        
        If IsNumeric(Selection) Then
            Selection.Value = Int(0.5 + Selection.Value)
        Else
            ActiveCell.Interior.ColorIndex = 38
            ActiveCell.Interior.Pattern = xlSolid
            success_flag = False
        End If
        
    ' Verify no duplicate XML parameter id entries
        Cells(param_row, xml_id_column).Select
        
        match_row = Application.Match(ActiveCell.Value, ActiveSheet.Columns(xml_id_column), 0)
        
        If (IsError(match_row)) Then
            ActiveSheet.Cells(match_row, xml_id_column).Interior.ColorIndex = 36
            ActiveSheet.Cells(match_row, xml_id_column).Interior.Pattern = xlSolid
            ActiveCell.Interior.ColorIndex = 36
            ActiveCell.Interior.Pattern = xlSolid
            success_flag = False
        Else
            If (match_row <> param_row) Then
                ActiveSheet.Cells(match_row, xml_id_column).Interior.ColorIndex = 36
                ActiveSheet.Cells(match_row, xml_id_column).Interior.Pattern = xlSolid
                ActiveCell.Interior.ColorIndex = 36
                ActiveCell.Interior.Pattern = xlSolid
                success_flag = False
            End If
        End If
        
    ' Advance to next row
        param_row = param_row + 1
    Wend
    
    If (True = success_flag) Then
    
        If (param_row > start_row) Then
            
        ' Sort Data by XML id
            Range(Cells(start_row, description_column), Cells(param_row - 1, datatype_column)).Select
            Selection.Sort Key1:=Range("C9"), Order1:=xlAscending, Header:=xlGuess, _
                OrderCustom:=1, MatchCase:=False, Orientation:=xlTopToBottom, _
                DataOption1:=xlSortNormal
            
         ' Open file and write title block
            Set fs = CreateObject("Scripting.FileSystemObject")
            file_path = ActiveWorkbook.Path & "\" & Range(file_path_cell).Value & Range(file_name_cell).Value
            Set XML_MAP_FILE = fs.CreateTextFile(file_path, True)
            XML_MAP_FILE.WriteLine ("/**")
            XML_MAP_FILE.WriteLine ("*****************************************************************************************")
            XML_MAP_FILE.WriteLine ("*   @file " & Cells(2, 2))
            XML_MAP_FILE.WriteLine ("*")
            XML_MAP_FILE.WriteLine ("*   @brief XML parameter mapping structures ")
            XML_MAP_FILE.WriteLine ("*")
            XML_MAP_FILE.WriteLine ("*   @details")
            XML_MAP_FILE.WriteLine ("*")
            XML_MAP_FILE.WriteLine ("*   @version C++ Style Guide Version 1.0")
            XML_MAP_FILE.WriteLine ("*")
            XML_MAP_FILE.WriteLine ("*   @copyright" & Str(Year(Now)) & " Eaton Corporation. All Rights Reserved.")
            XML_MAP_FILE.WriteLine ("*")
            XML_MAP_FILE.WriteLine ("*****************************************************************************************")
            XML_MAP_FILE.WriteLine ("*/")
            
        ' Add Includes
            include_row = 2
            While (include_row < 8 And "" <> Trim(Cells(include_row, include_column)))
                XML_MAP_FILE.WriteLine ("#include """ + Trim(Cells(include_row, include_column)) + """")
                include_row = include_row + 1
            Wend
            XML_MAP_FILE.WriteLine ("")
            
        ' Add Parameter List
            If (param_count > 0) Then
                XML_MAP_FILE.WriteLine ("/*")
                XML_MAP_FILE.WriteLine ("*****************************************************************************************")
                XML_MAP_FILE.WriteLine ("* See header file for function definition.")
                XML_MAP_FILE.WriteLine ("*****************************************************************************************")
                XML_MAP_FILE.WriteLine ("*/")
                XML_MAP_FILE.WriteLine ("const " & Trim(Range(struct_datatype_cell)) & " " & Trim(Range(parameter_list_cell)) & "[] = ")
                XML_MAP_FILE.WriteLine ("{")
                
                write_count = 0
                param_row = start_row
                success_flag = True
                
                While (Cells(param_row, dci_id_column) <> "" And Trim(Cells(param_row, dci_id_column)) <> "BEGIN_IGNORED_DATA")
                    If ("P" = Trim(Cells(param_row, xml_type_column))) Then
                        write_count = write_count + 1
                        XML_MAP_FILE.WriteLine ("    /* " & Trim(Cells(param_row, description_column)) & " */")
                        file_text = Str(Cells(param_row, xml_id_column))
                        file_text = "    {" & Space(6 - Len(file_text)) & file_text & ", " & Trim(Cells(param_row, dci_id_column)) & "_DCID }"
                        If (write_count < param_count) Then
                            XML_MAP_FILE.WriteLine (file_text & ",")
                            XML_MAP_FILE.WriteLine ("")
                        Else
                            XML_MAP_FILE.WriteLine (file_text)
                        End If
                    End If
                    param_row = param_row + 1
                Wend
                XML_MAP_FILE.WriteLine ("};")
                XML_MAP_FILE.WriteLine ("")
                XML_MAP_FILE.WriteLine ("const uint16_t " & Trim(Range(parameter_list_length_cell).Value) & " = " & Trim(Str(param_count)) & ";")
                XML_MAP_FILE.WriteLine ("")
            End If
            
            
        ' Add Diagnostic List
            If (diag_count > 0) Then
                XML_MAP_FILE.WriteLine ("/*")
                XML_MAP_FILE.WriteLine ("*****************************************************************************************")
                XML_MAP_FILE.WriteLine ("* See header file for function definition.")
                XML_MAP_FILE.WriteLine ("*****************************************************************************************")
                XML_MAP_FILE.WriteLine ("*/")
                XML_MAP_FILE.WriteLine ("const " + Trim(Range(struct_datatype_cell)) + " " + Trim(Range(diagnostic_list_cell)) + "[] = ")
                XML_MAP_FILE.WriteLine ("{")
                
                write_count = 0
                param_row = start_row
                success_flag = True
                
                While (Cells(param_row, dci_id_column) <> "" And Trim(Cells(param_row, dci_id_column)) <> "BEGIN_IGNORED_DATA")
                    If ("D" = Trim(Cells(param_row, xml_type_column))) Then
                        write_count = write_count + 1
                        XML_MAP_FILE.WriteLine ("    /* " & Trim(Cells(param_row, description_column)) & " */")
                        file_text = Str(Cells(param_row, xml_id_column))
                        file_text = "    {" & Space(6 - Len(file_text)) & file_text & ", " & Trim(Cells(param_row, dci_id_column)) & "_DCID }"
                        If (write_count < diag_count) Then
                            XML_MAP_FILE.WriteLine (file_text & ",")
                            XML_MAP_FILE.WriteLine ("")
                        Else
                            XML_MAP_FILE.WriteLine (file_text)
                        End If
                    End If
                    param_row = param_row + 1
                Wend
                XML_MAP_FILE.WriteLine ("};")
                XML_MAP_FILE.WriteLine ("")
                XML_MAP_FILE.WriteLine ("const uint16_t " & Trim(Range(diagnostic_list_length_cell).Value) & " = " & Trim(Str(diag_count)) & ";")
            End If
                        
            XML_MAP_FILE.Close
            MsgBox ("File Created")
        Else
            MsgBox ("No Data")
        End If
    Else
        MsgBox ("Problems in worksheet")
    End If
    
    Application.Calculation = xlCalculationAutomatic

End Sub



