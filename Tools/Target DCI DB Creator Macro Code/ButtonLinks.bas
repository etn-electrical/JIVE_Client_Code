Attribute VB_Name = "ButtonLinks"
'*************************************************************
'*************************************************************
'*** Gets the filename of the DCI_DB_Macro workbook.
'*************************************************************
'*************************************************************
Function Get_DCI_DB_Macro_Filename()
    Get_DCI_DB_Macro_Filename = Worksheets("DCI Descriptors").Range("A1").Cells(2, 8).text
End Function

'*************************************************************
'*************************************************************
'*** Gets the fully qualified pathname of the DCI_DB_Macro workbook.
'*************************************************************
'*************************************************************
Function Get_DCI_DB_Macro_Pathname()
    file_path = ActiveWorkbook.path & Worksheets("DCI Descriptors").Range("A1").Cells(1, 8).text
    Get_DCI_DB_Macro_Pathname = file_path & Get_DCI_DB_Macro_Filename()
End Function

'*************************************************************
'*************************************************************
'*** Runs the given macro from the DCI_DB_Macro workbook.
'*************************************************************
'*************************************************************
Sub Run_DCI_DB_Macro(name As String)
    macro_name = "'" & Get_DCI_DB_Macro_Filename() & "'!" & name
    Application.Run macro_name
End Sub

'*************************************************************
'*************************************************************
'*** Runs the given macro from the DCI_DB_Macro workbook, with a boolean argument.
'*************************************************************
'*************************************************************
Sub Run_DCI_DB_Macro_Bool(name As String, arg As Boolean)
    macro_name = "'" & Get_DCI_DB_Macro_Filename() & "'!" & name
    Application.Run macro_name, arg
End Sub

'*************************************************************
'*************************************************************
'*** DCI Descriptors Sheet Button Links
'*************************************************************
'*************************************************************
Sub DCI_Descriptors_CreateDCI()
    
    Call DCI_Descriptors_Open_DCI_Macros
    
    Run_DCI_DB_Macro ("Gen_DCI_Enum_Code")
    Run_DCI_DB_Macro ("Create_DCI_Structs")
    
    Sheets("DCI Descriptors").Select
End Sub

Sub DCI_Descriptors_Close_DCI_Macros()
    On Error Resume Next
    
    file_name = Get_DCI_DB_Macro_Filename()
    
    Set work_book = Workbooks(file_name)
    
    If work_book <> Empty Then
        Workbooks(file_name).Close
    End If

End Sub

Sub DCI_Descriptors_Open_DCI_Macros()
    On Error Resume Next
    Dim our_workbook As String
    
    our_workbook = ActiveWorkbook.name
    
    file_name = Get_DCI_DB_Macro_Filename()
    
    Set work_book = Workbooks(file_name)
    
    If work_book Is Nothing Then
        file_path = Get_DCI_DB_Macro_Pathname()
        Workbooks.Open (file_path)
    End If
    Workbooks(our_workbook).Activate
    
    Application.Wait (Now + TimeValue("0:00:01"))
End Sub

Sub DCI_Descriptors_Bitfields()
    Call DCI_Descriptors_Open_DCI_Macros
    
    If ActiveCell.row <= 8 Then
        MsgBox "Please select a valid DCID cell first", vbCritical, "Invalid Selection"
    ElseIf ActiveSheet.Cells(ActiveCell.row, "F").value = "" Then
        MsgBox "The selected DCI Parameter has an invalid Total Length", vbCritical, "Invalid Length"
    Else
        Run_DCI_DB_Macro ("BitFieldsFormShow")
    End If
End Sub

Sub DCI_Descriptors_Enums()
    Call DCI_Descriptors_Open_DCI_Macros
    
    If ActiveCell.row <= 8 Then
        MsgBox "Please select a valid DCID cell first", vbCritical, "Invalid Selection"
    ElseIf ActiveSheet.Cells(ActiveCell.row, "F").value = "" Then
        MsgBox "The selected DCI Parameter has an invalid Total Length", vbCritical, "Invalid Length"
    Else
        Run_DCI_DB_Macro ("EnumFormShow")
    End If
