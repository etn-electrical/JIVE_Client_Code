Attribute VB_Name = "Intercom_Builder"
'**************************
'**************************
'Intercom Construction sheet.
'**************************
'**************************

Sub Create_Intercom_DCI_Xfer()

    Call Verify_Intercom_List

'*********************
'******     Constants
'*********************
    Application.Calculation = xlCalculationManual
    
    Dim file_name As String
    Dim lower_case_file_name As String
    Dim upper_case_file_name As String
    Dim sheet_name As String
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    
    Dim data_dec_str As String
    Dim def_comma_string As String
    Dim def_comma_count As Integer
    Dim def_paren_string As String
    Dim def_paren_count As Integer
    


    Dim total_init_values As Integer
    
    file_name = Cells(2, 2).value
    lower_case_file_name = LCase(file_name)
    upper_case_file_name = UCase(file_name)


    START_ROW = 9
    
    DCI_DEFINES_START_ROW = 9
    DCI_DESCRIPTORS_COL = 1
    DCI_DEFINES_COL = 2
    DCI_DATATYPE_COL = 3
    DCI_ARRAY_COUNT_COL = 5
    DCI_LENGTH_COL = 6

    DESCRIP_COL = 1
    DATA_DEFINE_COL = 2
    DATA_LENGTH_COL = 3
    DATA_ID_COL = 4
    DATA_MONITOR_ONLY_COL = 5
    DATA_INITIAL_VAL_COL = 6
    
    ' DCI Sheet defines.
    DCI_DEFINES_START_ROW = 9
    DCI_DESCRIPTORS_COL = 1
    DCI_DEFINES_COL = 2
    DCI_LENGTH_COL = 6

    
    MAX_DATA_SIZE_ROW = 6
    MAX_DATA_SIZE_COL = 3

'*********************
'******     Begin
'*********************
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\" & Cells(3, 2).value
    Set FILE_DCI_C = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(4, 2).value
    Set FILE_DCI_H = fs.CreateTextFile(file_path, True)


'****************************************************************************************************************************
'******     Start Creating the .H file header
'****************************************************************************************************************************
    FILE_DCI_H.WriteLine ("/*")
    FILE_DCI_H.WriteLine ("*****************************************************************************************")
    FILE_DCI_H.WriteLine ("*       $Workfile:$")
    FILE_DCI_H.WriteLine ("*")
    FILE_DCI_H.WriteLine ("*       $Author:$")
    FILE_DCI_H.WriteLine ("*       $Date:$")
    FILE_DCI_H.WriteLine ("*       Copyright © Eaton Corporation. All Rights Reserved.")
    FILE_DCI_H.WriteLine ("*       Creator: Mark A Verheyen")
    FILE_DCI_H.WriteLine ("*")
    FILE_DCI_H.WriteLine ("*       Description:")
    FILE_DCI_H.WriteLine ("*")
    FILE_DCI_H.WriteLine ("*")
    FILE_DCI_H.WriteLine ("*       Code Inspection Date: ")
    FILE_DCI_H.WriteLine ("*")
    FILE_DCI_H.WriteLine ("*       $Header:$")
    FILE_DCI_H.WriteLine ("*****************************************************************************************")
    FILE_DCI_H.WriteLine ("*/")
    FILE_DCI_H.WriteLine ("#ifndef " & upper_case_file_name & "_H")
    FILE_DCI_H.WriteLine ("  #define " & upper_case_file_name & "_H")
    FILE_DCI_H.WriteLine ("")
    FILE_DCI_H.WriteLine ("#include ""DCI_Data.h""")
    FILE_DCI_H.WriteLine ("#include ""DCI_Defines.h""")
    FILE_DCI_H.WriteLine ("#include ""Intercom_App.h""")
    FILE_DCI_H.WriteLine ("")
    

'****************************************************************************************************************************
'******     Gotta calculate the total number of registers we are going to need given a certain length.
'****************************************************************************************************************************
    num_ids = 0
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        'This also takes the data and makes it possible to sort by whether it needs to be
        'checked by the queueing mechanism.
        If (Cells(counter, DATA_MONITOR_ONLY_COL).text <> Empty) Then
            Cells(counter, DATA_MONITOR_ONLY_COL).value = ""
        Else
            Cells(counter, DATA_MONITOR_ONLY_COL).value = "x"
        End If
        num_ids = num_ids + 1
        counter = counter + 1
    Wend
    
