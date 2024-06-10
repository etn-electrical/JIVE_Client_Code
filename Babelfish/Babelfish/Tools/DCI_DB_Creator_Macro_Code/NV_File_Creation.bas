Attribute VB_Name = "NV_File_Creation"
Public Const NV_MEM_SHEET_MAX_VAL_SIZE_ROW = 3
Public Const NV_MEM_SHEET_MAX_VAL_SIZE_COL = 2


Sub Create_NV_Address_File()

    Application.Calculation = xlCalculationManual
    
    Dim END_STRING As String
    END_STRING = "NV_DATA_END_NVCTRL"
        
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\" & Cells(1, 2).value
    Set DB_NVSPACE = fs.CreateTextFile(file_path & "\NV_Address.h", True)
    
    'Making font type and size consistent.
    Worksheets("NVM Address").Range("A:AJ").Font.name = "Arial"
    Worksheets("NVM Address").Range("A:AJ").Font.Size = 10
   
    CHIP_0_DESCRIPTOR_COL = 1
    CHIP_0_HEX_ADDRESS_COL = 3
    CHIP_0_DEFAULT_DATA_COL = 4
    
    CHIP_1_DESCRIPTOR_COL = 6
    CHIP_1_HEX_ADDRESS_COL = 8
    CHIP_1_DEFAULT_DATA_COL = 9

    CHIP_DECLARATION_ROW = 6
    CHIP_DECLARATION_COL_SPACING = 5
    
    'These offsets are assuming that we are adding to 1 and not Zero.  Keep that in mind.
    CHIP_DESCRIPTOR_OFFSET = 0
    CHIP_DEC_ADDRESS_OFFSET = 1
    CHIP_HEX_ADDRESS_OFFSET = 2
    CHIP_DEFAULT_DATA_OFFSET = 3

    NV_CHIP_SELECT_ID_SHIFT_ROW = 2
    NV_CHIP_SELECT_ID_SHIFT_COL = 2

    'These offsets are assuming that we are adding to 1 and not Zero.  Keep that in mind.
    EXCLUDE_CHECKSUM_SPACE_ROW = 5
    EXCLUDE_CHECKSUM_SPACE_COL_OFFSET = 1

    Sheets("NVM Address").Select

    DB_NVSPACE.WriteLine ("/**")
    DB_NVSPACE.WriteLine ("*****************************************************************************************")
    DB_NVSPACE.WriteLine ("*   @file")
    DB_NVSPACE.WriteLine ("*")
    DB_NVSPACE.WriteLine ("*   @brief")
    DB_NVSPACE.WriteLine ("*")
    DB_NVSPACE.WriteLine ("*   @details")
    DB_NVSPACE.WriteLine ("*")
    DB_NVSPACE.WriteLine ("*   @version")
    DB_NVSPACE.WriteLine ("*   C++ Style Guide Version 1.0")
    DB_NVSPACE.WriteLine ("*")
    DB_NVSPACE.WriteLine ("*   @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    DB_NVSPACE.WriteLine ("*")
    DB_NVSPACE.WriteLine ("*****************************************************************************************")
    DB_NVSPACE.WriteLine ("*/")
    DB_NVSPACE.WriteLine ("#ifndef NV_ADDRESS_H")
    DB_NVSPACE.WriteLine ("  #define NV_ADDRESS_H")
    
    DB_NVSPACE.WriteLine ("")
    DB_NVSPACE.WriteLine ("//***************************************************************************")
    DB_NVSPACE.WriteLine ("// NV Address and Control Masks")
    DB_NVSPACE.WriteLine ("//***************************************************************************")
    DB_NVSPACE.WriteLine ("typedef uint32_t nv_mem_address_t;")
    DB_NVSPACE.WriteLine ("static const uint8_t NV_ADDRESS_CTRL_SELECT_SHIFT = " & _
            Cells(NV_CHIP_SELECT_ID_SHIFT_ROW, NV_CHIP_SELECT_ID_SHIFT_COL).text & "U;")
    DB_NVSPACE.WriteLine ("static const nv_mem_address_t NV_ADDRESS_CTRL_SELECT_MASK =")
    DB_NVSPACE.WriteLine ("    static_cast<uint32_t>( ~( 0xFFFFFFFFU>>( ( sizeof( nv_mem_address_t ) * 8 ) - NV_ADDRESS_CTRL_SELECT_SHIFT ) ) );")
    DB_NVSPACE.WriteLine ("static const nv_mem_address_t NV_ADDRESS_CTRL_ADDRESS_MASK = static_cast<uint32_t>( ~NV_ADDRESS_CTRL_SELECT_MASK );")
    DB_NVSPACE.WriteLine ("")
    DB_NVSPACE.WriteLine ("static const uint16_t NV_MAX_DATA_VAL_LENGTH = " & _
            Cells(NV_MEM_SHEET_MAX_VAL_SIZE_ROW, NV_MEM_SHEET_MAX_VAL_SIZE_COL).text & "U;")
    DB_NVSPACE.WriteLine ("")
    
    chip_counter = 0
    chip_col_counter = 1
    While Cells(CHIP_DECLARATION_ROW, chip_col_counter).text <> Empty
        DB_NVSPACE.WriteLine ("")
        DB_NVSPACE.WriteLine ("")
        DB_NVSPACE.WriteLine ("//***************************************************************************")
        DB_NVSPACE.WriteLine ("// NV Ctrl " & chip_counter & " Addresses")
        DB_NVSPACE.WriteLine ("//***************************************************************************")
        DB_NVSPACE.WriteLine ("")
        If (Cells(EXCLUDE_CHECKSUM_SPACE_ROW, chip_col_counter + EXCLUDE_CHECKSUM_SPACE_COL_OFFSET).text = "") Then
            DB_NVSPACE.WriteLine ("static const bool_t NV_DATA_CTRL" & chip_counter & "_CHECKSUM_SPACE_INCLUDED = true;")
        Else
            DB_NVSPACE.WriteLine ("static const bool_t NV_DATA_CTRL" & chip_counter & "_CHECKSUM_SPACE_INCLUDED = false;")
        End If
        DB_NVSPACE.WriteLine ("")

        ' We go through and create the addresses on the fly now since we are having issues with the
        ' cell functions lasting after the update of this sheet.
        ' We skip over the checksum when it is not included in the final address list.
        Dim address_counter As Long
        'address_counter = 0
        address_counter = Cells(NV_MEM_DATA_START_ROW, chip_col_counter + CHIP_DEC_ADDRESS_OFFSET).value
        row_counter = NV_MEM_DATA_START_ROW
        While Cells(row_counter, chip_col_counter + CHIP_HEX_ADDRESS_OFFSET).text <> Empty
            If (Cells(EXCLUDE_CHECKSUM_SPACE_ROW, chip_col_counter + EXCLUDE_CHECKSUM_SPACE_COL_OFFSET).text = Empty) Then
                Cells(row_counter, chip_col_counter + CHIP_DEC_ADDRESS_OFFSET).value = address_counter
                Cells(row_counter, chip_col_counter + CHIP_HEX_ADDRESS_OFFSET).value = "0x" & Hexify(address_counter, 2)
                address_counter = address_counter + 1
            Else
                If ((Cells(row_counter, chip_col_counter + CHIP_DESCRIPTOR_OFFSET).text = "CHECKSUM")) Or _
                        (Cells(row_counter - 1, chip_col_counter + CHIP_DESCRIPTOR_OFFSET).text = "CHECKSUM") Then
                    Cells(row_counter, chip_col_counter + CHIP_DEC_ADDRESS_OFFSET).value = "'-"
                    Cells(row_counter, chip_col_counter + CHIP_HEX_ADDRESS_OFFSET).value = "'-"
                Else
                    Cells(row_counter, chip_col_counter + CHIP_DEC_ADDRESS_OFFSET).value = address_counter
                    Cells(row_counter, chip_col_counter + CHIP_HEX_ADDRESS_OFFSET).value = "0x" & Hexify(address_counter, 2)
                    address_counter = address_counter + 1
                End If
            End If
            If (Cells(row_counter, chip_col_counter + CHIP_DESCRIPTOR_OFFSET).text <> Empty) And _
                        (Cells(row_counter, chip_col_counter + CHIP_DESCRIPTOR_OFFSET).text <> "CHECKSUM") Then
                DB_NVSPACE.WriteLine ("static const nv_mem_address_t " & _
                            Cells(row_counter, chip_col_counter + CHIP_DESCRIPTOR_OFFSET).text & "_NVADD = " & _
                            "static_cast<nv_mem_address_t>( static_cast<nv_mem_address_t>( " & chip_counter & _
                            "<<" & Cells(NV_CHIP_SELECT_ID_SHIFT_ROW, NV_CHIP_SELECT_ID_SHIFT_COL).text & _
                            " ) | " & Cells(row_counter, chip_col_counter + CHIP_HEX_ADDRESS_OFFSET).text & "U );")
            End If
            If (Cells(row_counter, chip_col_counter).text = END_STRING & chip_counter) Then
                last_row = row_counter
            End If
            row_counter = row_counter + 1
        Wend
        
        DB_NVSPACE.WriteLine ("")
        DB_NVSPACE.WriteLine ("static const nv_mem_address_t NV_DATA_CTRL" & chip_counter & "_RANGE_SIZE = " & _
                        "( NV_DATA_END_NVCTRL" & chip_counter & "_NVADD - NV_DATA_START_NVCTRL" & chip_counter & "_NVADD );")
        
        'Remove any cell color after the END_STRING rows
        Range(Cells((last_row + 1), chip_col_counter), Cells((last_row + 1), chip_col_counter).End(xlDown)).Interior.ColorIndex = xlNone
        
        chip_col_counter = chip_col_counter + CHIP_DECLARATION_COL_SPACING
        chip_counter = chip_counter + 1
    Wend
    
    DB_NVSPACE.WriteLine ("")
    DB_NVSPACE.WriteLine ("#endif")
    DB_NVSPACE.Close

    Application.Calculation = xlCalculationAutomatic
    
End Sub
Sub Create_NV_Defaults_File()

    Application.Calculation = xlCalculationManual
    
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\" & Cells(1, 2).value
    Set DB_NVSPACE_CPP = fs.CreateTextFile(file_path & "\NV_Default.cpp", True)
    Set DB_NVSPACE_H = fs.CreateTextFile(file_path & "\NV_Default.h", True)
   
    CHIP_DECLARATION_ROW = 6
    CHIP_DECLARATION_COL_SPACING = 5
    
    CHIP_DESCRIPTOR_OFFSET = 0
    CHIP_HEX_ADDRESS_OFFSET = 2
    CHIP_DEFAULT_DATA_OFFSET = 3

    Sheets("NVM Address").Select

    DB_NVSPACE_H.WriteLine ("/**")
    DB_NVSPACE_H.WriteLine ("*****************************************************************************************")
    DB_NVSPACE_H.WriteLine ("*   @file")
    DB_NVSPACE_H.WriteLine ("*")
    DB_NVSPACE_H.WriteLine ("*   @brief Autogenerated NV Memory map address file.")
    DB_NVSPACE_H.WriteLine ("*")
    DB_NVSPACE_H.WriteLine ("*   @details")
    DB_NVSPACE_H.WriteLine ("*")
    DB_NVSPACE_H.WriteLine ("*   @version")
    DB_NVSPACE_H.WriteLine ("*   C++ Style Guide Version 1.0")
    DB_NVSPACE_H.WriteLine ("*")
    DB_NVSPACE_H.WriteLine ("*   @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    DB_NVSPACE_H.WriteLine ("*")
    DB_NVSPACE_H.WriteLine ("*****************************************************************************************")
    DB_NVSPACE_H.WriteLine ("*/")
    DB_NVSPACE_H.WriteLine ("#ifndef NV_DEFAULT_H")
    DB_NVSPACE_H.WriteLine ("  #define NV_DEFAULT_H")
    DB_NVSPACE_H.WriteLine ("")
    DB_NVSPACE_H.WriteLine ("")
    DB_NVSPACE_H.WriteLine ("//***************************************************************************")
    DB_NVSPACE_H.WriteLine ("// Typedef")
    DB_NVSPACE_H.WriteLine ("//***************************************************************************")
    DB_NVSPACE_H.WriteLine ("")
    DB_NVSPACE_H.WriteLine ("typedef struct")
    DB_NVSPACE_H.WriteLine ("{")
    DB_NVSPACE_H.WriteLine ("    uint8_t const* data;")
    DB_NVSPACE_H.WriteLine ("    uint32_t address;")
    DB_NVSPACE_H.WriteLine ("    uint32_t length;")
    DB_NVSPACE_H.WriteLine ("} NV_DEFAULT_LIST_ITEM_TD;")
    DB_NVSPACE_H.WriteLine ("")
    DB_NVSPACE_H.WriteLine ("")
    DB_NVSPACE_H.WriteLine ("//***************************************************************************")
    DB_NVSPACE_H.WriteLine ("// Chip Range and Extern")
    DB_NVSPACE_H.WriteLine ("//***************************************************************************")
    DB_NVSPACE_H.WriteLine ("")
   
    default_val_counter = 0
    chip_col_counter = 1
    While Cells(CHIP_DECLARATION_ROW, chip_col_counter).text <> Empty
        row_counter = NV_MEM_DATA_START_ROW
        While Cells(row_counter, chip_col_counter + CHIP_HEX_ADDRESS_OFFSET).text <> Empty
            If (Cells(row_counter, chip_col_counter + CHIP_DESCRIPTOR_OFFSET).text <> Empty) And _
                        (Cells(row_counter, chip_col_counter + CHIP_DESCRIPTOR_OFFSET).text <> "CHECKSUM") And _
                        (Cells(row_counter, chip_col_counter + CHIP_DEFAULT_DATA_OFFSET).text <> Empty) Then
                
                default_val_counter = default_val_counter + 1
                While (Cells(row_counter, chip_col_counter + CHIP_DESCRIPTOR_OFFSET).text <> "CHECKSUM") And _
                                (Cells(row_counter, chip_col_counter + CHIP_DEFAULT_DATA_OFFSET).text <> Empty)
                    row_counter = row_counter + 1
                Wend
            
            End If
            row_counter = row_counter + 1
        Wend
        chip_col_counter = chip_col_counter + CHIP_DECLARATION_COL_SPACING
    Wend

    DB_NVSPACE_H.WriteLine ("#define NV_DEFAULT_LIST_ITEM_COUNT                " & default_val_counter)
    If (default_val_counter <> 0) Then
        DB_NVSPACE_H.WriteLine ("extern const NV_DEFAULT_LIST_ITEM_TD nv_def_list[NV_DEFAULT_LIST_ITEM_COUNT];")
    End If
    DB_NVSPACE_H.WriteLine ("")
    DB_NVSPACE_H.WriteLine ("")
    DB_NVSPACE_H.WriteLine ("#endif")


'****************************************************************************************************************************
'******     Start Creating the .CPP file header
'****************************************************************************************************************************
    DB_NVSPACE_CPP.WriteLine ("/**")
    DB_NVSPACE_CPP.WriteLine ("*****************************************************************************************")
    DB_NVSPACE_CPP.WriteLine ("*   @file")
    DB_NVSPACE_CPP.WriteLine ("*   @details See header file for module overview.")
    DB_NVSPACE_CPP.WriteLine ("*   @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    DB_NVSPACE_CPP.WriteLine ("*")
    DB_NVSPACE_CPP.WriteLine ("*****************************************************************************************")
    DB_NVSPACE_CPP.WriteLine ("*/")
    DB_NVSPACE_CPP.WriteLine ("#include ""Includes.h""")
    DB_NVSPACE_CPP.WriteLine ("#include ""NV_Default.h""")
    DB_NVSPACE_CPP.WriteLine ("#include ""NV_Address.h""")
    DB_NVSPACE_CPP.WriteLine ("")

    DB_NVSPACE_CPP.WriteLine ("")
    DB_NVSPACE_CPP.WriteLine ("//***************************************************************************")
    DB_NVSPACE_CPP.WriteLine ("// Const Data")
    DB_NVSPACE_CPP.WriteLine ("//***************************************************************************")
    DB_NVSPACE_CPP.WriteLine ("")

    chip_col_counter = 1
    While Cells(CHIP_DECLARATION_ROW, chip_col_counter).text <> Empty
        row_counter = NV_MEM_DATA_START_ROW
        While Cells(row_counter, chip_col_counter + CHIP_HEX_ADDRESS_OFFSET).text <> Empty
            If (Cells(row_counter, chip_col_counter + CHIP_DESCRIPTOR_OFFSET).text <> Empty) And _
                        (Cells(row_counter, chip_col_counter + CHIP_DESCRIPTOR_OFFSET).text <> "CHECKSUM") And _
                        (Cells(row_counter, chip_col_counter + CHIP_DEFAULT_DATA_OFFSET).text <> Empty) Then
                DB_NVSPACE_CPP.WriteLine ("static const uint8_t " & Cells(row_counter, chip_col_counter + CHIP_DESCRIPTOR_OFFSET).text & _
                                                "_NV_DEF_DATA[] =")
                DB_NVSPACE_CPP.WriteLine ("    {")
                While (Cells(row_counter, chip_col_counter + CHIP_DESCRIPTOR_OFFSET).text <> "CHECKSUM") And _
                                (Cells(row_counter, chip_col_counter + CHIP_DEFAULT_DATA_OFFSET).text <> Empty)
                    DB_NVSPACE_CPP.WriteLine ("        " & Cells(row_counter, chip_col_counter + CHIP_DEFAULT_DATA_OFFSET).text & ",")
                    row_counter = row_counter + 1
                Wend
                DB_NVSPACE_CPP.WriteLine ("    };")
            End If
            row_counter = row_counter + 1
        Wend
        chip_col_counter = chip_col_counter + CHIP_DECLARATION_COL_SPACING
    Wend
    
    DB_NVSPACE_CPP.WriteLine ("")
    DB_NVSPACE_CPP.WriteLine ("")
    DB_NVSPACE_CPP.WriteLine ("//***************************************************************************")
    DB_NVSPACE_CPP.WriteLine ("// Default Struct")
    DB_NVSPACE_CPP.WriteLine ("//***************************************************************************")
    DB_NVSPACE_CPP.WriteLine ("")
    
    If (default_val_counter <> 0) Then
        chip_col_counter = 1
        DB_NVSPACE_CPP.WriteLine ("static const NV_DEFAULT_LIST_ITEM_TD nv_def_list[NV_DEFAULT_LIST_ITEM_COUNT] =")
        DB_NVSPACE_CPP.WriteLine ("    {")
        While Cells(CHIP_DECLARATION_ROW, chip_col_counter).text <> Empty
            row_counter = NV_MEM_DATA_START_ROW
            While Cells(row_counter, chip_col_counter + CHIP_HEX_ADDRESS_OFFSET).text <> Empty
                If (Cells(row_counter, chip_col_counter + CHIP_DESCRIPTOR_OFFSET).text <> Empty) And _
                            (Cells(row_counter, chip_col_counter + CHIP_DESCRIPTOR_OFFSET).text <> "CHECKSUM") And _
                            (Cells(row_counter, chip_col_counter + CHIP_DEFAULT_DATA_OFFSET).text <> Empty) Then
                    DB_NVSPACE_CPP.WriteLine ("        {")
                    DB_NVSPACE_CPP.WriteLine ("            " & Cells(row_counter, chip_col_counter + CHIP_DESCRIPTOR_OFFSET).text & "_NV_DEF_DATA,")
                    DB_NVSPACE_CPP.WriteLine ("            " & Cells(row_counter, chip_col_counter + CHIP_DESCRIPTOR_OFFSET).text & "_NVADD,")
                    Data_Length = 0
                    While (Cells(row_counter, chip_col_counter + CHIP_DESCRIPTOR_OFFSET).text <> "CHECKSUM") And _
                                    (Cells(row_counter, chip_col_counter + CHIP_DEFAULT_DATA_OFFSET).text <> Empty)
                        Data_Length = Data_Length + 1
                        row_counter = row_counter + 1
                    Wend
                    DB_NVSPACE_CPP.WriteLine ("            " & Data_Length)
                    DB_NVSPACE_CPP.WriteLine ("        },")
                End If
                row_counter = row_counter + 1
            Wend
            chip_col_counter = chip_col_counter + CHIP_DECLARATION_COL_SPACING
        Wend
        DB_NVSPACE_CPP.WriteLine ("};")
    End If

    DB_NVSPACE_CPP.Close
    DB_NVSPACE_H.Close

    Application.Calculation = xlCalculationAutomatic

End Sub
Sub CreateNVFile()
    
    Application.Calculation = xlCalculationManual
    
    code_dir_path_string = Sheets("Create EEPROM Files").Code_Dir_TextBox.text
    assembler_path_string = Sheets("Create EEPROM Files").Assm_Path_TextBox.text
    assembler_options_string = Sheets("Create EEPROM Files").Assm_Options_TextBox.text
    assembler_output_path_string = Sheets("Create EEPROM Files").Assm_Output_Path_TextBox.text
    temp_files_path_string = Sheets("Create EEPROM Files").Temp_Files_Path_TextBox.text
    
    INIT_COUNTER = 7
    counter = INIT_COUNTER
    DEFINE_DESC = 1
    DATA_COLUMN = 4
    ADD_COLUMN = 3

    Sheets("Create EEPROM Files").Select

    Set fs = CreateObject("Scripting.FileSystemObject")
    Set EEPROMA = fs.CreateTextFile(assembler_output_path_string & "\E2_MEM.asm", True)
    
    EEPROMA.WriteLine (".eseg")
    EEPROMA.WriteLine ("")
    EEPROMA.WriteLine (".org 0")
    EEPROMA.WriteLine ("")
    EEPROMA.WriteLine ("")

     
    While Sheets("NVM Address").Cells(counter, ADD_COLUMN).text <> Empty 'counter < (1024 + INIT_COUNTER)

            If Sheets("NVM Address").Cells(counter, DEFINE_DESC).text = Empty Or _
                        Sheets("NVM Address").Cells(counter, DEFINE_DESC).text = "CHECKSUM" Then
                EEPROMA.WriteLine ("ADD" & Sheets("NVM Address").Cells(counter, ADD_COLUMN).text _
                   & ": .db $" & Sheets("NVM Address").Cells(counter, DATA_COLUMN).text)
            Else
                EEPROMA.WriteLine (Sheets("NVM Address").Cells(counter, DEFINE_DESC).text & ": .db $" _
                   & Sheets("NVM Address").Cells(counter, DATA_COLUMN).text)
             End If
            
            counter = counter + 1
        
    Wend

    EEPROMA.Close

    MyPath = CurDir
    ChDir (temp_files_path_string)
    
    If Sheets("Create EEPROM Files").DB_AVRASM_Chkbox.value = True Then
        command_string = assembler_path_string & "\avrasm.exe " & assembler_options_string & " " _
            & assembler_output_path_string & "\E2_MEM.asm " _
            & temp_files_path_string & "\E2_MEM.lst" & " " _
            & temp_files_path_string & "\E2_MEM.hex"
    Else
        command_string = assembler_path_string & "\avrasm32.exe " & assembler_options_string & " " _
            & assembler_output_path_string & "\E2_MEM.asm"
    End If
    
    Sheets("Create EEPROM Files").Cmd_string_Label.caption = command_string
        
    counter = Shell(command_string, 1)
    
    ChDir (MyPath)
    Sheets("Create EEPROM Files").Select

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

Public Sub Verify_NV_List()

    Application.Calculation = xlCalculationManual
    
'*********************
'******     Begin
'*********************
    Sheets("NVM Address").Select
    NV_MEM_DEFINE_END_STRING_PART_SIZE = Len("NV_DATA_END_NVCTRL")
    NV_MEM_DEFINE_START_STRING_PART_SIZE = Len("NV_DATA_START_NVCTRL")

    Dim dci_end_row As Integer
    
    dci_end_row = Last_DCI_Row()


'****************************************************************************************************************************
'******     Start at the top left of the first NV memory val
'****************************************************************************************************************************
    nv_col_counter = 0
    decimal_value = 0
    nv_address_offset = NV_ADDRESS_COL
    nv_define_offset = NV_DEFINES_COL
    NV_HEX_ADDRESS_COL = 3
    NV_HEX_NUM_COL = 2
    NVX_NUM = 0
    'While nv_col_counter < NV_MEMORY_MAX_MEMORIES
        row_ctr = NV_MEM_DATA_START_ROW
        While Cells(row_ctr, nv_address_offset).text <> Empty
            decimal_value = Cells(row_ctr, nv_address_offset).value
            If (decimal_value < 16) Then
            Cells(row_ctr, nv_address_offset + 1).value = "0x000" & Evaluate("DEC2HEX(" & decimal_value & ")")
            End If
            If (decimal_value < 256) Then
            Cells(row_ctr, nv_address_offset + 1).value = "0x00" & Evaluate("DEC2HEX(" & decimal_value & ")")
            End If
            If (decimal_value < 4096) Then
            Cells(row_ctr, nv_address_offset + 1).value = "0x0" & Evaluate("DEC2HEX(" & decimal_value & ")")
            End If
            row_ctr = row_ctr + 1
        Wend
    'Wend
    nv_col_counter = 0
    nv_address_offset = NV_ADDRESS_COL
    nv_define_offset = NV_DEFINES_COL
    'First we start with column one and move across to the next.
    While nv_col_counter < NV_MEMORY_MAX_MEMORIES
        row_ctr = NV_MEM_DATA_START_ROW
        While Cells(row_ctr, nv_address_offset).text <> Empty
            Cells(row_ctr, nv_define_offset).Font.Italic = False
            Cells(row_ctr, nv_define_offset).Font.Bold = False
            Cells(row_ctr, nv_define_offset).Font.Strikethrough = False
        
            If ((Cells(row_ctr, nv_define_offset).text <> Empty) And _
                    (Cells(row_ctr, nv_define_offset).text <> NV_MEM_CHECKSUM_STRING) And _
                    (Left(Cells(row_ctr, nv_define_offset).text, NV_MEM_DEFINE_END_STRING_PART_SIZE) <> NV_MEM_DEFINE_END_STRING_PART) And _
                    (Left(Cells(row_ctr, nv_define_offset).text, NV_MEM_DEFINE_START_STRING_PART_SIZE) <> NV_MEM_DEFINE_START_STRING_PART)) Then
                Set C = Worksheets("DCI Descriptors").Range(Worksheets("DCI Descriptors").Cells(DCI_DEFINES_START_ROW, DCI_DEFINES_COL), _
                    Worksheets("DCI Descriptors").Cells(dci_end_row, DCI_DEFINES_COL)).Find(Cells(row_ctr, nv_define_offset).text, LookAt:=xlWhole)
                If C Is Nothing Then    'If the value was not found in the DCI Descriptors sheet then we change its color to indicate thus.
                    Cells(row_ctr, nv_define_offset).Interior.ColorIndex = NV_MEM_NON_DCI_COVERED_PARAM_COLOR
                    row_ctr = row_ctr + 1
                    While ((Cells(row_ctr, nv_define_offset).text <> NV_MEM_CHECKSUM_STRING) And _
                            (Cells(row_ctr, nv_define_offset).text <> Empty) And _
                            (row_ctr < NV_MEMORY_LAST_POSSIBLE_ADDRESS_ROW))
                        Cells(row_ctr, nv_define_offset).Interior.ColorIndex = NV_MEM_NON_DCI_COVERED_PARAM_COLOR
                        row_ctr = row_ctr + 1
                    Wend
                    If (Cells(row_ctr, nv_define_offset).text = NV_MEM_CHECKSUM_STRING) Then
                        'We catch the checksum with a special color as well.
                        Cells(row_ctr, nv_define_offset).Interior.ColorIndex = NV_MEM_NON_DCI_COVERED_CHECKSUM_COLOR
                        Cells(row_ctr + 1, nv_define_offset).Interior.ColorIndex = NV_MEM_NON_DCI_COVERED_CHECKSUM_COLOR
                        row_ctr = row_ctr + 2
                    End If
                Else    ' Else we move on to the next parameter.
                    row_ctr = row_ctr + 1
                End If
            Else
                row_ctr = row_ctr + 1
            End If
        Wend
        
        
'****************************************************************************************************************************
'******     NV Memory size validation in NV Address Tab
'****************************************************************************************************************************
        Last_NVXX_Row = Worksheets("NVM Address").Cells(NV_MEM_DATA_START_ROW, NV_HEX_ADDRESS_COL).End(xlDown).row
        NVX_Max_Size = Cells(Last_NVXX_Row, NV_HEX_ADDRESS_COL).value
        If (NVX_Max_Size > Cells(4, NV_HEX_NUM_COL).value) Then
            Cells(Last_NVXX_Row, NV_HEX_ADDRESS_COL).Interior.ColorIndex = 16
            Cells(Last_NVXX_Row, NV_HEX_ADDRESS_COL).Borders.ColorIndex = 15
            Cells(Last_NVXX_Row, NV_HEX_ADDRESS_COL).Borders.LineStyle = xlContinuous
            Cells(Last_NVXX_Row, NV_HEX_ADDRESS_COL).Borders.Weight = xlThin
                
            Cells(4, NV_HEX_NUM_COL).Interior.ColorIndex = 16
            Cells(4, NV_HEX_NUM_COL).Borders.ColorIndex = 15
            Cells(4, NV_HEX_NUM_COL).Borders.LineStyle = xlContinuous
            Cells(4, NV_HEX_NUM_COL).Borders.Weight = xlThin
            'create a message box
            MsgBox Prompt:="NV Mem " & NVX_NUM & " Maximum Available Size = " & Cells(4, NV_HEX_NUM_COL).value & vbNewLine & "NV Mem " & NVX_NUM & " Parameter Size = " & Cells(Last_NVXX_Row, NV_HEX_ADDRESS_COL).value & vbNewLine & "Parameter size exceeded beyond the available size ", Title:="NV Memory " & NVX_NUM & " Size"
        Else
            Cells(Last_NVXX_Row, NV_HEX_ADDRESS_COL).Interior.ColorIndex = 2
            Cells(Last_NVXX_Row, NV_HEX_ADDRESS_COL).Borders.ColorIndex = 15
            Cells(Last_NVXX_Row, NV_HEX_ADDRESS_COL).Borders.LineStyle = xlContinuous
            Cells(Last_NVXX_Row, NV_HEX_ADDRESS_COL).Borders.Weight = xlThin
                
            Cells(4, NV_HEX_NUM_COL).Interior.ColorIndex = 2
            Cells(4, NV_HEX_NUM_COL).Borders.ColorIndex = 15
            Cells(4, NV_HEX_NUM_COL).Borders.LineStyle = xlContinuous
            Cells(4, NV_HEX_NUM_COL).Borders.Weight = xlThin
        End If
        
        NV_HEX_NUM_COL = NV_HEX_NUM_COL + NV_MEMORY_COL_OFFSET
        NV_HEX_ADDRESS_COL = NV_HEX_ADDRESS_COL + NV_MEMORY_COL_OFFSET
        NVX_NUM = NVX_NUM + 1
        'Modification for LTK-8063
        
        nv_col_counter = nv_col_counter + 1
        nv_address_offset = nv_address_offset + NV_MEMORY_COL_OFFSET
        nv_define_offset = nv_define_offset + NV_MEMORY_COL_OFFSET
    Wend
    
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
Public Sub Generate_Dec_Hex_Address()
Dim address_counter As Integer
Dim CellString As String
Dim CellString1 As String
Dim CellString2 As String
Dim row_found As Boolean
Application.Calculation = xlCalculationManual
'*********************
'******     Begin
'*********************
    Sheets("NVM Address").Select
    NV_MEM_DEFINE_END_STRING_PART_SIZE = Len("NV_DATA_END_NVCTRL")
    NV_MEM_DEFINE_START_STRING_PART_SIZE = Len("NV_DATA_START_NVCTRL")
    
    Dim dci_end_row As Integer
    
    dci_end_row = Last_DCI_Row()
'****************************************************************************************************************************
'******     Start at the top left of the first NV memory val
'****************************************************************************************************************************
    nv_col_counter = 0
    decimal_value = 0
    address_counter = 0
    nv_address_offset = NV_ADDRESS_COL
    nv_define_offset = NV_DEFINES_COL
        'These offsets are assuming that we are adding to 1 and not Zero.  Keep that in mind.
    EXCLUDE_CHECKSUM_SPACE_ROW = 5
    EXCLUDE_CHECKSUM_SPACE_COL_OFFSET = 1
    chip_col_counter = 1
    'First we start with column one and move across to the next.
    While nv_col_counter < NV_MEMORY_MAX_MEMORIES
        row_ctr = NV_MEM_DATA_START_ROW
        CellString = Cells(row_ctr, NV_DEFINES_COL).text
        CellString1 = NV_CRYPTO_META_DATA_END_STRING_PART & nv_col_counter
        CellString2 = NV_MEM_DEFINE_END_STRING_PART & nv_col_counter
        row_found = True
        
        If (nv_col_counter >= 0) Then
            address_counter = 0
            While ((row_found = True))
                CellString = Cells(row_ctr, (NV_DEFINES_COL + (5 * nv_col_counter))).text
                If (CellString = Empty And address_counter = 0) Then
                    row_found = False
                End If
                    If (Cells(EXCLUDE_CHECKSUM_SPACE_ROW, chip_col_counter + EXCLUDE_CHECKSUM_SPACE_COL_OFFSET).text = Empty) Then
                        Cells(row_ctr, nv_address_offset).value = address_counter
                        If (address_counter < 16) Then
                            Cells(row_ctr, nv_address_offset + 1).value = "0x000" & Evaluate("DEC2HEX(" & address_counter & ")")
                        End If
                        If (address_counter < 256) Then
                            Cells(row_ctr, nv_address_offset + 1).value = "0x00" & Evaluate("DEC2HEX(" & address_counter & ")")
                        End If
                        If (address_counter < 4096) Then
                            Cells(row_ctr, nv_address_offset + 1).value = "0x0" & Evaluate("DEC2HEX(" & address_counter & ")")
                        End If
                    Else
                        Cells(row_ctr, nv_address_offset).value = "-"
                        row_ctr = row_ctr + 1
                        Cells(row_ctr, nv_address_offset + 1).value = "-"
                    End If
                row_ctr = row_ctr + 1
                address_counter = address_counter + 1
                If (CellString = CellString1) Then
                    row_found = False
                Else
                    If (CellString = CellString2 And nv_col_counter <> 1) Then
                        row_found = False
                    End If
                End If
                    
                
            Wend
        End If
        nv_col_counter = nv_col_counter + 1
        nv_address_offset = nv_address_offset + 5
    Wend
    Application.Calculation = xlCalculationAutomatic
End Sub

