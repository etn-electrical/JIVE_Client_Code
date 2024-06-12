Attribute VB_Name = "Delta_Com_Client_Server"
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'*    FUNCTION HEADER
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
Public Sub Create_Delta_Com_Definition()
    General_Delta_Com_Def (True)
End Sub
Public Sub Create_Virtual_Device()
    General_Delta_Com_Def (False)
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
Public Sub General_Delta_Com_Def(server_implementation As Boolean)

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
    
    Dim hex_string As String
    Dim bit_list_word As Double
    Dim range_string As String
    Dim dci_row As Integer
    Dim param_row As Integer
    Dim dci_length As Integer
    Dim pid_length As Integer

    Dim error As Boolean
    error = False
    
    ' The namespace associated with the generated data.  This will reference the Usage Notes page.  If usage notes is not filled in with the
    ' namespace then a default one will be assigned (Gen_Data::)
    Dim gen_data_namespace As String
    Dim gen_data_namespace_colons As String
    gen_data_namespace = Get_Gen_Data_Namespace()
    gen_data_namespace_colons = gen_data_namespace & "::"

    file_name = Cells(2, 2).value
    lower_case_file_name = LCase(file_name)
    upper_case_file_name = UCase(file_name)


'*********************
'******     Begin with the file creation
'*********************
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\" & Cells(3, 2).value
    Set FILE_DCI_C = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(4, 2).value
    Set FILE_DCI_H = fs.CreateTextFile(file_path, True)


    If (server_implementation = True) Then
        PARAMxDCI_DATA_START_ROW = PARAMxDCI_SRVR_DATA_START_ROW
        PARAMxDCI_PARAM_DESCRIPT_COL = PARAMxDCI_SRVR_PARAM_DESCRIPT_COL
        PARAMxDCI_PARAM_DEFINE_COL = PARAMxDCI_SRVR_PARAM_DEFINE_COL
        PARAMxDCI_MISSING_DCID_DESCRIPT_COL = PARAMxDCI_SRVR_DCI_DESCRIPT_COL
        PARAMxDCI_MISSING_DCID_DEFINE_COL = PARAMxDCI_SRVR_DCI_DEFINE_COL
        PARAMxDCI_PARAM_ID_COL = PARAMxDCI_SRVR_PARAM_ID_COL
        PARAMxDCI_SUBSCRIPTION_COL = PARAMxDCI_SRVR_SUBSCRIPTION_COL
        PARAMxDCI_OWNER_PATRON_COL = PARAMxDCI_SRVR_OWNER_PATRON_COL
        PARAMxDCI_SOURCE_OF_TRUTH_COL = PARAMxDCI_UNUSED_COL
        PARAMxDCI_DCI_DESCRIPT_COL = PARAMxDCI_SRVR_DCI_DESCRIPT_COL
        PARAMxDCI_DCI_DEFINE_COL = PARAMxDCI_SRVR_DCI_DEFINE_COL
        PARAMxDCI_DCID_COL = PARAMxDCI_SRVR_DCID_COL
        PARAMxDCI_PID_TO_DCID_SORT_KEY_COL = PARAMxDCI_SRVR_DCID_SORT_KEY_COL
        PARAMxDCI_MAX_COL = PARAMxDCI_SRVR_MAX_COL
        PARAMxDCI_ERROR_COL = PARAMxDCI_SRVR_ERROR_COL
        PARAMxDCI_WARN_COL = PARAMxDCI_SRVR_WARN_COL
        PARAMxDCI_MAX_ROW = PARAMxDCI_SRVR_MAX_ROW
        PARAMxDCI_DATA_CYCLE_COL = PARAMxDCI_SRVR_DCI_DEFINE_COL
    Else
        PARAMxDCI_DATA_START_ROW = PARAMxDCI_VIRT_DATA_START_ROW
        PARAMxDCI_PARAM_DESCRIPT_COL = PARAMxDCI_VIRT_PARAM_DESCRIPT_COL
        PARAMxDCI_PARAM_DEFINE_COL = PARAMxDCI_VIRT_PARAM_DEFINE_COL
        PARAMxDCI_MISSING_DCID_DESCRIPT_COL = PARAMxDCI_VIRT_DCI_DESCRIPT_COL
        PARAMxDCI_MISSING_DCID_DEFINE_COL = PARAMxDCI_VIRT_DCI_DEFINE_COL
        PARAMxDCI_PARAM_ID_COL = PARAMxDCI_VIRT_PARAM_ID_COL
        PARAMxDCI_SUBSCRIPTION_COL = PARAMxDCI_VIRT_SUBSCRIPTION_COL
        PARAMxDCI_OWNER_PATRON_COL = PARAMxDCI_VIRT_OWNER_PATRON_COL
        PARAMxDCI_SOURCE_OF_TRUTH_COL = PARAMxDCI_VIRT_SOUCE_OF_TRUTH_COL
        PARAMxDCI_DCI_DESCRIPT_COL = PARAMxDCI_VIRT_DCI_DESCRIPT_COL
        PARAMxDCI_DCI_DEFINE_COL = PARAMxDCI_VIRT_DCI_DEFINE_COL
        PARAMxDCI_DCID_COL = PARAMxDCI_VIRT_DCID_COL
        PARAMxDCI_PID_TO_DCID_SORT_KEY_COL = PARAMxDCI_VIRT_PARAM_ID_SORT_KEY_COL
        PARAMxDCI_MAX_COL = PARAMxDCI_VIRT_MAX_COL
        PARAMxDCI_ERROR_COL = PARAMxDCI_VIRT_ERROR_COL
        PARAMxDCI_WARN_COL = PARAMxDCI_VIRT_WARN_COL
        PARAMxDCI_MAX_ROW = PARAMxDCI_VIRT_MAX_ROW
        PARAMxDCI_DATA_CYCLE_COL = PARAMxDCI_VIRT_PARAM_DEFINE_COL
    End If