End Sub

'*************************************************************
'*************************************************************
'*** NV Memory Button Links
'*************************************************************
'*************************************************************
Sub NV_MEM_Generate()
    Call DCI_Descriptors_Open_DCI_Macros
    
    Run_DCI_DB_Macro ("Generate_Dec_Hex_Address")
    Run_DCI_DB_Macro ("Create_NV_Address_File")
    Run_DCI_DB_Macro ("Create_NV_Defaults_File")
    
End Sub

Sub NV_MEM_Verify()
    Call DCI_Descriptors_Open_DCI_Macros
    
    Run_DCI_DB_Macro ("Verify_NV_List")
End Sub

'*************************************************************
'*************************************************************
'*** REST Services Button Links
'*************************************************************
'*************************************************************
Sub REST_Create_DCI()
    Dim macro_name As String
    
    Call DCI_Descriptors_Open_DCI_Macros
    
    macro_name = Worksheets("DCI Descriptors").Range("A1").Cells(2, 8).text
    macro_name = "'" & macro_name & "'!Create_REST_DCI_Xfer"
    Application.Run macro_name
'    Application.Run "'DCI DB Creator Macros.xls'!Create_REST_DCI_Xfer"
End Sub

Sub CreateProfiBusDCI_Click()
    Dim macro_name As String
    Call DCI_Descriptors_Open_DCI_Macros
    
    macro_name = Worksheets("DCI Descriptors").Range("A1").Cells(2, 8).text
    macro_name = "'" & macro_name & "'!Create_ProfiBus_DCI_Xfer"
    Application.Run macro_name
End Sub
Sub Create_Prm_Data_Click()
    Dim macro_name As String
    Call DCI_Descriptors_Open_DCI_Macros
    
    macro_name = Worksheets("DCI Descriptors").Range("A1").Cells(2, 8).text
    macro_name = "'" & macro_name & "'!Create_Device_Profibus_Files"
    Application.Run macro_name
End Sub

Sub Create_DataEX_Module_Information()
    Dim macro_name As String
    Call DCI_Descriptors_Open_DCI_Macros
    
    macro_name = Worksheets("DCI Descriptors").Range("A1").Cells(2, 8).text
    macro_name = "'" & macro_name & "'!Create_Module_Info_Files"
    Application.Run macro_name
End Sub

Sub REST_Verify_DCI()
    Dim macro_name As String
    
    Call DCI_Descriptors_Open_DCI_Macros
'    Application.Run "'DCI DB Creator Macros.xls'!Create_REST_DCI_Xfer"
End Sub

'*************************************************************
'*************************************************************
'*** Javascript Button Link
'*************************************************************
'*************************************************************
Sub Javascript_Create_DCI()
    Call Create_JS_DCI
    Call Create_Modbus_Map
End Sub


'*************************************************************
'*************************************************************
'*** EtherNet/IP Button Links.
'*************************************************************
'*************************************************************
Sub EtherNetIP_Create_DCI()
    Call DCI_Descriptors_Open_DCI_Macros
    
    Run_DCI_DB_Macro ("Create_EtherNetIP_DCI_Xfer")
End Sub

Sub EtherNetIP_Verify_DCI()
    Call DCI_Descriptors_Open_DCI_Macros
    
    Run_DCI_DB_Macro ("Verify_EtherNetIP_List")
End Sub

Sub EtherNetIP_Create_Docs()
    Call DCI_Descriptors_Open_DCI_Macros
    
    Call Run_DCI_DB_Macro_Bool("Create_EIP_Documentation", False)
    Call Run_DCI_DB_Macro_Bool("Create_EIP_Documentation", True)
End Sub

Sub EtherNetIP_DynamicAssembly_Create_DCI()
    Call DCI_Descriptors_Open_DCI_Macros
    
    Run_DCI_DB_Macro ("Create_EtherNetIP_DynamicAssembly_DCI_Xfer")
