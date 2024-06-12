Attribute VB_Name = "Profibus_GSD_Builder"
Public Type OP_BIT_ClASS
    mod_num As Integer
    bit_num As Integer
    offset As Integer
    Display As String
End Type
Public Type IP_BIT_ClASS
    profi_id As Integer
    bit_num As Integer
    offset As Integer
    Display As String
End Type
Public Type IP_WORD_ClASS
    profi_id As Integer
    offset As Integer
    Display As String
End Type
Public Type OP_WORD_ClASS
    mod_num As Integer
    bit_num As Integer
    offset As Integer
    Display As String
End Type
Public Type IP_DWORD_ClASS
    profi_id As Integer
    offset As Integer
    Display As String
End Type
Public Type OP_DWORD_ClASS
    mod_num As Integer
    bit_num As Integer
    offset As Integer
    Display As String
End Type
Public Type OP_BIT_VALUE_CLASS
    default As Integer
    min As Integer
    max As Integer
    enum_descr As String
End Type
Public Type IP_BIT_VALUE_CLASS
    default As Integer
    min As Integer
    max As Integer
    enum_descr As String
End Type
Public Type IP_WORD_VALUE_CLASS
    default As Integer
    min As Integer
    max As Integer
    enum_descr As String
End Type
Public Type OP_WORD_VALUE_CLASS
    default As Integer
    min As Integer
    max As Integer
    enum_descr As String
End Type
Public Type IP_DWORD_VALUE_CLASS
    default As Integer
    min As Integer
    max As Integer
    enum_descr As String
End Type
Public Type OP_DWORD_VALUE_CLASS
    default As Integer
    min As Integer
    max As Integer
    enum_descr As String
End Type
Dim OP_BIT_TABLE(200) As OP_BIT_ClASS
Public OP_BIT_VALUE_TABLE As OP_BIT_VALUE_CLASS
Dim IP_BIT_TABLE(200) As IP_BIT_ClASS
Public IP_BIT_VALUE_TABLE As IP_BIT_VALUE_CLASS
Dim IP_WORD_TABLE(200) As IP_WORD_ClASS
Public IP_WORD_VALUE_TABLE As IP_WORD_VALUE_CLASS
Dim IP_DWORD_TABLE(200) As IP_DWORD_ClASS
Public IP_DWORD_VALUE_TABLE As IP_DWORD_VALUE_CLASS
Dim OP_WORD_TABLE(200) As OP_WORD_ClASS
Public OP_WORD_VALUE_TABLE As OP_WORD_VALUE_CLASS
Dim OP_DWORD_TABLE(200) As OP_DWORD_ClASS
Public OP_DWORD_VALUE_TABLE As OP_DWORD_VALUE_CLASS

Public OUTPUT_PARAM_LIST(3) As IP_BIT_VALUE_CLASS

Dim Get_Datatype_String1 As String
Dim Temp_Datatype_Desc As String


