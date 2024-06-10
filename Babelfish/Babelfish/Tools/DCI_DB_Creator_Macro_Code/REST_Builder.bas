Attribute VB_Name = "REST_Builder"
'**************************
'**************************
'Modbus Construction sheet.
'**************************
'**************************
Public CalculationState As Long
Public EventState As Boolean
Public DisplayPageBreakState As Boolean

Sub Disable_Features()

Application.ScreenUpdating = False

EventState = Application.EnableEvents
Application.EnableEvents = False
CalculationState = Application.Calculation
Application.Calculation = xlCalculationManual
DisplayPageBreakState = ActiveSheet.DisplayPageBreaks
ActiveSheet.DisplayPageBreaks = False
End Sub
Sub Restore_Features()
ActiveSheet.DisplayPageBreaks = DisplayPageBreakState
Application.Calculation = CalculationState
Application.EnableEvents = EventState
Application.ScreenUpdating = True
End Sub
Sub Create_REST_DCI_Xfer()
    Call Disable_Features
    Call Verify_Rest_List(False)
	
'*********************
'******     Constants
'*********************
    START_ROW = 9
    
    DESCRIP_COL = 1
    DATA_DEFINE_COL = 2
    PARAM_ID_COL = 3
    R_COL = 4
    W_COL = 5
    FORMAT_COL = 6
    
    ENUMS_COL = "Z"
    BITFIELD_COL = "K"
    
    MISC_DEFINITION_START_ROW = 2
    MISC_DEFINITION_COL = 3
    MISC_R_COL = 4
    MISC_W_COL = 5
    
    'DCI_DEFINES_START_ROW = 9
    'DCI_DESCRIPTORS_COL = 1
    'DCI_DEFINES_COL = 2
    DCI_SHORT_NAME_COL = 79
    DCI_LONG_DESCRIPTION = 81
    DCI_UNIT = 83
    
    ASSY_START_ROW = 9
    
    'ASSY_ID_COL = 10
    'ASSY_NAME_COL = 11
    ASSY_PARAM_COL = 2
    ASSY_ID_TYPE = 7
    
    NUM_OF_CUSTOM_ASSY = 10
    FIRST_CUSTOM_ASSY_NUMBER = 101
    
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    Dim sheet_name As String
    Dim description_string As String
    Dim format_string As String
    
    Dim DCIRows As New Dictionary
    
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select

    file_name = Cells(2, 2).value
    lower_case_file_name = LCase(file_name)
    upper_case_file_name = UCase(file_name)
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\" & Cells(3, 2).value
    Set REST_DCI_C = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(4, 2).value
    Set REST_DCI_H = fs.CreateTextFile(file_path, True)
    
    'Making font type and size consistent.
    Worksheets("REST").Range("A:K").Font.name = "Arial"
    Worksheets("REST").Range("A:K").Font.Size = 10


    DCI_DESC_DTYPE_COL = 3
    DCI_DESC_DTYPE_LEN_COL = 4
    DCI_DESC_ARR_COUNT_COL = 5
    DCI_DESC_TOTAL_LEN_COL = 6
            
    Dim max_rest_arr_count As Integer
    Dim max_rest_total_length As Integer
    
    max_rest_arr_count = 1
    max_rest_total_length = 1
    
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        dci_row = Find_DCID_Row(Cells(counter, DATA_DEFINE_COL).text)
        If (dci_row = 0) Then
            DCID_Lookup_DCID_Symbol (dci_row)
        Else
            If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_row, DCI_DESC_DTYPE_COL).text <> "DCI_DTYPE_STRING8") Then
                If (max_rest_arr_count < (Worksheets("DCI Descriptors").Range("A1").Cells(dci_row, DCI_DESC_ARR_COUNT_COL).value)) Then
                    max_rest_arr_count = (Worksheets("DCI Descriptors").Range("A1").Cells(dci_row, DCI_DESC_ARR_COUNT_COL).value)
                End If
            End If
            If (max_rest_total_length < (Worksheets("DCI Descriptors").Range("A1").Cells(dci_row, DCI_DESC_TOTAL_LEN_COL).value)) Then
                max_rest_total_length = (Worksheets("DCI Descriptors").Range("A1").Cells(dci_row, DCI_DESC_TOTAL_LEN_COL).value)
            End If
        End If
        counter = counter + 1
    Wend
    
'****************************************************************************************************************************
'******     Start Creating the .H file header
'****************************************************************************************************************************
    REST_DCI_H.WriteLine ("/*")
    REST_DCI_H.WriteLine ("*****************************************************************************************")
    REST_DCI_H.WriteLine ("*       $Workfile:$")
    REST_DCI_H.WriteLine ("*")
    REST_DCI_H.WriteLine ("*       $Author:$")
    REST_DCI_H.WriteLine ("*       $Date:$")
    REST_DCI_H.WriteLine ("*       Copyright © 2011, Eaton Corporation. All Rights Reserved.")
    REST_DCI_H.WriteLine ("*       Creator: Ting Yan")
    REST_DCI_H.WriteLine ("*")
    REST_DCI_H.WriteLine ("*       Description:")
    REST_DCI_H.WriteLine ("*")
    REST_DCI_H.WriteLine ("*")
    REST_DCI_H.WriteLine ("*       Code Inspection Date: ")
    REST_DCI_H.WriteLine ("*")
    REST_DCI_H.WriteLine ("*       $Header:$")
    REST_DCI_H.WriteLine ("*****************************************************************************************")
    REST_DCI_H.WriteLine ("*/")
    REST_DCI_H.WriteLine ("#ifndef " & upper_case_file_name & "_H")
    REST_DCI_H.WriteLine ("  #define " & upper_case_file_name & "_H")
    REST_DCI_H.WriteLine ("")
    REST_DCI_H.WriteLine ("#include ""DCI.h""")
    REST_DCI_H.WriteLine ("#include ""Httpd_Rest_DCI.h""")
    REST_DCI_H.WriteLine ("")
'****************************************************************************************************************************
'******     Create the header and associated file structure.  Get ready to plop the data down.
'****************************************************************************************************************************
    REST_DCI_H.WriteLine ("//*******************************************************")
    REST_DCI_H.WriteLine ("//******     RESTful structure and size follow.")
    REST_DCI_H.WriteLine ("//*******************************************************")
    REST_DCI_H.WriteLine ("")
    REST_DCI_H.WriteLine ("extern const REST_TARGET_INFO_ST_TD " & lower_case_file_name & "_target_info;")
    REST_DCI_H.WriteLine ("")
    REST_DCI_H.WriteLine ("static const uint8_t USER_ROLE_ADMIN = 99U;")
    REST_DCI_H.WriteLine ("extern const AUTH_STRUCT_TD auth_struct[];")
    REST_DCI_H.WriteLine ("extern const DCI_USER_MGMT_DB_TD USER_MGMT_USER_LIST[];")
    REST_DCI_H.WriteLine ("extern const uint8_t NUM_OF_AUTH_LEVELS;")
    REST_DCI_H.WriteLine ("extern const uint8_t MAX_NUM_OF_AUTH_USERS;")
    REST_DCI_H.WriteLine ("static const uint16_t REST_DCI_MAX_ARRAY_COUNT = " & max_rest_arr_count & "U;")
    REST_DCI_H.WriteLine ("static const uint16_t REST_DCI_MAX_LENGTH = " & max_rest_total_length & "U;")
    REST_DCI_H.WriteLine ("")
    REST_DCI_H.WriteLine ("")
    REST_DCI_H.WriteLine ("#endif")

