Attribute VB_Name = "Alarms_Builder"
'**************************
'**************************
'Alarms DCI Construction sheet.
'**************************
'**************************
'********************************************************************

'*********************************************************************
'****** Private global Constants(Not accessible outside this module)
'*********************************************************************

'***** "Alarms" sheet
Const START_ROW = 9
Const ALARM_RULE_COL = 1
Const MESSAGE_COL = ALARM_RULE_COL + 1
Const SEVERITY_LEVEL_COL = MESSAGE_COL + 1
Const PRIORITY_COL = SEVERITY_LEVEL_COL + 1
Const DELAY_COL = PRIORITY_COL + 1

'***** "Defines" sheet
Const SEVERITY_LEVEL_DEF_START_ROW = 7
Const SEVERITY_LEVEL_DEF_COL = 5
Const PUB_CADENCE_DEF_COL = 6

Const BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"

Sub Create_Alarms_DCI_Xfer()
    
    Dim sheet_name As String
    Dim col_counter As Integer
    Dim total_alarm_rules As Integer
    
    '*** Select alarms sheet to capture its contents
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select
    file_name = Cells(2, 3).value
    lower_case_file_name = LCase(file_name)
    upper_case_file_name = UCase(file_name)
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\" & Cells(3, 3).value
    Set ALARM_DCI_C = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(4, 3).value
    Set ALARM_DCI_H = fs.CreateTextFile(file_path, True)
    severity_level_table_name = "severity_level_table"
    alarm_target_struct_name = "alarm_target_info_st_t"
    
    'Making font type and size consistent.
    Worksheets(sheet_name).Range("A:E").Font.name = "Arial"
    Worksheets(sheet_name).Range("A:E").Font.Size = 10
    
    '****************************************************************************************************************************
    '****************************************************************************************************************************
    '****** Start Creating the .H file header
    '****************************************************************************************************************************
    '****************************************************************************************************************************
    ALARM_DCI_H.WriteLine ("/**")
    ALARM_DCI_H.WriteLine ("*****************************************************************************************")
    ALARM_DCI_H.WriteLine ("*  @file")
    ALARM_DCI_H.WriteLine ("*  @brief Sample database file for Alarms.")
    ALARM_DCI_H.WriteLine ("*")
    ALARM_DCI_H.WriteLine ("*  @details")
    ALARM_DCI_H.WriteLine ("*")
    ALARM_DCI_H.WriteLine ("*  @copyright 2021 Eaton Corporation. All Rights Reserved.")
    ALARM_DCI_H.WriteLine ("*")
    ALARM_DCI_H.WriteLine (" *****************************************************************************************")
    ALARM_DCI_H.WriteLine ("*/")
    ALARM_DCI_H.WriteLine ("")
    ALARM_DCI_H.WriteLine ("#ifndef " & upper_case_file_name & "_H")
    ALARM_DCI_H.WriteLine ("#define " & upper_case_file_name & "_H")
    ALARM_DCI_H.WriteLine ("")
    ALARM_DCI_H.WriteLine ("#include ""DCI.h""")
    ALARM_DCI_H.WriteLine ("#include ""DCI_Defines.h""")
    ALARM_DCI_H.WriteLine ("#include ""Alarm_DCI.h""")
    ALARM_DCI_H.WriteLine ("")
    
    ALARM_DCI_H.WriteLine ("//*******************************************************")
    ALARM_DCI_H.WriteLine ("//******     Alarms structure defines ")
    ALARM_DCI_H.WriteLine ("//*******************************************************")
    ALARM_DCI_H.WriteLine ("")
    ALARM_DCI_H.WriteLine ("extern const " & alarm_target_struct_name & " " & lower_case_file_name & "_target_info;")
    ALARM_DCI_H.WriteLine ("extern const severity_t " & severity_level_table_name & "[];")
    ALARM_DCI_H.WriteLine ("")
    ALARM_DCI_H.WriteLine ("#endif //" & upper_case_file_name & "_H")
    
        
    '****************************************************************************************************************************
    '****************************************************************************************************************************
    '****** Start Creating the .C file header
    '****************************************************************************************************************************
    '****************************************************************************************************************************
    ALARM_DCI_C.WriteLine ("/**")
    ALARM_DCI_C.WriteLine ("*****************************************************************************************")
    ALARM_DCI_C.WriteLine ("*  @file")
    ALARM_DCI_C.WriteLine ("*  @details See header file for module overview.")
    ALARM_DCI_C.WriteLine ("*  @copyright 2021 Eaton Corporation. All Rights Reserved.")
    ALARM_DCI_C.WriteLine ("*")
    ALARM_DCI_C.WriteLine ("*****************************************************************************************")
    ALARM_DCI_C.WriteLine ("*/")
    ALARM_DCI_C.WriteLine ("")
    ALARM_DCI_C.WriteLine ("#include ""Includes.h""")
    ALARM_DCI_C.WriteLine ("#include """ & file_name & ".h""")
    ALARM_DCI_C.WriteLine ("")
    
    '****** Throw an error if there's any duplication of values for Alarm rule, message and priority in Alarms Sheet.
    If (Check_Duplication(ALARM_RULE_COL) = True) Then
        MsgBox "Error: One or more duplicate alarm rules found"
        Exit Sub
    Else
        '***** export total rules to source file
        total_alarm_rules = Get_Total_Rules(ALARM_RULE_COL)
        ALARM_DCI_C.WriteLine ("static const uint8_t " & upper_case_file_name & "_TOTAL_RULES = " & total_alarm_rules & "U;")
    End If
    
    If (Check_Duplication(MESSAGE_COL) = True) Then
        MsgBox "Error: One or more duplicate alarm messages found"
        Exit Sub
    End If
    
    If (Check_Duplication(PRIORITY_COL) = True) Then
        MsgBox "Error: One or more duplicate priorities found"
        Exit Sub
    End If

   '****** Throw an error if priority level in alarms sheet is not in DCI descriptors sheet
    If (Check_in_DCI_Sheet(START_ROW, PRIORITY_COL) = True) Then
        MsgBox "Error: One or more priorities not present in DCI descriptors sheet"
        Exit Sub
    End If

   '****** Throw an error if severity level not defined in Defines worksheet, else export them
    Dim Severity_levels_array() As String
    If (Get_Severity_levels(Severity_levels_array) = True) Then
        total_severity_levels = UBound(Severity_levels_array)
        '** Check whether cadence is defined for each severity level. If not then throw an error
        If (Check_Cadence_Defined(total_severity_levels) = False) Then
            MsgBox "Please define publishing cadence for each severity level in Defines worksheet"
        End If
        '** Check whether cadence for each severity level is defined in DCI descriptors sheet
        If (Check_Cadence_in_DCI_Sheet(SEVERITY_LEVEL_DEF_START_ROW, PUB_CADENCE_DEF_COL) = True) Then
            MsgBox "Error: One or more publishing cadence is not present in DCI descriptors sheet"
        Exit Sub
        End If
        '**export TOTAL_SEVERITY_LEVELS and severity_level_table to source file
        ALARM_DCI_C.WriteLine ("static const uint8_t " & upper_case_file_name & "_TOTAL_SEVERITY_LEVELS = " & total_severity_levels & "U;")
        ALARM_DCI_C.WriteLine ("const severity_t " & severity_level_table_name & "[" & upper_case_file_name & "_TOTAL_SEVERITY_LEVELS] = ")
        ALARM_DCI_C.WriteLine ("{")
        counter = 0
        While (counter < total_severity_levels)
            ALARM_DCI_C.WriteLine ("    {" & Severity_levels_array(counter) & ", " & ActiveWorkbook.Worksheets("Defines").Cells(SEVERITY_LEVEL_DEF_START_ROW + counter, PUB_CADENCE_DEF_COL).text & "_DCID },")
            counter = counter + 1
        Wend
        ALARM_DCI_C.WriteLine ("};")
    Else
        MsgBox "Severity levels not defined in Defines worksheet"
        Exit Sub
    End If
       
    '****** If the delay is DCI based then throw an error if it is not present in DCI descriptors sheet
    If (Check_Delay_in_DCI_Sheet(DELAY_COL) = False) Then
        MsgBox "Error: One or more DCI based delays not found in DCI descriptors sheet"
        Exit Sub
    End If
    
    '****************************************************************************************************************************
    '****** Generate Alarms to DCID cross reference table
    '****************************************************************************************************************************
    alarms_lkup_struct_name = "dci_alarms_to_dcid_lkup_st_t"
    alarms_lkup_struct_array_name = "alarms_dci_data_alarms_to_dcid"
    ALARM_DCI_C.WriteLine ("")
    ALARM_DCI_C.WriteLine ("//*******************************************************")
    ALARM_DCI_C.WriteLine ("//******     Alarms to DCID Cross Reference table.")
    ALARM_DCI_C.WriteLine ("//*******************************************************")
    ALARM_DCI_C.WriteLine ("")
    ALARM_DCI_C.WriteLine ("static const " & alarms_lkup_struct_name & " " & alarms_lkup_struct_array_name & "[" & upper_case_file_name & "_TOTAL_RULES] =")
    ALARM_DCI_C.WriteLine ("{")
    
    counter = START_ROW
    While (Cells(counter, ALARM_RULE_COL).text <> Empty) And (Cells(counter, ALARM_RULE_COL).text <> BEGIN_IGNORED_DATA)
        ALARM_DCI_C.WriteLine ("    {")
        ALARM_DCI_C.WriteLine ("        " & Cells(counter, ALARM_RULE_COL).text & ",")
        ALARM_DCI_C.WriteLine ("        " & Cells(counter, MESSAGE_COL).text & ",")
        sev_lvl_arry_idx = Application.Match(Cells(counter, SEVERITY_LEVEL_COL).text, Severity_levels_array, 0) - 1
        ALARM_DCI_C.WriteLine ("        " & sev_lvl_arry_idx & ", //" & Severity_levels_array(sev_lvl_arry_idx) & " i.e element " & sev_lvl_arry_idx & " of " & severity_level_table_name & "[]")
        ALARM_DCI_C.WriteLine ("        " & Cells(counter, PRIORITY_COL).text & "_DCID" & ",")
        '***** If delay is DCI based then export it as DCI_DB_Macros with flag as true
        If (InStr(Cells(counter, DELAY_COL).text, "DCI_") = 1) Then
            ALARM_DCI_C.WriteLine ("        { true, " & Cells(counter, DELAY_COL).text & "_DCID }")
        Else
            ALARM_DCI_C.WriteLine ("        { false, " & Cells(counter, DELAY_COL).text & " }")
        End If
        ALARM_DCI_C.WriteLine ("    },")
        counter = counter + 1
    Wend
    ALARM_DCI_C.WriteLine ("};")
    
    '****************************************************************************************************************************
    '****** Generate the target definition structure.
    '****************************************************************************************************************************
    ALARM_DCI_C.WriteLine ("")
    ALARM_DCI_C.WriteLine ("//*******************************************************")
    ALARM_DCI_C.WriteLine ("//******     The Target Definition Structure.")
    ALARM_DCI_C.WriteLine ("//*******************************************************")
    ALARM_DCI_C.WriteLine ("")
    
    ALARM_DCI_C.WriteLine ("const " & alarm_target_struct_name & " " & lower_case_file_name & "_target_info = ")
    ALARM_DCI_C.WriteLine ("{")
    ALARM_DCI_C.WriteLine ("    " & upper_case_file_name & "_TOTAL_SEVERITY_LEVELS,")
    ALARM_DCI_C.WriteLine ("    " & upper_case_file_name & "_TOTAL_RULES,")
    ALARM_DCI_C.WriteLine ("    " & alarms_lkup_struct_array_name & ",")
    ALARM_DCI_C.WriteLine ("    " & severity_level_table_name)
    ALARM_DCI_C.WriteLine ("};")
    ALARM_DCI_C.WriteLine ("")
 