'****************************************************************************************************************************
'******     Cleanup the sheet.
'****************************************************************************************************************************
    row_counter = PARAMxDCI_DATA_START_ROW
    While (Cells(row_counter, PARAMxDCI_DATA_CYCLE_COL).value <> Empty) And _
            (Cells(row_counter, PARAMxDCI_DATA_CYCLE_COL).value <> BEGIN_IGNORED_DATA)
        row_counter = row_counter + 1
    Wend
    ' Hide the unused columns.
    Range(Cells(PARAMxDCI_DATA_START_ROW, PARAMxDCI_DCID_COL), Cells(PARAMxDCI_DATA_START_ROW, PARAMxDCI_DCID_COL)).Columns.Hidden = True
    Range(Cells(PARAMxDCI_DATA_START_ROW, PARAMxDCI_PARAM_ID_COL), Cells(PARAMxDCI_DATA_START_ROW, PARAMxDCI_PARAM_ID_COL)).Columns.Hidden = True
    Range(Cells(PARAMxDCI_DATA_START_ROW, PARAMxDCI_PID_TO_DCID_SORT_KEY_COL), Cells(PARAMxDCI_DATA_START_ROW, PARAMxDCI_PID_TO_DCID_SORT_KEY_COL)).Columns.Hidden = True
    
    ' Correct the alignment of the columns.
    Range(Cells(PARAMxDCI_DATA_START_ROW, PARAMxDCI_SUBSCRIPTION_COL), Cells(row_counter, PARAMxDCI_SUBSCRIPTION_COL)).HorizontalAlignment = xlCenter
    Range(Cells(PARAMxDCI_DATA_START_ROW, PARAMxDCI_OWNER_PATRON_COL), Cells(row_counter, PARAMxDCI_OWNER_PATRON_COL)).HorizontalAlignment = xlCenter
    Range(Cells(PARAMxDCI_DATA_START_ROW, PARAMxDCI_SOURCE_OF_TRUTH_COL), Cells(row_counter, PARAMxDCI_SOURCE_OF_TRUTH_COL)).HorizontalAlignment = xlCenter
    
    Range(Cells(PARAMxDCI_DATA_START_ROW, PARAMxDCI_PARAM_DESCRIPT_COL), Cells(row_counter, PARAMxDCI_PARAM_DESCRIPT_COL)).HorizontalAlignment = xlLeft
    Range(Cells(PARAMxDCI_DATA_START_ROW, PARAMxDCI_PARAM_DEFINE_COL), Cells(row_counter, PARAMxDCI_PARAM_DEFINE_COL)).HorizontalAlignment = xlLeft
    Range(Cells(PARAMxDCI_DATA_START_ROW, PARAMxDCI_DCI_DESCRIPT_COL), Cells(row_counter, PARAMxDCI_DCI_DESCRIPT_COL)).HorizontalAlignment = xlLeft
    Range(Cells(PARAMxDCI_DATA_START_ROW, PARAMxDCI_DCI_DEFINE_COL), Cells(row_counter, PARAMxDCI_DCI_DEFINE_COL)).HorizontalAlignment = xlLeft

'****************************************************************************************************************************
'******     Find where we should add the unadded items.
'****************************************************************************************************************************
    missing_row_counter = PARAMxDCI_DATA_START_ROW
    While (Cells(missing_row_counter, PARAMxDCI_DATA_CYCLE_COL).value <> Empty)
        missing_row_counter = missing_row_counter + 1
        Cells(missing_row_counter, PARAMxDCI_UNUSED_COL).Clear
    Wend
    Range(Cells(missing_row_counter, 1), Cells(ActiveSheet.Rows.Count, ActiveSheet.Columns.Count)).Rows.Delete

'****************************************************************************************************************************
'******     Find the missing data items
'****************************************************************************************************************************
    dci_defines_row_ctr = DCI_DEFINES_START_ROW
    While Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).value <> Empty
        lookup_string = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).value
        Set c = Worksheets(sheet_name).Range("B9:AZ5000").Find(lookup_string, LookAt:=xlWhole)
        If c Is Nothing Then
            Cells(missing_row_counter, PARAMxDCI_MISSING_DCID_DESCRIPT_COL).value = _
                Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DESCRIPTORS_COL).value
            Cells(missing_row_counter, PARAMxDCI_MISSING_DCID_DEFINE_COL).value = _
                Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).value
            missing_row_counter = missing_row_counter + 1
        End If
        dci_defines_row_ctr = dci_defines_row_ctr + 1
    Wend
    Range(Cells(missing_row_counter, 1), Cells(ActiveSheet.Rows.Count, ActiveSheet.Columns.Count)).Rows.Delete

