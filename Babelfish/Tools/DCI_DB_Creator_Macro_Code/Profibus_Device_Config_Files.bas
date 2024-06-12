Attribute VB_Name = "Profibus_Device_Config_Files"
'**************************
'**************************
'Profibus Device sheet.
'**************************
'**************************

Public Enum ModTypeLevel
        NotConnected
        InputModule
        OutputModule
        MaxLevel
End Enum
Public Enum ModSizeLevel
        'NONE
        BIT
        WORD
        MaxSizeLevel
End Enum


Public Function shl(ByVal value As Long, ByVal Shift As Byte) As Long
    shl = value
    If Shift > 0 Then
        Dim i As Byte
        Dim m As Long
        For i = 1 To Shift
            m = shl And &H40000000
            shl = (shl And &H3FFFFFFF) * 2
            If m <> 0 Then
                shl = shl Or &H80000000
            End If
        Next i
    End If
End Function



Public Function HexToString(ByVal HexToStr As String) As String
Dim strTemp   As String
Dim strReturn As String
Dim i         As Long
For i = 1 To Len(HexToStr) Step 3
strTemp = Chr$(Val("&H" & Mid$(HexToStr, i, 2)))
strReturn = strReturn & strTemp
Next i
HexToString = strReturn
End Function



Public Sub Verify_Profibus_Device_List()

    Application.Calculation = xlCalculationManual

'*********************
'******     Constants
'*********************
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    
    
    PROFI_START_ROW = 12
    PROFI_DESCRIPTORS_COL = 1
    PROFI_DEFINES_COL = 2
    PROFI_DATA_LEN_COL = 5
    PROFI_ID_COL = 6
    PROFI_MIN_COL = 5
    PROFI_MAX_COL = 6
    PROFI_DATA_TYPE_COL = 3
    PROFI_DATA_TYPE_LEN_COL = 4
    
    PROFI_CFG_MSB_ID_COL = 10
    PROFI_CFG_LSB_ID_COL = 11
    
    
    START_ROW = 12
    DESCRIP_COL = 1
    DATA_DEFINES_COL = 2
    DATA_PROFI_ID_COL = 5
    DATA_LEN_COL = 6
    DATA_MIN_COL = 5
    DATA_MAX_COL = 6
    DATA_TYPE_LEN_COL = 3
    DATA_TYPE_COL = 4
    DATA_USER_PARAM_COL = 10
    DATA_MODULE_PARAM_COL = 11 'k
    DATA_OUTPUT_COL = DATA_MODULE_PARAM_COL + 1 '12 L
    DATA_INPUT_COL = DATA_OUTPUT_COL + 1 '13 M
    DATA_CFG_DEFAULT_COL = DATA_INPUT_COL + 1 '14 N
    DATA_CFG_OP_LEN_CODE_COL = DATA_CFG_DEFAULT_COL + 1 '15 O
    DATA_CFG_IP_LEN_CODE_COL = DATA_CFG_OP_LEN_CODE_COL + 1 '16 P
    DATA_CFG_MSB_ID_COL = DATA_CFG_IP_LEN_CODE_COL + 1 ' 17 Q
    DATA_CFG_LSB_ID_COL = DATA_CFG_MSB_ID_COL + 1 ' 18 R
    
    
    
    
    
    

'*********************
'******     Begin
'*********************
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select
    
    


'****************************************************************************************************************************
'******     Flag the Data items that are no longer in main Profibus list
'****************************************************************************************************************************
    local_row_ctr = START_ROW
    
    
    While Cells(local_row_ctr, DATA_DEFINES_COL).text <> BEGIN_IGNORED_DATA
        lookup_string = Cells(local_row_ctr, DATA_DEFINES_COL).text
        profi_row_ctr = PROFI_START_ROW
        While (Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DEFINES_COL).value <> (Cells(local_row_ctr, DATA_DEFINES_COL).value)) And _
                (Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DEFINES_COL).value <> BEGIN_IGNORED_DATA)
            profi_row_ctr = profi_row_ctr + 1
        Wend
                        
        If Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DEFINES_COL).value = BEGIN_IGNORED_DATA Then
            Cells(local_row_ctr, DATA_DEFINES_COL).Interior.Color = RGB(255, 0, 0)
            Cells(local_row_ctr, DESCRIP_COL).Interior.Color = RGB(255, 0, 0)
        End If
        
        local_row_ctr = local_row_ctr + 1
    Wend


'****************************************************************************************************************************
'******  This will go through the main Profibus list and copy ID, data length, min & max, and type length to slave sheets
'****************************************************************************************************************************
    local_row_ctr = START_ROW
    
    While (Cells(local_row_ctr, DATA_DEFINES_COL).value <> BEGIN_IGNORED_DATA)
        profi_row_ctr = PROFI_START_ROW
        While (Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DEFINES_COL).value <> (Cells(local_row_ctr, DATA_DEFINES_COL).value)) And _
                (Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DEFINES_COL).value <> BEGIN_IGNORED_DATA)
            profi_row_ctr = profi_row_ctr + 1
        Wend
        If Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DEFINES_COL).value <> BEGIN_IGNORED_DATA Then
            Cells(local_row_ctr, DATA_PROFI_ID_COL).value = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_ID_COL).text
            Cells(local_row_ctr, DATA_LEN_COL).value = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DATA_LEN_COL).text
            Cells(local_row_ctr, DATA_MIN_COL).value = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_MIN_COL).text
            Cells(local_row_ctr, DATA_MAX_COL).value = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_MAX_COL).text
            Cells(local_row_ctr, DATA_TYPE_LEN_COL).value = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DATA_TYPE_LEN_COL).text
            Cells(local_row_ctr, DATA_TYPE_COL).value = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DATA_TYPE_COL).text
            If Cells(local_row_ctr, DATA_OUTPUT_COL).text = Empty And Cells(local_row_ctr, DATA_INPUT_COL).text = Empty Then
                'Do nothing
            Else
                'SSN: This has been commented since DATA_CFG_LEN_CODE_COL has been segregated into IN and OUT code LEN
                'Cells(local_row_ctr, DATA_CFG_LEN_CODE_COL).Value = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_CFG_LEN_BYTE_COL).Text
                Cells(local_row_ctr, DATA_CFG_MSB_ID_COL).value = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_CFG_MSB_ID_COL).text
                Cells(local_row_ctr, DATA_CFG_LSB_ID_COL).value = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_CFG_LSB_ID_COL).text
            End If
        Else
            Cells(local_row_ctr, DATA_PROFI_ID_COL).Interior.Color = RGB(255, 0, 0)
            Cells(local_row_ctr, DATA_LEN_COL).Interior.Color = RGB(255, 0, 0)
            Cells(local_row_ctr, DATA_MIN_COL).Interior.Color = RGB(255, 0, 0)
            Cells(local_row_ctr, DATA_MAX_COL).Interior.Color = RGB(255, 0, 0)
            Cells(local_row_ctr, DATA_TYPE_LEN_COL).Interior.Color = RGB(255, 0, 0)
            Cells(local_row_ctr, DATA_TYPE_COL).Interior.Color = RGB(255, 0, 0)
        End If
            
        local_row_ctr = local_row_ctr + 1
    Wend


    Application.Calculation = xlCalculationAutomatic

End Sub
Sub Verify_IO_Modules()
    local_row_ctr = 0
    local_row_ctr = local_row_ctr + 1
    
    
End Sub

    
   
 
    


Dim ModType As ModTypeLevel

Sub Generate_Enum_Values()


    Max_Bit_Value = 16
    ModType = NotConnected
    ModSize = BIT
   module_type = 0
   module_size = 0
   Profi_Addr = 0
   Target_Bit_Number = 0
   Module_Value = 0
   sheet_name = ActiveSheet.name
   Sheets(sheet_name).Select
    
   Dim DCI_STRING As String
    DCI_STRING = "BEGIN_IGNORED_DATA"
   DCI_Value = 0
   Dim DCI_Description As String
   Dim TestHex As String
   TestHex = ""
   Cells(5, 5).value = TestHex
   Cells(5, 6).value = TestHex
   Cells(6, 5).value = TestHex
   Cells(6, 6).value = TestHex
   Dim ConcatHex As String
   ConcatHex = ""
   
   START_ROW = 12
   DATA_DEFINES_COL = 2
   INPUT_COL = 3
   OUTPUT_COL = 4
   BIT_COL = 5
   WORD_COL = 6
   ENUM_COL = 8
    
    
   PROFI_DEFINES_COL = 4
   PROFI_DESCRIPTORS_COL = 1
   
   counter = START_ROW
   Cells(5, 5).value = "0=NOT CONNECTED;"
   Cells(5, 6).value = "0=NOT CONNECTED;"
   While (Cells(counter, DATA_DEFINES_COL).text <> "BEGIN_IGNORED_DATA")
        
        profi_row_ctr = START_ROW
        While (Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, DATA_DEFINES_COL).value <> Cells(counter, DATA_DEFINES_COL).value)
            profi_row_ctr = profi_row_ctr + 1
        Wend
        DCI_Value = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DEFINES_COL).value
        ModType = NotConnected
        If Cells(counter, INPUT_COL).text <> Empty Then
                ModType = InputModule
                If Cells(counter, BIT_COL).text <> Empty Then
                    ModSize = BIT
                ElseIf Cells(counter, WORD_COL).text <> Empty Then
                    ModSize = WORD
                End If
        ElseIf Cells(counter, OUTPUT_COL).text <> Empty Then
                ModType = OutputModule
                If Cells(counter, BIT_COL).text <> Empty Then
                    ModSize = BIT
                ElseIf Cells(counter, WORD_COL).text <> Empty Then
                    ModSize = WORD
                End If
        End If
        
            Select Case (ModType)
            Case NotConnected
                    TestHex = ""
                    module_type = &H0
                    module_size = &H0
                    Profi_Addr = &H0
                    Target_Bit_Number = &H0
                    DCI_Description = "Not-Connected"
                    Module_Value = shl(module_type, 28) Or shl(module_size, 24) Or shl(Profi_Addr, 8) Or Target_Bit_Number
                    ' Returns Module_Value.
                    ConcatHex = "0x" & Hex(Module_Value) & "=" & DCI_Description & "." & Target_Bit_Number '& ";" Cells(counter, DATA_DEFINES_COL).Text
                    TestHex = TestHex & ConcatHex
                    Cells(START_ROW, ENUM_COL).value = TestHex
            Case InputModule
                    module_type = &H1
                    minValue = &HFFFFFFFF
                    MaxValue = 0
                    DCI_Description = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DESCRIPTORS_COL).value
                        Select Case (ModSize)
                        Case BIT
                            TestHex = Cells(5, 5).value
                            module_size = 1
                            Profi_Addr = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DEFINES_COL).value
                            Target_Bit_Number = 0
                            Do While Target_Bit_Number < Max_Bit_Value
                                Module_Value = shl(module_type, 28) Or shl(module_size, 24) Or shl(Profi_Addr, 8) Or Target_Bit_Number
                                If (Module_Value < minValue) Then
                                    IPMinValue = Module_Value
                                End If
                                If (MaxValue > Module_Value) Then
                                    IPMaxValue = Module_Value
                                End If
                                ' Returns Module_Value.
                                'If Target_Bit_Number < (Max_Bit_Value - 1) Then
                                'ConcatHex = "0x" & Hex(Module_Value) & "=" & DCI_Description & "." & Target_Bit_Number & ";" 'Cells(counter, DATA_DEFINES_COL).Text
                                ConcatHex = Module_Value & "=" & DCI_Description & "." & Target_Bit_Number & ";" 'Cells(counter, DATA_DEFINES_COL).Text
                                'Else
                                'ConcatHex = "0x" & Hex(Module_Value) & "=" & DCI_Description & "." & Target_Bit_Number '& ";" Cells(counter, DATA_DEFINES_COL).Text
                                'End If
                                TestHex = TestHex & ConcatHex
                                'Cells(START_ROW, ENUM_COL).Value = TestHex
                                Cells(5, 5).value = TestHex
                                Target_Bit_Number = Target_Bit_Number + 1
                            Loop
                                                   
                            
                        Case WORD
                            TestHex = Cells(5, 6).value
                            module_size = 2
                            Profi_Addr = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DEFINES_COL).value
                            Target_Bit_Number = &H0
                            Module_Value = shl(module_type, 28) Or shl(module_size, 24) Or shl(Profi_Addr, 8) Or Target_Bit_Number
                            ' Returns Module_Value.
                            'ConcatHex = "0x" & Hex(Module_Value) & "=" & DCI_Description & "." & Target_Bit_Number & ";" 'Cells(counter, DATA_DEFINES_COL).Text
                            ConcatHex = (Module_Value) & "=" & DCI_Description & "." & Target_Bit_Number & ";" 'Cells(counter, DATA_DEFINES_COL).Text
                            TestHex = TestHex & ConcatHex
                            'Cells(START_ROW, ENUM_COL).Value = TestHex
                            Cells(5, 6).value = TestHex
                        Case Else
                            module_size = 0
                        End Select
            Case OutputModule
                    module_type = &H2
                    DCI_Description = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DESCRIPTORS_COL).value
                        Select Case (ModSize)
                        Case BIT
                            TestHex = Cells(6, 5).value
                            module_size = 1
                            Profi_Addr = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DEFINES_COL).value
                            Target_Bit_Number = 0
                            Do While Target_Bit_Number < Max_Bit_Value
                                Module_Value = shl(module_type, 28) Or shl(module_size, 24) Or shl(Profi_Addr, 8) Or Target_Bit_Number
                                ' Returns Module_Value.
                                'If Target_Bit_Number < (Max_Bit_Value - 1) Then
                                'ConcatHex = "0x" & Hex(Module_Value) & "=" & DCI_Description & "." & Target_Bit_Number & ";" 'Cells(counter, DATA_DEFINES_COL).Text
                                ConcatHex = Module_Value & "=" & DCI_Description & "." & Target_Bit_Number & ";" 'Cells(counter, DATA_DEFINES_COL).Text                                'Else
                                'ConcatHex = "0x" & Hex(Module_Value) & "=" & DCI_Description & "." & Target_Bit_Number '& ";" Cells(counter, DATA_DEFINES_COL).Text
                                'End If
                                TestHex = TestHex & ConcatHex
                                'Cells(START_ROW, ENUM_COL).Value = TestHex
                                Cells(6, 5).value = TestHex
                                Target_Bit_Number = Target_Bit_Number + 1
                            Loop
                            
                        Case WORD
                            TestHex = Cells(6, 6).value
                            module_size = 2
                            Profi_Addr = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DEFINES_COL).value
                            Target_Bit_Number = &H0
                            Module_Value = shl(module_type, 28) Or shl(module_size, 24) Or shl(Profi_Addr, 8) Or Target_Bit_Number
                            ' Returns Module_Value.
                            ConcatHex = "0x" & Hex(Module_Value) & "=" & DCI_Description & "." & Target_Bit_Number & ";" 'Cells(counter, DATA_DEFINES_COL).Text
                            ConcatHex = (Module_Value) & "=" & DCI_Description & "." & Target_Bit_Number & ";" 'Cells(counter, DATA_DEFINES_COL).Text
                            TestHex = TestHex & ConcatHex
                            'Cells(START_ROW, ENUM_COL).Value = TestHex
                            Cells(6, 6).value = TestHex
                        Case Else
                            module_size = 0
                        End Select
            Case MaxLevel
                    module_type = &H0
            Case Else
                    module_type = &H0
             
            End Select
                    'ModType = ModType + 1
                    
                    
                    'module_type = &H1
                    'module_size = &H2
                    'Profi_Addr = &HAC
                    'Target_Bit_Number = &HF

        'If (Cells(counter, DATA_USER_PARAM_COL).Text <> Empty) Then
            'num_user_parameters = num_user_parameters + 1
            'tot_user_param_len = tot_user_param_len + Cells(counter, DATA_LEN_COL).Value
        'End If
        counter = counter + 1
    Wend
    Basic_Type_module_1 = 43
    counter = Basic_Type_module_1
    DCI_LIST_COL = 4
    Component_Type_COL = 5
    Access_Type_Col = 6
    'SSN: Here work is to be done so there is a debug error, ignore it temporarily
    While ((Cells(counter, DCI_LIST_COL) <> Empty) And (Cells(counter, DCI_LIST_COL) <> BEGIN_IGNORED_DATA))
        If (Cells(counter, Component_Type_COL).text = "INPUT") Then
            If (Cells(counter, Access_Type_Col).text = "BIT") Then
                'search the row in the DCI Descripters sheet
                dci_sheet_row = Find_DCI_Row(Cells(counter, DCI_LIST_COL))
                If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_ENUM_SUPP_COL).text <> Empty) Then
                    Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DATA_MIN_COL).value = IPMinValue
                    Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DATA_MAX_COL).value = IPMaxValue
                End If
                '   PROFIBUS_GSD.Writeline ("Vendor_Name = " & Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, 7).Text)
            End If
        End If
        counter = counter + 1
    Wend
    