End Sub


'*************************************************************
'*************************************************************
'*** Modbus Button Links.
'*************************************************************
'*************************************************************
Sub Modbus_Create_DCI_Btn()
    Call DCI_Descriptors_Open_DCI_Macros
    
    Run_DCI_DB_Macro ("Create_Modbus_DCI_Xfer")
End Sub

Sub Modbus_Verify_Data_Items_Btn()
    Call DCI_Descriptors_Open_DCI_Macros
    
    Run_DCI_DB_Macro ("Verify_Modbus_List")
End Sub

Sub Modbus_Create_Docs_Btn()
    Call DCI_Descriptors_Open_DCI_Macros
    
    Call Run_DCI_DB_Macro_Bool("Create_Modbus_Documentation", False)
    Call Run_DCI_DB_Macro_Bool("Create_Modbus_Documentation", True)
End Sub
Sub Create_PROFIBUS_GSD_Files_Btn()

    Dim macro_name As String
    Call DCI_Descriptors_Open_DCI_Macros
    
    macro_name = Worksheets("DCI Descriptors").Range("A1").Cells(2, 8).text
    macro_name = "'" & macro_name & "'!Create_Profibus_GSD_File"
    Application.Run macro_name
End Sub
Sub Create_Profibus_Acyclic_Tables()

    Dim macro_name As String
    Call DCI_Descriptors_Open_DCI_Macros
    
    macro_name = Worksheets("DCI Descriptors").Range("A1").Cells(2, 8).text
    macro_name = "'" & macro_name & "'!Create_Profibus_Acyclic_LTables"
    Application.Run macro_name
End Sub

Sub Configure_Profibus_Modules_button()

    Dim macro_name As String
    Call DCI_Descriptors_Open_DCI_Macros
    If Worksheets("ProfiBus(IO Modules)").Range("A1").Cells(2, 2).value = "Custom Assignment" Then
    macro_name = Worksheets("DCI Descriptors").Range("A1").Cells(2, 8).text
    macro_name = "'" & macro_name & "'!Configure_Profibus_Modules"
    Application.Run macro_name
    Else
    MsgBox "Please select Module Configuration option as <Custom Assignment> first."
    End If
End Sub

'*************************************************************
'*************************************************************
'*** BLE Button Links.
'*************************************************************
'*************************************************************

Sub Ble_Create_DCI_Btn()
    Dim macro_name As String
    
    Call DCI_Descriptors_Open_DCI_Macros
    
    macro_name = Worksheets("DCI Descriptors").Range("A1").Cells(2, 8).text
    macro_name = "'" & macro_name & "'!Create_Ble_DCI_Xfer"
    Application.Run macro_name
    
   ' Run_DCI_DB_Macro ("Create_Ble_DCI_Xfer")
End Sub

'*************************************************************
'*************************************************************
'*** BACnet Definition Button Links.
'*************************************************************
'*************************************************************

Sub BACnet_Definition_Create()
    Dim macro_name As String
    
    Call DCI_Descriptors_Open_DCI_Macros
    
    macro_name = Worksheets("DCI Descriptors").Range("A1").Cells(2, 8).text
    macro_name = "'" & macro_name & "'!BACnet_Definition"
    Application.Run macro_name
End Sub

'*************************************************************
'*************************************************************
'*** SeedUI Button Links.
'*************************************************************
'*************************************************************

Sub Seed_UI_Macro()
    Dim macro_name As String
    
    Call DCI_Descriptors_Open_DCI_Macros
    
    macro_name = Worksheets("DCI Descriptors").Range("A1").Cells(2, 8).text
    macro_name = "'" & macro_name & "'!Create_Web_UI"
    Application.Run macro_name
End Sub


'*************************************************************
'*************************************************************
'*** IOT Button Links.
'*************************************************************
'*************************************************************