'****************************************************************************************************************************
'******     Gotta calculate the total number of parameters we are going to have.
'******     Fill in the sheet with the actual DCIDs
'****************************************************************************************************************************
    num_ids = 0

    error = False
    warning = False
    Cells(PARAMxDCI_DATA_START_ROW - 1, PARAMxDCI_ERROR_COL).Interior.ColorIndex = xlNone
    Cells(PARAMxDCI_DATA_START_ROW - 1, PARAMxDCI_ERROR_COL).value = "Error?"
    Cells(PARAMxDCI_DATA_START_ROW - 1, PARAMxDCI_WARN_COL).Interior.ColorIndex = xlNone
    Cells(PARAMxDCI_DATA_START_ROW - 1, PARAMxDCI_WARN_COL).value = "Warning?"

    counter = PARAMxDCI_DATA_START_ROW
    While (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> Empty) And _
            (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> BEGIN_IGNORED_DATA)

        Cells(counter, PARAMxDCI_ERROR_COL).value = ""
        Cells(counter, PARAMxDCI_ERROR_COL).Interior.ColorIndex = xlNone
        Cells(counter, PARAMxDCI_WARN_COL).value = ""
        Cells(counter, PARAMxDCI_WARN_COL).Interior.ColorIndex = xlNone

        dci_row = Get_DCI_Row(Cells(counter, PARAMxDCI_DCI_DEFINE_COL).value)
        If (dci_row <> 0) Then
            Cells(counter, PARAMxDCI_DCID_COL).value = dci_row - DCI_DEFINES_START_ROW
            Cells(counter, PARAMxDCI_DCI_DESCRIPT_COL).Interior.ColorIndex = xlNone
            Cells(counter, PARAMxDCI_DCI_DEFINE_COL).Interior.ColorIndex = xlNone
            dci_length = Worksheets("DCI Descriptors").Cells(dci_row, DCI_LENGTH_COL).value
            Cells(counter, PARAMxDCI_WARN_COL).value = ""