'****************************************************************************************************************************
'******     Start Creating the .C file header
'****************************************************************************************************************************
    REST_DCI_C.WriteLine ("/*")
    REST_DCI_C.WriteLine ("*****************************************************************************************")
    REST_DCI_C.WriteLine ("*       $Workfile:")
    REST_DCI_C.WriteLine ("*")
    REST_DCI_C.WriteLine ("*       $Author:$")
    REST_DCI_C.WriteLine ("*       $Date:$")
    REST_DCI_C.WriteLine ("*       Copyright© 2011, Eaton Corporation. All Rights Reserved.")
    REST_DCI_C.WriteLine ("*")
    REST_DCI_C.WriteLine ("*       $Header:$")
    REST_DCI_C.WriteLine ("*****************************************************************************************")
    REST_DCI_C.WriteLine ("*/")
    REST_DCI_C.WriteLine ("#include ""Includes.h""")
    REST_DCI_C.WriteLine ("#include """ & file_name & ".h""")
    REST_DCI_C.WriteLine ("#include ""Format_Handler.h""")
    REST_DCI_C.WriteLine ("#include ""Prod_Spec_Format_Handler.h""")
    REST_DCI_C.WriteLine ("")
    
    Range("A9:G5000").Sort Key1:=Range("C9"), Order1:=xlAscending, _
        Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
        Orientation:=xlTopToBottom
    
