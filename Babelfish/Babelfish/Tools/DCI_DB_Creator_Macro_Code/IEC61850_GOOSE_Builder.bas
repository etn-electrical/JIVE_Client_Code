Attribute VB_Name = "IEC61850_GOOSE_Builder"
'***************************************************************
'IEC61850 GOOSE Sheet Indexing.
'***************************************************************
Public Const IEC61850_GOOSE_DESCRIP_COL = "A"
Public Const IEC61850_GOOSE_DATA_DEFINE_COL = "B"
Public Const IEC61850_GOOSE_TYPE_COL = "C"
Public Const IEC61850_GOOSE_GROUP_ID_COL = "D"
Public Const IEC61850_GOOSE_PARAM_ID_COL = "E"
Public Const IEC61850_GOOSE_CB_REF_NAME_COL = "F"
Public Const IEC61850_GOOSE_DESCRIP_LOGIC_DETAILS_COL = "G"
Public Const IEC61850_GOOSE_DATA_TYPE_COL = "H"


'Row number used for User_Roles
Public Const IEC61850_GOOSE_DEVICE_ACCESS_LEVELS_ROW = 2

'Row numbers used in XML File
Public Const IEC61850_GOOSE_HEADER_ROW = 9
Public Const IEC61850_GOOSE_NAME_ROW = 10
Public Const IEC61850_GOOSE_OUT_ROW = 11
Public Const IEC61850_GOOSE_PREFIX_ROW = 12

'These Start Row Numbers has to be followed same since there are three services implemented in BLE
'1.To maintain the difference we have this Row numbers assigned for the start of each service
'2.Generic Access Service should not be changed anytime
'   It carries the device name information while advertising
'3.Device Profile parameters should not be deleted
'  it carries the mandatory information to create the Device GATT profile
'4.Any paramater can be added in the device profile service after 54th Row
'5.Any parameter can be added or modified in the customized BLE service

Public Const IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE = 15
'Public Const IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE = 54
'Public Const BLE_DATA_START_ROW_CUSTOMIZED_BLE_LED_SERVICE = 133

'Columns cannnot be interchanged or adding new column in the middle is prohibited

Public Const IEC61850_GOOSE_DCI_DATA_DEFINE_COL = "B"
Public Const IEC61850_GOOSE_DCI_PARAMACTERISTIC_VALUE_TYPE_COL = "C"
Public Const IEC61850_GOOSE_DCI_TOTAL_LENGTH_COL = "F"
Public Const IEC61850_GOOSE_DCI_DEFAULT_COL = "G"
Public Const IEC61850_GOOSE_DCI_MIN_VALUE_COL = "H"
Public Const IEC61850_GOOSE_DCI_MAX_VALUE_COL = "I"
Public Const IEC61850_GOOSE_DCI_BIT_FIELD_COL = "K"
Public Const IEC61850_GOOSE_DCI_ENUM_DETAILS_COL = "Z"
Public Const IEC61850_GOOSE_DCI_ENUM_SUPPORT_COL = "AI"
Public Const IEC61850_GOOSE_DCI_RAM_VAL_WRITE_ACCESS_COL = "BA"
Public Const IEC61850_GOOSE_DCI_INIT_VAL_READ_ACCESS_COL = "BB"
Public Const IEC61850_GOOSE_DCI_INIT_VAL_WRITE_ACCESS_COL = "BC"
Public Const IEC61850_GOOSE_DCI_DEFAULT_READ_ACCESS_COL = "BD"
Public Const IEC61850_GOOSE_DCI_SHORT_NAME_COL = "CA"

' Constant for Maximum Payload
Public Const MAX_PAYLOAD = 255


Public Const IEC61850_GOOSE_BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"

'**************************
'**************************
'IEC61850 GOOSE Construction sheet.
'**************************
'**************************

Sub Activate_Sheet()
    Worksheets("IEC61850_GOOSE").Activate
End Sub
   
Sub Create_IEC61850_GOOSE_DCI_Xfer()


    Application.Calculation = xlCalculationManual
    
    Dim sheet_name As String
    Dim description_string As String
    Dim DCI_STRING As String
    Dim num_unique_data As Integer
    Dim file_name As String
    Dim lower_case_file_name As String
    
'*********************
'******     Begin
'*********************
    Sheets("IEC61850_GOOSE").Select

    file_name = Cells(3, 3).value
    lower_case_file_name = LCase(file_name)
    upper_case_file_name = UCase(file_name)
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\" & Cells(4, 3).value
    Set IEC61850_GOOSE_DCI_C = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(5, 3).value
    Set IEC61850_GOOSE_DCI_H = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(6, 3).value
    Set IEC61850_GOOSE_DCI_Json_Tree = fs.CreateTextFile(file_path, True)
    
    'Making font type and size consistent.
    Worksheets("IEC61850_GOOSE").Range("A:U").Font.name = "Arial"
    Worksheets("IEC61850_GOOSE").Range("A:U").Font.Size = 10
        
