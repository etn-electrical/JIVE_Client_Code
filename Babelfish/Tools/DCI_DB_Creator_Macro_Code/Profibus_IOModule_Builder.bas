Attribute VB_Name = "Profibus_IOModule_Builder"

    'Dim BEGIN_IGNORED_DATA As String
    Const BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"


Sub Configure_Profibus_Modules()
'
' Configure_Profibus_Modules Macro
'
PROFI_START_ROW = 12
DATA_DEFINES_COL = 2
DATA_TYPE_COL = 4
DATA_TYPE_LIST_COL = 13
DATA_TYPE_LIST_ROW_START = 1
MODULE_HEADING_COLOR = 15 '22
MODULE_SUB_HEADING_COLOR = 15
Const TOTAL_DATA_TYPE_TO_PROCESS = 6
INPUT_SIZE_BYTE_COL = 9
OUTPUT_SIZE_BYTE_COL = 10
MODULE_DATA_TYPE_COL = 1
M_DATA_TYPE_COL = 1
M_OFFSET_COL = 2
M_PARAMETER_COL = 3
MODULE_NUMBER_COL = 2
'USER_PARAM_FIRST_ROW = 31
UASER_PARAM_FIRST_COL = 5
DCI_DEFAULT_COL = 7
INPUT_BIT_TYPE = "CYCLIC INPUT BIT"
INPUT_WORD_TYPE = "CYCLIC INPUT WORD"
INPUT_DWORD_TYPE = "CYCLIC INPUT DWORD"
OUTPUT_BIT_TYPE = "CYCLIC OUTPUT BIT"
OUTPUT_WORD_TYPE = "CYCLIC OUTPUT WORD"
OUTPUT_DWORD_TYPE = "CYCLIC OUTPUT DWORD"

Dim DATA_TYPE_LIST(1 To TOTAL_DATA_TYPE_TO_PROCESS) As String

DATA_TYPE_LIST(1) = INPUT_BIT_TYPE
DATA_TYPE_LIST(2) = INPUT_WORD_TYPE
DATA_TYPE_LIST(3) = INPUT_DWORD_TYPE
DATA_TYPE_LIST(4) = OUTPUT_BIT_TYPE
DATA_TYPE_LIST(5) = OUTPUT_WORD_TYPE
DATA_TYPE_LIST(6) = OUTPUT_DWORD_TYPE

Dim input_bit_List As String
Dim input_word_List As String
Dim input_dword_List As String
Dim output_bit_List As String
Dim output_word_List As String
Dim output_dword_List As String

Dim PARAM_LIST(1 To TOTAL_DATA_TYPE_TO_PROCESS) As String

Dim VALIDATION_LIST_COL(1 To TOTAL_DATA_TYPE_TO_PROCESS) As String
VALIDATION_LIST_COL(1) = "$AA"
VALIDATION_LIST_COL(2) = "$AB"
VALIDATION_LIST_COL(3) = "$AC"
VALIDATION_LIST_COL(4) = "$AD"
VALIDATION_LIST_COL(5) = "$AE"
VALIDATION_LIST_COL(6) = "$AF"

Const BEGIN_META_DATA = "BEGIN_META_DATA"
META_DATA_COL_START = 27 '"AA"


Const DEFAULT_VALIDATION_LIST_ROW_SIZE_START = 40
Const DEFAULT_VALIDATION_LIST_ROW_START = 41
                
Dim VALIDATION_LIST_ROW_SIZE_START As Integer
Dim VALIDATION_LIST_ROW_START As Integer

Fill_INPUT_WORD_TABLE
Fill_INPUT_DWORD_TABLE
Fill_INPUT_BIT_TABLE
Fill_OUTPUT_BIT_TABLE
Fill_OUTPUT_WORD_TABLE
Fill_OUTPUT_DWORD_TABLE

input_bit_List = IP_BIT_VALUE_TABLE.enum_descr
input_bit_List = Replace(input_bit_List, ";", ",")
input_word_List = IP_WORD_VALUE_TABLE.enum_descr
input_word_List = Replace(input_word_List, ";", ",")
input_dword_List = IP_DWORD_VALUE_TABLE.enum_descr
input_dword_List = Replace(input_dword_List, ";", ",")
output_bit_List = OP_BIT_VALUE_TABLE.enum_descr
output_bit_List = Replace(output_bit_List, ";", ",")
output_word_List = OP_WORD_VALUE_TABLE.enum_descr
output_word_List = Replace(output_word_List, ";", ",")
output_dword_List = OP_DWORD_VALUE_TABLE.enum_descr
output_dword_List = Replace(output_dword_List, ";", ",")
    
PARAM_LIST(1) = input_bit_List
PARAM_LIST(2) = input_word_List
PARAM_LIST(3) = input_dword_List
PARAM_LIST(4) = output_bit_List
PARAM_LIST(5) = output_word_List
PARAM_LIST(6) = output_dword_List

Dim meta_data_found As Boolean
Dim row_counter As Integer
row_counter = 1
        '  search for end of metadata columns (starts from AA:AF)
        While ((Worksheets("ProfiBus(IO Modules)").Cells(row_counter, META_DATA_COL_START).text <> BEGIN_META_DATA) And (row_counter < 5000))
        row_counter = row_counter + 1
        Wend
        ' set the variables(to be used in code subsequently) with row number
        If row_counter < 5000 Then ' random value this limit should not cross
                VALIDATION_LIST_ROW_SIZE_START = row_counter + 1
                VALIDATION_LIST_ROW_START = row_counter + 2
                meta_data_found = True
                
        Else
        ' begin ignore data row not found, set the default row number
                VALIDATION_LIST_ROW_SIZE_START = DEFAULT_VALIDATION_LIST_ROW_SIZE_START
                VALIDATION_LIST_ROW_START = DEFAULT_VALIDATION_LIST_ROW_START
                meta_data_found = False
                Worksheets("ProfiBus(IO Modules)").Cells(VALIDATION_LIST_ROW_SIZE_START - 1, META_DATA_COL_START).value = BEGIN_META_DATA
        End If

        For It_r = 1 To TOTAL_DATA_TYPE_TO_PROCESS
        Dim temp_str_temp As String
        
        If meta_data_found = True Then
        ' find the cell which contain size input paramters for each data type and the clear the previous meta data
                temp_str_temp = VALIDATION_LIST_COL(It_r) & VALIDATION_LIST_ROW_SIZE_START
                temp_str_temp = Worksheets("ProfiBus(IO Modules)").Range(temp_str_temp).value
                temp_str_temp = VALIDATION_LIST_COL(It_r) & "$" & (VALIDATION_LIST_ROW_START + temp_str_temp)
                Range(VALIDATION_LIST_COL(It_r) & VALIDATION_LIST_ROW_SIZE_START & ":" & temp_str_temp).Clear
        End If
        
         Dim Names() As String
         Names() = Split(PARAM_LIST(It_r), ",")
         Worksheets("ProfiBus(IO Modules)").Cells(VALIDATION_LIST_ROW_SIZE_START, META_DATA_COL_START + It_r - 1).value = UBound(Names)
         For i = 0 To UBound(Names) - 1
            'add list of paramters for each data type to be used as meta data (drop down list)
            Worksheets("ProfiBus(IO Modules)").Cells(VALIDATION_LIST_ROW_START + i, META_DATA_COL_START + It_r - 1).value = Names(i)
         Next
            ' clear end of list to avoid confusion
            Worksheets("ProfiBus(IO Modules)").Cells(VALIDATION_LIST_ROW_START + i, META_DATA_COL_START + It_r - 1).value = ""
        Next It_r
    
MODULE_START_ROW = 26
counter = MODULE_START_ROW
Module_Number = 0
Itr = 0
offset = 0
temp_count = MODULE_START_ROW
end_of_begin_ign_data = 1
' find end of module list
While ((Cells(temp_count, 2).text <> BEGIN_IGNORED_DATA) And Cells(temp_count, 2).value <> Empty)
end_of_begin_ign_data = end_of_begin_ign_data + 1
temp_count = temp_count + 1
Wend

Dim ask_for_backup_table As Boolean
ask_for_backup_table = False
' search for second begin ignored data
next_begin_ign_data_count = temp_count + 1
user_param_bi_ig_data_found = False
While ((Cells(next_begin_ign_data_count, 1).text <> BEGIN_IGNORED_DATA) And (next_begin_ign_data_count < 5000))
    next_begin_ign_data_count = next_begin_ign_data_count + 1
    
    If (Cells(next_begin_ign_data_count, 3).text <> Empty) And (Cells(next_begin_ign_data_count, 3).text <> "Parameter") Then
        ' Is any param assigned in module configuration table
        ' we should ask user if wants to backup table
            ask_for_backup_table = True
    End If
    ' find parallelly end of user definatable parameters list as well
    If (Cells(next_begin_ign_data_count, 5).text = BEGIN_IGNORED_DATA) Then
      user_param_bi_ig_data_row = next_begin_ign_data_count
    End If
    ' Is any connection assigned in user definable paramters table
    If user_param_bi_ig_data_row = 0 Then
        If (Cells(next_begin_ign_data_count, 7).text <> Empty) And (Cells(next_begin_ign_data_count, 7).text <> "Connection") Then
            ask_for_user_param_backup = True
        End If
    End If
Wend

' find the row which we want to clean up to
If user_param_bi_ig_data_row > next_begin_ign_data_count Then
    next_begin_ign_data_count = user_param_bi_ig_data_row
End If

' Is there any data to be backed up
If ((ask_for_backup_table = True) Or (ask_for_user_param_backup = True)) Then
    Dim answer As Integer
    answer = MsgBox("Data in Module Configuration Table will be lost. Do you want to save current module configuration?", vbYesNo)
    If answer = vbYes Then
        Dim rFileFullName As String
        
          rFileFullName = ActiveWorkbook.Path
          rFileFullName = rFileFullName & "\"
          
          new_file_name = "ProfibusIOmodules_"
          
          Range(Cells(temp_count + 1, M_DATA_TYPE_COL), Cells(next_begin_ign_data_count + 1, M_PARAMETER_COL + 4)).Copy
          Set NewBook = Workbooks.Add
          ' Copy tables in new xls
          NewBook.Worksheets("Sheet1").Range("A1").PasteSpecial (xlPasteValues)
          NewBook.Worksheets("Sheet1").Range("A1").PasteSpecial (xlPasteFormats)
          NewBook.SaveAs Filename:=rFileFullName & new_file_name & format(Now(), "mm_dd_yyyy_hh_mm"), FileFormat:=xlNormal
          NewBook.Close
          MsgBox ("Current Module Configuration backed up at location -" & rFileFullName)
    End If
End If
' clear the table data
Range(Cells(temp_count + 1, M_DATA_TYPE_COL), Cells(next_begin_ign_data_count + 1, M_PARAMETER_COL + 4)).Clear

counter = 0
Module_Number = 0
While ((Cells(MODULE_START_ROW + Module_Number, 2).text <> BEGIN_IGNORED_DATA) And (Cells(MODULE_START_ROW + Module_Number, 2).text <> Empty))
    Itr = 0
    If (Cells(MODULE_START_ROW + Module_Number, INPUT_SIZE_BYTE_COL).value <> 0) Then
        offset = offset + 1

        ' foramtting the table header
        With Cells(MODULE_START_ROW + offset + counter + end_of_begin_ign_data, 1)
         .value = "Module " & Module_Number + 1 & " Configuration"
         .Interior.ColorIndex = MODULE_HEADING_COLOR
         .Font.Bold = True
         .Borders.LineStyle = xlContinuous
         .HorizontalAlignment = xlCenter
        End With
        
        ' merge three columns
        With Range(Cells(MODULE_START_ROW + offset + counter + end_of_begin_ign_data, 1), Cells(MODULE_START_ROW + offset + counter + end_of_begin_ign_data, 3))
        .Merge (False)
        '.Borders.LineStyle = xlContinuous
        End With
        
        ' add column header names
        offset = offset + 1
        Cells(MODULE_START_ROW + offset + counter + end_of_begin_ign_data, 1).value = "Data Type"
        Cells(MODULE_START_ROW + offset + counter + end_of_begin_ign_data, 2).value = "Offset"
        Cells(MODULE_START_ROW + offset + counter + end_of_begin_ign_data, 3).value = "Parameter"
        
        ' formatting header columns
        With Range(Cells(MODULE_START_ROW + offset + counter + end_of_begin_ign_data - 1, 1), Cells(MODULE_START_ROW + offset + counter + end_of_begin_ign_data, 3))
         .Interior.ColorIndex = MODULE_SUB_HEADING_COLOR
         .Borders.LineStyle = xlContinuous
         .HorizontalAlignment = xlCenter
        End With
        
        offset = offset + 1
        
        ' will be processed only for input paramters(bit, word, dword)
        For j = 1 To TOTAL_DATA_TYPE_TO_PROCESS - 3
            sub_itr = 0
            ' add value and ofset in module configuration table
            While (sub_itr < (Cells(MODULE_START_ROW + Module_Number, j + MODULE_NUMBER_COL).value))
                Cells(MODULE_START_ROW + offset + counter + end_of_begin_ign_data + Itr, M_DATA_TYPE_COL).value = DATA_TYPE_LIST(j)
                Cells(MODULE_START_ROW + offset + counter + end_of_begin_ign_data + Itr, M_OFFSET_COL).value = sub_itr
                sub_itr = sub_itr + 1
                Itr = Itr + 1
            Wend
            
            If sub_itr > 0 Then
                ' find the cell which contain size input paramters for each data type and then insert drop down list for paramters
                temp_str = VALIDATION_LIST_COL(j) & VALIDATION_LIST_ROW_SIZE_START
                temp_str = Worksheets("ProfiBus(IO Modules)").Range(temp_str).value
                temp_str = VALIDATION_LIST_COL(j) & "$" & (VALIDATION_LIST_ROW_START + temp_str)
                Call insert_dropdown_paramaters(Range(Cells(MODULE_START_ROW + offset + counter + end_of_begin_ign_data + (Itr - sub_itr), M_PARAMETER_COL), Cells(MODULE_START_ROW + offset + counter + end_of_begin_ign_data + Itr - 1, M_PARAMETER_COL)), "=" & VALIDATION_LIST_COL(j) & "$" & VALIDATION_LIST_ROW_START & ":" & temp_str)
                Range(Cells(MODULE_START_ROW + offset + counter + end_of_begin_ign_data + (Itr - sub_itr), M_DATA_TYPE_COL), Cells(MODULE_START_ROW + offset + counter + end_of_begin_ign_data + Itr - 1, M_PARAMETER_COL)).Borders.LineStyle = xlContinuous
            End If
        Next j
    offset = offset + Itr
    counter = counter + 1
    End If
    Module_Number = Module_Number + 1