'****************************************************************************************************************************
'******     Gotta calculate the total number of RESTful parameters
'****************************************************************************************************************************
    
    REST_DCI_C.WriteLine ("")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("//******     The RESTful misc definitions ")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("")
    
    counter = MISC_DEFINITION_START_ROW
    found_all_param_access = 0
    While (Cells(counter, MISC_DEFINITION_COL).text <> Empty) And (Cells(counter, MISC_DEFINITION_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, MISC_DEFINITION_COL).text = "REST_PARAM_ALL") Then
            REST_DCI_C.WriteLine ("static const rest_pid16_t " & upper_case_file_name & "_ALL_PARAM_ACCESS_R= " & Cells(counter, MISC_R_COL).text & "U;")
            REST_DCI_C.WriteLine ("static const rest_pid16_t " & upper_case_file_name & "_ALL_PARAM_ACCESS_W= " & Cells(counter, MISC_W_COL).text & "U;")
            found_all_param_access = found_all_param_access + 1
        End If
        counter = counter + 1
    Wend

    If (found_all_param_access = 0) Then
        MsgBox "Did not find all parameter access levels ""REST_PARAM_ALL"""
    End If
    
    If (found_all_param_access > 1) Then
        MsgBox """REST_PARAM_ALL"" appeared more than once"
    End If
    
    num_parameters = 0
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        num_parameters = num_parameters + 1
        counter = counter + 1
    Wend
    
    REST_DCI_C.WriteLine ("static const rest_pid16_t NUM_TOTAL_" & upper_case_file_name & "_PARAMS= " & num_parameters & "U;")
    REST_DCI_C.WriteLine ("")
    
    REST_DCI_C.WriteLine ("")
    
    'check if any param_id is greater than 65536
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, PARAM_ID_COL).text > 65535) Then
        MsgBox ("Error: Param ID cannot be greater than 65535")
        Exit Sub
        End If
        counter = counter + 1
    Wend
    
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        REST_DCI_C.WriteLine ("static const rest_pid16_t " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_ID= " & Cells(counter, PARAM_ID_COL).text & "U;")
        counter = counter + 1
    Wend
    
    REST_DCI_C.WriteLine ("")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("//******     The RESTful parameter names ")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("")
    
    REST_DCI_C.WriteLine ("#ifndef REMOVE_REST_PARAM_NAME_TEXT")
    'Give the error if any parameter name is added multiple times in the rest param column
                
    counter = START_ROW
    NextCounter = 0
    ParameterNameFaultCounter = 0
    ParameterIDFaultCounter = 0
    
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
    Cells(counter, DATA_DEFINE_COL).Font.Color = RGB(0, 0, 0)
    Cells(counter, PARAM_ID_COL).Font.Color = RGB(0, 0, 0)
    NextCounter = 1
        While (Cells(counter + NextCounter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter + NextCounter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            If (Cells(counter, DATA_DEFINE_COL).text = Cells(counter + NextCounter, DATA_DEFINE_COL).text) Then
                ParameterNameFaultCounter = ParameterNameFaultCounter + 1
                Cells(counter + NextCounter, DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
            End If
            If (Cells(counter, PARAM_ID_COL).text = Cells(counter + NextCounter, PARAM_ID_COL).text) Then
                ParameterIDFaultCounter = ParameterIDFaultCounter + 1
                Cells(counter + NextCounter, PARAM_ID_COL).Font.Color = RGB(255, 0, 0)
            End If
            NextCounter = NextCounter + 1
        Wend
        counter = counter + 1
    Wend
    
    If (ParameterNameFaultCounter <> 0) Then
        MsgBox "Error: One or more rest parameters are added multiple times"
    End If
    If (ParameterIDFaultCounter <> 0) Then
        MsgBox "Error: One or more rest parameters are assigned same ids"
    End If
    
    
    
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        dci_row = Find_DCID_Row(Cells(counter, DATA_DEFINE_COL).text)
        If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_row, DCI_SHORT_NAME_COL).text = Empty) Then
                REST_DCI_C.WriteLine ("static const char_t " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_NAME []= " & """"";")
        Else
                REST_DCI_C.WriteLine ("static const char_t " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_NAME []= """ & Worksheets("DCI Descriptors").Range("A1").Cells(dci_row, DCI_SHORT_NAME_COL).text & """;")
        End If
        counter = counter + 1
    Wend
    REST_DCI_C.WriteLine ("#endif")
    
    REST_DCI_C.WriteLine ("")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("//******     The RESTful descriptions ")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("")
    REST_DCI_C.WriteLine ("#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT")
    
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        dci_row = Find_DCID_Row(Cells(counter, DATA_DEFINE_COL).text)
        If (dci_row = 0) Then
            MsgBox "DCI not found: " & Cells(counter, DATA_DEFINE_COL).text
            Exit Sub
        Else
            description_string = Worksheets("DCI Descriptors").Range("A1").Cells(dci_row, DCI_LONG_DESCRIPTION).text
            If (description_string = Empty) Then
               REST_DCI_C.WriteLine ("static const char_t " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_DESCRIPTION []= " & """"";")
            Else
               REST_DCI_C.WriteLine ("static const char_t " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_DESCRIPTION []= """ & Format_String_For_REST(description_string) & """;")
            End If
            
        
        End If
        counter = counter + 1
    Wend
    REST_DCI_C.WriteLine ("#endif")
    
    REST_DCI_C.WriteLine ("")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("//******     The RESTful Units ")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("")
    
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        dci_row = Find_DCID_Row(Cells(counter, DATA_DEFINE_COL).text)
        If (dci_row = 0) Then
            MsgBox "DCI not found: " & Cells(counter, DATA_DEFINE_COL).text
            Exit Sub
        Else
            description_string = Worksheets("DCI Descriptors").Range("A1").Cells(dci_row, DCI_UNIT).text
            If (description_string = Empty) Then
               REST_DCI_C.WriteLine ("static const char_t " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_UNIT[]= " & """"";")
            Else
               REST_DCI_C.WriteLine ("static const char_t " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_UNIT[]= """ & Format_String_For_REST(description_string) & """;")
            End If
            
        
        End If
        counter = counter + 1
    Wend

    REST_DCI_C.WriteLine ("")
    REST_DCI_C.WriteLine ("")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("//******     The RESTful parameter format names")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("")

    Dim TempArray() As String
    
    counter = START_ROW
    Dim index As Integer
    Dim first_entry As Boolean
    Dim duplicate_entry As Boolean
    index = 0
    first_entry = True
    duplicate_entry = False
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        dci_row = Find_DCID_Row(Cells(counter, DATA_DEFINE_COL).text)
        If (dci_row = 0) Then
            MsgBox "DCI not found: " & Cells(counter, DATA_DEFINE_COL).text
            Exit Sub
        Else
            ' We are reading FORMAT information from "DCI Description" sheet
            format_string = GetFormatString(Cells(counter, DATA_DEFINE_COL).text)
            If format_string <> Empty Then

                Dim Format_Type() As String
                Format_Type = Split(format_string, SCALE_START_DELIMITER)
                
                result = ValidateFormatString(Format_Type(0))
                If result = False Then
                    MsgBox "Invalid Format Name :" & Cells(counter, DATA_DEFINE_COL).text
                    Exit Sub
                End If
                
                'End If
                
                If (first_entry = True) Then
                    REST_DCI_C.WriteLine ("static const char_t " & "FORMAT_NAME_" & Format_Type(0) & "[]= """ & Format_Type(0) & """;")
                    ReDim Preserve TempArray(0)
                    TempArray(0) = Format_Type(0)
                    first_entry = False
                Else
                    For index = 0 To UBound(TempArray)
                        If TempArray(index) = Format_Type(0) Then
                            duplicate_entry = True
                            Exit For
                        End If
                    Next
                    If duplicate_entry = False Then
                        REST_DCI_C.WriteLine ("static const char_t " & "FORMAT_NAME_" & Format_Type(0) & "[]= """ & Format_Type(0) & """;")
                        ReDim Preserve TempArray(UBound(TempArray) + 1)
                        TempArray(UBound(TempArray)) = Format_Type(0)
                    End If
                    duplicate_entry = False
                End If
            End If
        End If
        counter = counter + 1
    Wend
    
    
    REST_DCI_C.WriteLine ("")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("//******     The RESTful parameter config structures")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("")
    
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        dci_row = Find_DCID_Row(Cells(counter, DATA_DEFINE_COL).text)
        If (dci_row = 0) Then
            MsgBox "DCI not found: " & Cells(counter, DATA_DEFINE_COL).text
            Exit Sub
        Else
            format_string = GetFormatString(Cells(counter, DATA_DEFINE_COL).text)
            'Erase spaces, if any
            format_string = Replace(format_string, " ", "")
            If (format_string <> Empty) Then
                Dim scale_factor As Integer
                Dim scale_val() As String
                Format_Type = Split(format_string, SCALE_START_DELIMITER)
                struct_template_name = LCase(Format_Type(0))
                scale_val = Split(Format_Type(1), SCALE_END_DELIMITER)
                argument_list = Split(scale_val(0), ",")
                total_argu_num = UBound(argument_list) + 1

                If (total_argu_num > 0) Then
                    REST_DCI_C.WriteLine ("const " & struct_template_name & "_formatter_config_t " & "FORMATTER_CONFIG_" & Cells(counter, DATA_DEFINE_COL).text & " = ")
                    REST_DCI_C.WriteLine ("{")

                    last_argu_index = total_argu_num - 1
                    For count = 0 To last_argu_index
                        If (count < last_argu_index) Then
                        REST_DCI_C.WriteLine ("    " & argument_list(count) & ",")
                        'Reached to last array entry
                        Else
                            REST_DCI_C.WriteLine ("    " & argument_list(count))
                        End If
                    Next
                    
                    REST_DCI_C.WriteLine ("};")
                    REST_DCI_C.WriteLine ("")
                End If
            End If
        End If
        counter = counter + 1
    Wend


    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("//******     The RESTful parameter format structures")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("")
    
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        dci_row = Find_DCID_Row(Cells(counter, DATA_DEFINE_COL).text)
        If (dci_row = 0) Then
            MsgBox "DCI not found: " & Cells(counter, DATA_DEFINE_COL).text
            Exit Sub
        Else
            format_string = GetFormatString(Cells(counter, DATA_DEFINE_COL).text)
            'Erase spaces, if any
            format_string = Replace(format_string, " ", "")
            If (format_string <> Empty) Then
                Format_Type = Split(format_string, SCALE_START_DELIMITER)
                Format_Name_LCase = LCase(Format_Type(0))
                Format_Name_UCase = UCase(Format_Type(0))
                scale_val = Split(Format_Type(1), SCALE_END_DELIMITER)
                argument_list = Split(scale_val(0), ",")
                total_argu_num = UBound(argument_list) + 1
                
                REST_DCI_C.WriteLine ("const formatter_struct_t " & Cells(counter, DATA_DEFINE_COL).text & "_FORMATTER = ")
                REST_DCI_C.WriteLine ("{")
                REST_DCI_C.WriteLine ("    " & "FORMAT_NAME_" & Format_Name_UCase & ",") 'Format type
                REST_DCI_C.WriteLine ("#ifdef REST_SERVER_FORMAT_HANDLING")
                REST_DCI_C.WriteLine ("    " & "&" & "FORMAT_HANDLER_" & Format_Name_UCase & ",") 'Format handler name is in lower case

                
                If (total_argu_num > 0) Then
                    REST_DCI_C.WriteLine ("    reinterpret_cast<void*>(const_cast<" & Format_Name_LCase & "_formatter_config_t*>(" & "&" & "FORMATTER_CONFIG_" & Cells(counter, DATA_DEFINE_COL).text & "))")
                Else
                    REST_DCI_C.WriteLine ("    nullptr")
                End If
                REST_DCI_C.WriteLine ("#endif")
                REST_DCI_C.WriteLine ("};")
                REST_DCI_C.WriteLine ("")

            End If
        End If
        counter = counter + 1
    Wend
            
    
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("//******     The RESTful parameter bitfield descriptions")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("")
    
    counter = START_ROW
    Dim RawString As String
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        dci_row = Find_DCID_Row(Cells(counter, DATA_DEFINE_COL).text)
        If (dci_row = 0) Then
            MsgBox "DCI not found: " & Cells(counter, DATA_DEFINE_COL).text
            Exit Sub
        Else
            RawString = Trim(Worksheets("DCI Descriptors").Range("A1").Cells(dci_row, BITFIELD_COL).text)
            If (RawString <> Empty) Then
                Dim ListItems() As String
                Dim ParsedItems() As String
                BitfieldCount = 0
                ListItems = Split(RawString, ITEM_DELIMITER)
                For index = 1 To UBound(ListItems) Step 1
                    If "" <> Trim(ListItems(index)) Then
                        ParsedItems = Split(ListItems(index), SUB_ITEM_DELIMITER)
                        'Remove line feeds
                        ParsedItems(0) = Replace(ParsedItems(0), vbLf, "")
                        'ParsedItems(1) = Replace(ParsedItems(1), vbLf, "")
                        ParsedItems(2) = Replace(ParsedItems(2), vbLf, "")
                        REST_DCI_C.WriteLine ("static const char_t " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_BITFIELD_DESC_VAL_" & ParsedItems(0) & "[]= """ & ParsedItems(2) & """;")
                        BitfieldCount = BitfieldCount + 1
                    End If
                Next index
                REST_DCI_C.WriteLine ("bitfield_desc_t " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_BITFIELD_DESC[ " & BitfieldCount & "U" & " ]= ")
                REST_DCI_C.WriteLine ("{")
                For index = 1 To UBound(ListItems) Step 1
                    If "" <> Trim(ListItems(index)) Then
                        ParsedItems = Split(ListItems(index), SUB_ITEM_DELIMITER)
                        'Remove line feeds
                        ParsedItems(0) = Replace(ParsedItems(0), vbLf, "")
                        'ParsedItems(1) = Replace(ParsedItems(1), vbLf, "")
                        ParsedItems(2) = Replace(ParsedItems(2), vbLf, "")
                        If index = BitfieldCount Then
                        REST_DCI_C.WriteLine ("    { " & ParsedItems(0) & "U, " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_BITFIELD_DESC_VAL_" & ParsedItems(0) & " }")
                        Else
                        REST_DCI_C.WriteLine ("    { " & ParsedItems(0) & "U, " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_BITFIELD_DESC_VAL_" & ParsedItems(0) & " },")
                        End If
                    End If
                Next index
                REST_DCI_C.WriteLine ("};")
            REST_DCI_C.WriteLine ("bitfield_desc_block_t " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_BITFIELD_DESC_BLOCK = ")
            REST_DCI_C.WriteLine ("{")
            REST_DCI_C.WriteLine ("    " & BitfieldCount & "U,")
            REST_DCI_C.WriteLine ("    " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_BITFIELD_DESC ")
            REST_DCI_C.WriteLine ("};")
            REST_DCI_C.WriteLine ("")
            End If
        End If
        counter = counter + 1
    Wend
    
    REST_DCI_C.WriteLine ("")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("//******     The RESTful parameter enum descriptions")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("")
    
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        dci_row = Find_DCID_Row(Cells(counter, DATA_DEFINE_COL).text)
        If (dci_row = 0) Then
            MsgBox "DCI not found: " & Cells(counter, DATA_DEFINE_COL).text
            Exit Sub
        Else
            RawString = Trim(Worksheets("DCI Descriptors").Range("A1").Cells(dci_row, ENUMS_COL).text)
            If (RawString <> Empty) Then
                EnumCount = 0
                ListItems = Split(RawString, ITEM_DELIMITER)
                For index = 1 To UBound(ListItems) Step 1
                    If "" <> Trim(ListItems(index)) Then
                        ParsedItems = Split(ListItems(index), SUB_ITEM_DELIMITER)
                        'Remove line feeds
                        ParsedItems(0) = Replace(ParsedItems(0), vbLf, "")
                        EnumValue = val(ParsedItems(0))
                        'ParsedItems(1) = Replace(ParsedItems(1), vbLf, "")
                        ParsedItems(2) = Replace(ParsedItems(2), vbLf, "")
                        'Checking enum value negative or positive
                        If EnumValue < 0 Then
                            REST_DCI_C.WriteLine ("static const char_t " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_ENUM_DESC_VAL_" & "NEG_" & CStr(Abs(EnumValue)) & "[]= """ & ParsedItems(2) & """;")
                        Else
                            REST_DCI_C.WriteLine ("static const char_t " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_ENUM_DESC_VAL_" & ParsedItems(0) & "[]= """ & ParsedItems(2) & """;")
                        End If
                        EnumCount = EnumCount + 1
                    End If
                Next index
                REST_DCI_C.WriteLine ("enum_desc_t " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_ENUM_DESC[ " & EnumCount & "U" & " ]= ")
                REST_DCI_C.WriteLine ("{")
                For index = 1 To UBound(ListItems) Step 1
                    If "" <> Trim(ListItems(index)) Then
                        ParsedItems = Split(ListItems(index), SUB_ITEM_DELIMITER)
                        'Remove line feeds
                        ParsedItems(0) = Replace(ParsedItems(0), vbLf, "")
                        EnumValue = val(ParsedItems(0))
                        'ParsedItems(1) = Replace(ParsedItems(1), vbLf, "")
                        ParsedItems(2) = Replace(ParsedItems(2), vbLf, "")
                        If index = EnumCount Then
                            'Checking enum value negative or positive
                            If EnumValue < 0 Then
                                REST_DCI_C.WriteLine ("    { " & ParsedItems(0) & ", " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_ENUM_DESC_VAL_" & "NEG_" & CStr(Abs(EnumValue)) & " }")
                            Else
                                REST_DCI_C.WriteLine ("    { " & ParsedItems(0) & "U, " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_ENUM_DESC_VAL_" & ParsedItems(0) & " }")
                            End If
                        Else
                            'Checking enum value negative or positive
                            If EnumValue < 0 Then
                                REST_DCI_C.WriteLine ("    { " & ParsedItems(0) & ", " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_ENUM_DESC_VAL_" & "NEG_" & CStr(Abs(EnumValue)) & " },")
                            Else
                                REST_DCI_C.WriteLine ("    { " & ParsedItems(0) & "U, " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_ENUM_DESC_VAL_" & ParsedItems(0) & " },")
                            End If
                        End If
                        'EnumCount = EnumCount + 1
                    End If
                Next index
                REST_DCI_C.WriteLine ("};")
            REST_DCI_C.WriteLine ("enum_desc_block_t " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_ENUM_DESC_BLOCK = ")
            REST_DCI_C.WriteLine ("{")
            REST_DCI_C.WriteLine ("    " & EnumCount & "U,")
            REST_DCI_C.WriteLine ("    " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_ENUM_DESC ")
            REST_DCI_C.WriteLine ("};")
            REST_DCI_C.WriteLine ("")
            End If
        End If
        counter = counter + 1
    Wend
    
    REST_DCI_C.WriteLine ("")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("//******     The Target DCI to RESTful parameters Structure.")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("")
    REST_DCI_C.WriteLine ("static const DCI_REST_TO_DCID_LKUP_ST_TD base_dci_rest_data_param_list[NUM_TOTAL_" & upper_case_file_name & "_PARAMS] =")
    REST_DCI_C.WriteLine ("{")
    
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        format_string = GetFormatString(Cells(counter, DATA_DEFINE_COL).text)
        REST_DCI_C.WriteLine ("    {")
        REST_DCI_C.WriteLine ("        // " & Cells(counter, DESCRIP_COL).text)
        REST_DCI_C.WriteLine ("        " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_ID" & ",")
        REST_DCI_C.WriteLine ("        " & Cells(counter, DATA_DEFINE_COL).text & "_DCID" & ",")
        REST_DCI_C.WriteLine ("#ifndef REMOVE_REST_PARAM_NAME_TEXT")
        REST_DCI_C.WriteLine ("        " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_NAME" & ",")
        REST_DCI_C.WriteLine ("#endif")
        REST_DCI_C.WriteLine ("#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT")
        REST_DCI_C.WriteLine ("        " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_DESCRIPTION" & ",")
        REST_DCI_C.WriteLine ("#endif")
        REST_DCI_C.WriteLine ("        " & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_UNIT" & ",")
        
        dci_row = Find_DCID_Row(Cells(counter, DATA_DEFINE_COL).text)
        If Worksheets("DCI Descriptors").Range("A1").Cells(dci_row, BITFIELD_COL).text <> Empty Then
            REST_DCI_C.WriteLine ("        " & "&" & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_BITFIELD_DESC_BLOCK" & ",")
        Else
            REST_DCI_C.WriteLine ("        nullptr,")
        End If
        
        If Worksheets("DCI Descriptors").Range("A1").Cells(dci_row, ENUMS_COL).text <> Empty Then
            REST_DCI_C.WriteLine ("        " & "&" & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_ENUM_DESC_BLOCK" & ",")
        Else
            REST_DCI_C.WriteLine ("        nullptr,")
        End If
        
        If format_string <> Empty Then
            REST_DCI_C.WriteLine ("        " & "&" & Cells(counter, DATA_DEFINE_COL).text & "_FORMATTER,")
        Else
            REST_DCI_C.WriteLine ("        nullptr,")
        End If
        
        REST_DCI_C.WriteLine ("#ifndef REMOVE_REST_PARAM_NAME_TEXT")
        REST_DCI_C.WriteLine ("        (static_cast<uint8_t>(sizeof(" & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_NAME" & "))-1U),")
        REST_DCI_C.WriteLine ("#endif")
        REST_DCI_C.WriteLine ("        (static_cast<uint8_t>(sizeof(" & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_UNIT" & ")) -1U),")
        REST_DCI_C.WriteLine ("#ifndef REMOVE_REST_PARAM_DESCRIPTION_TEXT")
        REST_DCI_C.WriteLine ("        (static_cast<uint16_t>(sizeof(" & Cells(counter, DATA_DEFINE_COL).text & "_REST_PARAM_DESCRIPTION" & ")) -1U),")
        REST_DCI_C.WriteLine ("#endif")
        
        
        If (Cells(counter, R_COL).text = Empty) Then
            MsgBox "Missing read access level at row : " & counter
            Exit Sub
        Else
            REST_DCI_C.WriteLine ("        " & Cells(counter, R_COL).text & "U,")
        End If
        
        If (Cells(counter, W_COL).text = Empty) Then
            MsgBox "Missing write access level at row : " & counter
            Exit Sub
        Else
            REST_DCI_C.WriteLine ("        " & Cells(counter, W_COL).text & "U")
        End If
               
        REST_DCI_C.WriteLine ("    },")
        counter = counter + 1
    Wend
    
    
    REST_DCI_C.WriteLine ("};")
    REST_DCI_C.WriteLine ("")
    
    
'****************************************************************************************************************************
'******     Predefined Assy
'****************************************************************************************************************************

    REST_DCI_C.WriteLine ("")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("//******     The Predefined Assemblies ")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("")
    
    Const MAX_ASSEMBLY_NUM As Integer = 50
        
    Dim AssemblyID(MAX_ASSEMBLY_NUM), AssemblyName(MAX_ASSEMBLY_NUM)
    Dim str, Seps(1), strlength
    Seps(0) = ";"
    str = Cells(1, ASSY_ID_TYPE + 1).text
    'Remove last ";" if any
    If (Right(str, 1) = ";") Then
        strlength = Len(str)
        str = Left(str, strlength - 1)
        Cells(1, ASSY_ID_TYPE + 1).ClearContents
        Cells(1, ASSY_ID_TYPE + 1).value = str
    End If
    
    Dim i, a
    a = Tokenize(str, Seps)
    
    Dim AssemblyIDSaved(MAX_ASSEMBLY_NUM)
    
    For i = 0 To UBound(a)
        Dim Str1, Seps1(1)
        Str1 = a(i)
        Seps1(0) = ":"
        Dim i1, a1
        a1 = Tokenize(Str1, Seps1)
        AssemblyID(i) = CInt(a1(0))
        AssemblyIDSaved(i) = CInt(a1(0))
        AssemblyName(i) = a1(1)
    Next

    'Clear the the all text in the "Predefined Assembly Name" column
    counter = START_ROW
    While (Cells(counter, ASSY_PARAM_COL).text <> BEGIN_IGNORED_DATA)
    Cells(counter, ASSY_ID_TYPE + 1).ClearContents
    counter = counter + 1
    Wend

    'Arrange the Assembly IDs in the asscending order "Predefined Assembly ID" column
    'Remove the duplicate declaring if any
    Dim ErrorMsgDisplayed
    counter = START_ROW
    While (Cells(counter, ASSY_PARAM_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, ASSY_ID_TYPE).text <> Empty) Then
            Dim Str4, Seps4(1)
            Seps4(0) = ","
            Str4 = Cells(counter, ASSY_ID_TYPE).text
            Dim i4, a4
            a4 = Tokenize(Str4, Seps4)
            For i4 = 0 To UBound(a4) - 1
                a4(i4) = CInt(a4(i4))
            Next
            
            If (UBound(a4) > 1) Then
                For i4 = 0 To UBound(a4) - 1
                    If (a4(i4 + 1) = a4(i4)) Then
                        MsgBox "Error: Parameter can not be added multiple times in same Predefined Assembly. Remove multiple declaration at : " & Cells(counter, ASSY_ID_TYPE).Address
                        Cells(counter, ASSY_ID_TYPE).Font.Color = RGB(255, 0, 0)
                        ErrorMsgDisplayed = True
                    Else
                        Cells(counter, ASSY_ID_TYPE).Font.Color = RGB(0, 0, 0)
                        If (a4(i4) > a4(i4 + 1)) Then
                            Call QuickSort(a4, 0, UBound(a4) - 1)
                            Cells(counter, ASSY_ID_TYPE).ClearContents
                            Cells(counter, ASSY_ID_TYPE).value = CStr(a4(0))
                            Dim i5
                            For i5 = 1 To UBound(a4) - 1
                                Cells(counter, ASSY_ID_TYPE).value = Cells(counter, ASSY_ID_TYPE).value & ", " & CStr(a4(i5))
                            Next
                            Exit For
                        End If
                    End If
                Next
            End If
        End If
        counter = counter + 1
    Wend
    
    'Remove the duplicate declaring if any after the the ids are arranged in ascending order.
    counter = START_ROW
    While ((Cells(counter, ASSY_PARAM_COL).text <> BEGIN_IGNORED_DATA) And (ErrorMsgDisplayed = False))
        If (Cells(counter, ASSY_ID_TYPE).text <> Empty) Then
            Dim Str7, Seps7(1)
            Seps7(0) = ","
            Str7 = Cells(counter, ASSY_ID_TYPE).text
            Dim i7, a7
            a7 = Tokenize(Str7, Seps7)
            For i7 = 0 To UBound(a7) - 1
                a7(i7) = CInt(a7(i7))
            Next
            
            If (UBound(a7) > 1) Then
                For i7 = 0 To UBound(a7) - 1
                    If (a7(i7 + 1) = a7(i7)) Then
                        MsgBox "Error: Parameter can not be added multiple times in same Predefined Assembly. Remove multiple declaration at : " & Cells(counter, ASSY_ID_TYPE).Address
                        Cells(counter, ASSY_ID_TYPE).Font.Color = RGB(255, 0, 0)
                    End If
                Next
            End If
        End If
        counter = counter + 1
    Wend
    
    'Rearragne the assembly names array as per the order of assembly IDs.
    
    Dim i8, i9, AssemblyNameSaved(MAX_ASSEMBLY_NUM)
    
    For i8 = 0 To UBound(a)
        AssemblyNameSaved(i8) = AssemblyName(i8)
    Next
    
    Call QuickSort(AssemblyID, 0, UBound(a) - 1)
    
    
    For i8 = 0 To UBound(a)
        For i9 = 0 To UBound(a)
            If (AssemblyID(i8) = AssemblyIDSaved(i9)) Then
            AssemblyName(i8) = AssemblyNameSaved(i9)
            End If
        Next
    Next
        
    
    assy_row = ASSY_START_ROW
    Dim i3, CellNotEmptyCounter
    If (Cells(1, ASSY_ID_TYPE + 1).text <> Empty) Then
        REST_DCI_C.WriteLine ("static const char_t " & upper_case_file_name & "_PREDEFINED_ASSY_" & AssemblyID(assy_row - 9) & "_NAME []= """ & AssemblyName(assy_row - 9) & """;")
        REST_DCI_C.WriteLine ("static const REST_PARAM_ID_TD " & upper_case_file_name & "_PREDEFINED_ASSY_" & AssemblyID(assy_row - 9) & "_PARAM_LIST[] =")
        REST_DCI_C.WriteLine ("{")
        For i3 = 0 To i - 2
            counter = START_ROW
            CellNotEmptyCounter = 0
            While (Cells(counter, ASSY_PARAM_COL).text <> BEGIN_IGNORED_DATA)
                If (Cells(counter, ASSY_ID_TYPE).text <> Empty) Then

                    Dim Str2, Seps2(1)
                    Seps2(0) = ","
                    Str2 = Cells(counter, ASSY_ID_TYPE).text
                    Dim i2, a2
                    a2 = Tokenize(Str2, Seps2)
                    If (UBound(a2) = 1) Then
                        If (CInt(a2(0)) = AssemblyID(i3)) Then
                            CellNotEmptyCounter = CellNotEmptyCounter + 1
                            REST_DCI_C.WriteLine ("    " & Cells(counter, ASSY_PARAM_COL).text & "_REST_PARAM_ID,")
                            Cells(counter, ASSY_ID_TYPE + 1).value = AssemblyName(i3)
                            Cells(counter, ASSY_ID_TYPE).Font.Color = RGB(0, 0, 0)
                        End If
                    Else
                        For i2 = 0 To UBound(a2) - 1
                            If (CInt(a2(i2)) = AssemblyID(i3)) Then
                                CellNotEmptyCounter = CellNotEmptyCounter + 1
                                REST_DCI_C.WriteLine ("    " & Cells(counter, ASSY_PARAM_COL).text & "_REST_PARAM_ID,")
                                If (i2 = 0) Then
                                    Cells(counter, ASSY_ID_TYPE + 1).value = AssemblyName(i3)
                                Else
                                    CellNotEmptyCounter = CellNotEmptyCounter + 1
                                    Cells(counter, ASSY_ID_TYPE + 1).value = Cells(counter, ASSY_ID_TYPE + 1).value & ", " & AssemblyName(i3)
                                End If
                            End If
                        Next
                    End If
                End If
                counter = counter + 1
            Wend
                If (CellNotEmptyCounter = 0) Then
                    MsgBox "Error: Unused assmebly " & AssemblyID(assy_row - 9) & " is declared. Keep only used assemblies at $H:$1"
                End If
                
                assy_row = assy_row + 1
                If (AssemblyID(assy_row - 9) <> Empty) Then
                    REST_DCI_C.WriteLine ("};")
                    REST_DCI_C.WriteLine ("static const char_t " & upper_case_file_name & "_PREDEFINED_ASSY_" & AssemblyID(assy_row - 9) & "_NAME []= """ & AssemblyName(assy_row - 9) & """;")
                    REST_DCI_C.WriteLine ("static const REST_PARAM_ID_TD " & upper_case_file_name & "_PREDEFINED_ASSY_" & AssemblyID(assy_row - 9) & "_PARAM_LIST[] =")
                    REST_DCI_C.WriteLine ("{")
                End If
        Next
        REST_DCI_C.WriteLine ("};")
        REST_DCI_C.WriteLine ("")
        
        REST_DCI_C.WriteLine ("static const REST_PREDEFINED_ASSY_TD " & upper_case_file_name & "_PREDEFINED_ASSY_LIST[] =")
        REST_DCI_C.WriteLine ("{")
        
        assy_row = ASSY_START_ROW
        While (AssemblyID(assy_row - 9) <> Empty)
                REST_DCI_C.WriteLine ("    {")
                REST_DCI_C.WriteLine ("        " & AssemblyID(assy_row - 9) & "U,")
                REST_DCI_C.WriteLine ("        " & "static_cast<uint16_t>(sizeof(" & upper_case_file_name & "_PREDEFINED_ASSY_" & AssemblyID(assy_row - 9) & "_PARAM_LIST)/sizeof(REST_PARAM_ID_TD)" & "),")
                REST_DCI_C.WriteLine ("        " & upper_case_file_name & "_PREDEFINED_ASSY_" & AssemblyID(assy_row - 9) & "_NAME,")
                REST_DCI_C.WriteLine ("        " & "(static_cast<uint8_t>(sizeof(" & upper_case_file_name & "_PREDEFINED_ASSY_" & AssemblyID(assy_row - 9) & "_NAME)) -1U),")
                REST_DCI_C.WriteLine ("        " & upper_case_file_name & "_PREDEFINED_ASSY_" & AssemblyID(assy_row - 9) & "_PARAM_LIST,")
                REST_DCI_C.WriteLine ("        7U,")
                REST_DCI_C.WriteLine ("        99U")
    
                REST_DCI_C.WriteLine ("    },")
            assy_row = assy_row + 1
        Wend
    
        REST_DCI_C.WriteLine ("};")
        REST_DCI_C.WriteLine ("")
    
    End If


    
'****************************************************************************************************************************
'******     Custom Assy
'****************************************************************************************************************************
    
    REST_DCI_C.WriteLine ("")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("//******     The Custom Assemblies ")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("")

    REST_DCI_C.WriteLine ("")
    REST_DCI_C.WriteLine ("static const rest_pid16_t " & upper_case_file_name & "_NUM_OF_CUSTOM_ASSY = TOTAL_NUM_OF_CUSTOM_ASSEMBLIES;")
    REST_DCI_C.WriteLine ("static const rest_pid8_t " & upper_case_file_name & "_NUM_OF_PARAM_IN_A_CUSTOM_ASSY = MAX_NUM_OF_PARAMS_IN_CUSTOM_ASSY;")
    REST_DCI_C.WriteLine ("")
        
    assy_number = 1
    While (assy_number <= NUM_OF_CUSTOM_ASSY)
        REST_DCI_C.WriteLine ("static const char_t " & upper_case_file_name & "_CUSTOM_ASSY_NAME_" & assy_number & "[] = ""USER ASM" & assy_number & """;")
        assy_number = assy_number + 1
    Wend
    
    REST_DCI_C.WriteLine ("")
   
    REST_DCI_C.WriteLine ("static const REST_CUSTOM_ASSY_TD " & upper_case_file_name & "_CUSTOM_ASSY_LIST[] = {")
   
    assy_number = 1
    
    While (assy_number <= NUM_OF_CUSTOM_ASSY)
        REST_DCI_C.WriteLine ("    {")
        
        REST_DCI_C.WriteLine ("        " & (FIRST_CUSTOM_ASSY_NUMBER - 1 + assy_number) & "U,")
        REST_DCI_C.WriteLine ("        " & upper_case_file_name & "_CUSTOM_ASSY_NAME_" & assy_number & ",")
        REST_DCI_C.WriteLine ("        (static_cast<uint8_t>( sizeof(" & upper_case_file_name & "_CUSTOM_ASSY_NAME_" & assy_number & ") )-1U),")
        REST_DCI_C.WriteLine ("        DCI_VALID_BITS_ASSY_" & (assy_number - 1) & "_DCID,")
        REST_DCI_C.WriteLine ("        DCI_MEMBERS_ASSY_" & (assy_number - 1) & "_DCID,")
        REST_DCI_C.WriteLine ("        7U,")
        REST_DCI_C.WriteLine ("        99U")
        
        REST_DCI_C.WriteLine ("    },")
        
        assy_number = assy_number + 1
    Wend
    
    REST_DCI_C.WriteLine ("};")
    REST_DCI_C.WriteLine ("")
   
    
        
'****************************************************************************************************************************
'******     Target Definition Structure
'****************************************************************************************************************************
        
        
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("//******     The Target Definition Structure.")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("")
    REST_DCI_C.WriteLine ("const REST_TARGET_INFO_ST_TD " & lower_case_file_name & "_target_info = ")
    REST_DCI_C.WriteLine ("{")
    REST_DCI_C.WriteLine ("    NUM_TOTAL_" & upper_case_file_name & "_PARAMS,")
    REST_DCI_C.WriteLine ("    base_dci_rest_data_param_list,")
    REST_DCI_C.WriteLine ("    {")
    REST_DCI_C.WriteLine ("        " & upper_case_file_name & "_ALL_PARAM_ACCESS_R,")
    REST_DCI_C.WriteLine ("        " & upper_case_file_name & "_ALL_PARAM_ACCESS_W,")
    REST_DCI_C.WriteLine ("    },")
    
    REST_DCI_C.WriteLine ("    static_cast<uint16_t>(sizeof(" & upper_case_file_name & "_PREDEFINED_ASSY_LIST) / sizeof(REST_PREDEFINED_ASSY_TD)),")
    REST_DCI_C.WriteLine ("    " & upper_case_file_name & "_PREDEFINED_ASSY_LIST,")
    REST_DCI_C.WriteLine ("    " & upper_case_file_name & "_NUM_OF_CUSTOM_ASSY,")
    REST_DCI_C.WriteLine ("    " & upper_case_file_name & "_NUM_OF_PARAM_IN_A_CUSTOM_ASSY,")
    REST_DCI_C.WriteLine ("    " & upper_case_file_name & "_CUSTOM_ASSY_LIST")
    REST_DCI_C.WriteLine ("")
    

    ' REST_DCI_C.WriteLine (vbTab & Format_String_For_REST(Worksheets("DCI Descriptors").Range("A1").Cells(25, 81).Text))
    
    REST_DCI_C.WriteLine ("};")
    ' MsgBox (num_parameters)
    ' REST_DCI_C.WriteLine (num_parameters)
    REST_DCI_C.WriteLine ("")

    '**************************************user management start***********************
   Dim NUM_OF_USERS As Integer
   Dim NUM_OF_AUTH_ROLES As Integer
   Dim Exit_Code As Boolean
   Exit_Code = False
   
   NUM_OF_USERS = 0
   NUM_OF_AUTH_ROLES = 0
   Const TOTAL_PARAMS_PER_USER = 8
   USER_ROLE_LIST = 7

   Dim user_dcid_list() As String
    REST_DCI_C.WriteLine ("")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("//******     The User Management ")
    REST_DCI_C.WriteLine ("//*******************************************************")
    REST_DCI_C.WriteLine ("")
    
    ' check if user role list cell is not empty
    If Cells(2, USER_ROLE_LIST + 1).text = Empty Then
        MsgBox ("""Web Access User Role List"" Field cannot be left Empty ")
    Else
        ' check if number of Max User allowed cell is not empty
        If Cells(3, USER_ROLE_LIST + 1).text = Empty Then
            MsgBox ("""Max User Allowed"" Field cannot be left Empty ")
        Else
    
            Dim UserRoleID(10), UserRoleName(10), UserRoleDesc(10)
            Dim User_Str, User_Seps(1), User_strlength
        
            User_Seps(0) = ";"
        
            User_Str = Cells(2, USER_ROLE_LIST + 1).text
            'Remove last ";" if any
            If (Right(User_Str, 1) = ";") Then
                User_strlength = Len(User_Str)
                User_Str = Left(User_Str, User_strlength - 1)
                Cells(2, USER_ROLE_LIST + 1).ClearContents
                Cells(2, USER_ROLE_LIST + 1).value = User_Str
            End If
            
            Dim User_a
            ' tokenize number of user roles
            User_a = Tokenize(User_Str, User_Seps)
            
            ' tokenize number of elements in each user role(role_name, level, description)
            For i = 0 To (UBound(User_a) - 1)
                Dim User_Str1, User_Seps1(2)
                User_Str1 = User_a(i)
                User_Seps1(0) = ":"
                User_Seps1(1) = ":"
                Dim User_i1, User_a1
                User_a1 = Tokenize(User_Str1, User_Seps1)
                If User_a1(0) <> Empty And User_a1(1) <> Empty And UBound(User_a1) < 4 Then
                    If IsNumeric(User_a1(0)) = False Then
                        MsgBox ("Web Access User Role List Format Error. Role Level should be a Numeric value ")
                        Exit_Code = True
                        Exit For
                    Else
                        If IsNumeric(User_a1(1)) = True Then
                            MsgBox ("Web Access User Role List Format Error. Role Name cannot be a Numeric value ")
                            Exit_Code = True
                            Exit For
                        Else
                            UserRoleID(i) = CInt(User_a1(0))
                            UserRoleName(i) = User_a1(1)
                            UserRoleDesc(i) = User_a1(2)
                        End If
                    End If
                Else
                    MsgBox ("Web Access User Role List Format Error. Role should be defined in format - Level:Name:Description; ")
                    Exit_Code = True
                    Exit For
                End If
            Next
            
            If (Exit_Code = False) Then
                ' check if there is Admin user with its level as 99
                Dim Admin_found As Boolean
                Admin_found = False
                For i = 0 To (UBound(User_a) - 1)
                    If (UserRoleName(i) = "Admin" And UserRoleID(i) = 99) Then
                        Admin_found = True
                    Exit For
                    End If
                Next
                
                If (Admin_found = False) Then
                    MsgBox (" ""Admin"" Role with level """ & 99 & """ is mandatory ")
                Else
                    ' check if there is duplicate entry of user role name or level
                    Dim Ind As Integer
                    For i = 0 To (UBound(User_a) - 1)
                        For Ind = i + 1 To (UBound(User_a) - 1)
                            If ((UserRoleID(i) = UserRoleID(Ind)) Or (UserRoleName(i) = UserRoleName(Ind))) Then
                                MsgBox (" Duplicate User Role Entries Found ")
                                Exit_Code = True
                            Exit For
                            End If
                        Next
                        If (Exit_Code = True) Then
                            Exit For
                        End If
                    Next
            
                    If (Exit_Code = False) Then
                        ' check if there is user role greater than 99
                        For i = 0 To (UBound(User_a) - 1)
                            If ((UserRoleID(i) > 99) Or (UserRoleID(i) < 1)) Then
                                MsgBox (" User Role Value must be in between ""1"" and ""99"" ")
                                Exit_Code = True
                            Exit For
                            End If
                        Next
                
                        If (Exit_Code = False) Then
                            ' check if user had entered more than allowed number of users
                            If (Cells(3, USER_ROLE_LIST + 1).value < 1 Or Cells(3, USER_ROLE_LIST + 1).value > 16) Then
                                MsgBox ("Minumum (1) and Maximum (16) Users are supported.")
                            Else
                                NUM_OF_USERS = Cells(3, USER_ROLE_LIST + 1).value
            
                                ' Create a temp array of all USER parameter DCIDs required
                                ReDim user_dcid_list(NUM_OF_USERS * TOTAL_PARAMS_PER_USER)
                                i = 0
                                For user_number = 1 To NUM_OF_USERS
                                    user_dcid_list(i) = "DCI_USER_" & user_number & "_FULLNAME"
                                    user_dcid_list(i + 1) = "DCI_USER_" & user_number & "_USERNAME"
                                    user_dcid_list(i + 2) = "DCI_USER_" & user_number & "_PWD"
                                    user_dcid_list(i + 3) = "DCI_USER_" & user_number & "_ROLE"
                                    user_dcid_list(i + 4) = "DCI_USER_" & user_number & "_PWD_SET_EPOCH_TIME"
                                    user_dcid_list(i + 5) = "DCI_USER_" & user_number & "_LAST_LOGIN_EPOCH_TIME"
                                    user_dcid_list(i + 6) = "DCI_USER_" & user_number & "_PWD_COMPLEXITY"
                                    user_dcid_list(i + 7) = "DCI_USER_" & user_number & "_PWD_AGING"
                                    i = i + TOTAL_PARAMS_PER_USER
                                Next
            
                        
                                ' set no of auth roles defined
                                NUM_OF_AUTH_ROLES = UBound(User_a)
                            
                                ' Create a auth_struct structure to be used to access user roles details
                                If (NUM_OF_AUTH_ROLES > 0) Then
                                    REST_DCI_C.WriteLine ("const AUTH_STRUCT_TD auth_struct[] = {")
                                    user_number = 0
                                    While (user_number < NUM_OF_AUTH_ROLES)
                                        REST_DCI_C.WriteLine ("    {")
                                        REST_DCI_C.WriteLine ("        """ & UserRoleName(user_number) & """,")
                                        REST_DCI_C.WriteLine ("        " & UserRoleID(user_number) & ",")
                                        If (UserRoleDesc(user_number) = Empty) Then
                                            REST_DCI_C.WriteLine ("        """ & "No Description Available" & """,")
                                        Else
                                            REST_DCI_C.WriteLine ("        """ & UserRoleDesc(user_number) & """,")
                                        End If
                                        REST_DCI_C.WriteLine ("    },")
                                    
                                        user_number = user_number + 1
                                    Wend
                                        REST_DCI_C.WriteLine ("};")
                                        REST_DCI_C.WriteLine ("")
                                        REST_DCI_C.WriteLine ("const uint8_t NUM_OF_AUTH_LEVELS = sizeof (auth_struct) / sizeof(AUTH_STRUCT_TD);")
                                        REST_DCI_C.WriteLine ("")
                                End If
            
                                ' search in DCI sheet if all required USER DCID parameters are defined
                                counter = START_ROW
                                i = 0
                                While (i < (NUM_OF_USERS * TOTAL_PARAMS_PER_USER))
                                    dci_row = Find_DCID_Row(user_dcid_list(i))
                                    If (dci_row = 0) Then
                                        MsgBox ("DCI Parameter """ & user_dcid_list(i) & """ not found ")
                                        i = NUM_OF_USERS * TOTAL_PARAMS_PER_USER
                                        Exit_Code = True
                                    End If
                                    i = i + 1
                                Wend

                                If (Exit_Code = False) Then
                                    ' Create a USER_MGMT_USER_LIST structure to be used to access each user account details
                                    REST_DCI_C.WriteLine ("const DCI_USER_MGMT_DB_TD USER_MGMT_USER_LIST[] = {")
                                    user_number = 0
                                    While (user_number < NUM_OF_USERS)
                                        REST_DCI_C.WriteLine ("    {")
                                        For i = 0 To (TOTAL_PARAMS_PER_USER - 1)
                                            REST_DCI_C.WriteLine ("        " & user_dcid_list(i + (user_number * TOTAL_PARAMS_PER_USER)) & "_DCID,")
                                        Next
                                        REST_DCI_C.WriteLine ("    },")
                                        user_number = user_number + 1
                                    Wend
                                    REST_DCI_C.WriteLine ("};")
                                    REST_DCI_C.WriteLine ("")
                                    REST_DCI_C.WriteLine ("const uint8_t MAX_NUM_OF_AUTH_USERS = sizeof (USER_MGMT_USER_LIST) / sizeof(DCI_USER_MGMT_DB_TD);")
                                    REST_DCI_C.WriteLine ("")
                                End If
                            End If
                        End If
                    End If
                End If
            End If
        End If
    End If
            '**************************************user management end***********************

    
    REST_DCI_H.Close
    REST_DCI_C.Close
    
    ' MsgBox (Find_DCID_Row("DCI_BF_FAULT_STATUS_BITS"))
    Call Restore_Features
End Sub

'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'*    FUNCTION HEADER
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
Function Format_String_For_REST(source_str As String) As String
    Dim temp_string As String
    temp_string = source_str

    temp_string = Replace(temp_string, vbCr & vbLf, "\n")
    temp_string = Replace(temp_string, vbLf & vbCr, "\n")
    temp_string = Replace(temp_string, vbCr, "\n")
    temp_string = Replace(temp_string, vbLf, "\n")
    
    ' this needs to be the first for any reference replacement
    temp_string = Replace(temp_string, "&", "&amp;")
    
    temp_string = Replace(temp_string, "<", "&lt;")
    temp_string = Replace(temp_string, ">", "&gt;")
    temp_string = Replace(temp_string, """", "&quot;")
    temp_string = Replace(temp_string, "'", "&apos;")
    
    temp_string = Replace(temp_string, "º", "&#176;")
    
    Format_String_For_REST = temp_string

End Function

Sub QuickSort(vec, loBound, hiBound)
  Dim pivot, loSwap, hiSwap, temp

  '== This procedure is adapted from the algorithm given in:
  '==    Data Abstractions & Structures using C++ by
  '==    Mark Headington and David Riley, pg. 586
  '== Quicksort is the fastest array sorting routine for
  '== unordered arrays.  Its big O is  n log n


  '== Two items to sort
  If hiBound - loBound = 1 Then
    If vec(loBound) > vec(hiBound) Then
      temp = vec(loBound)
      vec(loBound) = vec(hiBound)
      vec(hiBound) = temp
    End If
  End If

  '== Three or more items to sort
  pivot = vec(Int((loBound + hiBound) / 2))
  vec(Int((loBound + hiBound) / 2)) = vec(loBound)
  vec(loBound) = pivot
  loSwap = loBound + 1
  hiSwap = hiBound
  
  Do
    '== Find the right loSwap
    While loSwap < hiSwap And vec(loSwap) <= pivot
      loSwap = loSwap + 1
    Wend
    '== Find the right hiSwap
    While vec(hiSwap) > pivot
      hiSwap = hiSwap - 1
    Wend
    '== Swap values if loSwap is less then hiSwap
    If loSwap < hiSwap Then
      temp = vec(loSwap)
      vec(loSwap) = vec(hiSwap)
      vec(hiSwap) = temp
    End If
  Loop While loSwap < hiSwap
  
  vec(loBound) = vec(hiSwap)
  vec(hiSwap) = pivot
  
  '== Recursively call function .. the beauty of Quicksort
    '== 2 or more items in first section
    If loBound < (hiSwap - 1) Then Call QuickSort(vec, loBound, hiSwap - 1)
    '== 2 or more items in second section
    If hiSwap + 1 < hiBound Then Call QuickSort(vec, hiSwap + 1, hiBound)

End Sub  'QuickSort

Public Function GetFormatString(dcid_name As String) As String
    dci_row = DCI_DEFINES_START_ROW
    dcid_entry_status = False
    While (Worksheets("DCI Descriptors").Range("A1").Cells(dci_row, DCI_DEFINES_COL).text <> Empty) And (dcid_entry_status = False)
    
        If (dcid_name = Worksheets("DCI Descriptors").Range("A1").Cells(dci_row, DCI_DEFINES_COL).text) Then
            dcid_entry_status = True
            GetFormatString = Worksheets("DCI Descriptors").Range("A1").Cells(dci_row, DCI_FORMAT_DESC_COL).text
        End If
        dci_row = dci_row + 1
    Wend
End Function 'GetFormatString

Public Function ValidateFormatString(dcid_name As String) As String
    Dim output As String
    output = True
    
    Dim c 'since char type does not exist in vba, we have to use variant type.
    For i = 1 To Len(dcid_name)
        c = Mid(dcid_name, i, 1) 'Select the character at the i position
        If (c >= "a" And c <= "z") Or (c >= "0" And c <= "9") Or (c >= "A" And c <= "Z") Or (c = "_") Then
            'Format name contains first character as number
            'Format Handlers (function name) should not start with number
            If (i = 1) And (c >= "0" And c <= "9") Then
                output = False
                Exit For
            End If
        Else
            'Format name contains character other than a-z, A-Z and "_"
            output = False
            Exit For
        End If
    Next
    ValidateFormatString = output
End Function 'ValidateFormatString

Public Sub Verify_Rest_List(Optional fill_in_ignored As Boolean = True)

    Application.Calculation = xlCalculationManual
    START_ROW = 9
    DESCRIP_COL = 1
    DATA_DEFINE_COL = 2

    '****************************************************************************************************************************
    '******     Find the Data items that have been removed from the main list
    '****************************************************************************************************************************
    rest_row_ctr = START_ROW
    While (Cells(rest_row_ctr, DATA_DEFINE_COL).text <> Empty) And (Cells(rest_row_ctr, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        dci_row = Find_DCID_Row(Cells(rest_row_ctr, DATA_DEFINE_COL).text)
        If (dci_row = 0) Then
            'Fill red colour at Rest cell DCI which is not available in DCI descriptor sheet
            Cells(rest_row_ctr, DESCRIP_COL).Interior.Color = RGB(255, 0, 0)
            Cells(rest_row_ctr, DATA_DEFINE_COL).Interior.Color = RGB(255, 0, 0)
            'Throw the Error at Rest cell DCI which is not available in DCI descriptor sheet
            DCID_Lookup_DCID_Symbol (dci_row)
        End If
        rest_row_ctr = rest_row_ctr + 1
    Wend
    Application.Calculation = xlCalculationAutomatic
End Sub