'****************************************************************************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'***    Start Creating the .H file header
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************************************************************************

    IEC61850_GOOSE_DCI_H.WriteLine ("/**")
    IEC61850_GOOSE_DCI_H.WriteLine ("*****************************************************************************************")
    IEC61850_GOOSE_DCI_H.WriteLine ("*   @file")
    IEC61850_GOOSE_DCI_H.WriteLine ("*")
    IEC61850_GOOSE_DCI_H.WriteLine ("*   @brief")
    IEC61850_GOOSE_DCI_H.WriteLine ("*")
    IEC61850_GOOSE_DCI_H.WriteLine ("*   @details")
    IEC61850_GOOSE_DCI_H.WriteLine ("*")
    IEC61850_GOOSE_DCI_H.WriteLine ("*   @version")
    IEC61850_GOOSE_DCI_H.WriteLine ("*   C++ Style Guide Version 1.0")
    IEC61850_GOOSE_DCI_H.WriteLine ("*")
    IEC61850_GOOSE_DCI_H.WriteLine ("*   @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    IEC61850_GOOSE_DCI_H.WriteLine ("*")
    IEC61850_GOOSE_DCI_H.WriteLine ("*****************************************************************************************")
    IEC61850_GOOSE_DCI_H.WriteLine ("*/")
    IEC61850_GOOSE_DCI_H.WriteLine ("#ifndef " & upper_case_file_name & "_H")
    IEC61850_GOOSE_DCI_H.WriteLine ("#define " & upper_case_file_name & "_H")
    IEC61850_GOOSE_DCI_H.WriteLine ("")
    IEC61850_GOOSE_DCI_H.WriteLine ("#include ""DCI.h""")
    IEC61850_GOOSE_DCI_H.WriteLine ("#include ""DCI_Data.h""")
    IEC61850_GOOSE_DCI_H.WriteLine ("#include ""IEC61850_GOOSE_DCI.h""")
    IEC61850_GOOSE_DCI_H.WriteLine ("")
    
'****************************************************************************************************************************
'******     Create the header and associated file structure.  Get ready to plop the data down.
'****************************************************************************************************************************
    IEC61850_GOOSE_DCI_H.WriteLine ("//*******************************************************")
    IEC61850_GOOSE_DCI_H.WriteLine ("//******     IEC61850 GOOSE structures and constants.")
    IEC61850_GOOSE_DCI_H.WriteLine ("//*******************************************************")
    IEC61850_GOOSE_DCI_H.WriteLine ("")
    IEC61850_GOOSE_DCI_H.WriteLine ("extern const IEC61850_GOOSE_DCI::goose_message_info_t iec61850_goose_msg_info;")
    IEC61850_GOOSE_DCI_H.WriteLine ("")
    
    START_ROW = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
        
    IEC61850_GOOSE_DCI_H.WriteLine ("")
    IEC61850_GOOSE_DCI_H.WriteLine ("#endif")
    
'****************************************************************************************************************************
'****** Generating the DCID's Automatically and filling in DCID coloumn.
'****************************************************************************************************************************
' This check shall cover Generic Service and the Device Profile Service
        
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    DCID_Value = 1
    While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "par") Then
            Cells(counter, IEC61850_GOOSE_PARAM_ID_COL) = DCID_Value
            DCID_Value = DCID_Value + 1
        End If
        counter = counter + 1
    Wend

' This check shall cover Generic Service and the Device Profile Service
        
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    DCID_Value = 1
    While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Pub_Start") Then
            Cells(counter, IEC61850_GOOSE_GROUP_ID_COL) = DCID_Value
            DCID_Value = DCID_Value + 1
        End If
        counter = counter + 1
    Wend
    
    ' This check shall cover Generic Service and the Device Profile Service
        
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    DCID_Value = 1
    While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Sub_Start") Then
            Cells(counter, IEC61850_GOOSE_GROUP_ID_COL) = DCID_Value
            DCID_Value = DCID_Value + 1
        End If
        counter = counter + 1
    Wend
    