Wend
    ' add begin ignore data at the end
    Cells(MODULE_START_ROW + offset + counter + end_of_begin_ign_data - 1, 1).value = "BEGIN_IGNORED_DATA"
    Cells(MODULE_START_ROW + offset + counter + end_of_begin_ign_data - 1, 1).Interior.ColorIndex = 22

offset = 0
' Start of User Definable Parameters Confguration
' is there any output paramters

user_counter = 0
Dim INIT_PARAM_FOUND As Boolean
INIT_PARAM_FOUND = False

While ((Worksheets("ProfiBus (DCI)").Cells(PROFI_START_ROW + user_counter, 2).text <> Empty) And (INIT_PARAM_FOUND = False))
    If (Worksheets("ProfiBus (DCI)").Cells(PROFI_START_ROW + user_counter, DATA_TYPE_COL).text = "INITIALIZATION PARAMETER") Then
         INIT_PARAM_FOUND = True
    End If
    user_counter = user_counter + 1
Wend
               
If ((OUTPUT_PARAM_LIST(0).max > 0) Or (OUTPUT_PARAM_LIST(1).max > 0) Or (OUTPUT_PARAM_LIST(2).max > 0) Or (INIT_PARAM_FOUND = True)) Then
        offset = offset + 1
        start_of_user_config_table_row = MODULE_START_ROW + offset + end_of_begin_ign_data
        ' formatting of user definable parameters configuration table
        With Cells(MODULE_START_ROW + offset + end_of_begin_ign_data, UASER_PARAM_FIRST_COL)
         .value = "User Definable Parameters Configuration"
         .Interior.ColorIndex = MODULE_HEADING_COLOR
         .Font.Bold = True
         .HorizontalAlignment = xlCenter
        End With
        
        With Range(Cells(MODULE_START_ROW + offset + end_of_begin_ign_data, UASER_PARAM_FIRST_COL), Cells(MODULE_START_ROW + offset + end_of_begin_ign_data, UASER_PARAM_FIRST_COL + 2))
        .Merge (False)
        End With
        
        offset = offset + 1
        Cells(MODULE_START_ROW + offset + end_of_begin_ign_data, UASER_PARAM_FIRST_COL).value = "Data Type"
        Cells(MODULE_START_ROW + offset + end_of_begin_ign_data, UASER_PARAM_FIRST_COL + 1).value = "Parameter"
        Cells(MODULE_START_ROW + offset + end_of_begin_ign_data, UASER_PARAM_FIRST_COL + 2).value = "Connection"
        
        With Range(Cells(MODULE_START_ROW + offset + end_of_begin_ign_data - 1, UASER_PARAM_FIRST_COL), Cells(MODULE_START_ROW + offset + end_of_begin_ign_data, UASER_PARAM_FIRST_COL + 2))
         .Interior.ColorIndex = MODULE_SUB_HEADING_COLOR
         '.Borders.LineStyle = xlContinuous
         .HorizontalAlignment = xlCenter
        End With
                
                
''''''''''''''''INIT PARAMETER START ''''''''''''''''''''''''''''''''
    ExtUserPrmData = 1
    Enum_val = 7        'LTK Specific Change we are supporting general profiles.not product specific profiles like MMP
    counter = PROFI_START_ROW
    Dim default As String
    DCI_MIN_COL = 8
    DCI_MAX_COL = 9
    'SHORT_DESCRIP_COL = 79
    'DCI_ENUM_SUPP_COL = 35
    'DESCRIPTION_COL = 81
    Dim k As Integer
    k = 0
    
    USER_PARAM_FIRST_ROW = temp_count + 2
    
    Dim temp_string As String
    Dim splitStr() As String
    
    While ((Worksheets("ProfiBus (DCI)").Cells(counter, 2).text <> Empty) And (Worksheets("ProfiBus (DCI)").Cells(counter, 2).text <> BEGIN_IGNORED_DATA))
               If (Worksheets("ProfiBus (DCI)").Cells(counter, DATA_TYPE_COL).text = "INITIALIZATION PARAMETER") Then
                    dci_sheet_row = Find_DCID_Row(Worksheets("ProfiBus (DCI)").Cells(counter, 2))
                                  
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
                    
                    Cells(USER_PARAM_FIRST_ROW + offset + k, UASER_PARAM_FIRST_COL).value = "INITIALIZATION PARAMETER"
                    Cells(USER_PARAM_FIRST_ROW + offset + k, UASER_PARAM_FIRST_COL + 1).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_DESCRIPTORS_COL).text
                    
                    'Enum Values
                    If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, ENUM_ATTRIB_COL).text <> Empty) Then
                        Dim default_str As String
                        
                        description_content_string = Worksheets("DCI Descriptors").Cells(dci_sheet_row, DCI_LONG_DESC_COL).text
                        descriptionString = Split(description_content_string, vbLf)
                        For count = 0 To UBound(descriptionString) - 1
                            splitStr() = Split(descriptionString(count + 1), " = ")
                            temp_string = temp_string & splitStr(0) & "=" & splitStr(1) & ","
                            If (splitStr(0) = default) Then
                            default_str = splitStr(0) & "=" & splitStr(1)
                            End If
                        Next count
                        
                        
                        Call insert_dropdown_paramaters((Cells(USER_PARAM_FIRST_ROW + offset + k, UASER_PARAM_FIRST_COL + 2)), temp_string)
                        Cells(USER_PARAM_FIRST_ROW + offset + k, UASER_PARAM_FIRST_COL + 2).value = default_str
                    Else
                            Cells(USER_PARAM_FIRST_ROW + offset + k, UASER_PARAM_FIRST_COL + 2).value = default
                            With Cells(USER_PARAM_FIRST_ROW + offset + k, UASER_PARAM_FIRST_COL + 2).Validation
                            
                            .Delete
                            .Add type:=xlValidateWholeNumber, AlertStyle:=xlValidAlertStop, Operator:=xlBetween, Formula1:=start_range, Formula2:=end_range 'Minimum:=start_range, Maximum:=end_range
                            .InputTitle = ""
                            .ErrorTitle = ""
                            .InputMessage = "Enter a value between " & start_range & " and " & end_range & "."
                            .ErrorMessage = "You must enter a integer from " & start_range & " to " & end_range & "."
                            .ShowInput = True
                            .ShowError = True
                            End With
                    End If
                    k = k + 1
               End If
    counter = counter + 1
    Wend

    offset = offset + k
''''''''''''''''INIT PARAMETER END ''''''''''''''''''''''''''''''''
                
        For j = 3 To TOTAL_DATA_TYPE_TO_PROCESS
            
            If (OUTPUT_PARAM_LIST(j - 3).max > 0) Then
                Dim out_Names() As String
                out_Names() = Split(OUTPUT_PARAM_LIST(j - 3).enum_descr, ",")
        
                For k = 0 To (OUTPUT_PARAM_LIST(j - 3).max - 1)
                    Cells(USER_PARAM_FIRST_ROW + offset + k, UASER_PARAM_FIRST_COL).value = DATA_TYPE_LIST(j + 1)
                    Cells(USER_PARAM_FIRST_ROW + offset + k, UASER_PARAM_FIRST_COL + 1).value = out_Names(k)
                Next
                If k > 0 Then
                    temp_str = VALIDATION_LIST_COL(j + 1) & VALIDATION_LIST_ROW_SIZE_START
                    temp_str = Worksheets("ProfiBus(IO Modules)").Range(temp_str).value
                    temp_str = VALIDATION_LIST_COL(j + 1) & "$" & (VALIDATION_LIST_ROW_START + temp_str)
                    Call insert_dropdown_paramaters(Range(Cells(USER_PARAM_FIRST_ROW + offset, UASER_PARAM_FIRST_COL + 2), Cells(USER_PARAM_FIRST_ROW + offset + k - 1, UASER_PARAM_FIRST_COL + 2)), "=" & VALIDATION_LIST_COL(j + 1) & "$" & VALIDATION_LIST_ROW_START & ":" & temp_str)
                End If
                
                offset = offset + k
            End If
        Next
    Range(Cells(start_of_user_config_table_row, UASER_PARAM_FIRST_COL), Cells(USER_PARAM_FIRST_ROW + offset - 1, UASER_PARAM_FIRST_COL + 2)).Borders.LineStyle = xlContinuous
    Cells(USER_PARAM_FIRST_ROW + offset, UASER_PARAM_FIRST_COL).value = "BEGIN_IGNORED_DATA"
    Cells(USER_PARAM_FIRST_ROW + offset, UASER_PARAM_FIRST_COL).Interior.ColorIndex = 22
End If


End Sub

Private Sub insert_dropdown_paramaters(cell_num As Range, PARAM_LIST As String)
   
    With cell_num.Validation
    
    .Delete
    .Add type:=xlValidateList, AlertStyle:=xlValidAlertStop, _
    Operator:=xlBetween, Formula1:=PARAM_LIST '"=AA42:AA70" 'PARAM_LIST  '"=" & start_range & ":" & end_range    '"=Sheet1!A1:A6"
    .IgnoreBlank = True
    .InCellDropdown = True
    .InputTitle = ""
    .ErrorTitle = ""
    .InputMessage = ""
    .ErrorMessage = ""
    .ShowInput = True
    .ShowError = True
    End With
End Sub



Sub Create_Module_Info_Files()

    Application.Calculation = xlCalculationManual

'*********************
'******     Constants
'*********************
    
    'Dim BEGIN_IGNORED_DATA As String
    'BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    
    Dim MyDate
    MyDate = Date    ' MyDate contains the current system date.
    
    Dim DCI_STRING As String
    Dim num_unique_data As Integer
'    Dim even_reg_count As Boolean
'    Dim even_unique_reg_count As Boolean
    Dim dci_sheet_row_temp  As Integer
    PROFI_START_ROW = 26 '612
    MODULE_NUMBER_COL = 2
    INPUT_BIT_COL = MODULE_NUMBER_COL + 1
    INPUT_WORD_COL = INPUT_BIT_COL + 1
    INPUT_DWORD_COL = INPUT_WORD_COL + 1
    OUTPUT_BIT_COL = INPUT_DWORD_COL + 1
    OUTPUT_WORD_COL = OUTPUT_BIT_COL + 1
    OUTPUT_DWORD_COL = OUTPUT_WORD_COL + 1
    INPUT_SIZE_COL = OUTPUT_DWORD_COL + 1
    OUTPUT_SIZE_COL = INPUT_SIZE_COL + 1
    
    
    LTK_PROFI_OP_BIT_LKUP_FILENAME_ROW = 3 '587
    LTK_PROFI_OP_BIT_LKUP_FILENAME_COL = 6
    LTK_PROFI_OP_WORD_LKUP_FILENAME_ROW = 6 '590
    LTK_PROFI_OP_WORD_LKUP_FILENAME_COL = 6
    LTK_PROFI_OP_COMP_PRM_FILENAME_ROW = 9 '593
    LTK_PROFI_OP_COMP_PRM_FILENAME_COL = 6
    
    LTK_PROFI_OP_BIT_LKUP_CPP_PATH_ROW = 4 '588
    LTK_PROFI_OP_BIT_LKUP_CPP_PATH_COL = 6
    LTK_PROFI_OP_BIT_LKUP_H_PATH_ROW = 5 '589
    LTK_PROFI_OP_BIT_LKUP_H_PATH_COL = 6
    LTK_PROFI_OP_WORD_LKUP_CPP_PATH_ROW = 8 '592
    LTK_PROFI_OP_WORD_LKUP_CPP_PATH_COL = 6
    LTK_PROFI_OP_WORD_LKUP_H_PATH_ROW = 7 '591
    LTK_PROFI_OP_WORD_LKUP_H_PATH_COL = 6
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP_PATH_ROW = 11 '595
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP_PATH_COL = 6
    LTK_PROFI_OP_COMP_PRM_STRUCT_H_PATH_ROW = 10 '594
    LTK_PROFI_OP_COMP_PRM_STRUCT_H_PATH_COL = 6
    LTK_PROFI_IP_BIT_LKUP_FILE_NAME_ROW = 14 '598
    LTK_PROFI_IP_WORD_LKUP_FILE_NAME_ROW = 17 '601
    LTK_PROFI_IO_MODULE_STRUCT_FILE_NAME_ROW = 20 '604
    LTK_PROFI_FILE_NAME_COL = 6
    LTK_PROFI_FILE_NAME_INCLUDE_GUARD_COL = LTK_PROFI_FILE_NAME_COL '+ 1
    PROFI_START_ROW_BCM = 12
    PROFIBUS_IP_BITLOOKUP_C_ROW = 15
    PROFIBUS_IP_BITLOOKUP_H_ROW = 16
    PROFIBUS_IP_WORD_LOOKUP_C_ROW = 18
    PROFIBUS_IP_WORD_LOOKUP_H_ROW = 19
    PROFIBUS_IO_MODULE_STRUCT_C_ROW = 21
    PROFIBUS_IO_MODULE_STRUCT_H_ROW = 22
        PROFIBUS_MODULES_SUPPORTED_ROW = 23
    
    ACYCLIC_ROW_NUMBER = 51

'*********************
'******     Begin
'*********************
    
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select
    

    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\" & Cells(LTK_PROFI_OP_BIT_LKUP_CPP_PATH_ROW, LTK_PROFI_OP_BIT_LKUP_CPP_PATH_COL).value
    Set LTK_PROFI_OP_BIT_LKUP_CPP = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(LTK_PROFI_OP_BIT_LKUP_H_PATH_ROW, LTK_PROFI_OP_BIT_LKUP_H_PATH_COL).value
    Set LTK_PROFI_OP_BIT_LKUP_H = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(LTK_PROFI_OP_WORD_LKUP_CPP_PATH_ROW, LTK_PROFI_OP_WORD_LKUP_CPP_PATH_COL).value
    Set LTK_PROFI_OP_WORD_LKUP_CPP = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(LTK_PROFI_OP_WORD_LKUP_H_PATH_ROW, LTK_PROFI_OP_WORD_LKUP_H_PATH_COL).value
    Set LTK_PROFI_OP_WORD_LKUP_H = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(LTK_PROFI_OP_COMP_PRM_STRUCT_CPP_PATH_ROW, LTK_PROFI_OP_COMP_PRM_STRUCT_CPP_PATH_COL).value
    Set LTK_PROFI_OP_COMP_PRM_STRUCT_CPP = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(LTK_PROFI_OP_COMP_PRM_STRUCT_H_PATH_ROW, LTK_PROFI_OP_COMP_PRM_STRUCT_H_PATH_COL).value
    Set LTK_PROFI_OP_COMP_PRM_STRUCT_H = fs.CreateTextFile(file_path, True)
    
    'Making font type and size consistent.
    Worksheets(sheet_name).Range("A:N").Font.name = "Arial"
    Worksheets(sheet_name).Range("A:N").Font.Size = 10
 