'****************************************************************************************************************************
'******     Gotta sort the data by what needs to be monitored and what doesn't need to be.
'****************************************************************************************************************************
    Dim range_string As String
    range_string = START_ROW & ":" & (num_ids + START_ROW)
    Rows(range_string).Sort Key1:=Range("E9"), Order1:=xlAscending, _
        Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
        Orientation:=xlTopToBottom
  
    counter = START_ROW
    mon_only_data_start_row = num_ids           'We are trying to set the beginning of the mon only data.
    'This while loop reverses the things done above so that it appears to the user as if the mon only boxes are checked
    'instead of the other way around.  We needed to flip the check boxes for the sort to work and put the
    'monitor only values at the bottom of the list.  Now we reverse it so it appears to make sense to the
    'spreadsheet user.
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, DATA_MONITOR_ONLY_COL).text <> Empty) Then
            Cells(counter, DATA_MONITOR_ONLY_COL).value = ""
        Else
            If (mon_only_data_start_row > (counter - START_ROW)) Then       'This sets the start of the mon only data.
                mon_only_data_start_row = counter - START_ROW
            End If
            Cells(counter, DATA_MONITOR_ONLY_COL).value = "x"
        End If
        counter = counter + 1
    Wend
  
    FILE_DCI_H.WriteLine ("//*******************************************************")
    FILE_DCI_H.WriteLine ("//******     Definition Structure.")
    FILE_DCI_H.WriteLine ("//*******************************************************")
    FILE_DCI_H.WriteLine ("")
    FILE_DCI_H.WriteLine ("extern const INTERCOM_TARGET_INFO_STRUCT " & lower_case_file_name & "_intercom_dev_def;")
    FILE_DCI_H.WriteLine ("")
    FILE_DCI_H.WriteLine ("")
    FILE_DCI_H.WriteLine ("#endif")


'****************************************************************************************************************************
'******     Start Creating the .C file header
'****************************************************************************************************************************
    FILE_DCI_C.WriteLine ("/*")
    FILE_DCI_C.WriteLine ("*****************************************************************************************")
    FILE_DCI_C.WriteLine ("*       $Workfile:")
    FILE_DCI_C.WriteLine ("*")
    FILE_DCI_C.WriteLine ("*       $Author:$")
    FILE_DCI_C.WriteLine ("*       $Date:$")
    FILE_DCI_C.WriteLine ("*       Copyright© 2001-2004, Eaton Corporation. All Rights Reserved.")
    FILE_DCI_C.WriteLine ("*")
    FILE_DCI_C.WriteLine ("*       $Header:$")
    FILE_DCI_C.WriteLine ("*****************************************************************************************")
    FILE_DCI_C.WriteLine ("*/")
    FILE_DCI_C.WriteLine ("#include ""Includes.h""")
    FILE_DCI_C.WriteLine ("#include """ & file_name & ".h""")

'****************************************************************************************************************************
'******     Handle some of the initial defines.
'****************************************************************************************************************************
    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("//*******************************************************")
    FILE_DCI_C.WriteLine ("//******     Size Definitions follow.")
    FILE_DCI_C.WriteLine ("//*******************************************************")
    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("#define " & upper_case_file_name & "_ITEM_COUNT            " & num_ids)
    FILE_DCI_C.WriteLine ("#define " & upper_case_file_name & "_MAX_BYTE_SIZE            " & Cells(MAX_DATA_SIZE_ROW, MAX_DATA_SIZE_COL).text)
    FILE_DCI_C.WriteLine ("")

    FILE_DCI_C.WriteLine ("//*******************************************************")
    FILE_DCI_C.WriteLine ("//******     Intercom ID List")
    FILE_DCI_C.WriteLine ("//*******************************************************")
    FILE_DCI_C.WriteLine ("")
    
