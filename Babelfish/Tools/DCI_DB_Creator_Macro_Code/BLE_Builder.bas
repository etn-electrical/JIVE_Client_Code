Attribute VB_Name = "BLE_Builder"
'***************************************************************
'BLE Sheet Indexing.
'***************************************************************
Public Const BLE_DESCRIP_COL = "A"
Public Const BLE_DATA_DEFINE_COL = "B"
Public Const BLE_DISPLAY_TYPE_COL = "C"
Public Const BLE_MENU_PATH_COL = "F"
Public Const BLE_DISPLAY_NAME_COL = "G"
Public Const BLE_READ_PERMISSION_COL = "H"
Public Const BLE_WRITE_PERMISSION_COL = "I"
Public Const BLE_DCI_ID_COL = "J"
Public Const BLE_SERVICE_UUID_COL = "L"
Public Const BLE_TYPE_COL = "P"
Public Const BLE_DEVICE_CHARACTERISTIC_UUID_COL = "T"
Public Const BLE_DEVICE_ACCESS_LEVEL_COL = "T"
Public Const BLE_NOTIFICATION_ATTRIB_COL = "U"
Public Const BLE_WRITE_WITHOUT_RESPONSE_ATTRIB_COL = "V"
Public Const BLE_DEVICE_DISPALY_NAME_COL = "CA"
Public Const BLE_DEVICE_UNITS_LENGTH_COL = "CB"
Public Const BLE_DEVICE_FORMAT_COL = "CC"
Public Const BLE_PARAM_LONG_DESC_COL = "CD"
Public Const BLE_ASSEMBLY_COL = "CE"

'Row number used for User_Roles
Public Const BLE_DEVICE_ACCESS_LEVELS_ROW = 2

'Row number used for BLE_Module_Selection
Public Const BLE_DATA_BLE_MODULE_SELECTION_ROW = 10

Public Const BLE_SILABS_MODULE = "SILICON_LABS"
Public Const BLE_ESP32 = "ESP32"
Public Const BLE_STM32WB55 = "STM32WB55"

'Row numbers used in XML File
Public Const BLE_HEADER_ROW = 9
Public Const BLE_NAME_ROW = 10
Public Const BLE_OUT_ROW = 11
Public Const BLE_PREFIX_ROW = 12
Public Const HARDWARE_SELECT_ROW = 8

'These Start Row Numbers has to be followed same since there are three services implemented in BLE
'1.Generic Access Service should not be changed anytime
'   It carries the device name information while advertising
'2.Device Profile parameters should not be deleted
'  it carries the mandatory information to create the Device GATT profile
'3.Any parameter can be added or modified in the customized BLE service

Public Const BLE_DATA_START_ROW = 15

'Columns cannnot be interchanged or adding new column in the middle is prohibited

Public Const BLE_DCI_DATA_DEFINE_COL = "B"
Public Const BLE_DCI_CHARACTERISTIC_VALUE_TYPE_COL = "C"
Public Const BLE_DCI_TOTAL_LENGTH_COL = "F"
Public Const BLE_DCI_DEFAULT_COL = "G"
Public Const BLE_DCI_MIN_VALUE_COL = "H"
Public Const BLE_DCI_MAX_VALUE_COL = "I"
Public Const BLE_DCI_BIT_FIELD_COL = "K"
Public Const BLE_DCI_ENUM_DETAILS_COL = "Z"
Public Const BLE_DCI_ENUM_SUPPORT_COL = "AI"
Public Const BLE_DCI_RAM_VAL_WRITE_ACCESS_COL = "BA"
Public Const BLE_DCI_INIT_VAL_READ_ACCESS_COL = "BB"
Public Const BLE_DCI_INIT_VAL_WRITE_ACCESS_COL = "BC"
Public Const BLE_DCI_DEFAULT_READ_ACCESS_COL = "BD"
Public Const BLE_DCI_SHORT_NAME_COL = "CA"

' Constant for Maximum Payload
Public Const MAX_PAYLOAD = 255

' Base Char ID used in the Silabs BLE Module in the database
Public Const BASE_CHAR_ID_SI_LABS_GATT_DATA_BASE = 4

' Base Char ID used in the ESP32 Module in the database
Public Const BASE_CHAR_ID_ESP32_GENERIC_ATTRIBUTE_GATT_DATA_BASE = 28
Public Const BASE_CHAR_ID_ESP32_CUSTOM_SERVICE_GATT_DATA_BASE = 39

'Max number of characteristics UUID for STM32WB55
Public Const MAX_LIMIT_CHARACTERISTICS_UUID_STM32WB55 = 63

Public Unique_Service_UUID_Count As Integer
Public Unique_ServiceUUIDs() As Integer
Public Characteristics_UUIDs() As Integer
Public Service_UUID_Index As Integer
Public Custom_Sevice_Row As Integer
Private BLE_DCI_JSON As TextStream
Private BLE_DCI_Json_Tree As TextStream

'Generic access service UUID is standard service it never change.
Public Const GENERIC_ACCESS_SERVICE = 1800

Public Const BLE_BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"

'Dedicated service assembly names to identify the row number to align the Service in JSON (w.r.t) to mobile App screen
Public Const BLE_LED_CONTROL_SERVICE = "LED CONTROL"
Public Const BLE_DEVICE_PROFILE_SERVICE = "Device Details_1"
Public Const BLE_CUSTOM_PROFILE_SEPERATOR = "Type"



'**************************
'**************************
'BLE Construction sheet.
'**************************
'**************************

Sub Activate_Sheet()
    Worksheets("BLE").Activate
End Sub

Sub Create_BLE_DCI_Xfer()
    Application.Calculation = xlCalculationManual
    
    Dim sheet_name As String
    Dim description_string As String
    Dim DCI_STRING As String
    Dim num_unique_data As Integer
    Dim file_name As String
    Dim lower_case_file_name As String
    Dim ble_index_ctr As Integer
    Dim OneTimeFlag As Boolean
'*********************
'******     Begin
'*********************
    Sheets("BLE").Select

    file_name = Cells(3, 3).value
    lower_case_file_name = LCase(file_name)
    upper_case_file_name = UCase(file_name)
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\" & Cells(4, 3).value
    Set BLE_DCI_C = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(5, 3).value
    Set BLE_DCI_H = fs.CreateTextFile(file_path, True)
    
    'Making font type and size consistent.
    Worksheets("BLE").Range("A:CD").Font.name = "Arial"
    Worksheets("BLE").Range("A:CD").Font.Size = 10
    
'XML File only used and tested for Silicon Labs BLE Module and not applicable for other BLE Modules
    If (Cells(BLE_DATA_BLE_MODULE_SELECTION_ROW, BLE_DESCRIP_COL).value = BLE_SILABS_MODULE) Then
        file_path = ActiveWorkbook.Path & "\" & Cells(7, 3).value
        Set BLE_DCI_XML = fs.CreateTextFile(file_path, True)
    End If
    
'JSON, JSON Tree generation made it common for all BLE modules
    file_path = ActiveWorkbook.Path & "\" & Cells(6, 3).value
    Set BLE_DCI_JSON = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(8, 3).value
    Set BLE_DCI_Json_Tree = fs.CreateTextFile(file_path, True)
    
'Find the Row number of the custom profile
    ble_index_ctr = BLE_DATA_START_ROW
    START_ROW = BLE_DATA_START_ROW

    OneTimeFlag = False
    While (Cells(ble_index_ctr, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        If (Cells(ble_index_ctr, BLE_DISPLAY_TYPE_COL).text = BLE_CUSTOM_PROFILE_SEPERATOR) Then
            START_ROW = ble_index_ctr + 1
            Custom_Sevice_Row = START_ROW
        End If
        ble_index_ctr = ble_index_ctr + 1
    Wend
    
    Call Fill_ServiceUUIDs

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

    BLE_DCI_H.WriteLine ("/**")
    BLE_DCI_H.WriteLine ("*****************************************************************************************")
    BLE_DCI_H.WriteLine ("*   @file")
    BLE_DCI_H.WriteLine ("*")
    BLE_DCI_H.WriteLine ("*   @brief")
    BLE_DCI_H.WriteLine ("*")
    BLE_DCI_H.WriteLine ("*   @details")
    BLE_DCI_H.WriteLine ("*")
    BLE_DCI_H.WriteLine ("*   @version")
    BLE_DCI_H.WriteLine ("*   C++ Style Guide Version 1.0")
    BLE_DCI_H.WriteLine ("*")
    BLE_DCI_H.WriteLine ("*   @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    BLE_DCI_H.WriteLine ("*")
    BLE_DCI_H.WriteLine ("*****************************************************************************************")
    BLE_DCI_H.WriteLine ("*/")
    BLE_DCI_H.WriteLine ("#ifndef " & upper_case_file_name & "_H")
    BLE_DCI_H.WriteLine ("#define " & upper_case_file_name & "_H")
    BLE_DCI_H.WriteLine ("")
    BLE_DCI_H.WriteLine ("#include ""DCI.h""")
    BLE_DCI_H.WriteLine ("#include ""DCI_Data.h""")
    BLE_DCI_H.WriteLine ("#include ""Ble_DCI.h""")
    BLE_DCI_H.WriteLine ("")
    
    '****************************************************************************************************************************
    '******     Esp32 related structure generation.
    '****************************************************************************************************************************
    If (Cells(BLE_DATA_BLE_MODULE_SELECTION_ROW, BLE_DESCRIP_COL).value = BLE_ESP32) Then
        Call Esp32_Specific(BLE_DCI_H)
    End If
'****************************************************************************************************************************
'******     Create the header and associated file structure.  Get ready to plop the data down.
'****************************************************************************************************************************
    BLE_DCI_H.WriteLine ("//*******************************************************")
    BLE_DCI_H.WriteLine ("//******     BLE structures and constants.")
    BLE_DCI_H.WriteLine ("//*******************************************************")
    BLE_DCI_H.WriteLine ("")
    BLE_DCI_H.WriteLine ("extern const Ble_DCI::ble_target_info_st_td_t ble_dci_data_target_info;")
    BLE_DCI_H.WriteLine ("extern const Ble_DCI::ble_group_info_st_td_t ble_dci_data_group_info;")
    BLE_DCI_H.WriteLine ("")
    BLE_DCI_H.WriteLine ("// Ble bonding flag dcid will be accessed from app.c to retrieve and update the bonding info.")
    
    START_ROW = BLE_DATA_START_ROW
    
   Do While (Cells(START_ROW, BLE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(START_ROW, BLE_DATA_DEFINE_COL).text = "DCI_BLE_DEVICE_BONDING_INFO") Then
            BLE_DCI_H.WriteLine ("const Ble_DCI::CHAR_ID_TD DCI_DEVICE_BONDING_INFO_BLE_CHAR_ID " & "= " & Cells(START_ROW, BLE_DCI_ID_COL).text & "U;")
            Exit Do
        End If
        START_ROW = START_ROW + 1
   Loop
    
    BLE_DCI_H.WriteLine ("")

'**************************************************************************************************************************
'******     Generate the total RAM size consumed for BLE Parameters
'****************************************************************************************************************************
    Dim ble_dci_total_ram As Integer
    dci_defines_row_ctr = DCI_DEFINES_START_ROW
    ble_dci_total_ram = 0
    While Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> Empty
        DCI_STRING = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text
        row_counter = BLE_DATA_START_ROW
        While (Cells(row_counter, BLE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA) _
                    And (Cells(row_counter, BLE_DATA_DEFINE_COL).text <> DCI_STRING)
            row_counter = row_counter + 1
        Wend
        If Cells(row_counter, BLE_DATA_DEFINE_COL).text = DCI_STRING Then
            ble_dci_total_ram = ble_dci_total_ram + Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, TOTAL_LENGTH_COL).value
        End If
        dci_defines_row_ctr = dci_defines_row_ctr + 1
    Wend
    BLE_DCI_H.WriteLine ("static const uint32_t BLE_TOTAL_DCID_RAM_SIZE = " & ble_dci_total_ram & "U;")

    
'****************************************************************************************************************************
'****** Generating the BLE Param ID's Automatically and filling in Param ID (BLE_DCI_ID) coloumn.
'****************************************************************************************************************************
    
    Service_UUID_Index = 0
    Dim StrCompResult As Integer
    Dim Group_Assembly_Service_UUID_Index As Integer

    
    OneTimeFlag = False
    Group_Assembly_Service_UUID_Index = 1
    counter = BLE_DATA_START_ROW
    
    If (Cells(BLE_DATA_BLE_MODULE_SELECTION_ROW, BLE_DESCRIP_COL).value = BLE_ESP32) Then
        Cells(counter, BLE_DCI_ID_COL) = BASE_CHAR_ID_ESP32_GENERIC_ATTRIBUTE_GATT_DATA_BASE
        DCID_Value = BASE_CHAR_ID_ESP32_CUSTOM_SERVICE_GATT_DATA_BASE
    Else
        DCID_Value = BASE_CHAR_ID_SI_LABS_GATT_DATA_BASE
    End If

    
    'To Fill the BLE Param ID's Automatically in Param ID (BLE_DCI_ID) coloumn for Generic Access Service and Custom Service
    While (Service_UUID_Index < Unique_Service_UUID_Count)
        OneTimeFlag = False
        If (Cells(BLE_DATA_BLE_MODULE_SELECTION_ROW, BLE_DESCRIP_COL).value = BLE_ESP32) Then
            counter = BLE_DATA_START_ROW + 1
        Else
            counter = BLE_DATA_START_ROW
        End If
    
        While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            StrCompResult = StrComp(Cells(counter, BLE_SERVICE_UUID_COL).text, CStr(Unique_ServiceUUIDs(Service_UUID_Index)))
            If (StrCompResult = 0) Then
                If (OneTimeFlag = False) Then
                    DCID_Value = DCID_Value + 3
                    Cells(counter, BLE_DCI_ID_COL) = DCID_Value
                    OneTimeFlag = True
                ElseIf (OneTimeFlag = True) Then
                    DCID_Value = DCID_Value + 2
                    Cells(counter, BLE_DCI_ID_COL) = DCID_Value
                End If
            End If
            counter = counter + 1
        Wend
    Service_UUID_Index = Service_UUID_Index + 1
    Wend
    
'****************************************************************************************************************************
'****** Generating the Assembly name's Automatically and filling in Assembly name coloumn.
'****************************************************************************************************************************
    Dim Assembly_No As Integer
    counter = BLE_DATA_START_ROW

    While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, BLE_DISPLAY_TYPE_COL).text = "GroupStart") Then
            Cells(counter, BLE_MENU_PATH_COL) = "Assembly_" & Assembly_No
            Assembly_No = Assembly_No + 1
        ElseIf (Cells(counter, BLE_DISPLAY_TYPE_COL).text = "Type") Then
            Cells(counter + 1, BLE_MENU_PATH_COL) = "Assembly_" & Assembly_No
            Assembly_No = Assembly_No + 1
        End If
        counter = counter + 1
    Wend
    
'****************************************************************************************************************************
'****** Filling the Service UUID's from the user entry to the respective columns for the Group/Assembly feature
'****************************************************************************************************************************
    counter = BLE_DATA_START_ROW

    While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, BLE_DISPLAY_TYPE_COL).text = "GroupStart") Then
                Cells(counter, BLE_SERVICE_UUID_COL) = Cells(BLE_DEVICE_ACCESS_LEVELS_ROW, BLE_SERVICE_UUID_COL).text
        ElseIf (Cells(counter, BLE_DISPLAY_TYPE_COL).text = "Type") Then
            Cells(counter + 1, BLE_SERVICE_UUID_COL) = Cells(BLE_DEVICE_ACCESS_LEVELS_ROW, BLE_SERVICE_UUID_COL).text
        End If
        counter = counter + 1
    Wend
    