'****************************************************************************************************************************
'******     Start Creating the LTK_PROFI_OP_BIT_LKUP.CPP file
'****************************************************************************************************************************
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("/*")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("*****************************************************************************************")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("*       $Workfile:" & Cells(LTK_PROFI_OP_BIT_LKUP_FILENAME_ROW, LTK_PROFI_OP_BIT_LKUP_FILENAME_COL).text & ".cpp")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("*")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("*       $Author:$ Sumit Benarji")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("*       Copyright© 2013-2014, Eaton Corporation. All Rights Reserved.")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("*")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("*       $Header:$")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("*****************************************************************************************")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("*/")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("#include """ & Cells(LTK_PROFI_OP_BIT_LKUP_FILENAME_ROW, LTK_PROFI_OP_BIT_LKUP_FILENAME_COL).text & ".h""")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("")
 
'****************************************************************************************************************************
'******     Generate the Profibus C445 OP BIT lookup table.
'****************************************************************************************************************************
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("/*")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("*****************************************************************************************")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("*        " & Worksheets("DCI Descriptors").Cells(1, 7).text & " User Parameter Lookup structures")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("*****************************************************************************************")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("*/")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine (" const PROFIBUS_OP_BIT_LKUP_ST_TD " & "ProfiBus" & "_op_bit_lkup[ ProfiBus_TOTAL_OP_BITS_COMP ] =")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("{")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("    {    ")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("       0U,      //Module number")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("       0U,      //Bit number")
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("    },")
    row_counter = PROFI_START_ROW
    
    total_output_bits = 0
    While (Cells(row_counter, MODULE_NUMBER_COL).text <> Empty) And (Cells(row_counter, MODULE_NUMBER_COL).text <> BEGIN_IGNORED_DATA)
        
        If Cells(row_counter, OUTPUT_BIT_COL).text <> Empty Then
            total_output_bits = total_output_bits + Cells(row_counter, OUTPUT_BIT_COL).value
            bit_count = 0
            While (bit_count < Cells(row_counter, OUTPUT_BIT_COL).value)
                            
                    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("    {    ")
                    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("       " & Cells(row_counter, MODULE_NUMBER_COL).value & "U,    //Module number")
                    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("       " & bit_count & "U,      //Bit number")
                    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("    },")
                  bit_count = bit_count + 1
            Wend
            
        End If
        
        row_counter = row_counter + 1
    Wend
    ' Acyclic Starts here
    If Cells(ACYCLIC_ROW_NUMBER, OUTPUT_BIT_COL).text <> Empty Then
        total_output_bits = total_output_bits + Cells(ACYCLIC_ROW_NUMBER, OUTPUT_BIT_COL).value
        bit_count = 0
        While (bit_count < Cells(ACYCLIC_ROW_NUMBER, OUTPUT_BIT_COL).value)
            LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("    {    ")
            LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("       " & Cells(ACYCLIC_ROW_NUMBER, MODULE_NUMBER_COL).value & "U,    //Module number")
            LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("       " & bit_count & "U,      //Bit number")
            LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("    },")
            bit_count = bit_count + 1
        Wend
    End If
    
    LTK_PROFI_OP_BIT_LKUP_CPP.WriteLine ("};")
'****************************************************************************************************************************
'******     End of Profibus C445 OP BIT lookup table .CPP
'****************************************************************************************************************************
'****************************************************************************************************************************
'******     Start Creating the LTK_PROFI_OP_BIT_LKUP.H file header
'****************************************************************************************************************************
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("/*")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("*****************************************************************************************")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("*       $Workfile:" & Cells(LTK_PROFI_OP_BIT_LKUP_FILENAME_ROW, LTK_PROFI_OP_BIT_LKUP_FILENAME_COL).text & ".h")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("*")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("*       $Author:$ Sumit Benarji")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("*       Copyright © Eaton Corporation. All Rights Reserved.")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("*")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("*       Description:Defines the look up structure for the CYCLIC OUTPUT BIT LKUP TABLE ")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("*")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("*")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("*       Code Inspection Date: ")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("*")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("*       $Header:$")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("*****************************************************************************************")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("*/")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("#ifndef " & UCase(Cells(LTK_PROFI_OP_BIT_LKUP_FILENAME_ROW, LTK_PROFI_OP_BIT_LKUP_FILENAME_COL).text) & "_H")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("  #define " & UCase(Cells(LTK_PROFI_OP_BIT_LKUP_FILENAME_ROW, LTK_PROFI_OP_BIT_LKUP_FILENAME_COL).text) & "_H")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("#include ""Includes.h""")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("#include ""ProfiBus_Cfg_List.h""")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("#include ""ProfiBus_Identity.h""")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("#define ProfiBus_TOTAL_OP_BITS_COMP   " & total_output_bits + 1 & "U   ")
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("extern const PROFIBUS_OP_BIT_LKUP_ST_TD ProfiBus_op_bit_lkup[ ProfiBus_TOTAL_OP_BITS_COMP ];")
    
    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("")
        

    LTK_PROFI_OP_BIT_LKUP_H.WriteLine ("#endif")

'****************************************************************************************************************************
'******     End of LTK_PROFI_OP_BIT_LKUP lookup table .H.
'****************************************************************************************************************************
    
    
'****************************************************************************************************************************
'******     Start Creating the LTK_PROFI_OP_WORD_LKUP.CPP file
'****************************************************************************************************************************
     LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("/*")
     LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("*****************************************************************************************")
     LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("*       $Workfile:" & Cells(LTK_PROFI_OP_WORD_LKUP_FILENAME_ROW, LTK_PROFI_OP_WORD_LKUP_FILENAME_COL).text & ".cpp")
     LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("*")
     LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("*       $Author:$ Sumit Benarji")
     LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("*       Copyright© 2013-2014, Eaton Corporation. All Rights Reserved.")
     LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("*")
     LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("*       $Header:$")
     LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("*****************************************************************************************")
     LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("*/")
     LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("#include """ & Cells(LTK_PROFI_OP_WORD_LKUP_FILENAME_ROW, LTK_PROFI_OP_WORD_LKUP_FILENAME_COL).text & ".h""")
     LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("")
     LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("")
     LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("")
'****************************************************************************************************************************
'******     Generate the Profibus C445 OP WORD lookup table .CPP.
'****************************************************************************************************************************
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("/*")
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("*****************************************************************************************")
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("*        " & Worksheets("DCI Descriptors").Cells(1, 7).text & " User Parameter Lookup structures")
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("*****************************************************************************************")
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("*/")
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("")
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("const PROFIBUS_OP_WORD_LKUP_ST_TD " & "ProfiBus" & "_op_word_lkup[ " & "ProfiBus" & "_TOTAL_OP_WORD_COMP ] =")
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("{")
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("    {    ")
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("       0U,      //Module number")
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("       0U,      //Word number")
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("    },")
    row_counter = PROFI_START_ROW
    total_word_count = 0
    While (Cells(row_counter, MODULE_NUMBER_COL).text <> Empty) And (Cells(row_counter, MODULE_NUMBER_COL).text <> BEGIN_IGNORED_DATA)
        
        If (Cells(row_counter, OUTPUT_WORD_COL).text <> Empty) And (Cells(row_counter, OUTPUT_WORD_COL).value <> 0) Then
            
            word_count = 0
            total_word_count = total_word_count + Cells(row_counter, OUTPUT_WORD_COL).value
            While (word_count < Cells(row_counter, OUTPUT_WORD_COL).value)
                            
                    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("    {    ")
                    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("       " & Cells(row_counter, MODULE_NUMBER_COL).value & "U,    //Module number")
                    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("       " & word_count & "U,      //Word number")
                    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("    },")
                  word_count = word_count + 1
            Wend
            
        End If
        
        row_counter = row_counter + 1
    Wend
    
    ' Acyclic Starts here
    If Cells(ACYCLIC_ROW_NUMBER, OUTPUT_WORD_COL).text <> Empty Then
        word_count = 0
        total_word_count = total_word_count + Cells(ACYCLIC_ROW_NUMBER, OUTPUT_WORD_COL).value
        While (word_count < Cells(ACYCLIC_ROW_NUMBER, OUTPUT_WORD_COL).value)
            LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("    {    ")
            LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("       " & Cells(ACYCLIC_ROW_NUMBER, MODULE_NUMBER_COL).value & "U,    //Module number")
            LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("       " & word_count & "U,      //Word number")
            LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("    },")
            word_count = word_count + 1
        Wend
     End If
    
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("};")
    
    
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("const PROFIBUS_OP_DWORD_LKUP_ST_TD " & "ProfiBus" & "_op_dword_lkup[ " & "ProfiBus" & "_TOTAL_OP_DWORD_COMP ] =")
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("{")
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("    {    ")
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("       0U,      //Module number")
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("       0U,      //DWord number")
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("    },")
    row_counter = PROFI_START_ROW
    total_dword_count = 0
    While (Cells(row_counter, MODULE_NUMBER_COL).text <> Empty) And (Cells(row_counter, MODULE_NUMBER_COL).text <> BEGIN_IGNORED_DATA)
        
        If (Cells(row_counter, OUTPUT_DWORD_COL).text <> Empty) And (Cells(row_counter, OUTPUT_DWORD_COL).value <> 0) Then
            
            dword_count = 0
            total_dword_count = total_dword_count + Cells(row_counter, OUTPUT_DWORD_COL).value
            While (dword_count < Cells(row_counter, OUTPUT_DWORD_COL).value)
                            
                    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("    {    ")
                    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("       " & Cells(row_counter, MODULE_NUMBER_COL).value & "U,    //Module number")
                    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("       " & dword_count & "U,      //DWord number")
                    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("    },")
                  dword_count = dword_count + 1
            Wend
            
        End If
        
        row_counter = row_counter + 1
    Wend
    
    LTK_PROFI_OP_WORD_LKUP_CPP.WriteLine ("};")
     
'****************************************************************************************************************************
'******     End of Profibus C445 OP WORD lookup table .CPP
'****************************************************************************************************************************
'****************************************************************************************************************************
'******     Start Creating the LTK_PROFI_OP_WORD_LKUP.H file header
'****************************************************************************************************************************
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("/*")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("*****************************************************************************************")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("*       $Workfile:" & Cells(LTK_PROFI_OP_WORD_LKUP_FILENAME_ROW, LTK_PROFI_OP_WORD_LKUP_FILENAME_COL).text & ".h")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("*")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("*       $Author:$ Sumit Benarji")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("*       Copyright © Eaton Corporation. All Rights Reserved.")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("*")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("*       Description:Defines the look up structure for the CYCLIC OUTPUT BIT LKUP TABLE ")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("*")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("*")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("*       Code Inspection Date: ")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("*")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("*       $Header:$")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("*****************************************************************************************")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("*/")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("#ifndef " & UCase(Cells(LTK_PROFI_OP_WORD_LKUP_FILENAME_ROW, LTK_PROFI_OP_WORD_LKUP_FILENAME_COL).text) & "_H")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("  #define " & UCase(Cells(LTK_PROFI_OP_WORD_LKUP_FILENAME_ROW, LTK_PROFI_OP_WORD_LKUP_FILENAME_COL).text) & "_H")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("#include ""Includes.h""")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("#include ""ProfiBus_Cfg_List.h""")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("#include ""ProfiBus_Identity.h""")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("#define ProfiBus_TOTAL_OP_WORD_COMP " & total_word_count + 1 & "U")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("#define ProfiBus_TOTAL_OP_DWORD_COMP " & total_dword_count + 1 & "U")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("extern const PROFIBUS_OP_WORD_LKUP_ST_TD ProfiBus_op_word_lkup[ ProfiBus_TOTAL_OP_WORD_COMP ];")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("extern const PROFIBUS_OP_DWORD_LKUP_ST_TD ProfiBus_op_dword_lkup[ ProfiBus_TOTAL_OP_DWORD_COMP ];")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("")
    LTK_PROFI_OP_WORD_LKUP_H.WriteLine ("#endif")
    
'****************************************************************************************************************************
'******     End of LTK_PROFI_OP_WORD_LKUP lookup table .H.
'****************************************************************************************************************************
     