Public Function Get_Datatype_String(my_s As String) As String
       
    If (my_s = "DCI_DTYPE_BYTE") Or (my_s = "DCI_DTYPE_UINT8") Or (my_s = "DCI_DTYPE_BOOL") Then
        Get_Datatype_String1 = "uint8_t"
    ElseIf (my_s = "DCI_DTYPE_SINT8") Then
        Get_Datatype_String1 = "int8_t"
    ElseIf (my_s = "DCI_DTYPE_UINT16") Or (my_s = "DCI_DTYPE_WORD") Then
        Get_Datatype_String1 = "uint16_t"
    ElseIf (my_s = "DCI_DTYPE_SINT16") Then
        Get_Datatype_String1 = "int16_t"
    ElseIf (my_s = "DCI_DTYPE_UINT32") Or (my_s = "DCI_DTYPE_DWORD") Then
        Get_Datatype_String1 = "uint32_t"
    ElseIf (my_s = "DCI_DTYPE_SINT32") Then
        Get_Datatype_String1 = "int32_t"
    ElseIf (my_s = "DCI_DTYPE_FLOAT") Then
        Get_Datatype_String1 = "float"
    ElseIf (my_s = "DCI_DTYPE_LFLOAT") Then
        Get_Datatype_String1 = "float64_t"
    ElseIf (my_s = "DCI_DTYPE_DFLOAT") Then
        Get_Datatype_String1 = "float64_t"
    ElseIf (my_s = "DCI_DTYPE_UINT64") Then
        Get_Datatype_String1 = "uint64_t"
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
Public Function Fill_INPUT_WORD_TABLE()
    
    'Profi (BCM) row column settings
    START_ROW = 12
    DESCRIP_COL = 1
    DATA_DEFINE_COL = 2
    DATA_LENGTH_COL = 5
    DATA_PROFIBUS_REG_COL = 6
    DATA_NEXT_INDEX_COL = 5
    DATA_ATOMIBLOCK_SELECT = 5
    DATA_OFFSET_COL = 6
    'DATA_MIN_COL = 5 '7
    'DATA_MAX_COL = 6 '8
    DATA_TYPE_COL = 3
    DATA_TYPE_LENGTH_COL = 4
    GSD_DATATYPE_DEFINE_COL = 4 'LTK Req
    
    'Row Col settings for ProfiBus(IO Modules)sheet
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    Profibus_IO_Module_START_ROW = 26
    counter = START_ROW
    Module = 1
    counter = Profibus_IO_Module_START_ROW
    MODULE_DESCRIPTION_COL = 1 'col 1
    MODULE_NUMBER_COL = MODULE_DESCRIPTION_COL + 1 'col 2
    INPUT_BIT_COL = MODULE_NUMBER_COL + 1 'col 3
    INPUT_WORD_COL = INPUT_BIT_COL + 1  'col 4
    INPUT_DWORD_COL = INPUT_WORD_COL + 1  'col 4
    OUTPUT_BIT_COL = INPUT_DWORD_COL + 1 'col  5
    OUTPUT_WORD_COL = OUTPUT_BIT_COL + 1 'col 6
    OUTPUT_DWORD_COL = OUTPUT_WORD_COL + 1 'col 6
    INPUT_SIZE_BYTES_COL = MODULE_NUMBER_COL + 7
    OUTPUT_SIZE_BYTES_COL = INPUT_SIZE_BYTES_COL + 1
    IP_MAP_PRM_BYTES_COL = OUTPUT_SIZE_BYTES_COL + 1
    MOD_ID_MSB_COL = OUTPUT_SIZE_BYTES_COL + 9
    MOD_ID_LSB_COL = MOD_ID_MSB_COL + 1
    
    Const MAX_ENUM_NUM = 100
    Dim stEnumObValue(MAX_ENUM_NUM) As String
    Dim stEnumObTextId(MAX_ENUM_NUM) As String
    
    class_offset = 0
    IP_WORD_VALUE_TABLE.enum_descr = ""
    IP_WORD_VALUE_TABLE.max = 0
    IP_WORD_VALUE_TABLE.min = 0
            IP_WORD_TABLE(class_offset).profi_id = 0
            IP_WORD_TABLE(class_offset).offset = class_offset
            IP_WORD_TABLE(class_offset).Display = (class_offset & "=" & "IP WORD NOT CONNECTED" & ";")
            IP_WORD_VALUE_TABLE.enum_descr = IP_WORD_VALUE_TABLE.enum_descr & IP_WORD_TABLE(class_offset).Display

    class_offset = class_offset + 1
    counter = START_ROW
    While (Worksheets("ProfiBus (DCI)").Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            If ((Worksheets("ProfiBus (DCI)").Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC INPUT WORD") _
            Or (Worksheets("ProfiBus (DCI)").Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC INPUT BIT INPUT WORD")) Then
            dci_sheet_row = Find_DCID_Row_Search_Desc_Col(Worksheets("ProfiBus (DCI)").Cells(counter, 1))
            SHORT_DESCRIP_COL = 79
            description_content_string = Worksheets("DCI Descriptors").Cells(dci_sheet_row, SHORT_DESCRIP_COL).text
                    IP_WORD_TABLE(class_offset).profi_id = Worksheets("ProfiBus (DCI)").Cells(counter, DATA_PROFIBUS_REG_COL).value
                    IP_WORD_TABLE(class_offset).offset = class_offset
                    IP_WORD_TABLE(class_offset).Display = ("" & class_offset & "=" & description_content_string & ";")
                    IP_WORD_VALUE_TABLE.enum_descr = IP_WORD_VALUE_TABLE.enum_descr & IP_WORD_TABLE(class_offset).Display
                    If (IP_WORD_VALUE_TABLE.min > IP_WORD_TABLE(class_offset).offset) Then
                          IP_WORD_VALUE_TABLE.min = IP_WORD_TABLE(class_offset).offset
                    End If
                    If (IP_WORD_VALUE_TABLE.max < IP_WORD_TABLE(class_offset).offset) Then
                        IP_WORD_VALUE_TABLE.max = IP_WORD_TABLE(class_offset).offset
                    End If
                    class_offset = class_offset + 1
            End If
    counter = counter + 1
    Wend
    
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
Public Function Fill_INPUT_DWORD_TABLE()
    
    'Profi (BCM) row column settings
    START_ROW = 12
    DESCRIP_COL = 1
    DATA_DEFINE_COL = 2
    DATA_LENGTH_COL = 5
    DATA_PROFIBUS_REG_COL = 6
    DATA_NEXT_INDEX_COL = 5
    DATA_ATOMIBLOCK_SELECT = 5
    DATA_OFFSET_COL = 6
    'DATA_MIN_COL = 5 '7
    'DATA_MAX_COL = 6 '8
    DATA_TYPE_COL = 3
    DATA_TYPE_LENGTH_COL = 4
    GSD_DATATYPE_DEFINE_COL = 4 'LTK Req
    
    'Row Col settings for ProfiBus(IO Modules)sheet
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    Profibus_IO_Module_START_ROW = 26
    counter = START_ROW
    Module = 1
    counter = Profibus_IO_Module_START_ROW
    MODULE_DESCRIPTION_COL = 1 'col 1
    MODULE_NUMBER_COL = MODULE_DESCRIPTION_COL + 1 'col 2
    INPUT_BIT_COL = MODULE_NUMBER_COL + 1 'col 3
    INPUT_WORD_COL = INPUT_BIT_COL + 1  'col 4
    INPUT_DWORD_COL = INPUT_WORD_COL + 1  'col 4
    OUTPUT_BIT_COL = INPUT_DWORD_COL + 1 'col  5
    OUTPUT_WORD_COL = OUTPUT_BIT_COL + 1 'col 6
    OUTPUT_DWORD_COL = OUTPUT_WORD_COL + 1 'col 6
    INPUT_SIZE_BYTES_COL = MODULE_NUMBER_COL + 7
    OUTPUT_SIZE_BYTES_COL = INPUT_SIZE_BYTES_COL + 1
    IP_MAP_PRM_BYTES_COL = OUTPUT_SIZE_BYTES_COL + 1
    MOD_ID_MSB_COL = OUTPUT_SIZE_BYTES_COL + 9
    MOD_ID_LSB_COL = MOD_ID_MSB_COL + 1
    
    Const MAX_ENUM_NUM = 100
    Dim stEnumObValue(MAX_ENUM_NUM) As String
    Dim stEnumObTextId(MAX_ENUM_NUM) As String
    
    class_offset = 0
    IP_DWORD_VALUE_TABLE.enum_descr = ""
    IP_DWORD_VALUE_TABLE.max = 0
    IP_DWORD_VALUE_TABLE.min = 0
            IP_DWORD_TABLE(class_offset).profi_id = 0
            IP_DWORD_TABLE(class_offset).offset = class_offset
            IP_DWORD_TABLE(class_offset).Display = (class_offset & "=" & "IP DWORD NOT CONNECTED" & ";")
            IP_DWORD_VALUE_TABLE.enum_descr = IP_DWORD_VALUE_TABLE.enum_descr & IP_DWORD_TABLE(class_offset).Display
    class_offset = class_offset + 1
    counter = START_ROW
    While (Worksheets("ProfiBus (DCI)").Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            If ((Worksheets("ProfiBus (DCI)").Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC INPUT DWORD") _
            Or (Worksheets("ProfiBus (DCI)").Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC INPUT BIT INPUT DWORD")) Then
            dci_sheet_row = Find_DCID_Row_Search_Desc_Col(Worksheets("ProfiBus (DCI)").Cells(counter, 1))
            SHORT_DESCRIP_COL = 79
            description_content_string = Worksheets("DCI Descriptors").Cells(dci_sheet_row, SHORT_DESCRIP_COL).text
                    IP_DWORD_TABLE(class_offset).profi_id = Worksheets("ProfiBus (DCI)").Cells(counter, DATA_PROFIBUS_REG_COL).value
                    IP_DWORD_TABLE(class_offset).offset = class_offset
                    IP_DWORD_TABLE(class_offset).Display = ("" & class_offset & "=" & description_content_string & ";")
                    IP_DWORD_VALUE_TABLE.enum_descr = IP_DWORD_VALUE_TABLE.enum_descr & IP_DWORD_TABLE(class_offset).Display
                    If (IP_DWORD_VALUE_TABLE.min > IP_DWORD_TABLE(class_offset).offset) Then
                          IP_DWORD_VALUE_TABLE.min = IP_DWORD_TABLE(class_offset).offset
                    End If
                    If (IP_DWORD_VALUE_TABLE.max < IP_DWORD_TABLE(class_offset).offset) Then
                        IP_DWORD_VALUE_TABLE.max = IP_DWORD_TABLE(class_offset).offset
                    End If
                    class_offset = class_offset + 1
            End If
    counter = counter + 1
    Wend
    
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
Public Function Fill_INPUT_BIT_TABLE()
    
    'Profi (BCM) row column settings
    START_ROW = 12
    DESCRIP_COL = 1
    DATA_DEFINE_COL = 2
    DATA_LENGTH_COL = 5
    DATA_PROFIBUS_REG_COL = 6
    DATA_NEXT_INDEX_COL = 5
    DATA_ATOMIBLOCK_SELECT = 5
    DATA_OFFSET_COL = 6
    'DATA_MIN_COL = 5 '7
    'DATA_MAX_COL = 6 '8
    DATA_TYPE_COL = 3
    DATA_TYPE_LENGTH_COL = 4
    GSD_DATATYPE_DEFINE_COL = 4
    
    'Row Col settings for ProfiBus(IO Modules)sheet
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    Profibus_IO_Module_START_ROW = 26
    counter = START_ROW
    Module = 1
    counter = Profibus_IO_Module_START_ROW
    MODULE_DESCRIPTION_COL = 1 'col 1
    MODULE_NUMBER_COL = MODULE_DESCRIPTION_COL + 1 'col 2
    INPUT_BIT_COL = MODULE_NUMBER_COL + 1 'col 3
    INPUT_WORD_COL = INPUT_BIT_COL + 1  'col 4
    INPUT_DWORD_COL = INPUT_WORD_COL + 1  'col 4
    OUTPUT_BIT_COL = INPUT_DWORD_COL + 1 'col  5
    OUTPUT_WORD_COL = OUTPUT_BIT_COL + 1 'col 6
    OUTPUT_DWORD_COL = OUTPUT_WORD_COL + 1 'col 6
    INPUT_SIZE_BYTES_COL = MODULE_NUMBER_COL + 7
    OUTPUT_SIZE_BYTES_COL = INPUT_SIZE_BYTES_COL + 1
    IP_MAP_PRM_BYTES_COL = OUTPUT_SIZE_BYTES_COL + 1
    MOD_ID_MSB_COL = OUTPUT_SIZE_BYTES_COL + 9
    MOD_ID_LSB_COL = MOD_ID_MSB_COL + 1
    
    Const MAX_ENUM_NUM = 100
    Dim stEnumObValue(MAX_ENUM_NUM) As String
    Dim stEnumObTextId(MAX_ENUM_NUM) As String
    
    class_offset = 0
    IP_BIT_VALUE_TABLE.enum_descr = ""
    IP_BIT_VALUE_TABLE.max = 0
    IP_BIT_VALUE_TABLE.min = 0
            IP_BIT_TABLE(class_offset).profi_id = 0
            IP_BIT_TABLE(class_offset).bit_num = 0
            IP_BIT_TABLE(class_offset).offset = class_offset
            IP_BIT_TABLE(class_offset).Display = (class_offset & "=" & "IP BIT NOT CONNECTED" & ";")
            IP_BIT_VALUE_TABLE.enum_descr = IP_BIT_VALUE_TABLE.enum_descr & IP_BIT_TABLE(class_offset).Display
    class_offset = class_offset + 1
    counter = START_ROW
    While (Worksheets("ProfiBus (DCI)").Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            If ((Worksheets("ProfiBus (DCI)").Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC INPUT BIT") _
            Or (Worksheets("ProfiBus (DCI)").Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC INPUT BIT INPUT WORD") _
            Or (Worksheets("ProfiBus (DCI)").Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC INPUT BIT INPUT DWORD")) Then
            dci_sheet_row = Find_DCID_Row_Search_Desc_Col(Worksheets("ProfiBus (DCI)").Cells(counter, 1))
            DESCRIPTION_COL = 81
            description_content_string = Worksheets("DCI Descriptors").Cells(dci_sheet_row, DESCRIPTION_COL).text
            stEnumOb = Split(description_content_string, vbLf)
            
                For bit_count = 0 To UBound(stEnumOb) - 1
                    IP_BIT_TABLE(class_offset).profi_id = Worksheets("ProfiBus (DCI)").Cells(counter, DATA_PROFIBUS_REG_COL).value
                    IP_BIT_TABLE(class_offset).bit_num = bit_count
                    IP_BIT_TABLE(class_offset).offset = class_offset
                                stEnumTemp = Split(stEnumOb(bit_count + 1), " = ")
                                If stEnumOb(bit_count) <> "" Then
                                    stEnumObValue(bit_count) = stEnumTemp(0)
                                    stEnumObTextId(bit_count) = stEnumTemp(1)
                                Else
                                    upLimit = upLimit - 1
                                End If
                    IP_BIT_TABLE(class_offset).Display = ("" & class_offset & "=" & stEnumObTextId(bit_count) & ";")
                    IP_BIT_VALUE_TABLE.enum_descr = IP_BIT_VALUE_TABLE.enum_descr & IP_BIT_TABLE(class_offset).Display
                    If (IP_BIT_VALUE_TABLE.min > IP_BIT_TABLE(class_offset).offset) Then
                          IP_BIT_VALUE_TABLE.min = IP_BIT_TABLE(class_offset).offset
                    End If
                    If (IP_BIT_VALUE_TABLE.max < IP_BIT_TABLE(class_offset).offset) Then
                        IP_BIT_VALUE_TABLE.max = IP_BIT_TABLE(class_offset).offset
                    End If
                    class_offset = class_offset + 1
                Next
                    
            End If
            
            
            
            
            
            'For bit_count = 0 To Worksheets("ProfiBus(IO Modules)").Cells(counter, OUTPUT_BIT_COL).Value - 1
                
                
                
                
             
                             
            'Next
            'OP_BIT_VALUE_TABLE.default = OP_BIT_TABLE(0).offset
    
    counter = counter + 1
    Wend
    
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
Public Function Fill_OUTPUT_WORD_TABLE()
    
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    Profibus_IO_Module_START_ROW = 26
    counter = Profibus_IO_Module_START_ROW
    Module = 1
    counter = Profibus_IO_Module_START_ROW
    MODULE_DESCRIPTION_COL = 1 'col 1
    MODULE_NUMBER_COL = MODULE_DESCRIPTION_COL + 1 'col 2
    INPUT_BIT_COL = MODULE_NUMBER_COL + 1 'col 3
    INPUT_WORD_COL = INPUT_BIT_COL + 1  'col 4
    INPUT_DWORD_COL = INPUT_WORD_COL + 1  'col 4
    OUTPUT_BIT_COL = INPUT_DWORD_COL + 1 'col  5
    OUTPUT_WORD_COL = OUTPUT_BIT_COL + 1 'col 6
    OUTPUT_DWORD_COL = OUTPUT_WORD_COL + 1 'col 6
    INPUT_SIZE_BYTES_COL = MODULE_NUMBER_COL + 7
    OUTPUT_SIZE_BYTES_COL = INPUT_SIZE_BYTES_COL + 1
    IP_MAP_PRM_BYTES_COL = OUTPUT_SIZE_BYTES_COL + 1
    MOD_ID_MSB_COL = OUTPUT_SIZE_BYTES_COL + 9
    MOD_ID_LSB_COL = MOD_ID_MSB_COL + 1
    ACYCLIC_ROW_NUMBER = 51
    
    class_offset = 0
    OP_WORD_VALUE_TABLE.enum_descr = ""
    OP_WORD_VALUE_TABLE.max = 0
    OP_WORD_VALUE_TABLE.min = 0
            OP_WORD_TABLE(class_offset).mod_num = 0
            OP_WORD_TABLE(class_offset).bit_num = 0
            OP_WORD_TABLE(class_offset).offset = class_offset
            OP_WORD_TABLE(class_offset).Display = (class_offset & "=" & "OP WORD NOT CONNECTED" & ";")
            OP_WORD_VALUE_TABLE.enum_descr = OP_WORD_VALUE_TABLE.enum_descr & OP_WORD_TABLE(class_offset).Display
    class_offset = class_offset + 1
    'While ((Worksheets("ProfiBus(IO Modules)").Cells(counter, Module_Number_COL) <> Empty) And (Worksheets("ProfiBus(IO Modules)").Cells(counter, Module_Number_COL).Value <> BEGIN_IGNORED_DATA))
     While (Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_NUMBER_COL).text <> BEGIN_IGNORED_DATA)
            For word_count = 0 To Worksheets("ProfiBus(IO Modules)").Cells(counter, OUTPUT_WORD_COL).value - 1
                OP_WORD_TABLE(class_offset).mod_num = Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_NUMBER_COL).value
                OP_WORD_TABLE(class_offset).bit_num = word_count
                OP_WORD_TABLE(class_offset).offset = class_offset
                'OP_WORD_TABLE(class_offset).Display = ("" & class_offset & "=" & "MOD " & OP_WORD_TABLE(class_offset).mod_num & "Cyclic Rx- WORD" & word_count & ";")
                OP_WORD_TABLE(class_offset).Display = ("" & class_offset & "=" & Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_DESCRIPTION_COL).text & " WORD" & word_count & ";")
                OP_WORD_VALUE_TABLE.enum_descr = OP_WORD_VALUE_TABLE.enum_descr & OP_WORD_TABLE(class_offset).Display
                If (OP_WORD_VALUE_TABLE.min > OP_WORD_TABLE(class_offset).offset) Then
                    OP_WORD_VALUE_TABLE.min = OP_WORD_TABLE(class_offset).offset
                End If
                If (OP_WORD_VALUE_TABLE.max < OP_WORD_TABLE(class_offset).offset) Then
                    OP_WORD_VALUE_TABLE.max = OP_WORD_TABLE(class_offset).offset
                End If
                class_offset = class_offset + 1
            Next
            OP_WORD_VALUE_TABLE.default = OP_WORD_TABLE(0).offset
    
    counter = counter + 1
    Wend
    
    START_ROW = 12
    DATA_DEFINE_COL = 2
    GSD_DATATYPE_DEFINE_COL = 4
    DATA_PROFIBUS_REG_COL = 6
    
    class_offset = 0
    OUTPUT_PARAM_LIST(1).enum_descr = ""
    OUTPUT_PARAM_LIST(1).max = 0
    OUTPUT_PARAM_LIST(1).min = 0
    
    counter = START_ROW
    
    While (Worksheets("ProfiBus (DCI)").Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            If ((Worksheets("ProfiBus (DCI)").Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC OUTPUT WORD")) Then
                dci_sheet_row = Find_DCID_Row(Worksheets("ProfiBus (DCI)").Cells(counter, DATA_DEFINE_COL))

                description_content_string = Worksheets("DCI Descriptors").Cells(dci_sheet_row, DCI_DESCRIPTORS_COL).text
                
                Display = (description_content_string & ",")
                OUTPUT_PARAM_LIST(1).enum_descr = OUTPUT_PARAM_LIST(1).enum_descr & Display
                OUTPUT_PARAM_LIST(1).max = OUTPUT_PARAM_LIST(1).max + 1
            End If
    counter = counter + 1
    Wend
    
    'Acyclic Write Word configurations
    For word_count = 0 To Worksheets("ProfiBus(IO Modules)").Cells(ACYCLIC_ROW_NUMBER, OUTPUT_WORD_COL).value - 1
        OP_WORD_TABLE(class_offset).mod_num = Worksheets("ProfiBus(IO Modules)").Cells(ACYCLIC_ROW_NUMBER, MODULE_NUMBER_COL).value
        OP_WORD_TABLE(class_offset).bit_num = word_count
        OP_WORD_TABLE(class_offset).offset = class_offset
        OP_WORD_TABLE(class_offset).Display = ("" & class_offset & "=" & Worksheets("ProfiBus(IO Modules)").Cells(ACYCLIC_ROW_NUMBER, MODULE_DESCRIPTION_COL).text & " Word" & word_count & ";")
        OP_WORD_VALUE_TABLE.enum_descr = OP_WORD_VALUE_TABLE.enum_descr & OP_WORD_TABLE(class_offset).Display
        If (OP_WORD_VALUE_TABLE.min > OP_WORD_TABLE(class_offset).offset) Then
            OP_WORD_VALUE_TABLE.min = OP_WORD_TABLE(class_offset).offset
        End If
        If (OP_WORD_VALUE_TABLE.max < OP_WORD_TABLE(class_offset).offset) Then
            OP_WORD_VALUE_TABLE.max = OP_WORD_TABLE(class_offset).offset
        End If
        class_offset = class_offset + 1
    Next
    OP_WORD_VALUE_TABLE.default = OP_WORD_TABLE(0).offset
    
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
Public Function Fill_OUTPUT_DWORD_TABLE()
    
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    Profibus_IO_Module_START_ROW = 26
    counter = Profibus_IO_Module_START_ROW
    Module = 1
    counter = Profibus_IO_Module_START_ROW
    MODULE_DESCRIPTION_COL = 1 'col 1
    MODULE_NUMBER_COL = MODULE_DESCRIPTION_COL + 1 'col 2
    INPUT_BIT_COL = MODULE_NUMBER_COL + 1 'col 3
    INPUT_WORD_COL = INPUT_BIT_COL + 1  'col 4
    INPUT_DWORD_COL = INPUT_WORD_COL + 1  'col 4
    OUTPUT_BIT_COL = INPUT_DWORD_COL + 1 'col  5
    OUTPUT_WORD_COL = OUTPUT_BIT_COL + 1 'col 6
    OUTPUT_DWORD_COL = OUTPUT_WORD_COL + 1 'col 6
    INPUT_SIZE_BYTES_COL = MODULE_NUMBER_COL + 7
    OUTPUT_SIZE_BYTES_COL = INPUT_SIZE_BYTES_COL + 1
    IP_MAP_PRM_BYTES_COL = OUTPUT_SIZE_BYTES_COL + 1
    MOD_ID_MSB_COL = OUTPUT_SIZE_BYTES_COL + 9
    MOD_ID_LSB_COL = MOD_ID_MSB_COL + 1
    ACYCLIC_ROW_NUMBER = 51
    
    class_offset = 0
    OP_DWORD_VALUE_TABLE.enum_descr = ""
    OP_DWORD_VALUE_TABLE.max = 0
    OP_DWORD_VALUE_TABLE.min = 0
            OP_DWORD_TABLE(class_offset).mod_num = 0
            OP_DWORD_TABLE(class_offset).bit_num = 0
            OP_DWORD_TABLE(class_offset).offset = class_offset
            OP_DWORD_TABLE(class_offset).Display = (class_offset & "=" & "OP DWORD NOT CONNECTED" & ";")
            OP_DWORD_VALUE_TABLE.enum_descr = OP_DWORD_VALUE_TABLE.enum_descr & OP_DWORD_TABLE(class_offset).Display
    class_offset = class_offset + 1
    'While ((Worksheets("ProfiBus(IO Modules)").Cells(counter, Module_Number_COL) <> Empty) And (Worksheets("ProfiBus(IO Modules)").Cells(counter, Module_Number_COL).Value <> BEGIN_IGNORED_DATA))
     While (Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_NUMBER_COL).text <> BEGIN_IGNORED_DATA)
            For word_count = 0 To Worksheets("ProfiBus(IO Modules)").Cells(counter, OUTPUT_DWORD_COL).value - 1
                OP_DWORD_TABLE(class_offset).mod_num = Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_NUMBER_COL).value
                OP_DWORD_TABLE(class_offset).bit_num = word_count
                OP_DWORD_TABLE(class_offset).offset = class_offset
                'OP_DWORD_TABLE(class_offset).Display = ("" & class_offset & "=" & "MOD " & OP_DWORD_TABLE(class_offset).mod_num & "Cyclic Rx- DWORD" & word_count & ";")
                OP_DWORD_TABLE(class_offset).Display = ("" & class_offset & "=" & Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_DESCRIPTION_COL).text & " DWORD" & word_count & ";")
                OP_DWORD_VALUE_TABLE.enum_descr = OP_DWORD_VALUE_TABLE.enum_descr & OP_DWORD_TABLE(class_offset).Display
                If (OP_DWORD_VALUE_TABLE.min > OP_DWORD_TABLE(class_offset).offset) Then
                    OP_DWORD_VALUE_TABLE.min = OP_DWORD_TABLE(class_offset).offset
                End If
                If (OP_DWORD_VALUE_TABLE.max < OP_DWORD_TABLE(class_offset).offset) Then
                    OP_DWORD_VALUE_TABLE.max = OP_DWORD_TABLE(class_offset).offset
                End If
                class_offset = class_offset + 1
            Next
            OP_DWORD_VALUE_TABLE.default = OP_DWORD_TABLE(0).offset
    
    counter = counter + 1
    Wend
    
    START_ROW = 12
    DATA_DEFINE_COL = 2
    GSD_DATATYPE_DEFINE_COL = 4
    DATA_PROFIBUS_REG_COL = 6
    
    class_offset = 0
    OUTPUT_PARAM_LIST(2).enum_descr = ""
    OUTPUT_PARAM_LIST(2).max = 0
    OUTPUT_PARAM_LIST(2).min = 0
    
    counter = START_ROW
    
    While (Worksheets("ProfiBus (DCI)").Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            If ((Worksheets("ProfiBus (DCI)").Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC OUTPUT DWORD")) Then
                dci_sheet_row = Find_DCID_Row(Worksheets("ProfiBus (DCI)").Cells(counter, DATA_DEFINE_COL))

                description_content_string = Worksheets("DCI Descriptors").Cells(dci_sheet_row, DCI_DESCRIPTORS_COL).text
                
                Display = (description_content_string & ",")
                OUTPUT_PARAM_LIST(2).enum_descr = OUTPUT_PARAM_LIST(2).enum_descr & Display
                OUTPUT_PARAM_LIST(2).max = OUTPUT_PARAM_LIST(2).max + 1
            End If
    counter = counter + 1
    Wend

    'Acyclic Write Word configurations
    For word_count = 0 To Worksheets("ProfiBus(IO Modules)").Cells(ACYCLIC_ROW_NUMBER, OUTPUT_DWORD_COL).value - 1
        OP_DWORD_TABLE(class_offset).mod_num = Worksheets("ProfiBus(IO Modules)").Cells(ACYCLIC_ROW_NUMBER, MODULE_NUMBER_COL).value
        OP_DWORD_TABLE(class_offset).bit_num = word_count
        OP_DWORD_TABLE(class_offset).offset = class_offset
        OP_DWORD_TABLE(class_offset).Display = ("" & class_offset & "=" & Worksheets("ProfiBus(IO Modules)").Cells(ACYCLIC_ROW_NUMBER, MODULE_DESCRIPTION_COL).text & " DWord" & word_count & ";")
        OP_DWORD_VALUE_TABLE.enum_descr = OP_DWORD_VALUE_TABLE.enum_descr & OP_DWORD_TABLE(class_offset).Display
        If (OP_DWORD_VALUE_TABLE.min > OP_DWORD_TABLE(class_offset).offset) Then
            OP_DWORD_VALUE_TABLE.min = OP_DWORD_TABLE(class_offset).offset
        End If
        If (OP_DWORD_VALUE_TABLE.max < OP_DWORD_TABLE(class_offset).offset) Then
            OP_DWORD_VALUE_TABLE.max = OP_DWORD_TABLE(class_offset).offset
        End If
        class_offset = class_offset + 1
    Next
    OP_DWORD_VALUE_TABLE.default = OP_DWORD_TABLE(0).offset
    
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
Public Function Fill_OUTPUT_BIT_TABLE()
    
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    Profibus_IO_Module_START_ROW = 26
    counter = Profibus_IO_Module_START_ROW
    Module = 1
    counter = Profibus_IO_Module_START_ROW
    MODULE_DESCRIPTION_COL = 1 'col 1
    MODULE_NUMBER_COL = MODULE_DESCRIPTION_COL + 1 'col 2
    INPUT_BIT_COL = MODULE_NUMBER_COL + 1 'col 3
    INPUT_WORD_COL = INPUT_BIT_COL + 1  'col 4
    INPUT_DWORD_COL = INPUT_WORD_COL + 1  'col 4
    OUTPUT_BIT_COL = INPUT_DWORD_COL + 1 'col  5
    OUTPUT_WORD_COL = OUTPUT_BIT_COL + 1 'col 6
    OUTPUT_DWORD_COL = OUTPUT_WORD_COL + 1 'col 6
    INPUT_SIZE_BYTES_COL = MODULE_NUMBER_COL + 7
    OUTPUT_SIZE_BYTES_COL = INPUT_SIZE_BYTES_COL + 1
    IP_MAP_PRM_BYTES_COL = OUTPUT_SIZE_BYTES_COL + 1
    MOD_ID_MSB_COL = OUTPUT_SIZE_BYTES_COL + 9
    MOD_ID_LSB_COL = MOD_ID_MSB_COL + 1
    ACYCLIC_ROW_NUMBER = 51
    
    class_offset = 0
    OP_BIT_VALUE_TABLE.enum_descr = ""
    OP_BIT_VALUE_TABLE.max = 0
    OP_BIT_VALUE_TABLE.min = 0
            OP_BIT_TABLE(class_offset).mod_num = 0
            OP_BIT_TABLE(class_offset).bit_num = 0
            OP_BIT_TABLE(class_offset).offset = class_offset
            OP_BIT_TABLE(class_offset).Display = (class_offset & "=" & "OP BIT NOT CONNECTED" & ";")
            OP_BIT_VALUE_TABLE.enum_descr = OP_BIT_VALUE_TABLE.enum_descr & OP_BIT_TABLE(class_offset).Display
    class_offset = class_offset + 1
    While (Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_NUMBER_COL).text <> BEGIN_IGNORED_DATA)
            For bit_count = 0 To Worksheets("ProfiBus(IO Modules)").Cells(counter, OUTPUT_BIT_COL).value - 1
                OP_BIT_TABLE(class_offset).mod_num = Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_NUMBER_COL).value
                OP_BIT_TABLE(class_offset).bit_num = bit_count
                OP_BIT_TABLE(class_offset).offset = class_offset
                OP_BIT_TABLE(class_offset).Display = ("" & class_offset & "=" & Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_DESCRIPTION_COL).text & " Bit" & bit_count & ";")
                OP_BIT_VALUE_TABLE.enum_descr = OP_BIT_VALUE_TABLE.enum_descr & OP_BIT_TABLE(class_offset).Display
                If (OP_BIT_VALUE_TABLE.min > OP_BIT_TABLE(class_offset).offset) Then
                    OP_BIT_VALUE_TABLE.min = OP_BIT_TABLE(class_offset).offset
                End If
                If (OP_BIT_VALUE_TABLE.max < OP_BIT_TABLE(class_offset).offset) Then
                    OP_BIT_VALUE_TABLE.max = OP_BIT_TABLE(class_offset).offset
                End If
                class_offset = class_offset + 1
            Next
            OP_BIT_VALUE_TABLE.default = OP_BIT_TABLE(0).offset
    
    counter = counter + 1
    Wend
    
    Const MAX_ENUM_NUM = 100
    START_ROW = 12
    DATA_DEFINE_COL = 2
    GSD_DATATYPE_DEFINE_COL = 4
    DATA_PROFIBUS_REG_COL = 6
    
    Dim stEnumObTextId(MAX_ENUM_NUM) As String
    
    class_offset = 0
    OUTPUT_PARAM_LIST(0).enum_descr = ""
    OUTPUT_PARAM_LIST(0).max = 0
    OUTPUT_PARAM_LIST(0).min = 0
    
    counter = START_ROW
    While (Worksheets("ProfiBus (DCI)").Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            If ((Worksheets("ProfiBus (DCI)").Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC OUTPUT BIT")) Then
            
            dci_sheet_row = Find_DCID_Row_Search_Desc_Col(Worksheets("ProfiBus (DCI)").Cells(counter, 1))
            DESCRIPTION_COL = 81
            description_content_string = Worksheets("DCI Descriptors").Cells(dci_sheet_row, DESCRIPTION_COL).text
            stEnumOb = Split(description_content_string, vbLf)
            
                For bit_count = 0 To UBound(stEnumOb) - 1
                                stEnumTemp = Split(stEnumOb(bit_count + 1), " = ")
                                If stEnumOb(bit_count) <> "" Then
                                    'stEnumObValue(bit_count) = stEnumTemp(0)
                                    stEnumObTextId(bit_count) = stEnumTemp(1)
                                Else
                                    upLimit = upLimit - 1
                                End If
                    Display = (stEnumObTextId(bit_count) & ",")
                    OUTPUT_PARAM_LIST(0).enum_descr = OUTPUT_PARAM_LIST(0).enum_descr & Display
                    OUTPUT_PARAM_LIST(0).max = OUTPUT_PARAM_LIST(0).max + 1
                    class_offset = class_offset + 1
                Next
                    
            End If
    counter = counter + 1
    Wend
    
    'Acyclic Write Bit configurations
    For bit_count = 0 To Worksheets("ProfiBus(IO Modules)").Cells(ACYCLIC_ROW_NUMBER, OUTPUT_BIT_COL).value - 1
        OP_BIT_TABLE(class_offset).mod_num = Worksheets("ProfiBus(IO Modules)").Cells(ACYCLIC_ROW_NUMBER, MODULE_NUMBER_COL).value
        OP_BIT_TABLE(class_offset).bit_num = bit_count
        OP_BIT_TABLE(class_offset).offset = class_offset
        OP_BIT_TABLE(class_offset).Display = ("" & class_offset & "=" & Worksheets("ProfiBus(IO Modules)").Cells(ACYCLIC_ROW_NUMBER, MODULE_DESCRIPTION_COL).text & " Bit" & bit_count & ";")
        OP_BIT_VALUE_TABLE.enum_descr = OP_BIT_VALUE_TABLE.enum_descr & OP_BIT_TABLE(class_offset).Display
        If (OP_BIT_VALUE_TABLE.min > OP_BIT_TABLE(class_offset).offset) Then
            OP_BIT_VALUE_TABLE.min = OP_BIT_TABLE(class_offset).offset
        End If
        If (OP_BIT_VALUE_TABLE.max < OP_BIT_TABLE(class_offset).offset) Then
            OP_BIT_VALUE_TABLE.max = OP_BIT_TABLE(class_offset).offset
        End If
        class_offset = class_offset + 1
    Next
    OP_BIT_VALUE_TABLE.default = OP_BIT_TABLE(0).offset
    
End Function



'********************************
'********************************
'Profibus GSD Construction sheet.
'********************************
'********************************
Sub Create_Profibus_GSD_File()

    Application.Calculation = xlCalculationManual

'*********************
'******     Constants
'*********************
    Dim MyDate
    MyDate = Date    ' MyDate contains the current system date.


    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    Dim Temp_Datatype_String As String
    Dim default
    Dim dci_sheet_row As Integer
    Dim start_range
    Dim end_range
    Dim Enum_val As Integer
                
    Const MAX_ENUM_NUM = 300
    Dim stEnumOb() As String
    Dim enum_object_number As Integer
    Dim stEnumTemp() As String
    Dim stEnumObValue(MAX_ENUM_NUM) As String
    Dim stEnumObTextId(MAX_ENUM_NUM) As String
                
    Dim param_text_counter As Integer
    'Profi (BCM) column settings
    START_ROW = 12
    DESCRIP_COL = 1
    DATA_DEFINE_COL = 2
    DATA_LENGTH_COL = 5
    DATA_PROFIBUS_REG_COL = 6
    DATA_NEXT_INDEX_COL = 5
    DATA_ATOMIBLOCK_SELECT = 5
    DATA_OFFSET_COL = 6
    'DATA_MIN_COL = 5 '7
    'DATA_MAX_COL = 6 '8
    DATA_TYPE_COL = 3
    DATA_TYPE_LENGTH_COL = 4
    GSD_DATATYPE_DEFINE_COL = 4
    
    'DCI Descriptors sheet column settings
    Dim DCI_DEFINES_START_ROW As Integer
    DCI_DEFINES_START_ROW = 9
    Dim DCI_DESCRIPTORS_COL As Integer
    DCI_DESCRIPTORS_COL = 1
    Dim DCI_DEFINES_COL As Integer
    DCI_DEFINES_COL = 2
    Dim DCI_LENGTH_COL As Integer
    DCI_LENGTH_COL = 6
    DCI_DEFAULT_COL = 7
    DCI_MIN_COL = 8
    DCI_MAX_COL = 9
    DCI_ENUM_COL = 10
    DCI_ENUM_SUPP_COL = 35
    SHORT_DESCRIP_COL = 79
    DCI_ENUM_DESCRIPTION_COL = 85
    DCI_TYPE_COL = 3
    
    Const MAX_GENERAL_DP_KEYWORDS As Long = 14
    Const MAX_BAUDRATE_KEYWORDS As Long = 10
    Const MAX_SLAVE_SPEC_KEYWORDS As Long = 12
    Const MAX_DP_EXTN_KEYWORDS As Long = 17
        
    GENERAL_DP_KEYWORD_ROW_START = 24
    GENERAL_DP_KEYWORD_ROW_END = 37
    BAUD_RATE_ROW_START = 11
    BAUD_RATE_ROW_END = 20
    SLAVE_KEYWORD_ROW_START = 41
    SLAVE_KEYWORD_ROW_END = 52
    DP_EXTN_KEYWORD_ROW_START = 60
    DP_EXTN_KEYWORD_ROW_END = 76
    BAUD_RATE_VALUE_COL = 2
    BAUD_RATE_TSDR_COL = 3
    GENERAL_DP_KEYWORD_VALUE_COL = 2
    SLAVE_KEYWORD_VALUE_COL = 2
    DP_EXTN_VALUE_COL = 2
    TABLE_VALUE_COL = 1
        
    Dim BaudRates(MAX_BAUDRATE_KEYWORDS) As String
    BaudRates(0) = "9.6_supp"
    BaudRates(1) = "19.2_supp"
    BaudRates(2) = "45.45_supp"
    BaudRates(3) = "93.75_supp"
    BaudRates(4) = "187.5_supp"
    BaudRates(5) = "500_supp"
    BaudRates(6) = "1.5M_supp"
    BaudRates(7) = "3M_supp"
    BaudRates(8) = "6M_supp"
    BaudRates(9) = "12M_supp"
        
    Dim BaudRates_Max_Tsdr(MAX_BAUDRATE_KEYWORDS) As String
    BaudRates_Max_Tsdr(0) = "MaxTsdr_9.6"
    BaudRates_Max_Tsdr(1) = "MaxTsdr_19.2"
    BaudRates_Max_Tsdr(2) = "MaxTsdr_45.45"
    BaudRates_Max_Tsdr(3) = "MaxTsdr_93.75"
    BaudRates_Max_Tsdr(4) = "MaxTsdr_187.5"
    BaudRates_Max_Tsdr(5) = "MaxTsdr_500"
    BaudRates_Max_Tsdr(6) = "MaxTsdr_1.5M"
    BaudRates_Max_Tsdr(7) = "MaxTsdr_3M"
    BaudRates_Max_Tsdr(8) = "MaxTsdr_6M"
    BaudRates_Max_Tsdr(9) = "MaxTsdr_12M"
        
        
    Dim GeneralDPKeyWords(MAX_GENERAL_DP_KEYWORDS) As String
    GeneralDPKeyWords(0) = "GSD_Revision"
    GeneralDPKeyWords(1) = "Vendor_Name"
    GeneralDPKeyWords(2) = "Model_Name"
    GeneralDPKeyWords(3) = "Revision"
    GeneralDPKeyWords(4) = "Ident_Number"
    GeneralDPKeyWords(5) = "Protocol_Ident"
    GeneralDPKeyWords(6) = "Station_Type"
    GeneralDPKeyWords(7) = "FMS_supp"
    GeneralDPKeyWords(8) = "Hardware_Release"
    GeneralDPKeyWords(9) = "Software_Release"
    GeneralDPKeyWords(10) = "Redundancy"
    GeneralDPKeyWords(11) = "Repeater_Ctrl_Sig"
    GeneralDPKeyWords(12) = "24V_Pins"
    GeneralDPKeyWords(13) = "Implementation_Type"
        
        
    Dim SlaveSpecifications(MAX_SLAVE_SPEC_KEYWORDS) As String
    SlaveSpecifications(0) = "Freeze_Mode_supp"
    SlaveSpecifications(1) = "Sync_Mode_supp"
    SlaveSpecifications(2) = "Auto_Baud_supp"
    SlaveSpecifications(3) = "Set_Slave_Add_supp"
    SlaveSpecifications(4) = "Min_Slave_Intervall"
    SlaveSpecifications(5) = "Modular_Station"
    SlaveSpecifications(6) = "Max_Module"
    SlaveSpecifications(7) = "Fail_Safe"
    SlaveSpecifications(8) = "Max_Diag_Data_Len"
    SlaveSpecifications(9) = "Modul_Offset"
    SlaveSpecifications(10) = "Slave_Family"
    SlaveSpecifications(11) = "Info_Text"
        
        
    Dim DPExtensions(MAX_DP_EXTN_KEYWORDS) As String
    DPExtensions(0) = "DPV1_Slave"
    DPExtensions(1) = "C1_Read_Write_supp"
    DPExtensions(2) = "C1_Max_Data_Len"
    DPExtensions(3) = "C1_Response_Timeout"
    DPExtensions(4) = "Extra_Alarm_Sap_supp"
    DPExtensions(5) = "Diagnostic_Alarm_supp"
    DPExtensions(6) = "Process_Alarm_supp"
    DPExtensions(7) = "Alarm_Type_Mode_supp"
    DPExtensions(8) = "WD_Base_1ms_supp"
    DPExtensions(9) = "Publisher_supp"
    DPExtensions(10) = "C2_Read_Write_supp"
    DPExtensions(11) = "C2_Max_Data_Len"
    DPExtensions(12) = "C2_Response_Timeout"
    DPExtensions(13) = "C2_Max_Count_Channels"
    DPExtensions(14) = "Max_Initiate_PDU_Length"
    DPExtensions(15) = "DPV1_Data_Types"
    DPExtensions(16) = "Ident_Maintenance_supp"
           
    
    Profibus_IO_Module_START_ROW = 26
    Module = 1
    counter = Profibus_IO_Module_START_ROW
    MODULE_DESCRIPTION_COL = 1 'col 1
    MODULE_NUMBER_COL = MODULE_DESCRIPTION_COL + 1 'col 2
    INPUT_BIT_COL = MODULE_NUMBER_COL + 1 'col 3
    INPUT_WORD_COL = INPUT_BIT_COL + 1  'col 4
    INPUT_DWORD_COL = INPUT_WORD_COL + 1  'col 4
    OUTPUT_BIT_COL = INPUT_DWORD_COL + 1 'col  5
    OUTPUT_WORD_COL = OUTPUT_BIT_COL + 1 'col 6
    OUTPUT_DWORD_COL = OUTPUT_WORD_COL + 1 'col 6
    INPUT_SIZE_BYTES_COL = MODULE_NUMBER_COL + 7
    OUTPUT_SIZE_BYTES_COL = INPUT_SIZE_BYTES_COL + 1
    IP_MAP_PRM_BYTES_COL = OUTPUT_SIZE_BYTES_COL + 1
    MOD_ID_MSB_COL = OUTPUT_SIZE_BYTES_COL + 7
    MOD_ID_LSB_COL = MOD_ID_MSB_COL + 1
    
    DATA_CONFIG_MODULE_INPUT_BIT_SIZE_COL = 3
    DATA_CONFIG_MODULE_INPUT_WORD_SIZE_COL = 4
    DATA_CONFIG_MODULE_INPUT_DWORD_SIZE_COL = 5
    DATA_CONFIG_MODULE_OUTPUT_BIT_SIZE_COL = 6
    DATA_CONFIG_MODULE_OUTPUT_WORD_SIZE_COL = 7
    DATA_CONFIG_MODULE_OUTPUT_DWORD_SIZE_COL = 8

    DATA_CONFIG_MODULE_INPUT_SIZE_COL = 9
    DATA_CONFIG_MODULE_OUTPUT_SIZE_COL = 10
    
    ACYCLIC_ROW_NUMBER = 51
    MAX_MODULES = 15
    PARAMETERIZATION_WITH_MODULE_NUMBER_COL = 14 'q column assigned for this
    FAULT_CODE_ROW = 80
    FAULT_CODE_NUMBER_COL = 2
    FALUT_DESC_COL = 3
    
    Dim VALIDATION_LIST_ROW_SIZE_START As Integer
    Dim VALIDATION_LIST_ROW_START As Integer

    Const BEGIN_META_DATA = "BEGIN_META_DATA"
    META_DATA_COL_START = 27 '"AA"

    Const DEFAULT_VALIDATION_LIST_ROW_SIZE_START = 40
    Const DEFAULT_VALIDATION_LIST_ROW_START = 41

    Dim generate_custom_assignement As Boolean
    
    Fill_INPUT_WORD_TABLE
    Fill_INPUT_DWORD_TABLE
    Fill_INPUT_BIT_TABLE
    Fill_OUTPUT_BIT_TABLE
    Fill_OUTPUT_WORD_TABLE
    Fill_OUTPUT_DWORD_TABLE
'*********************
'******     Begin
'*********************
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select

'    Sheets("Profibus (GSD)").Select
    
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\" & Cells(2, 2).text & ".gsd"
    Set PROFIBUS_GSD = fs.CreateTextFile(file_path, True)
    
    'Making font type and size consistent.
    Worksheets(sheet_name).Range("A:N").Font.name = "Arial"
    Worksheets(sheet_name).Range("A:N").Font.Size = 10
    
'****************************************************************************************************************************
'******     Start Creating the .gsd file header
'****************************************************************************************************************************
    PROFIBUS_GSD.WriteLine (";************** GSD " & MyDate & "  LO TOOLKIT COMMUNICATION CARD ************************")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    PROFIBUS_GSD.WriteLine (";*       Vendor:    Eaton Corp.                                                          *")
    PROFIBUS_GSD.WriteLine (";*                  Industrial Control Division                                          *")
    PROFIBUS_GSD.WriteLine (";*                  W126, N7250 Flint Dr                                                 *")
    PROFIBUS_GSD.WriteLine (";*                  Menomonee Falls, WI, United States                                   *")
    PROFIBUS_GSD.WriteLine (";*                  Customer Support:00 1 440-523-4400                                   *")
    PROFIBUS_GSD.WriteLine (";*                                                                                       *")
    PROFIBUS_GSD.WriteLine (";*                  Copyright © Eaton Corporation. All Rights Reserved.                  *")
    PROFIBUS_GSD.WriteLine (";*                                                                                       *")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    PROFIBUS_GSD.WriteLine (";*                                                                                       *")
    PROFIBUS_GSD.WriteLine (";*      Function: LO TOOLKIT Communication Adapter                                       *")
    PROFIBUS_GSD.WriteLine (";*                                                                                       *")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    PROFIBUS_GSD.WriteLine (";*                                                                                       *")
    PROFIBUS_GSD.WriteLine (";*                                                                                       *")
    PROFIBUS_GSD.WriteLine (";*                                                                                       *")
    PROFIBUS_GSD.WriteLine (";*                                                                                       *")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine ("#Profibus_DP")
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    PROFIBUS_GSD.WriteLine (";************  General DP Keywords  ******************************************************")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    
    invalid_value = 0
    For gsd_count = 0 To MAX_GENERAL_DP_KEYWORDS - 1
        invalid_value = 0
        For VAL_START_ROW = GENERAL_DP_KEYWORD_ROW_START To GENERAL_DP_KEYWORD_ROW_END
            If (Cells(VAL_START_ROW, TABLE_VALUE_COL).text <> Empty) Then
                If (Cells(VAL_START_ROW, TABLE_VALUE_COL).text = GeneralDPKeyWords(gsd_count) And Cells(VAL_START_ROW, GENERAL_DP_KEYWORD_VALUE_COL).text <> Empty) Then
                    PROFIBUS_GSD.WriteLine (Cells(VAL_START_ROW, TABLE_VALUE_COL).text & " = " & Cells(VAL_START_ROW, GENERAL_DP_KEYWORD_VALUE_COL).value)
                    invalid_value = 1
                End If
            End If
        Next VAL_START_ROW
        If (invalid_value = 0) Then
                MsgBox "Error Contents For keyword " & GeneralDPKeyWords(gsd_count) & " Not found"
        End If
    Next gsd_count

    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    PROFIBUS_GSD.WriteLine (";************  Supported Baud Rates  *****************************************************")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    
        invalid_value = 0
        For gsd_count = 0 To MAX_BAUDRATE_KEYWORDS - 1
                invalid_value = 0
            For VAL_START_ROW = BAUD_RATE_ROW_START To BAUD_RATE_ROW_END
                If (Cells(VAL_START_ROW, TABLE_VALUE_COL).text <> Empty) Then
                    If (Cells(VAL_START_ROW, TABLE_VALUE_COL).text = BaudRates(gsd_count) And Cells(VAL_START_ROW, BAUD_RATE_TSDR_COL).text <> Empty And Cells(VAL_START_ROW, BAUD_RATE_VALUE_COL).text <> Empty) Then
                        PROFIBUS_GSD.WriteLine (Cells(VAL_START_ROW, TABLE_VALUE_COL).text & " = " & Cells(VAL_START_ROW, BAUD_RATE_VALUE_COL).value)
                        PROFIBUS_GSD.WriteLine (BaudRates_Max_Tsdr(gsd_count) & " = " & Cells(VAL_START_ROW, BAUD_RATE_TSDR_COL).value)
                        invalid_value = 1
                    End If
                End If
            Next VAL_START_ROW
            If (invalid_value = 0) Then
            MsgBox "Error Contents For keyword " & BaudRates(gsd_count) & " Not found"
            End If
        Next gsd_count
    
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    PROFIBUS_GSD.WriteLine (";************  Slave Specific Values  ****************************************************")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
        

        invalid_value = 0
        For gsd_count = 0 To MAX_SLAVE_SPEC_KEYWORDS - 1
                invalid_value = 0
            For VAL_START_ROW = SLAVE_KEYWORD_ROW_START To SLAVE_KEYWORD_ROW_END
                If (Cells(VAL_START_ROW, TABLE_VALUE_COL).text <> Empty) Then
                    If (Cells(VAL_START_ROW, TABLE_VALUE_COL).text = SlaveSpecifications(gsd_count) And Cells(VAL_START_ROW, SLAVE_KEYWORD_VALUE_COL).text <> Empty) Then
                        PROFIBUS_GSD.WriteLine (Cells(VAL_START_ROW, TABLE_VALUE_COL).text & " = " & Cells(VAL_START_ROW, SLAVE_KEYWORD_VALUE_COL).value)
                        invalid_value = 1
                    End If
                End If
            Next VAL_START_ROW
            If (invalid_value = 0) Then
                MsgBox "Error Contents For keyword " & SlaveSpecifications(gsd_count) & " Not found"
            End If
        Next gsd_count
    
    '*********************
    '******     Switch to ProfiBus(IO Modules) sheet
    '*********************
    Sheets("ProfiBus(IO Modules)").Select
    
    Dim Max_Input_Len As Integer
    Dim Max_Output_Len As Integer
    Max_Input_Len = 0
    Max_Output_Len = 0
    counter = Profibus_IO_Module_START_ROW
    While ((Cells(counter, 2) <> Empty) And (Cells(counter, 2) <> BEGIN_IGNORED_DATA))
        'Count input length
        If (Cells(counter, INPUT_SIZE_BYTES_COL).text <> Empty) Then
            'dci_sheet_row = Find_DCID_Row_Search_Desc_Col(Cells(counter, 1)) SSN commented
            Max_Input_Len = Max_Input_Len + Cells(counter, INPUT_SIZE_BYTES_COL).value
        End If
        'Count output length
        If (Cells(counter, OUTPUT_SIZE_BYTES_COL).text <> Empty) Then
            'dci_sheet_row = Find_DCID_Row_Search_Desc_Col(Cells(counter, 1)) SSN commented
            Max_Output_Len = Max_Output_Len + Cells(counter, OUTPUT_SIZE_BYTES_COL).value
        End If
        counter = counter + 1
    
    Wend
    
    
    PROFIBUS_GSD.WriteLine ("Max_Input_Len = " & Max_Input_Len)
    PROFIBUS_GSD.WriteLine ("Max_Output_Len = " & Max_Output_Len)
    Max_Output_Len = Max_Output_Len + Max_Input_Len
    PROFIBUS_GSD.WriteLine ("Max_Data_Len = " & Max_Output_Len)
    
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    PROFIBUS_GSD.WriteLine (";************  Prm-Text-Def_List  ********************************************************")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    
    '*********************
    '******     Switch to ProfiBus (DCI) sheet
    '*********************
    Sheets("ProfiBus (DCI)").Select
    param_text_counter = 1
    counter = START_ROW
     
        'Write the enum parameters for the IP BIT
                enum_contain_string = IP_BIT_VALUE_TABLE.enum_descr
                ExtUserPrmData_def_IP_BIT = IP_BIT_VALUE_TABLE.default
                stEnumOb = Split(enum_contain_string, ";")
                'Get Enumerate Value Information
                upLimit = UBound(stEnumOb)
                For enum_cnt = 0 To upLimit
                    stEnumTemp = Split(stEnumOb(enum_cnt), "=")
                    If stEnumOb(enum_cnt) <> "" Then
                        stEnumObValue(enum_cnt) = stEnumTemp(0)
                        stEnumObTextId(enum_cnt) = stEnumTemp(1)
                    Else
                        upLimit = upLimit - 1
                    End If
                Next enum_cnt
                
                PROFIBUS_GSD.WriteLine ("PrmText = " & param_text_counter)
                Prm_Text_Ref_IP_BIT = param_text_counter
                ' take the default value also
                For enum_cnt = 0 To upLimit
                    PROFIBUS_GSD.WriteLine ("Text(" & stEnumObValue(enum_cnt) & ") = " & Chr(&H22) & stEnumObTextId(enum_cnt) & Chr(&H22))
                Next enum_cnt
                PROFIBUS_GSD.WriteLine ("EndPrmText")
                param_text_counter = param_text_counter + 1
    'Write the enum parameters for the IP WORD
                enum_contain_string = IP_WORD_VALUE_TABLE.enum_descr
                stEnumOb = Split(enum_contain_string, ";")
                'Get Enumerate Value Information
                upLimit = UBound(stEnumOb)
                For enum_cnt = 0 To upLimit
                    stEnumTemp = Split(stEnumOb(enum_cnt), "=")
                    If stEnumOb(enum_cnt) <> "" Then
                        stEnumObValue(enum_cnt) = stEnumTemp(0)
                        stEnumObTextId(enum_cnt) = stEnumTemp(1)
                    Else
                        upLimit = upLimit - 1
                    End If
                Next enum_cnt
                
                PROFIBUS_GSD.WriteLine ("PrmText = " & param_text_counter)
                Prm_Text_Ref_IP_WORD = param_text_counter
                'Change,we will always keep the motor management profile word in last two location to handle it seperately
                For enum_cnt = 0 To (upLimit) ' - 2) RajeshC profi
                    PROFIBUS_GSD.WriteLine ("Text(" & stEnumObValue(enum_cnt) & ") = " & Chr(&H22) & stEnumObTextId(enum_cnt) & Chr(&H22))
                Next enum_cnt
                PROFIBUS_GSD.WriteLine ("EndPrmText")
                param_text_counter = param_text_counter + 1
                
       'Write the enum parameters for the IP DWORD
                enum_contain_string = IP_DWORD_VALUE_TABLE.enum_descr
                stEnumOb = Split(enum_contain_string, ";")
                'Get Enumerate Value Information
                upLimit = UBound(stEnumOb)
                For enum_cnt = 0 To upLimit
                    stEnumTemp = Split(stEnumOb(enum_cnt), "=")
                    If stEnumOb(enum_cnt) <> "" Then
                        stEnumObValue(enum_cnt) = stEnumTemp(0)
                        stEnumObTextId(enum_cnt) = stEnumTemp(1)
                    Else
                        upLimit = upLimit - 1
                    End If
                Next enum_cnt
                
                PROFIBUS_GSD.WriteLine ("PrmText = " & param_text_counter)
                Prm_Text_Ref_IP_DWORD = param_text_counter
                'Change,we will always keep the motor management profile word in last two location to handle it seperately
                For enum_cnt = 0 To (upLimit) ' - 2) RajeshC profi
                    PROFIBUS_GSD.WriteLine ("Text(" & stEnumObValue(enum_cnt) & ") = " & Chr(&H22) & stEnumObTextId(enum_cnt) & Chr(&H22))
                Next enum_cnt
                PROFIBUS_GSD.WriteLine ("EndPrmText")
                param_text_counter = param_text_counter + 1
    'Write the enum parameters for the OP BIT
                enum_contain_string = OP_BIT_VALUE_TABLE.enum_descr
                stEnumOb = Split(enum_contain_string, ";")
                'Get Enumerate Value Information
                upLimit = UBound(stEnumOb)
                For enum_cnt = 0 To upLimit
                    stEnumTemp = Split(stEnumOb(enum_cnt), "=")
                    If stEnumOb(enum_cnt) <> "" Then
                        stEnumObValue(enum_cnt) = stEnumTemp(0)
                        stEnumObTextId(enum_cnt) = stEnumTemp(1)
                    Else
                        upLimit = upLimit - 1
                    End If
                Next enum_cnt
                PROFIBUS_GSD.WriteLine ("PrmText = " & param_text_counter)
                Prm_Text_Ref_OP_BIT = param_text_counter
                For enum_cnt = 0 To upLimit
                    PROFIBUS_GSD.WriteLine ("Text(" & stEnumObValue(enum_cnt) & ") = " & Chr(&H22) & stEnumObTextId(enum_cnt) & Chr(&H22))
                Next enum_cnt
                PROFIBUS_GSD.WriteLine ("EndPrmText")
                param_text_counter = param_text_counter + 1
    
    'Write the enum parameters for the OP WORD
                enum_contain_string = OP_WORD_VALUE_TABLE.enum_descr
                stEnumOb = Split(enum_contain_string, ";")
                'Get Enumerate Value Information
                upLimit = UBound(stEnumOb)
                For enum_cnt = 0 To upLimit
                    stEnumTemp = Split(stEnumOb(enum_cnt), "=")
                    If stEnumOb(enum_cnt) <> "" Then
                        stEnumObValue(enum_cnt) = stEnumTemp(0)
                        stEnumObTextId(enum_cnt) = stEnumTemp(1)
                    Else
                        upLimit = upLimit - 1
                    End If
                Next enum_cnt
                
                PROFIBUS_GSD.WriteLine ("PrmText = " & param_text_counter)
                Prm_Text_Ref_OP_WORD = param_text_counter
                For enum_cnt = 0 To upLimit
                'Skip index 1 which is reserved for Motor management Control Word
                '    If enum_cnt <> 1 Then
                        PROFIBUS_GSD.WriteLine ("Text(" & stEnumObValue(enum_cnt) & ") = " & Chr(&H22) & stEnumObTextId(enum_cnt) & Chr(&H22))
                '    End If
                Next enum_cnt
                PROFIBUS_GSD.WriteLine ("EndPrmText")
                param_text_counter = param_text_counter + 1
                
                
                'Write the enum parameters for the OP DWORD
                enum_contain_string = OP_DWORD_VALUE_TABLE.enum_descr
                stEnumOb = Split(enum_contain_string, ";")
                'Get Enumerate Value Information
                upLimit = UBound(stEnumOb)
                For enum_cnt = 0 To upLimit
                    stEnumTemp = Split(stEnumOb(enum_cnt), "=")
                    If stEnumOb(enum_cnt) <> "" Then
                        stEnumObValue(enum_cnt) = stEnumTemp(0)
                        stEnumObTextId(enum_cnt) = stEnumTemp(1)
                    Else
                        upLimit = upLimit - 1
                    End If
                Next enum_cnt
                
                PROFIBUS_GSD.WriteLine ("PrmText = " & param_text_counter)
                Prm_Text_Ref_OP_DWORD = param_text_counter
                For enum_cnt = 0 To upLimit
                'Skip index 1 which is reserved for Motor management Control Word
                '    If enum_cnt <> 1 Then
                        PROFIBUS_GSD.WriteLine ("Text(" & stEnumObValue(enum_cnt) & ") = " & Chr(&H22) & stEnumObTextId(enum_cnt) & Chr(&H22))
                '    End If
                Next enum_cnt
                PROFIBUS_GSD.WriteLine ("EndPrmText")
                param_text_counter = param_text_counter + 1
                
    ''Write the enum parameters for Motor Management profile Status Word and Current Word 'RajeshC Profi
             '   enum_contain_string = IP_WORD_VALUE_TABLE.enum_descr
             '  stEnumOb = Split(enum_contain_string, ";")
                'Get Enumerate Value Information
             '   upLimit = UBound(stEnumOb)
              '  For enum_cnt = 0 To upLimit
              '      stEnumTemp = Split(stEnumOb(enum_cnt), "=")
              '      If stEnumOb(enum_cnt) <> "" Then
              '          stEnumObValue(enum_cnt) = stEnumTemp(0)
              '          stEnumObTextId(enum_cnt) = stEnumTemp(1)
              '      Else
              '          upLimit = upLimit - 1
              '      End If
              '  Next enum_cnt
                
              '  PROFIBUS_GSD.WriteLine ("PrmText = " & param_text_counter)
              '  Prm_Text_Ref_MMP_Status_WORD = param_text_counter
              '  'Change,
              '  enum_cnt = (upLimit - 1)
              '  MMP_Status_Default_Value = (upLimit - 1)
              '  PROFIBUS_GSD.WriteLine ("Text(" & stEnumObValue(enum_cnt) & ") = " & Chr(&H22) & stEnumObTextId(enum_cnt) & Chr(&H22))
              '  PROFIBUS_GSD.WriteLine ("EndPrmText")
              '  param_text_counter = param_text_counter + 1
              '  PROFIBUS_GSD.WriteLine ("PrmText = " & param_text_counter)
              '  Prm_Text_Ref_MMP_Current_WORD = param_text_counter
               ' 'Change
               ' enum_cnt = upLimit
               ' MMP_Current_Default_Value = upLimit
               '     PROFIBUS_GSD.WriteLine ("Text(" & stEnumObValue(enum_cnt) & ") = " & Chr(&H22) & stEnumObTextId(enum_cnt) & Chr(&H22))
               ' PROFIBUS_GSD.WriteLine ("EndPrmText")
               ' param_text_counter = param_text_counter + 1
                
     ''Write the enum parameters for Motor Management control word
               ' enum_contain_string = OP_WORD_VALUE_TABLE.enum_descr
               ' stEnumOb = Split(enum_contain_string, ";")
               ' 'Get Enumerate Value Information
               ' upLimit = UBound(stEnumOb)
               ' For enum_cnt = 0 To upLimit
                  '  stEnumTemp = Split(stEnumOb(enum_cnt), "=")
                  '  If stEnumOb(enum_cnt) <> "" Then
                  '      stEnumObValue(enum_cnt) = stEnumTemp(0)
                  '      stEnumObTextId(enum_cnt) = stEnumTemp(1)
                  '  Else
                  '      upLimit = upLimit - 1
                  '  End If
               ' Next enum_cnt
                
               ' PROFIBUS_GSD.WriteLine ("PrmText = " & param_text_counter)
               ' Prm_Text_Ref_Control_WORD = param_text_counter
               ' enum_cnt = 1
               '     PROFIBUS_GSD.WriteLine ("Text(" & stEnumObValue(enum_cnt) & ") = " & Chr(&H22) & stEnumObTextId(enum_cnt) & Chr(&H22))
               ' PROFIBUS_GSD.WriteLine ("EndPrmText")
               ' param_text_counter = param_text_counter + 1
                
                'Enum values for INITIALIZATION PARAMETERs
                
                counter = START_ROW
                While ((Cells(counter, 2) <> Empty) And (Cells(counter, 2) <> BEGIN_IGNORED_DATA))
                   If (Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "INITIALIZATION PARAMETER") Then
                        dci_sheet_row = Find_DCID_Row_Search_Desc_Col(Cells(counter, 1))
                        'Enum Values
                        If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_ENUM_SUPP_COL).text <> Empty) Then
                            DESCRIPTION_COL = 81
                            PROFIBUS_GSD.WriteLine ("PrmText = " & param_text_counter)
                            description_content_string = Worksheets("DCI Descriptors").Cells(dci_sheet_row, DESCRIPTION_COL).text
                            descriptionString = Split(description_content_string, vbLf)
                            For Count = 0 To UBound(descriptionString) - 1
                                splitStr = Split(descriptionString(Count + 1), " = ")
                                PROFIBUS_GSD.WriteLine ("Text(" & Count & ") =  """ & splitStr(1) & """")
                            Next Count
                            PROFIBUS_GSD.WriteLine ("EndPrmText")
                            param_text_counter = param_text_counter + 1
                        End If
                    End If
                    counter = counter + 1
                Wend
                        
                                        
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    PROFIBUS_GSD.WriteLine (";************  User-Prm-Data-Def_List  ***************************************************")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    
' start Generate default assignement
If (Worksheets("ProfiBus(IO Modules)").Cells(2, 2).text <> "Custom Assignment") Then
    generate_custom_assignement = False
Else
    generate_custom_assignement = True
End If

If (generate_custom_assignement = False) Then

    ExtUserPrmData = 1
    Enum_val = 7        'LTK Specific Change we are supporting general profiles.not product specific profiles like MMP
    counter = START_ROW
    While ((Cells(counter, 2) <> Empty) And (Cells(counter, 2) <> BEGIN_IGNORED_DATA))
               If (Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "INITIALIZATION PARAMETER") Then
                    dci_sheet_row = Find_DCID_Row_Search_Desc_Col(Cells(counter, 1))
                    Get_Datatype_String (Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_TYPE_COL).text)
                    If (Get_Datatype_String1 = "uint8_t") Then
                        Temp_Datatype_String = "Unsigned8"
                    ElseIf (Get_Datatype_String1 = "int8_t") Then
                        Temp_Datatype_String = "Signed8 "
                    ElseIf (Get_Datatype_String1 = "uint16_t") Then
                        Temp_Datatype_String = "Unsigned16 "
                    ElseIf (Get_Datatype_String1 = "int16_t") Then
                        Temp_Datatype_String = "Signed16"
                    ElseIf (Get_Datatype_String1 = "uint32_t") Then
                        Temp_Datatype_String = "Unsigned32"
                    ElseIf (Get_Datatype_String1 = "SINT32") Then
                        Temp_Datatype_String = "Signed32"
                    ElseIf (Get_Datatype_String1 = "FLOAT") Then
                        Temp_Datatype_String = "float"
                    ElseIf (Get_Datatype_String1 = "LFLOAT") Then
                        Temp_Datatype_String = "lfloat"
                    ElseIf (Get_Datatype_String1 = "DFLOAT") Then
                        Temp_Datatype_String = "Unsigned8 "
                    ElseIf (Get_Datatype_String1 = "UINT64") Then
                        Temp_Datatype_String = "Unsigned8 "
                    End If
                    
                                  
                    'Default Value
                    default = Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_DEFAULT_COL).text
                    
                    'Ranges
                    If ((Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_MIN_COL).text <> Empty) And _
                        (Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_MAX_COL).text <> Empty)) Then
                        start_range = Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_MIN_COL).text
                        end_range = Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_MAX_COL).text
                    Else
                        start_range = ""
                        end_range = ""
                    End If
                    
                    PROFIBUS_GSD.WriteLine ("ExtUserPrmData = " & ExtUserPrmData & " """ & Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, SHORT_DESCRIP_COL).text & """")
                    PROFIBUS_GSD.WriteLine (Temp_Datatype_String & " " & default & " " & start_range & " - " & end_range)
                    'Enum Values
                    If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_ENUM_SUPP_COL).text <> Empty) Then
                            PROFIBUS_GSD.WriteLine ("Prm_Text_Ref = " & Enum_val)
                            PROFIBUS_GSD.WriteLine ("EndExtUserPrmData")
                            Enum_val = Enum_val + 1
                    Else
                        PROFIBUS_GSD.WriteLine ("EndExtUserPrmData")
                    End If
                    
                    ExtUserPrmData = ExtUserPrmData + 1
                
               End If 'Else '(Cells(counter, GSD_DATATYPE_DEFINE_COL).Text <> Empty)
    counter = counter + 1
    Wend
        
    counter = START_ROW
    While ((Cells(counter, 2) <> Empty) And (Cells(counter, 2) <> BEGIN_IGNORED_DATA))
                    offset = 4
                    Temp_Datatype_String = "Unsigned8"
                    dci_sheet_row = Find_DCID_Row_Search_Desc_Col(Cells(counter, 1))
         
                                If (Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC OUTPUT BIT") Then
                                    'Default Value
                                    default = OP_BIT_VALUE_TABLE.default
                                    'Ranges
                                    start_range = OP_BIT_VALUE_TABLE.min
                                    end_range = OP_BIT_VALUE_TABLE.max
                                    'Get the Parameter Bit Breakup and print it
                                    DESCRIPTION_COL = 81
                                    description_content_string = Worksheets("DCI Descriptors").Cells(dci_sheet_row, DESCRIPTION_COL).text
                                   ' description_content_string = OP_BIT_VALUE_TABLE.enum_descr
                                    descriptionString = Split(description_content_string, vbLf)
                                    
                                    'Dim Count As Integer
                                    For Count = 1 To UBound(descriptionString)
                                    splitStr = Split(descriptionString(Count), " = ")
                                    PROFIBUS_GSD.WriteLine ("ExtUserPrmData = " & ExtUserPrmData & " """ & splitStr(1) & """")
                                    PROFIBUS_GSD.WriteLine (Temp_Datatype_String & " " & default & " " & start_range & " - " & end_range)
                                    PROFIBUS_GSD.WriteLine ("Prm_Text_Ref = " & Prm_Text_Ref_OP_BIT)
                                    PROFIBUS_GSD.WriteLine ("EndExtUserPrmData")
                                    ExtUserPrmData = ExtUserPrmData + 1
                                    Next Count
                                End If
                                If ((Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC OUTPUT WORD")) Then
                                    If (Cells(counter, DESCRIP_COL).text = "Motor Control Word") Then
                                        default = 1
                                        start_range = 1
                                        end_range = 1
                                        PROFIBUS_GSD.WriteLine ("ExtUserPrmData = " & ExtUserPrmData & " """ & Cells(counter, DESCRIP_COL).text & """")
                                        PROFIBUS_GSD.WriteLine (Temp_Datatype_String & " " & default & " " & start_range & " - " & end_range)
                                        PROFIBUS_GSD.WriteLine ("Prm_Text_Ref = " & Prm_Text_Ref_Control_WORD)
                                        PROFIBUS_GSD.WriteLine ("EndExtUserPrmData")
                                        ExtUserPrmData = ExtUserPrmData + 1
                                    Else
                                        'Default Value,
                                        default = OP_WORD_VALUE_TABLE.default
                                        'Ranges
                                        start_range = OP_WORD_VALUE_TABLE.min
                                        end_range = OP_WORD_VALUE_TABLE.max
                                        PROFIBUS_GSD.WriteLine ("ExtUserPrmData = " & ExtUserPrmData & " """ & Cells(counter, DESCRIP_COL).text & """")
                                        PROFIBUS_GSD.WriteLine (Temp_Datatype_String & " " & default & " " & start_range & " - " & end_range)
                                        PROFIBUS_GSD.WriteLine ("Prm_Text_Ref = " & Prm_Text_Ref_OP_WORD)
                                        PROFIBUS_GSD.WriteLine ("EndExtUserPrmData")
                                        ExtUserPrmData = ExtUserPrmData + 1
                                    End If
                                    IP_BIT_DataRef_offset = ExtUserPrmData
                                End If
                                
                                If ((Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC OUTPUT DWORD")) Then
                                    If (Cells(counter, DESCRIP_COL).text = "Motor Control Word") Then
                                        default = 1
                                        start_range = 1
                                        end_range = 1
                                        PROFIBUS_GSD.WriteLine ("ExtUserPrmData = " & ExtUserPrmData & " """ & Cells(counter, DESCRIP_COL).text & """")
                                        PROFIBUS_GSD.WriteLine (Temp_Datatype_String & " " & default & " " & start_range & " - " & end_range)
                                        PROFIBUS_GSD.WriteLine ("Prm_Text_Ref = " & Prm_Text_Ref_Control_DWORD)
                                        PROFIBUS_GSD.WriteLine ("EndExtUserPrmData")
                                        ExtUserPrmData = ExtUserPrmData + 1
                                    Else
                                        'Default Value,
                                        default = OP_DWORD_VALUE_TABLE.default
                                        'Ranges
                                        start_range = OP_DWORD_VALUE_TABLE.min
                                        end_range = OP_DWORD_VALUE_TABLE.max
                                        PROFIBUS_GSD.WriteLine ("ExtUserPrmData = " & ExtUserPrmData & " """ & Cells(counter, DESCRIP_COL).text & """")
                                        PROFIBUS_GSD.WriteLine (Temp_Datatype_String & " " & default & " " & start_range & " - " & end_range)
                                        PROFIBUS_GSD.WriteLine ("Prm_Text_Ref = " & Prm_Text_Ref_OP_DWORD)
                                        PROFIBUS_GSD.WriteLine ("EndExtUserPrmData")
                                        ExtUserPrmData = ExtUserPrmData + 1
                                    End If
                                    IP_BIT_DataRef_offset = ExtUserPrmData
                                End If
                                If ((Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC OUTPUT BIT OUTPUT WORD") _
                                Or (Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC OUTPUT BIT OUTPUT DWORD")) Then
                                    'Default Value
                                    default = OP_BIT_VALUE_TABLE.default
                                    'Ranges
                                    start_range = OP_BIT_VALUE_TABLE.min
                                    end_range = OP_BIT_VALUE_TABLE.max
                                    'Get the Parameter Bit Breakup and print it
                                    DESCRIPTION_COL = 81
                                    description_content_string = Worksheets("DCI Descriptors").Cells(dci_sheet_row, DESCRIPTION_COL).text
                                   ' description_content_string = OP_BIT_VALUE_TABLE.enum_descr
                                    descriptionString = Split(description_content_string, vbLf)
                                    
                                    'Dim Count As Integer
                                    For Count = 1 To UBound(descriptionString)
                                    splitStr = Split(descriptionString(Count), " = ")
                                    PROFIBUS_GSD.WriteLine ("ExtUserPrmData = " & ExtUserPrmData & " """ & splitStr(1) & """")
                                    PROFIBUS_GSD.WriteLine (Temp_Datatype_String & " " & default & " " & start_range & " - " & end_range)
                                    PROFIBUS_GSD.WriteLine ("Prm_Text_Ref = " & Prm_Text_Ref_OP_BIT)
                                    PROFIBUS_GSD.WriteLine ("EndExtUserPrmData")
                                    ExtUserPrmData = ExtUserPrmData + 1
                                    Next Count
                                    
                                    'Default Value,
                                    default = OP_WORD_VALUE_TABLE.default
                                    'Ranges
                                    start_range = OP_WORD_VALUE_TABLE.min
                                    end_range = OP_WORD_VALUE_TABLE.max
                                    PROFIBUS_GSD.WriteLine ("ExtUserPrmData = " & ExtUserPrmData & " """ & Cells(counter, DESCRIP_COL).text & """")
                                    PROFIBUS_GSD.WriteLine (Temp_Datatype_String & " " & default & " " & start_range & " - " & end_range)
                                    PROFIBUS_GSD.WriteLine ("Prm_Text_Ref = " & Prm_Text_Ref_OP_WORD)
                                    PROFIBUS_GSD.WriteLine ("EndExtUserPrmData")
                                    ExtUserPrmData = ExtUserPrmData + 1
                                    IP_BIT_DataRef_offset = ExtUserPrmData
                                End If
        counter = counter + 1
    Wend
    
    'Build USR PRM DATA DEF LIST for INPUT Parameters () fom Profibus IO Module sheet
    Module = 1
    counter = Profibus_IO_Module_START_ROW
    MODULE_DESCRIPTION_COL = 1 'col 1
    MODULE_NUMBER_COL = MODULE_DESCRIPTION_COL + 1 'col 2
    INPUT_BIT_COL = MODULE_NUMBER_COL + 1 'col 3
    INPUT_WORD_COL = INPUT_BIT_COL + 1  'col 4
    INPUT_DWORD_COL = INPUT_WORD_COL + 1  'col 4
    INPUT_SIZE_BYTES_COL = MODULE_NUMBER_COL + 7
    OUTPUT_SIZE_BYTES_COL = INPUT_SIZE_BYTES_COL + 1
    IP_MAP_PRM_BYTES_COL = OUTPUT_SIZE_BYTES_COL + 1
    MOD_ID_MSB_COL = OUTPUT_SIZE_BYTES_COL + 7
    MOD_ID_LSB_COL = MOD_ID_MSB_COL + 1
    max_Input_bit_count = 0
    max_Input_word_count = 0
    max_Input_dword_count = 0
    While (Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_NUMBER_COL).text <> BEGIN_IGNORED_DATA)
        If (Worksheets("ProfiBus(IO Modules)").Cells(counter, INPUT_BIT_COL).value > max_Input_bit_count) Then
                max_Input_bit_count = Worksheets("ProfiBus(IO Modules)").Cells(counter, INPUT_BIT_COL).value
        End If
        If (Worksheets("ProfiBus(IO Modules)").Cells(counter, INPUT_WORD_COL).value > max_Input_word_count) Then
                max_Input_word_count = Worksheets("ProfiBus(IO Modules)").Cells(counter, INPUT_WORD_COL).value
        End If
        If (Worksheets("ProfiBus(IO Modules)").Cells(counter, INPUT_DWORD_COL).value > max_Input_dword_count) Then
                max_Input_dword_count = Worksheets("ProfiBus(IO Modules)").Cells(counter, INPUT_DWORD_COL).value
        End If
    counter = counter + 1
    Wend '(Worksheets("ProfiBus(IO Modules)").Cells(counter, Module_Number_COL).Text <> BEGIN_IGNORED_DATA)
    
    'Input Bit Default Value
    default = IP_BIT_VALUE_TABLE.default
    'Input Bit Ranges
    start_range = IP_BIT_VALUE_TABLE.min
    end_range = IP_BIT_VALUE_TABLE.max
    For Count = 0 To max_Input_bit_count - 1
        If (Count < end_range) Then
            default_ip_bit = Count + 1
        Else
            default_ip_bit = default
        End If
        'PROFIBUS_GSD.Writeline ("ExtUserPrmData = " & ExtUserPrmData & " """ & descriptionString(Count) & """")
        PROFIBUS_GSD.WriteLine ("ExtUserPrmData = " & ExtUserPrmData & " " & """" & "Input Bit " & Count & """")
        PROFIBUS_GSD.WriteLine (Temp_Datatype_String & " " & default_ip_bit & " " & start_range & " - " & end_range)
        PROFIBUS_GSD.WriteLine ("Prm_Text_Ref = " & Prm_Text_Ref_IP_BIT)
        PROFIBUS_GSD.WriteLine ("EndExtUserPrmData")
        ExtUserPrmData = ExtUserPrmData + 1
    Next Count
    
    IP_WORD_DataRef_offset = ExtUserPrmData
    'Input Word Default Value
    'default = Worksheets("ProfiBus(IO Modules)").Cells(INPUT_WORD_DEF_VALUE_ROW, INPUT_WORD_DEF_VALUE_COL).Value
    default = IP_WORD_VALUE_TABLE.default
    'Input WORD Ranges
    start_range = IP_WORD_VALUE_TABLE.min
    ' 2 words are for Motor mangement profile,they can't be configured for other IO Module
    ' that's why subtracting 2 below
    end_range = IP_WORD_VALUE_TABLE.max ' - 2 RajeshC Profi - 2
    For Count = 0 To max_Input_word_count - 1
        If (Count < end_range) Then
            default_ip_word = Count + 1
        Else
            default_ip_word = default
        End If
        PROFIBUS_GSD.WriteLine ("ExtUserPrmData = " & ExtUserPrmData & " " & """" & "Input Word " & Count & """")
        PROFIBUS_GSD.WriteLine (Temp_Datatype_String & " " & default_ip_word & " " & start_range & " - " & end_range)
        PROFIBUS_GSD.WriteLine ("Prm_Text_Ref = " & Prm_Text_Ref_IP_WORD)
        PROFIBUS_GSD.WriteLine ("EndExtUserPrmData")
        ExtUserPrmData = ExtUserPrmData + 1
    Next Count
    
    IP_DWORD_DataRef_offset = ExtUserPrmData
    'Input Word Default Value
    'default = Worksheets("ProfiBus(IO Modules)").Cells(INPUT_WORD_DEF_VALUE_ROW, INPUT_WORD_DEF_VALUE_COL).Value
    default = IP_DWORD_VALUE_TABLE.default
    'Input WORD Ranges
    start_range = IP_DWORD_VALUE_TABLE.min
    ' 2 words are for Motor mangement profile,they can't be configured for other IO Module
    ' that's why subtracting 2 below
    end_range = IP_DWORD_VALUE_TABLE.max ' - 2 RajeshC Profi - 2
    For Count = 0 To max_Input_dword_count - 1
        If (Count < end_range) Then
            default_ip_dword = Count + 1
        Else
            default_ip_dword = default
        End If
        PROFIBUS_GSD.WriteLine ("ExtUserPrmData = " & ExtUserPrmData & " " & """" & "Input DWord " & Count & """")
        PROFIBUS_GSD.WriteLine (Temp_Datatype_String & " " & default_ip_dword & " " & start_range & " - " & end_range)
        PROFIBUS_GSD.WriteLine ("Prm_Text_Ref = " & Prm_Text_Ref_IP_DWORD)
        PROFIBUS_GSD.WriteLine ("EndExtUserPrmData")
        ExtUserPrmData = ExtUserPrmData + 1
    Next Count
    ''MMP Input Status Word 'RajeshC Profi
    'PROFIBUS_GSD.WriteLine ("ExtUserPrmData = " & ExtUserPrmData & " " & """" & "Motor Status Word """)
    'PROFIBUS_GSD.WriteLine (Temp_Datatype_String & " " & MMP_Status_Default_Value & " " & MMP_Status_Default_Value & " - " & MMP_Status_Default_Value)
    'PROFIBUS_GSD.WriteLine ("Prm_Text_Ref = " & Prm_Text_Ref_MMP_Status_WORD)
    'PROFIBUS_GSD.WriteLine ("EndExtUserPrmData")
    'MMP_Status_Input_Offset = ExtUserPrmData
    'ExtUserPrmData = ExtUserPrmData + 1
    
    'MMP Current
    'PROFIBUS_GSD.WriteLine ("ExtUserPrmData = " & ExtUserPrmData & " " & """" & "Motor Current Word """)
    'PROFIBUS_GSD.WriteLine (Temp_Datatype_String & " " & MMP_Current_Default_Value & " " & MMP_Current_Default_Value & " - " & MMP_Current_Default_Value)
    'PROFIBUS_GSD.WriteLine ("Prm_Text_Ref = " & Prm_Text_Ref_MMP_Current_WORD)
    'PROFIBUS_GSD.WriteLine ("EndExtUserPrmData")
    'MMP_Current_Offset = EndExtUserPrmData
    'ExtUserPrmData = ExtUserPrmData + 1
    
    'Acyclic Read Bits configuration
    'Input Bit Ranges,Acyclic Read is input so it will have same range like input
    start_range = IP_BIT_VALUE_TABLE.min
    end_range = IP_BIT_VALUE_TABLE.max
    Acyclic_Read_Ref_Offset = ExtUserPrmData
    For bit_count = 0 To Worksheets("ProfiBus(IO Modules)").Cells(ACYCLIC_ROW_NUMBER, INPUT_BIT_COL).value - 1
    PROFIBUS_GSD.WriteLine ("ExtUserPrmData = " & ExtUserPrmData & " " & """" & "Acyclic Read Bit" & bit_count & """")
    PROFIBUS_GSD.WriteLine (Temp_Datatype_String & " " & default & " " & start_range & " - " & end_range)
    PROFIBUS_GSD.WriteLine ("Prm_Text_Ref = " & Prm_Text_Ref_IP_BIT)
    PROFIBUS_GSD.WriteLine ("EndExtUserPrmData")
    ExtUserPrmData = ExtUserPrmData + 1
    Next bit_count
    
    'Acyclic Read Word configuration
    'Input Bit Ranges,Acyclic Read is input so it will have same range like input
    start_range = IP_WORD_VALUE_TABLE.min
    ' 2 words are for Motor mangement profile,they can't be configured for other IO Module
    ' that's why subtracting 2 below
    end_range = IP_WORD_VALUE_TABLE.max '- 2 RajeshC  Profi - 2 as reserved for MMP
    Acyclic_Read_Word_Ref_Offset = ExtUserPrmData
    For word_count = 0 To Worksheets("ProfiBus(IO Modules)").Cells(ACYCLIC_ROW_NUMBER, INPUT_WORD_COL).value - 1
    PROFIBUS_GSD.WriteLine ("ExtUserPrmData = " & ExtUserPrmData & " " & """" & "Acyclic Read Word " & word_count & """")
    PROFIBUS_GSD.WriteLine (Temp_Datatype_String & " " & default & " " & start_range & " - " & end_range)
    PROFIBUS_GSD.WriteLine ("Prm_Text_Ref = " & Prm_Text_Ref_IP_WORD)
    PROFIBUS_GSD.WriteLine ("EndExtUserPrmData")
    ExtUserPrmData = ExtUserPrmData + 1
    Next word_count


Else ' else Generate custom assignement

    Const TOTAL_DATA_TYPE_TO_PROCESS = 6
    
INPUT_BIT_TYPE_1 = "CYCLIC INPUT BIT"
INPUT_WORD_TYPE_1 = "CYCLIC INPUT WORD"
INPUT_DWORD_TYPE_1 = "CYCLIC INPUT DWORD"
OUTPUT_BIT_TYPE_1 = "CYCLIC OUTPUT BIT"
OUTPUT_WORD_TYPE_1 = "CYCLIC OUTPUT WORD"
OUTPUT_DWORD_TYPE_1 = "CYCLIC OUTPUT DWORD"

Dim DATA_TYPE_LIST_1(1 To TOTAL_DATA_TYPE_TO_PROCESS) As String

DATA_TYPE_LIST_1(1) = INPUT_BIT_TYPE_1
DATA_TYPE_LIST_1(2) = INPUT_WORD_TYPE_1
DATA_TYPE_LIST_1(3) = INPUT_DWORD_TYPE_1
DATA_TYPE_LIST_1(4) = OUTPUT_BIT_TYPE_1
DATA_TYPE_LIST_1(5) = OUTPUT_WORD_TYPE_1
DATA_TYPE_LIST_1(6) = OUTPUT_DWORD_TYPE_1

Dim VALIDATION_LIST_COL(1 To TOTAL_DATA_TYPE_TO_PROCESS) As String
VALIDATION_LIST_COL(1) = "$AA"
VALIDATION_LIST_COL(2) = "$AB"
VALIDATION_LIST_COL(3) = "$AC"
VALIDATION_LIST_COL(4) = "$AD"
VALIDATION_LIST_COL(5) = "$AE"
VALIDATION_LIST_COL(6) = "$AF"
VALIDATION_LIST_ROW_SIZE_START = 40
VALIDATION_LIST_ROW_START = 41

Dim NOT_CONNECTED(1 To TOTAL_DATA_TYPE_TO_PROCESS) As String
NOT_CONNECTED(1) = "DCI_IP_BIT_NOT_CONNECTED"
NOT_CONNECTED(2) = "DCI_IP_WORD_NOT_CONNECTED"
NOT_CONNECTED(3) = "DCI_IP_DWORD_NOT_CONNECTED"
NOT_CONNECTED(4) = "DCI_OP_BIT_NOT_CONNECTED"
NOT_CONNECTED(5) = "DCI_OP_WORD_NOT_CONNECTED"
NOT_CONNECTED(6) = "DCI_OP_DWORD_NOT_CONNECTED"

Dim REF_NAME_LIST(1 To TOTAL_DATA_TYPE_TO_PROCESS) As String
REF_NAME_LIST(1) = "Input Bit"
REF_NAME_LIST(2) = "Input Word"
REF_NAME_LIST(3) = "Input DWord"
REF_NAME_LIST(4) = "Output Bit"
REF_NAME_LIST(5) = "Output Word"
REF_NAME_LIST(6) = "Output DWord"

Dim p_row_count As Integer
Dim Prm_Text_Ref_val As Integer

Prm_Text_Ref_val = 0

Module_Number_row_start = Profibus_IO_Module_START_ROW

p_row_count = Module_Number_row_start

    UASER_PARAM_FIRST_COL = 5
    Dim paramter_ref As String
    Dim user_valid_list() As String
    Dim single_conn() As String
    Dim connection_str As String
    ExtUserPrmData = 1
    Enum_val = TOTAL_DATA_TYPE_TO_PROCESS
    Dim temp_enum_support As Boolean
    
Profi_module_begin_ign_data_row = Profibus_IO_Module_START_ROW
    While ((Worksheets("ProfiBus(IO Modules)").Cells(Profi_module_begin_ign_data_row, MODULE_NUMBER_COL).text <> BEGIN_IGNORED_DATA))
        Profi_module_begin_ign_data_row = Profi_module_begin_ign_data_row + 1
    Wend
'''''''''''''''''''''''''''''''''''''''''''
row_counter = 1

While ((Worksheets("ProfiBus(IO Modules)").Cells(row_counter, META_DATA_COL_START).text <> BEGIN_META_DATA) And (row_counter < 5000))
row_counter = row_counter + 1
Wend
If row_counter < 5000 Then
        VALIDATION_LIST_ROW_SIZE_START = row_counter + 1
        VALIDATION_LIST_ROW_START = row_counter + 2
        meta_data_found = True
        
Else
        VALIDATION_LIST_ROW_SIZE_START = DEFAULT_VALIDATION_LIST_ROW_SIZE_START
        VALIDATION_LIST_ROW_START = DEFAULT_VALIDATION_LIST_ROW_START
        meta_data_found = False
        Worksheets("ProfiBus (IO Modules)").Cells(VALIDATION_LIST_ROW_SIZE_START - 1, META_DATA_COL_START).text = BEGIN_META_DATA
End If
        
    '''''''''''''''''''''''''''''''''''''''''''''''
    p_row_count = Profi_module_begin_ign_data_row + 2 + 2 ' space + heading + sub heading
    
    Dim prayas As Integer
    Dim type_offset As Integer
            
    While ((Worksheets("ProfiBus(IO Modules)").Cells(p_row_count, UASER_PARAM_FIRST_COL).text <> BEGIN_IGNORED_DATA))
        paramter_ref = Worksheets("ProfiBus(IO Modules)").Cells(p_row_count, UASER_PARAM_FIRST_COL + 1).text
        If (Worksheets("ProfiBus(IO Modules)").Cells(p_row_count, UASER_PARAM_FIRST_COL).text = "INITIALIZATION PARAMETER") Then

            
            dci_sheet_row = Find_DCID_Row_Search_Desc_Col(paramter_ref)
            If dci_sheet_row <> 0 Then
                 data_type_size = Worksheets("DCI Descriptors").Cells(dci_sheet_row, DATATYPE_SIZE).value
                    If data_type_size = 2 Then
                        Temp_Datatype_String = "Unsigned16"
                    ElseIf data_type_size = 4 Then
                        Temp_Datatype_String = "Unsigned32"
                    Else
                        Temp_Datatype_String = "Unsigned8"
                    End If
            End If
            
            Dim temp_r As Range
            Set temp_r = Range(Worksheets("ProfiBus(IO Modules)").Cells(p_row_count, UASER_PARAM_FIRST_COL + 2), Worksheets("ProfiBus(IO Modules)").Cells(p_row_count, UASER_PARAM_FIRST_COL + 2))
            temp_enum_support = False
            If (temp_r.Validation.Type = xlValidateList) Then
                connection_str = temp_r.Validation.Formula1
                user_valid_list = Split(connection_str, ",")
                single_conn = Split(user_valid_list(0), "=")
                min_param_val = single_conn(0)
                single_conn = Split(user_valid_list(UBound(user_valid_list) - 1), "=")
                max_param_val = single_conn(0)
                connection_str = Worksheets("ProfiBus(IO Modules)").Cells(p_row_count, UASER_PARAM_FIRST_COL + 2).text
                single_conn = Split(connection_str, "=")
                default_param_val = single_conn(0)
                Enum_val = Enum_val + 1
                temp_enum_support = True
            ElseIf (temp_r.Validation.Type = xlValidateWholeNumber) Then
                    min_param_val = temp_r.Validation.Formula1
                    max_param_val = temp_r.Validation.Formula2
                    default_param_val = Worksheets("ProfiBus(IO Modules)").Cells(p_row_count, UASER_PARAM_FIRST_COL + 2).text
                    If default_param_val = Empty Then
                    Application.GoTo ActiveWorkbook.Sheets("ProfiBus(IO Modules)").Cells(p_row_count, UASER_PARAM_FIRST_COL + 2)
                    ActiveCell.Interior.ColorIndex = 3
                    MsgBox ("<Connection> in <User Definable Parameters Configuration> cannot be left empty")
                    ActiveCell.Interior.ColorIndex = 2
                    End
                    End If
            End If
            PROFIBUS_GSD.WriteLine ("ExtUserPrmData = " & ExtUserPrmData & " """ & paramter_ref & """")
            PROFIBUS_GSD.WriteLine (Temp_Datatype_String & " " & default_param_val & " " & min_param_val & " - " & max_param_val)
            If temp_enum_support = True Then
                PROFIBUS_GSD.WriteLine ("Prm_Text_Ref = " & Enum_val)
            End If
            PROFIBUS_GSD.WriteLine ("EndExtUserPrmData")
            
            ExtUserPrmData = ExtUserPrmData + 1
        Else
        ' all output params
            
            Prm_Text_Ref_val = 0
            ' For now skip output and just work on input bits, word, dwod
            For prayas = 4 To TOTAL_DATA_TYPE_TO_PROCESS
                If (Worksheets("ProfiBus(IO Modules)").Cells(p_row_count, UASER_PARAM_FIRST_COL).text = DATA_TYPE_LIST_1(prayas)) Then
                        Prm_Text_Ref_val = prayas
    
                        temp_str_temp = VALIDATION_LIST_COL(prayas) & VALIDATION_LIST_ROW_SIZE_START
                        temp_str_temp = Worksheets("ProfiBus(IO Modules)").Range(temp_str_temp).value
                        If temp_str_temp > 0 Then
                            end_range = temp_str_temp - 1
                        End If
                        prayas = TOTAL_DATA_TYPE_TO_PROCESS
                End If
            Next prayas
            If Prm_Text_Ref_val <> 0 Then
                If (Worksheets("ProfiBus(IO Modules)").Cells(p_row_count, UASER_PARAM_FIRST_COL + 2).text) <> Empty Then
                        temp_str = Worksheets("ProfiBus(IO Modules)").Cells(p_row_count, UASER_PARAM_FIRST_COL + 2).text
                        find_offset = InStr(1, temp_str, "=", 0)
                        temp_str = Left(temp_str, find_offset - 1)
                        default = temp_str
                Else
                        temp_str = NOT_CONNECTED(Prm_Text_Ref_val)
                        default = "0"
                End If
                
                start_range = 0

                PROFIBUS_GSD.WriteLine ("ExtUserPrmData = " & ExtUserPrmData & " """ & paramter_ref & """")
                PROFIBUS_GSD.WriteLine (Temp_Datatype_String & " " & default & " " & start_range & " - " & end_range)
                PROFIBUS_GSD.WriteLine ("Prm_Text_Ref = " & Prm_Text_Ref_val)
                PROFIBUS_GSD.WriteLine ("EndExtUserPrmData")
                
                ExtUserPrmData = ExtUserPrmData + 1
            End If
        End If
        p_row_count = p_row_count + 1
    Wend

'///////// end code init param and output param(user definable configuration table)
    
    
    
'////////////////////////////new code start//////////////////////////
'Dim TOTAL_DATA_TYPE_TO_PROCESS As Integer

Module_Number_row_start = Profibus_IO_Module_START_ROW


Prm_Text_Ref_val = 0
        
    p_row_count = Profi_module_begin_ign_data_row + 2
    While ((Worksheets("ProfiBus(IO Modules)").Cells(p_row_count, 1).text <> BEGIN_IGNORED_DATA) And (p_row_count < 5000))
    If Worksheets("ProfiBus(IO Modules)").Cells(Module_Number_row_start, DATA_CONFIG_MODULE_INPUT_SIZE_COL).value > 0 Then
        While (Worksheets("ProfiBus(IO Modules)").Cells(p_row_count, 1).text <> "Module " & Worksheets("ProfiBus(IO Modules)").Cells(Module_Number_row_start, MODULE_NUMBER_COL).text & " Configuration")
            p_row_count = p_row_count + 1
        Wend
        Module_Number_row_start = Module_Number_row_start + 1
        p_row_count = p_row_count + 2
        'Dim prayas As Integer
        'Dim type_offset As Integer
        
        While ((Worksheets("ProfiBus(IO Modules)").Cells(p_row_count, 1).text <> Empty) And (Worksheets("ProfiBus(IO Modules)").Cells(p_row_count, 1).text <> BEGIN_IGNORED_DATA))
            
            Prm_Text_Ref_val = 0
            ' For now skip output and just work on input bits, word, dwod
            For prayas = 1 To TOTAL_DATA_TYPE_TO_PROCESS - 3
                If (Worksheets("ProfiBus(IO Modules)").Cells(p_row_count, 1).text = DATA_TYPE_LIST_1(prayas)) Then
                        Prm_Text_Ref_val = prayas
    
                        temp_str_temp = VALIDATION_LIST_COL(prayas) & VALIDATION_LIST_ROW_SIZE_START
                        temp_str_temp = Worksheets("ProfiBus(IO Modules)").Range(temp_str_temp).value
                        If temp_str_temp > 0 Then
                            end_range = temp_str_temp - 1
                        End If
                        type_offset = Worksheets("ProfiBus(IO Modules)").Cells(p_row_count, 2).value
                        prayas = TOTAL_DATA_TYPE_TO_PROCESS
                End If
            Next prayas
            If Prm_Text_Ref_val <> 0 Then
                If (Worksheets("ProfiBus(IO Modules)").Cells(p_row_count, 3).text) <> Empty Then
                        temp_str = Worksheets("ProfiBus(IO Modules)").Cells(p_row_count, 3).text
                        find_offset = InStr(1, temp_str, "=", 0)
                        'len_1 = Len(temp_str) ' - find_offset
                        'len_1 = len_1 - find_offset - 1
                        temp_str = Left(temp_str, find_offset - 1)
                        default = temp_str
                Else
                        temp_str = NOT_CONNECTED(Prm_Text_Ref_val)
                        default = "0"
                End If
                'PROFIBUS_GSD.WriteLine ("ExtUserPrmData = " & ExtUserPrmData & " """ & Worksheets("ProfiBus(IO Modules)").Cells(p_row_count, 3).text & """")
                PROFIBUS_GSD.WriteLine ("ExtUserPrmData = " & ExtUserPrmData & " """ & REF_NAME_LIST(Prm_Text_Ref_val) & " " & type_offset & """")
                PROFIBUS_GSD.WriteLine (Temp_Datatype_String & " " & default & " " & start_range & " - " & end_range)
                PROFIBUS_GSD.WriteLine ("Prm_Text_Ref = " & Prm_Text_Ref_val)
                PROFIBUS_GSD.WriteLine ("EndExtUserPrmData")
                
                ExtUserPrmData = ExtUserPrmData + 1
            End If
            p_row_count = p_row_count + 1
       Wend
       Else
            Module_Number_row_start = Module_Number_row_start + 1
       End If
    Wend
    'End If
'/////////////////////////// new code end////////////////////////////
End If
    
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    PROFIBUS_GSD.WriteLine (";************  User-Prm_Data  ************************************************************")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    
    Dim Ext_User_Prm_Data_Len As Integer
    Ext_User_Prm_Data_Len = 3     'This is includes the mandatory 3 bytes for DPV1
    Dim Ext_Module_Prm_Data_Len As Integer
    Ext_Module_Prm_Data_Len = 0
    Dim Ext_Total_Prm_Data_Len As Integer
    Ext_Total_Prm_Data_Len = 0
    counter = START_ROW
    While ((Cells(counter, 2) <> Empty) And (Cells(counter, 2) <> BEGIN_IGNORED_DATA))
         If (Cells(counter, GSD_DATATYPE_DEFINE_COL).text <> Empty) Then
            If (Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC OUTPUT BIT") Then
                dci_sheet_row = Find_DCID_Row_Search_Desc_Col(Cells(counter, 1))
                DESCRIPTION_COL = 81
                description_content_string = Worksheets("DCI Descriptors").Cells(dci_sheet_row, DESCRIPTION_COL).text
                descriptionString = Split(description_content_string, vbLf)
                'Dim Count As Integer
                For Count = 1 To UBound(descriptionString)
                    Ext_User_Prm_Data_Len = Ext_User_Prm_Data_Len + 1 ' For 1 byte Parameterization Technique every parameter is a 1 byte size
                Next Count
            End If
            If ((Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC OUTPUT WORD") _
                Or (Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC OUTPUT DWORD")) Then
                Ext_User_Prm_Data_Len = Ext_User_Prm_Data_Len + 1 ' For 1 byte Parameterization Technique every parameter is a 1 byte size
            End If
            If ((Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC OUTPUT BIT OUTPUT WORD") _
            Or (Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC OUTPUT BIT OUTPUT DWORD")) Then
                dci_sheet_row = Find_DCID_Row_Search_Desc_Col(Cells(counter, 1))
                DESCRIPTION_COL = 81
                description_content_string = Worksheets("DCI Descriptors").Cells(dci_sheet_row, DESCRIPTION_COL).text
                descriptionString = Split(description_content_string, vbLf)
                'Dim Count As Integer
                For Count = 1 To UBound(descriptionString)
                    Ext_User_Prm_Data_Len = Ext_User_Prm_Data_Len + 1 ' For 1 byte Parameterization Technique every parameter is a 1 byte size
                Next Count
                Ext_User_Prm_Data_Len = Ext_User_Prm_Data_Len + 1 ' For 1 byte Parameterization Technique every parameter is a 1 byte size
            End If
            If (Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "INITIALIZATION PARAMETER") Then
                Ext_User_Prm_Data_Len = Ext_User_Prm_Data_Len + 1 ' For 1 byte Parameterization Technique every parameter is a 1 byte size
            End If
            'Ext_User_Prm_Data_Len = Ext_User_Prm_Data_Len + Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_LENGTH_COL).Value
        End If
        counter = counter + 1
    Wend
    
    
    counter = Profibus_IO_Module_START_ROW
    
    While (Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_NUMBER_COL).text <> BEGIN_IGNORED_DATA)
            Ext_Module_Prm_Data_Len = Ext_Module_Prm_Data_Len + Worksheets("ProfiBus(IO Modules)").Cells(counter, PARAMETERIZATION_WITH_MODULE_NUMBER_COL).value
            counter = counter + 1
    Wend
    
    Acyclic_Read_Bits = Worksheets("ProfiBus(IO Modules)").Cells(ACYCLIC_ROW_NUMBER, IP_MAP_PRM_BYTES_COL).value
    Ext_Total_Prm_Data_Len = Ext_User_Prm_Data_Len + Ext_Module_Prm_Data_Len + Acyclic_Read_Bits ' - MAX_MODULES
    PROFIBUS_GSD.WriteLine ("Max_User_Prm_Data_Len = " & Ext_Total_Prm_Data_Len)
    
    PROFIBUS_GSD.WriteLine ("Ext_User_Prm_Data_Const(0) = " & Worksheets("ProfiBus (DCI)").Range("A1").Cells(2, 5) _
    & "," & Worksheets("ProfiBus (DCI)").Range("A1").Cells(3, 5) _
    & "," & Worksheets("ProfiBus (DCI)").Range("A1").Cells(4, 5) & ",\")
        
    For i = 1 To (Ext_User_Prm_Data_Len - 4)
        PROFIBUS_GSD.Write ("0x00,")
    Next
    PROFIBUS_GSD.Write ("0x00")
    
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine ("")
    
    Dim User_Prm_Data_Len As Integer
    Dim Module_Prm_Data_Len As Integer
    Dim Prm_Ref_Offset As Integer
    Dim Prm_Count As Integer
    Dim rcount As Integer
    User_Prm_Data_Len = 0
    Prm_Ref_Offset = 3  'We start with 3, as first 3 bytes reserved for DPV1
    Prm_Count = 1 'INITIALIZATION PARAMETERs starts
    counter = START_ROW
    Dim new_Prm_Count As Integer
    new_Prm_Count = 2

    While ((Cells(counter, 2) <> Empty) And (Cells(counter, 2) <> BEGIN_IGNORED_DATA))
        If (Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "INITIALIZATION PARAMETER") Then
            PROFIBUS_GSD.WriteLine ("Ext_User_Prm_Data_Ref(" & Prm_Ref_Offset & ") = " & Prm_Count)
            Prm_Count = Prm_Count + 1
            dci_sheet_row = Find_DCID_Row_Search_Desc_Col(Cells(counter, 1))
            User_Prm_Data_Len = Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_LENGTH_COL).value
            Prm_Ref_Offset = Prm_Ref_Offset + User_Prm_Data_Len
        End If
        If (Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC OUTPUT BIT") Then
            'Get the Parameter Bit Breakup and print it
            DESCRIPTION_COL = 81
            dci_sheet_row = Find_DCID_Row_Search_Desc_Col(Cells(counter, 1))
            description_content_string = Worksheets("DCI Descriptors").Cells(dci_sheet_row, DESCRIPTION_COL).text
            descriptionString = Split(description_content_string, vbLf)
            
            'Dim Count As Integer
            For Count = 1 To UBound(descriptionString)
            PROFIBUS_GSD.WriteLine ("Ext_User_Prm_Data_Ref(" & Prm_Ref_Offset & ") = " & Prm_Count)
            Prm_Count = Prm_Count + 1
            User_Prm_Data_Len = 1 ' 1 byte parameterization technique
            Prm_Ref_Offset = Prm_Ref_Offset + User_Prm_Data_Len
            Next Count
        End If
        If ((Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC OUTPUT WORD")) Then
            PROFIBUS_GSD.WriteLine ("Ext_User_Prm_Data_Ref(" & Prm_Ref_Offset & ") = " & Prm_Count)
            Prm_Count = Prm_Count + 1
            User_Prm_Data_Len = 1 ' 1 byte parameterization technique
            Prm_Ref_Offset = Prm_Ref_Offset + User_Prm_Data_Len
        End If
        
       If ((Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC OUTPUT DWORD")) Then
            PROFIBUS_GSD.WriteLine ("Ext_User_Prm_Data_Ref(" & Prm_Ref_Offset & ") = " & Prm_Count)
            Prm_Count = Prm_Count + 1
            User_Prm_Data_Len = 1 ' 1 byte parameterization technique
            Prm_Ref_Offset = Prm_Ref_Offset + User_Prm_Data_Len
        End If
        If ((Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC OUTPUT BIT OUTPUT WORD") _
        Or (Cells(counter, GSD_DATATYPE_DEFINE_COL).text = "CYCLIC OUTPUT BIT OUTPUT DWORD")) Then
            'Get the Parameter Bit Breakup and print it
            DESCRIPTION_COL = 81
            dci_sheet_row = Find_DCID_Row_Search_Desc_Col(Cells(counter, 1))
            description_content_string = Worksheets("DCI Descriptors").Cells(dci_sheet_row, DESCRIPTION_COL).text
            descriptionString = Split(description_content_string, vbLf)
            
            'Dim Count As Integer
            For Count = 1 To UBound(descriptionString)
            PROFIBUS_GSD.WriteLine ("Ext_User_Prm_Data_Ref(" & Prm_Ref_Offset & ") = " & Prm_Count)
            Prm_Count = Prm_Count + 1
            User_Prm_Data_Len = 1 ' 1 byte parameterization technique
            Prm_Ref_Offset = Prm_Ref_Offset + User_Prm_Data_Len
            Next Count
            
            PROFIBUS_GSD.WriteLine ("Ext_User_Prm_Data_Ref(" & Prm_Ref_Offset & ") = " & Prm_Count)
            Prm_Count = Prm_Count + 1
            User_Prm_Data_Len = 1 ' 1 byte parameterization technique
            Prm_Ref_Offset = Prm_Ref_Offset + User_Prm_Data_Len
        End If
        counter = counter + 1
    Wend
    
    If (generate_custom_assignement = True) Then
            new_Prm_Count = Prm_Count
    End If
    Prm_Count = Acyclic_Read_Ref_Offset

    ' Prayas commented this for now as the code written here is for acyclic table in Profi IO module sheet which doesn't have table for acyclic unlike redball/Galaxy
    
'    'Acyclic Read Bits,we are putting it under User parameters tab
'    For bit_count = 0 To Worksheets("ProfiBus(IO Modules)").Cells(ACYCLIC_ROW_NUMBER, INPUT_BIT_COL).value - 1
'        'new_Prm_Count = new_Prm_Count + 1
'        PROFIBUS_GSD.WriteLine ("Ext_User_Prm_Data_Ref(" & Prm_Ref_Offset & ") = " & new_Prm_Count) 'Prm_Count)
'        Prm_Count = Prm_Count + 1
'        User_Prm_Data_Len = 1 ' 1 byte parameterization technique
'        Prm_Ref_Offset = Prm_Ref_Offset + User_Prm_Data_Len
'    Next bit_count
'
'    Prm_Count = Acyclic_Read_Word_Ref_Offset
'    'Acyclic Read Bits,we are putting it under User parameters tab
'    For word_count = 0 To Worksheets("ProfiBus(IO Modules)").Cells(ACYCLIC_ROW_NUMBER, INPUT_WORD_COL).value - 1
'    'new_Prm_Count = new_Prm_Count + 1
'        PROFIBUS_GSD.WriteLine ("Ext_User_Prm_Data_Ref(" & Prm_Ref_Offset & ") = " & new_Prm_Count) 'Prm_Count)
'        Prm_Count = Prm_Count + 1
'        User_Prm_Data_Len = 1 ' 1 byte parameterization technique
'        Prm_Ref_Offset = Prm_Ref_Offset + User_Prm_Data_Len
'    Next word_count
    
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    PROFIBUS_GSD.WriteLine (";************  Diagnostic Data  **********************************************************")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    PROFIBUS_GSD.WriteLine ("UnitDiagType = 129")
    PROFIBUS_GSD.WriteLine ("X_Unit_Diag_Area = 24 - 39")
    fault_code_row_var = FAULT_CODE_ROW + 1
    fault_code_number_col_var = FAULT_CODE_NUMBER_COL
    While (Worksheets("ProfiBus (GSD)").Cells(fault_code_row_var, fault_code_number_col_var).text <> Empty)
        PROFIBUS_GSD.WriteLine ("X_Value(" & Worksheets("ProfiBus (GSD)").Cells(fault_code_row_var, fault_code_number_col_var).value & ") = """ & Worksheets("ProfiBus (GSD)").Cells(fault_code_row_var, fault_code_number_col_var + 1).text & """")
        fault_code_row_var = fault_code_row_var + 1
    Wend
    PROFIBUS_GSD.WriteLine ("X_Unit_Diag_Area_End")
    PROFIBUS_GSD.WriteLine ("EndUnitDiagType")
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    PROFIBUS_GSD.WriteLine (";************  Module-Definition-List  ***************************************************")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    
    'Build inout, output and input modules
    'Dim Module As Integer
    Module = 1
    counter = Profibus_IO_Module_START_ROW
    MODULE_DESCRIPTION_COL = 1
    MODULE_NUMBER_COL = MODULE_DESCRIPTION_COL + 1
    IP_BIT_COL = MODULE_NUMBER_COL + 1
    IP_WORD_COL = IP_BIT_COL + 1
    IP_DWORD_COL = IP_WORD_COL + 1
    INPUT_SIZE_BYTES_COL = MODULE_NUMBER_COL + 7
    OUTPUT_SIZE_BYTES_COL = INPUT_SIZE_BYTES_COL + 1
    IP_MAP_PRM_BYTES_COL = OUTPUT_SIZE_BYTES_COL + 1
    MOD_ID_MSB_COL = OUTPUT_SIZE_BYTES_COL + 7
    MOD_ID_LSB_COL = MOD_ID_MSB_COL + 1
    
 '   IP_BIT_DataRef_offset = 16 ' This is the offset for Input Bits and words.
  '  IP_WORD_DataRef_offset = 32 ' This is the offset for Input Bits and words.
    Dim module_no_msb As Integer
    Dim module_no_lsb As Integer
    Dim length_word_output As Integer
    Dim length_word_input As Integer
    module_no_msb = 0
    module_no_lsb = 0
    length_word_output = 0
    length_word_input = 0
        module_populate_valid = 0
    While (Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_NUMBER_COL).text <> BEGIN_IGNORED_DATA)
                        module_populate_valid = 0
            If ((Worksheets("ProfiBus(IO Modules)").Cells(counter, OUTPUT_SIZE_BYTES_COL).text <> Empty) Or Worksheets("ProfiBus(IO Modules)").Cells(counter, INPUT_SIZE_BYTES_COL).text <> Empty) Then
                    module_no_msb = Int((Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_NUMBER_COL).value And &HFF00) / (2 ^ 8))
                    module_no_lsb = (Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_NUMBER_COL).value And &HFF)
                    If ((Worksheets("ProfiBus(IO Modules)").Cells(counter, OUTPUT_SIZE_BYTES_COL).text <> 0) And Worksheets("ProfiBus(IO Modules)").Cells(counter, INPUT_SIZE_BYTES_COL).text <> 0) Then
                    If ((Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_OUTPUT_WORD_SIZE_COL).value <> 0) Or (Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_OUTPUT_DWORD_SIZE_COL).value <> 0)) And (Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_OUTPUT_BIT_SIZE_COL).value = 0) And _
                    ((Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_INPUT_WORD_SIZE_COL).value <> 0) Or (Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_INPUT_DWORD_SIZE_COL).value <> 0)) And (Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_INPUT_BIT_SIZE_COL).value = 0) Then
                    'RC length_word_output = (64 Or (Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_OUTPUT_WORD_SIZE_COL).value + Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_OUTPUT_DWORD_SIZE_COL).value - 1))
                    'RC length_word_input = (64 Or (Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_INPUT_WORD_SIZE_COL).value + Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_INPUT_DWORD_SIZE_COL).value - 1))
                    module_populate_valid = 1
                                        length_word_output = ((Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_OUTPUT_WORD_SIZE_COL).value * 2 + Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_OUTPUT_DWORD_SIZE_COL).value * 4 - 1))
                    length_word_input = ((Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_INPUT_WORD_SIZE_COL).value * 2 + Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_INPUT_DWORD_SIZE_COL).value * 4 - 1))
                    
                                        
                                        PROFIBUS_GSD.WriteLine ("Module = """ & Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_DESCRIPTION_COL).text & """ 0xC2, " & "0x" & _
                    Format(Hex(length_word_output), "00") & ", " & "0x" & Format(Hex(length_word_input), "00") & ", 0x" & ConvertDecToHexString(module_no_msb) & ", 0x" & _
                    ConvertDecToHexString(module_no_lsb))
                    Else
                                        module_populate_valid = 1
                    PROFIBUS_GSD.WriteLine ("Module = """ & Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_DESCRIPTION_COL).text & """ 0xC2, " & "0x" & _
                    Format(Hex(Worksheets("ProfiBus(IO Modules)").Cells(counter, OUTPUT_SIZE_BYTES_COL).text - 1), "00") & ", " & "0x" & Format(Hex(Worksheets("ProfiBus(IO Modules)").Cells(counter, INPUT_SIZE_BYTES_COL).text - 1), "00") & ", 0x" & ConvertDecToHexString(module_no_msb) & ", 0x" & _
                    ConvertDecToHexString(module_no_lsb))
                    End If
                    IP_BIT_COUNT = Worksheets("ProfiBus(IO Modules)").Cells(counter, IP_BIT_COL).value
                    IP_WORD_COUNT = Worksheets("ProfiBus(IO Modules)").Cells(counter, IP_WORD_COL).value
                    IP_DWORD_COUNT = Worksheets("ProfiBus(IO Modules)").Cells(counter, IP_DWORD_COL).value
                    End If
                    
                    If ((Worksheets("ProfiBus(IO Modules)").Cells(counter, OUTPUT_SIZE_BYTES_COL).text <> 0) And Worksheets("ProfiBus(IO Modules)").Cells(counter, INPUT_SIZE_BYTES_COL).text = 0) Then
                    If ((Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_OUTPUT_WORD_SIZE_COL).value <> 0) Or (Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_OUTPUT_DWORD_SIZE_COL).value <> 0)) And (Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_OUTPUT_BIT_SIZE_COL).value = 0) Then
                    'length_word_output = (64 Or (Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_OUTPUT_WORD_SIZE_COL).value + Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_OUTPUT_DWORD_SIZE_COL).value - 1))
                                        length_word_output = ((Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_OUTPUT_WORD_SIZE_COL).value * 2 + Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_OUTPUT_DWORD_SIZE_COL).value * 4 - 1))
                    module_populate_valid = 1
                                        PROFIBUS_GSD.WriteLine ("Module = """ & Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_DESCRIPTION_COL).text & """ 0x82, " & "0x" & _
                    Format(Hex(length_word_output), "00") & ", " & _
                    "0x" & ConvertDecToHexString(module_no_msb) & ", 0x" & _
                    ConvertDecToHexString(module_no_lsb))
                    Else
                                        module_populate_valid = 1
                    PROFIBUS_GSD.WriteLine ("Module = """ & Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_DESCRIPTION_COL).text & """ 0x82, " & "0x" & _
                    Format(Hex(Worksheets("ProfiBus(IO Modules)").Cells(counter, OUTPUT_SIZE_BYTES_COL).text - 1), "00") & ", " & _
                    "0x" & ConvertDecToHexString(module_no_msb) & ", 0x" & _
                    ConvertDecToHexString(module_no_lsb))
                    End If
                    End If
                    
                    If ((Worksheets("ProfiBus(IO Modules)").Cells(counter, OUTPUT_SIZE_BYTES_COL).text = 0) And Worksheets("ProfiBus(IO Modules)").Cells(counter, INPUT_SIZE_BYTES_COL).text <> 0) Then
                    If ((Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_INPUT_WORD_SIZE_COL).value <> 0) Or (Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_INPUT_DWORD_SIZE_COL).value <> 0)) And (Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_INPUT_BIT_SIZE_COL).value = 0) Then
                    'length_word_input = (64 Or (Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_INPUT_WORD_SIZE_COL).value + Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_INPUT_DWORD_SIZE_COL).value - 1))
                                        
                                        module_populate_valid = 1
                                        length_word_input = ((Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_INPUT_WORD_SIZE_COL).value * 2 + Worksheets("ProfiBus(IO Modules)").Cells(counter, DATA_CONFIG_MODULE_INPUT_DWORD_SIZE_COL).value * 4 - 1))
                    PROFIBUS_GSD.WriteLine ("Module = """ & Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_DESCRIPTION_COL).text & """ 0x42, " & _
                    "0x" & Format(Hex(length_word_input), "00") & ", " & _
                    "0x" & ConvertDecToHexString(module_no_msb) & ", 0x" & _
                    ConvertDecToHexString(module_no_lsb))
                    Else
                                        module_populate_valid = 1
                    PROFIBUS_GSD.WriteLine ("Module = """ & Worksheets("ProfiBus(IO Modules)").Cells(counter, MODULE_DESCRIPTION_COL).text & """ 0x42, " & _
                    "0x" & Format(Hex(Worksheets("ProfiBus(IO Modules)").Cells(counter, INPUT_SIZE_BYTES_COL).value - 1), "00") & ", " & _
                    "0x" & ConvertDecToHexString(module_no_msb) & ", 0x" & _
                    ConvertDecToHexString(module_no_lsb))
                    End If
                    IP_BIT_COUNT = Worksheets("ProfiBus(IO Modules)").Cells(counter, IP_BIT_COL).value
                    IP_WORD_COUNT = Worksheets("ProfiBus(IO Modules)").Cells(counter, IP_WORD_COL).value
                    End If
            '
                        If (1 = module_populate_valid) Then
            PROFIBUS_GSD.WriteLine (Module)
                        End If
            Dim Module_Number As Integer
            Module_Number = Module
            If (Worksheets("ProfiBus(IO Modules)").Cells(counter, PARAMETERIZATION_WITH_MODULE_NUMBER_COL).value <> 0) Then
                PROFIBUS_GSD.WriteLine ("Ext_Module_Prm_Data_Len = " & Worksheets("ProfiBus(IO Modules)").Cells(counter, PARAMETERIZATION_WITH_MODULE_NUMBER_COL).value)
                PROFIBUS_GSD.Write ("Ext_User_Prm_Data_Const(0) = 0x" & ConvertDecToHexString(Module_Number))
                'PROFIBUS_GSD.Write (",")
            Else
                                If (1 = module_populate_valid) Then
                                        PROFIBUS_GSD.WriteLine ("Ext_Module_Prm_Data_Len = " & Worksheets("ProfiBus(IO Modules)").Cells(counter, IP_MAP_PRM_BYTES_COL).value)
                                        PROFIBUS_GSD.Write (";Ext_User_Prm_Data_Const(0) = ")
                                End If
            End If
            rcount = 1
            'we have added module number as part of parameterization,which does not contain actual parametere info
            ' so subtract it.
            For i = 1 To (Worksheets("ProfiBus(IO Modules)").Cells(counter, PARAMETERIZATION_WITH_MODULE_NUMBER_COL).value - 1)
                ' Set 8 character in first row
                If (i = (8 * rcount)) Then
                    PROFIBUS_GSD.Write (",\")
                    PROFIBUS_GSD.Write (vbNewLine)
                    rcount = rcount + 1
                Else
                    PROFIBUS_GSD.Write (",")
                End If
            
            PROFIBUS_GSD.Write ("0x00")
            
              '  If (I <> (Worksheets("ProfiBus(IO Modules)").Cells(counter, IP_MAP_PRM_BYTES_COL).value - 1)) Then
                 '   PROFIBUS_GSD.Write (",")
              '  End If

            
            Next
            PROFIBUS_GSD.WriteLine ("")
            Module_Prm_Data_Len = 0
            Prm_Ref_Offset = 1
            counter_mod_prm = Profibus_IO_Module_START_ROW
            Mod_Prm_Length_Col = 9
            
            If (Worksheets("ProfiBus(IO Modules)").Cells(counter, IP_BIT_COL).value <> 0) Then
                Prm_Count = IP_BIT_DataRef_offset
                For i = 1 To (Worksheets("ProfiBus(IO Modules)").Cells(counter, IP_BIT_COL).value)
                If (generate_custom_assignement = False) Then
                        PROFIBUS_GSD.WriteLine ("Ext_User_Prm_Data_Ref(" & Prm_Ref_Offset & ") = " & Prm_Count)
                Else
                        PROFIBUS_GSD.WriteLine ("Ext_User_Prm_Data_Ref(" & Prm_Ref_Offset & ") = " & new_Prm_Count) 'Prm_Count)
                        new_Prm_Count = new_Prm_Count + 1
                End If
                Prm_Count = Prm_Count + 1
                Prm_Ref_Offset = Prm_Ref_Offset + 1 ' Because every parameter is a 1 byte according to 1 byte Parameterization technique
                Next
            End If
            If (Worksheets("ProfiBus(IO Modules)").Cells(counter, IP_WORD_COL).value <> 0) Then
                'If Module = 1 Then 'RajeshC Profi
                '   Prm_Count = MMP_Status_Input_Offset
               ' Else
                   Prm_Count = IP_WORD_DataRef_offset
               ' End If
                For i = 1 To (Worksheets("ProfiBus(IO Modules)").Cells(counter, IP_WORD_COL).value)
                If (generate_custom_assignement = False) Then
                        PROFIBUS_GSD.WriteLine ("Ext_User_Prm_Data_Ref(" & Prm_Ref_Offset & ") = " & Prm_Count)
                Else
                        PROFIBUS_GSD.WriteLine ("Ext_User_Prm_Data_Ref(" & Prm_Ref_Offset & ") = " & new_Prm_Count) 'Prm_Count)
                        new_Prm_Count = new_Prm_Count + 1
                End If
                Prm_Count = Prm_Count + 1
                Prm_Ref_Offset = Prm_Ref_Offset + 1 ' Because every parameter is a 1 byte according to 1 byte Parameterization technique
                Next
            End If
            If (Worksheets("ProfiBus(IO Modules)").Cells(counter, IP_DWORD_COL).value <> 0) Then
                'If Module = 1 Then 'RajeshC Profi
                '   Prm_Count = MMP_Status_Input_Offset
               ' Else
                   Prm_Count = IP_DWORD_DataRef_offset
               ' End If
                For i = 1 To (Worksheets("ProfiBus(IO Modules)").Cells(counter, IP_DWORD_COL).value)
                If (generate_custom_assignement = False) Then
                        PROFIBUS_GSD.WriteLine ("Ext_User_Prm_Data_Ref(" & Prm_Ref_Offset & ") = " & Prm_Count)
                Else
                        PROFIBUS_GSD.WriteLine ("Ext_User_Prm_Data_Ref(" & Prm_Ref_Offset & ") = " & new_Prm_Count) 'Prm_Count)
                        new_Prm_Count = new_Prm_Count + 1
                End If
                Prm_Count = Prm_Count + 1
                Prm_Ref_Offset = Prm_Ref_Offset + 1 ' Because every parameter is a 1 byte according to 1 byte Parameterization technique
                Next
            End If
                        If (1 = module_populate_valid) Then
            PROFIBUS_GSD.WriteLine ("EndModule")
            PROFIBUS_GSD.WriteLine ("")
                        End If
            Module = Module + 1
            End If
            counter = counter + 1
    Wend '(Cells(counter, Module_Number_COL).Text <> BEGIN_IGNORED_DATA)
    

    Application.Calculation = xlCalculationAutomatic
    
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    PROFIBUS_GSD.WriteLine (";************  Modulstatus  **************************************************************")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    
    '*********************
    '******     Switch to PROFI GSD sheet
    '*********************
    Sheets("Profibus (GSD)").Select
    
     
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    PROFIBUS_GSD.WriteLine (";************  DP extensions *************************************************************")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    
        invalid_value = 0
        For gsd_count = 0 To MAX_DP_EXTN_KEYWORDS - 1
                invalid_value = 0
            For VAL_START_ROW = DP_EXTN_KEYWORD_ROW_START To DP_EXTN_KEYWORD_ROW_END
                If (Cells(VAL_START_ROW, TABLE_VALUE_COL).text <> Empty) Then
                    If (Cells(VAL_START_ROW, TABLE_VALUE_COL).text = DPExtensions(gsd_count) And Cells(VAL_START_ROW, DP_EXTN_VALUE_COL).text <> Empty) Then
                        PROFIBUS_GSD.WriteLine (Cells(VAL_START_ROW, TABLE_VALUE_COL).text & " = " & Cells(VAL_START_ROW, DP_EXTN_VALUE_COL).value)
                        invalid_value = 1
                    End If
                End If
            Next VAL_START_ROW
            If (invalid_value = 0) Then
                MsgBox "Error Contents For keyword " & DPExtensions(gsd_count) & " Not found"
            End If
        Next gsd_count
        
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    PROFIBUS_GSD.WriteLine (";************  Slot Definations **********************************************************")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    
      
    PROFIBUS_GSD.WriteLine ("")
        
    PROFIBUS_GSD.WriteLine ("; Start of SlotIndex Definitions")

    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    
    START_ROW = 12
    DESCRIP_COL = 1
    DATA_DEFINE_COL = 2
    DATA_LENGTH_COL = 5
    DATA_TYPE_COL = 3
    DATA_TYPE_LENGTH_COL = 4
        DATA_DCI_TOTAL_LENGTH = 6
    GSD_FILE_TYPE = 4
    DATA_SLOT_NO_COL = 5
    DATA_INDEX_COL = 6
    DATA_ASSEMBLY_ID_COL = 7
    
    Dim j As Variant
    Dim k As Variant
    Dim assembly_id(512) As Variant
    Dim assembly_params(512) As Byte
    Dim assembly_size(512) As Byte
    Dim assembly_numbers As Variant
    assembly_numbers = 0
    total_acyclic_parameters = 0
    
    
    Sheets("Profibus (DCI)").Select
    
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, DATA_ASSEMBLY_ID_COL).text <> Empty And Cells(counter, GSD_FILE_TYPE).text = "ACYCLIC PARAMETER") Then
            assembly_id(assembly_numbers) = Cells(counter, DATA_ASSEMBLY_ID_COL).value
            assembly_numbers = assembly_numbers + 1
        End If
        counter = counter + 1
    Wend
    
    total_acyclic_parameters = assembly_numbers
    i = 0
    While (i <= assembly_numbers)
        j = i + 1
        While (j <= assembly_numbers - 1)
            If (assembly_id(j) = assembly_id(i)) Then
                k = j
               While (k <= assembly_numbers)
                    assembly_id(k) = assembly_id(k + 1)
                    k = k + 1
                Wend
                assembly_numbers = (assembly_numbers - 1)
            End If
            j = j + 1
        Wend
        i = i + 1
    Wend
    
    i = 0
    For i = 0 To assembly_numbers - 1
        assembly_params(i) = 0
        assembly_size(i) = 0
        counter = START_ROW
        While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            If (Cells(counter, DATA_ASSEMBLY_ID_COL).text <> Empty And Cells(counter, GSD_FILE_TYPE).text = "ACYCLIC PARAMETER") Then
                If (Cells(counter, DATA_ASSEMBLY_ID_COL).value = assembly_id(i)) Then
                    assembly_params(i) = assembly_params(i) + 1
                    dci_sheet_row = Find_DCID_Row(Worksheets("ProfiBus (DCI)").Cells(counter, DATA_DEFINE_COL).text)
                                        dci_sheet_row_temp = dci_sheet_row
                    assembly_size(i) = assembly_size(i) + Worksheets("DCI Descriptors").Cells(dci_sheet_row_temp, DATA_TYPE_LENGTH_COL).value
                End If
            End If
            counter = counter + 1
        Wend
    Next i
'****************************************************************************************************************************
'******     Start Writing the Data into GSD file. Note GSD file can not be used for acyclic data exchange. Data is appended
'******     in GSD file just for information
'****************************************************************************************************************************
    PROFIBUS_GSD.WriteLine ("; Total Parameters for Acyclic Data Exchange = " & total_acyclic_parameters)
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine ("")
    
        For i = 0 To assembly_numbers - 1
        PROFIBUS_GSD.WriteLine ("; SLOT = " & (assembly_id(i) \ 256) & "  INDEX = " & (assembly_id(i) Mod 256))
        PROFIBUS_GSD.WriteLine ("; Total Parameters in slot " & (assembly_id(i) \ 256) & "  INDEX  " & (assembly_id(i) Mod 256) & " = " & assembly_params(i))
        PROFIBUS_GSD.WriteLine ("; Total Bytes in slot " & (assembly_id(i) \ 256) & "  INDEX  " & (assembly_id(i) Mod 256) & " = " & assembly_size(i))
        PROFIBUS_GSD.WriteLine ("; BYTE LOCATIONWISE PARAMETER LIST - ")
        counter = START_ROW
        byte_offset = 0
        While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            If (Cells(counter, DATA_ASSEMBLY_ID_COL).text <> Empty And Cells(counter, GSD_FILE_TYPE).text = "ACYCLIC PARAMETER") Then
                If (Cells(counter, DATA_ASSEMBLY_ID_COL).value = assembly_id(i)) Then
                                        dci_sheet_row = Find_DCID_Row(Worksheets("ProfiBus (DCI)").Cells(counter, DATA_DEFINE_COL).text)
                                        dci_sheet_row_temp = dci_sheet_row
                    PROFIBUS_GSD.WriteLine ("; Byte Location - " & byte_offset & " TO " & Worksheets("DCI Descriptors").Cells(dci_sheet_row_temp, DATA_DCI_TOTAL_LENGTH).value + byte_offset - 1 & " Parameter = " & Cells(counter, DESCRIP_COL).text)
                    byte_offset = byte_offset + Worksheets("DCI Descriptors").Cells(dci_sheet_row_temp, DATA_DCI_TOTAL_LENGTH).value
                End If
            End If
            counter = counter + 1
        Wend
        PROFIBUS_GSD.WriteLine ("")
    Next i

    PROFIBUS_GSD.WriteLine ("; End of Slot Index Definitions")
    
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    PROFIBUS_GSD.WriteLine (";************  End of GSD File ***********************************************************")
    PROFIBUS_GSD.WriteLine (";*****************************************************************************************")
    PROFIBUS_GSD.WriteLine ("")
    PROFIBUS_GSD.WriteLine ("")
        
    
End Sub



Function ConvertDecToHexString(number As Integer) As String
    If number < 10 Then
        ConvertDecToHexString = Format(Hex(number), "00")
    Else
        ConvertDecToHexString = "0" & Format(Hex(number))
    End If
    
End Function