End Sub

'***** Function that returns true if duplicates found, else returns false
Function Check_Duplication(column_number) As Boolean
    Dim sheet_name As String
    
'***** Select active sheet
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select
    
'***** Set black font color(by default)
    counter = START_ROW
    While (Cells(counter, column_number).text <> Empty) And (Cells(counter, column_number).text <> BEGIN_IGNORED_DATA)
        Cells(counter, column_number).Font.Color = vbBlack
        counter = counter + 1
    Wend
    
'***** Check for duplication
    counter = START_ROW
    NextCounter = 0
    FaultCounter = 0
    
    While (Cells(counter, column_number).text <> Empty) And (Cells(counter, column_number).text <> BEGIN_IGNORED_DATA)
        NextCounter = 1
        While (Cells(counter + NextCounter, column_number).text <> Empty) And (Cells(counter + NextCounter, column_number).text <> BEGIN_IGNORED_DATA)
            If (Cells(counter, column_number).text = Cells(counter + NextCounter, column_number).text) Then
                FaultCounter = FaultCounter + 1
                '***** Set Red font to indicate error
                Cells(counter + NextCounter, column_number).Font.Color = vbRed
            End If
            NextCounter = NextCounter + 1
        Wend
        counter = counter + 1
    Wend
    
    If (FaultCounter <> 0) Then
        Check_Duplication = True '***** Duplicate found
    Else
        Check_Duplication = False
    End If