'****************************************************************************************************************************
'******     Generate the Profibus LTK_PROFI_OP_COMP_PRM_STRUCT lookup table .CPP.
'****************************************************************************************************************************
'****************************************************************************************************************************
'******     Start Creating the LTK_PROFI_OP_COMP_PRM_STRUCT.Cpp file header
'****************************************************************************************************************************
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("/*")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("*****************************************************************************************")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("*       $Workfile:" & Cells(LTK_PROFI_OP_COMP_PRM_FILENAME_ROW, LTK_PROFI_OP_COMP_PRM_FILENAME_COL).text & ".cpp")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("*")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("*       $Author:$ Sumit Benarji")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("*       Copyright © Eaton Corporation. All Rights Reserved.")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("*")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("*       Description:Defines the look up structure for the OUTPUT COMPONENT PRM STRUCT TABLE ")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("*")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("*")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("*       Code Inspection Date: ")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("*")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("*       $Header:$")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("*****************************************************************************************")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("*/")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("#include """ & Cells(LTK_PROFI_OP_COMP_PRM_FILENAME_ROW, LTK_PROFI_OP_COMP_PRM_FILENAME_COL).text & ".h""")

    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("/*")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("*****************************************************************************************")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("*        " & "ProfiBus" & " User Parameter Lookup structures")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("*****************************************************************************************")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("*/")
    
    
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("PROFIBUS_OP_COMP_ST_TD " & "ProfiBus" & "_OP_COMP_PRM_STRUCT[ " & "ProfiBus" & "_TOTAL_OP_PRM_COMP ] =")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("{")
  
  START_ROW = PROFI_START_ROW_BCM
  row_counter = START_ROW
  DATA_DEFINES_COL = 2
  DATA_EXCHG_DEFINES_COL = 4
  total_output_component = 0
  total_bits = 0
    While (Worksheets("ProfiBus (DCI)").Cells(row_counter, DATA_DEFINES_COL).text <> BEGIN_IGNORED_DATA)
        If (Worksheets("ProfiBus (DCI)").Cells(row_counter, DATA_EXCHG_DEFINES_COL).text = "CYCLIC OUTPUT BIT") Then
                dci_sheet_row = Find_DCID_Row(Worksheets("ProfiBus (DCI)").Cells(row_counter, DATA_DEFINES_COL).text)
                dci_sheet_row_temp = dci_sheet_row
                'number_of_bits = Find_Number_Of_Bits()
                
                DESCRIPTION_COL = 81
                description_content_string = Worksheets("DCI Descriptors").Cells(dci_sheet_row_temp, DESCRIPTION_COL).text
                descriptionString = Split(description_content_string, vbLf)
                Dim count As Integer
                For count = 1 To UBound(descriptionString)
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("    {")
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        //" & descriptionString(count))
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        " & Worksheets("ProfiBus (DCI)").Cells(row_counter, DATA_DEFINES_COL) & "_PROFIBUS_MOD,")
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        BIT,")
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        " & count - 1 & "U,")
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        0U,") ' by default 0 is no module connected
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        0U,") ' by default 0 is for zero requested bit number
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        0U,") ' by default 0 is for zero requested word number
                                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        0U,") ' by default 0 is for zero requested Dword number

                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("    },")
                total_bits = total_bits + 1
                Next count
        End If
        row_counter = row_counter + 1
    Wend
    total_output_component = total_bits
    row_counter = START_ROW
     While (Worksheets("ProfiBus (DCI)").Cells(row_counter, DATA_DEFINES_COL).text <> BEGIN_IGNORED_DATA)
        If ((Worksheets("ProfiBus (DCI)").Cells(row_counter, DATA_EXCHG_DEFINES_COL).text = "CYCLIC OUTPUT WORD") _
        Or (Worksheets("ProfiBus (DCI)").Cells(row_counter, DATA_EXCHG_DEFINES_COL).text = "CYCLIC OUTPUT BIT OUTPUT WORD")) Then
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("    {")
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        //" & Worksheets("ProfiBus (DCI)").Cells(row_counter, 1).text)
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        " & Worksheets("ProfiBus (DCI)").Cells(row_counter, DATA_DEFINES_COL) & "_PROFIBUS_MOD,")
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        WORD,")
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        0U,")
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        0U,") ' by default 0 is no module connected
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        0U,") ' by default 0 is for zero requested bit number
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        0U,") ' by default 0 is for zero requested word number
                                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        0U,") ' by default 0 is for zero requested Dword number
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("    },")
                total_output_component = total_output_component + 1
        End If
    row_counter = row_counter + 1
    Wend
    row_counter = START_ROW
     While (Worksheets("ProfiBus (DCI)").Cells(row_counter, DATA_DEFINES_COL).text <> BEGIN_IGNORED_DATA)
        If ((Worksheets("ProfiBus (DCI)").Cells(row_counter, DATA_EXCHG_DEFINES_COL).text = "CYCLIC OUTPUT DWORD") _
        Or (Worksheets("ProfiBus (DCI)").Cells(row_counter, DATA_EXCHG_DEFINES_COL).text = "CYCLIC OUTPUT BIT OUTPUT DWORD")) Then
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("    {")
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        //" & Worksheets("ProfiBus (DCI)").Cells(row_counter, 1).text)
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        " & Worksheets("ProfiBus (DCI)").Cells(row_counter, DATA_DEFINES_COL) & "_PROFIBUS_MOD,")
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        DWORD,")
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        0U,")
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        0U,") ' by default 0 is no module connected
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        0U,") ' by default 0 is for zero requested bit number
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        0U,") ' by default 0 is for zero requested word number
                                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        0U,") ' by default 0 is for zero requested Dword number
                LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("    },")
                total_output_component = total_output_component + 1
        End If
    row_counter = row_counter + 1
    Wend
    '****************************************************************************************************************************
    '******     Start Creating the LTK_PROFI_OP_COMP_PRM_STRUCT.H file header
    '****************************************************************************************************************************
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("/*")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("*****************************************************************************************")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("*       $Workfile:" & Cells(LTK_PROFI_OP_COMP_PRM_FILENAME_ROW, LTK_PROFI_OP_COMP_PRM_FILENAME_COL).text & ".h")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("*")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("*       $Author:$ Sumit Benarji")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("*       Copyright © Eaton Corporation. All Rights Reserved.")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("*")
    
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("*       Description:Defines the look up structure for the OUTPUT COMPONENT PRM STRUCT TABLE ")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("*")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("*")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("*       Code Inspection Date: ")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("*")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("*       $Header:$")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("*****************************************************************************************")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("*/")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("#ifndef " & UCase(Cells(LTK_PROFI_OP_COMP_PRM_FILENAME_ROW, LTK_PROFI_OP_COMP_PRM_FILENAME_COL).text) & "_H")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("  #define " & UCase(Cells(LTK_PROFI_OP_COMP_PRM_FILENAME_ROW, LTK_PROFI_OP_COMP_PRM_FILENAME_COL).text) & "_H")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("#include ""Includes.h""")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("#include ""ProfiBus_Cfg_List.h""")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("#include ""ProfiBus_Identity.h""")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("#define ProfiBus_TOTAL_OP_PRM_COMP      " & total_output_component & "U")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("extern PROFIBUS_OP_COMP_ST_TD ProfiBus_OP_COMP_PRM_STRUCT[ ProfiBus_TOTAL_OP_PRM_COMP ];")
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.WriteLine ("#endif")
    
'****************************************************************************************************************************
'******     End of  LTK_PROFI_OP_COMP_PRM_STRUCT lookup table .H.
'****************************************************************************************************************************

    
    
    
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("};")
    LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.Close
    LTK_PROFI_OP_COMP_PRM_STRUCT_H.Close
    LTK_PROFI_OP_WORD_LKUP_H.Close
    LTK_PROFI_OP_WORD_LKUP_CPP.Close
    LTK_PROFI_OP_BIT_LKUP_CPP.Close
    LTK_PROFI_OP_BIT_LKUP_H.Close
'****************************************************************************************************************************
'******     End of Profibus C445 OP WORD lookup table .CPP
'****************************************************************************************************************************


' Sumit's changes

'*********************
'******     Constants
'*********************
    
    'Dim dci_sheet_row As Integer
    'Dim BEGIN_IGNORED_DATA As String
    'BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
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
    OUTPUT_WORD = "CYCLIC CYCLIC OUTPUT WORD"
    Dim OUTPUT_DWORD As String
    OUTPUT_DWORD = "CYCLIC OUTPUT DWORD"
    Dim INPUT_BIT_INPUT_WORD As String
    INPUT_BIT_INPUT_WORD = "CYCLIC INPUT BIT INPUT WORD"
    Dim INPUT_BIT_INPUT_DWORD As String
    INPUT_BIT_INPUT_DWORD = "CYCLIC INPUT BIT INPUT DWORD"
    Dim OUTPUT_BIT_OUTPUT_WORD As String
    OUTPUT_BIT_OUTPUT_WORD = "CYCLIC OUTPUT BIT OUTPUT WORD"
    Dim OUTPUT_BIT_OUTPUT_DWORD As String
    OUTPUT_BIT_OUTPUT_DWORD = "CYCLIC OUTPUT BIT OUTPUT DWORD"
    'Dim descriptionString() As String
    'Dim MyDate
    'MyDate = Date    ' MyDate contains the current system date.
    
    'Dim DCI_STRING As String
    'Dim num_unique_data As Integer
'    Dim even_reg_count As Boolean
'    Dim even_unique_reg_count As Boolean
    
    
    DATA_TYPE_ACCESS_COL = 4
    DATA_DEFINE_COL = 2
    MODULE_NUMBER_COL = 2
    DESCRIPTION_COL = 81
    SHORT_DESCRIPTION_COL = 79
    MODULE_BEGIN_ROW = 26
    MODULE_BEGIN_COL = 5
    IP_BIT_DATA_COL = 3
    IP_BYTE_DATA_COL = 9
    IP_WORD_DATA_COL = 4
    IP_DWORD_DATA_COL = 5
    OP_BIT_DATA_COL = 6
    OP_BYTE_DATA_COL = 11
    OP_WORD_DATA_COL = 7
    OP_DWORD_DATA_COL = 8
    MODULE_INPUT_SIZE_COL = 9
    MODULE_OUTPUT_SIZE_COL = 10
    MODULE_INPUT_MAPPING_SIZE_COL = 11
    MODULE_OUTPUT_MAPPING_SIZE_COL = 12
    INPUT_BITS_COL = 3
    INPUT_WORDS_COL = 4
    INPUT_DWORDS_COL = 5
    OUTPUT_BITS_COL = 6
    OUTPUT_WORDS_COL = 7
    OUTPUT_DWORDS_COL = 8
    INPUT_DATA_EXCHANGE_BYTES = 9
    OUTPUT_DATA_EXCHANGE_BYTES = 10
    ' Change start
    CONFIG_FILE_NAME_COL = 9
    CONFIG_FILE_INCLUDE_GUARD_COL = CONFIG_FILE_NAME_COL '+ 1
    DEFAULT_CONFIG_FILE_NAME_ROW = 3
    DEFAULT_CONFIG_H_FILE_NAME_ROW = DEFAULT_CONFIG_FILE_NAME_ROW + 1
    DEFAULT_CONFIG_CPP_FILE_NAME_ROW = DEFAULT_CONFIG_H_FILE_NAME_ROW + 1
    MODULE_CONFIG_FILE_NAME_ROW = 6
    MODULE_CONFIG_H_FILE_NAME_ROW = MODULE_CONFIG_FILE_NAME_ROW + 1
    MODULE_CONFIG_CPP_FILE_NAME_ROW = MODULE_CONFIG_H_FILE_NAME_ROW + 1
    LTK_PROFI_TAG_ROW = 3
    LTK_PROFI_TAG_COL = 11
    ' Config Module Constants
    DATA_CONFIG_MODULE_START_ROW = 26
    DATA_CONFIG_MODULE_START_COL = 2
    DATA_CONFIG_MODULE_INPUT_SIZE_COL = 9
    DATA_CONFIG_MODULE_OUTPUT_SIZE_COL = 10
    DATA_CONFIG_MODULE_MAPPING_BYTES_COL = 11
    DATA_CONFIG_MODULE_INPUT_BIT_SIZE_COL = 3
    DATA_CONFIG_MODULE_INPUT_WORD_SIZE_COL = 4
    DATA_CONFIG_MODULE_OUTPUT_BIT_SIZE_COL = 5
    DATA_CONFIG_MODULE_OUTPUT_WORD_SIZE_COL = 6
    DATA_DEFAULT_CONFIG_MODUES_SUPPORTED = Cells(PROFIBUS_MODULES_SUPPORTED_ROW, MODULE_NUMBER_COL).value
    ' This definition extracts Module No.(1to16) from the Column number
    DATA_CONFIG_MODULE_MODULE_NO_OFFSET = DATA_CONFIG_MODULE_START_ROW - 1
    Dim END_OF_TABLE As String
    END_OF_TABLE = "BEGIN_IGNORED_DATA"
    
    
'*********************
'******     Begin
'*********************
    
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select
    
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\" & Cells(PROFIBUS_IP_BITLOOKUP_C_ROW, LTK_PROFI_FILE_NAME_COL).value
    Set PROFIBUS_IP_BITLOOKUP_C = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(PROFIBUS_IP_BITLOOKUP_H_ROW, LTK_PROFI_FILE_NAME_COL).value
    Set PROFIBUS_IP_BITLOOKUP_H = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(PROFIBUS_IP_WORD_LOOKUP_C_ROW, LTK_PROFI_FILE_NAME_COL).value
    Set PROFIBUS_IP_WORD_LOOKUP_C = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(PROFIBUS_IP_WORD_LOOKUP_H_ROW, LTK_PROFI_FILE_NAME_COL).value
    Set PROFIBUS_IP_WORD_LOOKUP_H = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(PROFIBUS_IO_MODULE_STRUCT_C_ROW, LTK_PROFI_FILE_NAME_COL).value
    Set PROFIBUS_IO_MODULE_STRUCT_C = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(PROFIBUS_IO_MODULE_STRUCT_H_ROW, LTK_PROFI_FILE_NAME_COL).value
    Set PROFIBUS_IO_MODULE_STRUCT_H = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(DEFAULT_CONFIG_H_FILE_NAME_ROW, CONFIG_FILE_NAME_COL).value
    Set PROFIBUS_DEFAULT_CFG_H = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(DEFAULT_CONFIG_CPP_FILE_NAME_ROW, CONFIG_FILE_NAME_COL).value
    Set PROFIBUS_DEFAULT_CFG_C = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(MODULE_CONFIG_H_FILE_NAME_ROW, CONFIG_FILE_NAME_COL).value
    Set PROFIBUS_CFG_MODULE_H = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(MODULE_CONFIG_CPP_FILE_NAME_ROW, CONFIG_FILE_NAME_COL).value
    Set PROFIBUS_CFG_MODULE_C = fs.CreateTextFile(file_path, True)
    
    
