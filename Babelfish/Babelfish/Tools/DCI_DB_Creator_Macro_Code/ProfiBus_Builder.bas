Attribute VB_Name = "ProfiBus_Builder"
'**************************
'**************************
'Profibus Construction sheet.
'**************************
'**************************


Sub Create_Profibus_DCI_Xfer()

'*********************
'******     Constants
'*********************
    
    Application.Calculation = xlCalculationManual
    
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    
    Dim MyDate
    MyDate = Date    ' MyDate contains the current system date.
    
    Dim DCI_STRING As String
    Dim num_unique_data As Integer
'    Dim even_reg_count As Boolean
'    Dim even_unique_reg_count As Boolean
    
    
    START_ROW = 12
    

    DESCRIP_COL = 1
    DATA_DEFINE_COL = 2
    DATA_LENGTH_COL = 5
    DATA_PROFIBUS_REG_COL = 3
    '   DATA_NEXT_INDEX_COL = 5
    '   DATA_ATOMIBLOCK_SELECT = 5
    '   DATA_OFFSET_COL = 6
   ' DATA_MIN_COL = 5
   ' DATA_MAX_COL = 6
    DATA_TYPE_COL = 3
    DATA_TYPE_LENGTH_COL = 4
    '   PRM_DEFINE_COL = 11
    '   CFG_OUT_COL = 12
    '   CFG_IN_COL = 13
    '   CFG_DEFAULT_COL = 14
    '   CFG_LEN_COL = 9
    '   MSB_COL = 10
    '   LSB_COL = 11
    
    
    

'    DCI_DEFINES_START_ROW = 9
'    DCI_DESCRIPTORS_COL = 1
'    DCI_DEFINES_COL = 2
'    DCI_LENGTH_COL = 6

    
'*********************
'******     Begin
'*********************
    
    Sheets("Profibus (DCI)").Select

    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\" & Cells(3, 2).value
    Set PROFIBUS_DCI_C = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(4, 2).value
    Set PROFIBUS_DCI_H = fs.CreateTextFile(file_path, True)
    '   file_path = ActiveWorkbook.Path & "\" & Cells(6, 2).Value
    '   Set PROFIBUS_PRM_H = fs.CreateTextFile(file_path, True)
    '   file_path = ActiveWorkbook.Path & "\" & Cells(7, 2).Value
    '   Set PROFIBUS_PRM_C = fs.CreateTextFile(file_path, True)
    '   file_path = ActiveWorkbook.Path & "\" & Cells(9, 2).Value
    '   Set PROFIBUS_CFG_H = fs.CreateTextFile(file_path, True)
    '   file_path = ActiveWorkbook.Path & "\" & Cells(10, 2).Value
    '   Set PROFIBUS_CFG_C = fs.CreateTextFile(file_path, True)

    'Making font type and size consistent.
    Worksheets("Profibus (DCI)").Range("A:N").Font.name = "Arial"
    Worksheets("Profibus (DCI)").Range("A:N").Font.Size = 10