End Sub
Sub Generate_Output_Bit_Enum_Values()


    
   
   START_ROW = 12
   DATA_DESCRIPTION_COL = 1
   DATA_DEFINES_COL = 2
   Dest_Profi_Addr_MSB_COL = 4
   Dest_Profi_Addr_LSB_COL = 5
   
   
    
    
   PROFI_DEFINES_COL = 4
   PROFI_DESCRIPTORS_COL = 1
   DATA_TYPE_COL = 4
   
   sheet_name = ActiveSheet.name
   Sheets(sheet_name).Select
   
   DestBitsize = 0
   Dim data_type As String
   counter = 43
   While (Cells(counter, DATA_DEFINES_COL).text <> "BEGIN_IGNORED_DATA")
        
        profi_row_ctr = START_ROW
        While (Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, DATA_DEFINES_COL).value <> Cells(counter, DATA_DEFINES_COL).value)
            profi_row_ctr = profi_row_ctr + 1
        Wend
        DCI_Value = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DEFINES_COL).value
        data_type = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, DATA_TYPE_COL).value
        
        Select Case (data_type)
                Case "DCI_DTYPE_UINT8"
                DestBitsize = 8
                Case "DCI_DTYPE_BYTE"
                DestBitsize = 8
                Case "DCI_DTYPE_UINT16"
                DestBitsize = 16
                Case "DCI_DTYPE_WORD"
                DestBitsize = 16
                Case "DCI_DTYPE_UINT32"
                DestBitsize = 32
                Case "DCI_DTYPE_DWORD"
                DestBitsize = 32
                Case Else
                DestBitsize = 0
        End Select
        DestBitAddr = 0
        Do While DestBitAddr < DestBitsize
                    Module_COL = 2
                    Access_Type_Col = 6
                    Comp_Type_Col = 5
                    IO_Module_def_start_row = 73
                    profi_dci_start_row = 12
                    profi_dci_row_ctr = profi_dci_start_row
                    modulecount = IO_Module_def_start_row
                    While (Cells(modulecount, Module_COL).text <> "BEGIN_IGNORED_DATA")
                        If Cells(modulecount, Module_COL).text <> Empty Then
                                BitCount = modulecount
                                Bitsize = 0
                                While (Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_dci_row_ctr, DATA_DESCRIPTION_COL).text <> Cells(modulecount, Module_COL).text)
                                    profi_dci_row_ctr = profi_dci_row_ctr + 1
                                Wend
                                Module_Number = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_dci_row_ctr, PROFI_DEFINES_COL).value
                                Do While (Cells(BitCount, Comp_Type_Col).text <> "BEGIN_IGNORED_DATA")
                                    If Cells(BitCount, Comp_Type_Col).text = "OUTPUT" Then
                                            If Cells(BitCount, Access_Type_Col).text = "BIT" Then
                                                Enum_Value = shl(DCI_Value, 24) Or shl(DestBitAddr, 16) Or shl(Module_Number, 8) Or Bitsize
                                                ConcatHex = "0x" & Hex(Enum_Value) & "=" & DCI_Description & "." & Target_Bit_Number '& ";" Cells(counter, DATA_DEFINES_COL).Text
                                                TestHex = TestHex & ConcatHex
                                                Cells(37, 4).value = TestHex
                                                Bitsize = Bitsize + 1
                                            End If
                                    End If
                                    BitCount = BitCount + 1
                                Loop
                                
                                
                                
                        End If
                        modulecount = modulecount + 1
                    Wend
                    
        DestBitAddr = DestBitAddr + 1
        Loop
        ModType = NotConnected
        If Cells(counter, INPUT_COL).text <> Empty Then
                ModType = InputModule
                If Cells(counter, BIT_COL).text <> Empty Then
                    ModSize = BIT
                ElseIf Cells(counter, WORD_COL).text <> Empty Then
                    ModSize = WORD
                End If
        ElseIf Cells(counter, OUTPUT_COL).text <> Empty Then
                ModType = OutputModule
                If Cells(counter, BIT_COL).text <> Empty Then
                    ModSize = BIT
                ElseIf Cells(counter, WORD_COL).text <> Empty Then
                    ModSize = WORD
                End If
        End If
        
            Select Case (ModType)
            Case NotConnected
                    TestHex = ""
                    module_type = &H0
                    module_size = &H0
                    Profi_Addr = &H0
                    Target_Bit_Number = &H0
                    DCI_Description = "Not-Connected"
                    Module_Value = shl(module_type, 28) Or shl(module_size, 24) Or shl(Profi_Addr, 8) Or Target_Bit_Number
                    ' Returns Module_Value.
                    ConcatHex = "0x" & Hex(Module_Value) & "=" & DCI_Description & "." & Target_Bit_Number '& ";" Cells(counter, DATA_DEFINES_COL).Text
                    TestHex = TestHex & ConcatHex
                    Cells(START_ROW, ENUM_COL).value = TestHex
            Case InputModule
                    module_type = &H1
                    minValue = &HFFFFFFFF
                    MaxValue = 0
                    DCI_Description = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DESCRIPTORS_COL).value
                        Select Case (ModSize)
                        Case BIT
                            TestHex = Cells(5, 5).value
                            module_size = 1
                            Profi_Addr = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DEFINES_COL).value
                            Target_Bit_Number = 0
                            Do While Target_Bit_Number < Max_Bit_Value
                                Module_Value = shl(module_type, 28) Or shl(module_size, 24) Or shl(Profi_Addr, 8) Or Target_Bit_Number
                                If (Module_Value < minValue) Then
                                    IPMinValue = Module_Value
                                End If
                                If (MaxValue > Module_Value) Then
                                    IPMaxValue = Module_Value
                                End If
                                ' Returns Module_Value.
                                'If Target_Bit_Number < (Max_Bit_Value - 1) Then
                                ConcatHex = "0x" & Hex(Module_Value) & "=" & DCI_Description & "." & Target_Bit_Number & ";" 'Cells(counter, DATA_DEFINES_COL).Text
                                'Else
                                'ConcatHex = "0x" & Hex(Module_Value) & "=" & DCI_Description & "." & Target_Bit_Number '& ";" Cells(counter, DATA_DEFINES_COL).Text
                                'End If
                                TestHex = TestHex & ConcatHex
                                'Cells(START_ROW, ENUM_COL).Value = TestHex
                                Cells(5, 5).value = TestHex
                                Target_Bit_Number = Target_Bit_Number + 1
                            Loop
                                                   
                            
                        Case WORD
                            TestHex = Cells(5, 6).value
                            module_size = 2
                            Profi_Addr = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DEFINES_COL).value
                            Target_Bit_Number = &H0
                            Module_Value = shl(module_type, 28) Or shl(module_size, 24) Or shl(Profi_Addr, 8) Or Target_Bit_Number
                            ' Returns Module_Value.
                            ConcatHex = "0x" & Hex(Module_Value) & "=" & DCI_Description & "." & Target_Bit_Number & ";" 'Cells(counter, DATA_DEFINES_COL).Text
                            TestHex = TestHex & ConcatHex
                            'Cells(START_ROW, ENUM_COL).Value = TestHex
                            Cells(5, 6).value = TestHex
                        Case Else
                            module_size = 0
                        End Select
            Case OutputModule
                    module_type = &H2
                    DCI_Description = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DESCRIPTORS_COL).value
                        Select Case (ModSize)
                        Case BIT
                            TestHex = Cells(6, 5).value
                            module_size = 1
                            Profi_Addr = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DEFINES_COL).value
                            Target_Bit_Number = 0
                            Do While Target_Bit_Number < Max_Bit_Value
                                Module_Value = shl(module_type, 28) Or shl(module_size, 24) Or shl(Profi_Addr, 8) Or Target_Bit_Number
                                ' Returns Module_Value.
                                'If Target_Bit_Number < (Max_Bit_Value - 1) Then
                                ConcatHex = "0x" & Hex(Module_Value) & "=" & DCI_Description & "." & Target_Bit_Number & ";" 'Cells(counter, DATA_DEFINES_COL).Text
                                'Else
                                'ConcatHex = "0x" & Hex(Module_Value) & "=" & DCI_Description & "." & Target_Bit_Number '& ";" Cells(counter, DATA_DEFINES_COL).Text
                                'End If
                                TestHex = TestHex & ConcatHex
                                'Cells(START_ROW, ENUM_COL).Value = TestHex
                                Cells(6, 5).value = TestHex
                                Target_Bit_Number = Target_Bit_Number + 1
                            Loop
                            
                        Case WORD
                            TestHex = Cells(6, 6).value
                            module_size = 2
                            Profi_Addr = Worksheets("ProfiBus (DCI)").Range("A1").Cells(profi_row_ctr, PROFI_DEFINES_COL).value
                            Target_Bit_Number = &H0
                            Module_Value = shl(module_type, 28) Or shl(module_size, 24) Or shl(Profi_Addr, 8) Or Target_Bit_Number
                            ' Returns Module_Value.
                            ConcatHex = "0x" & Hex(Module_Value) & "=" & DCI_Description & "." & Target_Bit_Number & ";" 'Cells(counter, DATA_DEFINES_COL).Text
                            TestHex = TestHex & ConcatHex
                            'Cells(START_ROW, ENUM_COL).Value = TestHex
                            Cells(6, 6).value = TestHex
                        Case Else
                            module_size = 0
                        End Select
            Case MaxLevel
                    module_type = &H0
            Case Else
                    module_type = &H0
             
            End Select
                    'ModType = ModType + 1
                    
                    
                    'module_type = &H1
                    'module_size = &H2
                    'Profi_Addr = &HAC
                    'Target_Bit_Number = &HF

        'If (Cells(counter, DATA_USER_PARAM_COL).Text <> Empty) Then
            'num_user_parameters = num_user_parameters + 1
            'tot_user_param_len = tot_user_param_len + Cells(counter, DATA_LEN_COL).Value
        'End If
        counter = counter + 1
    Wend
    Basic_Type_module_1 = 43
    counter = Basic_Type_module_1
    DCI_LIST_COL = 4
    Component_Type_COL = 5
    Access_Type_Col = 6
    'SSN: Here work is to be done so there is a debug error, ignore it temporarily
    While ((Cells(counter, DCI_LIST_COL) <> Empty) And (Cells(counter, DCI_LIST_COL) <> BEGIN_IGNORED_DATA))
        If (Cells(counter, Component_Type_COL).text = "INPUT") Then
            If (Cells(counter, Access_Type_Col).text = "BIT") Then
                'search the row in the DCI Descripters sheet
                dci_sheet_row = Find_DCI_Row(Cells(counter, DCI_LIST_COL))
                If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_ENUM_SUPP_COL).text <> Empty) Then
                    Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DATA_MIN_COL).value = IPMinValue
                    Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DATA_MAX_COL).value = IPMaxValue
                End If
                '   PROFIBUS_GSD.Writeline ("Vendor_Name = " & Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, 7).Text)
            End If
        End If
        counter = counter + 1
    Wend
    
End Sub

Sub Create_Device_Profibus_Files()

    Application.Calculation = xlCalculationManual

'*********************
'******     Constants
'*********************
    
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    
    Dim MyDate
    MyDate = Date    ' MyDate contains the current system date.
    
    Dim DCI_STRING As String
    Dim num_unique_data As Integer
'    Dim even_reg_count As Boolean
'    Dim even_unique_reg_count As Boolean
    Dim INPUT_BIT As String
    INPUT_BIT = "CYCLIC INPUT BIT"
    Dim INPUT_BYTE As String
    INPUT_BYTE = "INPUT BYTE"
    Dim INPUT__WORD As String
    INPUT__WORD = "CYCLIC INPUT WORD"
    Dim INPUT_DWORD As String
    INPUT_DWORD = "CYCLIC INPUT DWORD"
    Dim OUTPUT_BIT As String
    OUTPUT_BIT = "CYCLIC OUTPUT BIT"
    Dim OUTPUT_BYTE As String
    OUTPUT_BYTE = "OUTPUT BYTE"
    Dim OUTPUT_WORD As String
    OUTPUT_WORD = "CYCLIC OUTPUT WORD"
    Dim OUTPUT_DWORD As String
    OUTPUT_DWORD = "CYCLIC OUTPUT DWORD"
    Dim BCM_PARAMETER As String
    BCM_PARAMTER = "INITIALIZATION PARAMETER"
    Dim INPUT_BIT_INPUT_WORD As String
    INPUT_BIT_INPUT_WORD = "CYCLIC INPUT BIT INPUT WORD"
    Dim INPUT_BIT_INPUT_DWORD As String
    INPUT_BIT_INPUT_DWORD = "CYCLIC INPUT BIT INPUT DWORD"
    Dim OUTPUT_BIT_OUTPUT_WORD As String
    OUTPUT_BIT_OUTPUT_WORD = "CYCLIC OUTPUT BIT OUTPUT WORD"
    Dim OUTPUT_BIT_OUTPUT_DWORD As String
    OUTPUT_BIT_OUTPUT_DWORD = "CYCLIC OUTPUT BIT OUTPUT DWORD"
    
    PROFI_START_ROW = 12
    PROFI_DESCRIPTORS_COL = 1
    PROFI_DEFINES_COL = 2
    PROFI_DATA_LEN_COL = 5
    PROFI_ID_COL = 6
    PROFI_MIN_COL = 5
    PROFI_MAX_COL = 6
    PROFI_DATA_TYPE_COL = 3
    PROFI_DATA_TYPE_LEN_COL = 4
    
    PROFI_CFG_MSB_ID_COL = 10
    PROFI_CFG_LSB_ID_COL = 11
    
    
    START_ROW = 12
    DESCRIP_COL = 1
    DATA_DEFINES_COL = 2
    DATA_PROFI_ID_COL = 6
    DATA_LEN_COL = 5
    Dim DATA_MIN_COL As Integer
    DATA_MIN_COL = 5
    Dim DATA_MAX_COL As Integer
    DATA_MAX_COL = 6
    DATA_TYPE_LEN_COL = 3
    DATA_TYPE_COL = 4
    DATA_USER_PARAM_COL = 10
    DATA_MODULE_PARAM_COL = 11 'k
    DATA_OUTPUT_COL = DATA_MODULE_PARAM_COL + 1 '12 L
    DATA_INPUT_COL = DATA_OUTPUT_COL + 1 '13 M
    DATA_CFG_DEFAULT_COL = DATA_INPUT_COL + 1 '14 N
    DATA_CFG_OP_LEN_CODE_COL = DATA_CFG_DEFAULT_COL + 1 '15 O
    DATA_CFG_IP_LEN_CODE_COL = DATA_CFG_OP_LEN_CODE_COL + 1 '16 P
    DATA_CFG_MSB_ID_COL = DATA_CFG_IP_LEN_CODE_COL + 1 ' 17 Q
    DATA_CFG_LSB_ID_COL = DATA_CFG_MSB_ID_COL + 1 ' 18 R
    DATA_TYPE_ACCESS_COL = 4
    
     ' Config Module Constants
    DATA_CONFIG_MODULE_START_ROW = 26 '612
    DATA_CONFIG_MODULE_START_COL = 2
    MODULE_INPUT_MAPPING_SIZE_COL = 11
    Dim END_OF_TABLE As String
    END_OF_TABLE = "BEGIN_IGNORED_DATA"
    
    DCI_TYPE_COL_DCI_TAB = 3
    DATA_LEN_COL_DCID_TAB = 4
    DATA_TOTAL_LEN_COL_DCID_TAB = 6
    DCI_MIN_COL_DCI_TAB = 8
    DCI_MAX_COL_DCI_TAB = 9
'*********************
'******     Begin
'*********************
    
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select
    

    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\" & Cells(6, 2).value
    Set PROFIBUS_PRM_C = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(7, 2).value
    Set PROFIBUS_PRM_H = fs.CreateTextFile(file_path, True)
    
    'Making font type and size consistent.
    Worksheets(sheet_name).Range("A:N").Font.name = "Arial"
    Worksheets(sheet_name).Range("A:N").Font.Size = 10
    