End Function

'***** Function that returns true if all the parameters in given column are present in DCI sheet, else returns false
Function Check_in_DCI_Sheet(row_num, column_number) As Boolean
    Dim sheet_name As String
    
'***** Select active sheet
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select
    
'***** Set black font color(by default)
    counter = row_num
    While (Cells(counter, column_number).text <> Empty) And (Cells(counter, column_number).text <> BEGIN_IGNORED_DATA)
        Cells(counter, column_number).Font.Color = vbBlack
        counter = counter + 1
    Wend
    
    counter = row_num
    FaultCounter = 0
    Dim row_number As Integer
    While (Cells(counter, column_number).text <> Empty) And (Cells(counter, column_number).text <> BEGIN_IGNORED_DATA)
        row_number = 0
        row_number = Find_DCID_Row(Cells(counter, column_number).text)
        If (row_number = 0) Then
            FaultCounter = FaultCounter + 1
            '***** Set Red font to indicate error
            Cells(counter, column_number).Font.Color = vbRed
        End If
        counter = counter + 1
    Wend
    
    If (FaultCounter <> 0) Then
        Check_in_DCI_Sheet = True '***** All params are present in DCI descriptors sheet
    Else
        Check_in_DCI_Sheet = False
    End If
End Function

'***** Function that populates Severity_levels_array with the severity levels defined in "Defines" sheet(for drop-down)
'***** Returns true if severity levels are defined in "Defines" sheet, else returns false
Function Get_Severity_levels(ByRef Severity_levels_array() As String) As Boolean