' The following warnings are causing more noise than needed.  It is possible for these conditions to occur.  Further error checking should be added in the future.
'            If ((Worksheets("DCI Descriptors").Cells(dci_row, PATRON_RAM_VAL_WRITEABLE_COL).Value = Empty) And (Cells(counter, PARAMxDCI_SUBSCRIPTION_COL).Value <> Empty)) Then
'                Cells(counter, PARAMxDCI_WARN_COL).Interior.Color = RGB(255, 255, 0)
'                Cells(counter, PARAMxDCI_WARN_COL).Value = Cells(counter, PARAMxDCI_WARN_COL).Value & "DCID Ram Value Not Writeable "
'                warning = True
'            End If
'            If ((Worksheets("DCI Descriptors").Cells(dci_row, PATRON_INIT_VAL_WRITEABLE_ACCESS_COL).Value = Empty) And (Cells(counter, PARAMxDCI_SUBSCRIPTION_COL).Value <> Empty)) Then
'                Cells(counter, PARAMxDCI_WARN_COL).Interior.Color = RGB(255, 255, 0)
'                Cells(counter, PARAMxDCI_WARN_COL).Value = Cells(counter, PARAMxDCI_WARN_COL).Value & "DCID Init Value Not Writeable"
'                warning = True
'            End If
        Else
            Cells(counter, PARAMxDCI_ERROR_COL).value = "DCID Does Not Exist Error"
            Cells(counter, PARAMxDCI_ERROR_COL).Interior.Color = RGB(255, 0, 0)
            error = True
        End If

        If (Cells(counter, PARAMxDCI_PARAM_DEFINE_COL).value <> "") Then
            param_row = Get_Param_ID_Row(Cells(counter, PARAMxDCI_PARAM_DEFINE_COL).value)
        Else
            param_row = 0
        End If
    
        If (param_row <> 0) Then
            Cells(counter, PARAMxDCI_PARAM_ID_COL).value = Get_Param_ID_From_Row(param_row)
            pid_length = Get_Param_Length_From_Row(param_row)
            If ((dci_length <> pid_length) And (pid_length <> 0)) Then
                Cells(counter, PARAMxDCI_ERROR_COL).Interior.Color = RGB(255, 0, 0)
                Cells(counter, PARAMxDCI_ERROR_COL).value = "Size Mismatch"
                error = True
            End If
        Else
            Cells(counter, PARAMxDCI_ERROR_COL).value = "PID Does Not Exist Error"
            Cells(counter, PARAMxDCI_ERROR_COL).Interior.Color = RGB(255, 0, 0)
            error = True
        End If
        
        num_ids = num_ids + 1
        counter = counter + 1
    Wend

    If (error = False) Then
        '****************************************************************************************************************************
        '******     Start Creating the .H file header
        '****************************************************************************************************************************
        FILE_DCI_H.WriteLine ("/**")
        FILE_DCI_H.WriteLine ("*****************************************************************************************")
        FILE_DCI_H.WriteLine ("*   @file")
        FILE_DCI_H.WriteLine ("*")
        FILE_DCI_H.WriteLine ("*   @brief Provides the crosslinking between DCID and Parameter ID.")
        FILE_DCI_H.WriteLine ("*")
        FILE_DCI_H.WriteLine ("*   @details This cross list is used by deltacom to translate DCID into Parameter ID")
        FILE_DCI_H.WriteLine ("*   and back again.")
        FILE_DCI_H.WriteLine ("*")
        FILE_DCI_H.WriteLine ("*   @version")
        FILE_DCI_H.WriteLine ("*   C++ Style Guide Version 1.0")
        FILE_DCI_H.WriteLine ("*")
        FILE_DCI_H.WriteLine ("*   @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
        FILE_DCI_H.WriteLine ("*")
        FILE_DCI_H.WriteLine ("*****************************************************************************************")
        FILE_DCI_H.WriteLine ("*/")
        FILE_DCI_H.WriteLine ("#ifndef " & upper_case_file_name & "_H")
        FILE_DCI_H.WriteLine ("    #define " & upper_case_file_name & "_H")
        FILE_DCI_H.WriteLine ("")
        FILE_DCI_H.WriteLine ("#include ""Includes.h""")
        FILE_DCI_H.WriteLine ("#include ""DCI_Data.h""")
        FILE_DCI_H.WriteLine ("#include ""Delta_Com_Defines.h""")
        FILE_DCI_H.WriteLine ("#include ""Delta_Data.h""")
        
        FILE_DCI_H.WriteLine ("")
        FILE_DCI_H.WriteLine ("")
        
        FILE_DCI_H.WriteLine ("//*******************************************************")
        FILE_DCI_H.WriteLine ("//******     Definition Structure.")
        FILE_DCI_H.WriteLine ("//*******************************************************")
        FILE_DCI_H.WriteLine ("extern const Delta_Data::data_def_t " & upper_case_file_name & ";")
        FILE_DCI_H.WriteLine ("")
        FILE_DCI_H.WriteLine ("")
        FILE_DCI_H.WriteLine ("#endif")
    
    
    
        '****************************************************************************************************************************
        '******     Start Creating the .CPP file header
        '****************************************************************************************************************************
        FILE_DCI_C.WriteLine ("/**")
        FILE_DCI_C.WriteLine ("*****************************************************************************************")
        FILE_DCI_C.WriteLine ("*   @file")
        FILE_DCI_C.WriteLine ("*   @details See header file for module overview.")
        FILE_DCI_C.WriteLine ("*   @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
        FILE_DCI_C.WriteLine ("*")
        FILE_DCI_C.WriteLine ("*****************************************************************************************")
        FILE_DCI_C.WriteLine ("*/")
        FILE_DCI_C.WriteLine ("#include """ & file_name & ".h""")
        FILE_DCI_C.WriteLine ("#include ""Param_List.h""")
    
        '****************************************************************************************************************************
        '******     Gotta sort the data first by PID
        '****************************************************************************************************************************
    
        If (num_ids <> 0) Then
            range_string = PARAMxDCI_DATA_START_ROW & ":" & (num_ids + PARAMxDCI_DATA_START_ROW)
            Rows(range_string).Sort Key1:=Cells(PARAMxDCI_DATA_START_ROW, PARAMxDCI_PARAM_ID_COL), _
            Order1:=xlAscending, _
                Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
                Orientation:=xlTopToBottom
' Server    Rows(range_string).Sort Key1:=Range("H9"), Order1:=xlAscending, _
'                Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
'                Orientation:=xlTopToBottom
' Virtual   Rows(range_string).Sort Key1:=Range("C9"), Order1:=xlAscending, _
'                Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
'                Orientation:=xlTopToBottom
        End If

    
        '******     Fill in the sort key list.
        sort_key_counter = 0
        counter = PARAMxDCI_DATA_START_ROW
        While (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> Empty) And (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> BEGIN_IGNORED_DATA)
'           Cells(counter, PARAMxDCI_SRVR_DCID_SORT_KEY_COL).Value = sort_key_counter
'           Cells(counter, PARAMxDCI_VIRT_PARAM_ID_SORT_KEY_COL).Value = sort_key_counter
            Cells(counter, PARAMxDCI_PID_TO_DCID_SORT_KEY_COL).value = sort_key_counter
            sort_key_counter = sort_key_counter + 1
            counter = counter + 1
        Wend
    
        '****************************************************************************************************************************
        '******     Handle the Param ID to DCID Sort List.
        '****************************************************************************************************************************
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        FILE_DCI_C.WriteLine ("//******     PID To DCID")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("static const Gen_Data::mpl_t " & upper_case_file_name & "_PID_DCID_LIST_LEN = " & num_ids & "U;")
        FILE_DCI_C.WriteLine ("")
    
        If (num_ids <> 0) Then
            FILE_DCI_C.WriteLine ("static const Delta_Com::pid_to_dcid_map_t " & upper_case_file_name & _
                        "_PID_TO_DCID[" & upper_case_file_name & "_PID_DCID_LIST_LEN] =")
            FILE_DCI_C.WriteLine ("{")
    
            counter = PARAMxDCI_DATA_START_ROW
            While (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> Empty) And (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> BEGIN_IGNORED_DATA)
                FILE_DCI_C.WriteLine (Chr(9) & "{")
                FILE_DCI_C.WriteLine (Chr(9) & Chr(9) & gen_data_namespace_colons & Cells(counter, PARAMxDCI_PARAM_DEFINE_COL).value & "_PID,      //" & Cells(counter, PARAMxDCI_PARAM_DESCRIPT_COL).value)
                FILE_DCI_C.WriteLine (Chr(9) & Chr(9) & Cells(counter, PARAMxDCI_DCI_DEFINE_COL).value & "_DCID,      //" & Cells(counter, PARAMxDCI_DCI_DESCRIPT_COL).value)
                FILE_DCI_C.WriteLine (Chr(9) & "},")
                counter = counter + 1
            Wend
            FILE_DCI_C.WriteLine ("};")
        Else
            FILE_DCI_C.WriteLine ("static const Delta_Com::pid_to_dcid_map_t* const " & upper_case_file_name & _
                "_PID_TO_DCID = nullptr;")
        End If
        
        
        '****************************************************************************************************************************
        '******     Handle The Subscription Bit - List Lengths.
        '******
        '****************************************************************************************************************************
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        FILE_DCI_C.WriteLine ("//******     Bit-List Lengths.")
        FILE_DCI_C.WriteLine ("//******     bit lists are represented as arrays of words.")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        FILE_DCI_C.WriteLine ("")
    
        BIT_LIST_PACK_LEN = 32
        
        bit_list_length = Fix((num_ids + BIT_LIST_PACK_LEN - 1) / BIT_LIST_PACK_LEN)
        FILE_DCI_C.WriteLine ("static const Gen_Data::mpl_t " & upper_case_file_name & "_BIT_LIST_LEN = " & bit_list_length & "U;")
        
        
        '****************************************************************************************************************************
        '******     Handle The Owner Bit - List.
        '******
        '****************************************************************************************************************************
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        FILE_DCI_C.WriteLine ("//******     Owner/!Patron Bit-List Follows.")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("")
        
        If (num_ids <> 0) Then
            FILE_DCI_C.WriteLine ("static const uint32_t " & upper_case_file_name & _
                        "_OWNER_PATRON_NOT_BIT_LIST[" & upper_case_file_name & "_BIT_LIST_LEN] =")
            FILE_DCI_C.WriteLine ("{")
    
            counter = PARAMxDCI_DATA_START_ROW
            While (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> Empty) And (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> BEGIN_IGNORED_DATA)
                offset_counter = 0
                bit_list_word = 0
                While (offset_counter < BIT_LIST_PACK_LEN) And (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> BEGIN_IGNORED_DATA)
                    If (Cells(counter, PARAMxDCI_OWNER_PATRON_COL).value <> "") Then
                        bit_list_word = bit_list_word + 2 ^ offset_counter
                        FILE_DCI_C.WriteLine ("//    " & Cells(counter, PARAMxDCI_PARAM_DEFINE_COL).value & "_PID,")
                    End If
                    offset_counter = offset_counter + 1
                    counter = counter + 1
                Wend
                hex_string = Convert_Double_To_Hex_String(bit_list_word)
                FILE_DCI_C.WriteLine ("         0x" & hex_string & "U,")
            Wend
            FILE_DCI_C.WriteLine ("};")
        Else
            FILE_DCI_C.WriteLine ("static const uint32_t* const " & upper_case_file_name & _
                        "_OWNER_PATRON_NOT_BIT_LIST = nullptr;")
        End If
            
            
        '****************************************************************************************************************************
        '******     Handle creating both sets of Subscription Bit Lists (General and trusted)
        '******
        '****************************************************************************************************************************
                
        Dim general_access As Boolean
        general_access = False
        
        list_counter = 0
        While (list_counter < 2)
            list_counter = list_counter + 1
            
            '****************************************************************************************************************************
            '******     Handle The Subscription Bit - List.
            '******
            '****************************************************************************************************************************
            FILE_DCI_C.WriteLine ("")
            FILE_DCI_C.WriteLine ("//*******************************************************")
            FILE_DCI_C.WriteLine ("//******     Subscription Bit-List Follows.")
            If (general_access = True) Then
                FILE_DCI_C.WriteLine ("//******     General Access Level (This will use a patron level access)")
            End If
            FILE_DCI_C.WriteLine ("//*******************************************************")
            FILE_DCI_C.WriteLine ("")
            FILE_DCI_C.WriteLine ("")
            
            If (num_ids <> 0) Then
                If (general_access = True) Then
                    FILE_DCI_C.WriteLine ("static const uint32_t " & upper_case_file_name & _
                            "_SUBSCRIPTION_BIT_LIST_PATRON[" & upper_case_file_name & "_BIT_LIST_LEN] =")
                Else
                    FILE_DCI_C.WriteLine ("static const uint32_t " & upper_case_file_name & _
                            "_SUBSCRIPTION_BIT_LIST_OWNER[" & upper_case_file_name & "_BIT_LIST_LEN] =")
                End If
                FILE_DCI_C.WriteLine ("{")
        
                counter = PARAMxDCI_DATA_START_ROW
                While (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> Empty) And (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> BEGIN_IGNORED_DATA)
                    offset_counter = 0
                    bit_list_word = 0
                    While (offset_counter < BIT_LIST_PACK_LEN) And (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> BEGIN_IGNORED_DATA)
                        If (Cells(counter, PARAMxDCI_SUBSCRIPTION_COL).value <> "") And (Cells(counter, PARAMxDCI_SOURCE_OF_TRUTH_COL).value = "") Then
                            If (general_access = True) Then    'If we are general access then we should scan the patron list to verify if we can write.
                                dci_row = Get_DCI_Row(Cells(counter, PARAMxDCI_DCI_DEFINE_COL).value)
                                If (Worksheets("DCI Descriptors").Cells(dci_row, PATRON_RAM_VAL_WRITEABLE_COL).value <> "") Then     'We are only checking the RAM value if it is writable.  It gets too complicated and is extremely rare for the RAM value to be RW and the other parameters not RW.
                                    bit_list_word = bit_list_word + 2 ^ offset_counter
                                    FILE_DCI_C.WriteLine ("//    " & Cells(counter, PARAMxDCI_PARAM_DEFINE_COL).value & "_PID,")
                                End If
                            Else
                                bit_list_word = bit_list_word + 2 ^ offset_counter
                                FILE_DCI_C.WriteLine ("//    " & Cells(counter, PARAMxDCI_PARAM_DEFINE_COL).value & "_PID,")
                            End If
                        End If
                        offset_counter = offset_counter + 1
                        counter = counter + 1
                    Wend
                    
                    hex_string = Convert_Double_To_Hex_String(bit_list_word)
                    FILE_DCI_C.WriteLine ("         0x" & hex_string & "U,")
                Wend
                FILE_DCI_C.WriteLine ("};")
            Else
                If (general_access = True) Then
                    FILE_DCI_C.WriteLine ("static const uint32_t " & upper_case_file_name & _
                            "_SUBSCRIPTION_BIT_LIST_PATRON = nullptr;")
                Else
                    FILE_DCI_C.WriteLine ("static const uint32_t* const " & upper_case_file_name & _
                            "_SUBSCRIPTION_BIT_LIST_OWNER = nullptr;")
                End If
            End If
            
            
            '****************************************************************************************************************************
            '******     Handle The Source of Truth Bit - List.
            '******
            '****************************************************************************************************************************
            
            FILE_DCI_C.WriteLine ("")
            FILE_DCI_C.WriteLine ("//*******************************************************")
            FILE_DCI_C.WriteLine ("//******     Source Of Truth Bit-List Follows.")
            If (general_access = True) Then
                FILE_DCI_C.WriteLine ("//******     General Access Level (This will use a patron level access)")
            End If
            FILE_DCI_C.WriteLine ("//*******************************************************")
            FILE_DCI_C.WriteLine ("")
            FILE_DCI_C.WriteLine ("")
            
            If (num_ids <> 0) Then
                If (general_access = True) Then
                    FILE_DCI_C.WriteLine ("static const uint32_t " & upper_case_file_name & _
                            "_CLIENT_SOT_BIT_LIST_PATRON[" & upper_case_file_name & "_BIT_LIST_LEN] =")
                Else
                    FILE_DCI_C.WriteLine ("static const uint32_t " & upper_case_file_name & _
                            "_CLIENT_SOT_BIT_LIST_OWNER[" & upper_case_file_name & "_BIT_LIST_LEN] =")
                End If
                FILE_DCI_C.WriteLine ("{")
        
                counter = PARAMxDCI_DATA_START_ROW
                While (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> Empty) And (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> BEGIN_IGNORED_DATA)
                    offset_counter = 0
                    bit_list_word = 0
                    While (offset_counter < BIT_LIST_PACK_LEN) And (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> BEGIN_IGNORED_DATA)
                        If (Cells(counter, PARAMxDCI_SUBSCRIPTION_COL).value <> "") And (Cells(counter, PARAMxDCI_SOURCE_OF_TRUTH_COL).value <> "") Then
                            If (general_access = True) Then    'If we are general access then we should scan the patron list to verify if we can write.
                                dci_row = Get_DCI_Row(Cells(counter, PARAMxDCI_DCI_DEFINE_COL).value)
                                If (Worksheets("DCI Descriptors").Cells(dci_row, PATRON_RAM_VAL_WRITEABLE_COL).value <> "") Then     'We are only checking the RAM value if it is writable.  It gets too complicated and is extremely rare for the RAM value to be RW and the other parameters not RW.
                                    bit_list_word = bit_list_word + 2 ^ offset_counter
                                    FILE_DCI_C.WriteLine ("//    " & Cells(counter, PARAMxDCI_PARAM_DEFINE_COL).value & "_PID,")
                                End If
                            Else
                                bit_list_word = bit_list_word + 2 ^ offset_counter
                                FILE_DCI_C.WriteLine ("//    " & Cells(counter, PARAMxDCI_PARAM_DEFINE_COL).value & "_PID,")
                            End If
                        End If
                        offset_counter = offset_counter + 1
                        counter = counter + 1
                    Wend
                    hex_string = Convert_Double_To_Hex_String(bit_list_word)
                    FILE_DCI_C.WriteLine ("         0x" & hex_string & "U,")
                    FILE_DCI_C.WriteLine ("")
                Wend
                FILE_DCI_C.WriteLine ("};")
            Else
                If (general_access = True) Then
                    FILE_DCI_C.WriteLine ("static const uint32_t* const " & upper_case_file_name & _
                            "_CLIENT_SOT_BIT_LIST_PATRON = nullptr;")
                Else
                    FILE_DCI_C.WriteLine ("static const uint32_t* const " & upper_case_file_name & _
                            "_CLIENT_SOT_BIT_LIST_OWNER = nullptr;")
                End If
            End If
        
            general_access = True
        Wend
    
    '****************************************************************************************************************************
    '******     Handle The Value Clear Bit - List.
    '******
    '****************************************************************************************************************************
        
    
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        FILE_DCI_C.WriteLine ("//******     Value Clear Bit-List Follows.")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("")
        
        If (num_ids <> 0) Then
            FILE_DCI_C.WriteLine ("static const uint32_t " & upper_case_file_name & _
                        "_VALUE_CLEAR_BIT_LIST[" & upper_case_file_name & "_BIT_LIST_LEN] =")
            FILE_DCI_C.WriteLine ("{")
    
            counter = PARAMxDCI_DATA_START_ROW
            While (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> Empty) And (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> BEGIN_IGNORED_DATA)
                offset_counter = 0
                bit_list_word = 0
                While (offset_counter < BIT_LIST_PACK_LEN) And (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> BEGIN_IGNORED_DATA)
                    If (Cells(counter, PARAMxDCI_SUBSCRIPTION_COL).value = "c") Or (Cells(counter, PARAMxDCI_SUBSCRIPTION_COL).value = "C") Then
                        bit_list_word = bit_list_word + 2 ^ offset_counter
                        FILE_DCI_C.WriteLine ("//    " & Cells(counter, PARAMxDCI_PARAM_DEFINE_COL).value & "_PID,")
                    End If
                    offset_counter = offset_counter + 1
                    counter = counter + 1
                Wend
                hex_string = Convert_Double_To_Hex_String(bit_list_word)
                FILE_DCI_C.WriteLine ("         0x" & hex_string & "U,")
            Wend
            FILE_DCI_C.WriteLine ("};")
        Else
            FILE_DCI_C.WriteLine ("static const uint32_t* const " & upper_case_file_name & _
                        "_VALUE_CLEAR_BIT_LIST = nullptr;")
        End If


    '****************************************************************************************************************************
    '******     Now sort the list for converting from DID to PID.
    '****************************************************************************************************************************
        If (num_ids <> 0) Then
            range_string = PARAMxDCI_DATA_START_ROW & ":" & (num_ids + PARAMxDCI_DATA_START_ROW - 1)
            Rows(range_string).Sort Key1:=Cells(PARAMxDCI_DATA_START_ROW, PARAMxDCI_DCID_COL), _
            Order1:=xlAscending, _
                Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
                Orientation:=xlTopToBottom
        End If

        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        FILE_DCI_C.WriteLine ("//******     DCID to PID")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        FILE_DCI_C.WriteLine ("")
    
        If (num_ids <> 0) Then
            FILE_DCI_C.WriteLine ("static const Gen_Data::mpl_t " & upper_case_file_name & _
                        "_DCID_TO_PID[" & upper_case_file_name & "_PID_DCID_LIST_LEN] =")
            FILE_DCI_C.WriteLine ("{")
    
            counter = PARAMxDCI_DATA_START_ROW
            While (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> Empty) And (Cells(counter, PARAMxDCI_DATA_CYCLE_COL).value <> BEGIN_IGNORED_DATA)
                FILE_DCI_C.WriteLine ("        " & Cells(counter, PARAMxDCI_PID_TO_DCID_SORT_KEY_COL).value & "U,      //" & Cells(counter, PARAMxDCI_PARAM_DESCRIPT_COL).value)
                counter = counter + 1
            Wend
            FILE_DCI_C.WriteLine ("};")
        Else
            FILE_DCI_C.WriteLine ("static const Gen_Data::mpl_t* const " & upper_case_file_name & _
                    "_DCID_TO_PID = nullptr;")
        End If
        
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        FILE_DCI_C.WriteLine ("//******     Delta Com Data Definition")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("const Delta_Data::data_def_t " & upper_case_file_name & " = ")
        FILE_DCI_C.WriteLine ("{")
        FILE_DCI_C.WriteLine ("    " & upper_case_file_name & "_PID_DCID_LIST_LEN,      //uint16_t pid_dcid_list_len;")
        FILE_DCI_C.WriteLine ("    " & upper_case_file_name & "_PID_TO_DCID,      //pid_to_dcid_map_t const* pid_to_dcid_map;")
        FILE_DCI_C.WriteLine ("    " & upper_case_file_name & "_DCID_TO_PID,      //uint16_t* dcid_to_pid_map;")
        FILE_DCI_C.WriteLine ("    reinterpret_cast<const uint8_t*>(" & upper_case_file_name & "_OWNER_PATRON_NOT_BIT_LIST),      //uint8_t * owner_patron_not_list;")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("    reinterpret_cast<const uint8_t*>(" & upper_case_file_name & "_SUBSCRIPTION_BIT_LIST_OWNER ),      //uint8_t * subscription_bit_list;")
        FILE_DCI_C.WriteLine ("    reinterpret_cast<const uint8_t*>(" & upper_case_file_name & "_CLIENT_SOT_BIT_LIST_OWNER ),      //uint8_t * client_sot_subscription_bit_list;")
        FILE_DCI_C.WriteLine ("    reinterpret_cast<const uint8_t*>(" & upper_case_file_name & "_SUBSCRIPTION_BIT_LIST_PATRON ),      //uint8_t * general_subscription_bit_list;")
        FILE_DCI_C.WriteLine ("    reinterpret_cast<const uint8_t*>(" & upper_case_file_name & "_CLIENT_SOT_BIT_LIST_PATRON ),      //uint8_t * general_client_sot_subscription_bit_list;")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("    reinterpret_cast<const uint8_t*>(" & upper_case_file_name & "_VALUE_CLEAR_BIT_LIST),      //uint8_t * value_clear;")
        FILE_DCI_C.WriteLine ("};")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("")
           
    
    '****************************************************************************************************************************
    '******     Now return to a sort of the data by DCID.
    '****************************************************************************************************************************
        range_string = PARAMxDCI_DATA_START_ROW & ":" & (num_ids + PARAMxDCI_DATA_START_ROW - 1)
        Rows(range_string).Sort Key1:=Range("C9"), Order1:=xlAscending, _
            Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
            Orientation:=xlTopToBottom
        
        Range(Cells(PARAMxDCI_DATA_START_ROW, PARAMxDCI_PID_TO_DCID_SORT_KEY_COL), Cells(PARAMxDCI_MAX_ROW, PARAMxDCI_PID_TO_DCID_SORT_KEY_COL)).Clear
        If (num_ids <> 0) Then
            Cells(PARAMxDCI_DATA_START_ROW, PARAMxDCI_PID_TO_DCID_SORT_KEY_COL).value = "---"
            Range(Cells(PARAMxDCI_DATA_START_ROW, PARAMxDCI_PID_TO_DCID_SORT_KEY_COL), Cells(num_ids + PARAMxDCI_DATA_START_ROW - 1, PARAMxDCI_PID_TO_DCID_SORT_KEY_COL)).FillDown
        End If
        
    End If

    If (error = True) Then
        Cells(PARAMxDCI_DATA_START_ROW - 1, PARAMxDCI_ERROR_COL).Interior.Color = RGB(255, 0, 0)
    End If
    If (warning = True) Then
        Cells(PARAMxDCI_DATA_START_ROW - 1, PARAMxDCI_WARN_COL).Interior.Color = RGB(255, 255, 0)
    End If
    
    ' The following line will clear out the rows so it does not contain a bunch of information in the export.
    Range(Cells(missing_row_counter, 1), Cells(ActiveSheet.Rows.Count, ActiveSheet.Columns.Count)).Rows.Delete

    FILE_DCI_H.Close
    FILE_DCI_C.Close

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
Public Function Get_DCI_Row(DCI_STRING As String) As Integer

    dci_defines_row_ctr = DCI_DEFINES_START_ROW
    While (Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).value <> Empty) And _
            (Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).value <> DCI_STRING)
        dci_defines_row_ctr = dci_defines_row_ctr + 1
    Wend
    
    If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).value = Empty) Then
        Get_DCI_Row = 0
    Else
        Get_DCI_Row = dci_defines_row_ctr
    End If
    