'****************************************************************************************************************************
'******     Handle the creation of the intercom IDs and the init strings if present.
'****************************************************************************************************************************
    total_init_values = 0
    row_counter = START_ROW
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        Cells(row_counter, DATA_ID_COL).value = row_counter - START_ROW
        FILE_DCI_C.WriteLine ("#define " & Cells(row_counter, DATA_DEFINE_COL).text & "_INTERCOM_ID            " & _
                                Cells(row_counter, DATA_ID_COL).text)
                                
        If (Cells(row_counter, DATA_INITIAL_VAL_COL).value <> Empty) Then
            dci_finder = DCI_DEFINES_START_ROW
            While (Worksheets("DCI Descriptors").Range("A1").Cells(dci_finder, DCI_DEFINES_COL).text <> Empty) And _
                    (Worksheets("DCI Descriptors").Range("A1").Cells(dci_finder, DCI_DEFINES_COL).text <> Cells(row_counter, DATA_DEFINE_COL).value)
                dci_finder = dci_finder + 1
            Wend
            If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_finder, DCI_DEFINES_COL).text = Cells(row_counter, DATA_DEFINE_COL).value) Then
                total_init_values = total_init_values + 1
                data_dec_str = "    const " & _
                        Get_Datatype_String(Worksheets("DCI Descriptors").Range("A1").Cells(dci_finder, DCI_DATATYPE_COL).text) & _
                        " " & upper_case_file_name & "_" & Cells(row_counter, DATA_DEFINE_COL).text

                If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_finder, DCI_ARRAY_COUNT_COL).text <> Empty) Then     ' If this is an array of elements.
                    def_comma_string = Replace(Cells(row_counter, DATA_INITIAL_VAL_COL).text, ",", "")
                    def_comma_count = Len(Cells(row_counter, DATA_INITIAL_VAL_COL).text) - Len(def_comma_string)  ' Count the number of commas
                    def_paren_string = Replace(Cells(row_counter, DATA_INITIAL_VAL_COL).text, """", "")
                    def_paren_count = Len(Cells(row_counter, DATA_INITIAL_VAL_COL).text) - Len(def_paren_string)  ' Count the number of parenthesis
                    
                    DCI_DATA_C.WriteLine (data_dec_str & "_INTERCOM_INIT[" & _
                                       Worksheets("DCI Descriptors").Range("A1").Cells(dci_finder, DCI_ARRAY_COUNT_COL).text & "] = ")
                    def_data_str = Cells(row_counter, DATA_INITIAL_VAL_COL).text
                    FILE_DCI_C.WriteLine ("    { ")
                    If Left(def_data_str, 1) = "{" Then         'If the string has some brackets on it then treat it as is (minus the brackets of course.
                        def_data_str = Replace(def_data_str, "{ ", "")
                        def_data_str = Replace(def_data_str, " }", "")
                        def_data_str = Replace(def_data_str, "{", "")
                        def_data_str = Replace(def_data_str, "}", "")
                        FILE_DCI_C.WriteLine ("        " & def_data_str)
                    ElseIf (def_comma_count + def_paren_count) > 0 Then 'Then the string is already full of all initiators.  Don't create duplicates for the array.
                        FILE_DCI_C.WriteLine ("        " & Cells(row_counter, DATA_INITIAL_VAL_COL).text)
                    Else
                        For i = 1 To Worksheets("DCI Descriptors").Range("A1").Cells(dci_finder, DCI_ARRAY_COUNT_COL).text - 1
                            FILE_DCI_C.WriteLine ("        " & Cells(row_counter, DATA_INITIAL_VAL_COL).text & ",")
                        Next i
                        FILE_DCI_C.WriteLine ("        " & Cells(row_counter, DATA_INITIAL_VAL_COL).text)
                    End If
                    FILE_DCI_C.WriteLine ("    }; ")
                Else
                    FILE_DCI_C.WriteLine (data_dec_str & "_INTERCOM_INIT = " & _
                                       Cells(row_counter, DATA_INITIAL_VAL_COL).text & ";")
                End If
            End If
        End If
        row_counter = row_counter + 1
    Wend

    While Cells(row_counter, DATA_DEFINE_COL).text <> Empty
        Cells(row_counter, DATA_ID_COL).value = ""
        row_counter = row_counter + 1
    Wend

    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("//*******************************************************")
    FILE_DCI_C.WriteLine ("//******     Intercom Init Structure Creation.")
    FILE_DCI_C.WriteLine ("//*******************************************************")
    FILE_DCI_C.WriteLine ("")

    FILE_DCI_C.WriteLine ("#define " & upper_case_file_name & "_INTERCOM_INIT_TOTAL        " & total_init_values)
    If (total_init_values <> 0) Then
        FILE_DCI_C.WriteLine ("static const INTERCOM_APP_INIT_DATA " & lower_case_file_name & _
                    "_intercom_init[" & upper_case_file_name & "_INTERCOM_INIT_TOTAL] =")
        FILE_DCI_C.WriteLine ("{")
        row_counter = START_ROW
        While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            If (Cells(row_counter, DATA_INITIAL_VAL_COL).value <> Empty) Then
                FILE_DCI_C.WriteLine ("    {")
                FILE_DCI_C.WriteLine ("        " & Cells(row_counter, DATA_DEFINE_COL).text & "_DCID,")
                FILE_DCI_C.WriteLine ("        (uint8_t*)&" & upper_case_file_name & "_" & Cells(row_counter, DATA_DEFINE_COL).text & "_INTERCOM_INIT,")
                FILE_DCI_C.WriteLine ("    },")
            End If
            row_counter = row_counter + 1
        Wend
        FILE_DCI_C.WriteLine ("};")
    Else
        FILE_DCI_C.WriteLine ("static const INTERCOM_APP_INIT_DATA* " & lower_case_file_name & _
                    "_intercom_init = NULL;")
    End If

'****************************************************************************************************************************
'******     Plop down the monitor only index value.
'****************************************************************************************************************************

    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("//*******************************************************")
    FILE_DCI_C.WriteLine ("//******     The beginning of the monitor only values.")
    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("///The following indicates the index at which the monitor only values start.")
    FILE_DCI_C.WriteLine ("#define " & upper_case_file_name & "_MON_ONLY_INDEX        " & mon_only_data_start_row)

'****************************************************************************************************************************
'******     Start creating the lists
'****************************************************************************************************************************
    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("//*******************************************************")
    FILE_DCI_C.WriteLine ("//******     Intercom ID to DCID List.")
    FILE_DCI_C.WriteLine ("//*******************************************************")
    FILE_DCI_C.WriteLine ("")
    
    If (num_ids = 0) Then
        FILE_DCI_C.WriteLine ("static const INTERCOM_ID_TO_DCID_STRUCT* const " & lower_case_file_name & "_int_id_to_dcid_list = NULL;")
    Else
        FILE_DCI_C.WriteLine ("static const INTERCOM_ID_TO_DCID_STRUCT " & lower_case_file_name & _
                            "_int_id_to_dcid_list[" & upper_case_file_name & "_ITEM_COUNT] = ")
        FILE_DCI_C.WriteLine ("{")
        counter = START_ROW
        While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            If Cells(counter, DATA_MONITOR_ONLY_COL).text <> Empty Then
                FILE_DCI_C.WriteLine ("    {        // " & Cells(counter, DESCRIP_COL).text & " -- Monitor Only")
            Else
                FILE_DCI_C.WriteLine ("    {        // " & Cells(counter, DESCRIP_COL).text)
            End If
            FILE_DCI_C.WriteLine ("        " & Cells(counter, DATA_DEFINE_COL).text & "_DCID,")
            FILE_DCI_C.WriteLine ("    }, ")
            counter = counter + 1
        Wend
        FILE_DCI_C.WriteLine ("};")
    End If

    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("//*******************************************************")
    FILE_DCI_C.WriteLine ("//******     DCID Reverse List.")
    FILE_DCI_C.WriteLine ("//*******************************************************")
    FILE_DCI_C.WriteLine ("")

    Dim debug_string_1 As String
    Dim debug_string_0 As String

    If (num_ids = 0) Then
        FILE_DCI_C.WriteLine ("static const INTERCOM_ID_FROM_DCID_STRUCT* const " & lower_case_file_name & "_int_id_from_dcid_list = NULL;")
    Else
        FILE_DCI_C.WriteLine ("static const INTERCOM_ID_FROM_DCID_STRUCT " & lower_case_file_name & _
                            "_int_id_from_dcid_list[" & upper_case_file_name & "_ITEM_COUNT] =")
        FILE_DCI_C.WriteLine ("{")
        dci_defines_row_ctr = DCI_DEFINES_START_ROW
        While Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> Empty
            local_row_ctr = START_ROW
            While ((Cells(local_row_ctr, DATA_DEFINE_COL).text <> Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text) And _
                            (Cells(local_row_ctr, DATA_DEFINE_COL).value <> Empty) And _
                            (Cells(local_row_ctr, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA))
                debug_string_0 = Cells(local_row_ctr, DATA_DEFINE_COL).text
                debug_string_1 = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text
                local_row_ctr = local_row_ctr + 1
            Wend
            If (Cells(local_row_ctr, DATA_DEFINE_COL).text = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text) Then
                FILE_DCI_C.WriteLine ("    {        // " & Cells(local_row_ctr, DESCRIP_COL).text)
                FILE_DCI_C.WriteLine ("        " & Cells(local_row_ctr, DATA_DEFINE_COL).text & "_DCID,")
                FILE_DCI_C.WriteLine ("        " & Cells(local_row_ctr, DATA_DEFINE_COL).text & "_INTERCOM_ID")
                FILE_DCI_C.WriteLine ("    }, ")
            End If
            dci_defines_row_ctr = dci_defines_row_ctr + 1
        Wend
        FILE_DCI_C.WriteLine ("};")
    End If
    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("//*******************************************************")
    FILE_DCI_C.WriteLine ("//******     Device Intercom Definition.")
    FILE_DCI_C.WriteLine ("//*******************************************************")
    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("static const INTERCOM_TARGET_INFO_STRUCT " & lower_case_file_name & "_intercom_dev_def =")
    FILE_DCI_C.WriteLine ("{")
    FILE_DCI_C.WriteLine ("    " & upper_case_file_name & "_ITEM_COUNT,")
    FILE_DCI_C.WriteLine ("    " & upper_case_file_name & "_MAX_BYTE_SIZE,")
    FILE_DCI_C.WriteLine ("    " & lower_case_file_name & "_int_id_to_dcid_list,")
    FILE_DCI_C.WriteLine ("    " & lower_case_file_name & "_int_id_from_dcid_list,")
    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("    " & upper_case_file_name & "_MON_ONLY_INDEX,")
    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("    " & upper_case_file_name & "_INTERCOM_INIT_TOTAL,")
    FILE_DCI_C.WriteLine ("    " & lower_case_file_name & "_intercom_init")
    FILE_DCI_C.WriteLine ("};")
    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("")

    FILE_DCI_H.Close
    FILE_DCI_C.Close

    Application.Calculation = xlCalculationAutomatic
    
    Call Clear_Intercom_Ignored_Items
    
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

Sub Verify_Intercom_List()

'*********************
'******     Constants
'*********************
    Application.Calculation = xlCalculationManual
    
    Dim sheet_name As String
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    
    START_ROW = 9
    DCI_DEFINES_START_ROW = 9
    DCI_DESCRIPTORS_COL = 1
    DCI_DEFINES_COL = 2
    DCI_LENGTH_COL = 6

    DESCRIP_COL = 1
    DATA_DEFINE_COL = 2
    DATA_LENGTH_COL = 3
    DATA_DESC_MISSING_COL = DESCRIP_COL
    DATA_DEFINE_MISSING_COL = DATA_DEFINE_COL
    DATA_LENGTH_MISSING_COL = DATA_LENGTH_COL
    
    MAX_DATA_SIZE_ROW = 6
    MAX_DATA_SIZE_COL = 3


'*********************
'******     Begin
'*********************
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select

    
'****************************************************************************************************************************
'******     Find where we should add the unadded items.
'****************************************************************************************************************************
    missing_row_counter = START_ROW
    While (Cells(missing_row_counter, DATA_DEFINE_COL).text <> Empty)
        missing_row_counter = missing_row_counter + 1
    Wend

'****************************************************************************************************************************
'******     Find the missing data items
'****************************************************************************************************************************
    dci_defines_row_ctr = DCI_DEFINES_START_ROW
    While Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> Empty
        lookup_string = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text
        Set C = Worksheets(sheet_name).Range("B9:AZ1000").Find(lookup_string, LookAt:=xlWhole)
        If C Is Nothing Then '
            Cells(missing_row_counter, DATA_DESC_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DESCRIPTORS_COL).text
            Cells(missing_row_counter, DATA_DEFINE_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text
            ' The following line is for creating a link between the length and the data id.
            'Cells(missing_counter, DATA_LENGTH_MISSING_COL).Value = "='DCI Descriptors'!" & "$F$" & dci_defines_row_ctr
            ' The next line will just copy the size and not correct it.
            Cells(missing_row_counter, DATA_LENGTH_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_LENGTH_COL).value
            missing_row_counter = missing_row_counter + 1
        End If
        dci_defines_row_ctr = dci_defines_row_ctr + 1
    Wend


    Sheets(sheet_name).Select
'****************************************************************************************************************************
'******     Find the Data items that have been removed from the main list
'****************************************************************************************************************************
    reg_row_ctr = START_ROW
    missing_counter = START_ROW
    While Cells(reg_row_ctr, DATA_DEFINE_COL).text <> Empty
        lookup_string = Cells(reg_row_ctr, DATA_DEFINE_COL).text
        If lookup_string <> BEGIN_IGNORED_DATA Then
            Set C = Worksheets("DCI Descriptors").Range("B9:B1000").Find(lookup_string, LookAt:=xlWhole)
            If C Is Nothing Then
                Cells(reg_row_ctr, DATA_DEFINE_COL).Interior.Color = RGB(255, 0, 0)
                Cells(reg_row_ctr, DESCRIP_COL).Interior.Color = RGB(255, 0, 0)
            End If
        End If
        reg_row_ctr = reg_row_ctr + 1
    Wend


'****************************************************************************************************************************
'******     This will go through the main list and make the lengths correct in the slave sheets
'****************************************************************************************************************************
    dci_defines_row_ctr = DCI_DEFINES_START_ROW
    While Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> Empty
        local_row_ctr = START_ROW
        While (Cells(local_row_ctr, DATA_DEFINE_MISSING_COL).value <> Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text) And _
                        (Cells(local_row_ctr, DATA_DEFINE_MISSING_COL).value <> Empty)
            local_row_ctr = local_row_ctr + 1
        Wend
        If (Cells(local_row_ctr, DATA_DEFINE_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text) Then
            Cells(local_row_ctr, DATA_LENGTH_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_LENGTH_COL).text
        End If
        dci_defines_row_ctr = dci_defines_row_ctr + 1
    Wend

'****************************************************************************************************************************
'******     Find the Data items that have been removed from the main list
'****************************************************************************************************************************
    max_data_size = 0
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If Cells(counter, DATA_LENGTH_COL).value > max_data_size Then
            max_data_size = Cells(counter, DATA_LENGTH_COL).value
        End If
        counter = counter + 1
    Wend
    
    Cells(MAX_DATA_SIZE_ROW, MAX_DATA_SIZE_COL).value = max_data_size

    Application.Calculation = xlCalculationAutomatic

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
Sub Clear_Intercom_Ignored_Items()
    Application.Calculation = xlCalculationManual
    
    Dim BEGIN_IGNORED_DATA As String
    Dim sheet_name As String
    
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    
    START_ROW = 9
    DATA_DEFINE_COL = 2

    sheet_row_ctr = START_ROW
    While (Cells(sheet_row_ctr, DATA_DEFINE_COL).text <> Empty) And _
        (Cells(sheet_row_ctr, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        sheet_row_ctr = sheet_row_ctr + 1
    Wend
    sheet_row_ctr = sheet_row_ctr + 1
    
    Range(Cells(sheet_row_ctr, 1), Cells(50000, 255)).Delete (xlShiftUp)
    
    Application.Calculation = xlCalculationAutomatic

End Sub


Public Function Get_Datatype_String(my_s As String) As String

    If (my_s = "DCI_DTYPE_BOOL") Then
        Get_Datatype_String = "bool"
    ElseIf (my_s = "DCI_DTYPE_UINT8") Or (my_s = "DCI_DTYPE_BYTE") Then
        Get_Datatype_String = "uint8_t"
    ElseIf (my_s = "DCI_DTYPE_SINT8") Then
        Get_Datatype_String = "int8_t"
    ElseIf (my_s = "DCI_DTYPE_UINT16") Or (my_s = "DCI_DTYPE_WORD") Then
        Get_Datatype_String = "uint16_t"
    ElseIf (my_s = "DCI_DTYPE_SINT16") Then
        Get_Datatype_String = "int16_t"
    ElseIf (my_s = "DCI_DTYPE_UINT32") Or (my_s = "DCI_DTYPE_DWORD") Then
        Get_Datatype_String = "uint32_t"
    ElseIf (my_s = "DCI_DTYPE_SINT32") Then
        Get_Datatype_String = "int32_t"
    ElseIf (my_s = "DCI_DTYPE_FLOAT") Then
        Get_Datatype_String = "float32_t"
    ElseIf (my_s = "DCI_DTYPE_LFLOAT") Then
        Get_Datatype_String = "float64_t"
    End If

End Function