'****************************************************************************************************************************
'****** Checking for the Groupstart and GroupEnd count.
'****************************************************************************************************************************
    Dim Group_Check As Integer
    counter = BLE_DATA_START_ROW
    
    While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, BLE_DISPLAY_TYPE_COL).text = "GroupStart") Then
            Group_Check = Group_Check + 1
        ElseIf (Cells(counter, BLE_DISPLAY_TYPE_COL).text = "GroupEnd") Then
            Group_Check = Group_Check - 1
        End If
        counter = counter + 1
    Wend
    
    If (Group_Check <> "0") Then
        MsgBox "Error: GroupStart and GroupEnd are not in sync "
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
    
    BLE_DCI_C.WriteLine ("/**")
    BLE_DCI_C.WriteLine (" *****************************************************************************************")
    BLE_DCI_C.WriteLine (" *   @file")
    BLE_DCI_C.WriteLine (" *   @details See header file for module overview.")
    BLE_DCI_C.WriteLine (" *   @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    BLE_DCI_C.WriteLine (" *")
    BLE_DCI_C.WriteLine (" *****************************************************************************************")
    BLE_DCI_C.WriteLine (" */")
    BLE_DCI_C.WriteLine ("#include ""Includes.h""")
    BLE_DCI_C.WriteLine ("#include """ & file_name & ".h""")
    BLE_DCI_C.WriteLine ("#include ""Ble_Defines.h""")
    BLE_DCI_C.WriteLine ("")
    BLE_DCI_C.WriteLine ("")

''****************************************************************************************************************************
''****** Give error if any parameter name is added multiple times in the Code Define column in the BLE Sheet.
''****************************************************************************************************************************
   ' This error check shall cover Generic Service and the Device Profile Service
    
    counter = BLE_DATA_START_ROW
    While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        If (Cells(counter, BLE_DATA_DEFINE_COL).text = Empty) Then
            counter = counter + 1
        Else
            Cells(counter, BLE_DATA_DEFINE_COL).Font.Color = RGB(0, 0, 0)
            counter = counter + 1
        End If
    Wend
    
    counter = BLE_DATA_START_ROW
    NextCounter = 0
    FaultCounter = 0
    
    While (Cells(counter, BLE_DISPLAY_TYPE_COL).text <> "Type")
       NextCounter = 1
       While (Cells(counter + NextCounter, BLE_DATA_DEFINE_COL).text <> Empty)
           If (Cells(counter, BLE_DATA_DEFINE_COL).text = Cells(counter + NextCounter, BLE_DATA_DEFINE_COL).text) Then
               FaultCounter = FaultCounter + 1
               Cells(counter + NextCounter, BLE_DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
           End If
           NextCounter = NextCounter + 1
       Wend
       counter = counter + 1
    Wend

    If (FaultCounter <> 0) Then
        MsgBox "Error: One or more BLE Generic Access Service parameters are added multiple times"
        Exit Sub
    End If
    
 ' This error check shall cover only Customized BLE Service
 
    counter = Custom_Sevice_Row
    NextCounter = 0
    FaultCounter = 0

    While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
         If (Cells(counter, BLE_DATA_DEFINE_COL).text = Empty) Then
             counter = counter + 1
         Else
            NextCounter = 1
            While (Cells(counter + NextCounter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
                If (Cells(counter, BLE_DATA_DEFINE_COL).text = Cells(counter + NextCounter, BLE_DATA_DEFINE_COL).text) Then
                    FaultCounter = FaultCounter + 1
                    Cells(counter + NextCounter, BLE_DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
                End If
                NextCounter = NextCounter + 1
            Wend
            counter = counter + 1
         End If
    Wend

    If (FaultCounter <> 0) Then
        MsgBox "Error: One or more BLE Customized Service parameters are added multiple times"
        Exit Sub
    End If

''****************************************************************************************************************************
''****** Check if the parameter mentioned in the BLE sheet is present in the DCI sheet.
''****************************************************************************************************************************
   ' This error check shall cover Generic Service and the Device Profile Service
    
    counter = BLE_DATA_START_ROW
    While (Cells(counter, BLE_DISPLAY_TYPE_COL).text <> "Type")
        Cells(counter, BLE_DATA_DEFINE_COL).Font.Color = RGB(0, 0, 0)
        counter = counter + 1
    Wend

    counter = BLE_DATA_START_ROW
    FaultCounter = 0
    Dim row_number6 As Integer
    While (Cells(counter, BLE_DISPLAY_TYPE_COL).text <> "Type")
        If ((Cells(counter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA) And (Cells(counter, BLE_DATA_DEFINE_COL).text <> Empty)) Then
            row_number6 = 0
            row_number6 = Find_DCID_Row(Cells(counter, BLE_DATA_DEFINE_COL).text)
            If (row_number6 = 0) Then
                FaultCounter = FaultCounter + 1
                Cells(counter, BLE_DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
            End If
        End If
        counter = counter + 1
    Wend

    If (FaultCounter <> 0) Then
        MsgBox "Error: One or more BLE Generic Access Service parameters are not present in the DCI descriptor sheet"
        Exit Sub
    End If

''****************************************************************************************************************************
''****** Check if the parameter mentioned in the BLE sheet is present in the DCI sheet.
''****************************************************************************************************************************
   ' This error check shall cover only Customized BLE Service
    
    counter = Custom_Sevice_Row
    While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        Cells(counter, BLE_DATA_DEFINE_COL).Font.Color = RGB(0, 0, 0)
        counter = counter + 1
    Wend

    counter = Custom_Sevice_Row
    FaultCounter = 0
    Dim row_number7 As Integer
    While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        If (Cells(counter, BLE_DATA_DEFINE_COL).text = Empty) Then
            counter = counter + 1
        Else
            row_number7 = 0
            row_number7 = Find_DCID_Row(Cells(counter, BLE_DATA_DEFINE_COL).text)
            If (row_number7 = 0) Then
                FaultCounter = FaultCounter + 1
                Cells(counter, BLE_DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
            End If
            counter = counter + 1
        End If
    Wend

    If (FaultCounter <> 0) Then
        MsgBox "Error: One or more BLE Customized Service parameters are not present in the DCI descriptor sheet"
        Exit Sub
    End If

'****************************************************************************************************************************
'****** Give the error if the same DCI ID is assigned to multiple BLE parameters.
'****************************************************************************************************************************
' This error check shall cover Generic Service and the Device Profile Service

    counter = BLE_DATA_START_ROW
    While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        If (Cells(counter, BLE_DATA_DEFINE_COL).text = Empty) Then
            counter = counter + 1
        Else
            Cells(counter, BLE_DCI_ID_COL).Font.Color = RGB(0, 0, 0)
            counter = counter + 1
        End If
    Wend
    
    counter = BLE_DATA_START_ROW
    NextCounter = 0
    FaultCounter = 0

    While (Cells(counter, BLE_DISPLAY_TYPE_COL).text <> "Type")
            NextCounter = 1
        While (Cells(counter + NextCounter, BLE_DATA_DEFINE_COL).text <> Empty)
            If (Cells(counter, BLE_DCI_ID_COL).text = Cells(counter + NextCounter, BLE_DCI_ID_COL).text) Then
                FaultCounter = FaultCounter + 1
                Cells(counter + NextCounter, BLE_DCI_ID_COL).Font.Color = RGB(255, 0, 0)
            End If
            NextCounter = NextCounter + 1
        Wend
        counter = counter + 1
    Wend

    If (FaultCounter <> 0) Then
        MsgBox "Error: One or more BLE Generic service parameters are assigned the same DCI IDs"
        Exit Sub
    End If
    
 ' This error check shall cover only Customized BLE Service
 
    counter = Custom_Sevice_Row
    NextCounter = 0
    FaultCounter = 0

    While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
    If (Cells(counter, BLE_DATA_DEFINE_COL).text = Empty) Then
        counter = counter + 1
    Else
        NextCounter = 1
        While (Cells(counter + NextCounter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
            If (Cells(counter, BLE_DCI_ID_COL).text = Cells(counter + NextCounter, BLE_DCI_ID_COL).text) Then
                FaultCounter = FaultCounter + 1
                Cells(counter + NextCounter, BLE_DCI_ID_COL).Font.Color = RGB(255, 0, 0)
            End If
            NextCounter = NextCounter + 1
        Wend
        counter = counter + 1
    End If
    Wend

    If (FaultCounter <> 0) Then
        MsgBox "Error: One or more BLE Customized service parameters are assigned the same DCI IDs"
        Exit Sub
    End If

''****************************************************************************************************************************
''****** Give the error if the same Characteristic UUID is assigned to multiple BLE parameters.
''****************************************************************************************************************************
' This error check shall cover Generic Service and the Device Profile Service
    
    counter = BLE_DATA_START_ROW
    While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        If (Cells(counter, BLE_DATA_DEFINE_COL).text = Empty) Then
            counter = counter + 1
        Else
            Cells(counter, BLE_DEVICE_CHARACTERISTIC_UUID_COL).Font.Color = RGB(0, 0, 0)
            counter = counter + 1
        End If
    Wend
    
    counter = BLE_DATA_START_ROW
    NextCounter = 0
    FaultCounter = 0

    While (Cells(counter, BLE_DISPLAY_TYPE_COL).text <> "Type")
        NextCounter = 1
        While (Cells(counter + NextCounter, BLE_DATA_DEFINE_COL).text <> Empty)
            If (Cells(counter, BLE_DEVICE_CHARACTERISTIC_UUID_COL).text = Cells(counter + NextCounter, BLE_DEVICE_CHARACTERISTIC_UUID_COL).text) Then
                FaultCounter = FaultCounter + 1
                Cells(counter + NextCounter, BLE_DEVICE_CHARACTERISTIC_UUID_COL).Font.Color = RGB(255, 0, 0)
            End If
            NextCounter = NextCounter + 1
        Wend
        counter = counter + 1
    Wend

    If (FaultCounter <> 0) Then
        MsgBox "Error: One or more BLE Generic service parameters are assigned the same Characteristic UUID's"
        Exit Sub
    End If

 ' This error check shall cover only Customized BLE Service
 
    counter = Custom_Sevice_Row
    NextCounter = 0
    FaultCounter = 0

    While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
    If (Cells(counter, BLE_DATA_DEFINE_COL).text = Empty) Then
        counter = counter + 1
    Else
        NextCounter = 1
        While (Cells(counter + NextCounter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
            If (Cells(counter, BLE_DEVICE_CHARACTERISTIC_UUID_COL).text = Cells(counter + NextCounter, BLE_DEVICE_CHARACTERISTIC_UUID_COL).text) Then
                FaultCounter = FaultCounter + 1
                Cells(counter + NextCounter, BLE_DEVICE_CHARACTERISTIC_UUID_COL).Font.Color = RGB(255, 0, 0)
            End If
            NextCounter = NextCounter + 1
        Wend
        counter = counter + 1
    End If
    Wend

    If (FaultCounter <> 0) Then
        MsgBox "Error: One or more BLE Customized service parameters are assigned the same Characteristic UUID's"
        Exit Sub
    End If

'**************************************************************************************************************************
'******     Generate the BLE DCI_ID List.
'****************************************************************************************************************************
    BLE_DCI_C.WriteLine ("// *******************************************************")
    BLE_DCI_C.WriteLine ("// ******     BLE DCI_ID List")
    BLE_DCI_C.WriteLine ("// *******************************************************")
    BLE_DCI_C.WriteLine ("")
    
    row_counter = BLE_DATA_START_ROW
    
    While (Cells(row_counter, BLE_DISPLAY_TYPE_COL).text <> "Type") And (Cells(row_counter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        If (Cells(row_counter, BLE_DATA_DEFINE_COL).text <> Empty) Then
            BLE_DCI_C.WriteLine ("const Ble_DCI::CHAR_ID_TD " & Cells(row_counter, BLE_DATA_DEFINE_COL).text & "_BLE_CHAR_ID" & " = " & Cells(row_counter, BLE_DCI_ID_COL).text & "U;")
        End If
        row_counter = row_counter + 1
    Wend
    
    row_counter1 = Custom_Sevice_Row
    
    While (Cells(row_counter1, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        If (Cells(row_counter1, BLE_DATA_DEFINE_COL).text = Empty) Then
            row_counter1 = row_counter1 + 1
        Else
            BLE_DCI_C.WriteLine ("const Ble_DCI::CHAR_ID_TD " & Cells(row_counter1, BLE_DATA_DEFINE_COL).text & "_BLE_CHAR_ID" & " = " & Cells(row_counter1, BLE_DCI_ID_COL).text & "U;")
            row_counter1 = row_counter1 + 1
        End If
    Wend
    
'****************************************************************************************************************************
'******     Gotta calculate the total number of Characteristic uuids.
'****************************************************************************************************************************
    num_registers = 0
    num_registers1 = 0
    num_registers2 = 0
    counter = BLE_DATA_START_ROW
    counter1 = Custom_Sevice_Row
    
    While (Cells(counter, BLE_DISPLAY_TYPE_COL).text <> "Type") And (Cells(counter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        If (Cells(counter, BLE_DATA_DEFINE_COL).text = Empty) Then
            counter = counter + 1
        Else
            counter = counter + 1
            num_registers1 = num_registers1 + 1
        End If
    Wend
    
    While (Cells(counter1, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        If (Cells(counter1, BLE_DATA_DEFINE_COL).text = Empty) Then
            counter1 = counter1 + 1
        Else
            counter1 = counter1 + 1
            num_registers2 = num_registers2 + 1
        End If
    Wend
    
    num_registers = num_registers1 + num_registers2

'**************************************************************************************************************************
'******     Generate the BLE Characteristic ID's count.
'****************************************************************************************************************************
    BLE_DCI_H.WriteLine ("")
    BLE_DCI_H.WriteLine ("// ******************************************************* ")
    BLE_DCI_H.WriteLine ("// ******     The Ble characteristic Id Counts.")
    BLE_DCI_H.WriteLine ("// *******************************************************")
    BLE_DCI_H.WriteLine ("static const Ble_DCI::CHAR_ID_TD DCI_BLE_DATA_TOTAL_CHAR_ID = " & num_registers & "U;")
    BLE_DCI_H.WriteLine ("#endif")
    
    'Check whether the max number of characterisictics UUID exceeding the limit for STM32WB55
    'STM32WB55 have memory restrictions on the Gatt table and Max limit is set to 63
    If (Cells(BLE_DATA_BLE_MODULE_SELECTION_ROW, BLE_DESCRIP_COL).value = BLE_STM32WB55) Then
        If (num_registers > MAX_LIMIT_CHARACTERISTICS_UUID_STM32WB55) Then
        MsgBox "Error: Max Number of characteristics for STM32WB55 is 63. Remove atleast: " & (num_registers - MAX_LIMIT_CHARACTERISTICS_UUID_STM32WB55) & " Characteristics from the BLE Sheet"
        Exit Sub
        End If
    End If

'**************************************************************************************************************************
'******     Generate the BLE Parameters Structure.
'****************************************************************************************************************************
    BLE_DCI_C.WriteLine ("")
    BLE_DCI_C.WriteLine ("//*******************************************************")
    BLE_DCI_C.WriteLine ("//******     The Ble DCIDS Cross Reference Table.")
    BLE_DCI_C.WriteLine ("//*******************************************************")
    BLE_DCI_C.WriteLine ("")
    BLE_DCI_C.WriteLine ("const Ble_DCI::dci_ble_to_dcid_lkup_st_td_t ble_to_dcid_struct[DCI_BLE_DATA_TOTAL_CHAR_ID] =")
    BLE_DCI_C.WriteLine ("{")
    
    counter = Custom_Sevice_Row
    counter1 = BLE_DATA_START_ROW
    ble_index_ctr = 0
    While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        While (Cells(counter1, BLE_DISPLAY_TYPE_COL).text <> "Type") And (Cells(counter1, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
            If (Cells(counter1, BLE_DATA_DEFINE_COL).text <> Empty) Then
                BLE_DCI_C.WriteLine ("    {")
                BLE_DCI_C.WriteLine ("        // " & Cells(counter1, BLE_DESCRIP_COL).text)
                BLE_DCI_C.WriteLine ("        " & Cells(counter1, BLE_DATA_DEFINE_COL).text & "_BLE_CHAR_ID" & ",")
                BLE_DCI_C.WriteLine ("        " & Cells(counter1, BLE_DATA_DEFINE_COL).text & "_DCID" & ",")
                BLE_DCI_C.WriteLine ("        " & Cells(counter1, BLE_READ_PERMISSION_COL).text & "U,")
                BLE_DCI_C.WriteLine ("        " & Cells(counter1, BLE_WRITE_PERMISSION_COL).text & "U,")
                BLE_DCI_C.WriteLine ("#ifdef STM32WB55_WPAN")
                BLE_DCI_C.WriteLine ("        (")
                BLE_DCI_C.WriteLine ("            ( " & "BLE_CHAR_PROP_READ " & ")")
                If (Cells(counter1, BLE_NOTIFICATION_ATTRIB_COL).value <> Empty) Then
                BLE_DCI_C.WriteLine ("            | ( " & "BLE_CHAR_PROP_NOTIFY" & " )")
                End If
                If (Cells(counter1, BLE_WRITE_PERMISSION_COL).value > 0) Then
                    If (Cells(counter1, BLE_WRITE_WITHOUT_RESPONSE_ATTRIB_COL).value <> Empty) Then
                        BLE_DCI_C.WriteLine ("            | ( " & "BLE_CHAR_PROP_WRITE_WITHOUT_RESP" & " )")
                    Else
                        BLE_DCI_C.WriteLine ("            | ( " & "BLE_CHAR_PROP_WRITE" & " )")
                    End If
                End If
                BLE_DCI_C.WriteLine ("        ),")
                Call Print_UUIDs(counter1, BLE_DCI_C, BLE_SERVICE_UUID_COL)
                Call Print_UUIDs(counter1, BLE_DCI_C, BLE_DEVICE_CHARACTERISTIC_UUID_COL)
                BLE_DCI_C.WriteLine ("#endif")
                BLE_DCI_C.WriteLine ("    },")
            End If
            counter1 = counter1 + 1
        Wend
        If (Cells(counter, BLE_DATA_DEFINE_COL).text = Empty) Then
            counter = counter + 1
        Else
            BLE_DCI_C.WriteLine ("    {")
            BLE_DCI_C.WriteLine ("        // " & Cells(counter, BLE_DESCRIP_COL).text)
            BLE_DCI_C.WriteLine ("        " & Cells(counter, BLE_DATA_DEFINE_COL).text & "_BLE_CHAR_ID" & ",")
            BLE_DCI_C.WriteLine ("        " & Cells(counter, BLE_DATA_DEFINE_COL).text & "_DCID" & ",")
        
            If (Cells(counter, BLE_DISPLAY_TYPE_COL).text = Empty) And (Cells(counter, BLE_READ_PERMISSION_COL).text = Empty) Then
                MsgBox "Missing read access level at row : " & counter
                Exit Sub
                Else
                BLE_DCI_C.WriteLine ("        " & Cells(counter, BLE_READ_PERMISSION_COL).text & "U,")
            End If
            If (Cells(counter, BLE_DISPLAY_TYPE_COL).text = Empty) And (Cells(counter, BLE_WRITE_PERMISSION_COL).text = Empty) Then
                MsgBox "Missing write access level at row : " & counter
                Exit Sub
                Else
                BLE_DCI_C.WriteLine ("        " & Cells(counter, BLE_WRITE_PERMISSION_COL).text & "U,")
            End If
            
            BLE_DCI_C.WriteLine ("#ifdef STM32WB55_WPAN")
            BLE_DCI_C.WriteLine ("        (")
            BLE_DCI_C.WriteLine ("            ( " & "BLE_CHAR_PROP_READ " & ")")
            
            If (Cells(counter, BLE_NOTIFICATION_ATTRIB_COL).value <> Empty) Then
                BLE_DCI_C.WriteLine ("            | ( " & "BLE_CHAR_PROP_NOTIFY" & " )")
            End If
            
            If (Cells(counter, BLE_WRITE_PERMISSION_COL).value > 0) Then
                If (Cells(counter, BLE_WRITE_WITHOUT_RESPONSE_ATTRIB_COL).value <> Empty) Then
                    BLE_DCI_C.WriteLine ("            | ( " & "BLE_CHAR_PROP_WRITE_WITHOUT_RESP" & " )")
                Else
                    BLE_DCI_C.WriteLine ("            | ( " & "BLE_CHAR_PROP_WRITE" & " )")
                End If
            End If
            BLE_DCI_C.WriteLine ("        ),")
            Call Print_UUIDs(counter, BLE_DCI_C, BLE_SERVICE_UUID_COL)
            Call Print_UUIDs(counter, BLE_DCI_C, BLE_DEVICE_CHARACTERISTIC_UUID_COL)
            BLE_DCI_C.WriteLine ("#endif")
            BLE_DCI_C.WriteLine ("    },")
            counter = counter + 1
        End If
    Wend
    BLE_DCI_C.WriteLine ("};")
 
'**************************************************************************************************************************
'******     Generate the BLE Parameters Structure.
'****************************************************************************************************************************
    BLE_DCI_C.WriteLine ("")
    BLE_DCI_C.WriteLine ("// *******************************************************")
    BLE_DCI_C.WriteLine ("// ******     The BLE Target Structure.")
    BLE_DCI_C.WriteLine ("// *******************************************************")
    BLE_DCI_C.WriteLine ("")
    BLE_DCI_C.WriteLine ("const Ble_DCI::ble_target_info_st_td_t ble_dci_data_target_info =")
    BLE_DCI_C.WriteLine ("{")
    BLE_DCI_C.WriteLine ("    DCI_BLE_DATA_TOTAL_CHAR_ID,        //CHAR_ID_TD total_char_id")
    BLE_DCI_C.WriteLine ("    ble_to_dcid_struct,        //const dci_ble_to_dcid_lkup_st_td_t* ble_to_dcid_struct")
    BLE_DCI_C.WriteLine ("};")
        
'**************************************************************************************************************************
'******     Store the BLE Groups in Array.
'****************************************************************************************************************************

    Dim Groupstart_Flag As Integer
    Dim Groupzero_Flag As Integer
    Dim Group_Counter As Integer
    Dim Max_Parameters As Integer
    Dim Group_Parameters() As Integer
    Dim Group_Index As Integer
    Dim Group_Zero_Index As Integer
    
    counter = BLE_DATA_START_ROW
    Group_Counter = 0
    Max_Parameters = 0
    
    While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        If (Cells(counter, BLE_DISPLAY_TYPE_COL).text = "GroupStart") Then
            Group_Counter = Group_Counter + 1
        End If
        counter = counter + 1
    Wend

    counter = BLE_DATA_START_ROW + 1
    ReDim Group_Parameters(0 To Group_Counter) As Integer
    Group_Index = -1
    
    While (Cells(counter, BLE_DISPLAY_TYPE_COL).text <> "Type")
        If (Cells(counter, BLE_DISPLAY_TYPE_COL).text = "GroupStart") Then
            Group_Index = Group_Index + 1
            If (Group_Parameters(Group_Index) > Max_Parameters) Then
                Max_Parameters = Group_Parameters(Group_Index)
            End If
        End If
        If (Cells(counter, BLE_DATA_DEFINE_COL).text <> Empty) Then
            Group_Parameters(Group_Index) = Group_Parameters(Group_Index) + 1
        End If
        counter = counter + 1
    Wend
    
    counter = Custom_Sevice_Row
    Groupstart_Flag = 0
    Group_Zero_Index = Group_Index + 1
    Group_Index = Group_Index + 1
    
    While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        If (Cells(counter, BLE_DISPLAY_TYPE_COL).text = "GroupStart") Then
            If (Group_Parameters(Group_Index) > Max_Parameters) Then
                Max_Parameters = Group_Parameters(Group_Index)
            End If
            Groupstart_Flag = 1
            Group_Index = Group_Index + 1
        ElseIf (Cells(counter, BLE_DISPLAY_TYPE_COL).text = "GroupEnd") Then
            If (Group_Parameters(Group_Index) > Max_Parameters) Then
                Max_Parameters = Group_Parameters(Group_Index)
            End If
            Groupstart_Flag = 0
        End If
        If (Cells(counter, BLE_DATA_DEFINE_COL).text <> Empty) Then
            If (Groupstart_Flag = 1) Then
                Group_Parameters(Group_Index) = Group_Parameters(Group_Index) + 1
            ElseIf (Groupstart_Flag = 0) Then
                If (Cells(counter, BLE_DISPLAY_TYPE_COL).text = "par") Then
                    Group_Parameters(Group_Zero_Index) = Group_Parameters(Group_Zero_Index) + 1
                End If
            End If
        End If
        counter = counter + 1
    Wend

    If (Max_Parameters < Group_Parameters(Group_Zero_Index)) Then
        Max_Parameters = Group_Parameters(Group_Zero_Index)
    End If


'**************************************************************************************************************************
'******     Generate the BLE Group Cross Reference Table.
'****************************************************************************************************************************
    
    BLE_DCI_C.WriteLine ("")
    BLE_DCI_C.WriteLine ("// *******************************************************")
    BLE_DCI_C.WriteLine ("// ******     The BLE Group Cross Reference Table.")
    BLE_DCI_C.WriteLine ("// *******************************************************")
    BLE_DCI_C.WriteLine ("")
    BLE_DCI_C.WriteLine ("static const uint8_t DCI_BLE_DATA_TOTAL_GROUP_ID = " & (Group_Counter + 1) & "U;")
    BLE_DCI_C.WriteLine ("const Ble_DCI::ble_group_assembly_t ble_dci_group_struct[DCI_BLE_DATA_TOTAL_GROUP_ID] =")
    BLE_DCI_C.WriteLine ("{")

    'Skip the Generic Access Service - it should not be accessed
    counter = BLE_DATA_START_ROW + 1
    
    Groupstart_Flag = 0
    Group_Index = 0
    
    While (Cells(counter, BLE_DISPLAY_TYPE_COL).text <> "Type")
        If (Cells(counter, BLE_DISPLAY_TYPE_COL).text = "GroupStart") Then
            Groupstart_Flag = 1
            BLE_DCI_C.WriteLine ("    {")
            BLE_DCI_C.WriteLine ("        " & Group_Parameters(Group_Index) & "U,")
            BLE_DCI_C.WriteLine ("        " & Cells(counter, BLE_DCI_ID_COL).text & "U,")
            BLE_DCI_C.WriteLine ("    {")
            Group_Index = Group_Index + 1
        End If
        If (Cells(counter, BLE_DATA_DEFINE_COL).text <> Empty) Then
            BLE_DCI_C.WriteLine ("        " & Cells(counter, BLE_DATA_DEFINE_COL).text & "_BLE_CHAR_ID,")
        End If
        If (Cells(counter + 1, BLE_DISPLAY_TYPE_COL).text = "GroupEnd") Then
            BLE_DCI_C.WriteLine ("    },")
            BLE_DCI_C.WriteLine ("    },")
        End If
        counter = counter + 1
    Wend
    
    Groupstart_Flag = 0
    Groupzero_Flag = 1
    Group_Index = Group_Zero_Index + 1
    counter = Custom_Sevice_Row
    
    While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        If (Cells(counter, BLE_DISPLAY_TYPE_COL).text = "GroupStart") Then
            Groupstart_Flag = 1
            If (Groupzero_Flag = 0) Then
                BLE_DCI_C.WriteLine ("    {")
                BLE_DCI_C.WriteLine ("        " & Group_Parameters(Group_Index) & "U,")
                BLE_DCI_C.WriteLine ("        " & Cells(counter, BLE_DCI_ID_COL).text & "U,")
                BLE_DCI_C.WriteLine ("    {")
                Group_Index = Group_Index + 1
            End If
        ElseIf (Cells(counter, BLE_DISPLAY_TYPE_COL).text = "GroupEnd") Then
            Groupstart_Flag = 0
        End If
        If (Cells(counter, BLE_DATA_DEFINE_COL).text <> Empty) Then
            If (Groupstart_Flag = 0) And (Groupzero_Flag > 0) And (Cells(counter, BLE_DISPLAY_TYPE_COL).text = "par") Then
                If (Groupzero_Flag = 1) Then
                    BLE_DCI_C.WriteLine ("    {")
                    BLE_DCI_C.WriteLine ("        " & Group_Parameters(Group_Zero_Index) & "U,")
                    BLE_DCI_C.WriteLine ("        " & Cells(Custom_Sevice_Row, BLE_DCI_ID_COL).text & "U,")
                    BLE_DCI_C.WriteLine ("    {")
                    BLE_DCI_C.WriteLine ("        " & Cells(counter, BLE_DATA_DEFINE_COL).text & "_BLE_CHAR_ID,")
                    Groupzero_Flag = Groupzero_Flag + 1
                Else
                    BLE_DCI_C.WriteLine ("        " & Cells(counter, BLE_DATA_DEFINE_COL).text & "_BLE_CHAR_ID,")
                End If
            ElseIf (Groupstart_Flag = 1) And (Groupzero_Flag = 0) Then
                If (Cells(counter + 1, BLE_DATA_DEFINE_COL).text <> Empty) Then
                    BLE_DCI_C.WriteLine ("        " & Cells(counter, BLE_DATA_DEFINE_COL).text & "_BLE_CHAR_ID,")
                Else
                    BLE_DCI_C.WriteLine ("        " & Cells(counter, BLE_DATA_DEFINE_COL).text & "_BLE_CHAR_ID")
                    BLE_DCI_C.WriteLine ("    },")
                    BLE_DCI_C.WriteLine ("    },")
                    Groupstart_Flag = 0
                End If
            End If
        End If
        If ((Cells(counter + 1, BLE_DATA_DEFINE_COL).text = BLE_BEGIN_IGNORED_DATA) And (Groupzero_Flag > 0)) Then
            BLE_DCI_C.WriteLine ("    },")
            BLE_DCI_C.WriteLine ("    },")
            Groupzero_Flag = 0
            counter = Custom_Sevice_Row
        End If
        counter = counter + 1
    Wend
    BLE_DCI_C.WriteLine ("};")
'**************************************************************************************************************************
'******     Generate the BLE Group Structure.
'****************************************************************************************************************************
    BLE_DCI_C.WriteLine ("")
    BLE_DCI_C.WriteLine ("//*******************************************************")
    BLE_DCI_C.WriteLine ("//******     The BLE Group Structure.")
    BLE_DCI_C.WriteLine ("//*******************************************************")
    BLE_DCI_C.WriteLine ("")
    BLE_DCI_C.WriteLine ("const Ble_DCI::ble_group_info_st_td_t ble_dci_data_group_info =")
    BLE_DCI_C.WriteLine ("{")
    BLE_DCI_C.WriteLine ("    DCI_BLE_DATA_TOTAL_GROUP_ID,   //CHAR_ID_TD total_group_id")
    BLE_DCI_C.WriteLine ("    ble_dci_group_struct,        //const ble_group_assembly_t* ble_dci_group_struct")
    BLE_DCI_C.WriteLine ("};")
    
    
'**************************************************************************************************************************
'******     Callback function for verifying the parameters.
'****************************************************************************************************************************
    
    Call Verify_BLE_List

'XML File , JSON File, JSON Tree generation Currently only used and tested for Silicon Labs BLE Module
'Start of #Silabs Specific_Routines
If (Cells(BLE_DATA_BLE_MODULE_SELECTION_ROW, BLE_DESCRIP_COL).value = BLE_SILABS_MODULE) Then


'****************************************************************************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'***    Start Creating the .XML file header
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************************************************************************

'**************************************************************************************************************************
'******     Generate the Generic Access Service.
'****************************************************************************************************************************
    
    BLE_DCI_XML.WriteLine ("<?xml version=""1.0"" encoding=""UTF-8"" standalone=""no""?> ")
    BLE_DCI_XML.WriteLine ("<project>")
    BLE_DCI_XML.WriteLine ("")
    BLE_DCI_XML.WriteLine ("  <!--Custom BLE GATT--> ")
    BLE_DCI_XML.WriteLine ("  <gatt generic_attribute_service=""true"" header=""" & Cells(BLE_HEADER_ROW, BLE_DISPLAY_TYPE_COL).text & """ name = """ & Cells(BLE_NAME_ROW, BLE_DISPLAY_TYPE_COL).text & """ out =""" & Cells(BLE_OUT_ROW, BLE_DISPLAY_TYPE_COL).text & """ prefix = """ & Cells(BLE_PREFIX_ROW, BLE_DISPLAY_TYPE_COL).text & """>")
    BLE_DCI_XML.WriteLine ("")
    BLE_DCI_XML.WriteLine ("  <!--Generic Access Service 0x1800 --> ")
    BLE_DCI_XML.WriteLine ("  <service advertise=""false"" name=""Generic Access"" requirement=""mandatory"" sourceId=""org.bluetooth.service.generic_access"" type=""primary""" & " uuid=""" & Cells(BLE_DATA_START_ROW, BLE_SERVICE_UUID_COL).text & """" & ">")
    BLE_DCI_XML.WriteLine ("")
    BLE_DCI_XML.WriteLine ("    <!--" & Cells(BLE_DATA_START_ROW, BLE_DATA_DEFINE_COL).text & "--> ")
    BLE_DCI_XML.WriteLine ("    <characteristic id=""device_name""" & " " & "name=""" & Cells(BLE_DATA_START_ROW, BLE_DATA_DEFINE_COL).text & """" & " " & "sourceId=""org.bluetooth.characteristic.gap.device_name""" & " uuid=""" & Cells(BLE_DATA_START_ROW, BLE_DEVICE_CHARACTERISTIC_UUID_COL).text & """" & ">")
    
    
    Dim temp_default_string As String
    Dim Readflag As Integer
    Dim Writeflag As Integer
    Readflag = 1
    
    If Cells(BLE_DATA_START_ROW, BLE_DATA_DEFINE_COL).text <> Empty Then
        temp_default_string = Replace(Cells(BLE_DATA_START_ROW, BLE_DATA_DEFINE_COL).text, """", "")
    End If

    row_number = 0
    row_number = Find_DCID_Row(temp_default_string)
    
    If Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text <> Empty Then
        temp_default_string = Replace(Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text, """", "")
    End If
    
    BLE_DCI_XML.WriteLine ("      <value length =" & """" & Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_TOTAL_LENGTH_COL).text & """")
    BLE_DCI_XML.WriteLine ("       type=""" & Cells(BLE_DATA_START_ROW, BLE_TYPE_COL).text & """" & ">" & temp_default_string & "</value>")
            
    If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_RAM_VAL_WRITE_ACCESS_COL).text <> Empty) Or (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_INIT_VAL_WRITE_ACCESS_COL).text <> Empty) Then
        Writeflag = 1
    Else
        Writeflag = 0
    End If
    
    If ((Readflag = 1) And (Writeflag = 1)) Then
        BLE_DCI_XML.WriteLine ("      <properties authenticated_read=""true"" authenticated_write=""true"" /> ")
    ElseIf (Readflag = 1) Then
        BLE_DCI_XML.WriteLine ("      <properties read=""true"" /> ")
    ElseIf (Writeflag = 1) Then
        BLE_DCI_XML.WriteLine ("      <properties write=""true"" /> ")
    End If
    
    BLE_DCI_XML.WriteLine ("    </characteristic> ")
    BLE_DCI_XML.WriteLine ("")
    BLE_DCI_XML.WriteLine ("  </service> ")
    
'**************************************************************************************************************************
'******     Generate the Customized BLE service UUID details.
'**************************************************************************************************************************
    
    Dim Device_Name_Service As Integer
    Dim intResult2 As Integer
    Device_Name_Service = 1
    
    'Skip the generic access default service UUID 0x1800 , since its handled seperately above this logic
    'Generic Access service 0x1800 have different format when compare to the other customized services
    Service_UUID_Index = 1
    
    While (Service_UUID_Index < Unique_Service_UUID_Count)
        BLE_DCI_XML.WriteLine ("")
        BLE_DCI_XML.WriteLine ("    <!-- Custom Service UUID's --> ")
        intResult2 = StrComp(Cells(BLE_DEVICE_ACCESS_LEVELS_ROW, BLE_SERVICE_UUID_COL).text, CStr(Unique_ServiceUUIDs(Service_UUID_Index)))
        If (intResult2 = 0) Then
            BLE_DCI_XML.WriteLine ("        <service uuid=""" & Unique_ServiceUUIDs(Service_UUID_Index) & """" & " name= ""Custom Profile_" & (Service_UUID_Index) & " - " & (Cells((BLE_DEVICE_ACCESS_LEVELS_ROW + 1), BLE_SERVICE_UUID_COL).text) & """" & " advertise=""true"" > ")
        Else
            BLE_DCI_XML.WriteLine ("        <service uuid=""" & Unique_ServiceUUIDs(Service_UUID_Index) & """" & " name= ""Custom Profile_" & (Service_UUID_Index) & """" & " advertise=""true"" > ")
        End If
        
        BLE_DCI_XML.WriteLine ("")
        BLE_DCI_XML.WriteLine ("    <!-- Custom Service """ & Unique_ServiceUUIDs(Service_UUID_Index) & """--> ")
        BLE_DCI_XML.WriteLine ("")
        
        row_counter = BLE_DATA_START_ROW

    
        While (Cells(row_counter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
            If (Cells(row_counter, BLE_DISPLAY_TYPE_COL).text <> "Type") Then
                intResult2 = StrComp(Cells(row_counter, BLE_SERVICE_UUID_COL).text, CStr(Unique_ServiceUUIDs(Service_UUID_Index)))
                If (intResult2 = 0) Then
        '**************************************************************************************************************************
        '******      <Characteristic>
        '****************************************************************************************************************************
                If Cells(row_counter, BLE_DATA_DEFINE_COL).text <> Empty Then
                
                    If Cells(row_counter, BLE_DATA_DEFINE_COL).text <> Empty Then
                        temp_default_string = Replace(Cells(row_counter, BLE_DATA_DEFINE_COL).text, """", "")
                    End If
    
                    row_number = 0
                    row_number = Find_DCID_Row(temp_default_string)
                    Cells(row_counter, BLE_DATA_DEFINE_COL).Font.Color = RGB(0, 0, 0)
                    
                    BLE_DCI_XML.WriteLine ("          <!-- " & Cells(row_counter, BLE_DATA_DEFINE_COL).text & " -->")
                    BLE_DCI_XML.WriteLine ("          <characteristic id=" & """" & Cells(row_counter, BLE_DATA_DEFINE_COL).text & """" & " " & "name=""" & Cells(row_counter, BLE_DATA_DEFINE_COL).text & """" & " " & "uuid=""" & Cells(row_counter, BLE_DEVICE_CHARACTERISTIC_UUID_COL).text & """>")
                     
                    If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_RAM_VAL_WRITE_ACCESS_COL).text <> Empty) Or (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_INIT_VAL_WRITE_ACCESS_COL).text <> Empty) Then
                        Writeflag = 1
                    Else
                        Writeflag = 0
                    End If
    
        '**************************************************************************************************************************
        '******    <properties>
        '****************************************************************************************************************************
                    If (Worksheets("DCI Descriptors").Cells(row_counter, BLE_DATA_DEFINE_COL).text <> "DCI_BLE_DEVICE_BONDING_INFO") Then
                        If ((Readflag = 1) And (Writeflag = 1)) Then
                           BLE_DCI_XML.WriteLine ("               <properties authenticated_read=""true"" authenticated_write=""true"" /> ")
                        ElseIf (Readflag = 1) Then
                           BLE_DCI_XML.WriteLine ("               <properties authenticated_read=""true"" /> ")
                        ElseIf (Writeflag = 1) Then
                           BLE_DCI_XML.WriteLine ("               <properties authenticated_write=""true"" /> ")
                        End If
                    Else
                        If (Readflag = 1) Then
                           BLE_DCI_XML.WriteLine ("               <properties read =""true""/> ")
                        Else
                           BLE_DCI_XML.WriteLine ("               <properties """" />")
                        End If
                    End If
        '**************************************************************************************************************************
        '******    <value>
        '****************************************************************************************************************************
                    Dim DefaultValueflag As Boolean
                    Dim lengthflag As Boolean
                     
                    If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_TOTAL_LENGTH_COL).text <> Empty) Then
                        lengthflag = True
                    Else
                        lengthflag = False
                    End If
                    
                    If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text <> Empty) Then
                        DefaultValueflag = True
                    Else
                        DefaultValueflag = False
                    End If
                    
                    If Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text <> Empty Then
                        temp_default_string = Replace(Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text, """", "")
                    End If
                    
                    If (lengthflag) And (DefaultValueflag) Then
                        BLE_DCI_XML.WriteLine ("               <value" & " length = """ & Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_TOTAL_LENGTH_COL).text & """")
                    ElseIf (lengthflag) Then
                        BLE_DCI_XML.WriteLine ("               <value" & " length = """ & Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_TOTAL_LENGTH_COL).text & """")
                    ElseIf (DefaultValueflag) Then
                        BLE_DCI_XML.WriteLine ("               <value" & temp_default_string & ">" & " </value>")
                    End If
                    
                    If (Cells(row_counter, BLE_TYPE_COL).text <> Empty) Then
                        BLE_DCI_XML.WriteLine ("                type=""" & Cells(row_counter, BLE_TYPE_COL).text & """" & ">" & temp_default_string & "</value>")
                    End If

        '**************************************************************************************************************************
        '******    <description>
        '****************************************************************************************************************************
                    If (Cells(row_counter, BLE_DESCRIP_COL).text <> Empty) And (Cells(row_counter, BLE_DESCRIP_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                        BLE_DCI_XML.WriteLine ("                <description>" & Cells(row_counter, BLE_DESCRIP_COL).text & "</description>")
                    Else
                        BLE_DCI_XML.WriteLine (" ")
                    End If
                    
                        BLE_DCI_XML.WriteLine ("          </characteristic> ")
                        BLE_DCI_XML.WriteLine ("")
                    ElseIf (Cells(row_counter, BLE_MENU_PATH_COL).text <> Empty) Then
                        BLE_DCI_XML.WriteLine ("          <!-- " & Cells(row_counter, BLE_DISPLAY_NAME_COL).text & " -->")
                        BLE_DCI_XML.WriteLine ("          <characteristic id=" & """" & Cells(row_counter, BLE_MENU_PATH_COL).text & """" & " " & "name=""" & Cells(row_counter, BLE_DISPLAY_NAME_COL).text & """" & " uuid=""" & Cells(row_counter, BLE_DEVICE_CHARACTERISTIC_UUID_COL).text & """>")
                        BLE_DCI_XML.WriteLine ("              <properties authenticated_read=""true"" authenticated_write=""true"" /> ")
                        BLE_DCI_XML.WriteLine ("              <value type=""" & Cells(row_counter, BLE_TYPE_COL).text & """" & ">" & "</value> ")
                        BLE_DCI_XML.WriteLine ("          </characteristic> ")
                    End If
                End If
            End If
            row_counter = row_counter + 1
        Wend
        Service_UUID_Index = Service_UUID_Index + 1
        BLE_DCI_XML.WriteLine ("     </service> ")
    Wend
    BLE_DCI_XML.WriteLine ("")
    BLE_DCI_XML.WriteLine ("</gatt> ")
    BLE_DCI_XML.WriteLine ("</project>")
    BLE_DCI_XML.Close
    End If
    'End of #Silabs Specific_Routines
    
'****************************************************************************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'***    Start Creating the .JSON file header
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************************************************************************

'**************************************************************************************************************************
'******   BLE Customized parameter list
'****************************************************************************************************************************
        
    BLE_DCI_JSON.WriteLine ("{")
    
    ''''' Start of User '''''''
  
    Dim ValidParam As Integer
    Dim hexval As Variant
    Dim i As Integer
    Dim j As Integer
    Dim k As Integer
    Dim data As Variant
    ReDim data(0 To 32) As Variant
    Dim temp_enum_string As String
    Dim Default_Value As Variant
    Dim Group_Parameter As Integer
    Dim Char_no As Integer
    Dim Char_zero_no As Integer
    Dim total_len As Integer
    Dim total_len_zero As Integer
    
    row_value = BLE_DEVICE_ACCESS_LEVELS_ROW
    total_len = 0
    total_len_zero = 0
    OneTimeFlag = False
    
    If (START_ROW <> BLE_DATA_START_ROW) Then
    
        BLE_DCI_JSON.WriteLine (" ""User"": {")
        
        While (Cells(row_value, BLE_DEVICE_DISPALY_NAME_COL).text <> Empty)
            Access_Level = Cells(row_value, BLE_DEVICE_ACCESS_LEVEL_COL).value
            Groupstart_Flag = 0
            Char_zero_no = 0
            total_len_zero = 0
            START_ROW = Custom_Sevice_Row
        
            BLE_DCI_JSON.WriteLine ("""" & Cells(row_value, BLE_DEVICE_DISPALY_NAME_COL).text & """" & ": {")
        
            BLE_DCI_JSON.WriteLine ("")
            BLE_DCI_JSON.WriteLine ("       """ & Cells(START_ROW, BLE_MENU_PATH_COL).text & ",Main""" & ":{")
            BLE_DCI_JSON.WriteLine ("")
            BLE_DCI_JSON.WriteLine ("           """ & "GroupcharacteristicUUID" & """" & ": " & """" & Cells(START_ROW, BLE_DEVICE_CHARACTERISTIC_UUID_COL) & """" & ",")
            BLE_DCI_JSON.WriteLine ("           """ & "GroupserviceUUID" & """" & ": " & """" & Cells(START_ROW, BLE_SERVICE_UUID_COL) & """" & ",")
            BLE_DCI_JSON.WriteLine ("           ""Groupproperties"": [""read"", ""write""],")
            BLE_DCI_JSON.WriteLine ("           ""Groupteam"": ""User"",")
            BLE_DCI_JSON.WriteLine ("")
    
            While (Cells(START_ROW, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
            
            'Skip the LED Control Service since it should not be displayed in the main screeen
            'The param inside this service is have dedicated purpose and it should not be displayed
            If (Cells(START_ROW, BLE_DISPLAY_NAME_COL).text = BLE_LED_CONTROL_SERVICE) Then
                While (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text <> "CategorySecEnd")
                    START_ROW = START_ROW + 1
                Wend
                If (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "CategorySecEnd") And (Cells((START_ROW + 1), BLE_DISPLAY_TYPE_COL).text = "GroupEnd") Then
                    START_ROW = START_ROW + 2
                End If
            End If
            
            If (Cells(START_ROW, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
            
            If (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = Empty) Then
                START_ROW = START_ROW + 1
            Else
                If ((Cells(START_ROW, BLE_READ_PERMISSION_COL).value <= Access_Level) Or (Cells(START_ROW, BLE_WRITE_PERMISSION_COL).value <= Access_Level)) Then
    
                    If (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "CategorySecStart") Or (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "GroupStart") Then
                    
                        ValidParam = 0
                        value_counter = START_ROW
                        
                        Do While (Cells(value_counter, BLE_DISPLAY_TYPE_COL).text <> "CategorySecEnd")
                        
                            If (Cells(START_ROW + 2, BLE_DISPLAY_TYPE_COL).text = "CategorySecStart") Then
                                ValidParam = ValidParam + 1
                                Exit Do
                            Else
                                If ((Cells(value_counter, BLE_READ_PERMISSION_COL).value <> Empty) And (Cells(value_counter, BLE_READ_PERMISSION_COL).value <= Access_Level)) Or ((Cells(value_counter, BLE_WRITE_PERMISSION_COL).value <> Empty) And (Cells(value_counter, BLE_WRITE_PERMISSION_COL).value <= Access_Level)) Then
                                    ValidParam = ValidParam + 1
                                    Exit Do
                                End If
                                value_counter = value_counter + 1
                            End If
                        Loop
                        
                        If (Cells(value_counter, BLE_DISPLAY_TYPE_COL).text = "CategorySecEnd") Then
                        
                            START_ROW = value_counter
                        
                            If (Cells((value_counter + 1), BLE_DISPLAY_TYPE_COL).text = "GroupEnd") Then
                            
                                START_ROW = value_counter + 1
                                          
                            End If
                        
                        End If
                        
                        If (ValidParam = 0) Then
                            Groupstart_Flag = 0
                        Else
                            BLE_DCI_JSON.WriteLine ("    " & """" & Cells(START_ROW, BLE_DISPLAY_NAME_COL).text & "," & Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text & """" & ": {")
                        End If
                        
                        If (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "GroupStart") Then
                            Groupstart_Flag = 1
                            Char_no = 0
                            counter = START_ROW
                            Group_Parameter = 0
                            While (Cells(counter, BLE_DISPLAY_TYPE_COL).text <> "GroupEnd")
                                If ((Cells(counter, BLE_READ_PERMISSION_COL).value <> Empty) And (Cells(counter, BLE_READ_PERMISSION_COL).value <= Access_Level)) Or ((Cells(counter, BLE_WRITE_PERMISSION_COL).value <> Empty) And (Cells(counter, BLE_WRITE_PERMISSION_COL).value <= Access_Level)) Then
                                    Group_Parameter = Group_Parameter + 1
                                End If
                                counter = counter + 1
                            Wend
                            counter = START_ROW
                            If (Group_Parameter > 0) Then
                            BLE_DCI_JSON.WriteLine ("")
                            BLE_DCI_JSON.WriteLine ("       """ & Cells(START_ROW, BLE_MENU_PATH_COL).text & ",Main""" & ":{")
                            BLE_DCI_JSON.WriteLine ("")
                            BLE_DCI_JSON.WriteLine ("           """ & "GroupcharacteristicUUID" & """" & ": " & """" & Cells(START_ROW, BLE_DEVICE_CHARACTERISTIC_UUID_COL) & """" & ",")
                            BLE_DCI_JSON.WriteLine ("           """ & "GroupserviceUUID" & """" & ": " & """" & Cells(START_ROW, BLE_SERVICE_UUID_COL) & """" & ",")
                            BLE_DCI_JSON.WriteLine ("           ""Groupproperties"": [""read"", ""write""],")
                            BLE_DCI_JSON.WriteLine ("           ""Groupteam"": ""User"",")
                            BLE_DCI_JSON.WriteLine ("")
                            total_len = 0
                        Else
                            Groupstart_Flag = 0
                        End If
                    End If
    
                
                ElseIf (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "par") Then
                    
                    If (Cells(START_ROW, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                        temp_enum_string = Replace(Cells(START_ROW, BLE_DATA_DEFINE_COL).text, """", "")
                    End If
    
                    row_number = 0
                    row_number = Find_DCID_Row(temp_enum_string)
    
                    If (row_number = 0) Then
                        FaultCounter = FaultCounter + 1
                        Cells(START_ROW, BLE_DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
                        BLE_DCI_JSON.WriteLine ("       " & """" & Cells(START_ROW, BLE_DISPLAY_NAME_COL).text & "," & Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text & """" & ": { ")
    
                    Else
                        Cells(START_ROW, BLE_DATA_DEFINE_COL).Font.Color = RGB(0, 0, 0)
    
                        BLE_DCI_JSON.WriteLine ("       " & """" & Cells(START_ROW, BLE_DISPLAY_NAME_COL).text & "," & Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text & """" & ": { ")
    
                        If (Cells(START_ROW, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
    
                            BLE_DCI_JSON.WriteLine ("           " & """characteristicUUID"": " & """" & Cells(START_ROW, BLE_DEVICE_CHARACTERISTIC_UUID_COL).text & """" & ",")
                            If (Groupstart_Flag = 1) Then
                                BLE_DCI_JSON.WriteLine ("           " & """characteristicNo"": " & """" & Char_no & """" & ",")
                                Char_no = Char_no + 1
                            Else
                                BLE_DCI_JSON.WriteLine ("           " & """characteristicNo"": " & """" & Char_zero_no & """" & ",")
                                Char_zero_no = Char_zero_no + 1
                            End If
                            BLE_DCI_JSON.WriteLine ("           " & """ServiceUUID"": " & """" & Cells(START_ROW, BLE_SERVICE_UUID_COL).text & """" & ",")
    
                            Readflag = 1
                            If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_RAM_VAL_WRITE_ACCESS_COL).text <> Empty) Or (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_INIT_VAL_WRITE_ACCESS_COL).text <> Empty) Then
                                Writeflag = 1
                            Else
                                Writeflag = 0
                            End If
                            
                            
                            If (Writeflag = 1) Then
                                If (row_value <= BLE_DEVICE_ACCESS_LEVELS_ROW + 1) Then
                                    If ((Cells(START_ROW, BLE_READ_PERMISSION_COL).value <= Access_Level) Or (Cells(START_ROW, BLE_WRITE_PERMISSION_COL).value <= Access_Level)) Then
                                        BLE_DCI_JSON.WriteLine ("           " & """properties"": [""read""],")
                                    Else
                                        BLE_DCI_JSON.WriteLine ("           " & """properties"": [""read"",""write""],")
                                    End If
                                Else
                                    If ((Cells(START_ROW, BLE_READ_PERMISSION_COL).value <= Access_Level) Or (Cells(START_ROW, BLE_WRITE_PERMISSION_COL).value <= Access_Level)) Then
                                        If (Cells(START_ROW, BLE_DATA_DEFINE_COL).text <> "DCI_BLE_USER4_NAME") And (Cells(START_ROW, BLE_DATA_DEFINE_COL).text <> "DCI_BLE_USER4_ROLE") Then
                                            BLE_DCI_JSON.WriteLine ("           " & """properties"": [""read"",""write""],")
                                        End If
                                    Else
                                        BLE_DCI_JSON.WriteLine ("           " & """properties"": [""read""],")
                                    End If
                                End If
                            Else
                                    BLE_DCI_JSON.WriteLine ("           " & """properties"": [""read""],")
                            End If
                           
                            
                            If (Cells(START_ROW, BLE_DEVICE_UNITS_LENGTH_COL).text <> Empty) And (Cells(START_ROW, BLE_DEVICE_UNITS_LENGTH_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                BLE_DCI_JSON.WriteLine ("           " & """length"": " & Cells(START_ROW, BLE_DEVICE_UNITS_LENGTH_COL).text & ",")
                            Else
                                BLE_DCI_JSON.WriteLine ("           " & """length"": " & """" & """" & ",")
                            End If
                            
                            If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_TOTAL_LENGTH_COL).text <> Empty) Then
                                If (Groupstart_Flag = 1) Then
                                    BLE_DCI_JSON.WriteLine ("           " & """total_length"": " & """" & total_len & "-" & (total_len + Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_TOTAL_LENGTH_COL).text) & """" & ",")
                                    total_len = total_len + Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_TOTAL_LENGTH_COL).text
                                    If (total_len > MAX_PAYLOAD) Then
                                        MsgBox "Error: Buffer size of Assemblies exceeded the limit (256 bytes). Reduce the number of parameters in " & (Cells(counter, BLE_DISPLAY_NAME_COL).text)
                                        While (Cells(counter, BLE_DISPLAY_TYPE_COL).text <> "GroupEnd")
                                            Cells(counter, BLE_DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
                                            counter = counter + 1
                                        Wend
                                        Exit Sub
                                    End If
                                Else
                                    BLE_DCI_JSON.WriteLine ("           " & """total_length"": " & """" & total_len_zero & "-" & (total_len_zero + Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_TOTAL_LENGTH_COL).text) & """" & ",")
                                    total_len_zero = total_len_zero + Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_TOTAL_LENGTH_COL).text
                                    If (total_len_zero > MAX_PAYLOAD) Then
                                        counter = Custom_Sevice_Row
                                        Groupstart_Flag = 0
                                        While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
                                            If (Cells(counter, BLE_DISPLAY_TYPE_COL).text = "GroupStart") Then
                                                Groupstart_Flag = 1
                                            ElseIf (Cells(counter, BLE_DISPLAY_TYPE_COL).text = "GroupEnd") Then
                                                Groupstart_Flag = 0
                                            End If
                                            If (Cells(counter, BLE_DATA_DEFINE_COL).text <> Empty) Then
                                                If (Groupstart_Flag = 0) Then
                                                    If (Cells(counter, BLE_DISPLAY_TYPE_COL).text = "par") Then
                                                        Cells(counter, BLE_DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
                                                    End If
                                                End If
                                            End If
                                            counter = counter + 1
                                        Wend
                                        MsgBox "Error: Buffer size of Assemblies exceeded the limit (256 bytes). Reduce the number of parameters in Assembly 0"
                                        Exit Sub
                                    End If
                                End If
                            Else
                                BLE_DCI_JSON.WriteLine ("           " & """total_length"": " & """" & """" & ",")
                            End If
    
                            If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_CHARACTERISTIC_VALUE_TYPE_COL).text <> Empty) And (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_CHARACTERISTIC_VALUE_TYPE_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                
                                If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DATA_DEFINE_COL).text = "DCI_SIMPLE_APP_CONTROL_WORD") Or (Worksheets("DCI Descriptors").Cells(row_number, BLE_DATA_DEFINE_COL).text = "DCI_SIMPLE_APP_STATUS_WORD") Then
                                    BLE_DCI_JSON.WriteLine ("           " & """type"": " & """" & "WORD" & """" & ",")
                                Else
                                    BLE_DCI_JSON.WriteLine ("           " & """type"": " & """" & Get_Datatype_String(Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_CHARACTERISTIC_VALUE_TYPE_COL).text) & """" & ",")
                                End If
                            Else
                                BLE_DCI_JSON.WriteLine ("           " & """type"": " & """" & """" & ",")
                            End If
    
                            If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text <> Empty) And (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                
                                If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_CHARACTERISTIC_VALUE_TYPE_COL).text = "DCI_DTYPE_BYTE") Or (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_CHARACTERISTIC_VALUE_TYPE_COL).text = "DCI_DTYPE_WORD") Or (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_CHARACTERISTIC_VALUE_TYPE_COL).text = "DCI_DTYPE_DWORD") Then
                                                                  
                                    hexval = Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).value
                                    hexval = Split(hexval, "x")(1)
                                                                    
                                    BLE_DCI_JSON.WriteLine ("           " & """defaultValue"": " & Application.WorksheetFunction.Hex2Dec(hexval) & ",")
                                
                                ElseIf (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_SHORT_NAME_COL).text = "MAC_ADDRESS") Or (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_SHORT_NAME_COL).text = "IP") Then
                                
                                    BLE_DCI_JSON.Write ("   ")
                                    Default_Value = Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text
                                    Hex_To_Dec (Default_Value)
                                ElseIf (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_CHARACTERISTIC_VALUE_TYPE_COL).text = "DCI_DTYPE_BOOL") Then
                                    BLE_DCI_JSON.WriteLine ("           " & """defaultValue"": """ & Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text & """,")
                                Else
                                    BLE_DCI_JSON.WriteLine ("           " & """defaultValue"": " & Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text & ",")
                                End If
                            Else
                                BLE_DCI_JSON.WriteLine ("           " & """defaultValue"": " & ",")
                            End If
    
                            If ((Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_MIN_VALUE_COL).text <> Empty) And (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_MIN_VALUE_COL).text <> BLE_BEGIN_IGNORED_DATA)) And ((Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_MAX_VALUE_COL).text <> Empty) And (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_MAX_VALUE_COL).text <> BLE_BEGIN_IGNORED_DATA)) Then
                                BLE_DCI_JSON.WriteLine ("           " & """range"":" & "[" & Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_MIN_VALUE_COL).text & "," & Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_MAX_VALUE_COL).text & "],")
                            Else
                                BLE_DCI_JSON.WriteLine ("           " & """range"":" & "[" & "],")
                            End If
                            
                            If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_ENUM_SUPPORT_COL).text <> Empty) Then
                                Enum_flag = 1
                            Else
                                Enum_flag = 0
                            End If
                            
                            If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DATA_DEFINE_COL).text = "DCI_SIMPLE_APP_CONTROL_WORD") Then
                                    BLE_DCI_JSON.WriteLine ("           " & """enum"":[""LED 1"",""LED 2"",""LED 3"",""LED 4""],")
                            ElseIf (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DATA_DEFINE_COL).text = "DCI_SIMPLE_APP_STATUS_WORD") Then
                                    BLE_DCI_JSON.WriteLine ("           " & """enum"":[""SWITCH 1"",""SWITCH 2"",""SWITCH 3"",""SWITCH 4""],")
                              
                            ElseIf (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_ENUM_DETAILS_COL).text <> Empty) And (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_ENUM_DETAILS_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                    If (Enum_flag = 1) Then
                                        temp = Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_ENUM_DETAILS_COL).text
                                        length = Left(temp, 2)
                                    If (Right(length, 1) = ";") Then
                                        length = Split(length, ";")(0)
                                        length = length * 2
                                    Else
                                        length = length * 2
                                    End If
                                        
                                        k = 0
                                        j = 0
                                        For i = 1 To length Step 2
                                        data_string = Split(temp, ",")(i)
                                        data = Split(data_string, ";")
                                        If k = 0 Then
                                            k = 1
                                            BLE_DCI_JSON.WriteLine ("           " & """enum"":" & "[" & """" & data(j) & """" & ",")
                                        Else
                                            BLE_DCI_JSON.WriteLine ("                   " & """" & data(j) & """" & ",")
                                        End If
                                        Next i
                                        BLE_DCI_JSON.WriteLine ("           " & "],")
                                    End If
                                                      
                            ElseIf (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_BIT_FIELD_COL).text <> Empty) And (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_BIT_FIELD_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                            
                                temp = Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_BIT_FIELD_COL).text
                                length = Left(temp, 2)
                                
                                If (Right(length, 1) = ";") Then
                                    length = Split(length, ";")(0)
                                    length = length * 2
                                Else
                                    length = length * 2
                                End If
                                
                                k = 0
                                j = 0
                                                            
                                For i = 1 To length Step 2
                                    data_string = Split(temp, ",")(i)
                                    data = Split(data_string, ";")
                                    If k = 0 Then
                                        k = 1
                                        BLE_DCI_JSON.WriteLine ("           " & """enum"":" & "[" & """" & data(j) & """" & ",")
                                    Else
                                        BLE_DCI_JSON.WriteLine ("                   " & """" & data(j) & """" & ",")
                                    End If
                                Next i
                                BLE_DCI_JSON.WriteLine ("           " & "],")
                                
                            Else
                                BLE_DCI_JSON.WriteLine ("           " & """enum"":" & "[" & "],")
                            End If
                            
                            If (Cells(START_ROW, BLE_PARAM_LONG_DESC_COL).text <> Empty) And (Cells(START_ROW, BLE_PARAM_LONG_DESC_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                BLE_DCI_JSON.WriteLine ("           " & """description"": " & """" & Cells(START_ROW, BLE_PARAM_LONG_DESC_COL).text & """" & ",")
                            Else
                                BLE_DCI_JSON.WriteLine ("           " & """description"": " & """" & """" & ",")
                            End If
                            
                            If (Cells(START_ROW, BLE_DEVICE_FORMAT_COL).text <> Empty) And (Cells(START_ROW, BLE_DEVICE_FORMAT_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                BLE_DCI_JSON.WriteLine ("           " & """display"": " & """" & Cells(START_ROW, BLE_DEVICE_FORMAT_COL).text & """")
                            Else
                                BLE_DCI_JSON.WriteLine ("           " & """display"": " & """" & """")
                            End If
                            
                            BLE_DCI_JSON.WriteLine ("       },")
    
                        End If
    
                    End If
    
                ElseIf (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "CategorySecEnd") Then
                     BLE_DCI_JSON.WriteLine ("       },")
                     Groupstart_Flag = 0
                ElseIf (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "GroupEnd") Then
                     BLE_DCI_JSON.WriteLine ("       },")
                     BLE_DCI_JSON.WriteLine ("       },")
                     Groupstart_Flag = 0
                End If
    
                START_ROW = START_ROW + 1
    
            Else
    
                START_ROW = START_ROW + 1
    
            End If
        End If
    
        End If
        Wend
        
        BLE_DCI_JSON.WriteLine ("    },")
        BLE_DCI_JSON.WriteLine ("    },")
        row_value = row_value + 1
    Wend
        
    BLE_DCI_JSON.WriteLine ("    },")
    ''''' End of User'''''
    End If
    
    
    ''''' Start of LED Control '''''''
    
    'Find the LED Control service starting Row
    ble_index_ctr = BLE_DATA_START_ROW
    START_ROW = BLE_DATA_START_ROW
    OneTimeFlag = False
    While (Cells(ble_index_ctr, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        If (Cells(ble_index_ctr, BLE_DISPLAY_NAME_COL).text = BLE_LED_CONTROL_SERVICE) Then
            START_ROW = ble_index_ctr
        End If
        ble_index_ctr = ble_index_ctr + 1
    Wend
    
    If (START_ROW <> BLE_DATA_START_ROW) Then
        BLE_DCI_JSON.WriteLine ("""" & Cells(START_ROW, BLE_DISPLAY_NAME_COL).text & """" & ": {")
        If (Cells((START_ROW + 1), BLE_DISPLAY_TYPE_COL).text = "CategorySecStart") And (Cells((START_ROW + 2), BLE_DISPLAY_TYPE_COL).text = "par") Then
        BLE_DCI_JSON.WriteLine ("""" & Cells((START_ROW + 2), BLE_SERVICE_UUID_COL).text & """" & ": {")
        expected_service_UUID = Cells((START_ROW + 2), BLE_SERVICE_UUID_COL).text
        Char_no = 0
        total_len = 0
        
        While (Cells(START_ROW, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
            
            If (OneTimeFlag = False) Then
            
                If (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "GroupStart") Then
                    BLE_DCI_JSON.WriteLine ("")
                    BLE_DCI_JSON.WriteLine ("    """ & Cells(START_ROW, BLE_MENU_PATH_COL).text & ",Main""" & ":{")
                    BLE_DCI_JSON.WriteLine ("")
                    BLE_DCI_JSON.WriteLine ("        """ & "GroupcharacteristicUUID" & """" & ": " & """" & Cells(START_ROW, BLE_DEVICE_CHARACTERISTIC_UUID_COL) & """" & ",")
                    BLE_DCI_JSON.WriteLine ("        """ & "GroupserviceUUID" & """" & ": " & """" & Cells(START_ROW, BLE_SERVICE_UUID_COL) & """" & ",")
                    BLE_DCI_JSON.WriteLine ("        ""Groupproperties"": [""read"", ""write""],")
                    BLE_DCI_JSON.WriteLine ("        ""Groupteam"": ""LED Control"",")
                    BLE_DCI_JSON.WriteLine ("")
                    total_len = 0
                    counter = START_ROW
                End If
                
                If (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "CategorySecStart") Then
                    BLE_DCI_JSON.WriteLine ("       "",CategorySecStart"": {")
                End If
                
                If (Cells(START_ROW, BLE_DATA_DEFINE_COL).text <> Empty) Then
                    temp_enum_string = Replace(Cells(START_ROW, BLE_DATA_DEFINE_COL).text, """", "")
                End If
                
                row_number = 0
                row_number = Find_DCID_Row(temp_enum_string)
        
                If (row_number = 0) Then
                    FaultCounter = FaultCounter + 1
        
                    Cells(START_ROW, BLE_DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
                Else
        
                    Cells(START_ROW, BLE_DATA_DEFINE_COL).Font.Color = RGB(0, 0, 0)
                    
                    If (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "CategorySecEnd") Then
                        OneTimeFlag = True
                    ElseIf (Cells(START_ROW, BLE_DATA_DEFINE_COL).text <> Empty) Then
                    'Below check is added to ensure that LED Control assembly/groups have same service UUID's
                        If (Cells(START_ROW, BLE_SERVICE_UUID_COL).text = expected_service_UUID) Then
                            Cells(START_ROW, BLE_SERVICE_UUID_COL).Font.Color = RGB(0, 0, 0)
                            BLE_DCI_JSON.WriteLine ("    " & """" & Cells(START_ROW, BLE_DEVICE_DISPALY_NAME_COL).text & """" & ": { ")
                            BLE_DCI_JSON.WriteLine ("        " & """characteristicUUID"": " & """" & Cells(START_ROW, BLE_DEVICE_CHARACTERISTIC_UUID_COL).text & """" & ",")
                            BLE_DCI_JSON.WriteLine ("        " & """characteristicNo"": " & """" & Char_no & """" & ",")
                            Char_no = Char_no + 1
                            BLE_DCI_JSON.WriteLine ("        " & """serviceUUID"": " & """" & Cells(START_ROW, BLE_SERVICE_UUID_COL).text & """" & ",")
                            
                            If (Cells(START_ROW, BLE_DEVICE_UNITS_LENGTH_COL).text <> Empty) And (Cells(START_ROW, BLE_DEVICE_UNITS_LENGTH_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                BLE_DCI_JSON.WriteLine ("        " & """length"": " & Cells(START_ROW, BLE_DEVICE_UNITS_LENGTH_COL).text & ",")
                            Else
                                BLE_DCI_JSON.WriteLine ("        " & """length"": " & """" & """" & ",")
                            End If
                            
                            If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_TOTAL_LENGTH_COL).text <> Empty) Then
                                BLE_DCI_JSON.WriteLine ("        " & """total_length"": " & """" & total_len & "-" & (total_len + Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_TOTAL_LENGTH_COL).text) & """" & ",")
                                total_len = total_len + Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_TOTAL_LENGTH_COL).text
                                If (total_len > MAX_PAYLOAD) Then
                                    MsgBox "Error: Buffer size of Assemblies exceeded the limit (256 bytes). Reduce the number of parameters in " & (Cells(counter, BLE_DISPLAY_NAME_COL).text)
                                    While (Cells(counter, BLE_DISPLAY_TYPE_COL).text <> "GroupEnd")
                                        Cells(counter, BLE_DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
                                        counter = counter + 1
                                    Wend
                                    Exit Sub
                                End If
                            Else
                                BLE_DCI_JSON.WriteLine ("        " & """total_length"": " & """" & """" & ",")
                            End If
                            
                            If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_RAM_VAL_WRITE_ACCESS_COL).text <> Empty) Or (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_INIT_VAL_WRITE_ACCESS_COL).text <> Empty) Then
                                Writeflag = 1
                            Else
                                Writeflag = 0
                            End If
            
                            If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_CHARACTERISTIC_VALUE_TYPE_COL).text <> Empty) And (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_CHARACTERISTIC_VALUE_TYPE_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                BLE_DCI_JSON.WriteLine ("        " & """type"": " & """" & Get_Datatype_String(Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_CHARACTERISTIC_VALUE_TYPE_COL).text) & """" & ",")
                            Else
                                BLE_DCI_JSON.WriteLine ("        " & """type"": " & """" & """" & ",")
                            End If
            
                            If ((Readflag = 1) And (Writeflag = 1)) Then
                                BLE_DCI_JSON.WriteLine ("        " & """properties"": [""read"",""write""],")
                            ElseIf (Readflag = 1) Then
                                BLE_DCI_JSON.WriteLine ("        " & """properties"": [""read""],")
                            ElseIf (Writeflag = 1) Then
                                BLE_DCI_JSON.WriteLine ("           " & """properties"": [""write""],")
                            End If
            
                            If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text <> Empty) And (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_SHORT_NAME_COL).text = "Complete Adverisement Packet ") Or (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_SHORT_NAME_COL).text = "iBeacon 128 bit uuid") Or (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_SHORT_NAME_COL).text = "iBeacon Major Number") Or (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_SHORT_NAME_COL).text = "iBeacon Minor Number") Then
                                    Default_Value = Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text
                                    Hex_To_Dec (Default_Value)
                                ElseIf (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_CHARACTERISTIC_VALUE_TYPE_COL).text = "DCI_DTYPE_BOOL") Then
                                            BLE_DCI_JSON.WriteLine ("           " & """defaultValue"": """ & Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text & """,")
                                Else
                                    BLE_DCI_JSON.WriteLine ("        " & """defaultValue"": " & Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text & ",")
                                End If
                            Else
                                BLE_DCI_JSON.WriteLine ("        " & """defaultValue"": " & ",")
                            End If
            
                            If ((Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_MIN_VALUE_COL).text <> Empty) And (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_MIN_VALUE_COL).text <> BLE_BEGIN_IGNORED_DATA)) And ((Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_MAX_VALUE_COL).text <> Empty) And (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_MAX_VALUE_COL).text <> BLE_BEGIN_IGNORED_DATA)) Then
                                BLE_DCI_JSON.WriteLine ("        " & """range"":" & "[" & Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_MIN_VALUE_COL).text & "," & Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_MAX_VALUE_COL).text & "],")
                            Else
                                BLE_DCI_JSON.WriteLine ("        " & """range"":" & "[" & "],")
                            End If
            
                            If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_ENUM_DETAILS_COL).text <> Empty) And (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_ENUM_DETAILS_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                BLE_DCI_JSON.WriteLine ("        " & """enum"":" & "[" & Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_ENUM_DETAILS_COL).text & "],")
                            Else
                                BLE_DCI_JSON.WriteLine ("        " & """enum"":" & "[" & "],")
                            End If
                            
                            If (Cells(START_ROW, BLE_DEVICE_FORMAT_COL).text <> Empty) And (Cells(START_ROW, BLE_DEVICE_FORMAT_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                BLE_DCI_JSON.WriteLine ("        " & """display"": " & """" & Cells(START_ROW, BLE_DEVICE_FORMAT_COL).text & """" & ",")
                            Else
                                BLE_DCI_JSON.WriteLine ("        " & """display"": " & """" & """" & ",")
                            End If
                            
                            If (Cells(START_ROW, BLE_PARAM_LONG_DESC_COL).text <> Empty) And (Cells(START_ROW, BLE_PARAM_LONG_DESC_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                BLE_DCI_JSON.WriteLine ("        " & """description"": " & """" & Cells(START_ROW, BLE_PARAM_LONG_DESC_COL).text & """" & ",")
                            Else
                                BLE_DCI_JSON.WriteLine ("        " & """description"": " & """" & """" & ",")
                            End If
                            
                            If (Cells(START_ROW, BLE_DISPLAY_NAME_COL).text <> Empty) And (Cells(START_ROW, BLE_DISPLAY_NAME_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                BLE_DCI_JSON.WriteLine ("        " & """Role_name"": " & """" & Cells(START_ROW, BLE_DISPLAY_NAME_COL).text & """")
                            Else
                                BLE_DCI_JSON.WriteLine ("        " & """Role_name"": " & """" & """")
                            End If
                             
                            BLE_DCI_JSON.WriteLine ("       },")
                    
                        Else
                            Cells(START_ROW, BLE_SERVICE_UUID_COL).Font.Color = RGB(255, 0, 0)
                            MsgBox "Error: All parameters should have same service UUID"
                            Exit Sub
                        End If
                             
                    End If
                    
                End If
                
            End If
            START_ROW = START_ROW + 1
    
        Wend
        
        BLE_DCI_JSON.WriteLine ("    },")
        BLE_DCI_JSON.WriteLine ("    },")
        BLE_DCI_JSON.WriteLine ("    },")
        BLE_DCI_JSON.WriteLine ("    },")
        End If
    End If

    ''''' Start of Device Details '''''''
    
    'Find the Device Profile service starting Row
    ble_index_ctr = BLE_DATA_START_ROW
    START_ROW = BLE_DATA_START_ROW
    
    While (Cells(ble_index_ctr, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        If (Cells(ble_index_ctr, BLE_DISPLAY_NAME_COL).text = BLE_DEVICE_PROFILE_SERVICE) Then
            START_ROW = ble_index_ctr
        End If
        ble_index_ctr = ble_index_ctr + 1
    Wend
    
    If (START_ROW <> BLE_DATA_START_ROW) Then
        BLE_DCI_JSON.WriteLine ("""" & Cells(START_ROW, BLE_DISPLAY_NAME_COL).text & """" & ": {")
        If (Cells((START_ROW + 1), BLE_DISPLAY_TYPE_COL).text = "CategorySecStart") And (Cells((START_ROW + 2), BLE_DISPLAY_TYPE_COL).text = "par") Then
            BLE_DCI_JSON.WriteLine ("""" & Cells((START_ROW + 2), BLE_SERVICE_UUID_COL).text & """" & ": {")
            expected_service_UUID = Cells((START_ROW + 2), BLE_SERVICE_UUID_COL).text
            Char_no = 0
            total_length = 0
    
            While (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text <> "Type")
                condition_check = False
                If (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "GroupStart") Then
                    BLE_DCI_JSON.WriteLine ("")
                    BLE_DCI_JSON.WriteLine ("    """ & Cells(START_ROW, BLE_MENU_PATH_COL).text & ",Main""" & ":{")
                    BLE_DCI_JSON.WriteLine ("")
                    BLE_DCI_JSON.WriteLine ("        """ & "GroupcharacteristicUUID" & """" & ": " & """" & Cells(START_ROW, BLE_DEVICE_CHARACTERISTIC_UUID_COL) & """" & ",")
                    BLE_DCI_JSON.WriteLine ("        """ & "GroupserviceUUID" & """" & ": " & """" & Cells(START_ROW, BLE_SERVICE_UUID_COL) & """" & ",")
                    BLE_DCI_JSON.WriteLine ("        ""Groupproperties"": [""read"", ""write""],")
                    BLE_DCI_JSON.WriteLine ("        ""Groupteam"": ""Device Details"",")
                    BLE_DCI_JSON.WriteLine ("")
                    Char_no = 0
                    total_len = 0
                    counter = START_ROW
                End If
                
                If (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "CategorySecStart") Then
                    BLE_DCI_JSON.WriteLine ("       "",CategorySecStart"": {")
                End If
                
                If (Cells(START_ROW, BLE_DATA_DEFINE_COL).text <> Empty) Then
                    temp_enum_string = Replace(Cells(START_ROW, BLE_DATA_DEFINE_COL).text, """", "")
                End If
                
                
                row_number = 0
                row_number = Find_DCID_Row(temp_enum_string)
        
                If (row_number = 0) Then
                    FaultCounter = FaultCounter + 1
        
                    Cells(START_ROW, BLE_DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
                Else
        
                    Cells(START_ROW, BLE_DATA_DEFINE_COL).Font.Color = RGB(0, 0, 0)
        
                    If (Cells(START_ROW, BLE_DATA_DEFINE_COL).text <> Empty) Then
                        
                        'Validating the service UUID for the Device profile whether all BLE parameters are having the same service UUID
                        If (Cells(START_ROW, BLE_SERVICE_UUID_COL).text = expected_service_UUID And Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "par") Then
                        
                            Cells(START_ROW, BLE_SERVICE_UUID_COL).Font.Color = RGB(0, 0, 0)
                            BLE_DCI_JSON.WriteLine ("    " & """" & Cells(START_ROW, BLE_DEVICE_DISPALY_NAME_COL).text & """" & ": { ")
                            BLE_DCI_JSON.WriteLine ("        " & """characteristicUUID"": " & """" & Cells(START_ROW, BLE_DEVICE_CHARACTERISTIC_UUID_COL).text & """" & ",")
                            BLE_DCI_JSON.WriteLine ("        " & """characteristicNo"": " & """" & Char_no & """" & ",")
                            Char_no = Char_no + 1
                            BLE_DCI_JSON.WriteLine ("        " & """serviceUUID"": " & """" & Cells(START_ROW, BLE_SERVICE_UUID_COL).text & """" & ",")
                            
                            If (Cells(START_ROW, BLE_DEVICE_UNITS_LENGTH_COL).text <> Empty) And (Cells(START_ROW, BLE_DEVICE_UNITS_LENGTH_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                BLE_DCI_JSON.WriteLine ("        " & """length"": " & Cells(START_ROW, BLE_DEVICE_UNITS_LENGTH_COL).text & ",")
                            Else
                                BLE_DCI_JSON.WriteLine ("        " & """length"": " & """" & """" & ",")
                            End If
                            
                            If ((Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_TOTAL_LENGTH_COL).text <> Empty) And (Cells(START_ROW, BLE_DATA_DEFINE_COL).text <> "DCI_BLE_USERNAME")) Then
                                BLE_DCI_JSON.WriteLine ("        " & """total_length"": " & """" & total_len & "-" & (total_len + Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_TOTAL_LENGTH_COL).text) & """" & ",")
                                total_len = total_len + Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_TOTAL_LENGTH_COL).text
                                If (total_len > MAX_PAYLOAD) Then
                                    MsgBox "Error: Buffer size of Assemblies exceeded the limit (256 bytes). Reduce the number of parameters in " & (Cells(counter, BLE_DISPLAY_NAME_COL).text)
                                    While (Cells(counter, BLE_DISPLAY_TYPE_COL).text <> "GroupEnd")
                                        Cells(counter, BLE_DATA_DEFINE_COL).Font.Color = RGB(255, 0, 0)
                                        counter = counter + 1
                                    Wend
                                    Exit Sub
                                End If
                            Else
                                BLE_DCI_JSON.WriteLine ("        " & """total_length"": " & """" & """" & ",")
                            End If
                            
                            If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_RAM_VAL_WRITE_ACCESS_COL).text <> Empty) Or (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_INIT_VAL_WRITE_ACCESS_COL).text <> Empty) Then
                                Writeflag = 1
                            Else
                                Writeflag = 0
                            End If
            
                            If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_CHARACTERISTIC_VALUE_TYPE_COL).text <> Empty) And (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_CHARACTERISTIC_VALUE_TYPE_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                BLE_DCI_JSON.WriteLine ("        " & """type"": " & """" & Get_Datatype_String(Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_CHARACTERISTIC_VALUE_TYPE_COL).text) & """" & ",")
                            Else
                                BLE_DCI_JSON.WriteLine ("        " & """type"": " & """" & """" & ",")
                            End If
            
                            If ((Readflag = 1) And (Writeflag = 1)) Then
                                BLE_DCI_JSON.WriteLine ("        " & """properties"": [""read"",""write""],")
                            ElseIf (Readflag = 1) Then
                                BLE_DCI_JSON.WriteLine ("        " & """properties"": [""read""],")
                            ElseIf (Writeflag = 1) Then
                                BLE_DCI_JSON.WriteLine ("           " & """properties"": [""write""],")
                            End If
            
                            If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text <> Empty) And (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_SHORT_NAME_COL).text = "Complete Adverisement Packet ") Or (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_SHORT_NAME_COL).text = "iBeacon 128 bit uuid") Or (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_SHORT_NAME_COL).text = "iBeacon Major Number") Or (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_SHORT_NAME_COL).text = "iBeacon Minor Number") Then
                                    Default_Value = Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text
                                    Hex_To_Dec (Default_Value)
                                ElseIf (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_CHARACTERISTIC_VALUE_TYPE_COL).text = "DCI_DTYPE_BOOL") Then
                                            BLE_DCI_JSON.WriteLine ("           " & """defaultValue"": """ & Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text & """,")
                                Else
                                    BLE_DCI_JSON.WriteLine ("        " & """defaultValue"": " & Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_DEFAULT_COL).text & ",")
                                End If
                            Else
                                BLE_DCI_JSON.WriteLine ("        " & """defaultValue"": " & ",")
                            End If
            
                            If ((Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_MIN_VALUE_COL).text <> Empty) And (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_MIN_VALUE_COL).text <> BLE_BEGIN_IGNORED_DATA)) And ((Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_MAX_VALUE_COL).text <> Empty) And (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_MAX_VALUE_COL).text <> BLE_BEGIN_IGNORED_DATA)) Then
                                BLE_DCI_JSON.WriteLine ("        " & """range"":" & "[" & Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_MIN_VALUE_COL).text & "," & Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_MAX_VALUE_COL).text & "],")
                            Else
                                BLE_DCI_JSON.WriteLine ("        " & """range"":" & "[" & "],")
                            End If
            
                            If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_ENUM_DETAILS_COL).text <> Empty) And (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_ENUM_DETAILS_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                BLE_DCI_JSON.WriteLine ("        " & """enum"":" & "[" & Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_ENUM_DETAILS_COL).text & "],")
                            Else
                                BLE_DCI_JSON.WriteLine ("        " & """enum"":" & "[" & "],")
                            End If
                            
                            If (Cells(START_ROW, BLE_DEVICE_FORMAT_COL).text <> Empty) And (Cells(START_ROW, BLE_DEVICE_FORMAT_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                BLE_DCI_JSON.WriteLine ("        " & """display"": " & """" & Cells(START_ROW, BLE_DEVICE_FORMAT_COL).text & """" & ",")
                            Else
                                BLE_DCI_JSON.WriteLine ("        " & """display"": " & """" & """" & ",")
                            End If
                            
                            If (Cells(START_ROW, BLE_PARAM_LONG_DESC_COL).text <> Empty) And (Cells(START_ROW, BLE_PARAM_LONG_DESC_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                BLE_DCI_JSON.WriteLine ("        " & """description"": " & """" & Cells(START_ROW, BLE_PARAM_LONG_DESC_COL).text & """" & ",")
                            Else
                                BLE_DCI_JSON.WriteLine ("        " & """description"": " & """" & """" & ",")
                            End If
                            
                            If (Cells(START_ROW, BLE_DISPLAY_NAME_COL).text <> Empty) And (Cells(START_ROW, BLE_DISPLAY_NAME_COL).text <> BLE_BEGIN_IGNORED_DATA) Then
                                BLE_DCI_JSON.WriteLine ("        " & """Role_name"": " & """" & Cells(START_ROW, BLE_DISPLAY_NAME_COL).text & """")
                            Else
                                BLE_DCI_JSON.WriteLine ("        " & """Role_name"": " & """" & """")
                            End If
            
                            BLE_DCI_JSON.WriteLine ("       },")
                            
                        Else
                            Cells(START_ROW, BLE_SERVICE_UUID_COL).Font.Color = RGB(255, 0, 0)
                            MsgBox "Error: All parameters should have same service UUID"
                            Exit Sub
                        End If
            
                    End If
            
                End If
                If ((Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "GroupEnd") Or (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "CategorySecEnd")) Then
                    BLE_DCI_JSON.WriteLine ("       },")
                End If
        
                START_ROW = START_ROW + 1
        
            Wend
              
            BLE_DCI_JSON.WriteLine ("    },")
            BLE_DCI_JSON.WriteLine ("    },")
        
        End If
    End If
    ''''' End of Device Details'''''
    
    BLE_DCI_JSON.WriteLine ("}")
    
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
    
    START_ROW = Custom_Sevice_Row
    Dim count As Integer
    count = 0
    
    While (Cells(START_ROW, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
    
        If (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = Empty) Then
            START_ROW = START_ROW + 1
        Else
            If (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "CategorySecStart") Then
                If (Cells(START_ROW, BLE_DISPLAY_NAME_COL).text <> Empty) Then
                    BLE_DCI_Json_Tree.WriteLine (Space(count) & "--" & Cells(START_ROW, BLE_DISPLAY_NAME_COL).text & "--")
                End If
            ElseIf (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "GroupStart") Then
                If (Cells(START_ROW, BLE_DISPLAY_NAME_COL).text <> Empty) Then
                    BLE_DCI_Json_Tree.WriteLine (Space(count) & Cells(START_ROW, BLE_DISPLAY_NAME_COL).text & Chr(172))
                    If (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "GroupStart") Then
                        count = count + 4
                    End If
                End If
            ElseIf (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "par") Then
                BLE_DCI_Json_Tree.WriteLine (Space(count) & Cells(START_ROW, BLE_DISPLAY_NAME_COL).text)
            ElseIf (Cells(START_ROW, BLE_DISPLAY_TYPE_COL).text = "GroupEnd") Then
                count = count - 4
            End If
        START_ROW = START_ROW + 1
        End If
    
    Wend
    BLE_DCI_JSON.Close
    BLE_DCI_Json_Tree.Close
    BLE_DCI_C.Close
    BLE_DCI_H.Close
  
End Sub


Public Function Get_Datatype_String(my_s As String) As String

    If (my_s = "DCI_DTYPE_STRING8") Then
        Get_Datatype_String = "STRING"
    Else
        Get_Datatype_String = Split(my_s, "_")(UBound(Split(my_s, "_")))
    End If
    
End Function

Public Function Hex_To_Dec(ByRef Default_Value As Variant) As Integer

Dim Default_Index As Long
Dim Default_Packet(0 To 20) As Variant

BLE_DCI_JSON.Write ("        " & """defaultValue"": " & "[")
For Default_Index = 0 To UBound(Split(Default_Value, ",")) Step 1
    Default_Packet(Default_Index) = Split(Default_Value, ",")(Default_Index)
    Default_Packet(Default_Index) = Right(Default_Packet(Default_Index), 2)
    Default_Packet(Default_Index) = Application.WorksheetFunction.Hex2Dec(Default_Packet(Default_Index))
    If (Default_Index = UBound(Split(Default_Value, ","))) Then
        BLE_DCI_JSON.Write (Default_Packet(Default_Index))
    Else
        BLE_DCI_JSON.Write (Default_Packet(Default_Index) & ",")
    End If
Next Default_Index
BLE_DCI_JSON.WriteLine ("],")

End Function

Public Sub Verify_BLE_List()

'*********************
'******     Constants
'*********************
    Application.Calculation = xlCalculationManual
    Call Ble_Restore_Cell_Color
    
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
    ble_row_ctr = BLE_DATA_START_ROW
    dci_end_row = Last_DCI_Row()
    While Cells(ble_row_ctr, BLE_DATA_DEFINE_COL).text <> Empty
        lookup_string = Cells(ble_row_ctr, BLE_DATA_DEFINE_COL).text
        If lookup_string <> Empty Then
            Set c = Find_DCID_Cell(lookup_string)
                If c Is Nothing Then
                    Cells(ble_row_ctr, BLE_DATA_DEFINE_COL).Interior.Color = RGB(255, 0, 0)
                    Cells(ble_row_ctr, BLE_DESCRIP_COL).Interior.Color = RGB(255, 0, 0)
                Else    ' If found then we go through and fix the descriptions.
                    Cells(ble_row_ctr, BLE_DESCRIP_COL).value = c.offset(0, DCI_DESCRIPTORS_COL - DCI_DEFINES_COL).text
                    
                End If
            
        End If
        ble_row_ctr = ble_row_ctr + 1
    Wend

' This below lines description column check shall cover only Customized BLE Service
 ble_row_ctr = Custom_Sevice_Row
    dci_end_row = Last_DCI_Row()
    While Cells(ble_row_ctr, BLE_DISPLAY_TYPE_COL).text <> Empty
        lookup_string = Cells(ble_row_ctr, BLE_DATA_DEFINE_COL).text
        If lookup_string <> Empty Then
            Set c = Find_DCID_Cell(lookup_string)
                If c Is Nothing Then
                    Cells(ble_row_ctr, BLE_DATA_DEFINE_COL).Interior.Color = RGB(255, 0, 0)
                    Cells(ble_row_ctr, BLE_DESCRIP_COL).Interior.Color = RGB(255, 0, 0)
                Else    ' If found then we go through and fix the descriptions.
                    Cells(ble_row_ctr, BLE_DESCRIP_COL).value = c.offset(0, DCI_DESCRIPTORS_COL - DCI_DEFINES_COL).text
                    
                End If
            
        End If
        ble_row_ctr = ble_row_ctr + 1
    Wend
   
End Sub


'****************************************************************************************************************************
'
'   Restores interior color of cells to normal.
'
'****************************************************************************************************************************

Sub Ble_Restore_Cell_Color()
    Dim row As Long
    row = BLE_DATA_START_ROW
    While (Cells(row, BLE_DATA_DEFINE_COL).text <> BLE_BEGIN_IGNORED_DATA)
        Rows(row).Interior.ColorIndex = 0
        row = row + 1
    Wend
End Sub


'****************************************************************************************************************************
'   Print Service and characterstic UUIDs in the DCI Data structure.
'****************************************************************************************************************************
Sub Print_UUIDs(counter1, BLE_DCI_C, UUID_COL)
    If (Len(Cells(counter1, UUID_COL).text) = 4) Then
        BLE_DCI_C.WriteLine ("        { 0x" & Mid(Cells(counter1, UUID_COL).text, 1, 2) & ", " & _
                                       "0x" & Mid(Cells(counter1, UUID_COL).text, 3, 2) & " },")
    Else
        BLE_DCI_C.WriteLine ("        { 0x" & Mid(Cells(counter1, UUID_COL).text, 1, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 3, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 5, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 7, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 10, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 12, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 15, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 17, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 20, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 22, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 25, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 27, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 29, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 31, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 33, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 35, 2) & " },")

    End If
End Sub
'****************************************************************************************************************************
'   Fill the Unique service UUID array by searching the BLE Sheet.
'****************************************************************************************************************************

Public Sub Fill_ServiceUUIDs()

    Dim Loop_Counter_1 As Integer
    Dim Loop_Counter_2 As Integer
    Dim Loop_Counter_3 As Integer
    Dim Loop_Counter_4 As Integer
    Dim Total_Valid_Rows As Integer
    Dim Service_UUID_Index As Integer
    Dim Group_Service_UUID As Integer
    Dim Sort_Index As Integer
    Sheets("BLE").Select

    counter = BLE_DATA_START_ROW
    Total_Valid_Rows = 0
    Unique_Service_UUID_Count = 0
    Service_UUID_Index = 0
    
    'Count the valid number of rows
    While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, BLE_SERVICE_UUID_COL).text <> Empty) Then
            Total_Valid_Rows = Total_Valid_Rows + 1
        End If
        counter = counter + 1
    Wend
    
    ReDim Unique_ServiceUUIDs(0 To Total_Valid_Rows) As Integer

    
    'Fill the ServiceUUID's available for each BLE parameters
    counter = BLE_DATA_START_ROW
    While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, BLE_SERVICE_UUID_COL).text <> Empty) Then
            Unique_ServiceUUIDs(Service_UUID_Index) = Cells(counter, BLE_SERVICE_UUID_COL).text
            Unique_Service_UUID_Count = Service_UUID_Index + 1
            Service_UUID_Index = Service_UUID_Index + 1
        End If
        counter = counter + 1
    Wend
    
    'Remove the redundant service UUID's and keep the unique service UUID's for further file generations
    Loop_Counter_4 = 0
    While (Loop_Counter_4 < Total_Valid_Rows)
        Loop_Counter_1 = 0
        Loop_Counter_2 = 0
        Loop_Counter_3 = 0
        While (Loop_Counter_1 < Unique_Service_UUID_Count)
                Loop_Counter_2 = Loop_Counter_1 + 1
                While (Loop_Counter_2 < Unique_Service_UUID_Count)
                    If (Unique_ServiceUUIDs(Loop_Counter_1) = Unique_ServiceUUIDs(Loop_Counter_2)) Then
                        Loop_Counter_3 = Loop_Counter_2
                        While (Loop_Counter_3 < Unique_Service_UUID_Count)
                            Unique_ServiceUUIDs(Loop_Counter_3) = Unique_ServiceUUIDs(Loop_Counter_3 + 1)
                            Loop_Counter_3 = Loop_Counter_3 + 1
                        Wend
                        Unique_Service_UUID_Count = Unique_Service_UUID_Count - 1
                    End If
                    Loop_Counter_2 = Loop_Counter_2 + 1
                Wend
            Loop_Counter_1 = Loop_Counter_1 + 1
        Wend
        Loop_Counter_4 = Loop_Counter_4 + 1
    Wend
    
    'Moving Group Assembly Service at the last of the array
    'While generating the BLE Characteristics ID for Silabs and ESP32 Ble Module Gatt table should be serialized.
    Dim Swap_Value As Integer
    Swap_Value = 0
    Sort_Index = 0
    Group_Service_UUID = Cells(BLE_DEVICE_ACCESS_LEVELS_ROW, BLE_SERVICE_UUID_COL).text
    While (Sort_Index < Unique_Service_UUID_Count)
        If (Unique_ServiceUUIDs(Sort_Index) = Group_Service_UUID) Then
            While (Sort_Index < Unique_Service_UUID_Count - 1)
                Swap_Value = Unique_ServiceUUIDs(Sort_Index + 1)
                Unique_ServiceUUIDs(Sort_Index) = Swap_Value
                Sort_Index = Sort_Index + 1
            Wend
        End If
        Sort_Index = Sort_Index + 1
    Wend
    Unique_ServiceUUIDs(Unique_Service_UUID_Count - 1) = Group_Service_UUID
    
    
End Sub

'****************************************************************************************************************************
'   Print ESP32 related structures and details.
'****************************************************************************************************************************

Sub Esp32_Specific(BLE_DCI_H)

    BLE_DCI_H.WriteLine ("")
    BLE_DCI_H.WriteLine ("#include ""Ble_Defines.h""")
    BLE_DCI_H.WriteLine ("")
    BLE_DCI_H.WriteLine ("// *******************************************************")
    BLE_DCI_H.WriteLine ("// ******     Standard Function UUID.")
    BLE_DCI_H.WriteLine ("// *******************************************************")
    BLE_DCI_H.WriteLine ("")
    BLE_DCI_H.WriteLine ("#define TOTAL_SERVICE " & Unique_Service_UUID_Count)
    Service_UUID_Index = 0
    While (Service_UUID_Index < Unique_Service_UUID_Count)
        BLE_DCI_H.WriteLine ("#define SERVICE_" & Service_UUID_Index + 1 & "_APP_ID " & Service_UUID_Index)
        Service_UUID_Index = Service_UUID_Index + 1
    Wend
    BLE_DCI_H.WriteLine ("")
    BLE_DCI_H.WriteLine ("// *******************************************************")
    BLE_DCI_H.WriteLine ("// ******     Variables and constants.")
    BLE_DCI_H.WriteLine ("// *******************************************************")
    BLE_DCI_H.WriteLine ("")
    BLE_DCI_H.WriteLine (" static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE; ")
    BLE_DCI_H.WriteLine (" static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE; ")
    BLE_DCI_H.WriteLine (" static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;")
    BLE_DCI_H.WriteLine (" static const uint8_t char_prop_notify = ESP_GATT_CHAR_PROP_BIT_NOTIFY; ")
    BLE_DCI_H.WriteLine (" static const uint8_t char_prop_read = ESP_GATT_CHAR_PROP_BIT_READ; ")
    BLE_DCI_H.WriteLine (" static const uint8_t char_prop_read_write = ESP_GATT_CHAR_PROP_BIT_WRITE |  ESP_GATT_CHAR_PROP_BIT_READ; ")
    BLE_DCI_H.WriteLine ("")
    BLE_DCI_H.WriteLine ("///< UUID : Services and Characteristics UUID ")
    BLE_DCI_H.WriteLine ("")
    BLE_DCI_H.WriteLine ("/* Services and Characteristics UUID variables ")
    BLE_DCI_H.WriteLine (" LSB <--------------------------------------------------------------------------------> MSB */")
    BLE_DCI_H.WriteLine ("")

    Dim Characteristic_Count As Integer
    Dim StrCompResult As Integer
    Characteristic_Count = 0
    loop_check = 0
    
    ReDim Characteristics_UUIDs(0 To Unique_Service_UUID_Count) As Integer
    Service_UUID_Index = 1
    Characteristics_UUIDs(0) = 1
    
    'Prints Service and Characteristic UUID for BLE_DEVICE_NAME - Standard default service
    BLE_DCI_H.WriteLine ("///< GATTS_DEVICE_UUID " & Cells(BLE_DATA_START_ROW, BLE_DATA_DEFINE_COL).text)
    BLE_DCI_H.WriteLine ("static const uint8_t serv1_svc_uuid[ BYTES_UUID_16BIT ] = "): Call Print_HUUIDs(BLE_DATA_START_ROW, BLE_DCI_H, BLE_SERVICE_UUID_COL)
    BLE_DCI_H.WriteLine ("///< GATTS_SERV1_CHAR1_VAL_UUID 0x2A00 ")
    BLE_DCI_H.WriteLine ("static const uint8_t serv1_char1_val_uuid[ BYTES_UUID_16BIT ] = "): Call Print_HUUIDs(BLE_DATA_START_ROW, BLE_DCI_H, BLE_DEVICE_CHARACTERISTIC_UUID_COL)

    'Prints UUIDS for each services and characteristics
    While (Service_UUID_Index < Unique_Service_UUID_Count)
        loop_check = 0
        counter = BLE_DATA_START_ROW
        While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            StrCompResult = StrComp(Cells(counter, BLE_SERVICE_UUID_COL).text, CStr(Unique_ServiceUUIDs(Service_UUID_Index)))
            If ((StrCompResult = 0) And (loop_check = 0)) Then
                    BLE_DCI_H.WriteLine ("static const uint8_t serv" & Service_UUID_Index + 1 & "_svc_uuid[BYTES_UUID_16BIT] = "): Call Print_HUUIDs(counter, BLE_DCI_H, BLE_SERVICE_UUID_COL)
                    loop_check = 1
            End If
            Characteristic_Count = 0
            counter = counter + 1
            If (loop_check = 1) Then
                counter = BLE_DATA_START_ROW
                BLE_DCI_H.WriteLine ("///< GATTS_SERV" & Service_UUID_Index + 1 & "_CHAR_UUID ( " & CStr(Unique_ServiceUUIDs(Service_UUID_Index)) & " ) ")
                While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
                    StrCompResult = StrComp(Cells(counter, BLE_SERVICE_UUID_COL).text, CStr(Unique_ServiceUUIDs(Service_UUID_Index)))
                    If (((Cells(counter, BLE_DISPLAY_TYPE_COL).text = "par") Or (Cells(counter, BLE_SERVICE_UUID_COL).text = Cells(BLE_DEVICE_ACCESS_LEVELS_ROW, BLE_SERVICE_UUID_COL).text)) And (StrCompResult = 0)) Then
                            Characteristic_Count = Characteristic_Count + 1
                            BLE_DCI_H.WriteLine ("static const uint8_t serv" & Service_UUID_Index + 1 & "_char" & Characteristic_Count & "_val_uuid[BYTES_UUID_128BIT] = "): Call Print_HUUIDs(counter, BLE_DCI_H, BLE_DEVICE_CHARACTERISTIC_UUID_COL)
                        
                    End If
                    counter = counter + 1
                Wend
            Characteristics_UUIDs(Service_UUID_Index) = Characteristic_Count
            Service_UUID_Index = Service_UUID_Index + 1
            End If

        Wend
            
    Wend
    BLE_DCI_H.WriteLine ("")
    BLE_DCI_H.WriteLine ("static uint8_t sec_service_uuid[ BYTES_UUID_128BIT ] = {")
    BLE_DCI_H.WriteLine ("      /// first uuid, 16bit, [12],[13] is the value")
    BLE_DCI_H.WriteLine ("      0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, serv1_svc_uuid[0], serv1_svc_uuid[1], 0x00,")
    BLE_DCI_H.WriteLine ("      0x00,")
    BLE_DCI_H.WriteLine ("};")
    BLE_DCI_H.WriteLine ("///< Define Total Number of Attributes in each Services ")
    Service_UUID_Index = 0
    ' Generates Total Attributes for each Services
    While (Service_UUID_Index < Unique_Service_UUID_Count)
        BLE_DCI_H.WriteLine ("#define TOTAL_ATTRIB_OF_SERV" & Service_UUID_Index + 1 & "             " & (1 + Characteristics_UUIDs(Service_UUID_Index) * 2))
        Service_UUID_Index = Service_UUID_Index + 1
    Wend
    
    BLE_DCI_H.WriteLine ("")
    BLE_DCI_H.WriteLine ("/**")
    BLE_DCI_H.WriteLine (" * @brief GATTS attr database table structure")
    BLE_DCI_H.WriteLine (" */")
    BLE_DCI_H.WriteLine ("typedef struct")
    BLE_DCI_H.WriteLine ("{")
    BLE_DCI_H.WriteLine ("    uint8_t total_attrib;")
    BLE_DCI_H.WriteLine ("    esp_gatts_attr_db_t attrib_tbl[ MAX_GATTS_ATTIRIBUTE ]; ")
    BLE_DCI_H.WriteLine ("} esp_gatts_attrib_table_t;")
    BLE_DCI_H.WriteLine ("")
    BLE_DCI_H.WriteLine ("const esp_gatts_attrib_table_t esp_gatts_attrib_db[ TOTAL_SERVICE ] =")
    BLE_DCI_H.WriteLine ("{")

    Dim temp_default_string As String
    Dim row_number As Integer
    
   'Generates GATTS attribute database table structure
    Service_UUID_Index = 0
    While (Service_UUID_Index < Unique_Service_UUID_Count)
        loop_check = 0
        counter = BLE_DATA_START_ROW
        While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            StrCompResult = StrComp(Cells(counter, BLE_SERVICE_UUID_COL).text, CStr(Unique_ServiceUUIDs(Service_UUID_Index)))
            'Generates structure for Service UUIDs for each services
            If ((StrCompResult = 0) And (loop_check = 0)) Then
                    loop_check = 1
                    BLE_DCI_H.WriteLine ("   {")
                    BLE_DCI_H.WriteLine ("      TOTAL_ATTRIB_OF_SERV" & (Service_UUID_Index + 1) & "," & "           /// Total number of attributes included in service " & (1 + Characteristics_UUIDs(Service_UUID_Index) * 2))
                    BLE_DCI_H.WriteLine ("       {")
                    BLE_DCI_H.WriteLine ("           {")
                    BLE_DCI_H.WriteLine ("               { ESP_GATT_RSP_BY_APP },")
                    BLE_DCI_H.WriteLine ("               {")
                    BLE_DCI_H.WriteLine ("                  ESP_UUID_LEN_16,")
                    BLE_DCI_H.WriteLine ("                  ( uint8_t * )&primary_service_uuid,")
                    BLE_DCI_H.WriteLine ("                  ESP_GATT_PERM_READ,")
                    BLE_DCI_H.WriteLine ("                  sizeof( uint16_t ),")
                    BLE_DCI_H.WriteLine ("                  sizeof( serv" & (Service_UUID_Index + 1) & "_svc_uuid ),")
                    BLE_DCI_H.WriteLine ("                  ( uint8_t * )&serv" & (Service_UUID_Index + 1) & "_svc_uuid ")
                    BLE_DCI_H.WriteLine ("                }")
                    BLE_DCI_H.WriteLine ("           },")
                    BLE_DCI_H.WriteLine ("")
            End If
            Characteristic_Count = 0
            counter = counter + 1
            
            If (loop_check = 1) Then
                counter = BLE_DATA_START_ROW
                While (Cells(counter, BLE_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
                    StrCompResult = StrComp(Cells(counter, BLE_SERVICE_UUID_COL).text, CStr(Unique_ServiceUUIDs(Service_UUID_Index)))
                    'Generates structure for each Characteristic's Declaration and Value of each services
                    If (((Cells(counter, BLE_DISPLAY_TYPE_COL).text = "par") Or (Cells(counter, BLE_SERVICE_UUID_COL).text = Cells(BLE_DEVICE_ACCESS_LEVELS_ROW, BLE_SERVICE_UUID_COL).text) Or (Cells(BLE_DATA_START_ROW, BLE_SERVICE_UUID_COL).text = GENERIC_ACCESS_SERVICE)) And (StrCompResult = 0)) Then
                            Characteristic_Count = Characteristic_Count + 1
                            BLE_DCI_H.WriteLine ("            /* Service " & CStr(Unique_ServiceUUIDs(Service_UUID_Index)) & " Characteristic " & Cells(counter, BLE_DATA_DEFINE_COL).text)
                            BLE_DCI_H.WriteLine ("               Service " & Service_UUID_Index + 1 & " Characteristic " & Characteristic_Count & " Declaration */")
                            
                            BLE_DCI_H.WriteLine ("           {")
                            BLE_DCI_H.WriteLine ("               { ESP_GATT_RSP_BY_APP },")
                            BLE_DCI_H.WriteLine ("               {")
                            BLE_DCI_H.WriteLine ("                   ESP_UUID_LEN_16,")
                            BLE_DCI_H.WriteLine ("                   ( uint8_t * )&character_declaration_uuid,")
                            If (Cells(counter, BLE_DATA_DEFINE_COL).text <> Empty) Then
                                    If Cells(counter, BLE_DATA_DEFINE_COL).text <> Empty Then
                                    temp_default_string = Replace(Cells(counter, BLE_DATA_DEFINE_COL).text, """", "")
                                    End If
                                    row_number = 0
                                    row_number = Find_DCID_Row(temp_default_string)
                                    If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_RAM_VAL_WRITE_ACCESS_COL).text <> Empty) Or (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_INIT_VAL_WRITE_ACCESS_COL).text <> Empty) Then
                                        BLE_DCI_H.WriteLine ("                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,")
                                    Else
                                        BLE_DCI_H.WriteLine ("                   ESP_GATT_PERM_READ,")
                                    End If
                            Else
                                    BLE_DCI_H.WriteLine ("                   ESP_GATT_PERM_READ,")
                            End If
                            BLE_DCI_H.WriteLine ("                   CHAR_DECLARATION_SIZE,")
                            BLE_DCI_H.WriteLine ("                   CHAR_DECLARATION_SIZE,")
                            If (Cells(counter, BLE_DATA_DEFINE_COL).text <> Empty) Then
                                    If Cells(counter, BLE_DATA_DEFINE_COL).text <> Empty Then
                                    temp_default_string = Replace(Cells(counter, BLE_DATA_DEFINE_COL).text, """", "")
                                    End If
                                    row_number = 0
                                    row_number = Find_DCID_Row(temp_default_string)
                                    If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_RAM_VAL_WRITE_ACCESS_COL).text <> Empty) Or (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_INIT_VAL_WRITE_ACCESS_COL).text <> Empty) Then
                                        BLE_DCI_H.WriteLine ("                   ( uint8_t * )&char_prop_read_write")
                                    Else
                                        BLE_DCI_H.WriteLine ("                   ( uint8_t * )&char_prop_read")
                                    End If
                            Else
                                    BLE_DCI_H.WriteLine ("                   ( uint8_t * )&char_prop_read")
                            End If
                            
                            BLE_DCI_H.WriteLine ("               }")
                            BLE_DCI_H.WriteLine ("           },")
                            BLE_DCI_H.WriteLine ("")
                            BLE_DCI_H.WriteLine ("            /* Service " & CStr(Unique_ServiceUUIDs(Service_UUID_Index)) & " Characteristic " & Cells(counter, BLE_DATA_DEFINE_COL).text)
                            BLE_DCI_H.WriteLine ("               Service " & (Service_UUID_Index + 1) & " Characteristic " & Characteristic_Count & " Value */")
                            BLE_DCI_H.WriteLine ("           {")
                            BLE_DCI_H.WriteLine ("               { ESP_GATT_RSP_BY_APP },")
                            BLE_DCI_H.WriteLine ("               {")
                            BLE_DCI_H.WriteLine ("                   ESP_UUID_LEN_128,")
                            BLE_DCI_H.WriteLine ("                   ( uint8_t * )&serv" & (Service_UUID_Index + 1) & "_char" & Characteristic_Count & "_val_uuid,")
                            If (Cells(counter, BLE_DATA_DEFINE_COL).text <> Empty) Then
                                    If Cells(counter, BLE_DATA_DEFINE_COL).text <> Empty Then
                                    temp_default_string = Replace(Cells(counter, BLE_DATA_DEFINE_COL).text, """", "")
                                    End If
                                    row_number = 0
                                    row_number = Find_DCID_Row(temp_default_string)
                                    If (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_RAM_VAL_WRITE_ACCESS_COL).text <> Empty) Or (Worksheets("DCI Descriptors").Cells(row_number, BLE_DCI_INIT_VAL_WRITE_ACCESS_COL).text <> Empty) Then
                                        BLE_DCI_H.WriteLine ("                   ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,")
                                    Else
                                        BLE_DCI_H.WriteLine ("                   ESP_GATT_PERM_READ,")
                                    End If
                            Else
                                    BLE_DCI_H.WriteLine ("                   ESP_GATT_PERM_READ,")
                            End If
                            BLE_DCI_H.WriteLine ("                   0,")
                            BLE_DCI_H.WriteLine ("                   0,")
                            BLE_DCI_H.WriteLine ("                   NULL")
                            BLE_DCI_H.WriteLine ("               }")
                            BLE_DCI_H.WriteLine ("           },")
                                            
                    End If
                    counter = counter + 1
                Wend

            End If

        Wend
        BLE_DCI_H.WriteLine ("       }")
        BLE_DCI_H.WriteLine ("   },")
        Service_UUID_Index = Service_UUID_Index + 1
    Wend
        BLE_DCI_H.WriteLine ("};")

End Sub


'****************************************************************************************************************************
'   Print Service and characterstic UUIDs in the DCI Data structure (reverse format).
'****************************************************************************************************************************
Sub Print_HUUIDs(counter1, BLE_DCI_H, UUID_COL)
    If (Len(Cells(counter1, UUID_COL).text) = 4) Then
        BLE_DCI_H.WriteLine ("        { 0x" & Mid(Cells(counter1, UUID_COL).text, 3, 2) & ", " & _
                                       "0x" & Mid(Cells(counter1, UUID_COL).text, 1, 2) & " };")
    Else
        BLE_DCI_H.WriteLine ("{ 0x" & Mid(Cells(counter1, UUID_COL).text, 35, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 33, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 31, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 29, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 27, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 25, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 22, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 20, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 17, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 15, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 12, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 10, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 7, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 5, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 3, 2) & ", " & _
                                        "0x" & Mid(Cells(counter1, UUID_COL).text, 1, 2) & " };")

    End If
End Sub




