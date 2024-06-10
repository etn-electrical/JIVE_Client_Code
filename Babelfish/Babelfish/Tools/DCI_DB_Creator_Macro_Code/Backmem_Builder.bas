Attribute VB_Name = "Backmem_Builder"
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'*    FUNCTION HEADER
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
Public Sub Create_Backmem_Definition()

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

    BACKMEM_MISSING_DCID_DESCRIPT_COL = BACKMEM_DCI_DESCRIPT_COL
    BACKMEM_MISSING_DCID_DEFINE_COL = BACKMEM_DCI_DEFINE_COL
    BACKMEM_PID_TO_DCID_SORT_KEY_COL = BACKMEM_DCID_SORT_KEY_COL
    BACKMEM_DATA_CYCLE_COL = BACKMEM_DCI_DEFINE_COL

'****************************************************************************************************************************
'******     Find where we should add the unadded items and cleanup the sheet
'****************************************************************************************************************************
    row_counter = BACKMEM_DATA_START_ROW
    While (Cells(row_counter, BACKMEM_DATA_CYCLE_COL).value <> Empty) And _
            (Cells(row_counter, BACKMEM_DATA_CYCLE_COL).value <> BEGIN_IGNORED_DATA)
        row_counter = row_counter + 1
    Wend
    missing_row_start = row_counter
    missing_row_counter = row_counter + 1
    Range(Cells(missing_row_counter, 1), Cells(BACKMEM_MAX_ROW, BACKMEM_MAX_COL)).Clear
    
    ' Hide the internal-use-only columns.
    Range(Cells(BACKMEM_DATA_START_ROW, BACKMEM_DCID_COL), Cells(BACKMEM_DATA_START_ROW, BACKMEM_DCID_COL)).Columns.Hidden = True
    Range(Cells(BACKMEM_DATA_START_ROW, BACKMEM_PARAM_ID_COL), Cells(BACKMEM_DATA_START_ROW, BACKMEM_PARAM_ID_COL)).Columns.Hidden = True
    Range(Cells(BACKMEM_DATA_START_ROW, BACKMEM_DCID_SIZE_COL), Cells(BACKMEM_DATA_START_ROW, BACKMEM_DCID_SIZE_COL)).Columns.Hidden = True
    Range(Cells(BACKMEM_DATA_START_ROW, BACKMEM_NV_ADDRESS_COL), Cells(BACKMEM_DATA_START_ROW, BACKMEM_NV_ADDRESS_COL)).Columns.Hidden = True
    Range(Cells(BACKMEM_DATA_START_ROW, BACKMEM_MAX_COL), Cells(BACKMEM_DATA_START_ROW, BACKMEM_MAX_COL)).Columns.Hidden = True
    
    ' Correct the alignment of the columns.
    Range(Cells(BACKMEM_DATA_START_ROW, BACKMEM_PARAM_DESCRIPT_COL), Cells(row_counter, BACKMEM_PARAM_DESCRIPT_COL)).HorizontalAlignment = xlLeft
    Range(Cells(BACKMEM_DATA_START_ROW, BACKMEM_PARAM_DEFINE_COL), Cells(row_counter, BACKMEM_PARAM_DEFINE_COL)).HorizontalAlignment = xlLeft
    Range(Cells(BACKMEM_DATA_START_ROW, BACKMEM_DCI_DESCRIPT_COL), Cells(row_counter, BACKMEM_DCI_DESCRIPT_COL)).HorizontalAlignment = xlLeft
    Range(Cells(BACKMEM_DATA_START_ROW, BACKMEM_DCI_DEFINE_COL), Cells(row_counter, BACKMEM_DCI_DEFINE_COL)).HorizontalAlignment = xlLeft