'***** Count the number of severity levels from "Defines" sheet(non-active sheet)
    row_counter = SEVERITY_LEVEL_DEF_START_ROW
    array_size = 0
    While (ActiveWorkbook.Worksheets("Defines").Cells(row_counter, SEVERITY_LEVEL_DEF_COL).text <> Empty) And (ActiveWorkbook.Worksheets("Defines").Cells(row_counter, SEVERITY_LEVEL_DEF_COL).text <> BEGIN_IGNORED_DATA)
        row_counter = row_counter + 1
        array_size = array_size + 1
    Wend
    
    If (array_size = 0) Then
        Get_Severity_levels = False '***** Severity levels are not defined in "Defines" sheet
    Else
        ReDim Severity_levels_array(array_size) '***** Dynamic array size
    '***** Populate Severity_levels_array with the severity levels
        row_counter = SEVERITY_LEVEL_DEF_START_ROW
        index = 0
        While (ActiveWorkbook.Worksheets("Defines").Cells(row_counter, SEVERITY_LEVEL_DEF_COL).text <> Empty) And (ActiveWorkbook.Worksheets("Defines").Cells(row_counter, SEVERITY_LEVEL_DEF_COL).text <> BEGIN_IGNORED_DATA)
            Severity_levels_array(index) = ActiveWorkbook.Worksheets("Defines").Cells(row_counter, SEVERITY_LEVEL_DEF_COL).text
            index = index + 1
            row_counter = row_counter + 1
        Wend
    
    End If
        Get_Severity_levels = True
End Function

'***** Function that returns total rules
Function Get_Total_Rules(column_number) As Variant
    Dim sheet_name As String
    