'****************************************************************************************************************************
'******     Start Creating the IP Bit look up Cpp file
'****************************************************************************************************************************
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("/*")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("*       $Workfile:" & Cells(LTK_PROFI_IP_BIT_LKUP_FILE_NAME_ROW, LTK_PROFI_FILE_NAME_COL).text & ".cpp")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("*")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("*       $Author:$ Sumit Benarji")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("*       Copyright© 2001-2004, Eaton Corporation. All Rights Reserved.")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("*")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("*       $Header:$")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("*/")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("#include ""Includes.h""")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("#include ""ProfiBus_Identity.h""")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("#include """ & Cells(LTK_PROFI_IP_BIT_LKUP_FILE_NAME_ROW, LTK_PROFI_FILE_NAME_COL).text & ".h""")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("const PROFIBUS_IP_BIT_LKUP_ST_TD  " & Cells(LTK_PROFI_IP_BIT_LKUP_FILE_NAME_ROW, LTK_PROFI_FILE_NAME_COL).text & "[ProfiBus_TOTAL_IP_BIT_COMP] = ")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("{")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("    {")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("        //IP_BIT_NOT_CONNECTED ")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("        DCI_IP_BIT_NOT_CONNECTED_PROFIBUS_MOD,")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("        0U")
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("    },")
    ' Logic Starts from here to fill Bit look up table
    row_counter = PROFI_START_ROW_BCM
    Sheets("ProfiBus (DCI)").Select
    'Dim total_bits As Integer
    total_bits = 0
   While (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If ((Cells(row_counter, DATA_TYPE_ACCESS_COL).text = INPUT_BIT) _
        Or (Cells(row_counter, DATA_TYPE_ACCESS_COL).text = INPUT_BIT_INPUT_WORD) _
        Or (Cells(row_counter, DATA_TYPE_ACCESS_COL).text = INPUT_BIT_INPUT_DWORD)) Then
            dci_sheet_row = Find_DCID_Row(Cells(row_counter, DATA_DEFINE_COL))
            description_content_string = Worksheets("DCI Descriptors").Cells(dci_sheet_row, DESCRIPTION_COL).text
            descriptionString = Split(description_content_string, vbLf)
            'Dim Count As Integer
            For count = 1 To UBound(descriptionString)
            PROFIBUS_IP_BITLOOKUP_C.WriteLine ("    {")
            PROFIBUS_IP_BITLOOKUP_C.WriteLine ("        //" & descriptionString(count))
            PROFIBUS_IP_BITLOOKUP_C.WriteLine ("        " & Cells(row_counter, DATA_DEFINE_COL) & "_PROFIBUS_MOD,")
            PROFIBUS_IP_BITLOOKUP_C.WriteLine ("        " & count - 1 & "U")
            PROFIBUS_IP_BITLOOKUP_C.WriteLine ("    },")
            total_bits = total_bits + 1
            Next count
        End If
    row_counter = row_counter + 1
    Wend
    PROFIBUS_IP_BITLOOKUP_C.WriteLine ("};")
    '****************************************************************************************************************************
    '******     Start Creating the IP Bit look up Header file
    '****************************************************************************************************************************
    Sheets("ProfiBus(IO Modules)").Select
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("/*")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("*****************************************************************************************")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("*       $Workfile:" & Cells(LTK_PROFI_IP_BIT_LKUP_FILE_NAME_ROW, LTK_PROFI_FILE_NAME_COL).text & ".h")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("*")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("*       $Author:$ Sumit Benarji")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("*       Copyright© 2001-2004, Eaton Corporation. All Rights Reserved.")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("*")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("*       $Header:$")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("*****************************************************************************************")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("*/")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("#ifndef " & UCase(Cells(LTK_PROFI_IP_BIT_LKUP_FILE_NAME_ROW, LTK_PROFI_FILE_NAME_INCLUDE_GUARD_COL).text) & "_H")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("  #define " & UCase(Cells(LTK_PROFI_IP_BIT_LKUP_FILE_NAME_ROW, LTK_PROFI_FILE_NAME_INCLUDE_GUARD_COL).text) & "_H")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("")
    ' 1 array element is reseved for not connected so add 1
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("#define ProfiBus_TOTAL_IP_BIT_COMP " & total_bits + 1 & "U")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("typedef struct PROFIBUS_IP_BIT_LKUP_ST_TD")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("{")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("    uint16_t    req_profibus_id;")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("    uint8_t     req_bit_num;")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("}PROFIBUS_IP_BIT_LKUP_ST_TD;")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("extern const PROFIBUS_IP_BIT_LKUP_ST_TD ProfiBus_ip_bit_lkup[ ProfiBus_TOTAL_IP_BIT_COMP ] ;")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("")
    PROFIBUS_IP_BITLOOKUP_H.WriteLine ("#endif")
   
    '****************************************************************************************************************************
    '******     Start Creating the IP Word look up Cpp file
    '****************************************************************************************************************************
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("/*")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("*        @file:" & Cells(LTK_PROFI_IP_WORD_LKUP_FILE_NAME_ROW, LTK_PROFI_FILE_NAME_COL).text & ".cpp")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("*")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("*        @details See header file for module overview.")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("*        @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("*")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("*/")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("#include ""Includes.h""")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("#include ""ProfiBus_Identity.h""")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("#include """ & Cells(LTK_PROFI_IP_WORD_LKUP_FILE_NAME_ROW, LTK_PROFI_FILE_NAME_COL).text & ".h""")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("const uint16_t  " & Cells(LTK_PROFI_IP_WORD_LKUP_FILE_NAME_ROW, LTK_PROFI_FILE_NAME_COL).text & "[LTK_PROFI_TOTAL_IP_WORD_COMP] = ")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("{")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("        //NOT_CONNECTED ")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("        DCI_IP_WORD_NOT_CONNECTED_PROFIBUS_MOD,")
    ' Logic Starts from here to fill Bit look up table
    row_counter = PROFI_START_ROW_BCM
    Sheets("ProfiBus (DCI)").Select
    Dim total_words As Integer
    total_words = 0
    While (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If ((Cells(row_counter, DATA_TYPE_ACCESS_COL).text = INPUT__WORD) _
        Or (Cells(row_counter, DATA_TYPE_ACCESS_COL).text = INPUT_BIT_INPUT_WORD)) Then
            dci_sheet_row = Find_DCID_Row(Cells(row_counter, DATA_DEFINE_COL))
            description_content_string = Worksheets("DCI Descriptors").Cells(dci_sheet_row, SHORT_DESCRIPTION_COL).text
            PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("        //" & description_content_string)
            PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("        " & Cells(row_counter, DATA_DEFINE_COL) & "_PROFIBUS_MOD,")
            total_words = total_words + 1
        End If
    row_counter = row_counter + 1
    Wend
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("};")
    
    
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("const uint16_t  " & "ProfiBus" & "_ip_dword_lkup[ LTK_PROFI_TOTAL_IP_DWORD_COMP ] = ")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("{")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("        //NOT_CONNECTED ")
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("        DCI_IP_DWORD_NOT_CONNECTED_PROFIBUS_MOD,")
    ' Logic Starts from here to fill Bit look up table
    row_counter = PROFI_START_ROW_BCM
    Sheets("ProfiBus (DCI)").Select
    Dim total_dwords As Integer
    total_dwords = 0
    While (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If ((Cells(row_counter, DATA_TYPE_ACCESS_COL).text = INPUT_DWORD) _
        Or (Cells(row_counter, DATA_TYPE_ACCESS_COL).text = INPUT_BIT_INPUT_DWORD)) Then
            dci_sheet_row = Find_DCID_Row(Cells(row_counter, DATA_DEFINE_COL))
            description_content_string = Worksheets("DCI Descriptors").Cells(dci_sheet_row, SHORT_DESCRIPTION_COL).text
            PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("        //" & description_content_string)
            PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("        " & Cells(row_counter, DATA_DEFINE_COL) & "_PROFIBUS_MOD,")
            total_dwords = total_dwords + 1
        End If
    row_counter = row_counter + 1
    Wend
        
    PROFIBUS_IP_WORD_LOOKUP_C.WriteLine ("};")
    '****************************************************************************************************************************
    '******     Start Creating the IP Word look up Header file
    '****************************************************************************************************************************
    Sheets("ProfiBus(IO Modules)").Select
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("/*")
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("*****************************************************************************************")
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("*       $Workfile:" & Cells(LTK_PROFI_IP_WORD_LKUP_FILE_NAME_ROW, LTK_PROFI_FILE_NAME_COL).text & ".h")
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("*")
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("*       $Author:$ Sumit Benarji")
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("*       Copyright© 2001-2004, Eaton Corporation. All Rights Reserved.")
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("*")
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("*       $Header:$")
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("*****************************************************************************************")
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("*/")
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("#ifndef " & UCase(Cells(LTK_PROFI_IP_WORD_LKUP_FILE_NAME_ROW, LTK_PROFI_FILE_NAME_INCLUDE_GUARD_COL).text) & "_H")
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("  #define " & UCase(Cells(LTK_PROFI_IP_WORD_LKUP_FILE_NAME_ROW, LTK_PROFI_FILE_NAME_INCLUDE_GUARD_COL).text) & "_H")
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("")
    ' 1 array element is reseved for not connected so add 1
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("#define LTK_PROFI_TOTAL_IP_WORD_COMP " & total_words + 1 & "U")
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("")
    ' 1 array element is reseved for not connected so add 1
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("#define LTK_PROFI_TOTAL_IP_DWORD_COMP " & total_dwords + 1 & "U")
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("")
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("extern const uint16_t " & Cells(LTK_PROFI_IP_WORD_LKUP_FILE_NAME_ROW, LTK_PROFI_FILE_NAME_COL).text & "[ LTK_PROFI_TOTAL_IP_WORD_COMP ] ;")
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("")
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("extern const uint16_t  " & "ProfiBus" & "_ip_dword_lkup[ LTK_PROFI_TOTAL_IP_DWORD_COMP ] ;")
    'PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("extern const uint16_t " & Cells(LTK_PROFI_IP_WORD_LKUP_FILE_NAME_ROW, LTK_PROFI_FILE_NAME_COL).text & "_dword" & "[ LTK_PROFI_TOTAL_IP_DWORD_COMP ] ;")
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("")
    PROFIBUS_IP_WORD_LOOKUP_H.WriteLine ("#endif")
    '****************************************************************************************************************************
    '******     Start Creating IO Module struct Cpp
    '****************************************************************************************************************************
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("/*")
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("*       $Workfile:" & Cells(LTK_PROFI_IO_MODULE_STRUCT_FILE_NAME_ROW, LTK_PROFI_FILE_NAME_COL).text & ".cpp")
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("*")
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("*       $Author:$ Sumit Benarji")
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("*       Copyright© 2001-2004, Eaton Corporation. All Rights Reserved.")
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("*")
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("*       $Header:$")
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("*/")
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("#include """ & Cells(LTK_PROFI_IO_MODULE_STRUCT_FILE_NAME_ROW, LTK_PROFI_FILE_NAME_COL).text & ".h""")
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("#include ""DCI_Profibus_Data.h""")
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("#include ""ProfiBus_DCI.h""")
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("#include ""ProfiBus_Cfg_List.h""")
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("")
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("")
    '''Acyclic change starts here
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("#define ACYCLIC_MODULE_INPUT_MAPPING_BYTES   " & Cells(ACYCLIC_ROW_NUMBER, MODULE_INPUT_MAPPING_SIZE_COL).value & "U")
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("INP_COMP_ST_TD LTK_PROFI_IP_COMP_list_Acyclic_Module[ACYCLIC_MODULE_INPUT_MAPPING_BYTES] = ")
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("{")
    ''' TODO : This will work with only one module
    ''For Count = 1 To Cells(ACYCLIC_ROW_NUMBER, IP_BIT_DATA_COL).value
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("    {")
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        BIT,")
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        0U,")
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        0U")
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("    },")
    ''Next Count
    ''For Count = 1 To Cells(ACYCLIC_ROW_NUMBER, IP_WORD_DATA_COL).value
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("    {")
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        WORD,")
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        0U,")
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        0U")
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("    },")
    ''Next Count
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("};")
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("")
    
    counter = DATA_CONFIG_MODULE_START_ROW
    Module_Number = 0
    While (Cells(counter, DATA_CONFIG_MODULE_START_COL).text <> END_OF_TABLE)
        Module_Number = Module_Number + 1
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("#define MODULE" & Module_Number & "_INPUT_MAPPING_BYTES   " & Cells(counter, MODULE_INPUT_MAPPING_SIZE_COL).value & "U")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("#define MODULE" & Module_Number & "_OUTPUT_MAPPING_BYTES   " & Cells(counter, MODULE_OUTPUT_MAPPING_SIZE_COL).value & "U")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("")
        counter = counter + 1
    Wend
    counter = DATA_CONFIG_MODULE_START_ROW
    Module_Number = 0
    While (Cells(counter, DATA_CONFIG_MODULE_START_COL).text <> END_OF_TABLE)
        Module_Number = Module_Number + 1
        If ((Cells(counter, IP_BIT_DATA_COL).value <> 0) Or (Cells(counter, IP_WORD_DATA_COL).value <> 0) Or (Cells(counter, IP_DWORD_DATA_COL).value <> 0)) Then
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("INP_COMP_ST_TD LTK_PROFI_IP_COMP_list_Module_" & Module_Number & "[MODULE" & Module_Number & "_INPUT_MAPPING_BYTES] = ")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("{")
        ' TODO : This will work with only one module
        For count = 1 To Cells(counter, IP_BIT_DATA_COL).value
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("    {")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        BIT,")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        0U,")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        0U")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("    },")
        Next count
        For count = 1 To Cells(counter, IP_WORD_DATA_COL).value
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("    {")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        WORD,")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        0U,")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        0U")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("    },")
        Next count
        For count = 1 To Cells(counter, IP_DWORD_DATA_COL).value
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("    {")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        DWORD,")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        0U,")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        0U")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("    },")
        Next count
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("};")
        Else
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("INP_COMP_ST_TD LTK_PROFI_IP_COMP_list_Module_" & Module_Number & "[] = ")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("{")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("    0U")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("};")
        End If
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("")
        counter = counter + 1
    Wend
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("")
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("const Module_Map Map_For_All_Modules[] =")
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("{")
    counter = DATA_CONFIG_MODULE_START_ROW
    While (Cells(counter, DATA_CONFIG_MODULE_START_COL).text <> END_OF_TABLE)
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("    {")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        " & Cells(counter, MODULE_INPUT_MAPPING_SIZE_COL).value & "U,")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        " & Cells(counter, MODULE_OUTPUT_MAPPING_SIZE_COL).value & "U,")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        " & Cells(counter, INPUT_BITS_COL).value & "U,")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        " & Cells(counter, INPUT_WORDS_COL).value & "U,")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        " & Cells(counter, INPUT_DWORDS_COL).value & "U,")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        " & Cells(counter, OUTPUT_BITS_COL).value & "U,")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        " & Cells(counter, OUTPUT_WORDS_COL).value & "U,")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        " & Cells(counter, OUTPUT_DWORDS_COL).value & "U,")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        " & Cells(counter, INPUT_DATA_EXCHANGE_BYTES).value & "U,")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        " & Cells(counter, OUTPUT_DATA_EXCHANGE_BYTES).value & "U ")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("    },")
        counter = counter + 1
    Wend
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("};")
    
    '''Acyclic Map
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("const Acyclic_Data_Map LTK_PROFI_Acyclic_Data_Map =")
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("{")
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        " & Cells(ACYCLIC_ROW_NUMBER, INPUT_BITS_COL).value & "U,")
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        " & Cells(ACYCLIC_ROW_NUMBER, INPUT_WORDS_COL).value & "U,")
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        " & Cells(ACYCLIC_ROW_NUMBER, OUTPUT_BITS_COL).value & "U,")
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        " & Cells(ACYCLIC_ROW_NUMBER, OUTPUT_WORDS_COL).value & "U,")
    ''PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("};")
    
    counter = DATA_CONFIG_MODULE_START_ROW
    Module_Number = 0
    While (Cells(counter, DATA_CONFIG_MODULE_START_COL).text <> END_OF_TABLE)
        Module_Number = Module_Number + 1
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("INP_COMP_ST_TD*  Module" & Module_Number & "_IP_COMP_LIST_PTR = LTK_PROFI_IP_COMP_list_Module_" & Module_Number & ";")
        counter = counter + 1
    Wend
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("const MODULE_ST_TD Module[ProfiBus_MAX_CFG_MODS] =")
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("{")
    counter = DATA_CONFIG_MODULE_START_ROW
    Module_Number = 0
    While (Cells(counter, DATA_CONFIG_MODULE_START_COL).text <> END_OF_TABLE)
        Module_Number = Module_Number + 1
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("    {")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        MODULE" & Module_Number & "_INPUT_MAPPING_BYTES,")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        MODULE" & Module_Number & "_OUTPUT_MAPPING_BYTES,")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("        Module" & Module_Number & "_IP_COMP_LIST_PTR,")
        PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("    },")
        counter = counter + 1
     Wend
    PROFIBUS_IO_MODULE_STRUCT_C.WriteLine ("};")
    '****************************************************************************************************************************
    '******     Start Creating IO Module struct header
    '****************************************************************************************************************************
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("/*")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("*****************************************************************************************")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("*       $Workfile:" & Cells(LTK_PROFI_IO_MODULE_STRUCT_FILE_NAME_ROW, LTK_PROFI_FILE_NAME_COL).text & ".h")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("*")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("*       $Author:$ Sumit Benarji")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("*       Copyright© 2001-2004, Eaton Corporation. All Rights Reserved.")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("*")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("*       $Header:$")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("*****************************************************************************************")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("*/")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("#ifndef " & UCase(Cells(LTK_PROFI_IO_MODULE_STRUCT_FILE_NAME_ROW, LTK_PROFI_FILE_NAME_INCLUDE_GUARD_COL).text) & "_H")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("  #define " & UCase(Cells(LTK_PROFI_IO_MODULE_STRUCT_FILE_NAME_ROW, LTK_PROFI_FILE_NAME_INCLUDE_GUARD_COL).text) & "_H")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("#include ""Includes.h""")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("typedef struct")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("{")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint8_t ModSize;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint32_t ProfiId;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint8_t BitAddr;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("}INP_COMP_ST_TD;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("struct Bit_Hanlder {")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint32_t b0     :1;  // bit 0 single bit")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint32_t b1     :1;  // bit 1 single bit")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint32_t b2     :1;  // bit 2 single bit")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint32_t b3     :1;  // bit 3 single bit")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint32_t b4     :1;  // bit 4 single bit")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint32_t b5     :1;  // bit 5 single bit")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint32_t b6     :1;  // bit 6 single bit")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint32_t b7     :1;  // bit 7 single bit")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint32_t b8     :1;  // bit 8 single bit")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint32_t b9     :1;  // bit 9 single bit")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint32_t b10     :1;  // bit 10 single bit")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint32_t b11     :1;  // bit 11 single bit")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint32_t b12     :1;  // bit 12 single bit")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint32_t b13     :1;  // bit 13 single bit")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint32_t b14     :1;  // bit 14 single bit")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint32_t b15     :1;  // bit 15 single bit")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("};")
    
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("typedef struct")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("{")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint8_t NoOfBytesForInputMapping;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint8_t NoOfBytesForOutputMapping;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint8_t NoOfInputBitsInModule;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint8_t NoOfInputWordInModule;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint8_t NoOfInputDWordInModule;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint8_t NoOfOutputBitsInModule;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint8_t NoOfOutputWordInModule;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint8_t NoOfOutputDWordInModule;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint8_t NoOfInputByte_DataExchange;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint8_t NoOfOutputByte_DataExchange;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("}Module_Map;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("")
    
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("typedef struct")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("{")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint8_t NoOfInputBitsInAcyclicRead;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint8_t NoOfInputWordInAcyclicRead;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint8_t NoOfOutputBitsInAcyclicWrite;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("    uint8_t NoOfOutputWordInAcyclicWrite;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("}Acyclic_Data_Map;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("")
    
    
    
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("typedef struct")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("{")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("  uint8_t Total_Input_Components;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("  uint8_t Total_Output_Components;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("  INP_COMP_ST_TD *  LTK_PROFI_INP_COMP_LIST_PTR;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("}MODULE_ST_TD;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("extern const MODULE_ST_TD Module[];")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("extern const Module_Map Map_For_All_Modules[];")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("extern const Acyclic_Data_Map LTK_PROFI_Acyclic_Data_Map;")
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("extern INP_COMP_ST_TD LTK_PROFI_IP_COMP_list_Acyclic_Module[];")

    
    PROFIBUS_IO_MODULE_STRUCT_H.WriteLine ("#endif")
    
    ' Config file changs begin:
    '****************************************************************************************************************************
    '******     Start Creating the Default_CFG.H file header
    '****************************************************************************************************************************
    
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("/*")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("*****************************************************************************************")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("*       @file:" & Cells(DEFAULT_CONFIG_FILE_NAME_ROW, CONFIG_FILE_NAME_COL).text & ".h")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("*")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("*       @brief: ")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("*")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("*       @details:Defines default configuration loaded into slave at power-up")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("*                   This is Automation Macro generated file,")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("*                   Please avoid manual editing.")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("*       @version                                 ")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("*       C++ Style Guide Version 1.0")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("*")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("*       @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("*")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("*****************************************************************************************")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("*/")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("#ifndef " & UCase(Cells(DEFAULT_CONFIG_FILE_NAME_ROW, CONFIG_FILE_INCLUDE_GUARD_COL).text) & "_H")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("  #define " & UCase(Cells(DEFAULT_CONFIG_FILE_NAME_ROW, CONFIG_FILE_INCLUDE_GUARD_COL).text) & "_H")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("#include ""DCI.h""")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("#include ""DCI_Defines.h""")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("#include ""DCI_Profibus_Data.h""")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("#include ""ProfiBus_Configuration.h""")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("")
    
'***************************************************************************************************************************
'******      Count number of default parameters to determine how large the default cfg array should be
'***************************************************************************************************************************
    cfg_array_size = 0
    default_modules = 0
    default_io_modules = 0
    counter = DATA_CONFIG_MODULE_START_ROW
    default_modules_supported = 0
    While (Cells(counter, DATA_CONFIG_MODULE_START_COL).text <> END_OF_TABLE And default_modules_supported <> DATA_DEFAULT_CONFIG_MODUES_SUPPORTED)
        ' E9898897,Sumit: Why this logic ?If (Cells(counter, DATA_CFG_DEFAULT_COL).Text <> Empty) Then
                If ((Cells(counter, DATA_CONFIG_MODULE_INPUT_SIZE_COL).text <> Empty) And (Cells(counter, DATA_CONFIG_MODULE_OUTPUT_SIZE_COL).text <> Empty)) Then
                    default_io_modules = default_io_modules + 1
          '      Else
           '         If ((Cells(counter, DATA_CONFIG_MODULE_INPUT_SIZE_COL).text <> Empty) Or (Cells(counter, DATA_CONFIG_MODULE_OUTPUT_SIZE_COL).text <> Empty)) Then
              '          default_modules = default_modules + 1
             '       End If
                End If
        ' End If
        counter = counter + 1
        default_modules_supported = default_modules_supported + 1
    Wend
    'SSN: There is no need of base module which is of constant size 3
    'cfg_array_size = ((default_io_modules * 5) + (default_modules * 4) + 3)    ' IO modules cfg_len = 4, base module cfg_len = 3, (Default IO + Base)
    cfg_array_size = ((default_io_modules * 5) + (default_modules * 4))    ' IO modules cfg_len = 4, base module cfg_len = 3, (Default IO + Base)
'***************************************************************************************************************************
'******      Build default cfg array structure
'***************************************************************************************************************************
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("//*******************************************************")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("//******     " & "ProfiBus" & " Profibus Cfg Default Data Structure ")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("//*******************************************************")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("//#Pre-Conformance Change:default module only two")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("#define      " & "ProfiBus" & "_DEFAULT_CFG_SIZE    " & cfg_array_size & "U")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("extern const uint8_t " & "ProfiBus" & "_def_cfg_data[ " & "ProfiBus" & "_DEFAULT_CFG_SIZE ];")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("extern const Profibus_Cfg::DEF_CFG_STRUCT " & "ProfiBus" & "_DEF_CFG_STRUCT;")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("")
    PROFIBUS_DEFAULT_CFG_H.WriteLine ("#endif")

'****************************************************************************************************************************
'******     Start Creating the Default CFG.C file header
'****************************************************************************************************************************
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("/*")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("*       @file: " & Cells(DEFAULT_CONFIG_FILE_NAME_ROW, CONFIG_FILE_NAME_COL).text & ".cpp")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("*")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("*       @details See header file for module overview.")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("*       @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("*")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("*/")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("#include ""Includes.h""")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("#include """ & Cells(DEFAULT_CONFIG_FILE_NAME_ROW, CONFIG_FILE_NAME_COL).text & ".h""")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("")


'****************************************************************************************************************************
'******     Generate the Default configuration structure.
'****************************************************************************************************************************
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("/*")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("*        Default Cfg for " & "ProfiBus" & " Profibus Adapter")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("*/")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("//#Pre-Conformance Change:default module only two")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("const uint8_t " & "ProfiBus" & "_def_cfg_data[ " & "ProfiBus" & "_DEFAULT_CFG_SIZE ] = ")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("{")
    
    'SSN: No Need of C445 Base Module configuration.
    'Load Base module cfg_data into def_cfg
    'PROFIBUS_DEFAULT_CFG_C.Writeline ("    0x02, " & "0x" & Cells(8, DATA_CFG_MSB_ID_COL).Text & ", 0x" & Cells(8, DATA_CFG_LSB_ID_COL).Text & _
    '                        ",      //" & Cells(1, 7).Text & " Base Module")
    
    counter = DATA_CONFIG_MODULE_START_ROW
    module_no_msb = 0
    module_no_lsb = 0
    length_Word = 0
    'only two modules are supported,we are assuming it to be IO module,not the input or output but both input/output
    default_modules_supported = 0
    'Check for default output modules
    While (Cells(counter, DATA_CONFIG_MODULE_START_COL).text <> END_OF_TABLE And default_modules_supported <> DATA_DEFAULT_CONFIG_MODUES_SUPPORTED)
        If (Cells(counter, DATA_CONFIG_MODULE_INPUT_SIZE_COL).value <> 0) And (Cells(counter, DATA_CONFIG_MODULE_OUTPUT_SIZE_COL).value <> 0) Then
            module_no_msb = Int(((counter - DATA_CONFIG_MODULE_MODULE_NO_OFFSET) And &HFF00) / (2 ^ 8))
            module_no_lsb = ((counter - DATA_CONFIG_MODULE_MODULE_NO_OFFSET) And &HFF)
            length_word_input = 0
            length_word_output = 0
            If (Cells(counter, DATA_CONFIG_MODULE_OUTPUT_WORD_SIZE_COL).value <> 0) And (Cells(counter, DATA_CONFIG_MODULE_OUTPUT_BIT_SIZE_COL).value = 0) And _
                (Cells(counter, DATA_CONFIG_MODULE_INPUT_WORD_SIZE_COL).value <> 0) And (Cells(counter, DATA_CONFIG_MODULE_INPUT_BIT_SIZE_COL).value = 0) Then
            length_word_output = (64 Or (Cells(counter, DATA_CONFIG_MODULE_OUTPUT_WORD_SIZE_COL).value - 1))
            length_word_input = (64 Or (Cells(counter, DATA_CONFIG_MODULE_INPUT_WORD_SIZE_COL).value - 1))
            PROFIBUS_DEFAULT_CFG_C.WriteLine ("    0xC2U, " & "0x" & Hex(length_word_output) & "U, 0x" & Hex(length_word_input) & _
                            "U, 0x" & Hex(module_no_msb) & "U, 0x" & Hex(module_no_lsb) & _
                            "U,  //" & Cells(counter, DATA_CONFIG_MODULE_START_COL).text)
          '  Else
        '    PROFIBUS_DEFAULT_CFG_C.WriteLine ("    0xC2U, " & "0x" & Hex(Cells(counter, DATA_CONFIG_MODULE_OUTPUT_SIZE_COL).text - 1) & "U, 0x" & Hex(Cells(counter, DATA_CONFIG_MODULE_INPUT_SIZE_COL).value - 1) & _
           '                 "U, 0x" & Hex(module_no_msb) & "U, 0x" & Hex(module_no_lsb) & _
         '                   "U,  //" & Cells(counter, DATA_CONFIG_MODULE_START_COL).text)
            default_modules_supported = default_modules_supported + 1
            End If
            
        End If
        counter = counter + 1
    Wend
     PROFIBUS_DEFAULT_CFG_C.WriteLine ("};")
    
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("const uint8_t*     " & "ProfiBus" & "_DEF_CFG_PTR = &" & "ProfiBus" & "_def_cfg_data[0];")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("")
    
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("/*")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("*        Create " & "ProfiBus" & " Default CFG Structure")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("*/")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("const Profibus_Cfg::DEF_CFG_STRUCT     " & "ProfiBus" & "_DEF_CFG_STRUCT = ")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("{")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("     const_cast<uint8_t*>(" & "ProfiBus" & "_DEF_CFG_PTR),")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("     " & "ProfiBus" & "_DEFAULT_CFG_SIZE,")
    PROFIBUS_DEFAULT_CFG_C.WriteLine ("};")


'****************************************************************************************************************************
'******     End of Default Configuration Creation.
'****************************************************************************************************************************
    
       

    PROFIBUS_DEFAULT_CFG_H.Close
    PROFIBUS_DEFAULT_CFG_C.Close


'****************************************************************************************************************************
'******     Start Creating the Cfg_List.H file header
'****************************************************************************************************************************
   
    PROFIBUS_CFG_MODULE_H.WriteLine ("/*")
    PROFIBUS_CFG_MODULE_H.WriteLine ("*****************************************************************************************")
    PROFIBUS_CFG_MODULE_H.WriteLine ("*       file@:" & Cells(MODULE_CONFIG_FILE_NAME_ROW, CONFIG_FILE_NAME_COL).text & ".h")
    PROFIBUS_CFG_MODULE_H.WriteLine ("*")
    PROFIBUS_CFG_MODULE_H.WriteLine ("*       @brief: ")
    PROFIBUS_CFG_MODULE_H.WriteLine ("*")
    PROFIBUS_CFG_MODULE_H.WriteLine ("*       @details: Defines configuration modules available for a product")
    PROFIBUS_CFG_MODULE_H.WriteLine ("*                    This is Automation Macro generated file,")
    PROFIBUS_CFG_MODULE_H.WriteLine ("*                    please avoid manual editing.")
    PROFIBUS_CFG_MODULE_H.WriteLine ("*       @version                                 ")
    PROFIBUS_CFG_MODULE_H.WriteLine ("*       C++ Style Guide Version 1.0")
    PROFIBUS_CFG_MODULE_H.WriteLine ("*")
    PROFIBUS_CFG_MODULE_H.WriteLine ("*       @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    PROFIBUS_CFG_MODULE_H.WriteLine ("*")
    PROFIBUS_CFG_MODULE_H.WriteLine ("*****************************************************************************************")
    PROFIBUS_CFG_MODULE_H.WriteLine ("*/")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#ifndef " & UCase(Cells(MODULE_CONFIG_FILE_NAME_ROW, CONFIG_FILE_INCLUDE_GUARD_COL).text) & "_H")
    PROFIBUS_CFG_MODULE_H.WriteLine ("  #define " & UCase(Cells(MODULE_CONFIG_FILE_NAME_ROW, CONFIG_FILE_INCLUDE_GUARD_COL).text) & "_H")
    PROFIBUS_CFG_MODULE_H.WriteLine ("")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#include ""DCI.h""")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#include ""DCI_Defines.h""")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#include ""DCI_Profibus_Data.h""")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#include ""Profibus_DCI.h""")
    PROFIBUS_CFG_MODULE_H.WriteLine ("")

'***************************************************************************************************************************
'******      Count number of available configuration modules to determine the size of cfg_list array
'***************************************************************************************************************************
    num_cfg_modules = 0
    num_prod_modules = 0
    num_cons_modules = 0
    num_prod_cons_modules = 0
    prod_data_size = 0
    cons_data_size = 0
    counter = DATA_CONFIG_MODULE_START_ROW
    
    While (Cells(counter, DATA_CONFIG_MODULE_START_COL).text <> END_OF_TABLE)
            num_cfg_modules = num_cfg_modules + 1
        If (Cells(counter, DATA_CONFIG_MODULE_INPUT_SIZE_COL).text <> Empty) And (Cells(counter, DATA_CONFIG_MODULE_OUTPUT_SIZE_COL).text = Empty) Then
            num_prod_modules = num_prod_modules + 1
            prod_data_size = prod_data_size + Cells(counter, DATA_CONFIG_MODULE_INPUT_SIZE_COL).value
        End If
        If (Cells(counter, DATA_CONFIG_MODULE_OUTPUT_SIZE_COL).text <> Empty) And (Cells(counter, DATA_CONFIG_MODULE_INPUT_SIZE_COL).text = Empty) Then
            num_cons_modules = num_cons_modules + 1
            cons_data_size = cons_data_size + Cells(counter, DATA_CONFIG_MODULE_OUTPUT_SIZE_COL).value
        End If
        If (Cells(counter, DATA_CONFIG_MODULE_OUTPUT_SIZE_COL).text <> Empty) And (Cells(counter, DATA_CONFIG_MODULE_INPUT_SIZE_COL).text <> Empty) Then
            num_prod_cons_modules = num_prod_cons_modules + 1
            prod_data_size = prod_data_size + Cells(counter, DATA_CONFIG_MODULE_INPUT_SIZE_COL).value
            cons_data_size = cons_data_size + Cells(counter, DATA_CONFIG_MODULE_OUTPUT_SIZE_COL).value
        End If
        counter = counter + 1
    Wend
    
    
    PROFIBUS_CFG_MODULE_H.WriteLine ("//*******************************************************")
    PROFIBUS_CFG_MODULE_H.WriteLine ("//******     " & "ProfiBus" & " Profibus Cfg Defines")
    PROFIBUS_CFG_MODULE_H.WriteLine ("//*******************************************************")
    PROFIBUS_CFG_MODULE_H.WriteLine ("")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#define      " & "ProfiBus" & "_MAX_CFG_MODS    " & num_cfg_modules & "U")
    PROFIBUS_CFG_MODULE_H.WriteLine ("// Production Module = Input Modules")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#define      " & "ProfiBus" & "_MAX_PROD_MODULES    " & num_prod_modules & "U")
    PROFIBUS_CFG_MODULE_H.WriteLine ("// Consumption Module = Output Modules")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#define      " & "ProfiBus" & "_MAX_CONS_MODULES    " & num_cons_modules & "U")
    PROFIBUS_CFG_MODULE_H.WriteLine ("// Production Consumption Module = Input Output Modules")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#define      " & "ProfiBus" & "_MAX_PROD_CONS_MODULES    " & num_prod_cons_modules & "U")
        If (prod_data_size <> 0) Then
    PROFIBUS_CFG_MODULE_H.WriteLine ("#define      " & "ProfiBus" & "_MAX_PROD_DATA_SIZE    " & prod_data_size & "U")
        Else
        PROFIBUS_CFG_MODULE_H.WriteLine ("#define      " & "ProfiBus" & "_MAX_PROD_DATA_SIZE    " & (prod_data_size + 1) & "U /*This is 1 because we dont have any ip parameter in module*/")
        End If
        If (cons_data_size <> 0) Then
    PROFIBUS_CFG_MODULE_H.WriteLine ("#define      " & "ProfiBus" & "_MAX_CONS_DATA_SIZE    " & cons_data_size & "U")
        Else
        PROFIBUS_CFG_MODULE_H.WriteLine ("#define      " & "ProfiBus" & "_MAX_CONS_DATA_SIZE    " & cons_data_size + 1 & "U /*This is 1 because we dont have any op parameter in module*/")
        End If
    PROFIBUS_CFG_MODULE_H.WriteLine ("")
    PROFIBUS_CFG_MODULE_H.WriteLine ("")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#define         EMPTY_MODULE            (static_cast<uint8_t>(0x00))")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#define         BASE_MODULE             (static_cast<uint8_t>(0x02))")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#define         INPUT_MODULE            (static_cast<uint8_t>(0x42))")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#define         OUTPUT_MODULE           (static_cast<uint8_t>(0x82))")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#define         OUTPUT_INPUT_MODULE     (static_cast<uint8_t>(0xC2))")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#define         UNSUPPORTED_MODULE      (static_cast<uint8_t>(0xFF))")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#define         EMPTY_PROFI_ID          (static_cast<uint16_t>(0xFFFF))")
    
    PROFIBUS_CFG_MODULE_H.WriteLine ("//Component size definition")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#define EMPTY 0U")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#define BIT 1U")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#define WORD 2U")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#define DWORD 3U")
    PROFIBUS_CFG_MODULE_H.WriteLine ("")
    
    
    PROFIBUS_CFG_MODULE_H.WriteLine ("//enum")
    PROFIBUS_CFG_MODULE_H.WriteLine ("//{")
    PROFIBUS_CFG_MODULE_H.WriteLine ("static const uint8_t BYTE_WORD = 0U;")
    PROFIBUS_CFG_MODULE_H.WriteLine ("static const uint8_t WHOLE_LENGTH = 1U;")
    PROFIBUS_CFG_MODULE_H.WriteLine ("//};")
    PROFIBUS_CFG_MODULE_H.WriteLine ("static const uint8_t MAX_PROFIBUS_MODULES = " & DATA_DEFAULT_CONFIG_MODUES_SUPPORTED & "U /* This is number of profibus module we want to have*/;")
    PROFIBUS_CFG_MODULE_H.WriteLine ("typedef struct")
    PROFIBUS_CFG_MODULE_H.WriteLine ("{")
    PROFIBUS_CFG_MODULE_H.WriteLine ("  MOD_NUM_TD            mod_num;")
    PROFIBUS_CFG_MODULE_H.WriteLine ("  DCI_LENGTH_TD         total_op_data_size;")
    PROFIBUS_CFG_MODULE_H.WriteLine ("  DCI_LENGTH_TD         total_ip_data_size;")
    PROFIBUS_CFG_MODULE_H.WriteLine ("  uint8_t                   unit_data_size;")
    PROFIBUS_CFG_MODULE_H.WriteLine ("  uint8_t                   mod_type;")
    PROFIBUS_CFG_MODULE_H.WriteLine ("  uint8_t                   data_consistency;")
    PROFIBUS_CFG_MODULE_H.WriteLine ("}CFG_MODULE_STRUCT;")
    PROFIBUS_CFG_MODULE_H.WriteLine ("typedef struct")
    PROFIBUS_CFG_MODULE_H.WriteLine ("{")
    PROFIBUS_CFG_MODULE_H.WriteLine ("  CFG_MODULE_STRUCT*  cfg_module_ptr;")
    PROFIBUS_CFG_MODULE_H.WriteLine ("  uint8_t             num_cfg_modules;")
    PROFIBUS_CFG_MODULE_H.WriteLine ("  uint8_t             max_prod_modules;")
    PROFIBUS_CFG_MODULE_H.WriteLine ("  uint8_t             max_cons_modules;")
    PROFIBUS_CFG_MODULE_H.WriteLine ("  uint8_t             max_prod_cons_modules;")
    PROFIBUS_CFG_MODULE_H.WriteLine ("  uint8_t             max_prod_data_size;")
    PROFIBUS_CFG_MODULE_H.WriteLine ("  uint8_t             max_cons_data_size;")
    PROFIBUS_CFG_MODULE_H.WriteLine ("}PROFI_CFG_STRUCT;")
    
'***************************************************************************************************************************
'******      Define cfg array
'***************************************************************************************************************************
    PROFIBUS_CFG_MODULE_H.WriteLine ("//*******************************************************")
    PROFIBUS_CFG_MODULE_H.WriteLine ("//******     " & "ProfiBus" & " Profibus Cfg Array ")
    PROFIBUS_CFG_MODULE_H.WriteLine ("//*******************************************************")
    PROFIBUS_CFG_MODULE_H.WriteLine ("")
    PROFIBUS_CFG_MODULE_H.WriteLine ("extern const CFG_MODULE_STRUCT " & "ProfiBus" & "_cfg_list[ " & "ProfiBus" & "_MAX_CFG_MODS ];")
    PROFIBUS_CFG_MODULE_H.WriteLine ("extern const PROFI_CFG_STRUCT " & "ProfiBus" & "_CFG_STRUCT;")
    PROFIBUS_CFG_MODULE_H.WriteLine ("")
    PROFIBUS_CFG_MODULE_H.WriteLine ("#endif")

'****************************************************************************************************************************
'******     End of Cfg_List.h
'****************************************************************************************************************************



'****************************************************************************************************************************
'******     Start Creating the CFG_List.C file header
'****************************************************************************************************************************
    PROFIBUS_CFG_MODULE_C.WriteLine ("/*")
    PROFIBUS_CFG_MODULE_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_CFG_MODULE_C.WriteLine ("*       @file:" & Cells(MODULE_CONFIG_FILE_NAME_ROW, CONFIG_FILE_NAME_COL).text & ".cpp")
    PROFIBUS_CFG_MODULE_C.WriteLine ("*")
    PROFIBUS_CFG_MODULE_C.WriteLine ("*")
    PROFIBUS_CFG_MODULE_C.WriteLine ("*       @details See header file for module overview.")
    PROFIBUS_CFG_MODULE_C.WriteLine ("*       @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    PROFIBUS_CFG_MODULE_C.WriteLine ("*")
    PROFIBUS_CFG_MODULE_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_CFG_MODULE_C.WriteLine ("*/")
    PROFIBUS_CFG_MODULE_C.WriteLine ("#include ""Includes.h""")
    PROFIBUS_CFG_MODULE_C.WriteLine ("#include """ & Cells(MODULE_CONFIG_FILE_NAME_ROW, CONFIG_FILE_NAME_COL).text & ".h""")
    PROFIBUS_CFG_MODULE_C.WriteLine ("")
    PROFIBUS_CFG_MODULE_C.WriteLine ("")
    PROFIBUS_CFG_MODULE_C.WriteLine ("")


'****************************************************************************************************************************
'******     Generate the Configuration List Array.
'****************************************************************************************************************************
    PROFIBUS_CFG_MODULE_C.WriteLine ("/*")
    PROFIBUS_CFG_MODULE_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_CFG_MODULE_C.WriteLine ("*        " & "ProfiBus" & " Cfg List Array")
    PROFIBUS_CFG_MODULE_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_CFG_MODULE_C.WriteLine ("*/")
    PROFIBUS_CFG_MODULE_C.WriteLine ("")
    PROFIBUS_CFG_MODULE_C.WriteLine ("const CFG_MODULE_STRUCT " & "ProfiBus" & "_cfg_list[ " & "ProfiBus" & "_MAX_CFG_MODS ] =")
    PROFIBUS_CFG_MODULE_C.WriteLine ("{")
    row_counter = DATA_CONFIG_MODULE_START_ROW
    ProfiID = 1
    While (Cells(row_counter, DATA_CONFIG_MODULE_START_COL).text <> END_OF_TABLE)
        If (Cells(row_counter, DATA_CONFIG_MODULE_OUTPUT_SIZE_COL).text <> Empty) Or (Cells(row_counter, DATA_CONFIG_MODULE_INPUT_SIZE_COL).text <> Empty) Then
            PROFIBUS_CFG_MODULE_C.WriteLine ("    {")
            PROFIBUS_CFG_MODULE_C.WriteLine ("        " & ProfiID & "U,")
            If (Cells(row_counter, DATA_CONFIG_MODULE_OUTPUT_SIZE_COL).text <> Empty) Then
                PROFIBUS_CFG_MODULE_C.WriteLine ("        " & Cells(row_counter, DATA_CONFIG_MODULE_OUTPUT_SIZE_COL).value & "U,")
            Else
                PROFIBUS_CFG_MODULE_C.WriteLine ("        " & 0 & "U,")
            End If
            If (Cells(row_counter, DATA_CONFIG_MODULE_INPUT_SIZE_COL).text <> Empty) Then
            PROFIBUS_CFG_MODULE_C.WriteLine ("        " & Cells(row_counter, DATA_CONFIG_MODULE_INPUT_SIZE_COL).value & "U,")
            Else
                PROFIBUS_CFG_MODULE_C.WriteLine ("        " & 0 & "U,")
            End If
            PROFIBUS_CFG_MODULE_C.WriteLine ("        " & 1 & "U,")
            If (Cells(row_counter, DATA_CONFIG_MODULE_OUTPUT_SIZE_COL).text <> Empty) And (Cells(row_counter, DATA_CONFIG_MODULE_INPUT_SIZE_COL).text = Empty) Then
                PROFIBUS_CFG_MODULE_C.WriteLine ("        OUTPUT_MODULE,")
            ElseIf (Cells(row_counter, DATA_CONFIG_MODULE_INPUT_SIZE_COL).text <> Empty) And (Cells(row_counter, DATA_CONFIG_MODULE_OUTPUT_SIZE_COL).text = Empty) Then
                    PROFIBUS_CFG_MODULE_C.WriteLine ("        INPUT_MODULE,")
            ElseIf (Cells(row_counter, DATA_CONFIG_MODULE_INPUT_SIZE_COL).text <> Empty) And (Cells(row_counter, DATA_CONFIG_MODULE_OUTPUT_SIZE_COL).text <> Empty) Then
                    PROFIBUS_CFG_MODULE_C.WriteLine ("        OUTPUT_INPUT_MODULE,")
            End If
            
            PROFIBUS_CFG_MODULE_C.WriteLine ("         BYTE_WORD,")
            PROFIBUS_CFG_MODULE_C.WriteLine ("    },")
            
        End If
        row_counter = row_counter + 1
        ProfiID = ProfiID + 1
    Wend
    PROFIBUS_CFG_MODULE_C.WriteLine ("};")
    PROFIBUS_CFG_MODULE_C.WriteLine ("")
    PROFIBUS_CFG_MODULE_C.WriteLine ("const CFG_MODULE_STRUCT*     " & "ProfiBus" & "_CFG_LIST_PTR = &" & "ProfiBus" & "_cfg_list[0];")
    PROFIBUS_CFG_MODULE_C.WriteLine ("")
    PROFIBUS_CFG_MODULE_C.WriteLine ("")
    
    PROFIBUS_CFG_MODULE_C.WriteLine ("/*")
    PROFIBUS_CFG_MODULE_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_CFG_MODULE_C.WriteLine ("*        Create " & "ProfiBus" & " Configuration Structure")
    PROFIBUS_CFG_MODULE_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_CFG_MODULE_C.WriteLine ("*/")
    PROFIBUS_CFG_MODULE_C.WriteLine ("")
    PROFIBUS_CFG_MODULE_C.WriteLine ("const PROFI_CFG_STRUCT     " & "ProfiBus" & "_CFG_STRUCT = ")
    PROFIBUS_CFG_MODULE_C.WriteLine ("{")
    PROFIBUS_CFG_MODULE_C.WriteLine ("     const_cast<CFG_MODULE_STRUCT*>(" & "ProfiBus" & "_CFG_LIST_PTR),")
    PROFIBUS_CFG_MODULE_C.WriteLine ("     " & "ProfiBus" & "_MAX_CFG_MODS,")
    PROFIBUS_CFG_MODULE_C.WriteLine ("     " & "ProfiBus" & "_MAX_PROD_MODULES,")
    PROFIBUS_CFG_MODULE_C.WriteLine ("     " & "ProfiBus" & "_MAX_CONS_MODULES,")
    PROFIBUS_CFG_MODULE_C.WriteLine ("     " & "ProfiBus" & "_MAX_PROD_CONS_MODULES,")
    PROFIBUS_CFG_MODULE_C.WriteLine ("     " & "ProfiBus" & "_MAX_PROD_DATA_SIZE,")
    PROFIBUS_CFG_MODULE_C.WriteLine ("     " & "ProfiBus" & "_MAX_CONS_DATA_SIZE,")
    PROFIBUS_CFG_MODULE_C.WriteLine ("};")
        
'****************************************************************************************************************************
'******     End of Cfg_Structure Creation
'****************************************************************************************************************************


    PROFIBUS_CFG_MODULE_H.Close
    PROFIBUS_CFG_MODULE_C.Close
    PROFIBUS_IP_BITLOOKUP_C.Close
    PROFIBUS_IP_BITLOOKUP_H.Close
    PROFIBUS_IP_WORD_LOOKUP_C.Close
    PROFIBUS_IP_WORD_LOOKUP_H.Close
    PROFIBUS_IO_MODULE_STRUCT_C.Close
    PROFIBUS_IO_MODULE_STRUCT_H.Close


End Sub



Function Find_Number_Of_Bits()
            DESCRIPTION_COL = 81
            description_content_string = Worksheets("DCI Descriptors").Cells(dci_sheet_row_temp, DESCRIPTION_COL).text
            descriptionString = Split(description_content_string, vbLf)
            Dim count As Integer
            For count = 1 To UBound(descriptionString)
            LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("    {")
            LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        //" & descriptionString(count))
            LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        " & Cells(row_counter, DATA_DEFINE_COL) & "_PROFIBUS_MOD,")
            LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("        " & count - 1)
            LTK_PROFI_OP_COMP_PRM_STRUCT_CPP.WriteLine ("    },")
            total_bits = total_bits + 1
            Next count
End Function


''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
'''Acyclic Implementation
Sub Create_Profibus_Acyclic_LTables()

    Application.Calculation = xlCalculationManual
    
    'Dim BEGIN_IGNORED_DATA As String
    'BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    'ACYCLIC Parameter = "ACYCLIC PARAMETER"
    
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
        DATA_DCI_TOTAL_LENGTH = 6
    DATA_TYPE_COL = 3
    DATA_TYPE_LENGTH_COL = 4
    GSD_FILE_TYPE = 4
    DATA_SLOT_NO_COL = 5
    DATA_INDEX_COL = 6
    DATA_ASSEMBLY_ID_COL = 7
    
    Dim i As Variant
    Dim j As Variant
    Dim k As Variant
    Dim assembly_id(512) As Variant
    Dim assembly_params(512) As Byte
    Dim assembly_size(512) As Byte
    Dim assembly_numbers As Variant
    assembly_numbers = 0
    total_acyclic_parameters = 0
    
    
    Sheets("Profibus (DCI)").Select
    
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\" & Cells(9, 2).value
    Set PROFIBUS_ACYCLIC_C = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(10, 2).value
    Set PROFIBUS_ACYCLIC_H = fs.CreateTextFile(file_path, True)
    
    'Making font type and size consistent.
    Worksheets("Profibus (DCI)").Range("A:N").Font.name = "Arial"
    Worksheets("Profibus (DCI)").Range("A:N").Font.Size = 10
    
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
'******     Start Creating the Profibus_Acyclic_Data.H file header
'****************************************************************************************************************************
    PROFIBUS_ACYCLIC_H.WriteLine ("/*")
    PROFIBUS_ACYCLIC_H.WriteLine ("*****************************************************************************************")
    PROFIBUS_ACYCLIC_H.WriteLine ("*       $Workfile:" & Cells(8, 2).text & ".h$")
    PROFIBUS_ACYCLIC_H.WriteLine ("*")
    PROFIBUS_ACYCLIC_H.WriteLine ("*       $Author:$ Rajesh Chavan")
    PROFIBUS_ACYCLIC_H.WriteLine ("*       Copyright © Eaton Corporation. All Rights Reserved.")
    PROFIBUS_ACYCLIC_H.WriteLine ("*       Creator: Rajesh Chavan")
    PROFIBUS_ACYCLIC_H.WriteLine ("*")
    PROFIBUS_ACYCLIC_H.WriteLine ("*       Description:Defines the look up structure for the Acyclic Data exchange ")
    PROFIBUS_ACYCLIC_H.WriteLine ("*")
    PROFIBUS_ACYCLIC_H.WriteLine ("*")
    PROFIBUS_ACYCLIC_H.WriteLine ("*       Code Inspection Date: ")
    PROFIBUS_ACYCLIC_H.WriteLine ("*")
    PROFIBUS_ACYCLIC_H.WriteLine ("*       $Header:$")
    PROFIBUS_ACYCLIC_H.WriteLine ("*****************************************************************************************")
    PROFIBUS_ACYCLIC_H.WriteLine ("*/")
    PROFIBUS_ACYCLIC_H.WriteLine ("#ifndef " & UCase(Cells(8, 2).text) & "_H")
    PROFIBUS_ACYCLIC_H.WriteLine ("  #define " & UCase(Cells(8, 2).text) & "_H")
    PROFIBUS_ACYCLIC_H.WriteLine ("")
    PROFIBUS_ACYCLIC_H.WriteLine ("")
    If (total_acyclic_parameters > 0) Then
    PROFIBUS_ACYCLIC_H.WriteLine ("#define ENABLE_ACYCLIC_COMMUNICATION 1")
    PROFIBUS_ACYCLIC_H.WriteLine ("")
    End If
    PROFIBUS_ACYCLIC_H.WriteLine ("#ifdef ENABLE_ACYCLIC_COMMUNICATION")
    PROFIBUS_ACYCLIC_H.WriteLine ("#include " & """DCI.h""")
    PROFIBUS_ACYCLIC_H.WriteLine ("#include " & """DCI_Defines.h""")
    PROFIBUS_ACYCLIC_H.WriteLine ("#include " & """DCI_Profibus_Data.h""")
    PROFIBUS_ACYCLIC_H.WriteLine ("#include " & """ProfiBus_Identity.h""")
    PROFIBUS_ACYCLIC_H.WriteLine ("")
    PROFIBUS_ACYCLIC_H.WriteLine ("")
    PROFIBUS_ACYCLIC_H.WriteLine ("#define TOTAL_ACYCLIC_ASSEMBLIES " & assembly_numbers & "U")
    PROFIBUS_ACYCLIC_H.WriteLine ("#define TOTAL_ACYCLIC_PARAMETERS " & total_acyclic_parameters & "U")
    PROFIBUS_ACYCLIC_H.WriteLine ("extern const PROFIBUS_ACYCLIC_TABLE_TD PROFIBUS_ASSEMBLY_DATA[TOTAL_ACYCLIC_ASSEMBLIES];")
    PROFIBUS_ACYCLIC_H.WriteLine ("extern const PROFIBUS_PRM_ASSEMLY_TABLE_TD PROFIBUS_PRM_ASSEMBLY_DATA[TOTAL_ACYCLIC_PARAMETERS];")
    
    
    PROFIBUS_ACYCLIC_H.WriteLine ("")
    PROFIBUS_ACYCLIC_H.WriteLine ("")
    PROFIBUS_ACYCLIC_H.WriteLine ("#endif")
    PROFIBUS_ACYCLIC_H.WriteLine ("#endif")
    