'****************************************************************************************************************************
'******     Find the missing data items
'****************************************************************************************************************************
    Dim BACKMEM_CHECKBOX_COL As Long
    BACKMEM_CHECKBOX_COL = Worksheets("DCI Descriptors").Range("EB1").column
    Dim dci_defines_row_ctr As Long
    dci_defines_row_ctr = DCI_DEFINES_START_ROW
    While Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).value <> Empty
    If DCID_Is_Box_Checked(dci_defines_row_ctr, BACKMEM_CHECKBOX_COL) Then
        lookup_string = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).value
        Set C = Worksheets(sheet_name).Range("B9:B2000").Find(lookup_string, LookAt:=xlWhole)
        If C Is Nothing Then
            Cells(missing_row_counter, BACKMEM_MISSING_DCID_DESCRIPT_COL).value = _
            Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DESCRIPTORS_COL).value
            Cells(missing_row_counter, BACKMEM_MISSING_DCID_DEFINE_COL).value = _
            Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).value
            missing_row_counter = missing_row_counter + 1
        End If
    End If
    dci_defines_row_ctr = dci_defines_row_ctr + 1
    Wend

'****************************************************************************************************************************
'******     Gotta calculate the total number of parameters we are going to have.
'******     Fill in the sheet with the actual DCIDs
'****************************************************************************************************************************
    num_ids = 0

    error = False
    warning = False
    Cells(BACKMEM_DATA_START_ROW - 1, BACKMEM_ERROR_COL).Interior.ColorIndex = xlNone
    Cells(BACKMEM_DATA_START_ROW - 1, BACKMEM_ERROR_COL).value = "Error?"
    Cells(BACKMEM_DATA_START_ROW - 1, BACKMEM_WARN_COL).Interior.ColorIndex = xlNone
    Cells(BACKMEM_DATA_START_ROW - 1, BACKMEM_WARN_COL).value = "Warning?"

    counter = BACKMEM_DATA_START_ROW
    nv_address = BACKMEM_NV_FIRST_ITEM_ADDR
    While (Cells(counter, BACKMEM_DATA_CYCLE_COL).value <> Empty) And _
            (Cells(counter, BACKMEM_DATA_CYCLE_COL).value <> BEGIN_IGNORED_DATA)

        Cells(counter, BACKMEM_ERROR_COL).value = ""
        Cells(counter, BACKMEM_ERROR_COL).Interior.ColorIndex = xlNone
        Cells(counter, BACKMEM_WARN_COL).value = ""
        Cells(counter, BACKMEM_WARN_COL).Interior.ColorIndex = xlNone

        dci_row = Get_DCI_Row(Cells(counter, BACKMEM_DCI_DEFINE_COL).value)
        If (dci_row <> 0) Then
            Cells(counter, BACKMEM_DCID_COL).value = dci_row - DCI_DEFINES_START_ROW
            Cells(counter, BACKMEM_DCI_DESCRIPT_COL).Interior.ColorIndex = xlNone
            Cells(counter, BACKMEM_DCI_DEFINE_COL).Interior.ColorIndex = xlNone
            dci_length = Worksheets("DCI Descriptors").Cells(dci_row, DCI_LENGTH_COL).value
            dci_descr = Worksheets("DCI Descriptors").Cells(dci_row, DCI_DESCRIPTORS_COL).value
            Cells(counter, BACKMEM_WARN_COL).value = ""
        Else
            Cells(counter, BACKMEM_ERROR_COL).value = "DCID Does Not Exist Error"
            Cells(counter, BACKMEM_ERROR_COL).Interior.Color = RGB(255, 0, 0)
            dci_length = 0
            dci_descr = "???"
            error = True
        End If
        Cells(counter, BACKMEM_DCID_SIZE_COL).value = dci_length
        Cells(counter, BACKMEM_DCI_DESCRIPT_COL).value = dci_descr
        Cells(counter, BACKMEM_NV_ADDRESS_COL).value = nv_address
        nv_address = nv_address + dci_length + BACKMEM_NV_CRC_SIZE

        If (Cells(counter, BACKMEM_PARAM_DEFINE_COL).value <> "") Then
            param_row = Get_Param_ID_Row(Cells(counter, BACKMEM_PARAM_DEFINE_COL).value)
        Else
            param_row = 0
        End If
    
        If (param_row <> 0) Then
            Cells(counter, BACKMEM_PARAM_ID_COL).value = Get_Param_ID_From_Row(param_row)
            pid_length = Get_Param_Length_From_Row(param_row)
            If ((dci_length <> pid_length) And (pid_length <> 0)) Then
                Cells(counter, BACKMEM_ERROR_COL).Interior.Color = RGB(255, 0, 0)
                Cells(counter, BACKMEM_ERROR_COL).value = "Size Mismatch"
                error = True
            End If
            pid_descr = Get_Param_Description_From_Row(param_row)
        Else
            Cells(counter, BACKMEM_ERROR_COL).value = "PID Does Not Exist Error"
            Cells(counter, BACKMEM_ERROR_COL).Interior.Color = RGB(255, 0, 0)
            pid_descr = "?????"
            error = True
        End If
        Cells(counter, BACKMEM_PARAM_DESCRIPT_COL).value = pid_descr
        
        num_ids = num_ids + 1
        counter = counter + 1
    Wend

    If (error = False) Then
        '****************************************************************************************************************************
        '******     Sort the list by DCID.
        '****************************************************************************************************************************
        If (num_ids <> 0) Then
            range_string = BACKMEM_DATA_START_ROW & ":" & (num_ids + BACKMEM_DATA_START_ROW - 1)
            Rows(range_string).Sort Key1:=Cells(BACKMEM_DATA_START_ROW, BACKMEM_DCID_COL), _
            Order1:=xlAscending, _
                Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
                Orientation:=xlTopToBottom
        End If

        '****************************************************************************************************************************
        '******     Start Creating the .H file header
        '****************************************************************************************************************************
        FILE_DCI_H.WriteLine ("/**")
        FILE_DCI_H.WriteLine ("*****************************************************************************************")
        FILE_DCI_H.WriteLine ("*   @file")
        FILE_DCI_H.WriteLine ("*")
        FILE_DCI_H.WriteLine ("*   @brief Provides the definition of parameters stored in Backup Memory / RTC module.")
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
        FILE_DCI_H.WriteLine ("#include ""Delta_Com.h""")
        FILE_DCI_H.WriteLine ("#include ""NV_Ctrl.h""")
        FILE_DCI_H.WriteLine ("")
        FILE_DCI_H.WriteLine ("/**")
        FILE_DCI_H.WriteLine (" *  @brief Constant number of items in backup memory definition table.")
        FILE_DCI_H.WriteLine (" */")
        FILE_DCI_H.WriteLine ("extern const uint16_t BACKMEM_DATA_NUM_ITEMS;")
        FILE_DCI_H.WriteLine ("")
        FILE_DCI_H.WriteLine ("/**")
        FILE_DCI_H.WriteLine (" *  @brief Constant for size of the checksum that NV_Ctrl inserts after our items.")
        FILE_DCI_H.WriteLine (" */")
        FILE_DCI_H.WriteLine ("extern const uint32_t BACKMEM_NV_CRC_SIZE;")
        FILE_DCI_H.WriteLine ("")
        FILE_DCI_H.WriteLine ("/**")
        FILE_DCI_H.WriteLine (" *  @brief Constant for the location of identity block in backup memory.")
        FILE_DCI_H.WriteLine (" */")
        FILE_DCI_H.WriteLine ("extern const uint16_t BACKMEM_NV_IDENTITY_ADDR;")
        FILE_DCI_H.WriteLine ("")
        FILE_DCI_H.WriteLine ("/**")
        FILE_DCI_H.WriteLine (" *  @brief Constant for the location of the first item in backup memory.")
        FILE_DCI_H.WriteLine (" */")
        FILE_DCI_H.WriteLine ("extern const uint16_t BACKMEM_NV_FIRST_ITEM_ADDR;")
        FILE_DCI_H.WriteLine ("")
        FILE_DCI_H.WriteLine ("/**")
        FILE_DCI_H.WriteLine (" *  @brief Structure of entry in backup memory definition table.")
        FILE_DCI_H.WriteLine (" */")
        FILE_DCI_H.WriteLine ("struct backmem_data_def_t")
        FILE_DCI_H.WriteLine ("{")
        FILE_DCI_H.WriteLine ("    /** DCI ID of the parameter */")
        FILE_DCI_H.WriteLine ("    DCI_ID_TD                               dci_id;")
        FILE_DCI_H.WriteLine ("")
        FILE_DCI_H.WriteLine ("    /** MPL ID of the parameter */")
        FILE_DCI_H.WriteLine ("    Delta_Com::param_id_t                   param_id;")
        FILE_DCI_H.WriteLine ("")
        FILE_DCI_H.WriteLine ("    /** NV address in FRAM of Backup Memory / RTC module */")
        FILE_DCI_H.WriteLine ("    NV_CTRL_ADDRESS_TD                      item_address;")
        FILE_DCI_H.WriteLine ("};")
        FILE_DCI_H.WriteLine ("")
        FILE_DCI_H.WriteLine ("/**")
        FILE_DCI_H.WriteLine (" *  @brief Backup memory definition table.")
        FILE_DCI_H.WriteLine (" */")
        FILE_DCI_H.WriteLine ("extern const backmem_data_def_t BACKMEM_DATA_TABLE[];")
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
    
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("const uint16_t BACKMEM_DATA_NUM_ITEMS = " & num_ids & "U;")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("const uint32_t BACKMEM_NV_CRC_SIZE = " & BACKMEM_NV_CRC_SIZE & "U;")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("const uint16_t BACKMEM_NV_IDENTITY_ADDR = " & BACKMEM_NV_IDENTITY_ADDR & "U;")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("const uint16_t BACKMEM_NV_FIRST_ITEM_ADDR = " & BACKMEM_NV_FIRST_ITEM_ADDR & "U;")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("const backmem_data_def_t BACKMEM_DATA_TABLE[ BACKMEM_DATA_NUM_ITEMS ] =")
        FILE_DCI_C.WriteLine ("{")

        If (num_ids <> 0) Then
            counter = BACKMEM_DATA_START_ROW
            While (Cells(counter, BACKMEM_DATA_CYCLE_COL).value <> Empty) And (Cells(counter, BACKMEM_DATA_CYCLE_COL).value <> BEGIN_IGNORED_DATA)
                'FILE_DCI_C.Writeline ("        " & Cells(counter, BACKMEM_PID_TO_DCID_SORT_KEY_COL).Value & "U,      //" & Cells(counter, BACKMEM_PARAM_DESCRIPT_COL).Value)
                FILE_DCI_C.WriteLine ("    {")
                FILE_DCI_C.WriteLine ("        " & Cells(counter, BACKMEM_DCI_DEFINE_COL).value & "_DCID,")
                FILE_DCI_C.WriteLine ("        " & gen_data_namespace_colons & Cells(counter, BACKMEM_PARAM_DEFINE_COL).value & "_PID,")
                FILE_DCI_C.WriteLine ("        " & Cells(counter, BACKMEM_NV_ADDRESS_COL).value & "U")
                FILE_DCI_C.WriteLine ("    },")
                counter = counter + 1
            Wend
        Else
            FILE_DCI_C.WriteLine (" THERE IS NOTHING TO EXPORT !")
        End If
        
        FILE_DCI_C.WriteLine ("};")
        FILE_DCI_C.WriteLine ("")
           
    End If

    If (error = True) Then
        Cells(BACKMEM_DATA_START_ROW - 1, BACKMEM_ERROR_COL).Interior.Color = RGB(255, 0, 0)
    End If
    If (warning = True) Then
        Cells(BACKMEM_DATA_START_ROW - 1, BACKMEM_WARN_COL).Interior.Color = RGB(255, 255, 0)
    End If

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
Public Function Get_Param_Description_From_Row(pid_row As Integer) As String

    Dim DCI_DB_Creator As Workbook
 
    our_workbook = ActiveWorkbook.name

    file_name = Worksheets("Delta Com Parameters").Range("A1").Cells(6, 2).text
    
    Set work_book = Workbooks(file_name)
    
    If work_book Is Nothing Then
        file_path = ActiveWorkbook.Path & "\" & Worksheets("Delta Com Parameters").Range("A1").Cells(5, 2).text & file_name
        Workbooks.Open (file_path)
    End If
    Workbooks(our_workbook).Activate
    
    Get_Param_Description_From_Row = work_book.Worksheets("DCom Parameters").Range("A1").Cells(pid_row, PID_DESCRIPTORS_COL).value
    
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