'***** Select active sheet
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select
    
'***** Count alarm rules
    counter = START_ROW
    While (Cells(counter, column_number).text <> Empty) And (Cells(counter, column_number).text <> BEGIN_IGNORED_DATA)
        counter = counter + 1
    Wend
    
    Get_Total_Rules = counter - START_ROW
End Function

'***** Function that returns true if delay(DCI based) present in DCI sheet, else returns false
Function Check_Delay_in_DCI_Sheet(column_number) As Boolean
    Dim sheet_name As String
    Dim ret_val As Boolean
    ret_val = True
    
'***** Select active sheet
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select
    
'***** Set black font color(by default)
    counter = START_ROW
    While (Cells(counter, column_number).text <> Empty) And (Cells(counter, column_number).text <> BEGIN_IGNORED_DATA)
        Cells(counter, column_number).Font.Color = vbBlack
        counter = counter + 1
    Wend

'***** Check if the delay(if configured as DCI) is present in DCI sheet
    counter = START_ROW
    FaultCounter = 0
    Dim row_number As Integer
    While (Cells(counter, column_number).text <> Empty) And (Cells(counter, column_number).text <> BEGIN_IGNORED_DATA)
        If (InStr(Cells(counter, column_number).text, "DCI_") = 1) Then
            row_number = 0
            row_number = Find_DCID_Row(Cells(counter, column_number).text)
            If (row_number = 0) Then
                FaultCounter = FaultCounter + 1
                ret_val = False
                '***** Set Red font to indicate error
                Cells(counter, column_number).Font.Color = vbRed
            End If
        End If
        counter = counter + 1
    Wend

 Check_Delay_in_DCI_Sheet = ret_val
End Function

'***** Function that returns true if publishing cadence is defined for each severity level, else returns false
Function Check_Cadence_Defined(total_severity_levels) As Boolean
    row_counter = SEVERITY_LEVEL_DEF_START_ROW
    While (ActiveWorkbook.Worksheets("Defines").Cells(row_counter, PUB_CADENCE_DEF_COL).text <> Empty) And (ActiveWorkbook.Worksheets("Defines").Cells(row_counter, PUB_CADENCE_DEF_COL).text <> BEGIN_IGNORED_DATA)
        row_counter = row_counter + 1
    Wend
    If (row_counter < (SEVERITY_LEVEL_DEF_START_ROW + total_severity_levels)) Then
        Check_Cadence_Defined = False  '***One or more cadence not defined
    Else
        Check_Cadence_Defined = True
    End If
End Function

'***** Function that returns true if publishing cadence rates are present in DCI sheet, else returns false
Function Check_Cadence_in_DCI_Sheet(row_num, column_number) As Boolean
    
'***** Set black font color(by default)
    counter = row_num
    While (ActiveWorkbook.Worksheets("Defines").Cells(counter, column_number).text <> Empty) And (ActiveWorkbook.Worksheets("Defines").Cells(counter, column_number).text <> BEGIN_IGNORED_DATA)
        ActiveWorkbook.Worksheets("Defines").Cells(counter, column_number).Font.Color = vbBlack
        counter = counter + 1
    Wend
    
    counter = row_num
    FaultCounter = 0
    Dim row_number As Integer
    While (ActiveWorkbook.Worksheets("Defines").Cells(counter, column_number).text <> Empty) And (ActiveWorkbook.Worksheets("Defines").Cells(counter, column_number).text <> BEGIN_IGNORED_DATA)
        row_number = 0
        row_number = Find_DCID_Row(ActiveWorkbook.Worksheets("Defines").Cells(counter, column_number).text)
        If (row_number = 0) Then
            FaultCounter = FaultCounter + 1
            '***** Set Red font to indicate error
            ActiveWorkbook.Worksheets("Defines").Cells(counter, column_number).Font.Color = vbRed
        End If
        counter = counter + 1
    Wend
    
    If (FaultCounter <> 0) Then
        Check_Cadence_in_DCI_Sheet = True '***** All cadence rates are present in DCI descriptors sheet
    Else
        Check_Cadence_in_DCI_Sheet = False
    End If
End Function
