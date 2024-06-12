Attribute VB_Name = "Create_BACnet_Definition"
'**************************
'**************************
'BACnet object instance to DCI Construction sheet.
'**************************
'**************************

'***************************************************************
'Sheet File Indexing.


Sub BACnet_Definition()
'    Call Verify_Param_List

'*********************
'******     Constants
'*********************
    START_ROW = 8
    
    DESCRIP_COL = 1
    DATA_DEFINE_COL = 2
    OBJ_TYPE_COL = 3
    OBJ_INST_COL = 4
    BIT_NUM_COL = 5
    OBJ_NAME_COL = 6
    OBJ_DESC_COL = 7
    OBJ_CODE = 8
    UNITS_COL = 9
    INACTIVE_TEXT_COL = 10
    ACTIVE_TEXT_COL = 11
    Access_Type_Col = 12
    SCALE_COL = 13
    COV_INCREMENT_COL = 14
    MSV0_STATE_TEXT = 18
    MSV1_STATE_TEXT = 19
    
    Application.Calculation = xlCalculationManual
    
    Dim file_name As String
    Dim lower_case_file_name As String
    Dim upper_case_file_name As String
    Dim sheet_name As String
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    Dim MyDate
    MyDate = Date    ' MyDate contains the current system date.
    Dim range_string As String

    Dim error As Boolean
    error = False
    
    file_name = Cells(2, 2).value
    lower_case_file_name = LCase(file_name)
    upper_case_file_name = UCase(file_name)

'filing the object abbrivations
Dim arr(59)
arr(0) = "AI"
arr(1) = "AO"
arr(2) = "AV"
arr(3) = "BI"
arr(4) = "BO"
arr(5) = "BV"
arr(6) = "CAL"
arr(7) = "COM"
arr(8) = "DEV"
arr(9) = "EE"
arr(10) = "FILE"
arr(11) = "GRP"
arr(12) = "LOOP"
arr(13) = "MSI"
arr(14) = "MSO"
arr(15) = "NC"
arr(16) = "PRO"
arr(17) = "SCH"
arr(18) = "AVG"
arr(19) = "MSV"
arr(20) = "TL"
arr(21) = "LSP"
arr(22) = "LSZ"
arr(23) = "ACC"
arr(24) = "PC"
arr(25) = "EL"
arr(26) = "GG"
arr(27) = "TLM"
arr(28) = "LC"
arr(29) = "SV"
arr(30) = "AD"
arr(31) = "TMR"
arr(32) = "AC"
arr(33) = "AP"
arr(34) = "AR"
arr(35) = "AU"
arr(36) = "AZ"
arr(37) = "CDI"
arr(38) = "NS"
arr(39) = "BSV"
arr(40) = "CSV"
arr(41) = "DPV"
arr(42) = "DV"
arr(43) = "DTPV"
arr(44) = "DTV"
arr(45) = "IV"
arr(46) = "LAV"
arr(47) = "OSV"
arr(48) = "PIV"
arr(49) = "TPV"
arr(50) = "TV"
arr(51) = "NF"
arr(52) = "AE"
arr(53) = "CHA"
arr(54) = "LO"
arr(55) = "BLO"
arr(56) = "NP"
arr(57) = "EG"
arr(58) = "ESC"
arr(59) = "LIFT"



'****************************************************************************************************************************
'******     Sort the Bacnet list
'*************************************************************************************************************
        
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        counter = counter + 1
    Wend
    
    'Cells(counter + 1, DATA_DEFINE_COL) = "hello"
    
    Dim strDataRange As Range
    Set strDataRange = Range(Cells(START_ROW, DESCRIP_COL), Cells(counter, COV_INCREMENT_COL))
    
    
    strDataRange.Sort Key1:=Range("E8"), Order1:=xlAscending, _
    Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
    Orientation:=xlTopToBottom
    
    strDataRange.Sort Key1:=Range("D8"), Order1:=xlAscending, _
    Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
    Orientation:=xlTopToBottom
        
    strDataRange.Sort Key1:=Range("C8"), Order1:=xlAscending, _
    Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
    Orientation:=xlTopToBottom