End Function

'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'*    FUNCTION HEADER
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
Public Function Get_Param_Length_From_Row(pid_row As Integer) As Integer

    Dim DCI_DB_Creator As Workbook
 
    our_workbook = ActiveWorkbook.name

    file_name = Worksheets("Delta Com Parameters").Range("A1").Cells(6, 2).text
    
    Set work_book = Workbooks(file_name)
    
    If work_book Is Nothing Then
        file_path = ActiveWorkbook.Path & "\" & Worksheets("Delta Com Parameters").Range("A1").Cells(5, 2).text & file_name
        Workbooks.Open (file_path)
    End If
    Workbooks(our_workbook).Activate
    
    Get_Param_Length_From_Row = work_book.Worksheets("DCom Parameters").Range("A1").Cells(pid_row, PID_LENGTH_COL).value
    
End Function

'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'*    FUNCTION HEADER
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
Public Function Get_Param_ID_From_Row(pid_row As Integer) As Integer

    Dim DCI_DB_Creator As Workbook
    
    our_workbook = ActiveWorkbook.name

    file_name = Worksheets("Delta Com Parameters").Range("A1").Cells(6, 2).text
    
    Set work_book = Workbooks(file_name)
    
    If work_book Is Nothing Then
        file_path = ActiveWorkbook.Path & "\" & Worksheets("Delta Com Parameters").Range("A1").Cells(5, 2).text & file_name
        Workbooks.Open (file_path)
    End If
    Workbooks(our_workbook).Activate
    
    Get_Param_ID_From_Row = work_book.Worksheets("DCom Parameters").Range("A1").Cells(pid_row, PID_ID_COL).value