'****************************************************************************************************************************
'****** Checking for the Pub_Start and Pub_End count.
'****************************************************************************************************************************
    Dim Group_Check As Integer
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    
    While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Pub_Start") Then
            Group_Check = Group_Check + 1
        ElseIf (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Pub_End") Then
            Group_Check = Group_Check - 1
        End If
        counter = counter + 1
    Wend
    
    If (Group_Check <> "0") Then
        MsgBox "Error: Pub_Start and Pub_End are not in sync "
        Exit Sub
    End If
    
'****************************************************************************************************************************
'****** Checking for the Sub_Start and Sub_End count.
'****************************************************************************************************************************
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    
    While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Sub_Start") Then
            Group_Check = Group_Check + 1
        ElseIf (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Sub_End") Then
            Group_Check = Group_Check - 1
        End If
        counter = counter + 1
    Wend
    
    If (Group_Check <> "0") Then
        MsgBox "Error: Sub_Start and Sub_End are not in sync "
        Exit Sub
    End If
    
'****************************************************************************************************************************
'****** Checking for the start and End count.
'****************************************************************************************************************************
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    
    While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Start") Then
            Group_Check = Group_Check + 1
        ElseIf (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "End") Then
            Group_Check = Group_Check - 1
        End If
        counter = counter + 1
    Wend
    
    If (Group_Check <> "0") Then
        MsgBox "Error: Start and End are not in sync "
        Exit Sub
    End If
    
'****************************************************************************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'***    Start Creating the .C file header
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************************************************************************
    
    IEC61850_GOOSE_DCI_C.WriteLine ("/**")
    IEC61850_GOOSE_DCI_C.WriteLine ("*****************************************************************************************")
    IEC61850_GOOSE_DCI_C.WriteLine ("*   @file")
    IEC61850_GOOSE_DCI_C.WriteLine ("*   @details See header file for module overview.")
    IEC61850_GOOSE_DCI_C.WriteLine ("*   @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    IEC61850_GOOSE_DCI_C.WriteLine ("*")
    IEC61850_GOOSE_DCI_C.WriteLine ("*****************************************************************************************")
    IEC61850_GOOSE_DCI_C.WriteLine ("*/")
    IEC61850_GOOSE_DCI_C.WriteLine ("#include ""Includes.h""")
    IEC61850_GOOSE_DCI_C.WriteLine ("#include """ & file_name & ".h""")
    IEC61850_GOOSE_DCI_C.WriteLine ("")
    IEC61850_GOOSE_DCI_C.WriteLine ("")

''****************************************************************************************************************************
''****** Give error if any parameter name is added multiple times in the Code Define column in the IEC61850 Sheet.
''****************************************************************************************************************************
   ' This error check shall cover Generic Service and the Device Profile Service
    
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text = Empty) Then
            counter = counter + 1
        Else
            Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).Font.Color = RGB(0, 0, 0)
            counter = counter + 1
        End If
    Wend
    
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    NextCounter = 0
    FaultCounter = 0
    
    While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
       NextCounter = 1
       While (Cells(counter + NextCounter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> Empty)
           If (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text = Cells(counter + NextCounter, IEC61850_GOOSE_DATA_DEFINE_COL).text) Then
               FaultCounter = FaultCounter + 1
               Cells(counter + NextCounter, IEC61850_GOOSE_DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
           End If
           NextCounter = NextCounter + 1
       Wend
       counter = counter + 1
    Wend

    If (FaultCounter <> 0) Then
        MsgBox "Error: One or more IEC61850 GOOSE parameters are added multiple times"
        Exit Sub
    End If


'****************************************************************************************************************************
'****** Give the error if the same DCI ID is assigned to multiple IEC61850 parameters.
'****************************************************************************************************************************
' This error check shall cover Generic Service and the Device Profile Service

    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text = Empty) Then
            counter = counter + 1
        Else
            Cells(counter, IEC61850_GOOSE_PARAM_ID_COL).Font.Color = RGB(0, 0, 0)
            counter = counter + 1
        End If
    Wend
    
 ' This error check shall cover all IEC61850 parameters
 
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    NextCounter = 0
    FaultCounter = 0

    While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
    If (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text = Empty) Then
        counter = counter + 1
    Else
        NextCounter = 1
        While (Cells(counter + NextCounter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            If (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text = Cells(counter + NextCounter, IEC61850_GOOSE_DATA_DEFINE_COL).text) Then
                FaultCounter = FaultCounter + 1
                Cells(counter + NextCounter, IEC61850_GOOSE_DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
            End If
            NextCounter = NextCounter + 1
        Wend
        counter = counter + 1
    End If
    Wend

    If (FaultCounter <> 0) Then
        MsgBox "Error: One or more IEC61850 GOOSE parameters are assigned the same DCI IDs"
        Exit Sub
    End If


    
'****************************************************************************************************************************
'******     Gotta calculate the total number of Characteristic IDs.
'****************************************************************************************************************************
    num_registers = 0
    num_registers1 = 0
    num_registers2 = 0
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    counter1 = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    
    While (Cells(counter1, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter1, IEC61850_GOOSE_DATA_DEFINE_COL).text = Empty) Then
            counter1 = counter1 + 1
        Else
            counter1 = counter1 + 1
            num_registers2 = num_registers2 + 1
        End If
    Wend
    
    num_registers = num_registers1 + num_registers2

'**************************************************************************************************************************
'******     Generate the IEC61850 Group publish array details.
'****************************************************************************************************************************
    Dim Groupstart_Flag As Integer
    Dim Groupzero_Flag As Integer
    Dim Group_Counter As Integer
    Dim Max_Parameters As Integer
    Dim Group_Parameters() As Integer
    Dim Group_Index As Integer
    Dim Group_Zero_Index As Integer
    Dim SubGroupstart_Flag As Integer
    Dim SubGroup_Counter As Integer
    Dim Total_SubGroup_Counter As Integer
    
    IEC61850_GOOSE_DCI_C.WriteLine ("//*******************************************************")
    IEC61850_GOOSE_DCI_C.WriteLine ("//******     The IEC61850 GOOSE Publish Group and Sub group array details.")
    IEC61850_GOOSE_DCI_C.WriteLine ("//*******************************************************")

   Groupstart_Flag = 0
    Group_Index = 0
    SubGroupstart_Flag = 0
    Groupzero_Flag = 0
    Group_Counter = 0
    SubGroup_Counter = 0
    Total_SubGroup_Counter = 0
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    
   While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Pub_Start") Then
                Groupstart_Flag = 1
                Group_Counter = Group_Counter + 1
        ElseIf (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Pub_End") Then
                Groupstart_Flag = 0
                Total_SubGroup_Counter = 0
                Groupzero_Flag = 0
        End If
        If ((Groupstart_Flag = 1) And (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Start")) Then
                If (Groupzero_Flag = 0) Then
                Groupzero_Flag = 1
                End If
                Total_SubGroup_Counter = Total_SubGroup_Counter + 1
                IEC61850_GOOSE_DCI_C.WriteLine ("const iec61850_tagtype_t DCI_GOOSE_PUBLISH_SUB_GROUP_PARAMS_TAG_" & Group_Counter & "_" & Total_SubGroup_Counter & "[] = ")
                IEC61850_GOOSE_DCI_C.WriteLine ("{")
                SubGroupstart_Flag = 1
                SubGroup_Counter = 0
        ElseIf ((Groupstart_Flag = 1) And (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "End")) Then
                SubGroupstart_Flag = 0
                IEC61850_GOOSE_DCI_C.WriteLine ("};")
        End If
        
            If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "par") Then
                If ((Groupstart_Flag = 1) And (SubGroupstart_Flag = 1)) Then
                    IEC61850_GOOSE_DCI_C.WriteLine ("    " & Cells(counter, IEC61850_GOOSE_DATA_TYPE_COL).text & ",")
                    SubGroup_Counter = SubGroup_Counter + 1
                End If
        End If
        counter = counter + 1
    Wend
    
    Groupstart_Flag = 0
    Group_Index = 0
    SubGroupstart_Flag = 0
    Groupzero_Flag = 0
    Group_Counter = 0
    SubGroup_Counter = 0
    Total_SubGroup_Counter = 0
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    
   While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Pub_Start") Then
                Groupstart_Flag = 1
                Group_Counter = Group_Counter + 1
                IEC61850_GOOSE_DCI_C.WriteLine ("const IEC61850_GOOSE_DCI::group_param_t DCI_GOOSE_PUBLISH_MAIN_GROUP_PARAM_" & Group_Counter & "[] = ")
                IEC61850_GOOSE_DCI_C.WriteLine ("{")
        ElseIf (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Pub_End") Then
                Groupstart_Flag = 0
                Total_SubGroup_Counter = 0
                Groupzero_Flag = 0
        End If
        If ((Groupstart_Flag = 1) And (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Start")) Then
                If (Groupzero_Flag = 0) Then
                Groupzero_Flag = 1
                IEC61850_GOOSE_DCI_C.WriteLine ("};")
                End If
                Total_SubGroup_Counter = Total_SubGroup_Counter + 1
                IEC61850_GOOSE_DCI_C.WriteLine ("const IEC61850_GOOSE_DCI::sub_group_param_t DCI_GOOSE_PUBLISH_SUB_GROUP_PARAM_" & Group_Counter & "_" & Total_SubGroup_Counter & "[] = ")
                IEC61850_GOOSE_DCI_C.WriteLine ("{")
                SubGroupstart_Flag = 1
                SubGroup_Counter = 0
        ElseIf ((Groupstart_Flag = 1) And (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "End")) Then
                SubGroupstart_Flag = 0
                IEC61850_GOOSE_DCI_C.WriteLine ("};")
        End If
        
            If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "par") Then
                If ((Groupstart_Flag = 1) And (SubGroupstart_Flag = 0)) Then
                    IEC61850_GOOSE_DCI_C.WriteLine ("    " & Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text & "_DCID,")
                ElseIf (SubGroupstart_Flag = 1) Then
                    IEC61850_GOOSE_DCI_C.WriteLine ("    " & Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text & "_DCID,")
                    SubGroup_Counter = SubGroup_Counter + 1
                End If
        End If
        counter = counter + 1
    Wend
'**************************************************************************************************************************
'******     Generate the IEC61850 GOOSE publish subgroup straucture array details.
'****************************************************************************************************************************
 
    IEC61850_GOOSE_DCI_C.WriteLine ("//*******************************************************")
    IEC61850_GOOSE_DCI_C.WriteLine ("//******     The IEC61850 GOOSE Publish Sub group structure array details.")
    IEC61850_GOOSE_DCI_C.WriteLine ("//*******************************************************")

    Groupstart_Flag = 0
    Group_Index = 0
    SubGroupstart_Flag = 0
    Groupzero_Flag = 0
    Group_Counter = 0
    SubGroup_Counter = 0
    Total_SubGroup_Counter = 0
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    
   While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Pub_Start") Then
                Groupstart_Flag = 1
                Group_Counter = Group_Counter + 1
                IEC61850_GOOSE_DCI_C.WriteLine ("static const char_t DCI_GOOSE_PUBLISH_CB_REF_NAME_" & Group_Counter & "[] = {" & Cells(counter, IEC61850_GOOSE_CB_REF_NAME_COL).text & "};")
        ElseIf (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Pub_End") Then
                Groupstart_Flag = 0
                Total_SubGroup_Counter = 0
                Groupzero_Flag = 0
                IEC61850_GOOSE_DCI_C.WriteLine ("};")
        End If
        If ((Groupstart_Flag = 1) And (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Start")) Then
                If (Groupzero_Flag = 0) Then
                IEC61850_GOOSE_DCI_C.WriteLine ("const IEC61850_GOOSE_DCI::subgroup_assembly_t DCI_GOOSE_PUBLISH_SUB_GROUP_STRUCT_" & Group_Counter & "[] = ")
                IEC61850_GOOSE_DCI_C.WriteLine ("{")
                Groupzero_Flag = 1
                End If
                Total_SubGroup_Counter = Total_SubGroup_Counter + 1
                IEC61850_GOOSE_DCI_C.WriteLine ("  {")
                SubGroupstart_Flag = 1
                SubGroup_Counter = 0
        ElseIf ((Groupstart_Flag = 1) And (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "End")) Then
                SubGroupstart_Flag = 0
                IEC61850_GOOSE_DCI_C.WriteLine ("    DCI_GOOSE_PUBLISH_SUB_GROUP_PARAM_" & Group_Counter & "_" & Total_SubGroup_Counter & ",")
                IEC61850_GOOSE_DCI_C.WriteLine ("    DCI_GOOSE_PUBLISH_SUB_GROUP_PARAMS_TAG_" & Group_Counter & "_" & Total_SubGroup_Counter & ",")
                IEC61850_GOOSE_DCI_C.WriteLine ("    " & SubGroup_Counter & "U,")
                IEC61850_GOOSE_DCI_C.WriteLine ("  },")
        End If
        
            If ((Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "par") And (SubGroupstart_Flag = 1)) Then
                SubGroup_Counter = SubGroup_Counter + 1
            End If
        counter = counter + 1
    Wend

'**************************************************************************************************************************
'******     Store the IEC61850 GOOSE publish Groups in Array.
'****************************************************************************************************************************
    
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    Group_Counter = 0
    Max_Parameters = 0
    
    While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Pub_Start") Then
            Group_Counter = Group_Counter + 1
        End If
        counter = counter + 1
    Wend
    
    IEC61850_GOOSE_DCI_C.WriteLine ("")
    IEC61850_GOOSE_DCI_C.WriteLine ("//*******************************************************")
    IEC61850_GOOSE_DCI_C.WriteLine ("//******     The IEC61850 GOOSE Publish Group Cross Reference Table.")
    IEC61850_GOOSE_DCI_C.WriteLine ("//*******************************************************")
    IEC61850_GOOSE_DCI_C.WriteLine ("")
    IEC61850_GOOSE_DCI_C.WriteLine ("static const uint16_t DCI_IEC61850_GOOSE_PUB_DATA_TOTAL_GROUP_ID = " & (Group_Counter) & "U;")
    IEC61850_GOOSE_DCI_C.WriteLine ("const IEC61850_GOOSE_DCI::group_assembly_t iec61850_goose_pub_dci_group_struct[DCI_IEC61850_GOOSE_PUB_DATA_TOTAL_GROUP_ID] =")
    IEC61850_GOOSE_DCI_C.WriteLine ("{")
    
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    Groupstart_Flag = 0
    Group_Index = 0
    SubGroupstart_Flag = 0
    Groupzero_Flag = 0
    Group_Counter = 0
    SubGroup_Counter = 0
    Total_SubGroup_Counter = 0
    Max_Parameters = 0
    While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Pub_Start") Then
                Groupstart_Flag = 1
                Max_Parameters = 0
                Group_Counter = Group_Counter + 1
                IEC61850_GOOSE_DCI_C.WriteLine ("    {")
                IEC61850_GOOSE_DCI_C.WriteLine ("        " & Cells(counter, IEC61850_GOOSE_GROUP_ID_COL).text & "U,")
                IEC61850_GOOSE_DCI_C.WriteLine ("        DCI_GOOSE_PUBLISH_CB_REF_NAME_" & Group_Counter & ",")
                IEC61850_GOOSE_DCI_C.WriteLine ("        DCI_GOOSE_PUBLISH_MAIN_GROUP_PARAM_" & Group_Counter & ",")
        ElseIf (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Pub_End") Then
                Groupstart_Flag = 0
                IEC61850_GOOSE_DCI_C.WriteLine ("        " & Total_SubGroup_Counter & "U,")
                IEC61850_GOOSE_DCI_C.WriteLine ("        " & Max_Parameters & "U,")
                IEC61850_GOOSE_DCI_C.WriteLine ("    },")
                Total_SubGroup_Counter = 0
                Groupzero_Flag = 0
        End If
        If ((Groupstart_Flag = 1) And (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Start")) Then
                If (Groupzero_Flag = 0) Then
                Groupzero_Flag = 1
                IEC61850_GOOSE_DCI_C.WriteLine ("        DCI_GOOSE_PUBLISH_SUB_GROUP_STRUCT_" & Group_Counter & ",")
                End If
                Total_SubGroup_Counter = Total_SubGroup_Counter + 1
                SubGroupstart_Flag = 1
                SubGroup_Counter = 0
        ElseIf ((Groupstart_Flag = 1) And (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "End")) Then
                SubGroupstart_Flag = 0
        End If
        
            If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "par") Then
                If ((Groupstart_Flag = 1) And (SubGroupstart_Flag = 0)) Then
                    Max_Parameters = Max_Parameters + 1
                ElseIf (SubGroupstart_Flag = 1) Then
                    SubGroup_Counter = SubGroup_Counter + 1
                End If
        End If
        counter = counter + 1
    Wend
    IEC61850_GOOSE_DCI_C.WriteLine ("};")
'**************************************************************************************************************************
'******     Generate the IEC61850 Group Structure.
'****************************************************************************************************************************
    IEC61850_GOOSE_DCI_C.WriteLine ("")
    IEC61850_GOOSE_DCI_C.WriteLine ("//*******************************************************")
    IEC61850_GOOSE_DCI_C.WriteLine ("//******     The IEC61850 GOOSE publish Group Structure.")
    IEC61850_GOOSE_DCI_C.WriteLine ("//*******************************************************")
    IEC61850_GOOSE_DCI_C.WriteLine ("")
    IEC61850_GOOSE_DCI_C.WriteLine ("const IEC61850_GOOSE_DCI::goose_group_info_t iec61850_goose_pub_dci_data_group_info =")
    IEC61850_GOOSE_DCI_C.WriteLine ("{")
    IEC61850_GOOSE_DCI_C.WriteLine ("    DCI_IEC61850_GOOSE_PUB_DATA_TOTAL_GROUP_ID,   //iec_param_t iec61850_goose_total_group")
    IEC61850_GOOSE_DCI_C.WriteLine ("    iec61850_goose_pub_dci_group_struct,        //const group_assembly_t* iec61850_goose_dci_group_struct")
    IEC61850_GOOSE_DCI_C.WriteLine ("};")



'**************************************************************************************************************************
'******     Generate the IEC61850 GOOSE subscribe group deatils
'****************************************************************************************************************************
    IEC61850_GOOSE_DCI_C.WriteLine ("//*******************************************************")
    IEC61850_GOOSE_DCI_C.WriteLine ("//******     The IEC61850 GOOSE Subscribe Group and Sub group array details.")
    IEC61850_GOOSE_DCI_C.WriteLine ("//*******************************************************")

Groupstart_Flag = 0
    Group_Index = 0
    SubGroupstart_Flag = 0
    Groupzero_Flag = 0
    Group_Counter = 0
    SubGroup_Counter = 0
    Total_SubGroup_Counter = 0
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    
   While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Sub_Start") Then
                Groupstart_Flag = 1
                Group_Counter = Group_Counter + 1
        ElseIf (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Sub_End") Then
                Groupstart_Flag = 0
                Total_SubGroup_Counter = 0
                Groupzero_Flag = 0
        End If
        If ((Groupstart_Flag = 1) And (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Start")) Then
                If (Groupzero_Flag = 0) Then
                Groupzero_Flag = 1
                End If
                Total_SubGroup_Counter = Total_SubGroup_Counter + 1
                IEC61850_GOOSE_DCI_C.WriteLine ("const iec61850_tagtype_t DCI_GOOSE_SUBSCRIBE_SUB_GROUP_PARAMS_TAG_" & Group_Counter & "_" & Total_SubGroup_Counter & "[] = ")
                IEC61850_GOOSE_DCI_C.WriteLine ("{")
                SubGroupstart_Flag = 1
                SubGroup_Counter = 0
        ElseIf ((Groupstart_Flag = 1) And (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "End")) Then
                SubGroupstart_Flag = 0
                IEC61850_GOOSE_DCI_C.WriteLine ("};")
        End If
        
            If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "par") Then
                If ((Groupstart_Flag = 1) And (SubGroupstart_Flag = 1)) Then
                    IEC61850_GOOSE_DCI_C.WriteLine ("    " & Cells(counter, IEC61850_GOOSE_DATA_TYPE_COL).text & ",")
                    SubGroup_Counter = SubGroup_Counter + 1
                End If
        End If
        counter = counter + 1
    Wend
    
    
    Groupstart_Flag = 0
    Group_Index = 0
    SubGroupstart_Flag = 0
    Groupzero_Flag = 0
    Group_Counter = 0
    SubGroup_Counter = 0
    Total_SubGroup_Counter = 0
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    
   While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Sub_Start") Then
                Groupstart_Flag = 1
                Group_Counter = Group_Counter + 1
                IEC61850_GOOSE_DCI_C.WriteLine ("const IEC61850_GOOSE_DCI::group_param_t DCI_GOOSE_SUBSCRIBE_MAIN_GROUP_PARAM_" & Group_Counter & "[] = ")
                IEC61850_GOOSE_DCI_C.WriteLine ("{")
        ElseIf (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Sub_End") Then
                Groupstart_Flag = 0
                Total_SubGroup_Counter = 0
                Groupzero_Flag = 0
        End If
        If ((Groupstart_Flag = 1) And (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Start")) Then
                If (Groupzero_Flag = 0) Then
                Groupzero_Flag = 1
                IEC61850_GOOSE_DCI_C.WriteLine ("};")
                End If
                Total_SubGroup_Counter = Total_SubGroup_Counter + 1
                IEC61850_GOOSE_DCI_C.WriteLine ("const IEC61850_GOOSE_DCI::sub_group_param_t DCI_GOOSE_SUBSCRIBE_SUB_GROUP_PARAM_" & Group_Counter & "_" & Total_SubGroup_Counter & "[] = ")
                IEC61850_GOOSE_DCI_C.WriteLine ("{")
                SubGroupstart_Flag = 1
                SubGroup_Counter = 0
        ElseIf ((Groupstart_Flag = 1) And (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "End")) Then
                SubGroupstart_Flag = 0
                IEC61850_GOOSE_DCI_C.WriteLine ("};")
        End If
        
            If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "par") Then
                If ((Groupstart_Flag = 1) And (SubGroupstart_Flag = 0)) Then
                    IEC61850_GOOSE_DCI_C.WriteLine ("    " & Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text & "_DCID,")
                ElseIf (SubGroupstart_Flag = 1) Then
                    IEC61850_GOOSE_DCI_C.WriteLine ("    " & Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text & "_DCID,")
                    SubGroup_Counter = SubGroup_Counter + 1
                End If
        End If
        counter = counter + 1
    Wend
'**************************************************************************************************************************
'******     Generate the IEC61850 GOOSE publish subgroup straucture array details.
'****************************************************************************************************************************
 
    IEC61850_GOOSE_DCI_C.WriteLine ("//*******************************************************")
    IEC61850_GOOSE_DCI_C.WriteLine ("//******     The IEC61850 GOOSE Subscribe Sub group structure array details.")
    IEC61850_GOOSE_DCI_C.WriteLine ("//*******************************************************")

    Groupstart_Flag = 0
    Group_Index = 0
    SubGroupstart_Flag = 0
    Groupzero_Flag = 0
    Group_Counter = 0
    SubGroup_Counter = 0
    Total_SubGroup_Counter = 0
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    
   While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Sub_Start") Then
                Groupstart_Flag = 1
                Group_Counter = Group_Counter + 1
                IEC61850_GOOSE_DCI_C.WriteLine ("static const char_t DCI_GOOSE_SUBSCRIBE_CB_REF_NAME_" & Group_Counter & "[] = {" & Cells(counter, IEC61850_GOOSE_CB_REF_NAME_COL).text & "};")
        ElseIf (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Sub_End") Then
                Groupstart_Flag = 0
                Total_SubGroup_Counter = 0
                Groupzero_Flag = 0
                IEC61850_GOOSE_DCI_C.WriteLine ("};")
        End If
        If ((Groupstart_Flag = 1) And (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Start")) Then
                If (Groupzero_Flag = 0) Then
                IEC61850_GOOSE_DCI_C.WriteLine ("const IEC61850_GOOSE_DCI::subgroup_assembly_t DCI_GOOSE_SUBSCRIBE_SUB_GROUP_STRUCT_" & Group_Counter & "[] = ")
                IEC61850_GOOSE_DCI_C.WriteLine ("{")
                Groupzero_Flag = 1
                End If
                Total_SubGroup_Counter = Total_SubGroup_Counter + 1
                IEC61850_GOOSE_DCI_C.WriteLine ("  {")
                SubGroupstart_Flag = 1
                SubGroup_Counter = 0
        ElseIf ((Groupstart_Flag = 1) And (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "End")) Then
                SubGroupstart_Flag = 0
                IEC61850_GOOSE_DCI_C.WriteLine ("    DCI_GOOSE_SUBSCRIBE_SUB_GROUP_PARAM_" & Group_Counter & "_" & Total_SubGroup_Counter & ",")
                IEC61850_GOOSE_DCI_C.WriteLine ("    DCI_GOOSE_SUBSCRIBE_SUB_GROUP_PARAMS_TAG_" & Group_Counter & "_" & Total_SubGroup_Counter & ",")
                IEC61850_GOOSE_DCI_C.WriteLine ("    " & SubGroup_Counter & "U,")
                IEC61850_GOOSE_DCI_C.WriteLine ("  },")
        End If
        
            If ((Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "par") And (SubGroupstart_Flag = 1)) Then
                SubGroup_Counter = SubGroup_Counter + 1
            End If
        counter = counter + 1
    Wend

'**************************************************************************************************************************
'******     Store the IEC61850 GOOSE publish Groups in Array.
'****************************************************************************************************************************
    
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    Group_Counter = 0
    Max_Parameters = 0
    
    While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Sub_Start") Then
            Group_Counter = Group_Counter + 1
        End If
        counter = counter + 1
    Wend
    
    IEC61850_GOOSE_DCI_C.WriteLine ("")
    IEC61850_GOOSE_DCI_C.WriteLine ("//*******************************************************")
    IEC61850_GOOSE_DCI_C.WriteLine ("//******     The IEC61850 GOOSE Subscribe Group Cross Reference Table.")
    IEC61850_GOOSE_DCI_C.WriteLine ("//*******************************************************")
    IEC61850_GOOSE_DCI_C.WriteLine ("")
    IEC61850_GOOSE_DCI_C.WriteLine ("static const uint16_t DCI_IEC61850_GOOSE_SUB_DATA_TOTAL_GROUP_ID = " & (Group_Counter) & "U;")
    IEC61850_GOOSE_DCI_C.WriteLine ("const IEC61850_GOOSE_DCI::group_assembly_t iec61850_goose_sub_dci_group_struct[DCI_IEC61850_GOOSE_SUB_DATA_TOTAL_GROUP_ID] =")
    IEC61850_GOOSE_DCI_C.WriteLine ("{")
    
    counter = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    Groupstart_Flag = 0
    Group_Index = 0
    SubGroupstart_Flag = 0
    Groupzero_Flag = 0
    Group_Counter = 0
    SubGroup_Counter = 0
    Total_SubGroup_Counter = 0
    Max_Parameters = 0
    While (Cells(counter, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Sub_Start") Then
                Groupstart_Flag = 1
                Max_Parameters = 0
                Group_Counter = Group_Counter + 1
                IEC61850_GOOSE_DCI_C.WriteLine ("    {")
                IEC61850_GOOSE_DCI_C.WriteLine ("        " & Cells(counter, IEC61850_GOOSE_GROUP_ID_COL).text & "U,")
                IEC61850_GOOSE_DCI_C.WriteLine ("        DCI_GOOSE_SUBSCRIBE_CB_REF_NAME_" & Group_Counter & ",")
                IEC61850_GOOSE_DCI_C.WriteLine ("        DCI_GOOSE_SUBSCRIBE_MAIN_GROUP_PARAM_" & Group_Counter & ",")
        ElseIf (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Sub_End") Then
                Groupstart_Flag = 0
                IEC61850_GOOSE_DCI_C.WriteLine ("        " & Total_SubGroup_Counter & "U,")
                IEC61850_GOOSE_DCI_C.WriteLine ("        " & Max_Parameters & "U,")
                IEC61850_GOOSE_DCI_C.WriteLine ("    },")
                Total_SubGroup_Counter = 0
                Groupzero_Flag = 0
        End If
        If ((Groupstart_Flag = 1) And (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "Start")) Then
                If (Groupzero_Flag = 0) Then
                Groupzero_Flag = 1
                IEC61850_GOOSE_DCI_C.WriteLine ("        DCI_GOOSE_SUBSCRIBE_SUB_GROUP_STRUCT_" & Group_Counter & ",")
                End If
                Total_SubGroup_Counter = Total_SubGroup_Counter + 1
                SubGroupstart_Flag = 1
                SubGroup_Counter = 0
        ElseIf ((Groupstart_Flag = 1) And (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "End")) Then
                SubGroupstart_Flag = 0
        End If
        
            If (Cells(counter, IEC61850_GOOSE_TYPE_COL).text = "par") Then
                If ((Groupstart_Flag = 1) And (SubGroupstart_Flag = 0)) Then
                    Max_Parameters = Max_Parameters + 1
                ElseIf (SubGroupstart_Flag = 1) Then
                    SubGroup_Counter = SubGroup_Counter + 1
                End If
        End If
        counter = counter + 1
    Wend
    IEC61850_GOOSE_DCI_C.WriteLine ("};")
'**************************************************************************************************************************
'******     Generate the IEC61850 Group Structure.
'****************************************************************************************************************************
    IEC61850_GOOSE_DCI_C.WriteLine ("")
    IEC61850_GOOSE_DCI_C.WriteLine ("//*******************************************************")
    IEC61850_GOOSE_DCI_C.WriteLine ("//******     The IEC61850 GOOSE Subscribe Group Structure.")
    IEC61850_GOOSE_DCI_C.WriteLine ("//*******************************************************")
    IEC61850_GOOSE_DCI_C.WriteLine ("")
    IEC61850_GOOSE_DCI_C.WriteLine ("const IEC61850_GOOSE_DCI::goose_group_info_t iec61850_goose_sub_dci_data_group_info =")
    IEC61850_GOOSE_DCI_C.WriteLine ("{")
    IEC61850_GOOSE_DCI_C.WriteLine ("    DCI_IEC61850_GOOSE_SUB_DATA_TOTAL_GROUP_ID,   //iec_param_t iec61850_goose_total_group")
    IEC61850_GOOSE_DCI_C.WriteLine ("    iec61850_goose_sub_dci_group_struct,        //const group_assembly_t* iec61850_goose_dci_group_struct")
    IEC61850_GOOSE_DCI_C.WriteLine ("};")

'**************************************************************************************************************************
'******     Generate the IEC61850 GOOSE publish subgroup straucture array details.
'****************************************************************************************************************************
 
    IEC61850_GOOSE_DCI_C.WriteLine ("//*******************************************************")
    IEC61850_GOOSE_DCI_C.WriteLine ("//******     The IEC61850 GOOSE message info structure ")
    IEC61850_GOOSE_DCI_C.WriteLine ("//*******************************************************")

    IEC61850_GOOSE_DCI_C.WriteLine ("const IEC61850_GOOSE_DCI::goose_message_info_t iec61850_goose_msg_info =")
    IEC61850_GOOSE_DCI_C.WriteLine ("{")
    IEC61850_GOOSE_DCI_C.WriteLine ("    &iec61850_goose_pub_dci_data_group_info,       //const goose_group_info_t* pub_group_profile")
    IEC61850_GOOSE_DCI_C.WriteLine ("    &iec61850_goose_sub_dci_data_group_info        //const goose_group_info_t* sub_group_profile")
    IEC61850_GOOSE_DCI_C.WriteLine ("};")

'**************************************************************************************************************************
'******     Callback function for verifying the parameters.
'****************************************************************************************************************************
    
    Call Verify_IEC61850_GOOSE_List
   

'****************************************************************************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'***    Start Creating the .Json_Tree file header
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************************************************************************

START_ROW = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
Dim count As Integer
count = 0
Groupstart_Flag = 0
SubGroupstart_Flag = 0
IEC61850_GOOSE_DCI_Json_Tree.WriteLine ("-- GOOSE Publish Groups -- ")
While (Cells(START_ROW, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)

    If (Cells(START_ROW, IEC61850_GOOSE_TYPE_COL).text = Empty) Then
        START_ROW = START_ROW + 1
    Else
        If (Cells(START_ROW, IEC61850_GOOSE_TYPE_COL).text = "Pub_Start") Then
            IEC61850_GOOSE_DCI_Json_Tree.WriteLine ((Cells(START_ROW, IEC61850_GOOSE_DESCRIP_LOGIC_DETAILS_COL).text) & Chr(172))
            Groupstart_Flag = 1
        ElseIf ((Cells(START_ROW, IEC61850_GOOSE_TYPE_COL).text = "par") And (SubGroupstart_Flag = 0) And (Groupstart_Flag = 1)) Then
            count = 8
            IEC61850_GOOSE_DCI_Json_Tree.WriteLine (Space(count) & Cells(START_ROW, IEC61850_GOOSE_DESCRIP_COL).text)
        ElseIf ((Cells(START_ROW, IEC61850_GOOSE_TYPE_COL).text = "Start") And (Groupstart_Flag = 1)) Then
            count = 8
            IEC61850_GOOSE_DCI_Json_Tree.WriteLine (Space(count) & Cells(START_ROW, IEC61850_GOOSE_DESCRIP_LOGIC_DETAILS_COL).text & Chr(172))
            SubGroupstart_Flag = 1
        ElseIf ((Cells(START_ROW, IEC61850_GOOSE_TYPE_COL).text = "par") And (SubGroupstart_Flag = 1) And (Groupstart_Flag = 1)) Then
            count = 16
            IEC61850_GOOSE_DCI_Json_Tree.WriteLine (Space(count) & Cells(START_ROW, IEC61850_GOOSE_DESCRIP_COL).text)
        ElseIf ((Cells(START_ROW, IEC61850_GOOSE_TYPE_COL).text = "End") And (SubGroupstart_Flag = 1)) Then
            SubGroupstart_Flag = 0
        ElseIf (Cells(START_ROW, IEC61850_GOOSE_TYPE_COL).text = "Pub_End") Then
            Groupstart_Flag = 0
            IEC61850_GOOSE_DCI_Json_Tree.WriteLine (Chr(10))
        End If
    START_ROW = START_ROW + 1
    End If

Wend

START_ROW = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
count = 0
Groupstart_Flag = 0
SubGroupstart_Flag = 0
IEC61850_GOOSE_DCI_Json_Tree.WriteLine ("-- GOOSE Subscribe Groups -- ")
While (Cells(START_ROW, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)

    If (Cells(START_ROW, IEC61850_GOOSE_TYPE_COL).text = Empty) Then
        START_ROW = START_ROW + 1
    Else
        If (Cells(START_ROW, IEC61850_GOOSE_TYPE_COL).text = "Sub_Start") Then
            IEC61850_GOOSE_DCI_Json_Tree.WriteLine ((Cells(START_ROW, IEC61850_GOOSE_DESCRIP_LOGIC_DETAILS_COL).text) & Chr(172))
            Groupstart_Flag = 1
        ElseIf ((Cells(START_ROW, IEC61850_GOOSE_TYPE_COL).text = "par") And (SubGroupstart_Flag = 0) And (Groupstart_Flag = 1)) Then
            count = 8
            IEC61850_GOOSE_DCI_Json_Tree.WriteLine (Space(count) & Cells(START_ROW, IEC61850_GOOSE_DESCRIP_COL).text)
        ElseIf ((Cells(START_ROW, IEC61850_GOOSE_TYPE_COL).text = "Start") And (Groupstart_Flag = 1)) Then
            count = 8
            IEC61850_GOOSE_DCI_Json_Tree.WriteLine (Space(count) & Cells(START_ROW, IEC61850_GOOSE_DESCRIP_LOGIC_DETAILS_COL).text & Chr(172))
            SubGroupstart_Flag = 1
        ElseIf ((Cells(START_ROW, IEC61850_GOOSE_TYPE_COL).text = "par") And (SubGroupstart_Flag = 1) And (Groupstart_Flag = 1)) Then
            count = 16
            IEC61850_GOOSE_DCI_Json_Tree.WriteLine (Space(count) & Cells(START_ROW, IEC61850_GOOSE_DESCRIP_COL).text)
        ElseIf ((Cells(START_ROW, IEC61850_GOOSE_TYPE_COL).text = "End") And (SubGroupstart_Flag = 1)) Then
            SubGroupstart_Flag = 0
        ElseIf (Cells(START_ROW, IEC61850_GOOSE_TYPE_COL).text = "Sub_End") Then
            Groupstart_Flag = 0
            IEC61850_GOOSE_DCI_Json_Tree.WriteLine (Chr(10))
        End If
    START_ROW = START_ROW + 1
    End If

Wend
    
    IEC61850_GOOSE_DCI_H.Close
    IEC61850_GOOSE_DCI_C.Close
End Sub

Public Sub Verify_IEC61850_GOOSE_List()

'*****************************************************************************************************************************
'******     Constants
'**************************************************************************************************************************
    Application.Calculation = xlCalculationManual
    Call IEC61850_GOOSE_Restore_Cell_Color
    
    Dim sheet_name As String
   
'*********************
'******     Begin
'*********************
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select

''****************************************************************************************************************************
''******     Find the Data items that have been removed from the main list
''******     This will go through the main list and make the descriptions correct in the slave sheets
''****************************************************************************************************************************
 ' This below lines description column check shall cover only Generic BLE Service
    IEC61850_GOOSE_row_ctr = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    dci_end_row = Last_DCI_Row()
    While Cells(IEC61850_GOOSE_row_ctr, IEC61850_GOOSE_TYPE_COL).text <> Empty
        lookup_string = Cells(IEC61850_GOOSE_row_ctr, IEC61850_GOOSE_DATA_DEFINE_COL).text
        If lookup_string <> Empty Then
            Set c = Find_DCID_Cell(lookup_string)
                If c Is Nothing Then
                    Cells(IEC61850_GOOSE_row_ctr, IEC61850_GOOSE_DATA_DEFINE_COL).Interior.Color = RGB(255, 0, 0)
                    Cells(IEC61850_GOOSE_row_ctr, IEC61850_GOOSE_DESCRIP_COL).Interior.Color = RGB(255, 0, 0)
                Else    ' If found then we go through and fix the descriptions.
                    Cells(IEC61850_GOOSE_row_ctr, IEC61850_GOOSE_DESCRIP_COL).value = c.offset(0, DCI_DESCRIPTORS_COL - DCI_DEFINES_COL).text
                    
                End If
            
        End If
        IEC61850_GOOSE_row_ctr = IEC61850_GOOSE_row_ctr + 1
    Wend
  
End Sub


'****************************************************************************************************************************
'
'   Restores interior color of cells to normal.
'
'****************************************************************************************************************************

Sub IEC61850_GOOSE_Restore_Cell_Color()
    Dim row As Long
    row = IEC61850_GOOSE_DATA_START_ROW_GENERIC_ACCESS_SERVICE
    While (Cells(row, IEC61850_GOOSE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        Rows(row).Interior.ColorIndex = 0
        row = row + 1
    Wend
End Sub