'****************************************************************************************************************************
'******     Start Creating the PRM.H file header
'****************************************************************************************************************************
    PROFIBUS_PRM_H.WriteLine ("/*")
    PROFIBUS_PRM_H.WriteLine ("*****************************************************************************************")
    PROFIBUS_PRM_H.WriteLine ("*       $Workfile:" & Cells(5, 2).text & ".h$")
    PROFIBUS_PRM_H.WriteLine ("*")
    PROFIBUS_PRM_H.WriteLine ("*       $Author:$ Steve Toebe")
    PROFIBUS_PRM_H.WriteLine ("*       Copyright © Eaton Corporation. All Rights Reserved.")
    PROFIBUS_PRM_H.WriteLine ("*       Creator: Steve Toebe")
    PROFIBUS_PRM_H.WriteLine ("*")
    PROFIBUS_PRM_H.WriteLine ("*       Description:Defines the look up structure for the Parameterization Check ")
    PROFIBUS_PRM_H.WriteLine ("*")
    PROFIBUS_PRM_H.WriteLine ("*")
    PROFIBUS_PRM_H.WriteLine ("*       Code Inspection Date: ")
    PROFIBUS_PRM_H.WriteLine ("*")
    PROFIBUS_PRM_H.WriteLine ("*       $Header:$")
    PROFIBUS_PRM_H.WriteLine ("*****************************************************************************************")
    PROFIBUS_PRM_H.WriteLine ("*/")
    PROFIBUS_PRM_H.WriteLine ("#ifndef " & UCase(Cells(5, 2).text) & "_H")
    PROFIBUS_PRM_H.WriteLine ("  #define " & UCase(Cells(5, 2).text) & "_H")
    PROFIBUS_PRM_H.WriteLine ("")
    PROFIBUS_PRM_H.WriteLine ("#include ""DCI.h""")
    PROFIBUS_PRM_H.WriteLine ("#include ""DCI_Defines.h""")
    PROFIBUS_PRM_H.WriteLine ("#include ""DCI_Profibus_Data.h""")
    PROFIBUS_PRM_H.WriteLine ("#include ""ProfiBus_Identity.h""")
    PROFIBUS_PRM_H.WriteLine ("#include ""ProfiBus_Parameterization.h""")
    PROFIBUS_PRM_H.WriteLine ("")
        
    ' Calculate INITIALIZATION PARAMETERs
    num_BCM_parameters = 0
    tot_BCM_param_len = 0
    dci_sheet_row = 0
    counter = START_ROW
    While (Cells(counter, DATA_DEFINES_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, DATA_TYPE_ACCESS_COL).text = BCM_PARAMTER) Then
            num_BCM_parameters = num_BCM_parameters + 1
            dci_sheet_row = Find_DCI_Row(Cells(counter, 1))
            tot_BCM_param_len = tot_BCM_param_len + Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_LENGTH_COL).value
        End If
        counter = counter + 1
    Wend

'****************************************************************************************************************************
'****** Gotta calculate the total number & data length of the user parameters we are going to have in Parameterization telegram.
'****************************************************************************************************************************
    num_user_parameters = 0
    tot_param_len = 0
    tot_user_param_len = 0
    counter = START_ROW
    While (Cells(counter, DATA_DEFINES_COL).text <> BEGIN_IGNORED_DATA)
        If ((Cells(counter, DATA_TYPE_ACCESS_COL).text = OUTPUT_BIT) _
        Or (Cells(counter, DATA_TYPE_ACCESS_COL).text = OUTPUT_BYTE) _
        Or (Cells(counter, DATA_TYPE_ACCESS_COL).text = OUTPUT_WORD) _
        Or (Cells(counter, DATA_TYPE_ACCESS_COL).text = OUTPUT_DWORD) _
        Or (Cells(counter, DATA_TYPE_ACCESS_COL).text = OUTPUT_BIT_OUTPUT_WORD) _
        Or (Cells(counter, DATA_TYPE_ACCESS_COL).text = OUTPUT_BIT_OUTPUT_DWORD)) Then
            If (Cells(counter, DATA_TYPE_ACCESS_COL).text = OUTPUT_BIT) Then
                num_user_parameters = num_user_parameters + Get_Number_Of_Bits(Cells(counter, DATA_DEFINES_COL).text)
                ' For Parameterization Each Bit takes one byte
                tot_user_param_len = tot_user_param_len + Get_Number_Of_Bits(Cells(counter, DATA_DEFINES_COL).text)
            Else
                If (Cells(counter, DATA_TYPE_ACCESS_COL).text = OUTPUT_BIT_OUTPUT_WORD) _
                Or (Cells(counter, DATA_TYPE_ACCESS_COL).text = OUTPUT_BIT_OUTPUT_DWORD) Then
                num_user_parameters = num_user_parameters + Get_Number_Of_Bits(Cells(counter, DATA_DEFINES_COL).text)
                num_user_parameters = num_user_parameters + 1
                tot_user_param_len = tot_user_param_len + Get_Number_Of_Bits(Cells(counter, DATA_DEFINES_COL).text)
                tot_user_param_len = tot_user_param_len + 1
                 'This is because we have both bits and bytes
                Else
                num_user_parameters = num_user_parameters + 1
                ' thought word has two bytes but it takes only one byte during parameterization
                tot_user_param_len = tot_user_param_len + 1
                End If
            End If
            
        End If
        counter = counter + 1
    Wend

'****************************************************************************************************************************
'****** Gotta calculate the total number & data length of the module parameters we are going to have in Parameterization telegram.
'****************************************************************************************************************************
    num_mod_parameters = 0
    tot_mod_param_len = 0
    mandatory_bytes = 7
    DPV1_status_bytes = 3
    Acyclic_Parameterization_bytes = 0
    counter = START_ROW
    max_input_byte_in_module = 0
    While (Cells(counter, DATA_DEFINES_COL).text <> BEGIN_IGNORED_DATA)
        If ((Cells(counter, DATA_TYPE_ACCESS_COL).text = INPUT_BIT) _
        Or (Cells(counter, DATA_TYPE_ACCESS_COL).text = INPUT_BYTE) _
        Or (Cells(counter, DATA_TYPE_ACCESS_COL).text = INPUT__WORD) _
        Or (Cells(counter, DATA_TYPE_ACCESS_COL).text = INPUT_DWORD) _
        Or (Cells(counter, DATA_TYPE_ACCESS_COL).text = INPUT_BIT_INPUT_WORD) _
        Or (Cells(counter, DATA_TYPE_ACCESS_COL).text = INPUT_BIT_INPUT_DWORD)) Then
            If (Cells(counter, DATA_TYPE_ACCESS_COL).text = INPUT_BIT) Then
            num_mod_parameters = num_mod_parameters + Get_Number_Of_Bits(Cells(counter, DATA_DEFINES_COL).text)
            Else
                If (Cells(counter, DATA_TYPE_ACCESS_COL).text = INPUT_BIT_INPUT_WORD) _
                Or (Cells(counter, DATA_TYPE_ACCESS_COL).text = INPUT_BIT_INPUT_DWORD) Then
                num_mod_parameters = num_mod_parameters + Get_Number_Of_Bits(Cells(counter, DATA_DEFINES_COL).text)
                num_mod_parameters = num_mod_parameters + 1
                'This is because we have both bits and bytes
                Else
                num_mod_parameters = num_mod_parameters + 1
                End If
            End If
        End If
        counter = counter + 1
    Wend
    Sheets("ProfiBus(IO Modules)").Select
    counter = DATA_CONFIG_MODULE_START_ROW
    'get the size of input mapping bytes for biggest module
    While (Cells(counter, DATA_CONFIG_MODULE_START_COL).text <> END_OF_TABLE)
        If (Cells(counter, MODULE_INPUT_MAPPING_SIZE_COL).value > max_input_byte_in_module) Then
            max_input_byte_in_module = Cells(counter, MODULE_INPUT_MAPPING_SIZE_COL).value
        End If
        counter = counter + 1
    Wend
    ACYCLIC_ROW_NUMBER = 51
    OUTPUT_DATA_SIZE = 8
    INPUT_DATA_SIZE = 7
    
    max_allowed_modules = Worksheets("ProfiBus (GSD)").Cells(47, 2).value
    
    tot_mod_param_len = max_input_byte_in_module * max_allowed_modules
    
    Acyclic_Parameterization_bytes = Cells(ACYCLIC_ROW_NUMBER, MODULE_INPUT_MAPPING_SIZE_COL).value
    Acyclic_Read_bytes = Cells(ACYCLIC_ROW_NUMBER, INPUT_DATA_SIZE).value
    Acyclic_Write_bytes = Cells(ACYCLIC_ROW_NUMBER, OUTPUT_DATA_SIZE).value
    tot_param_len = Acyclic_Parameterization_bytes + tot_BCM_param_len + tot_user_param_len + tot_mod_param_len + mandatory_bytes + DPV1_status_bytes + max_allowed_modules '** Default parameter bytes + DPV1 bytes + PRM_enable byte = 11

'****************************************************************************************************************************
'******     Create the ProfiBus PRM Look-up structure.  Get ready to plop the data down.
'****************************************************************************************************************************
    
    Sheets(sheet_name).Select
    PROFIBUS_PRM_H.WriteLine ("//*******************************************************")
    PROFIBUS_PRM_H.WriteLine ("//******     " & "ProfiBus" & " Parameterization Defines           ")
    PROFIBUS_PRM_H.WriteLine ("//*******************************************************")
    PROFIBUS_PRM_H.WriteLine ("")
    PROFIBUS_PRM_H.WriteLine ("#define " & "ProfiBus" & "_ONE_TIME_CHECK_IN_PARAMETER       " & num_BCM_parameters & "U")
    PROFIBUS_PRM_H.WriteLine ("#define " & "ProfiBus" & "_ONE_TIME_CHECK_IN_PARAMETER_LENGTH       " & tot_BCM_param_len & "U")
    ''PROFIBUS_PRM_H.WriteLine ("#define " & "ProfiBus" & "_ACYCLIC_READ_PARAMETER_LENGTH       " & Acyclic_Parameterization_bytes & "U")
    ''PROFIBUS_PRM_H.WriteLine ("#define " & "ProfiBus" & "_ACYCLIC_READ_BYTES_LENGTH       " & Acyclic_Read_bytes & "U")
    ''PROFIBUS_PRM_H.WriteLine ("#define " & "ProfiBus" & "_ACYCLIC_WRITE_BYTES_LENGTH       " & Acyclic_Write_bytes & "U")
    PROFIBUS_PRM_H.WriteLine ("#define " & "ProfiBus" & "_PROFIBUS_USR_PRM_TOTAL        " & num_user_parameters & "U")
    PROFIBUS_PRM_H.WriteLine ("//The Total Bytes of User Parameter Data")
    PROFIBUS_PRM_H.WriteLine ("#define " & "ProfiBus" & "_DOWNLOAD_USR_PRM_LEN          " & num_user_parameters & "U")
    PROFIBUS_PRM_H.WriteLine ("#define " & "ProfiBus" & "_PROFIBUS_MOD_PRM_TOTAL        " & num_mod_parameters & "U")
    PROFIBUS_PRM_H.WriteLine ("//The Total Bytes of Module Parameter Data")
        If (tot_mod_param_len <> 0) Then
    PROFIBUS_PRM_H.WriteLine ("#define " & "ProfiBus" & "_DOWNLOAD_MOD_PRM_LEN          " & tot_mod_param_len & "U")
        Else
        PROFIBUS_PRM_H.WriteLine ("#define " & "ProfiBus" & "_DOWNLOAD_MOD_PRM_LEN          " & (tot_mod_param_len + 1) & "U/*This is 1 because we dont have any ip parameter in module*/")
        End If
    PROFIBUS_PRM_H.WriteLine ("#define " & "ProfiBus" & "_MANDATORY_DATA_LEN          " & mandatory_bytes & "U")
    PROFIBUS_PRM_H.WriteLine ("#define " & "ProfiBus" & "_DPV1_STATUS_DATA_LEN          " & DPV1_status_bytes & "U")
    PROFIBUS_PRM_H.WriteLine ("#define " & "ProfiBus" & "_MAX_PRM_DATA_LEN          " & tot_param_len & "U")
    PROFIBUS_PRM_H.WriteLine ("")
    PROFIBUS_PRM_H.WriteLine ("//*******************************************************")
    PROFIBUS_PRM_H.WriteLine ("//******     Profibus User Parameter Look-up Structure ")
    PROFIBUS_PRM_H.WriteLine ("//*******************************************************")
    PROFIBUS_PRM_H.WriteLine ("")
    PROFIBUS_PRM_H.WriteLine ("")
    PROFIBUS_PRM_H.WriteLine ("#define " & "ProfiBus" & "_MAX_USR_PRM_DATA_LEN          " & tot_param_len & "U")
    PROFIBUS_PRM_H.WriteLine ("extern const PROFIBUS_USR_PRM_LKUP_ST_TD     " & "ProfiBus" & "_usr_prm_lkup[ " & "ProfiBus" & "_ONE_TIME_CHECK_IN_PARAMETER ];")
    PROFIBUS_PRM_H.WriteLine ("extern const PROFI_USR_PRM_STRUCT    " & "ProfiBus" & "_USR_PRM_STRUCT;")
    PROFIBUS_PRM_H.WriteLine ("")
    PROFIBUS_PRM_H.WriteLine ("")
    PROFIBUS_PRM_H.WriteLine ("")
    PROFIBUS_PRM_H.WriteLine ("#endif")
    
    
    