End Function

Public Function Convert_Double_To_Hex_String(value As Double) As String

    Dim hex_string_1 As String
    Dim hex_string_2 As String
    
    Dim top_half As Long
    Dim small_value As Long
    
    top_half = Fix(value / (2 ^ 16))
    small_value = (value - (top_half * 2 ^ 16))
    
    hex_string_1 = Hexify(top_half, 2)
    hex_string_2 = Hexify(small_value, 2)
    
    Convert_Double_To_Hex_String = hex_string_1 & hex_string_2

End Function


'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'*    FUNCTION HEADER
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
Public Function Get_Param_ID_Row(pid_string As String) As Integer

    Dim DCI_DB_Creator As Workbook
    
    our_workbook = ActiveWorkbook.name
    
    file_name = Worksheets("Delta Com Parameters").Range("A1").Cells(6, 2).text
    
    Set work_book = Workbooks(file_name)
    
    If work_book Is Nothing Then
        file_path = ActiveWorkbook.Path & "\" & Worksheets("Delta Com Parameters").Range("A1").Cells(5, 2).text & file_name
        Workbooks.Open (file_path)
    End If
    Workbooks(our_workbook).Activate
    
    defines_row_ctr = PID_DEFINES_START_ROW
    
    While (work_book.Worksheets("DCom Parameters").Range("A1").Cells(defines_row_ctr, PID_DEFINES_COL).value <> Empty) And _
            (work_book.Worksheets("DCom Parameters").Range("A1").Cells(defines_row_ctr, PID_DEFINES_COL).value <> pid_string)
        defines_row_ctr = defines_row_ctr + 1
    Wend
    
    If (work_book.Worksheets("DCom Parameters").Range("A1").Cells(defines_row_ctr, PID_DEFINES_COL).value = Empty) Then
        Get_Param_ID_Row = 0
    Else
        Get_Param_ID_Row = defines_row_ctr
    End If

End Function