Sub IOT_Macro()
    Dim macro_name As String
    
    Call DCI_Descriptors_Open_DCI_Macros
    
    macro_name = Worksheets("DCI Descriptors").Range("A1").Cells(2, 8).text
    macro_name = "'" & macro_name & "'!Create_IOT_DCI_Xfer()"
    Application.Run macro_name
End Sub

'*************************************************************
'*************************************************************
'*** J1939_MSG Definition Button Links.
'*************************************************************
'*************************************************************

Sub J1939_MSG_Definition_Create()
    Dim macro_name As String
    
    Call DCI_Descriptors_Open_DCI_Macros
    
    macro_name = Worksheets("DCI Descriptors").Range("A1").Cells(2, 8).text
    macro_name = "'" & macro_name & "'!J1939_MSG_Definition"
    Application.Run macro_name
End Sub


Sub J1939_MSG_Definition_Verify()
    Call DCI_Descriptors_Open_DCI_Macros
    
    Run_DCI_DB_Macro ("Verify_J1939_MSG")
End Sub

'*************************************************************
'*************************************************************
'*** SeedUI 2.0 Button Links.
'*************************************************************
'*************************************************************
Sub Seed_UI_2_Macro()
    Dim macro_name As String
    
    Call DCI_Descriptors_Open_DCI_Macros
    
    macro_name = Worksheets("DCI Descriptors").Range("A1").Cells(2, 8).text
    macro_name = "'" & macro_name & "'!Create_Web_UI_2"
    Application.Run macro_name
End Sub
'*************************************************************
'*************************************************************
'*** UPDATE DCI LIST BUTTON LINKS
'*************************************************************
'*************************************************************
Sub Update_DCI_List_Btn()
    Dim macro_name As String
    
    Call DCI_Descriptors_Open_DCI_Macros
    
    macro_name = Worksheets("DCI Descriptors").Range("A1").Cells(2, 8).text
    macro_name = "'" & macro_name & "'!Update_Lang_List"
    Application.Run macro_name
End Sub
'*************************************************************
'*************************************************************
'*** VERIFY DCI LIST BUTTON LINKS
'*************************************************************
'*************************************************************
Sub Verify_DCI_List_Btn()
    Dim macro_name As String
    
    Call DCI_Descriptors_Open_DCI_Macros
    
    macro_name = Worksheets("DCI Descriptors").Range("A1").Cells(2, 8).text
    macro_name = "'" & macro_name & "'!Verify_Lang_List"
    Application.Run macro_name
End Sub
'*************************************************************
'*************************************************************
'*** UPDATE Language Pack BUTTON LINKS
'*************************************************************
'*************************************************************
Sub Create_Lang_Files_Btn()
    Dim macro_name As String
    
    Call DCI_Descriptors_Open_DCI_Macros
    
    macro_name = Worksheets("DCI Descriptors").Range("A1").Cells(2, 8).text
    macro_name = "'" & macro_name & "'!Generate_Lang_INI_JSON"
    Application.Run macro_name
End Sub
'*************************************************************
'*************************************************************
'*** IEC61850 GOOSE Button Links.
'*************************************************************
'*************************************************************

Sub IEC61850_GOOSE_Create_DCI_Btn()
    Dim macro_name As String
    
    Call DCI_Descriptors_Open_DCI_Macros
    
    macro_name = Worksheets("DCI Descriptors").Range("A1").Cells(2, 8).text
    macro_name = "'" & macro_name & "'!Create_IEC61850_GOOSE_DCI_Xfer"
    Application.Run macro_name
End Sub
'*************************************************************
'*************************************************************
'*** IEC61850 SAV Button Links.
'*************************************************************
'*************************************************************
Sub IEC61850_SAV_Create_DCI_Btn()
    Dim macro_name As String
    
    Call DCI_Descriptors_Open_DCI_Macros
    
    macro_name = Worksheets("DCI Descriptors").Range("A1").Cells(2, 8).text
    macro_name = "'" & macro_name & "'!Create_IEC61850_SAV_DCI_Xfer"
    Application.Run macro_name
End Sub