'****************************************************************************************************************************
'******     Start Creating the PRM.C file header
'****************************************************************************************************************************
    PROFIBUS_PRM_C.WriteLine ("/**")
    PROFIBUS_PRM_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_PRM_C.WriteLine ("*   @file")
    PROFIBUS_PRM_C.WriteLine ("*   @details See header file for module overview.")
    PROFIBUS_PRM_C.WriteLine ("*   @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    PROFIBUS_PRM_C.WriteLine ("*")
    PROFIBUS_PRM_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_PRM_C.WriteLine ("*/")
    PROFIBUS_PRM_C.WriteLine ("#include ""Includes.h""")
    PROFIBUS_PRM_C.WriteLine ("#include """ & Cells(5, 2).text & ".h""")
    PROFIBUS_PRM_C.WriteLine ("")
    PROFIBUS_PRM_C.WriteLine ("")
    PROFIBUS_PRM_C.WriteLine ("")


'****************************************************************************************************************************
'******     Generate the min & max constants for parameter lookup table.
'****************************************************************************************************************************
    PROFIBUS_PRM_C.WriteLine ("/*")
    PROFIBUS_PRM_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_PRM_C.WriteLine ("*        " & "ProfiBus" & " Parameter min & max range constants")
    PROFIBUS_PRM_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_PRM_C.WriteLine ("*/")
    PROFIBUS_PRM_C.WriteLine ("")
    row_counter = START_ROW
    Dim data_min_col_contents As String
    Dim data_max_col_contents As String
    Dim data_type_col_contents As String
    Dim min_value As Variant
    Dim max_value As Variant
    Dim value_changed As Variant
        
    While (Cells(row_counter, DATA_DEFINES_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(row_counter, DATA_TYPE_ACCESS_COL).text = BCM_PARAMTER) Then
            ' Write the Min value & max value.
            dci_sheet_row = Find_DCI_Row(Cells(row_counter, 1))
            value_changed = 0
            data_type_col_contents = Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_TYPE_COL_DCI_TAB).text
            'Get_Datatype_String (data_type_col_contents)
            If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_MIN_COL_DCI_TAB).text = "") Then
                If (data_type_col_contents = "DCI_DTYPE_BOOL") Then
                    min_value = 0
                    max_value = 1
                    value_changed = 1
                ElseIf (data_type_col_contents = "DCI_DTYPE_UINT8") Then
                    min_value = 0
                    max_value = 255
                    value_changed = 1
                ElseIf (data_type_col_contents = "DCI_DTYPE_SINT8") Then
                                        min_value = -128
                                        max_value = 127
                                        value_changed = 1
                                ElseIf (data_type_col_contents = "DCI_DTYPE_UINT16") Or (data_type_col_contents = "DCI_DTYPE_WORD") Then
                                        min_value = 0
                                        max_value = 65535
                                        value_changed = 1
                                ElseIf (data_type_col_contents = "DCI_DTYPE_SINT16") Then
                                        min_value = -32768
                                        max_value = 32767
                                        value_changed = 1
                                ElseIf (data_type_col_contents = "DCI_DTYPE_UINT32") Or (data_type_col_contents = "DCI_DTYPE_DWORD") Then
                                        min_value = 0
                                        max_value = 4294967295#
                                        value_changed = 1
                                ElseIf (data_type_col_contents = "DCI_DTYPE_SINT32") Then
                                        min_value = -2147483648#
                                        max_value = 2147483647
                                        value_changed = 1
                                'ElseIf (data_type_col_contents = "DCI_DTYPE_FLOAT") Then
                                '    min_value = "1.175494351E–38"
                                '    max_value = "3.402823466E+38"
                                '    value_changed = 1
                                'ElseIf (data_type_col_contents = "DCI_DTYPE_LFLOAT") Then
                                '    min_value = "2.2250738585072014E–308"
                                '    max_value = "1.79769313486232E+308"
                                '        value_changed = 1
                                'ElseIf (data_type_col_contents = "DCI_DTYPE_DFLOAT") Then
                                '    min_value = "2.2250738585072014E–308"
                                '    max_value = "1.79769313486232E+308"
                                '    value_changed = 1
                                'ElseIf (data_type_col_contents = "DCI_DTYPE_UINT64") Then
                                '    min_value = 0
                                '    max_value = "1.84467440737096E+19"
                                '    value_changed = 1
                End If
            End If
            If (1 = value_changed) Then
                Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_MIN_COL_DCI_TAB).value = min_value
                Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_MAX_COL_DCI_TAB).value = max_value
            End If
                        
            data_min_col_contents = Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_MIN_COL_DCI_TAB).text
            data_max_col_contents = Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_MAX_COL_DCI_TAB).text
            data_type_col_contents = Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_TYPE_COL_DCI_TAB).text
            If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_MIN_COL_DCI_TAB).text <> Empty _
                And (Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_TYPE_COL_DCI_TAB).text = "DCI_DTYPE_UINT16" _
                Or Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_TYPE_COL_DCI_TAB).text = "DCI_DTYPE_UINT8" _
                Or Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_TYPE_COL_DCI_TAB).text = "DCI_DTYPE_BYTE")) Then
                 PROFIBUS_PRM_C.WriteLine ("const  " & Get_Datatype_String(data_type_col_contents) & _
                                "  " & Cells(row_counter, DATA_DEFINES_COL).text & "_MIN_VALUE = " & _
                                data_min_col_contents & "U;")
            ElseIf Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_MIN_COL_DCI_TAB).text <> Empty Then
            PROFIBUS_PRM_C.WriteLine ("const  " & Get_Datatype_String(data_type_col_contents) & _
                                "  " & Cells(row_counter, DATA_DEFINES_COL).text & "_MIN_VALUE = " & _
                                data_min_col_contents & ";")
            End If
            If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_MAX_COL_DCI_TAB).text <> Empty _
                And (Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_TYPE_COL_DCI_TAB).text = "DCI_DTYPE_UINT16" _
                Or Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_TYPE_COL_DCI_TAB).text = "DCI_DTYPE_UINT8" _
                Or Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_TYPE_COL_DCI_TAB).text = "DCI_DTYPE_BYTE")) Then
                PROFIBUS_PRM_C.WriteLine ("const  " & Get_Datatype_String(data_type_col_contents) & _
                                "  " & Cells(row_counter, DATA_DEFINES_COL).text & "_MAX_VALUE = " & _
                                data_max_col_contents & "U;")
            ElseIf Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_MAX_COL_DCI_TAB).text <> Empty Then
                PROFIBUS_PRM_C.WriteLine ("const  " & Get_Datatype_String(data_type_col_contents) & _
                                "  " & Cells(row_counter, DATA_DEFINES_COL).text & "_MAX_VALUE = " & _
                                data_max_col_contents & ";")
            End If
        End If
        row_counter = row_counter + 1
    Wend
    
'****************************************************************************************************************************
'******     Generate the Profibus User Prm lookup table.
'****************************************************************************************************************************
    PROFIBUS_PRM_C.WriteLine ("/*")
    PROFIBUS_PRM_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_PRM_C.WriteLine ("*        " & "ProfiBus" & " User Parameter Lookup structures")
    PROFIBUS_PRM_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_PRM_C.WriteLine ("*/")
    PROFIBUS_PRM_C.WriteLine ("")
    PROFIBUS_PRM_C.WriteLine ("const PROFIBUS_USR_PRM_LKUP_ST_TD " & "ProfiBus" & "_usr_prm_lkup[ " & "ProfiBus" & "_ONE_TIME_CHECK_IN_PARAMETER ] =")
    PROFIBUS_PRM_C.WriteLine ("{")
    row_counter = START_ROW
    While (Cells(row_counter, DATA_DEFINES_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINES_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(row_counter, DATA_TYPE_ACCESS_COL).text = BCM_PARAMTER) Then
            dci_sheet_row = Find_DCI_Row(Cells(row_counter, 1))
            '
            PROFIBUS_PRM_C.WriteLine ("    {        // " & Cells(row_counter, DESCRIP_COL).text)
            PROFIBUS_PRM_C.WriteLine ("        " & Cells(row_counter, DATA_DEFINES_COL).text & "_PROFIBUS_MOD,")
            PROFIBUS_PRM_C.WriteLine ("        " & Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DATA_LEN_COL_DCID_TAB).text & "U,")
            PROFIBUS_PRM_C.WriteLine ("        " & Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_TYPE_COL_DCI_TAB).text & ",")
            PROFIBUS_PRM_C.WriteLine ("        " & Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DATA_TOTAL_LEN_COL_DCID_TAB).text & "U,")
            If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_TYPE_COL_DCI_TAB).text = "DCI_DTYPE_UINT16") Then
            PROFIBUS_PRM_C.WriteLine ("        " & "reinterpret_cast<DCI_RANGE_DATA_TD*>(const_cast<uint16_t*>(" & "&" & Cells(row_counter, DATA_DEFINES_COL).text & "_MIN_VALUE)),")
            PROFIBUS_PRM_C.WriteLine ("        " & "reinterpret_cast<DCI_RANGE_DATA_TD*>(const_cast<uint16_t*>(" & "&" & Cells(row_counter, DATA_DEFINES_COL).text & "_MAX_VALUE)),")
            ElseIf (Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_TYPE_COL_DCI_TAB).text = "DCI_DTYPE_UINT8" Or Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_TYPE_COL_DCI_TAB).text = "DCI_DTYPE_BYTE") Then
            PROFIBUS_PRM_C.WriteLine ("        " & "reinterpret_cast<DCI_RANGE_DATA_TD*>(const_cast<uint8_t*>(" & "&" & Cells(row_counter, DATA_DEFINES_COL).text & "_MIN_VALUE)),")
            PROFIBUS_PRM_C.WriteLine ("        " & "reinterpret_cast<DCI_RANGE_DATA_TD*>(const_cast<uint8_t*>(" & "&" & Cells(row_counter, DATA_DEFINES_COL).text & "_MAX_VALUE)),")
            ElseIf (Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_TYPE_COL_DCI_TAB).text = "DCI_DTYPE_UINT32") Then
            PROFIBUS_PRM_C.WriteLine ("        " & "reinterpret_cast<DCI_RANGE_DATA_TD*>(const_cast<uint32_t*>(" & "&" & Cells(row_counter, DATA_DEFINES_COL).text & "_MIN_VALUE)),")
            PROFIBUS_PRM_C.WriteLine ("        " & "reinterpret_cast<DCI_RANGE_DATA_TD*>(const_cast<uint32_t*>(" & "&" & Cells(row_counter, DATA_DEFINES_COL).text & "_MAX_VALUE)),")
            ElseIf (Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_TYPE_COL_DCI_TAB).text = "DCI_DTYPE_BOOL") Then
            PROFIBUS_PRM_C.WriteLine ("        " & "reinterpret_cast<DCI_RANGE_DATA_TD*>(const_cast<bool_t*>(" & "&" & Cells(row_counter, DATA_DEFINES_COL).text & "_MIN_VALUE)),")
            PROFIBUS_PRM_C.WriteLine ("        " & "reinterpret_cast<DCI_RANGE_DATA_TD*>(const_cast<bool_t*>(" & "&" & Cells(row_counter, DATA_DEFINES_COL).text & "_MAX_VALUE)),")
            Else
            PROFIBUS_PRM_C.WriteLine ("        " & "reinterpret_cast<DCI_RANGE_DATA_TD*>(const_cast<int16_t*>(" & "&" & Cells(row_counter, DATA_DEFINES_COL).text & "_MIN_VALUE)),")
            PROFIBUS_PRM_C.WriteLine ("        " & "reinterpret_cast<DCI_RANGE_DATA_TD*>(const_cast<int16_t*>(" & "&" & Cells(row_counter, DATA_DEFINES_COL).text & "_MAX_VALUE)),")
            End If
            PROFIBUS_PRM_C.WriteLine ("    },")
        End If
        row_counter = row_counter + 1
    Wend
    PROFIBUS_PRM_C.WriteLine ("};")
    PROFIBUS_PRM_C.WriteLine ("")
    PROFIBUS_PRM_C.WriteLine ("const PROFIBUS_USR_PRM_LKUP_ST_TD*     " & "ProfiBus" & "_USR_PRM_LIST_PTR = &" & "ProfiBus" & "_usr_prm_lkup[0];")
    PROFIBUS_PRM_C.WriteLine ("")
    PROFIBUS_PRM_C.WriteLine ("")
    
    PROFIBUS_PRM_C.WriteLine ("/*")
    PROFIBUS_PRM_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_PRM_C.WriteLine ("*        Create " & "ProfiBus" & " User Parameter Structure")
    PROFIBUS_PRM_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_PRM_C.WriteLine ("*/")
    PROFIBUS_PRM_C.WriteLine ("")
    PROFIBUS_PRM_C.WriteLine ("const PROFI_USR_PRM_STRUCT     " & "ProfiBus" & "_USR_PRM_STRUCT = ")
    PROFIBUS_PRM_C.WriteLine ("{")
    PROFIBUS_PRM_C.WriteLine ("     const_cast<PROFIBUS_USR_PRM_LKUP_ST_TD*>(" & "ProfiBus" & "_USR_PRM_LIST_PTR),")
    PROFIBUS_PRM_C.WriteLine ("     " & "ProfiBus" & "_ONE_TIME_CHECK_IN_PARAMETER,")
    PROFIBUS_PRM_C.WriteLine ("     " & "ProfiBus" & "_MAX_USR_PRM_DATA_LEN,")
    PROFIBUS_PRM_C.WriteLine ("     " & "ProfiBus" & "_DOWNLOAD_USR_PRM_LEN,")
    PROFIBUS_PRM_C.WriteLine ("};")
    
    
'****************************************************************************************************************************
'******     End of Profibus User Prm lookup table.
'****************************************************************************************************************************
   
    
    Application.Calculation = xlCalculationAutomatic
End Sub
Sub Create_Diag_Data_Files()

    Application.Calculation = xlCalculationManual

'*********************
'******     Constants
'*********************
    
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    
    Dim MyDate
    MyDate = Date    ' MyDate contains the current system date.
    
    Dim DCI_STRING As String
    Dim num_unique_data As Integer
'    Dim even_reg_count As Boolean
'    Dim even_unique_reg_count As Boolean
    
    
    PROFI_START_ROW = 12
    PROFI_DESCRIPTORS_COL = 1
    PROFI_DEFINES_COL = 2
    PROFI_DATA_LEN_COL = 5
    PROFI_ID_COL = 6
    PROFI_MIN_COL = 5
    PROFI_MAX_COL = 6
    PROFI_DATA_TYPE_COL = 3
    PROFI_DATA_TYPE_LEN_COL = 4
    
    PROFI_CFG_MSB_ID_COL = 10
    PROFI_CFG_LSB_ID_COL = 11
    
    
    START_ROW = 12
    DESCRIP_COL = 1
    DATA_DEFINES_COL = 2
    DATA_PROFI_ID_COL = 6
    DATA_LEN_COL = 5
    DATA_MIN_COL = 5
    DATA_MAX_COL = 6
    DATA_TYPE_LEN_COL = 3
    DATA_TYPE_COL = 4
    DATA_USER_PARAM_COL = 10
    DATA_OUTPUT_COL = 11
    DATA_INPUT_COL = 12
    DATA_CFG_DEFAULT_COL = 13
    DATA_CFG_LEN_CODE_COL = 14
    DATA_CFG_MSB_ID_COL = 15
    DATA_CFG_LSB_ID_COL = 16
    
    DATA_DIAG_PARAM_COL = 18
    BIT_0_COL = 19
    BIT_1_COL = 20
    BIT_2_COL = 21
    BIT_3_COL = 22
    BIT_4_COL = 23
    BIT_5_COL = 24
    BIT_6_COL = 25
    BIT_7_COL = 26
    BIT_8_COL = 27
    BIT_9_COL = 28
    BIT_10_COL = 29
    BIT_11_COL = 30
    BIT_12_COL = 31
    BIT_13_COL = 32
    BIT_14_COL = 33
    BIT_15_COL = 34
    FAULT_MASK_DECIMAL_COL = 35
    FAULT_MASK_HEX_COL = 36

    
'*********************
'******     Begin
'*********************
    
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select
    

    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\" & Cells(3, 18).value
    Set PROFIBUS_DIAG_C = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(4, 18).value
    Set PROFIBUS_DIAG_H = fs.CreateTextFile(file_path, True)
   
'****************************************************************************************************************************
'******     Calculate Fault Masks for Diagnostic parameters
'****************************************************************************************************************************
counter = START_ROW

 While (Cells(counter, DATA_DIAG_PARAM_COL).text <> Empty)
        fault_mask = 0
        If (Cells(counter, BIT_0_COL).value = 1) Then
            fault_mask = fault_mask + 1
        End If
        If (Cells(counter, BIT_1_COL).value = 1) Then
            fault_mask = fault_mask + 2
        End If
        If (Cells(counter, BIT_2_COL).value = 1) Then
            fault_mask = fault_mask + 4
        End If
        If (Cells(counter, BIT_3_COL).value = 1) Then
            fault_mask = fault_mask + 8
        End If
        If (Cells(counter, BIT_4_COL).value = 1) Then
            fault_mask = fault_mask + 16
        End If
        If (Cells(counter, BIT_5_COL).value = 1) Then
            fault_mask = fault_mask + 32
        End If
        If (Cells(counter, BIT_6_COL).value = 1) Then
            fault_mask = fault_mask + 64
        End If
        If (Cells(counter, BIT_7_COL).value = 1) Then
            fault_mask = fault_mask + 128
        End If
        If (Cells(counter, BIT_8_COL).value = 1) Then
            fault_mask = fault_mask + 256
        End If
        If (Cells(counter, BIT_9_COL).value = 1) Then
            fault_mask = fault_mask + 512
        End If
        If (Cells(counter, BIT_10_COL).value = 1) Then
            fault_mask = fault_mask + 1024
        End If
        If (Cells(counter, BIT_11_COL).value = 1) Then
            fault_mask = fault_mask + 2048
        End If
        If (Cells(counter, BIT_12_COL).value = 1) Then
            fault_mask = fault_mask + 4096
        End If
        If (Cells(counter, BIT_13_COL).value = 1) Then
            fault_mask = fault_mask + 8192
        End If
        If (Cells(counter, BIT_14_COL).value = 1) Then
            fault_mask = fault_mask + 16384
        End If
        If (Cells(counter, BIT_15_COL).value = 1) Then
            fault_mask = fault_mask + 32768
        End If
        Cells(counter, FAULT_MASK_DECIMAL_COL) = fault_mask
        counter = counter + 1
        
    Wend
    
    