'Give the error if any parameter name is added multiple times in the rest param column
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        Cells(counter, DATA_DEFINE_COL).Font.Color = RGB(0, 0, 0)
        counter = counter + 1
    Wend
                
    counter = START_ROW
    NextCounter = 0
    FaultCounter = 0
    SameCodeDefine = 0
    'index As Integer
        
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
    NextCounter = 1
        While (Cells(counter + NextCounter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter + NextCounter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            If (Cells(counter + NextCounter, OBJ_INST_COL).text <> Empty) Then
                If (Cells(counter, OBJ_TYPE_COL).text = Cells(counter + NextCounter, OBJ_TYPE_COL).text) And (Cells(counter, OBJ_INST_COL).text = Cells(counter + NextCounter, OBJ_INST_COL).text) Then
                    'FaultCounter = FaultCounter + 1
                    Cells(counter + NextCounter, DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
                    MsgBox "Error: Parameters """ & Cells(counter + NextCounter, DATA_DEFINE_COL).text & """ and """ & Cells(counter, DATA_DEFINE_COL).text & """ have same object instance"
                    error = True
                    GoTo Error_detected
                End If
            End If
            index = Cells(counter + NextCounter, OBJ_TYPE_COL).value
            If (index > 59) Then
                    Cells(counter + NextCounter, DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
                    MsgBox "Error: Object type """ & Cells(counter + NextCounter, OBJ_TYPE_COL).text & """ is not a supported bacnet object type"
                    error = True
                    GoTo Error_detected
            End If
            If (Cells(counter + NextCounter, BIT_NUM_COL).text <> Empty) Then
                If (Cells(counter, DATA_DEFINE_COL).text = Cells(counter + NextCounter, DATA_DEFINE_COL).text) And (Cells(counter, BIT_NUM_COL).text = Cells(counter + NextCounter, BIT_NUM_COL).text) Then
                    'FaultCounter = FaultCounter + 1
                    Cells(counter + NextCounter, DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
                    MsgBox "Error: """ & Cells(counter + NextCounter, DATA_DEFINE_COL).text & """ parameters bit number """ & Cells(counter + NextCounter, BIT_NUM_COL).text & """ has already been assigned"
                    error = True
                    GoTo Error_detected
                End If
            Else
                If (Cells(counter, DATA_DEFINE_COL).text = Cells(counter + NextCounter, DATA_DEFINE_COL).text) And (Cells(counter, OBJ_TYPE_COL).text = Cells(counter + NextCounter, OBJ_TYPE_COL).text) Then
                    Cells(counter + NextCounter, DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
                    MsgBox "Error: Parameter """ & Cells(counter + NextCounter, DATA_DEFINE_COL).text & """ code define is already used"
                    error = True
                    GoTo Error_detected
                End If
            End If
            NextCounter = NextCounter + 1
        Wend
        counter = counter + 1
    Wend
    
    'If (FaultCounter <> 0) Then
        'MsgBox "Error: Parameters cannot have same object instance"
    'End If
    
' search in DCI sheet if all required USER DCID parameters are defined
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        dci_row = Find_DCID_Row(Cells(counter, DATA_DEFINE_COL).text)
        If (dci_row = 0) Then
            MsgBox ("DCI Parameter """ & Cells(counter, DATA_DEFINE_COL).text & """ not found ")
            'i = NUM_OF_USERS * TOTAL_PARAMS_PER_USER
            error = True
            GoTo Error_detected
        End If
        counter = counter + 1
    Wend
' colour of rows
    counter = START_ROW
    Dim preObject As Integer
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, OBJ_TYPE_COL).text <> Empty) Then
        preObject = Cells(counter, OBJ_TYPE_COL).text
        End If
        While (Cells(counter, OBJ_TYPE_COL).text = preObject) And (Cells(counter, OBJ_TYPE_COL).text <> Empty)
            Range(Cells(counter, DESCRIP_COL), Cells(counter, COV_INCREMENT_COL)).Interior.Color = RGB(173, 226, 253)
            counter = counter + 1
        Wend
        If (Cells(counter, OBJ_TYPE_COL).text <> Empty) Then
        preObject = Cells(counter, OBJ_TYPE_COL).text
        End If
        While (Cells(counter, OBJ_TYPE_COL).text = preObject) And (Cells(counter, OBJ_TYPE_COL).text <> Empty)
                Range(Cells(counter, DESCRIP_COL), Cells(counter, COV_INCREMENT_COL)).Interior.Color = RGB(219, 233, 193)
                counter = counter + 1
        Wend
    Wend
    
    
'populating the object ids
counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        Cells(counter, OBJ_CODE) = arr(Cells(counter, OBJ_TYPE_COL).value)
        Cells(counter, OBJ_CODE).Interior.Color = RGB(217, 217, 217)
        'Cells(counter, OBJ_CODE).Locked = True
        counter = counter + 1
    Wend
    
    
counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        dci_row = Find_DCID_Row(Cells(counter, DATA_DEFINE_COL).text)
        Cells(counter, DESCRIP_COL) = Worksheets("DCI Descriptors").Cells(dci_row, DCI_DESCRIPTORS_COL).text
'        If (dci_row = 0) Then
'            MsgBox ("DCI Parameter """ & Cells(counter, DATA_DEFINE_COL).text & """ not found ")
'            'i = NUM_OF_USERS * TOTAL_PARAMS_PER_USER
'            error = True
'            GoTo Error_detected
'        End If
        counter = counter + 1
    Wend
    
Error_detected:
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
    
    'Making font type and size consistent.
    Worksheets(sheet_name).Range("A:S").Font.name = "Arial"
    Worksheets(sheet_name).Range("A:S").Font.Size = 10

If (error = False) Then
'****************************************************************************************************************************
'******     Start Creating the .H file header
'****************************************************************************************************************************
    FILE_DCI_C.WriteLine ("/**")
    FILE_DCI_C.WriteLine ("*****************************************************************************************")
    FILE_DCI_C.WriteLine ("*   @file")
    FILE_DCI_C.WriteLine ("*")
    FILE_DCI_C.WriteLine ("*   @brief Provides the BACnet DCI map.")
    FILE_DCI_C.WriteLine ("*")
    FILE_DCI_C.WriteLine ("*   @details This cross list is used to translate BAcnet instances to Parameter ID")
    FILE_DCI_C.WriteLine ("*")
    FILE_DCI_C.WriteLine ("*   @version")
    FILE_DCI_C.WriteLine ("*   C++ Style Guide Version 1.0")
    FILE_DCI_C.WriteLine ("*   Last Modified Date: " & MyDate)
    FILE_DCI_C.WriteLine ("*   @copyright 2013 Eaton Corporation. All Rights Reserved.")
    FILE_DCI_C.WriteLine ("*")
    FILE_DCI_C.WriteLine ("*****************************************************************************************")
    FILE_DCI_C.WriteLine ("*/")
    FILE_DCI_C.WriteLine ("")
    FILE_DCI_H.WriteLine ("#ifndef " & upper_case_file_name & "_H")
    FILE_DCI_H.WriteLine ("    #define " & upper_case_file_name & "_H")
    FILE_DCI_H.WriteLine ("")
    FILE_DCI_H.WriteLine ("#include ""Includes.h""")
    FILE_DCI_H.WriteLine ("#include ""core.h""")
    FILE_DCI_H.WriteLine ("#include ""DCI_Data.h""")
    FILE_DCI_H.WriteLine ("")
    FILE_DCI_C.WriteLine ("#include ""Base_DCI_BACnet_Data.h""")
    

        '----------------Let's count total Number of Object properties------
    
    row_counter = START_ROW
    no_of_classes = 0
    While (Cells(row_counter, DATA_DEFINE_COL).value <> BEGIN_IGNORED_DATA)
        total_obj = 0
        object_type = Cells(row_counter + 1, OBJ_TYPE_COL).value
        pre_object_type = Cells(row_counter, OBJ_TYPE_COL).value
        If (pre_object_type <> object_type) Then
            no_of_classes = no_of_classes + 1
        End If
        row_counter = row_counter + 1
    Wend
    FILE_DCI_H.WriteLine ("#define BACNET_TOTAL_CLASSES    " & no_of_classes & "U")
    
    row_counter = START_ROW
    total_obj = 0
    object_type = 0
    msv_count = 0
    Dim obj_abbr As String
    While (Cells(row_counter, DATA_DEFINE_COL).value <> BEGIN_IGNORED_DATA)
        total_obj = 0
        object_type = Cells(row_counter, OBJ_TYPE_COL).value
        While (Cells(row_counter, OBJ_TYPE_COL).value = object_type)
                total_obj = total_obj + 1
                obj_abbr = Cells(row_counter, OBJ_CODE).value
            row_counter = row_counter + 1
            If (object_type = 19) Then
            msv_count = msv_count + 1
            End If
        Wend
        FILE_DCI_H.WriteLine ("#define TOTAL_" & arr(object_type) & " " & total_obj & "U")
    Wend

    
    For i = 0 To (msv_count - 1)
    total_msv_states = 0
    row_counter = START_ROW
    While (Cells(row_counter, MSV0_STATE_TEXT + i).value <> BEGIN_IGNORED_DATA)
            total_msv_states = total_msv_states + 1
        row_counter = row_counter + 1
    Wend
    FILE_DCI_H.WriteLine ("#define MSV" & i & "_STATES  " & total_msv_states & "U")
    Next
    
       
    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("uint16_t MSV_Object_State_Count[TOTAL_MSV]=")
    FILE_DCI_H.WriteLine ("")
    FILE_DCI_H.WriteLine ("extern uint16_t MSV_Object_State_Count[TOTAL_MSV];")
    FILE_DCI_C.WriteLine ("{")
    For i = 0 To (msv_count - 1)
    total_msv_states = 0
    row_counter = START_ROW
    comma = ""
    If (i + 1 <= (msv_count - 1)) Then
    comma = ","
    End If
    While (Cells(row_counter, MSV0_STATE_TEXT + i).value <> BEGIN_IGNORED_DATA)
            total_msv_states = total_msv_states + 1
        row_counter = row_counter + 1
    Wend
    FILE_DCI_C.WriteLine ("    MSV" & i & "_STATES" & comma)
    Next
    FILE_DCI_C.WriteLine ("};")
    
    FILE_DCI_C.WriteLine ("")
    
    FILE_DCI_H.WriteLine ("")
    FILE_DCI_H.WriteLine ("typedef uint8_t BACNET_ID;")
    FILE_DCI_H.WriteLine ("")
    FILE_DCI_H.WriteLine ("")
    
    FILE_DCI_H.WriteLine ("struct MSV_length_to_strings")
    FILE_DCI_H.WriteLine ("{")
    FILE_DCI_H.WriteLine ("    frString msv;")
    FILE_DCI_H.WriteLine ("};")
    FILE_DCI_H.WriteLine ("")
    
        row_counter = START_ROW
    Bit_avail = 0
    While (Cells(row_counter, DATA_DEFINE_COL).value <> BEGIN_IGNORED_DATA)
        total_obj = 0
        object_type = Cells(row_counter + 1, OBJ_TYPE_COL).value
        pre_object_type = Cells(row_counter, OBJ_TYPE_COL).value
        If (pre_object_type <> object_type) Then
            obj_abbr = Cells(row_counter, OBJ_CODE).value
            FILE_DCI_H.WriteLine ("struct BACnet_" & arr(pre_object_type) & "_to_DCID")
            FILE_DCI_H.WriteLine ("{")
            FILE_DCI_H.WriteLine ("    BACNET_ID bacnet_id;")
            FILE_DCI_H.WriteLine ("    DCI_ID_TD dcid;")
            If (Cells(row_counter, BIT_NUM_COL).text <> Empty) Then
                FILE_DCI_H.WriteLine ("    uint8_t bitNumber;")
            End If
            If (Cells(row_counter, Access_Type_Col).text <> Empty) Then
                FILE_DCI_H.WriteLine ("    uint8_t accessType;")
            End If
            If (Cells(row_counter, UNITS_COL).text <> Empty) Then
                FILE_DCI_H.WriteLine ("    uint16_t units;")
            End If
            If (Cells(row_counter, SCALE_COL).text <> Empty) Then
                FILE_DCI_H.WriteLine ("    uint16_t scale;")
            End If
            If (Cells(row_counter, COV_INCREMENT_COL).text <> Empty) Then
                FILE_DCI_H.WriteLine ("    float32_t cov_increment;")
            End If
            If (Cells(row_counter, OBJ_NAME_COL).text <> Empty) Then
                FILE_DCI_H.WriteLine ("    frString name;")
            End If
            If (Cells(row_counter, OBJ_DESC_COL).text <> Empty) Then
                FILE_DCI_H.WriteLine ("    frString desc;")
            End If
            If (Cells(row_counter, INACTIVE_TEXT_COL).text <> Empty) Then
                FILE_DCI_H.WriteLine ("    frString inactive_text;")
            End If
            If (Cells(row_counter, ACTIVE_TEXT_COL).text <> Empty) Then
                FILE_DCI_H.WriteLine ("    frString active_text;")
            End If
            If (Cells(row_counter, OBJ_TYPE_COL).text = 19) Then
                FILE_DCI_H.WriteLine ("    const MSV_length_to_strings *MSV_len_to_Obj;")
            End If
            FILE_DCI_H.WriteLine ("};")
            FILE_DCI_H.WriteLine ("")
        End If
        row_counter = row_counter + 1
    Wend
   
  'Copy Description of MSV objects
   For i = 0 To (msv_count - 1)
    total_msv_states = 0
    start_row_MSV = START_ROW
    FILE_DCI_C.WriteLine ("const struct MSV_length_to_strings MSV" & i & "_len_to_State_Txt[MSV" & i & "_STATES] = ")
    FILE_DCI_C.WriteLine ("{")
    While (Cells(start_row_MSV, MSV0_STATE_TEXT + i).text <> BEGIN_IGNORED_DATA)
        FILE_DCI_C.WriteLine ("    " & Len(Cells(start_row_MSV, MSV0_STATE_TEXT + i)) & "U," & "64U" & "," & "0U" & "," & "const_cast<char*>(" & """" & Cells(start_row_MSV, MSV0_STATE_TEXT + i).text & """" & ")" & ",")
        If (Cells(start_row_MSV, MSV0_STATE_TEXT + i).text = BEGIN_IGNORED_DATA) Then
            FILE_DCI_C.WriteLine ("    } ")
        End If
        start_row_MSV = start_row_MSV + 1
    Wend
    FILE_DCI_C.WriteLine ("};  ")
    FILE_DCI_C.WriteLine ("     ")
    Next
    
    '---------------- Create Map for Objs
    row_counter = START_ROW
    While (Cells(row_counter, DATA_DEFINE_COL).value <> BEGIN_IGNORED_DATA)
        total_obj = 0
        object_type = Cells(row_counter, OBJ_TYPE_COL).value
        obj_abbr = Cells(row_counter, OBJ_CODE).value
        FILE_DCI_C.WriteLine ("const BACnet_" & arr(object_type) & "_to_DCID BACnet_" & arr(object_type) & "_to_DCID_map[TOTAL_" & arr(object_type) & "] =   ")
        FILE_DCI_C.WriteLine ("{   ")
        dev_prop_num = 0
        While (Cells(row_counter, OBJ_TYPE_COL).value = object_type)
            If (Cells(row_counter, OBJ_INST_COL).text <> Empty) Then
                FILE_DCI_C.WriteLine ("    {    " & Cells(row_counter, OBJ_INST_COL).text & "U                                   /*Instance*/")
            Else
                FILE_DCI_C.WriteLine ("    {    " & dev_prop_num & "U ")
            End If
            If (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) Then
                FILE_DCI_C.WriteLine ("         ," & Cells(row_counter, DATA_DEFINE_COL).text & "_DCID" & "              /* " & Cells(row_counter, DESCRIP_COL).text) & "*/"
            End If
            If (Cells(row_counter, BIT_NUM_COL).text <> Empty) Then
                FILE_DCI_C.WriteLine ("         ," & Cells(row_counter, BIT_NUM_COL).text & "U                                         /*Bit Number*/")
            End If
            If (Cells(row_counter, Access_Type_Col).text <> Empty) Then
                FILE_DCI_C.WriteLine ("         ," & Cells(row_counter, Access_Type_Col).text) & "U                                    /*Access Type*/"
            End If
            If (Cells(row_counter, UNITS_COL).text <> Empty) Then
                FILE_DCI_C.WriteLine ("         ," & Cells(row_counter, UNITS_COL).text) & "U                                   /*Unit*/"
            End If
            If (Cells(row_counter, SCALE_COL).text <> Empty) Then
                FILE_DCI_C.WriteLine ("         ," & Cells(row_counter, SCALE_COL).text) & "U                                  /*Scale*/"
            End If
            If (Cells(row_counter, COV_INCREMENT_COL).text <> Empty) Then
                FILE_DCI_C.WriteLine ("         ," & Cells(row_counter, COV_INCREMENT_COL).text) & "U                                  /*COV Increament*/"
            End If
            If (Cells(row_counter, OBJ_NAME_COL).text <> Empty) Then
                FILE_DCI_C.WriteLine ("         ," & Len(Cells(row_counter, OBJ_NAME_COL)) & "U," & "64U" & "," & "0U" & "," & "const_cast<char*>(" & """" & Cells(row_counter, OBJ_NAME_COL).text & """") & ")" & "              /*Name*/"
            End If
            If (Cells(row_counter, OBJ_DESC_COL).text <> Empty) Then
                FILE_DCI_C.WriteLine ("         ," & Len(Cells(row_counter, OBJ_DESC_COL)) & "U," & "64U" & "," & "0U" & "," & "const_cast<char*>(" & """" & Cells(row_counter, OBJ_DESC_COL).text & """") & ")" & "               /*Description*/"
            End If
            If (Cells(row_counter, INACTIVE_TEXT_COL).text <> Empty) Then
                FILE_DCI_C.WriteLine ("         ," & Len(Cells(row_counter, INACTIVE_TEXT_COL)) & "," & "64U" & "," & "0U" & "," & "const_cast<char*>(" & """" & Cells(row_counter, INACTIVE_TEXT_COL).text & """") & ")" & "                /*Inactive Text*/"
            End If
            If (Cells(row_counter, ACTIVE_TEXT_COL).text <> Empty) Then
                FILE_DCI_C.WriteLine ("         ," & Len(Cells(row_counter, ACTIVE_TEXT_COL)) & "," & "64U" & "," & "0U" & "," & "const_cast<char*>(" & """" & Cells(row_counter, ACTIVE_TEXT_COL).text & """") & ")" & "                       /*Active Text*/"
            End If
            If (Cells(row_counter, OBJ_TYPE_COL).text = 19) Then
                FILE_DCI_C.WriteLine ("         ," & "MSV" & Cells(row_counter, OBJ_INST_COL).text & "_len_to_State_Txt")
            End If
            If (Cells(row_counter, DATA_DEFINE_COL).text = Empty) Or (Cells(row_counter, DATA_DEFINE_COL).text = BEGIN_IGNORED_DATA) Or (Cells(row_counter + 1, OBJ_TYPE_COL).value <> object_type) Then
                FILE_DCI_C.WriteLine ("    } ")
            Else
                FILE_DCI_C.WriteLine ("    }, ")
            End If
            row_counter = row_counter + 1
            dev_prop_num = dev_prop_num + 1
        Wend
        FILE_DCI_C.WriteLine ("};  ")
        FILE_DCI_C.WriteLine ("     ")
    Wend


FILE_DCI_C.WriteLine ("const BACNET_CLASS_ST_TD bacnet_dci_data_cip_class_list[BACNET_TOTAL_CLASSES] =")
FILE_DCI_C.WriteLine ("{")
    row_counter = START_ROW
    Bit_avail = 0
    While (Cells(row_counter, DATA_DEFINE_COL).value <> BEGIN_IGNORED_DATA)
        total_obj = 0
        object_type = Cells(row_counter + 1, OBJ_TYPE_COL).value
        pre_object_type = Cells(row_counter, OBJ_TYPE_COL).value
        If (pre_object_type <> object_type) Then
            obj_abbr = Cells(row_counter, OBJ_CODE).value
            FILE_DCI_C.WriteLine ("    BACnet_" & arr(pre_object_type) & "_to_DCID_map,")
        End If
        row_counter = row_counter + 1
    Wend
FILE_DCI_C.WriteLine ("};")

FILE_DCI_C.WriteLine ("//*******************************************************")
FILE_DCI_C.WriteLine ("//******      Define the Class Structure List")
FILE_DCI_C.WriteLine ("//*******************************************************")

FILE_DCI_C.WriteLine ("const BACNET_TARGET_INFO_ST_TD bacnet_dci_data_target_info =")
FILE_DCI_C.WriteLine ("{")
FILE_DCI_C.WriteLine ("    BACNET_TOTAL_CLASSES,")
FILE_DCI_C.WriteLine ("    bacnet_dci_data_cip_class_list,")
FILE_DCI_C.WriteLine ("};")
FILE_DCI_C.WriteLine ("     ")

    row_counter = START_ROW
    Bit_avail = 0
    While (Cells(row_counter, DATA_DEFINE_COL).value <> BEGIN_IGNORED_DATA)
        total_obj = 0
        object_type = Cells(row_counter + 1, OBJ_TYPE_COL).value
        pre_object_type = Cells(row_counter, OBJ_TYPE_COL).value
        If (pre_object_type <> object_type) Then
            obj_abbr = Cells(row_counter, OBJ_CODE).value
            FILE_DCI_H.WriteLine ("extern const BACnet_" & arr(pre_object_type) & "_to_DCID BACnet_" & arr(pre_object_type) & "_to_DCID_map[TOTAL_" & arr(pre_object_type) & "];")
        End If
        row_counter = row_counter + 1
    Wend

    For i = 0 To (msv_count - 1)
    total_msv_states = 0
    row_counter = START_ROW
    While (Cells(row_counter, MSV0_STATE_TEXT + i).value <> BEGIN_IGNORED_DATA)
            total_msv_states = total_msv_states + 1
        row_counter = row_counter + 1
    Wend
    FILE_DCI_H.WriteLine ("extern const struct MSV_length_to_strings MSV" & i & "_len_to_State_Txt[MSV" & i & "_STATES];")
    Next
FILE_DCI_H.WriteLine ("     ")
FILE_DCI_H.WriteLine ("typedef struct BACNET_CLASS_ST_TD")
FILE_DCI_H.WriteLine ("{")

row_counter = START_ROW
    Bit_avail = 0
    While (Cells(row_counter, DATA_DEFINE_COL).value <> BEGIN_IGNORED_DATA)
        total_obj = 0
        object_type = Cells(row_counter + 1, OBJ_TYPE_COL).value
        pre_object_type = Cells(row_counter, OBJ_TYPE_COL).value
        If (pre_object_type <> object_type) Then
            obj_abbr = Cells(row_counter, OBJ_CODE).value
            FILE_DCI_H.WriteLine ("    BACnet_" & arr(pre_object_type) & "_to_DCID const *BACnet_" & arr(pre_object_type) & "_to_DCID_map;")
        End If
        row_counter = row_counter + 1
    Wend
FILE_DCI_H.WriteLine ("} BACNET_CLASS_ST_TD;")
FILE_DCI_H.WriteLine ("     ")
FILE_DCI_H.WriteLine ("typedef struct BACNET_TARGET_INFO_ST_TD")
FILE_DCI_H.WriteLine ("{")
FILE_DCI_H.WriteLine ("    uint16_t total_classes;")
FILE_DCI_H.WriteLine ("    BACNET_CLASS_ST_TD const* class_list;")
FILE_DCI_H.WriteLine ("} BACNET_TARGET_INFO_ST_TD;")
FILE_DCI_H.WriteLine ("     ")
FILE_DCI_H.WriteLine ("extern const BACNET_TARGET_INFO_ST_TD bacnet_dci_data_target_info;")

    FILE_DCI_H.WriteLine ("#endif")
    
    FILE_DCI_H.Close
    FILE_DCI_C.Close

    Application.Calculation = xlCalculationAutomatic
    
    End If
End_vba:
End Sub