'****************************************************************************************************************************
'******     Start Creating the Profibus_Acyclic_Data.CPP file
'****************************************************************************************************************************
    PROFIBUS_ACYCLIC_C.WriteLine ("/*")
    PROFIBUS_ACYCLIC_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_ACYCLIC_C.WriteLine ("*       $Workfile:" & Cells(8, 2).text & ".CPP$")
    PROFIBUS_ACYCLIC_C.WriteLine ("*")
    PROFIBUS_ACYCLIC_C.WriteLine ("*       $Author:$ Rajesh Chavan")
    PROFIBUS_ACYCLIC_C.WriteLine ("*       Copyright © Eaton Corporation. All Rights Reserved.")
    PROFIBUS_ACYCLIC_C.WriteLine ("*       Creator: Rajesh Chavan")
    PROFIBUS_ACYCLIC_C.WriteLine ("*")
    PROFIBUS_ACYCLIC_C.WriteLine ("*       Description: Defines the look up structure for the Acyclic Data exchange ")
    PROFIBUS_ACYCLIC_C.WriteLine ("*")
    PROFIBUS_ACYCLIC_C.WriteLine ("*")
    PROFIBUS_ACYCLIC_C.WriteLine ("*       Code Inspection Date: ")
    PROFIBUS_ACYCLIC_C.WriteLine ("*")
    PROFIBUS_ACYCLIC_C.WriteLine ("*       $Header:$")
    PROFIBUS_ACYCLIC_C.WriteLine ("*****************************************************************************************")
    PROFIBUS_ACYCLIC_C.WriteLine ("*/")
    PROFIBUS_ACYCLIC_H.WriteLine ("")

    PROFIBUS_ACYCLIC_C.WriteLine ("#include ""Includes.h""")
    PROFIBUS_ACYCLIC_C.WriteLine ("#include " & "" & """" & Cells(8, 2).text & ".h""")
    PROFIBUS_ACYCLIC_C.WriteLine ("")
    PROFIBUS_ACYCLIC_C.WriteLine ("")
    PROFIBUS_ACYCLIC_C.WriteLine ("#ifdef ENABLE_ACYCLIC_COMMUNICATION")
    PROFIBUS_ACYCLIC_C.WriteLine ("")
    PROFIBUS_ACYCLIC_C.WriteLine ("")
    
    
    PROFIBUS_ACYCLIC_C.WriteLine ("const PROFIBUS_ACYCLIC_TABLE_TD PROFIBUS_ASSEMBLY_DATA[TOTAL_ACYCLIC_ASSEMBLIES] = ")
    PROFIBUS_ACYCLIC_C.WriteLine ("{")
    i = 0
    For i = 0 To assembly_numbers - 1
        PROFIBUS_ACYCLIC_C.WriteLine ("    {")
        PROFIBUS_ACYCLIC_C.WriteLine ("        " & assembly_id(i) & "U, /* Assembly_id */")
        PROFIBUS_ACYCLIC_C.WriteLine ("        " & assembly_params(i) & "U,/* Number of params in Assembly */")
        PROFIBUS_ACYCLIC_C.WriteLine ("        " & assembly_size(i) & "U, /* Total bytes in Assembly */")
        PROFIBUS_ACYCLIC_C.WriteLine ("    },")
    Next i
    PROFIBUS_ACYCLIC_C.WriteLine ("};")
    
    PROFIBUS_ACYCLIC_C.WriteLine ("") ' Give some space between structures
    PROFIBUS_ACYCLIC_C.WriteLine ("")
    PROFIBUS_ACYCLIC_C.WriteLine ("const PROFIBUS_PRM_ASSEMLY_TABLE_TD PROFIBUS_PRM_ASSEMBLY_DATA[TOTAL_ACYCLIC_PARAMETERS] = ")
    PROFIBUS_ACYCLIC_C.WriteLine ("{")
    
    
    
    For i = 0 To assembly_numbers - 1
        counter = START_ROW
        byte_offset = 0
        While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            If (Cells(counter, DATA_ASSEMBLY_ID_COL).text <> Empty And Cells(counter, GSD_FILE_TYPE).text = "ACYCLIC PARAMETER") Then
                If (Cells(counter, DATA_ASSEMBLY_ID_COL).value = assembly_id(i)) Then
                    PROFIBUS_ACYCLIC_C.WriteLine (" /* This parameter belongs to assembly Id " & assembly_id(i) & "*/")
                                        
                                        dci_sheet_row = Find_DCID_Row(Worksheets("ProfiBus (DCI)").Cells(counter, DATA_DEFINE_COL).text)
                                        dci_sheet_row_temp = dci_sheet_row
                    PROFIBUS_ACYCLIC_C.WriteLine ("    {")
                    PROFIBUS_ACYCLIC_C.WriteLine ("        " & Cells(counter, DATA_DEFINE_COL).text & "_PROFIBUS_MOD, /* DCID */")
                    PROFIBUS_ACYCLIC_C.WriteLine ("        " & Worksheets("DCI Descriptors").Cells(dci_sheet_row_temp, DATA_DCI_TOTAL_LENGTH).value & "U, /* Data length */")
                    PROFIBUS_ACYCLIC_C.WriteLine ("        " & Worksheets("DCI Descriptors").Cells(dci_sheet_row_temp, DATA_TYPE_COL).text & ", /* Data type*/")
                    PROFIBUS_ACYCLIC_C.WriteLine ("        " & Worksheets("DCI Descriptors").Cells(dci_sheet_row_temp, DATA_TYPE_LENGTH_COL).text & "U, /* Data type length */")
                    PROFIBUS_ACYCLIC_C.WriteLine ("        " & byte_offset & "U, /* Byte Offset in assembly " & assembly_id(i) & "*/")
                    PROFIBUS_ACYCLIC_C.WriteLine ("    },")
                    byte_offset = byte_offset + Worksheets("DCI Descriptors").Cells(dci_sheet_row_temp, DATA_DCI_TOTAL_LENGTH).value
                End If
            End If
            counter = counter + 1
        Wend
        PROFIBUS_ACYCLIC_C.WriteLine ("")
        PROFIBUS_ACYCLIC_C.WriteLine ("")
    Next i
        
    PROFIBUS_ACYCLIC_C.WriteLine ("};")
    
    PROFIBUS_ACYCLIC_C.WriteLine ("#endif")
End Sub
