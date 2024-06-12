Attribute VB_Name = "IOT_Builder"
'**************************
'**************************
'IOT Construction sheet.
'**************************
'**************************

Sub Create_IOT_DCI_Xfer()

    '*********************
    '****** Constants
    '*********************
    'IOT sheet data
    START_ROW = 12
    DESCRIP_COL = 1
    DATA_DEFINE_COL = 2
    TAG_ID_COL = 3
    ARRAY_OFFSET_COL = 27
    GROUP_NO_START_COL = 4
    GROUP_NO_END_COL = 26
    
    'IOT Groups sheet data
    GROUP_START_ROW = 9
    GROUP_DATA_DEFINE_COL = 2
    CADENCE_DCI_COL = 3
    SEE_ME_COL = 4
    STORE_ME_COL = 5
    STORE_ME_ON_INTERVAL_COL = 6
    PUB_ALL_ON_CONNECT_COL = (STORE_ME_ON_INTERVAL_COL + 1)
    GROUP_DESCRIPTION_COL = 1
    NO_COLOR = 0
    RED_COLOR_FOR_ERROR = 3
    
    Const UINT8_TO_STR = 3
    Const SINT8_TO_STR = 4
    Const UINT16_TO_STR = 5
    Const SINT16_TO_STR = 6
    Const UINT32_TO_STR = 10
    Const SINT32_TO_STR = 11
    Const INT64_TO_STR = 20
    Const FLOAT_TO_STR = 15
    Const DFLOAT_TO_STR = 23
    Const NULL_CHAR = 1
    
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    Dim sheet_name As String
    Dim col_counter As Integer
    Dim cnt_x As Integer
    Dim Total_IoT_Groups As Integer
    
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select

    file_name = Cells(3, 3).value
    lower_case_file_name = LCase(file_name)
    upper_case_file_name = UCase(file_name)
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\" & Cells(4, 3).value
    Set IOT_DCI_C = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(5, 3).value
    Set IOT_DCI_H = fs.CreateTextFile(file_path, True)
    
    'Making font type and size consistent.
    Worksheets(sheet_name).Range("A:AD").Font.name = "Arial"
    Worksheets(sheet_name).Range("A:AD").Font.Size = 10
    
    '****************************************************************************************************************************
    '****************************************************************************************************************************
    '****** Start Creating the .H file header
    '****************************************************************************************************************************
    '****************************************************************************************************************************
    IOT_DCI_H.WriteLine ("/**")
    IOT_DCI_H.WriteLine ("*****************************************************************************************")
    IOT_DCI_H.WriteLine ("*  @file")
    IOT_DCI_H.WriteLine ("*  @brief Sample database file for IOT_DCI communication.")
    IOT_DCI_H.WriteLine ("*")
    IOT_DCI_H.WriteLine ("*  @details")
    IOT_DCI_H.WriteLine ("*")
    IOT_DCI_H.WriteLine ("*  @copyright 2018 Eaton Corporation. All Rights Reserved.")
    IOT_DCI_H.WriteLine ("*")
    IOT_DCI_H.WriteLine (" *****************************************************************************************")
    IOT_DCI_H.WriteLine ("*/")
    IOT_DCI_H.WriteLine ("")
    IOT_DCI_H.WriteLine ("#ifndef " & upper_case_file_name & "_H")
    IOT_DCI_H.WriteLine ("#define " & upper_case_file_name & "_H")
    IOT_DCI_H.WriteLine ("")
    IOT_DCI_H.WriteLine ("#include ""DCI.h""")
    IOT_DCI_H.WriteLine ("#include ""DCI_Defines.h""")
    IOT_DCI_H.WriteLine ("#include ""IoT_DCI.h""")
    IOT_DCI_H.WriteLine ("")
    
    IOT_DCI_H.WriteLine ("//*******************************************************")
    IOT_DCI_H.WriteLine ("//******     IOT structure and size follow.")
    IOT_DCI_H.WriteLine ("//*******************************************************")
    IOT_DCI_H.WriteLine ("")
    IOT_DCI_H.WriteLine ("extern const iot_target_info_st_t " & lower_case_file_name & "_target_info;")
    
    '****************************************************************************************************************************
    '****************************************************************************************************************************
    '****** Start Creating the .C file header
    '****************************************************************************************************************************
    '****************************************************************************************************************************
    IOT_DCI_C.WriteLine ("/**")
    IOT_DCI_C.WriteLine ("*****************************************************************************************")
    IOT_DCI_C.WriteLine ("*  @file")
    IOT_DCI_C.WriteLine ("*  @details See header file for module overview.")
    IOT_DCI_C.WriteLine ("*  @copyright 2018 Eaton Corporation. All Rights Reserved.")
    IOT_DCI_C.WriteLine ("*")
    IOT_DCI_C.WriteLine ("*****************************************************************************************")
    IOT_DCI_C.WriteLine ("*/")
    IOT_DCI_C.WriteLine ("")
    IOT_DCI_C.WriteLine ("#include ""Includes.h""")
    IOT_DCI_C.WriteLine ("#include """ & file_name & ".h""")
    IOT_DCI_C.WriteLine ("")

    '****************************************************************************************************************************
    '****** Sort the rows according to the increasing order of the IOT tag ID.
    '****************************************************************************************************************************
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        counter = counter + 1
    Wend
    end_of_data = counter - 1

    Rows(START_ROW & ":" & end_of_data).Sort Key1:=Cells(START_ROW, TAG_ID_COL), _
        Order1:=xlAscending, Header:=xlGuess, OrderCustom:=1, MatchCase:=False, Orientation:=xlTopToBottom
        
    '****************************************************************************************************************************
    '****** Give error if any parameter name is added multiple times in the Code Define column in the IOT Sheet.
    '****************************************************************************************************************************
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        Cells(counter, DATA_DEFINE_COL).Font.Color = RGB(0, 0, 0)
        counter = counter + 1
    Wend
                
    counter = START_ROW
    NextCounter = 0
    FaultCounter = 0
    
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        NextCounter = 1
        While (Cells(counter + NextCounter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter + NextCounter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            If (Cells(counter, DATA_DEFINE_COL).text = Cells(counter + NextCounter, DATA_DEFINE_COL).text) Then
                FaultCounter = FaultCounter + 1
                Cells(counter + NextCounter, DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
            End If
            NextCounter = NextCounter + 1
        Wend
        counter = counter + 1
    Wend
    
    If (FaultCounter <> 0) Then
        MsgBox "Error: One or more IOT parameters are added multiple times"
        Exit Sub
    End If
    
    '****************************************************************************************************************************
    '****** Check if the parameter mentioned in the IOT sheet is present in the DCI sheet.
    '****************************************************************************************************************************
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        Cells(counter, DATA_DEFINE_COL).Font.Color = RGB(0, 0, 0)
        counter = counter + 1
    Wend
    
    counter = START_ROW
    FaultCounter = 0
    Dim row_number As Integer
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        row_number = 0
        row_number = Find_DCID_Row(Cells(counter, DATA_DEFINE_COL).text)
        If (row_number = 0) Then
            FaultCounter = FaultCounter + 1
            Cells(counter, DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
        End If
        counter = counter + 1
    Wend
    
    If (FaultCounter <> 0) Then
        MsgBox "Error: One or more IOT parameters are not present in the DCI descriptor sheet"
        Exit Sub
    End If
    
    '****************************************************************************************************************************
    '****** Give the error if the same tag ID is assigned to multiple IOT parameters.
    '****************************************************************************************************************************
    counter = START_ROW
    While (Cells(counter, TAG_ID_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        Cells(counter, TAG_ID_COL).Font.Color = RGB(0, 0, 0)
        counter = counter + 1
    Wend
                
    counter = START_ROW
    NextCounter = 0
    FaultCounter = 0
    
    While (Cells(counter, TAG_ID_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
    NextCounter = 1
        While (Cells(counter + NextCounter, TAG_ID_COL).text <> Empty) And (Cells(counter + NextCounter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            If (Cells(counter, TAG_ID_COL).text = Cells(counter + NextCounter, TAG_ID_COL).text) Then
                FaultCounter = FaultCounter + 1
                Cells(counter + NextCounter, TAG_ID_COL).Font.Color = RGB(255, 0, 0)
            End If
            NextCounter = NextCounter + 1
        Wend
        counter = counter + 1
    Wend
    
    If (FaultCounter <> 0) Then
        MsgBox "Error: One or more IOT parameters are assigned the same TAG IDs"
        Exit Sub
    End If
    
    '****************************************************************************************************************************
    '****** Total number of parameters in the IOT DCI sheet.
    '****************************************************************************************************************************
    IOT_DCI_C.WriteLine ("//*******************************************************")
    IOT_DCI_C.WriteLine ("//******     IOT Misc Definition ")
    IOT_DCI_C.WriteLine ("//*******************************************************")
    IOT_DCI_C.WriteLine ("")
    
    num_parameters = 0
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        num_parameters = num_parameters + 1
        counter = counter + 1
    Wend
    
    total_size = 0
    counter = START_ROW
    max_val = 0
    Dim val As Integer
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        row_number = 0
        row_number = Find_DCID_Row(Cells(counter, DATA_DEFINE_COL).text)
        
        If (row_number <> 0) Then
            datatype = Worksheets("DCI Descriptors").Cells(row_number, DCI_DATATYPE_COL).text
            Select Case datatype
                Case "DCI_DTYPE_STRING8", "DCI_DTYPE_BOOL"
                    val = Worksheets("DCI Descriptors").Cells(row_number, TOTAL_LENGTH_COL).value + NULL_CHAR
                    
                Case "DCI_DTYPE_UINT8"
                    val = (((Worksheets("DCI Descriptors").Cells(row_number, TOTAL_LENGTH_COL).value) / _
                    (Worksheets("DCI Descriptors").Cells(row_number, DATATYPE_SIZE).value)) * UINT8_TO_STR) + NULL_CHAR
                    
                Case "DCI_DTYPE_SINT8"
                    val = (((Worksheets("DCI Descriptors").Cells(row_number, TOTAL_LENGTH_COL).value) / _
                    (Worksheets("DCI Descriptors").Cells(row_number, DATATYPE_SIZE).value)) * SINT8_TO_STR) + NULL_CHAR
                    
                Case "DCI_DTYPE_UINT16"
                    val = (((Worksheets("DCI Descriptors").Cells(row_number, TOTAL_LENGTH_COL).value) / _
                    (Worksheets("DCI Descriptors").Cells(row_number, DATATYPE_SIZE).value)) * UINT16_TO_STR) + NULL_CHAR
                    
                Case "DCI_DTYPE_SINT16"
                    val = (((Worksheets("DCI Descriptors").Cells(row_number, TOTAL_LENGTH_COL).value) / _
                    (Worksheets("DCI Descriptors").Cells(row_number, DATATYPE_SIZE).value)) * SINT16_TO_STR) + NULL_CHAR
                    
                Case "DCI_DTYPE_UINT32"
                    val = (((Worksheets("DCI Descriptors").Cells(row_number, TOTAL_LENGTH_COL).value) / _
                    (Worksheets("DCI Descriptors").Cells(row_number, DATATYPE_SIZE).value)) * UINT32_TO_STR) + NULL_CHAR
                    
                Case "DCI_DTYPE_SINT32"
                    val = (((Worksheets("DCI Descriptors").Cells(row_number, TOTAL_LENGTH_COL).value) / _
                    (Worksheets("DCI Descriptors").Cells(row_number, DATATYPE_SIZE).value)) * SINT32_TO_STR) + NULL_CHAR
                    
                Case "DCI_DTYPE_UINT64", "DCI_DTYPE_SINT64"
                    val = (((Worksheets("DCI Descriptors").Cells(row_number, TOTAL_LENGTH_COL).value) / _
                    (Worksheets("DCI Descriptors").Cells(row_number, DATATYPE_SIZE).value)) * INT64_TO_STR) + NULL_CHAR

                Case "DCI_DTYPE_FLOAT"
                    val = (((Worksheets("DCI Descriptors").Cells(row_number, TOTAL_LENGTH_COL).value) / _
                    (Worksheets("DCI Descriptors").Cells(row_number, DATATYPE_SIZE).value)) * FLOAT_TO_STR) + NULL_CHAR
                    
                Case "DCI_DTYPE_DFLOAT"
                    val = (((Worksheets("DCI Descriptors").Cells(row_number, TOTAL_LENGTH_COL).value) / _
                    (Worksheets("DCI Descriptors").Cells(row_number, DATATYPE_SIZE).value)) * DFLOAT_TO_STR) + NULL_CHAR
            End Select
            
            total_size = total_size + val
            If (val > max_val) Then
                max_val = val
            End If
            
        End If
        counter = counter + 1
    Wend
    
    IOT_DCI_C.WriteLine ("static const uint16_t " & upper_case_file_name & "_TOTAL_CHANNELS = " & num_parameters & "U;")
    IOT_DCI_H.WriteLine ("static const uint16_t " & upper_case_file_name & "_BUFFER_SIZE = " & total_size + 5 & "U;")
    IOT_DCI_H.WriteLine ("static const uint16_t " & upper_case_file_name & "_MAX_VAL_LENGTH = " & max_val + 2 & "U;")
    IOT_DCI_H.WriteLine ("")
    IOT_DCI_H.WriteLine ("#endif")
    
    '****************************************************************************************************************************
    '****** Total number of cadence groups in the IOT Groups sheet.
    '****************************************************************************************************************************
    num_parameters = 0
    counter = GROUP_START_ROW
    While (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        num_parameters = num_parameters + 1
        counter = counter + 1
    Wend
    
    Total_IoT_Groups = num_parameters
    
    IOT_DCI_C.WriteLine ("static const uint16_t " & upper_case_file_name & "_TOTAL_GROUPS = " & num_parameters & ";")
    IOT_DCI_C.WriteLine ("")
    
    
    IOT_DCI_C.WriteLine ("")
    IOT_DCI_C.WriteLine ("//*******************************************************")
    IOT_DCI_C.WriteLine ("//******     IOT Channel List ")
    IOT_DCI_C.WriteLine ("//*******************************************************")
    IOT_DCI_C.WriteLine ("")
    
    '****************************************************************************************************************************
    '****** Give error if Group number is added multiple times in the Group number column in the IOT Groups Sheet.
    '****************************************************************************************************************************
    counter = GROUP_START_ROW
    While (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).text <> Empty) And (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).Font.Color = RGB(0, 0, 0)
        counter = counter + 1
    Wend
                
    counter = GROUP_START_ROW
    NextCounter = 0
    FaultCounter = 0
    
    While (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).text <> Empty) And (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        NextCounter = 1
        While (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter + NextCounter, GROUP_DATA_DEFINE_COL).text <> Empty) And (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter + NextCounter, GROUP_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            If (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).text = ActiveWorkbook.Worksheets("IOT Groups").Cells(counter + NextCounter, GROUP_DATA_DEFINE_COL).text) Then
                FaultCounter = FaultCounter + 1
                ActiveWorkbook.Worksheets("IOT Groups").Cells(counter + NextCounter, GROUP_DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
            End If
            NextCounter = NextCounter + 1
        Wend
        counter = counter + 1
    Wend
    
    If (FaultCounter <> 0) Then
        Sheets("IOT Groups").Select
        MsgBox "Error: One or more Group numbers are added multiple times in IOT Groups sheet"
        Exit Sub
    End If
    
    '****************************************************************************************************************************
    '****** Give error if Cadence DCI is added multiple times in the Cadence DCI column in the IOT Groups Sheet.
    '****************************************************************************************************************************
    counter = GROUP_START_ROW
    While (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, CADENCE_DCI_COL).text <> Empty) And (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, CADENCE_DCI_COL).Font.Color = RGB(0, 0, 0)
        counter = counter + 1
    Wend
                
    counter = GROUP_START_ROW
    NextCounter = 0
    FaultCounter = 0
    
    While (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, CADENCE_DCI_COL).text <> Empty) And (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        NextCounter = 1
        While (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter + NextCounter, CADENCE_DCI_COL).text <> Empty) And (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter + NextCounter, GROUP_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            If (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, CADENCE_DCI_COL).text = ActiveWorkbook.Worksheets("IOT Groups").Cells(counter + NextCounter, CADENCE_DCI_COL).text) Then
                FaultCounter = FaultCounter + 1
                ActiveWorkbook.Worksheets("IOT Groups").Cells(counter + NextCounter, CADENCE_DCI_COL).Font.Color = RGB(255, 0, 0)
            End If
            NextCounter = NextCounter + 1
        Wend
        counter = counter + 1
    Wend
    
    If (FaultCounter <> 0) Then
        Sheets("IOT Groups").Select
        MsgBox "Error: One or more Cadance DCIs are added multiple times in IOT Groups sheet"
        Exit Sub
    End If

    '****************************************************************************************************************************
    '****** Give error if the cadence DCI mentioned in the IOT Groups sheet is not present in the DCI descriptors sheet.
    '****************************************************************************************************************************
    counter = GROUP_START_ROW
    While (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, CADENCE_DCI_COL).text <> Empty) And (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, CADENCE_DCI_COL).Font.Color = RGB(0, 0, 0)
        counter = counter + 1
    Wend
    
    counter = GROUP_START_ROW
    FaultCounter = 0
    While (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, CADENCE_DCI_COL).text <> Empty) And (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        row_number = 0
        row_number = Find_DCID_Row(ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, CADENCE_DCI_COL).text)
        If (row_number = 0) Then
            FaultCounter = FaultCounter + 1
            ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, CADENCE_DCI_COL).Font.Color = RGB(255, 0, 0)
        End If
        counter = counter + 1
    Wend
        
    If (FaultCounter <> 0) Then
        Sheets("IOT Groups").Select
        MsgBox "Error: One or more Cadence DCI in IOT Groups sheet are not present in the DCI descriptor sheet"
        Exit Sub
    End If
    
    '****************************************************************************************************************************
    '****** Give error if group number is greater than total groups.
    '****************************************************************************************************************************
        
    counter = GROUP_START_ROW
    FaultCounter = 0
    While (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).text <> Empty) And (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).Font.Color = RGB(0, 0, 0)
        If (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).value >= Total_IoT_Groups) Then
            FaultCounter = FaultCounter + 1
            ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
        End If
        counter = counter + 1
    Wend
    
    If (FaultCounter <> 0) Then
        Sheets("IOT Groups").Select
        MsgBox "Error: One or more group number in IOT Groups sheet is greater than total groups"
        Exit Sub
    End If
    
    '****************************************************************************************************************************
    '****** Give error if both SeeMe and StoreMe are kept blank for one or more groups
    '****************************************************************************************************************************
    
    
    counter = GROUP_START_ROW
    While (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).text <> Empty) And (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, SEE_ME_COL).text = "x") Or (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, STORE_ME_COL).text = "x") Or (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, STORE_ME_ON_INTERVAL_COL).text = "x") Then
            ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, SEE_ME_COL).Interior.ColorIndex = NO_COLOR
            ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, STORE_ME_COL).Interior.ColorIndex = NO_COLOR
        Else
            ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, SEE_ME_COL).Interior.ColorIndex = RED_COLOR_FOR_ERROR
            ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, STORE_ME_COL).Interior.ColorIndex = RED_COLOR_FOR_ERROR
            FaultCounter = FaultCounter + 1
        End If
        counter = counter + 1
    Wend
    
    If (FaultCounter <> 0) Then
        Sheets("IOT Groups").Select
        MsgBox "Error: One or more groups have kept both SeeMe and StoreMe column in IOT Groups sheet blank. Atleast one should be selected"
        Exit Sub
    End If
    
    '****************************************************************************************************************************
    '****** Check if any tag_id is greater than 4294967295(uint32_t)
    '****************************************************************************************************************************
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, TAG_ID_COL).text > 4294967295#) Then   '"#" is appearing automatically
        MsgBox ("Error: TAG ID cannot be greater than 4294967295")
        Exit Sub
        End If
        counter = counter + 1
    Wend
    
    '****************************************************************************************************************************
    '****** Generate IOT channel list
    '****************************************************************************************************************************
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        IOT_DCI_C.WriteLine ("static const uint32_t " & Cells(counter, DATA_DEFINE_COL).text & "_IOT_CHANNEL_ID = " & Cells(counter, TAG_ID_COL).text & "U;")
        counter = counter + 1
    Wend
    
    IOT_DCI_C.WriteLine ("")
    IOT_DCI_C.WriteLine ("")
    
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        IOT_DCI_C.WriteLine ("static const char_t " & Cells(counter, DATA_DEFINE_COL).text & "_IOT_CHANNEL_ID_STR[] = " & Chr(34) & Cells(counter, TAG_ID_COL).text & Chr(34) & ";")
        counter = counter + 1
    Wend
    
    '****************************************************************************************************************************
    '****** Generate IOT channel array offset list
    '****************************************************************************************************************************
    IOT_DCI_C.WriteLine ("")
    IOT_DCI_C.WriteLine ("//*******************************************************")
    IOT_DCI_C.WriteLine ("//******     IOT Channel Array Offset List ")
    IOT_DCI_C.WriteLine ("//*******************************************************")
    IOT_DCI_C.WriteLine ("")
    
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, ARRAY_OFFSET_COL).text = Empty) Then
            IOT_DCI_C.WriteLine ("static const uint16_t " & Cells(counter, DATA_DEFINE_COL).text & "_IOT_ARRAY_OFFSET = 0U;")
        Else
            IOT_DCI_C.WriteLine ("static const uint16_t " & Cells(counter, DATA_DEFINE_COL).text & "_IOT_ARRAY_OFFSET = " & Cells(counter, ARRAY_OFFSET_COL).text & "U;")
        End If
        counter = counter + 1
    Wend
    
    '****************************************************************************************************************************
    '****** Generate the IOT to DCID cross reference table
    '****************************************************************************************************************************
    IOT_DCI_C.WriteLine ("")
    IOT_DCI_C.WriteLine ("//*******************************************************")
    IOT_DCI_C.WriteLine ("//******     The IOT to DCID Cross Reference table.")
    IOT_DCI_C.WriteLine ("//*******************************************************")
    IOT_DCI_C.WriteLine ("")
    IOT_DCI_C.WriteLine ("static const dci_iot_to_dcid_lkup_st_t iot_dci_data_iot_to_dcid[" & upper_case_file_name & "_TOTAL_CHANNELS] =")
    IOT_DCI_C.WriteLine ("{")
    
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        IOT_DCI_C.WriteLine ("    {")
        IOT_DCI_C.WriteLine ("        // " & Cells(counter, DESCRIP_COL).text)
        IOT_DCI_C.WriteLine ("        " & Cells(counter, DATA_DEFINE_COL).text & "_IOT_CHANNEL_ID" & ",")
        IOT_DCI_C.WriteLine ("        " & Cells(counter, DATA_DEFINE_COL).text & "_IOT_CHANNEL_ID_STR" & ",")
        IOT_DCI_C.WriteLine ("        " & Cells(counter, DATA_DEFINE_COL).text & "_DCID" & ",")
        col_counter = GROUP_NO_START_COL
        cnt_x = 0
        While (col_counter < GROUP_NO_END_COL)
            If (Cells(counter, col_counter).text = "x") Then
                cnt_x = cnt_x + 1
                If (cnt_x = 1) Then
                    IOT_DCI_C.Write ("        IOT_DCI::CADENCE_GROUP" & (col_counter - GROUP_NO_START_COL))
                Else
                    IOT_DCI_C.Write (" + IOT_DCI::CADENCE_GROUP" & (col_counter - GROUP_NO_START_COL))
                End If
            End If
            col_counter = col_counter + 1
        Wend
        If (cnt_x = 0) Then
            IOT_DCI_C.Write ("        IOT_DCI::CADENCE_GROUP_NONE")
        End If
        IOT_DCI_C.WriteLine (",")
        IOT_DCI_C.WriteLine ("        " & Cells(counter, DATA_DEFINE_COL).text & "_IOT_ARRAY_OFFSET")
        IOT_DCI_C.WriteLine ("    },")
        counter = counter + 1
    Wend
    IOT_DCI_C.WriteLine ("};")
    IOT_DCI_C.WriteLine ("")
    
    '****************************************************************************************************************************
    '****** Generate IOT cadence groups table.
    '****************************************************************************************************************************
    IOT_DCI_C.WriteLine ("")
    IOT_DCI_C.WriteLine ("//*******************************************************")
    IOT_DCI_C.WriteLine ("//******     The IOT Cadence Groups table.")
    IOT_DCI_C.WriteLine ("//*******************************************************")
    IOT_DCI_C.WriteLine ("")
    
    IOT_DCI_C.WriteLine ("static const iot_cadence_group_struct_t iot_cadence_groups[" & upper_case_file_name & "_TOTAL_GROUPS] =")
    IOT_DCI_C.WriteLine ("{")
    counter = GROUP_START_ROW
    While (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        IOT_DCI_C.WriteLine ("    {")
        IOT_DCI_C.WriteLine ("        // " & ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DESCRIPTION_COL).text)
        IOT_DCI_C.WriteLine ("        " & ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, CADENCE_DCI_COL).text & "_DCID,")
        IOT_DCI_C.Write ("        IOT_DCI::PUB_TYPE")
        
        If (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, SEE_ME_COL).text = "x") Then
            IOT_DCI_C.Write ("_SEEME")
        End If
        If (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, STORE_ME_COL).text = "x") Then
            IOT_DCI_C.Write ("_STOREME")
        End If
        If (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, STORE_ME_ON_INTERVAL_COL).text = "x") Then
            IOT_DCI_C.Write ("_STOREMEONINTERVAL")
        End If
    
        IOT_DCI_C.WriteLine (",")
        IOT_DCI_C.WriteLine ("        IOT_DCI::CADENCE_GROUP" & ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, GROUP_DATA_DEFINE_COL).text) & ","
        If (ActiveWorkbook.Worksheets("IOT Groups").Cells(counter, PUB_ALL_ON_CONNECT_COL).text = "x") Then
            IOT_DCI_C.WriteLine ("        IOT_DCI::PUBLISH_ALL_ON_CONNECT")
        Else
            IOT_DCI_C.WriteLine ("        IOT_DCI::DONT_PUBLISH_ALL_ON_CONNECT")
        End If
        IOT_DCI_C.WriteLine ("    },")
        counter = counter + 1
    Wend
    IOT_DCI_C.WriteLine ("};")
    IOT_DCI_C.WriteLine ("")
    
    '****************************************************************************************************************************
    '****** Generate the target definition structure.
    '****************************************************************************************************************************
    IOT_DCI_C.WriteLine ("")
    IOT_DCI_C.WriteLine ("//*******************************************************")
    IOT_DCI_C.WriteLine ("//******     The Target Definition Structure.")
    IOT_DCI_C.WriteLine ("//*******************************************************")
    IOT_DCI_C.WriteLine ("")
    
    IOT_DCI_C.WriteLine ("const iot_target_info_st_t " & lower_case_file_name & "_target_info = ")
    IOT_DCI_C.WriteLine ("{")
    IOT_DCI_C.WriteLine ("    " & upper_case_file_name & "_TOTAL_CHANNELS,")
    IOT_DCI_C.WriteLine ("    iot_dci_data_iot_to_dcid,")
    IOT_DCI_C.WriteLine ("    " & upper_case_file_name & "_TOTAL_GROUPS,")
    IOT_DCI_C.WriteLine ("    iot_cadence_groups")
    IOT_DCI_C.WriteLine ("};")
    IOT_DCI_C.WriteLine ("")

End Sub