'****************************************************************************************************************************
'******     Start Creating the .H file header
'****************************************************************************************************************************
    PROFIBUS_DCI_H.WriteLine ("/**")
    PROFIBUS_DCI_H.WriteLine ("*****************************************************************************************")
    PROFIBUS_DCI_H.WriteLine ("*   @file")
    PROFIBUS_DCI_H.WriteLine ("*")
    PROFIBUS_DCI_H.WriteLine ("*   @brief")
    PROFIBUS_DCI_H.WriteLine ("*")
    PROFIBUS_DCI_H.WriteLine ("*   @details")
    PROFIBUS_DCI_H.WriteLine ("*")
    PROFIBUS_DCI_H.WriteLine ("*   @version")
    PROFIBUS_DCI_H.WriteLine ("*   C++ Style Guide Version 1.0")
    PROFIBUS_DCI_H.WriteLine ("*")
    PROFIBUS_DCI_H.WriteLine ("*   @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    PROFIBUS_DCI_H.WriteLine ("*")
    PROFIBUS_DCI_H.WriteLine ("*****************************************************************************************")
    PROFIBUS_DCI_H.WriteLine ("*/")
    PROFIBUS_DCI_H.WriteLine ("#ifndef " & UCase(Cells(2, 2).text) & "_H")
    PROFIBUS_DCI_H.WriteLine ("  #define " & UCase(Cells(2, 2).text) & "_H")
    PROFIBUS_DCI_H.WriteLine ("")
    PROFIBUS_DCI_H.WriteLine ("#include ""DCI.h""")
    PROFIBUS_DCI_H.WriteLine ("")
    
'****************************************************************************************************************************
'******     Sort the profibus register list
'****************************************************************************************************************************
    'Rows("12:499").Select
    Rows("12:499").Sort Key1:=Range("C9"), Order1:=xlAscending, _
        Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
        Orientation:=xlTopToBottom
    'Range("A9").Select

'****************************************************************************************************************************
'******     Gotta calculate the total number of Profibus registers we are going to need given a certain length.
'****************************************************************************************************************************
    num_registers = 0
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        counter = counter + 1
        num_registers = num_registers + 1
    Wend


'****************************************************************************************************************************
'******     Create the header and associated file structure.  Get ready to plop the data down.
'****************************************************************************************************************************
    PROFIBUS_DCI_H.WriteLine ("//*******************************************************")
    PROFIBUS_DCI_H.WriteLine ("//******     Profibus structure and size follow.")
    PROFIBUS_DCI_H.WriteLine ("//*******************************************************")
    PROFIBUS_DCI_H.WriteLine ("")
    PROFIBUS_DCI_H.WriteLine ("#define DCI_PROFIBUS_DATA_TOTAL_MODULES        " & num_registers & "U")
    PROFIBUS_DCI_H.WriteLine ("")
    PROFIBUS_DCI_H.WriteLine ("typedef uint16_t         PROFI_ID_TD;")
    PROFIBUS_DCI_H.WriteLine ("typedef struct DCI_PROFIBUS_DATA_LKUP_ST_TD")
    PROFIBUS_DCI_H.WriteLine ("{")
    PROFIBUS_DCI_H.WriteLine ("    PROFI_ID_TD profibus_id;")
    PROFIBUS_DCI_H.WriteLine ("    DCI_ID_TD dcid;")
    PROFIBUS_DCI_H.WriteLine ("} DCI_PROFIBUS_DATA_LKUP_ST_TD;")
    PROFIBUS_DCI_H.WriteLine ("extern const DCI_PROFIBUS_DATA_LKUP_ST_TD dci_profibus_lkup[DCI_PROFIBUS_DATA_TOTAL_MODULES];")
    PROFIBUS_DCI_H.WriteLine ("")
    PROFIBUS_DCI_H.WriteLine ("")
    PROFIBUS_DCI_H.WriteLine ("typedef struct DCI_ID_DATA_LKUP_ST_TD")
    PROFIBUS_DCI_H.WriteLine ("{")
    PROFIBUS_DCI_H.WriteLine ("    DCI_ID_TD dci_id;")
    PROFIBUS_DCI_H.WriteLine ("    PROFI_ID_TD profi_id;")
    PROFIBUS_DCI_H.WriteLine ("} DCI_ID_DATA_LKUP_ST_TD;")
    PROFIBUS_DCI_H.WriteLine ("extern const DCI_ID_DATA_LKUP_ST_TD dci_id_lkup[DCI_PROFIBUS_DATA_TOTAL_MODULES];")
    PROFIBUS_DCI_H.WriteLine ("")
    PROFIBUS_DCI_H.WriteLine ("")
    PROFIBUS_DCI_H.WriteLine ("//*******************************************************")
    PROFIBUS_DCI_H.WriteLine ("//******     Profibus Modules List")
    PROFIBUS_DCI_H.WriteLine ("//*******************************************************")
    PROFIBUS_DCI_H.WriteLine ("")
    
    PROFIBUS_DCI_H.WriteLine ("")
    row_counter = START_ROW
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        PROFIBUS_DCI_H.WriteLine ("#define " & Cells(row_counter, DATA_DEFINE_COL).text & "_PROFIBUS_MOD            " & _
                                Cells(row_counter, DATA_PROFIBUS_REG_COL).text & "U")
        row_counter = row_counter + 1
    Wend
    PROFIBUS_DCI_H.WriteLine ("")
    PROFIBUS_DCI_H.WriteLine ("")
    PROFIBUS_DCI_H.WriteLine ("#endif")



'****************************************************************************************************************************
'******     Start Creating the .C file header
'****************************************************************************************************************************
    PROFIBUS_DCI_C.WriteLine ("/**")
    PROFIBUS_DCI_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_DCI_C.WriteLine ("*   @file")
    PROFIBUS_DCI_C.WriteLine ("*   @details See header file for module overview.")
    PROFIBUS_DCI_C.WriteLine ("*   @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    PROFIBUS_DCI_C.WriteLine ("*")
    PROFIBUS_DCI_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_DCI_C.WriteLine ("*/")
    PROFIBUS_DCI_C.WriteLine ("#include ""Includes.h""")
    PROFIBUS_DCI_C.WriteLine ("#include """ & Cells(2, 2).value & ".h""")
    PROFIBUS_DCI_C.WriteLine ("")



'****************************************************************************************************************************
'******     This takes the profibus data and expands it to a big list with all parameters represented.
'****************************************************************************************************************************
'****************************************************************************************************************************
'******     Generate the straight profibus to DCI lookup table.
'****************************************************************************************************************************
    
    PROFIBUS_DCI_C.WriteLine ("")
    PROFIBUS_DCI_C.WriteLine ("")
    PROFIBUS_DCI_C.WriteLine ("//*******************************************************")
    PROFIBUS_DCI_C.WriteLine ("//******    The Profibus Parameter Cross Reference Table.")
    PROFIBUS_DCI_C.WriteLine ("//*******************************************************")
    PROFIBUS_DCI_C.WriteLine ("")
    
    PROFIBUS_DCI_C.WriteLine ("const DCI_PROFIBUS_DATA_LKUP_ST_TD dci_profibus_lkup[DCI_PROFIBUS_DATA_TOTAL_MODULES] = ")
    PROFIBUS_DCI_C.WriteLine ("{")
    row_counter = START_ROW
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        PROFIBUS_DCI_C.WriteLine ("    {        // " & Cells(row_counter, DESCRIP_COL).text)
        PROFIBUS_DCI_C.WriteLine ("        " & Cells(row_counter, DATA_DEFINE_COL).text & "_PROFIBUS_MOD,")
        PROFIBUS_DCI_C.WriteLine ("        " & Cells(row_counter, DATA_DEFINE_COL).text & "_DCID,")
        If (Cells(row_counter, DATA_DEFINE_COL).text = Empty) Or (Cells(row_counter, DATA_DEFINE_COL).text = BEGIN_IGNORED_DATA) Then
            PROFIBUS_DCI_C.WriteLine ("    } ")
        Else
            PROFIBUS_DCI_C.WriteLine ("    }, ")
        End If
        row_counter = row_counter + 1
    Wend
    PROFIBUS_DCI_C.WriteLine ("};")
        
'****************************************************************************************************************************
'******     Generate the reverse DCI to profibus lookup table.
'****************************************************************************************************************************


    PROFIBUS_DCI_C.WriteLine ("")
    PROFIBUS_DCI_C.WriteLine ("")
    PROFIBUS_DCI_C.WriteLine ("//*******************************************************")
    PROFIBUS_DCI_C.WriteLine ("//***The DCI to Profibus Parameter Cross Reference Table.")
    PROFIBUS_DCI_C.WriteLine ("//*******************************************************")
    PROFIBUS_DCI_C.WriteLine ("")
    
    dci_defines_row_ctr = DCI_DEFINES_START_ROW
    PROFIBUS_DCI_C.WriteLine ("const DCI_ID_DATA_LKUP_ST_TD dci_id_lkup[DCI_PROFIBUS_DATA_TOTAL_MODULES] = ")
    PROFIBUS_DCI_C.WriteLine ("{")
    While Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> Empty
        DCI_STRING = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text
        row_counter = START_ROW
        While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA) _
                    And (Cells(row_counter, DATA_DEFINE_COL).text <> DCI_STRING)
            row_counter = row_counter + 1
        Wend
        If Cells(row_counter, DATA_DEFINE_COL).text = DCI_STRING Then
            PROFIBUS_DCI_C.WriteLine ("    {        // " & Cells(row_counter, DESCRIP_COL).text)
            PROFIBUS_DCI_C.WriteLine ("        " & DCI_STRING & "_DCID,")     'Cells(row_counter, DATA_MODBUS_REG_COL).Value & ",")
            
            PROFIBUS_DCI_C.WriteLine ("        " & Cells(row_counter, DATA_DEFINE_COL).text & "_PROFIBUS_MOD")
            If (Cells(row_counter + 1, DATA_DEFINE_COL).text = Empty) Then
                PROFIBUS_DCI_C.WriteLine ("    } ")
            Else
                PROFIBUS_DCI_C.WriteLine ("    }, ")
            End If
        End If
        dci_defines_row_ctr = dci_defines_row_ctr + 1
    Wend
    PROFIBUS_DCI_C.WriteLine ("};")
    

    PROFIBUS_DCI_C.Close
    PROFIBUS_DCI_H.Close
   
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

Public Sub Verify_Profibus_List()

    Application.Calculation = xlCalculationManual

'*********************
'******     Constants
'*********************
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    
    START_ROW = 12
    DCI_DEFINES_START_ROW = 9
    DCI_DESCRIPTORS_COL = 1
    DCI_DEFINES_COL = 2
    DCI_DATA_TYPE_COL = 3
    DCI_DATA_TYPE_LENGTH_COL = 4
    DCI_LENGTH_COL = 6
    DCI_DATA_MIN_COL = 8
    DCI_DATA_MAX_COL = 9
    

    DESCRIP_COL = 1
    DATA_DEFINE_COL = 2
    DATA_LENGTH_COL = 3
    DATA_MIN_COL = 5
    DATA_MAX_COL = 6
    DATA_TYPE_COL = 7
    DATA_TYPE_LENGTH_COL = 8
    DATA_DESC_MISSING_COL = DESCRIP_COL
    DATA_DEFINE_MISSING_COL = DATA_DEFINE_COL
    DATA_LENGTH_MISSING_COL = DATA_LENGTH_COL
    DATA_MIN_MISSING_COL = DATA_MIN_COL
    DATA_MAX_MISSING_COL = DATA_MAX_COL
    DATA_TYPE_MISSING_COL = DATA_TYPE_COL
    DATA_TYPE_LENGTH_MISSING_COL = DATA_TYPE_LENGTH_COL
    


'*********************
'******     Begin
'*********************
    Sheets("Profibus (DCI)").Select

    
    
'****************************************************************************************************************************
'******     Find end of Profibus parameter list.
'****************************************************************************************************************************
    end_of_list = START_ROW
    While (Cells(end_of_list, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        end_of_list = end_of_list + 1
    Wend
    end_of_list = end_of_list - 1
    
'****************************************************************************************************************************
'******     Find where we should put the missing parameter list.
'****************************************************************************************************************************
    
    missing_row_counter = end_of_list + 2

'****************************************************************************************************************************
'******     Find the missing profibus data items
'****************************************************************************************************************************
    dci_defines_row_ctr = DCI_DEFINES_START_ROW
    While Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> Empty
        lookup_string = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text
        Set C = Worksheets("Profibus (DCI)").Range(Cells(12, 2), Cells(end_of_list, 2)).Find(lookup_string, LookAt:=xlWhole)
        If C Is Nothing Then
            Cells(missing_row_counter, DATA_DESC_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DESCRIPTORS_COL).text
            Cells(missing_row_counter, DATA_DEFINE_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text
            ' The following line is for creating a link between the length and the data id.
            'Cells(missing_counter, DATA_LENGTH_MISSING_COL).Value = "='DCI Descriptors'!" & "$F$" & dci_defines_row_ctr
            ' The next line will just copy the size and not correct it.
            Cells(missing_row_counter, DATA_LENGTH_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_LENGTH_COL).value
            missing_row_counter = missing_row_counter + 1
        End If
        dci_defines_row_ctr = dci_defines_row_ctr + 1
    Wend


    Sheets("Profibus (DCI)").Select
'****************************************************************************************************************************
'******     Find the Data items that have been removed from the main list
'****************************************************************************************************************************
    profibus_row_ctr = START_ROW
    missing_counter = START_ROW
    While Cells(profibus_row_ctr, DATA_DEFINE_COL).text <> Empty
        lookup_string = Cells(profibus_row_ctr, DATA_DEFINE_COL).text
        If lookup_string <> BEGIN_IGNORED_DATA Then
            Set C = Worksheets("DCI Descriptors").Range("B9:B500").Find(lookup_string, LookAt:=xlWhole)
            If C Is Nothing Then
                Cells(profibus_row_ctr, DATA_DEFINE_COL).Interior.Color = RGB(255, 0, 0)
                Cells(profibus_row_ctr, DESCRIP_COL).Interior.Color = RGB(255, 0, 0)
            End If
        End If
        profibus_row_ctr = profibus_row_ctr + 1
    Wend


'****************************************************************************************************************************
'******     This will go through the main list and make the lengths correct in the slave sheets
'****************************************************************************************************************************
    dci_defines_row_ctr = DCI_DEFINES_START_ROW
    While Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> Empty
        local_row_ctr = START_ROW
        While (Cells(local_row_ctr, DATA_DEFINE_MISSING_COL).value <> Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text) And _
                        (Cells(local_row_ctr, DATA_DEFINE_MISSING_COL).value <> Empty)
            local_row_ctr = local_row_ctr + 1
        Wend
        If (Cells(local_row_ctr, DATA_DEFINE_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text) Then
            Cells(local_row_ctr, DATA_LENGTH_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_LENGTH_COL).text
        End If
        dci_defines_row_ctr = dci_defines_row_ctr + 1
    Wend


'****************************************************************************************************************************
'******     This will go through the main list and make the range correct in the slave sheets
'****************************************************************************************************************************
    dci_defines_row_ctr = DCI_DEFINES_START_ROW
    While Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> Empty
        local_row_ctr = START_ROW
        While (Cells(local_row_ctr, DATA_DEFINE_MISSING_COL).value <> Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text) And _
                        (Cells(local_row_ctr, DATA_DEFINE_MISSING_COL).value <> Empty)
            local_row_ctr = local_row_ctr + 1
        Wend
        If (Cells(local_row_ctr, DATA_DEFINE_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text) Then
            Cells(local_row_ctr, DATA_MIN_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DATA_MIN_COL).text
            Cells(local_row_ctr, DATA_MAX_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DATA_MAX_COL).text
            Cells(local_row_ctr, DATA_TYPE_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DATA_TYPE_COL).text
            Cells(local_row_ctr, DATA_TYPE_LENGTH_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DATA_TYPE_LENGTH_COL).text
        End If
        dci_defines_row_ctr = dci_defines_row_ctr + 1
    Wend

    Application.Calculation = xlCalculationAutomatic

End Sub