'****************************************************************************************************************************
'******     Start Creating the Diag_Data.H file header
'****************************************************************************************************************************
    PROFIBUS_DIAG_H.WriteLine ("/**")
    PROFIBUS_DIAG_H.WriteLine ("*****************************************************************************************")
    PROFIBUS_DIAG_H.WriteLine ("*   @file")
    PROFIBUS_DIAG_H.WriteLine ("*")
    PROFIBUS_DIAG_H.WriteLine ("*   @brief")
    PROFIBUS_DIAG_H.WriteLine ("*")
    PROFIBUS_DIAG_H.WriteLine ("*   @details")
    PROFIBUS_DIAG_H.WriteLine ("*")
    PROFIBUS_DIAG_H.WriteLine ("*   @version")
    PROFIBUS_DIAG_H.WriteLine ("*   C++ Style Guide Version 1.0")
    PROFIBUS_DIAG_H.WriteLine ("*")
    PROFIBUS_DIAG_H.WriteLine ("*   @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    PROFIBUS_DIAG_H.WriteLine ("*")
    PROFIBUS_DIAG_H.WriteLine ("*****************************************************************************************")
    PROFIBUS_DIAG_H.WriteLine ("*/")
    PROFIBUS_DIAG_H.WriteLine ("#ifndef " & Cells(2, 19).text & "_H")
    PROFIBUS_DIAG_H.WriteLine ("  #define " & Cells(2, 19).text & "_H")
    PROFIBUS_DIAG_H.WriteLine ("")
    PROFIBUS_DIAG_H.WriteLine ("#include ""DCI.h""")
    PROFIBUS_DIAG_H.WriteLine ("#include ""DCI_Defines.h""")
    PROFIBUS_DIAG_H.WriteLine ("#include ""DCI_Profibus_Data.h""")
    PROFIBUS_DIAG_H.WriteLine ("#include ""ProfiBus_Diagnostics.h""")
    PROFIBUS_DIAG_H.WriteLine ("")
    PROFIBUS_DIAG_H.WriteLine ("")
    PROFIBUS_DIAG_H.WriteLine ("//******************************************************")
    PROFIBUS_DIAG_H.WriteLine ("//*****  " & Cells(1, 7).text & " Diagnostic Defines")
    PROFIBUS_DIAG_H.WriteLine ("//******************************************************")
    PROFIBUS_DIAG_H.WriteLine ("")

'***************************************************************************************************************************
'******      Determine Length of diagnostic parameters to determine the size of diag_data array
'***************************************************************************************************************************
    counter = START_ROW
    diag_len = 0
    num_params = 0
    While (Cells(counter, DATA_DIAG_PARAM_COL).text <> Empty)
        num_params = num_params + 1
        row_counter = START_ROW
        While (Cells(row_counter, DATA_DEFINES_COL).text <> BEGIN_IGNORED_DATA)
            If (Cells(counter, DATA_DIAG_PARAM_COL).text = Cells(row_counter, DATA_DEFINES_COL).text) Then
                diag_len = (diag_len + Cells(row_counter, DATA_LEN_COL).value)
            End If
        row_counter = row_counter + 1
        Wend
        
        counter = counter + 1
    Wend
    
     PROFIBUS_DIAG_H.WriteLine ("#define    " & Cells(1, 7).text & "_USER_DIAG_LEN       " & diag_len)
     PROFIBUS_DIAG_H.WriteLine ("#define    " & Cells(1, 7).text & "_NUM_DIAG_PARAMS       " & num_params)
     PROFIBUS_DIAG_H.WriteLine ("#define    " & Cells(1, 7).text & "_TOTAL_USER_DIAG_LEN       " & _
                                "( " & Cells(1, 7).text & "_USER_DIAG_LEN + DIAG_HEADER_LEN )")
     
     
'***************************************************************************************************************************
'******      Write defines for FAULT MASKs
'***************************************************************************************************************************
    counter = START_ROW
    While (Cells(counter, DATA_DIAG_PARAM_COL).text <> Empty)
        PROFIBUS_DIAG_H.WriteLine ("#define    " & Cells(counter, DATA_DIAG_PARAM_COL).text & "_MASK         " & _
                                    "((SPLIT_UINT16)0x" & Cells(counter, FAULT_MASK_HEX_COL).text & ")")
        counter = counter + 1
    Wend
    
    
    
    
    
    PROFIBUS_DIAG_H.Close
    PROFIBUS_DIAG_C.Close

    Application.Calculation = xlCalculationAutomatic
End Sub
Public Function Get_Datatype_String(my_s As String) As String

    Application.Calculation = xlCalculationManual
    
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

    Application.Calculation = xlCalculationAutomatic
End Function







Sub Generate_Profi_html()

'*************************************************************************************************************
 '*************************************************************************************************************
    'Generate the .lua file & .html files for Profibus Parameter List
    '*************************************************************************************************************
    '*********************
    '******     Begin
    '*********************
    GENERATED_DOCS_PATH_ROW = 26
    GENERATED_DOCS_PATH_COL = 2
    START_ROW = 12
    DATA_DEFINE_COL = 2
    DATA_LENGTH_COL = 5
    USER_PARAM_COL = 10
    MODULE_PARAM_COL = 11
    DATA_DESC_COL = 1
    DCI_DEFINE_COL = 3
    DATA_LEN_COL = 4
    MIN_VAL_COL = 5
    MAX_VAL_COL = 6
    DATA_TYPE_COL = 3
    DATA_TYPE_LEN_COL = 4
    DCI_LONG_DESC_COL = 1
    
    DATA_OUT_LEN = 12
    DATA_IN_LEN = 13
    CFG_DEFAULT_LEN = 14
    CFG_OP_LEN = 15
    CFG_IP_LEN = 16
    MSB_ID_LEN = 17
    LSB_ID_LEN = 18

   
         
    file_name = "Profibus Requirements Data"
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\"
    file_path = file_path & Worksheets("Usage Notes").Range("A1").Cells(GENERATED_DOCS_PATH_ROW, GENERATED_DOCS_PATH_COL).text
       
    Set GENED_REQ_DOC = fs.CreateTextFile(file_path & file_name & ".html", True)
    Set GENED_LUA_DOC = fs.CreateTextFile(file_path & file_name & ".lua", True)
    
    '****************************************************************************************************************************
    '******     Macros to generate the Requirements in an HTML Format
    '****************************************************************************************************************************
    
    '*****************************************************
    '******     HTML Cell Column Width and alignment
    '*****************************************************
    HTML_ID_WIDTH = """5%"""
    HTML_DCID_WIDTH = """10%"""
    HTML_DESC_WIDTH = """20%"""
    HTML_DEFAULT_WIDTH = """10%"""
    HTML_ENUM_WIDTH = """5%"""
    HTML_RANGE_WIDTH = """5%"""
    HTML_INIT_WIDTH = """5%"""
    HTML_MEM_WIDTH = """5%"""
    
    '****************************************************************************************************************************
    '******     Create the header information for the HTML file.
    '****************************************************************************************************************************
    GENED_REQ_DOC.WriteLine ("<html><body>" & vbNewLine & "<style>")
    GENED_REQ_DOC.WriteLine (vbNewLine & "body" & vbNewLine & "{" & vbNewLine & "background-color:White;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "h1" & vbNewLine & "{" & vbNewLine & "color:Navy;" & vbNewLine & "text-align:center;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "h2" & vbNewLine & "{" & vbNewLine & "color:Black;" & vbNewLine & "text-align:left;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "p" & vbNewLine & "{" & vbNewLine & "font-size:12px" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "p1" & vbNewLine & "{" & vbNewLine & "color:Navy;" & vbNewLine & "text-align:left;" & "font-size:16px" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "</style>" & vbNewLine & "</head>" & vbNewLine & "<body>")
    GENED_REQ_DOC.WriteLine ("<h2>File Name     : " & file_name & "</h2>")
    GENED_REQ_DOC.WriteLine ("<p1>Description    : Profibus Parameters List auto-generated from DCI DB Creator Macros.xls Sheet</p1>")
    GENED_REQ_DOC.WriteLine ("<br><p1>Created on     : " & Date & " at " & Time & "</p1><br>")
    file_name = "User Parameter Data"
    GENED_REQ_DOC.WriteLine ("<h2>Table Name     : " & file_name & "</h2>")

    '*************************************************************************************************************************
    
    '****************************************************************************************************************************
    '******     Start Creating the HTML body surrounding the table.
    '****************************************************************************************************************************
    GENED_REQ_DOC.WriteLine ("")
    GENED_REQ_DOC.WriteLine ("<table class=""reference"" cellspacing=""0"" cellpadding=""2"" border=""1"" width=""100%"" style=""font-family:arial"">")
    GENED_REQ_DOC.WriteLine (vbTab & "<tr>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">Sr.No</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_DESC_WIDTH & "align = ""center"" valign = ""top"">Description</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">DCI_Description</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_DCID_WIDTH & "align = ""center"" valign = ""top"">Profi_ID</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">Data_Length</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_RANGE_WIDTH & "align = ""center"" valign = ""top"">MinValue</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_RANGE_WIDTH & "align = ""center"" valign = ""top"">MaxValue</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">DataTypeLength</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_DEFAULT_WIDTH & "align = ""center"" valign = ""top"">DataType</th>")
    
    
    'GENED_REQ_DOC.Writeline (vbTab & vbTab & "<th width=" & HTML_ENUM_WIDTH & "align = ""center"" valign = ""top"">Enum Values</th>")
    'GENED_REQ_DOC.Writeline (vbTab & vbTab & "<th width=" & HTML_INIT_WIDTH & "align = ""center"" valign = ""top"">Init support</th>")
    'GENED_REQ_DOC.Writeline (vbTab & vbTab & "<th width=" & HTML_INIT_WIDTH & "align = ""center"" valign = ""top"">Default support</th>")
    'GENED_REQ_DOC.Writeline (vbTab & vbTab & "<th width=" & HTML_MEM_WIDTH & "align = ""center"" valign = ""top"">Access</th>")
    'GENED_REQ_DOC.Writeline (vbTab & "</tr>")
    
    '****************************************************************************************************************************
    '******     Start Creating the LUA output file.
    '****************************************************************************************************************************
    GENED_LUA_DOC.WriteLine ("--**************************************************************************")
    GENED_LUA_DOC.WriteLine ("--*    Excel Generated LUA File for BUI Parameter Reference.")
    GENED_LUA_DOC.WriteLine ("--**************************************************************************")
    GENED_LUA_DOC.WriteLine ("")
    GENED_LUA_DOC.WriteLine ("")
    
    '****************************************************************************************************************************
    '******     Build the table that will list all the data related the Profibus Parameters
    '****************************************************************************************************************************
    
    Dim profi_search_string As String
    Dim profi_param As String
    Dim compare_string As String
    Dim reg_addr As Long
    Dim coil_size
    'Dim generate_doc As Boolean
    Dim string_len As Integer
    Dim init_val As String
    Dim default
    Dim Access
    Dim Enum_val
    Dim start_range
    Dim end_range
    Dim mem
    Dim DESC_DEFAULT_COL
    Dim key
    
    compare_string = "x"
    str_len = Len(compare_string)
    
    counter = START_ROW
    req_counter = 0
    'GENED_LUA_DOC.Writeline (Spacify("BUI_REGISTERS = {", 17))
   ' While (Cells(counter, USER_PARAM_COL).Text <> Empty) And (Cells(counter, DATA_DEFINE_COL).Text <> BEGIN_IGNORED_DATA)
     While (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)

        profi_param = Cells(counter, USER_PARAM_COL).text
        'profi_search_string = Mid(profi_param, 1, str_len)
        profi_search_string = Cells(counter, USER_PARAM_COL).text

        
        If (profi_search_string = compare_string) Then
            string_len = Len(profi_param)
            desc_sheet_row = Find_DCID_Row(Cells(counter, DATA_DEFINE_COL).text)
    
            ' Descriptor Address
            DESCRIPTION = Cells(counter, DATA_DESC_COL).text
      
            ' DCI_define
                   
            DCI_define = Cells(counter, DATA_DEFINE_COL).text
         
            'Profi_DCI_ID
            
            Profi_DCI_ID = Cells(counter, DCI_DEFINE_COL).text
            
            'DATA LENGTH Column
            
            Data_Length = Cells(counter, DATA_LEN_COL).value
            
            'Min Value Column
            
            min_value = Cells(counter, MIN_VAL_COL).value
            
            'MAX Value Column
            
            max_value = Cells(counter, MAX_VAL_COL).value
            
            'DATA TYPE LEN  column
             DATA_TYPE_LEN = Cells(counter, DATA_TYPE_COL).value

            ' Data Type
            
              data_type = Cells(counter, DATA_TYPE_LEN_COL).value

                  
           
           ' key = key + 1
            
              '          GENED_LUA_DOC.Writeline (Spacify(" ", 16) & "[" & key & "] = {[""PARAM_NAME""] = " & """" & Spacify((Cells(counter, DATA_DEFINE_COL).Text), string_len) _
                 '           & """, " & Spacify("[""ADDR""] =", 8) & reg_addr & Spacify(", [""DEFAULT""] =", 15) & "{" & default & "}" & _
                         '   Spacify(", [""LENGTH""] =", 13) & integer_convert & ", [""ACCESS""] = """ & Access & """")
           ' GENED_LUA_DOC.Writeline (Spacify(" ", 16) & Spacify(", [""ENUM""] =", 12) & "{" & Enum_val & "}" & Spacify(", [""START_RANGE""] =", 15) _
                            & start_range & Spacify(", [""END_RANGE""] =", 15) & end_range & ", [""INIT_VAL_SUPP""] = """ & init_val & """, [""DEFAULT_SUPP""] = """ & def_supp & """},")
           ' GENED_LUA_DOC.Writeline ("")



           ' GENED_LUA_DOC.Writeline (Spacify(" ", 16) & "[" & key & "] = {[""PARAM_NAME""] = " & """" & Spacify((Cells(counter, DATA_DEFINE_COL).Text), string_len) _
                            & """, " & Spacify("[""ADDR""] =", 8) & reg_addr & Spacify(", [""DEFAULT""] =", 15) & "{" & default & "}" & _
            '                Spacify(", [""LENGTH""] =", 13) & integer_convert & ", [""ACCESS""] = """ & Access & """")
           ' GENED_LUA_DOC.Writeline (Spacify(" ", 16) & Spacify(", [""ENUM""] =", 12) & "{" & Enum_val & "}" & Spacify(", [""START_RANGE""] =", 15) _
          '                  & start_range & Spacify(", [""END_RANGE""] =", 15) & end_range & ", [""INIT_VAL_SUPP""] = """ & init_val & """, [""DEFAULT_SUPP""] = """ & def_supp & """},")
          '  GENED_LUA_DOC.Writeline ("")
                         
                         
            '****** Start the HTML version for the requirements document
            req_counter = req_counter + 1
            
            If (default = "nil") Then
                default = "--"
            End If
            If (Enum_val = "nil") Then
                Enum_val = "--"
            End If
            If (start_range = "nil") Then
                start_range = "--"
            End If
            If (end_range = "nil") Then
                end_range = "--"
            End If
                        
            'DCID Description column in HTML File
            If (Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_LONG_DESC_COL).text <> Empty) Then
                dcid_desc = Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_LONG_DESC_COL).text
            Else
                dcid_desc = "--"
            End If
            
            GENED_REQ_DOC.WriteLine (vbTab & "<tr>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""left"" valign=""top""><p>" & req_counter & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""left"" valign=""top""><p>" & DESCRIPTION & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_DCID_WIDTH & "align=""left"" valign=""top""><p>" & DCI_define & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_DCID_WIDTH & "align=""left"" valign=""top""><p>" & Profi_DCI_ID & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""left"" valign=""top""><p>" & Data_Length & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_DEFAULT_WIDTH & "align=""left"" valign=""top""><p>" & min_value & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ENUM_WIDTH & "align=""left"" valign=""top""><p>" & max_value & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_RANGE_WIDTH & "align=""left"" valign=""top""><p>" & DATA_TYPE_LEN & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_RANGE_WIDTH & "align=""left"" valign=""top""><p>" & data_type & "</p></td>")
'            GENED_REQ_DOC.Writeline (vbTab & vbTab & "<td width=" & HTML_INIT_WIDTH & "align=""left"" valign=""top""><p>" & init_val & "</p></td>")
'            GENED_REQ_DOC.Writeline (vbTab & vbTab & "<td width=" & HTML_INIT_WIDTH & "align=""left"" valign=""top""><p>" & def_supp & "</p></td>")
'            GENED_REQ_DOC.Writeline (vbTab & vbTab & "<td width=" & HTML_MEM_WIDTH & "align=""left"" valign=""top""><p>" & Access & "</p></td>")
        End If
       counter = counter + 1
    Wend
'    GENED_LUA_DOC.Writeline (Spacify("", 12) & "}")
'    GENED_LUA_DOC.Writeline (vbNewLine & "TOTAL_PARAM = " & key)
    GENED_REQ_DOC.WriteLine ("</table>")
    GENED_REQ_DOC.WriteLine ("")
    
    
    
    '****************************************************************************************************************************
    '******     Create the header information for the HTML file.
    '****************************************************************************************************************************
    GENED_REQ_DOC.WriteLine ("<html><body>" & vbNewLine & "<style>")
    GENED_REQ_DOC.WriteLine (vbNewLine & "body" & vbNewLine & "{" & vbNewLine & "background-color:White;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "h1" & vbNewLine & "{" & vbNewLine & "color:Navy;" & vbNewLine & "text-align:center;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "h2" & vbNewLine & "{" & vbNewLine & "color:Black;" & vbNewLine & "text-align:left;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "p" & vbNewLine & "{" & vbNewLine & "font-size:12px" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "p1" & vbNewLine & "{" & vbNewLine & "color:Navy;" & vbNewLine & "text-align:left;" & "font-size:16px" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "</style>" & vbNewLine & "</head>" & vbNewLine & "<body>")
    
    file_name = "Configuration Parameter Data"
    GENED_REQ_DOC.WriteLine ("<h2>Table Name     : " & file_name & "</h2>")

    '*************************************************************************************************************************
    
    '****************************************************************************************************************************
    '******     Start Creating the HTML body surrounding the table.
    '****************************************************************************************************************************
    GENED_REQ_DOC.WriteLine ("")
    GENED_REQ_DOC.WriteLine ("<table class=""reference"" cellspacing=""0"" cellpadding=""2"" border=""1"" width=""100%"" style=""font-family:arial"">")
    GENED_REQ_DOC.WriteLine (vbTab & "<tr>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">Sr.No</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">Description</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">DCI_Description</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_DCID_WIDTH & "align = ""center"" valign = ""top"">Profi_ID</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">Data_Length</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_RANGE_WIDTH & "align = ""center"" valign = ""top"">MinValue</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_RANGE_WIDTH & "align = ""center"" valign = ""top"">MaxValue</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">DataTypeLength</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_DCID_WIDTH & "align = ""center"" valign = ""top"">DataType</th>")
    
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_DCID_WIDTH & "align = ""center"" valign = ""top"">Out</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">In</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_RANGE_WIDTH & "align = ""center"" valign = ""top"">CFG default</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_RANGE_WIDTH & "align = ""center"" valign = ""top"">CFG OP LEN CODE</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">CFG IP LEN CODE</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_DCID_WIDTH & "align = ""center"" valign = ""top"">MSB ID</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_DCID_WIDTH & "align = ""center"" valign = ""top"">LSB ID</th>")

      
    '****************************************************************************************************************************
    '******     Start Creating the LUA output file.
    '****************************************************************************************************************************
    GENED_LUA_DOC.WriteLine ("--**************************************************************************")
    GENED_LUA_DOC.WriteLine ("--*    Excel Generated LUA File for BUI Parameter Reference.")
    GENED_LUA_DOC.WriteLine ("--**************************************************************************")
    GENED_LUA_DOC.WriteLine ("")
    GENED_LUA_DOC.WriteLine ("")
    
    '****************************************************************************************************************************
    '******     Build the table that will list all the data related the Profibus Parameters
    '****************************************************************************************************************************
    
   
    
    compare_string = "x"
    str_len = Len(compare_string)
    
    counter = START_ROW
    req_counter = 0
    'GENED_LUA_DOC.Writeline (Spacify("BUI_REGISTERS = {", 17))
   ' While (Cells(counter, USER_PARAM_COL).Text <> Empty) And (Cells(counter, DATA_DEFINE_COL).Text <> BEGIN_IGNORED_DATA)
     While (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)

        profi_param = Cells(counter, MODULE_PARAM_COL).text
        'profi_search_string = Mid(profi_param, 1, str_len)
        profi_search_string = Cells(counter, DATA_OUT_LEN).text

        
        If (profi_search_string = compare_string) Then
            string_len = Len(profi_param)
            desc_sheet_row = Find_DCID_Row(Cells(counter, DATA_DEFINE_COL).text)
    
            ' Descriptor Address
            DESCRIPTION = Cells(counter, DATA_DESC_COL).text
      
            ' DCI_define
                   
            DCI_define = Cells(counter, DATA_DEFINE_COL).text
         
            'Profi_DCI_ID
            
            Profi_DCI_ID = Cells(counter, DCI_DEFINE_COL).text
            
            'DATA LENGTH Column
            
            Data_Length = Cells(counter, DATA_LEN_COL).value
            
            'Min Value Column
            
            min_value = Cells(counter, MIN_VAL_COL).value
            
            'MAX Value Column
            
            max_value = Cells(counter, MAX_VAL_COL).value
            
            'DATA TYPE LEN  column
             DATA_TYPE_LEN = Cells(counter, DATA_TYPE_COL).value

            ' Data Type
            
            data_type = Cells(counter, DATA_TYPE_LEN_COL).value
                            

            'Out
             OUT_value = Cells(counter, DATA_OUT_LEN).value
          
            'In
             IN_value = Cells(counter, DATA_IN_LEN).value
             
            'CFG default
            CFG_DEFAULT_value = Cells(counter, CFG_DEFAULT_LEN).value
            
            'CFG OP LEN CODE
            CFG_OP_value = Cells(counter, CFG_OP_LEN).value
            
            'CFG IP LEN CODE
             CFG_IP_value = Cells(counter, CFG_IP_LEN).value
             
            'MSB ID
            MSB_ID_value = Cells(counter, MSB_ID_LEN).value
                             
            'LSB ID
            LSB_ID_value = Cells(counter, LSB_ID_LEN).value


        ' key = key + 1
            
              '          GENED_LUA_DOC.Writeline (Spacify(" ", 16) & "[" & key & "] = {[""PARAM_NAME""] = " & """" & Spacify((Cells(counter, DATA_DEFINE_COL).Text), string_len) _
                 '           & """, " & Spacify("[""ADDR""] =", 8) & reg_addr & Spacify(", [""DEFAULT""] =", 15) & "{" & default & "}" & _
                         '   Spacify(", [""LENGTH""] =", 13) & integer_convert & ", [""ACCESS""] = """ & Access & """")
           ' GENED_LUA_DOC.Writeline (Spacify(" ", 16) & Spacify(", [""ENUM""] =", 12) & "{" & Enum_val & "}" & Spacify(", [""START_RANGE""] =", 15) _
                            & start_range & Spacify(", [""END_RANGE""] =", 15) & end_range & ", [""INIT_VAL_SUPP""] = """ & init_val & """, [""DEFAULT_SUPP""] = """ & def_supp & """},")
           ' GENED_LUA_DOC.Writeline ("")



           ' GENED_LUA_DOC.Writeline (Spacify(" ", 16) & "[" & key & "] = {[""PARAM_NAME""] = " & """" & Spacify((Cells(counter, DATA_DEFINE_COL).Text), string_len) _
                            & """, " & Spacify("[""ADDR""] =", 8) & reg_addr & Spacify(", [""DEFAULT""] =", 15) & "{" & default & "}" & _
            '                Spacify(", [""LENGTH""] =", 13) & integer_convert & ", [""ACCESS""] = """ & Access & """")
           ' GENED_LUA_DOC.Writeline (Spacify(" ", 16) & Spacify(", [""ENUM""] =", 12) & "{" & Enum_val & "}" & Spacify(", [""START_RANGE""] =", 15) _
          '                  & start_range & Spacify(", [""END_RANGE""] =", 15) & end_range & ", [""INIT_VAL_SUPP""] = """ & init_val & """, [""DEFAULT_SUPP""] = """ & def_supp & """},")
          '  GENED_LUA_DOC.Writeline ("")
                         
                         
            '****** Start the HTML version for the requirements document
            req_counter = req_counter + 1
            
            If (default = "nil") Then
                default = "--"
            End If
            If (Enum_val = "nil") Then
                Enum_val = "--"
            End If
            If (start_range = "nil") Then
                start_range = "--"
            End If
            If (end_range = "nil") Then
                end_range = "--"
            End If
                        
            'DCID Description column in HTML File
            If (Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_LONG_DESC_COL).text <> Empty) Then
                dcid_desc = Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_LONG_DESC_COL).text
            Else
                dcid_desc = "--"
            End If
            
            GENED_REQ_DOC.WriteLine (vbTab & "<tr>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""left"" valign=""top""><p>" & req_counter & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""left"" valign=""top""><p>" & DESCRIPTION & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_DCID_WIDTH & "align=""left"" valign=""top""><p>" & DCI_define & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_DCID_WIDTH & "align=""left"" valign=""top""><p>" & Profi_DCI_ID & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""left"" valign=""top""><p>" & Data_Length & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_DEFAULT_WIDTH & "align=""left"" valign=""top""><p>" & min_value & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ENUM_WIDTH & "align=""left"" valign=""top""><p>" & max_value & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_RANGE_WIDTH & "align=""left"" valign=""top""><p>" & DATA_TYPE_LEN & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_RANGE_WIDTH & "align=""left"" valign=""top""><p>" & data_type & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_INIT_WIDTH & "align=""left"" valign=""top""><p>" & OUT_value & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_INIT_WIDTH & "align=""left"" valign=""top""><p>" & IN_value & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_MEM_WIDTH & "align=""left"" valign=""top""><p>" & CFG_DEFAULT_value & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_RANGE_WIDTH & "align=""left"" valign=""top""><p>" & CFG_OP_value & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_INIT_WIDTH & "align=""left"" valign=""top""><p>" & CFG_IP_value & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_INIT_WIDTH & "align=""left"" valign=""top""><p>" & MSB_ID_value & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_MEM_WIDTH & "align=""left"" valign=""top""><p>" & LSB_ID_value & "</p></td>")

                    
        End If
       counter = counter + 1
    Wend
'    GENED_LUA_DOC.Writeline (Spacify("", 12) & "}")
'    GENED_LUA_DOC.Writeline (vbNewLine & "TOTAL_PARAM = " & key)
    GENED_REQ_DOC.WriteLine ("</table>")
    GENED_REQ_DOC.WriteLine ("")
    
    '****************************************************************************************************************************
    '******     Build the table that will list all the data related the Module Parameters
    '****************************************************************************************************************************
    
      '****************************************************************************************************************************
    '******     Create the header information for the HTML file.
    '****************************************************************************************************************************
    GENED_REQ_DOC.WriteLine ("<html><body>" & vbNewLine & "<style>")
    GENED_REQ_DOC.WriteLine (vbNewLine & "body" & vbNewLine & "{" & vbNewLine & "background-color:White;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "h1" & vbNewLine & "{" & vbNewLine & "color:Navy;" & vbNewLine & "text-align:center;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "h2" & vbNewLine & "{" & vbNewLine & "color:Black;" & vbNewLine & "text-align:left;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "p" & vbNewLine & "{" & vbNewLine & "font-size:12px" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "p1" & vbNewLine & "{" & vbNewLine & "color:Navy;" & vbNewLine & "text-align:left;" & "font-size:16px" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "</style>" & vbNewLine & "</head>" & vbNewLine & "<body>")
    file_name = "Module Parameter Data"
    GENED_REQ_DOC.WriteLine ("<h2>Table Name     : " & file_name & "</h2>")

    '*************************************************************************************************************************
    
    '****************************************************************************************************************************
    '******     Start Creating the HTML body surrounding the table.
    '****************************************************************************************************************************
    GENED_REQ_DOC.WriteLine ("")
    GENED_REQ_DOC.WriteLine ("<table class=""reference"" cellspacing=""0"" cellpadding=""2"" border=""1"" width=""100%"" style=""font-family:arial"">")
    GENED_REQ_DOC.WriteLine (vbTab & "<tr>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">Sr.No</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">Description</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">DCI_Description</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_DCID_WIDTH & "align = ""center"" valign = ""top"">Profi_ID</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_DCID_WIDTH & "align = ""center"" valign = ""top"">Data_Length</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_RANGE_WIDTH & "align = ""center"" valign = ""top"">MinValue</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_RANGE_WIDTH & "align = ""center"" valign = ""top"">MaxValue</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">DataTypeLength</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_DEFAULT_WIDTH & "align = ""center"" valign = ""top"">DataType</th>")
    
    
    'GENED_REQ_DOC.Writeline (vbTab & vbTab & "<th width=" & HTML_ENUM_WIDTH & "align = ""center"" valign = ""top"">Enum Values</th>")
    'GENED_REQ_DOC.Writeline (vbTab & vbTab & "<th width=" & HTML_INIT_WIDTH & "align = ""center"" valign = ""top"">Init support</th>")
    'GENED_REQ_DOC.Writeline (vbTab & vbTab & "<th width=" & HTML_INIT_WIDTH & "align = ""center"" valign = ""top"">Default support</th>")
    'GENED_REQ_DOC.Writeline (vbTab & vbTab & "<th width=" & HTML_MEM_WIDTH & "align = ""center"" valign = ""top"">Access</th>")
    'GENED_REQ_DOC.Writeline (vbTab & "</tr>")
    
    '****************************************************************************************************************************
    '******     Start Creating the LUA output file.
    '****************************************************************************************************************************
    GENED_LUA_DOC.WriteLine ("--**************************************************************************")
    GENED_LUA_DOC.WriteLine ("--*    Excel Generated LUA File for BUI Parameter Reference.")
    GENED_LUA_DOC.WriteLine ("--**************************************************************************")
    GENED_LUA_DOC.WriteLine ("")
    GENED_LUA_DOC.WriteLine ("")
    
    '****************************************************************************************************************************
    '******     Build the table that will list all the data related the Profibus Parameters
    '****************************************************************************************************************************
    
 
    
    compare_string = "x"
    str_len = Len(compare_string)
    
    counter = START_ROW
    req_counter = 0
    'GENED_LUA_DOC.Writeline (Spacify("BUI_REGISTERS = {", 17))
   ' While (Cells(counter, USER_PARAM_COL).Text <> Empty) And (Cells(counter, DATA_DEFINE_COL).Text <> BEGIN_IGNORED_DATA)
     While (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)

        profi_param = Cells(counter, MODULE_PARAM_COL).text
        'profi_search_string = Mid(profi_param, 1, str_len)
        profi_search_string = Cells(counter, MODULE_PARAM_COL).text

        
        If (profi_search_string = compare_string) Then
            string_len = Len(profi_param)
            desc_sheet_row = Find_DCID_Row(Cells(counter, DATA_DEFINE_COL).text)
    
            ' Descriptor Address
            DESCRIPTION = Cells(counter, DATA_DESC_COL).text
      
            ' DCI_define
                   
            DCI_define = Cells(counter, DATA_DEFINE_COL).text
         
            'Profi_DCI_ID
            
            Profi_DCI_ID = Cells(counter, DCI_DEFINE_COL).text
            
            'DATA LENGTH Column
            
            Data_Length = Cells(counter, DATA_LEN_COL).value
            
            'Min Value Column
            
            min_value = Cells(counter, MIN_VAL_COL).value
            
            'MAX Value Column
            
            max_value = Cells(counter, MAX_VAL_COL).value
            
            'DATA TYPE LEN  column
             DATA_TYPE_LEN = Cells(counter, DATA_TYPE_COL).value

            ' Data Type
            
              data_type = Cells(counter, DATA_TYPE_LEN_COL).value

                  
           
           ' key = key + 1
            
              '          GENED_LUA_DOC.Writeline (Spacify(" ", 16) & "[" & key & "] = {[""PARAM_NAME""] = " & """" & Spacify((Cells(counter, DATA_DEFINE_COL).Text), string_len) _
                 '           & """, " & Spacify("[""ADDR""] =", 8) & reg_addr & Spacify(", [""DEFAULT""] =", 15) & "{" & default & "}" & _
                         '   Spacify(", [""LENGTH""] =", 13) & integer_convert & ", [""ACCESS""] = """ & Access & """")
           ' GENED_LUA_DOC.Writeline (Spacify(" ", 16) & Spacify(", [""ENUM""] =", 12) & "{" & Enum_val & "}" & Spacify(", [""START_RANGE""] =", 15) _
                            & start_range & Spacify(", [""END_RANGE""] =", 15) & end_range & ", [""INIT_VAL_SUPP""] = """ & init_val & """, [""DEFAULT_SUPP""] = """ & def_supp & """},")
           ' GENED_LUA_DOC.Writeline ("")



           ' GENED_LUA_DOC.Writeline (Spacify(" ", 16) & "[" & key & "] = {[""PARAM_NAME""] = " & """" & Spacify((Cells(counter, DATA_DEFINE_COL).Text), string_len) _
                            & """, " & Spacify("[""ADDR""] =", 8) & reg_addr & Spacify(", [""DEFAULT""] =", 15) & "{" & default & "}" & _
            '                Spacify(", [""LENGTH""] =", 13) & integer_convert & ", [""ACCESS""] = """ & Access & """")
           ' GENED_LUA_DOC.Writeline (Spacify(" ", 16) & Spacify(", [""ENUM""] =", 12) & "{" & Enum_val & "}" & Spacify(", [""START_RANGE""] =", 15) _
          '                  & start_range & Spacify(", [""END_RANGE""] =", 15) & end_range & ", [""INIT_VAL_SUPP""] = """ & init_val & """, [""DEFAULT_SUPP""] = """ & def_supp & """},")
          '  GENED_LUA_DOC.Writeline ("")
                         
                         
            '****** Start the HTML version for the requirements document
            req_counter = req_counter + 1
            
            If (default = "nil") Then
                default = "--"
            End If
            If (Enum_val = "nil") Then
                Enum_val = "--"
            End If
            If (start_range = "nil") Then
                start_range = "--"
            End If
            If (end_range = "nil") Then
                end_range = "--"
            End If
                        
            'DCID Description column in HTML File
            If (Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_LONG_DESC_COL).text <> Empty) Then
                dcid_desc = Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_LONG_DESC_COL).text
            Else
                dcid_desc = "--"
            End If
            
            GENED_REQ_DOC.WriteLine (vbTab & "<tr>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""left"" valign=""top""><p>" & req_counter & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""left"" valign=""top""><p>" & DESCRIPTION & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_DCID_WIDTH & "align=""left"" valign=""top""><p>" & DCI_define & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_DCID_WIDTH & "align=""left"" valign=""top""><p>" & Profi_DCI_ID & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""left"" valign=""top""><p>" & Data_Length & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_DEFAULT_WIDTH & "align=""left"" valign=""top""><p>" & min_value & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ENUM_WIDTH & "align=""left"" valign=""top""><p>" & max_value & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_RANGE_WIDTH & "align=""left"" valign=""top""><p>" & DATA_TYPE_LEN & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_RANGE_WIDTH & "align=""left"" valign=""top""><p>" & data_type & "</p></td>")
'            GENED_REQ_DOC.Writeline (vbTab & vbTab & "<td width=" & HTML_INIT_WIDTH & "align=""left"" valign=""top""><p>" & init_val & "</p></td>")
'            GENED_REQ_DOC.Writeline (vbTab & vbTab & "<td width=" & HTML_INIT_WIDTH & "align=""left"" valign=""top""><p>" & def_supp & "</p></td>")
'            GENED_REQ_DOC.Writeline (vbTab & vbTab & "<td width=" & HTML_MEM_WIDTH & "align=""left"" valign=""top""><p>" & Access & "</p></td>")
        End If
       counter = counter + 1
    Wend
'    GENED_LUA_DOC.Writeline (Spacify("", 12) & "}")
'    GENED_LUA_DOC.Writeline (vbNewLine & "TOTAL_PARAM = " & key)
    GENED_REQ_DOC.WriteLine ("</table>")
    GENED_REQ_DOC.WriteLine ("")


    '****************************************************************************************************************************
    '******     Build the table that will list all the data related the Diagnostics Parameters
    '****************************************************************************************************************************
    
      '****************************************************************************************************************************
    '******     Create the header information for the HTML file.
    '****************************************************************************************************************************
    GENED_REQ_DOC.WriteLine ("<html><body>" & vbNewLine & "<style>")
    GENED_REQ_DOC.WriteLine (vbNewLine & "body" & vbNewLine & "{" & vbNewLine & "background-color:White;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "h1" & vbNewLine & "{" & vbNewLine & "color:Navy;" & vbNewLine & "text-align:center;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "h2" & vbNewLine & "{" & vbNewLine & "color:Black;" & vbNewLine & "text-align:left;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "p" & vbNewLine & "{" & vbNewLine & "font-size:12px" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "p1" & vbNewLine & "{" & vbNewLine & "color:Navy;" & vbNewLine & "text-align:left;" & "font-size:16px" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "</style>" & vbNewLine & "</head>" & vbNewLine & "<body>")
    file_name = "Diagnostics Parameter Data"
    GENED_REQ_DOC.WriteLine ("<h2>Table Name     : " & file_name & "</h2>")

    '*************************************************************************************************************************
    
    '****************************************************************************************************************************
    '******     Start Creating the HTML body surrounding the table.
    '****************************************************************************************************************************
    GENED_REQ_DOC.WriteLine ("")
    GENED_REQ_DOC.WriteLine ("<table class=""reference"" cellspacing=""0"" cellpadding=""2"" border=""1"" width=""100%"" style=""font-family:arial"">")
    GENED_REQ_DOC.WriteLine (vbTab & "<tr>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">Sr.No</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">Description</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">DCI_Description</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">Profi_ID</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">Data_Length</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">DataTypeLength</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_DEFAULT_WIDTH & "align = ""center"" valign = ""top"">DataType</th>")
    
    
    'GENED_REQ_DOC.Writeline (vbTab & vbTab & "<th width=" & HTML_ENUM_WIDTH & "align = ""center"" valign = ""top"">Enum Values</th>")
    'GENED_REQ_DOC.Writeline (vbTab & vbTab & "<th width=" & HTML_INIT_WIDTH & "align = ""center"" valign = ""top"">Init support</th>")
    'GENED_REQ_DOC.Writeline (vbTab & vbTab & "<th width=" & HTML_INIT_WIDTH & "align = ""center"" valign = ""top"">Default support</th>")
    'GENED_REQ_DOC.Writeline (vbTab & vbTab & "<th width=" & HTML_MEM_WIDTH & "align = ""center"" valign = ""top"">Access</th>")
    'GENED_REQ_DOC.Writeline (vbTab & "</tr>")
    
    '****************************************************************************************************************************
    '******     Start Creating the LUA output file.
    '****************************************************************************************************************************
    GENED_LUA_DOC.WriteLine ("--**************************************************************************")
    GENED_LUA_DOC.WriteLine ("--*    Excel Generated LUA File for BUI Parameter Reference.")
    GENED_LUA_DOC.WriteLine ("--**************************************************************************")
    GENED_LUA_DOC.WriteLine ("")
    GENED_LUA_DOC.WriteLine ("")
    
    '****************************************************************************************************************************
    '******     Build the table that will list all the data related the Profibus Parameters
    '****************************************************************************************************************************
    
 
    
    compare_string = "DCI_"
    str_len = Len(compare_string)
    DIAG_PARAM_COL = 20
    req_counter = 0
    
    counter = START_ROW
    'GENED_LUA_DOC.Writeline (Spacify("BUI_REGISTERS = {", 17))
   ' While (Cells(counter, USER_PARAM_COL).Text <> Empty) And (Cells(counter, DATA_DEFINE_COL).Text <> BEGIN_IGNORED_DATA)
     While (Cells(counter, DIAG_PARAM_COL).text <> "BEGIN_IGNORED_DATA")

        profi_param = Cells(counter, DIAG_PARAM_COL).text
        'profi_search_string = Mid(profi_param, 1, str_len)
        profi_search_string = Cells(counter, DIAG_PARAM_COL).text

        
        If ((profi_search_string = compare_string) Or (profi_search_string <> Empty)) Then
            string_len = Len(profi_param)
            desc_sheet_row = Find_Profi_DCID_Row(Cells(counter, DIAG_PARAM_COL).text)
    
            ' Descriptor Address
            DESCRIPTION = Worksheets("ProfiBus (DCI)").Range("A1").Cells(desc_sheet_row, DATA_DESC_COL).text
            'Description = Cells(desc_sheet_row, DATA_DESC_COL).Text
      
            ' DCI_define
            DCI_define = Worksheets("ProfiBus (DCI)").Range("A1").Cells(desc_sheet_row, DATA_DEFINE_COL).text
            'DCI_define = Cells(desc_sheet_row, DATA_DEFINE_COL).Text
         
            'Profi_DCI_ID
            Profi_DCI_ID = Worksheets("ProfiBus (DCI)").Range("A1").Cells(desc_sheet_row, 4).text
            'Profi_DCI_ID = Cells(desc_sheet_row, 4).Text
            
            'DATA LENGTH Column
            Data_Length = Worksheets("ProfiBus (DCI)").Range("A1").Cells(desc_sheet_row, 3).value
            'Data_Length = Cells(desc_sheet_row, 3).Value
            
              
            'DATA TYPE LEN  column
            DATA_TYPE_LEN = Worksheets("ProfiBus (DCI)").Range("A1").Cells(desc_sheet_row, 8).value
             'DATA_TYPE_LEN = Cells(desc_sheet_row, 8).Value

            ' Data Type
            data_type = Worksheets("ProfiBus (DCI)").Range("A1").Cells(desc_sheet_row, 7).value
            '  DATA_TYPE = Cells(desc_sheet_row, 7).Value

                  
           
           ' key = key + 1
            
              '          GENED_LUA_DOC.Writeline (Spacify(" ", 16) & "[" & key & "] = {[""PARAM_NAME""] = " & """" & Spacify((Cells(counter, DATA_DEFINE_COL).Text), string_len) _
                 '           & """, " & Spacify("[""ADDR""] =", 8) & reg_addr & Spacify(", [""DEFAULT""] =", 15) & "{" & default & "}" & _
                         '   Spacify(", [""LENGTH""] =", 13) & integer_convert & ", [""ACCESS""] = """ & Access & """")
           ' GENED_LUA_DOC.Writeline (Spacify(" ", 16) & Spacify(", [""ENUM""] =", 12) & "{" & Enum_val & "}" & Spacify(", [""START_RANGE""] =", 15) _
                            & start_range & Spacify(", [""END_RANGE""] =", 15) & end_range & ", [""INIT_VAL_SUPP""] = """ & init_val & """, [""DEFAULT_SUPP""] = """ & def_supp & """},")
           ' GENED_LUA_DOC.Writeline ("")



           ' GENED_LUA_DOC.Writeline (Spacify(" ", 16) & "[" & key & "] = {[""PARAM_NAME""] = " & """" & Spacify((Cells(counter, DATA_DEFINE_COL).Text), string_len) _
                            & """, " & Spacify("[""ADDR""] =", 8) & reg_addr & Spacify(", [""DEFAULT""] =", 15) & "{" & default & "}" & _
            '                Spacify(", [""LENGTH""] =", 13) & integer_convert & ", [""ACCESS""] = """ & Access & """")
           ' GENED_LUA_DOC.Writeline (Spacify(" ", 16) & Spacify(", [""ENUM""] =", 12) & "{" & Enum_val & "}" & Spacify(", [""START_RANGE""] =", 15) _
          '                  & start_range & Spacify(", [""END_RANGE""] =", 15) & end_range & ", [""INIT_VAL_SUPP""] = """ & init_val & """, [""DEFAULT_SUPP""] = """ & def_supp & """},")
          '  GENED_LUA_DOC.Writeline ("")
                         
                         
            '****** Start the HTML version for the requirements document
            req_counter = req_counter + 1
            
            If (default = "nil") Then
                default = "--"
            End If
            If (Enum_val = "nil") Then
                Enum_val = "--"
            End If
            If (start_range = "nil") Then
                start_range = "--"
            End If
            If (end_range = "nil") Then
                end_range = "--"
            End If
                        
            'DCID Description column in HTML File
            If (Worksheets("ProfiBus (DCI)").Range("A1").Cells(desc_sheet_row, DCI_LONG_DESC_COL).text <> Empty) Then
                dcid_desc = Worksheets("ProfiBus (DCI)").Range("A1").Cells(desc_sheet_row, DCI_LONG_DESC_COL).text
            Else
                dcid_desc = "--"
            End If
            
            GENED_REQ_DOC.WriteLine (vbTab & "<tr>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""left"" valign=""top""><p>" & req_counter & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""left"" valign=""top""><p>" & DESCRIPTION & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_DCID_WIDTH & "align=""left"" valign=""top""><p>" & DCI_define & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_DCID_WIDTH & "align=""left"" valign=""top""><p>" & Profi_DCI_ID & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""left"" valign=""top""><p>" & Data_Length & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_RANGE_WIDTH & "align=""left"" valign=""top""><p>" & DATA_TYPE_LEN & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_RANGE_WIDTH & "align=""left"" valign=""top""><p>" & data_type & "</p></td>")
'            GENED_REQ_DOC.Writeline (vbTab & vbTab & "<td width=" & HTML_INIT_WIDTH & "align=""left"" valign=""top""><p>" & init_val & "</p></td>")
'            GENED_REQ_DOC.Writeline (vbTab & vbTab & "<td width=" & HTML_INIT_WIDTH & "align=""left"" valign=""top""><p>" & def_supp & "</p></td>")
'            GENED_REQ_DOC.Writeline (vbTab & vbTab & "<td width=" & HTML_MEM_WIDTH & "align=""left"" valign=""top""><p>" & Access & "</p></td>")
        End If
       counter = counter + 1
    Wend
'    GENED_LUA_DOC.Writeline (Spacify("", 12) & "}")
'    GENED_LUA_DOC.Writeline (vbNewLine & "TOTAL_PARAM = " & key)
    GENED_REQ_DOC.WriteLine ("</table>")
    GENED_REQ_DOC.WriteLine ("")
    
   '*********************************************************************************************************
   '*********************************************************************************************************
   '*********************************************************************************************************
   
   
   'Input Data Table
    '*****************************************************
    '******     HTML Cell Column Width and alignment
    '*****************************************************
    HTML_ID_WIDTH = """5%"""
    HTML_DCID_WIDTH = """10%"""
    HTML_DESC_WIDTH = """20%"""
    HTML_DEFAULT_WIDTH = """10%"""
    HTML_ENUM_WIDTH = """5%"""
    HTML_RANGE_WIDTH = """5%"""
    HTML_INIT_WIDTH = """5%"""
    HTML_MEM_WIDTH = """5%"""
    
    '****************************************************************************************************************************
    '******     Create the header information for the HTML file.
    '****************************************************************************************************************************
    GENED_REQ_DOC.WriteLine ("<html><body>" & vbNewLine & "<style>")
    GENED_REQ_DOC.WriteLine (vbNewLine & "body" & vbNewLine & "{" & vbNewLine & "background-color:White;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "h1" & vbNewLine & "{" & vbNewLine & "color:Navy;" & vbNewLine & "text-align:center;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "h2" & vbNewLine & "{" & vbNewLine & "color:Black;" & vbNewLine & "text-align:left;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "p" & vbNewLine & "{" & vbNewLine & "font-size:12px" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "p1" & vbNewLine & "{" & vbNewLine & "color:Navy;" & vbNewLine & "text-align:left;" & "font-size:16px" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "</style>" & vbNewLine & "</head>" & vbNewLine & "<body>")
    file_name = "Bitwise Input Parameter Data"
    GENED_REQ_DOC.WriteLine ("<h2>Table Name     : " & file_name & "</h2>")

    '*************************************************************************************************************************
    
    '****************************************************************************************************************************
    '******     Start Creating the HTML body surrounding the table.
    '****************************************************************************************************************************
    GENED_REQ_DOC.WriteLine ("")
    GENED_REQ_DOC.WriteLine ("<table class=""reference"" cellspacing=""0"" cellpadding=""2"" border=""1"" width=""100%"" style=""font-family:arial"">")
    GENED_REQ_DOC.WriteLine (vbTab & "<tr>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">Sr.No</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">Input Data</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">Input Data Description</th>")
 
      
    '****************************************************************************************************************************
    '******     Start Creating the LUA output file.
    '****************************************************************************************************************************
    GENED_LUA_DOC.WriteLine ("--**************************************************************************")
    GENED_LUA_DOC.WriteLine ("--*    Excel Generated LUA File for BUI Parameter Reference.")
    GENED_LUA_DOC.WriteLine ("--**************************************************************************")
    GENED_LUA_DOC.WriteLine ("")
    GENED_LUA_DOC.WriteLine ("")
    
   req_counter = 1
   'counter = START_ROW = 5
    
                     
                         
   ' While (Cells(counter, DATA_DEFINE_COL).Text <> BEGIN_IGNORED_DATA)
     
          DESCRIPTION = Worksheets("ProfiBus(IO Modules)").Range("A1").Cells(5, 5).text
             
           Dim input1 As String
           Dim Input_data As String
           Dim Input_data_Desc As String
           Dim fields() As String
           Dim fields1() As String


    ' Split the string at the comma characters and add each field to a ListBox
    fields() = Split(DESCRIPTION, ";")
    
    For i = 0 To UBound(fields)
        input1 = Trim$(fields(i))
        fields1() = Split(input1, "=")
         
           For j = 0 To UBound(fields1)
             Input_data = Trim$(fields1(0))
             Input_data_Desc = Trim$(fields1(1))
           Next
            
            GENED_REQ_DOC.WriteLine (vbTab & "<tr>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""center"" valign=""top""><p>" & req_counter & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""center"" valign=""top""><p>" & Input_data & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""center"" valign=""top""><p>" & Input_data_Desc & "</p></td>")
            
            
       req_counter = req_counter + 1
       Next
'    Wend
'    GENED_LUA_DOC.Writeline (Spacify("", 12) & "}")
'    GENED_LUA_DOC.Writeline (vbNewLine & "TOTAL_PARAM = " & key)
    GENED_REQ_DOC.WriteLine ("</table>")
    GENED_REQ_DOC.WriteLine ("")
    
    
    
     'Word Input Data Table
    '*****************************************************
    '******     HTML Cell Column Width and alignment
    '*****************************************************
    HTML_ID_WIDTH = """5%"""
    HTML_DCID_WIDTH = """10%"""
    HTML_DESC_WIDTH = """20%"""
    HTML_DEFAULT_WIDTH = """10%"""
    HTML_ENUM_WIDTH = """5%"""
    HTML_RANGE_WIDTH = """5%"""
    HTML_INIT_WIDTH = """5%"""
    HTML_MEM_WIDTH = """5%"""
    
    '****************************************************************************************************************************
    '******     Create the header information for the HTML file.
    '****************************************************************************************************************************
    GENED_REQ_DOC.WriteLine ("<html><body>" & vbNewLine & "<style>")
    GENED_REQ_DOC.WriteLine (vbNewLine & "body" & vbNewLine & "{" & vbNewLine & "background-color:White;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "h1" & vbNewLine & "{" & vbNewLine & "color:Navy;" & vbNewLine & "text-align:center;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "h2" & vbNewLine & "{" & vbNewLine & "color:Black;" & vbNewLine & "text-align:left;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "p" & vbNewLine & "{" & vbNewLine & "font-size:12px" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "p1" & vbNewLine & "{" & vbNewLine & "color:Navy;" & vbNewLine & "text-align:left;" & "font-size:16px" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "</style>" & vbNewLine & "</head>" & vbNewLine & "<body>")
     file_name = "Word Input Parameter Data"
    GENED_REQ_DOC.WriteLine ("<h2>Table Name     : " & file_name & "</h2>")

    '*************************************************************************************************************************
    
    '****************************************************************************************************************************
    '******     Start Creating the HTML body surrounding the table.
    '****************************************************************************************************************************
    GENED_REQ_DOC.WriteLine ("")
    GENED_REQ_DOC.WriteLine ("<table class=""reference"" cellspacing=""0"" cellpadding=""2"" border=""1"" width=""100%"" style=""font-family:arial"">")
    GENED_REQ_DOC.WriteLine (vbTab & "<tr>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">Sr.No</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">Input Data</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">Input Data Description</th>")
 
      
    '****************************************************************************************************************************
    '******     Start Creating the LUA output file.
    '****************************************************************************************************************************
    GENED_LUA_DOC.WriteLine ("--**************************************************************************")
    GENED_LUA_DOC.WriteLine ("--*    Excel Generated LUA File for BUI Parameter Reference.")
    GENED_LUA_DOC.WriteLine ("--**************************************************************************")
    GENED_LUA_DOC.WriteLine ("")
    GENED_LUA_DOC.WriteLine ("")
    
   req_counter = 1
   'counter = START_ROW = 5
    
                     
                         
   ' While (Cells(counter, DATA_DEFINE_COL).Text <> BEGIN_IGNORED_DATA)
     
          DESCRIPTION = Worksheets("ProfiBus(IO Modules)").Range("A1").Cells(5, 6).text
             
           Dim Input_word As String
           Dim Input_word_data As String
           Dim Input_word_data_Desc As String
           Dim Input_fields() As String
           Dim Input_fields1() As String


    ' Split the string at the comma characters and add each field to a ListBox
    Input_fields() = Split(DESCRIPTION, ";")
    
    For i = 0 To UBound(Input_fields)
        Input_word = Trim$(fields(i))
        Input_fields1() = Split(Input_word, "=")
         
           For j = 0 To UBound(Input_fields1)
             Input_word_data = Trim$(Input_fields1(0))
             Input_word_data_Desc = Trim$(Input_fields1(1))
           Next
            
            GENED_REQ_DOC.WriteLine (vbTab & "<tr>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""center"" valign=""top""><p>" & req_counter & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""center"" valign=""top""><p>" & Input_word_data & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""center"" valign=""top""><p>" & Input_word_data_Desc & "</p></td>")
            
            
       req_counter = req_counter + 1
       Next
'    Wend
'    GENED_LUA_DOC.Writeline (Spacify("", 12) & "}")
'    GENED_LUA_DOC.Writeline (vbNewLine & "TOTAL_PARAM = " & key)
    GENED_REQ_DOC.WriteLine ("</table>")
    GENED_REQ_DOC.WriteLine ("")
   
   
   
   '*********************************************************************************************************
   '*********************************************************************************************************
'****************************************************************************************************************************
    '******     Create the header information for the HTML file.
    '****************************************************************************************************************************
    GENED_REQ_DOC.WriteLine ("<html><body>" & vbNewLine & "<style>")
    GENED_REQ_DOC.WriteLine (vbNewLine & "body" & vbNewLine & "{" & vbNewLine & "background-color:White;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "h1" & vbNewLine & "{" & vbNewLine & "color:Navy;" & vbNewLine & "text-align:center;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "h2" & vbNewLine & "{" & vbNewLine & "color:Black;" & vbNewLine & "text-align:left;" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "p" & vbNewLine & "{" & vbNewLine & "font-size:12px" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "p1" & vbNewLine & "{" & vbNewLine & "color:Navy;" & vbNewLine & "text-align:left;" & "font-size:16px" & vbNewLine & "}")
    GENED_REQ_DOC.WriteLine (vbNewLine & "</style>" & vbNewLine & "</head>" & vbNewLine & "<body>")
    
    file_name = "Output Parameter Data"
    GENED_REQ_DOC.WriteLine ("<h2>Table Name     : " & file_name & "</h2>")

    '*************************************************************************************************************************
    
    '****************************************************************************************************************************
    '******     Start Creating the HTML body surrounding the table.
    '****************************************************************************************************************************
    GENED_REQ_DOC.WriteLine ("")
    GENED_REQ_DOC.WriteLine ("<table class=""reference"" cellspacing=""0"" cellpadding=""2"" border=""1"" width=""100%"" style=""font-family:arial"">")
    GENED_REQ_DOC.WriteLine (vbTab & "<tr>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">Sr.No</th>")
    GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<th width=" & HTML_ID_WIDTH & "align = ""center"" valign = ""top"">Output Parameter Data</th>")
      
 
      
    '****************************************************************************************************************************
    '******     Start Creating the LUA output file.
    '****************************************************************************************************************************
    GENED_LUA_DOC.WriteLine ("--**************************************************************************")
    GENED_LUA_DOC.WriteLine ("--*    Excel Generated LUA File for BUI Parameter Reference.")
    GENED_LUA_DOC.WriteLine ("--**************************************************************************")
    GENED_LUA_DOC.WriteLine ("")
    GENED_LUA_DOC.WriteLine ("")
    
    '****************************************************************************************************************************
    '******     Build the table that will list all the data related the Profibus Parameters
    '****************************************************************************************************************************
    
   
    
    compare_string = "x"
    str_len = Len(compare_string)
    START_ROW = 66
    OUTPU_DATA_COL = 2
    
    counter = START_ROW
    req_counter = 1
    
     
       
       ActiveWorkbook.Sheets("ProfiBus(IO Modules)").Activate
       
       While (Cells(counter, DATA_DEFINE_COL).text <> compare_string)

            Output_Data = Worksheets("ProfiBus(IO Modules)").Range("A1").Cells(counter, OUTPU_DATA_COL).text
            If (Output_Data <> Empty) Then
            
            GENED_REQ_DOC.WriteLine (vbTab & "<tr>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""center"" valign=""top""><p>" & req_counter & "</p></td>")
            GENED_REQ_DOC.WriteLine (vbTab & vbTab & "<td width=" & HTML_ID_WIDTH & "align=""center"" valign=""top""><p>" & Output_Data & "</p></td>")
            req_counter = req_counter + 1
            End If
            counter = counter + 1
       Wend
       
    GENED_REQ_DOC.WriteLine ("</table>")
    GENED_REQ_DOC.WriteLine ("")
    
    
    
    
    'end of the file
    GENED_REQ_DOC.WriteLine ("</body></html>")

    GENED_REQ_DOC.Close
    'GENED_LUA_DOC.Close
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
Function Find_attri_row(attr_string As String) As Integer
    req_row_ctr = EIP_REQ_START_ROW
                
    While (Worksheets("EthernetIP_Req").Range("A1").Cells(req_row_ctr, EIP_ATTR_CODE_COL).text <> Empty) And _
          (Worksheets("EthernetIP_Req").Range("A1").Cells(req_row_ctr, EIP_ATTR_CODE_COL).text <> attr_string)
            req_row_ctr = req_row_ctr + 1
    Wend
    
    If (Worksheets("EthernetIP_Req").Range("A1").Cells(req_row_ctr, EIP_ATTR_CODE_COL).text = attr_string) Then
        Find_attri_row = req_row_ctr
    Else
         Find_attri_row = 0
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
Public Function Spacify(my_string As String, col_pos As Integer) As String
    Dim string_len As Integer
    Dim space_cnt As Integer
    Dim tab_cnt_pos As Integer
    Dim num_tabs As Integer
    
    Spacify = my_string
    string_len = Len(my_string)
    
    If (col_pos > string_len) Then
        space_cnt = col_pos - string_len
    Else
        space_cnt = 1
    End If
    
    Spacify = my_string & Space(space_cnt)

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

Function Find_mem_loc(search_string As String)
    START_ROW = 7
    NV0_LOC_DEFINE_COL = 1
    NV1_LOC_DEFINE_COL = 6
    END_MEM_NV0 = "NV_DATA_END_NVCTRL0"
    END_MEM_NV1 = "NV_DATA_END_NVCTRL1"
    
    row_ctr1 = START_ROW
    row_ctr2 = START_ROW
    
    While (Worksheets("NVM Address").Range("A1").Cells(row_ctr1, NV0_LOC_DEFINE_COL).text <> END_MEM_NV0) And _
            (Worksheets("NVM Address").Range("A1").Cells(row_ctr1, NV0_LOC_DEFINE_COL).text <> search_string)
        row_ctr1 = row_ctr1 + 1
    Wend
    
    While (Worksheets("NVM Address").Range("A1").Cells(row_ctr2, NV1_LOC_DEFINE_COL).text <> END_MEM_NV1) And _
            (Worksheets("NVM Address").Range("A1").Cells(row_ctr2, NV1_LOC_DEFINE_COL).text <> search_string)
        row_ctr2 = row_ctr2 + 1
    Wend
    
    If (Worksheets("NVM Address").Range("A1").Cells(row_ctr1, NV0_LOC_DEFINE_COL).text = search_string) Then
        Find_mem_loc = "NV0"
    ElseIf (Worksheets("NVM Address").Range("A1").Cells(row_ctr2, NV1_LOC_DEFINE_COL).text = search_string) Then
        Find_mem_loc = "NV1"
    Else
        Find_mem_loc = "--"
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
Function Find_edc_owner_Row(search_string As String) As Integer

    EDC_OWNER_COL = 1
    EDC_OWNER_START_ROW = 8
    
    row_ctr = EDC_OWNER_START_ROW

    While (Worksheets("EDC_Param").Range("A1").Cells(row_ctr, EDC_OWNER_COL).text <> Empty) And _
            (Worksheets("EDC_Param").Range("A1").Cells(row_ctr, EDC_OWNER_COL).text <> _
                        search_string)
        row_ctr = row_ctr + 1
    Wend
    
    If (Worksheets("EDC_Param").Range("A1").Cells(row_ctr, EDC_OWNER_COL).text = search_string) Then
        Find_edc_owner_Row = row_ctr
    Else
        Find_edc_owner_Row = 0
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
Function Find_Profi_DCID_Row(search_string As String) As Integer

    DCI_DEFINES_COL = 2
    dci_defines_row_ctr = 12


    While (Worksheets("ProfiBus (DCI)").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> Empty) And _
            (Worksheets("ProfiBus (DCI)").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> _
                        search_string)
        dci_defines_row_ctr = dci_defines_row_ctr + 1
    Wend
    
    If (Worksheets("ProfiBus (DCI)").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text = _
                        search_string) Then
        Find_Profi_DCID_Row = dci_defines_row_ctr
    Else
        Find_Profi_DCID_Row = 0
        
    End If
End Function
'************************************************************************
Function Get_Number_Of_Bits(search_string As String) As Integer
    DESCRIPTION_COL = 81
    total_bits = 0
    Dim DCI_DEFINES_START_ROW As Integer
    DCI_DEFINES_START_ROW = 9
    Dim DCI_DESCRIPTORS_COL As Integer
    DCI_DESCRIPTORS_COL = 1
    Dim DCI_DEFINES_COL As Integer
    DCI_DEFINES_COL = 2
    
    dci_defines_row_ctr = DCI_DEFINES_START_ROW

    While (Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> Empty) And _
            (Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> _
                        search_string)
        dci_defines_row_ctr = dci_defines_row_ctr + 1
    Wend
    description_content_string = Worksheets("DCI Descriptors").Cells(dci_defines_row_ctr, DESCRIPTION_COL).text
    descriptionString = Split(description_content_string, vbLf)
    Dim Count As Integer
    For Count = 1 To UBound(descriptionString)
    total_bits = total_bits + 1
    Next Count
    Get_Number_Of_Bits = total_bits
End Function
Function Find_DCI_Row(search_string As String) As Integer

    Dim DCI_DEFINES_START_ROW As Integer
    DCI_DEFINES_START_ROW = 9
    Dim DCI_DESCRIPTORS_COL As Integer
    DCI_DESCRIPTORS_COL = 1
    Dim DCI_DEFINES_COL As Integer
    DCI_DEFINES_COL = 2
    
    dci_defines_row_ctr = DCI_DEFINES_START_ROW

    While (Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DESCRIPTORS_COL).text <> Empty) And _
            (Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DESCRIPTORS_COL).text <> _
                        search_string)
        dci_defines_row_ctr = dci_defines_row_ctr + 1
    Wend
    
    If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DESCRIPTORS_COL).text = _
                        search_string) Then
        Find_DCI_Row = dci_defines_row_ctr
    Else
        Find_DCI_Row = 0
    End If
End Function


'****************************************************************************************************************************
'
'   Build enum description string
'
'****************************************************************************************************************************
Private Function Find_Enum_Range(desc_sheet_row As Long) As String
    
    Dim index As Long
    Dim ListItems() As String
    Dim ParsedItems() As String
    Dim TotalEnums As Long
    Dim First_Item As Boolean
    
    First_Item = False
    

    enumlist = DCID_Lookup_Enum(desc_sheet_row)
    ListItems = Split(enumlist, ITEM_DELIMITER)
    TotalEnums = CInt(ListItems(0))
    For index = 1 To UBound(ListItems) Step 1
        If "" <> Trim(ListItems(index)) Then
            ParsedItems = Split(ListItems(index), SUB_ITEM_DELIMITER)
            'Remove line feeds
            ParsedItems(0) = Replace(ParsedItems(0), vbLf, "")
            ParsedItems(1) = Replace(ParsedItems(1), vbLf, "")
            ParsedItems(2) = Replace(ParsedItems(2), vbLf, "")

        End If
    Next index
    long_desc = long_desc & "</table>" & vbCrLf

    Build_Html_Enum_Description = long_desc
End Function




