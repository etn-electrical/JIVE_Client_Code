Attribute VB_Name = "Modbus_Builder"
'***************************************************************
'Modbus Sheet Indexing.
Public Const MODBUS_DATA_START_ROW = 9
Public Const MODBUS_DESCRIP_COL = 1
Public Const MODBUS_DATA_DEFINE_COL = 2
Public Const MODBUS_DATA_LENGTH_COL = 3
Public Const MODBUS_DATA_REG_COL = 4
Public Const MODBUS_DATA_ATOMIBLOCK_SELECT = 5
Public Const MODBUS_USER_DOC_IGNORE_COL = 27
Public Const MODBUS_ATOMIC_FILENAME_ROW = 5
Public Const MODBUS_ATOMIC_FILENAME_COL = 2

Public Const DEVICE_ID_VENDOR_NAME_DCID_ROW = 1
Public Const DEVICE_ID_VENDOR_NAME_DCID_COL = 35
Public Const DEVICE_ID_PROD_CODE_DCID_ROW = 2
Public Const DEVICE_ID_PROD_CODE_DCID_COL = 35
Public Const DEVICE_ID_FW_REV_DCID_ROW = 3
Public Const DEVICE_ID_FW_REV_DCID_COL = 35
Public Const DEVICE_ID_VENDOR_URL_DCID_ROW = 4
Public Const DEVICE_ID_VENDOR_URL_DCID_COL = 35
Public Const DEVICE_ID_PROD_NAME_DCID_ROW = 5
Public Const DEVICE_ID_PROD_NAME_DCID_COL = 35
Public Const DEVICE_ID_MODEL_NAME_DCID_ROW = 6
Public Const DEVICE_ID_MODEL_NAME_DCID_COL = 35
Public Const DEVICE_ID_APP_NAME_DCID_ROW = 7
Public Const DEVICE_ID_APP_NAME_DCID_COL = 35

Public Const BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"

'**************************
'**************************
'Modbus Construction sheet.
'**************************
'**************************
Sub Create_Modbus_DCI_Xfer()

'*********************
'******     Constants
'*********************
    Call Verify_Modbus_List(False)
    Call Create_Modbus_Documentation

    Application.Calculation = xlCalculationManual
    
    Dim sheet_name As String

    Dim DCI_STRING As String
    Dim num_unique_data As Integer
    Dim file_name As String
    Dim lower_case_file_name As String

    INTERCOM_CHANGE_TRACKER_ROW = 1
    INTERCOM_CHANGE_TRACKER_COL = 45
    INTER_MESSAGE_DELAY_ROW = 2
    INTER_MESSAGE_DELAY_COL = 45

'*********************
'******     Begin
'*********************
       
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select

    file_name = Cells(2, 2).value
    lower_case_file_name = LCase(file_name)
    upper_case_file_name = UCase(file_name)
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\" & Cells(3, 2).value
    Set MODBUS_DCI_C = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(4, 2).value
    Set MODBUS_DCI_H = fs.CreateTextFile(file_path, True)
    
    'Making font type and size consistent.
    Worksheets(sheet_name).Range("A:AN").Font.name = "Arial"
    Worksheets(sheet_name).Range("A:AN").Font.Size = 10


'****************************************************************************************************************************
'******     Start Creating the .H file header
'****************************************************************************************************************************
    MODBUS_DCI_H.WriteLine ("/**")
    MODBUS_DCI_H.WriteLine ("*****************************************************************************************")
    MODBUS_DCI_H.WriteLine ("*   @file")
    MODBUS_DCI_H.WriteLine ("*")
    MODBUS_DCI_H.WriteLine ("*   @brief")
    MODBUS_DCI_H.WriteLine ("*")
    MODBUS_DCI_H.WriteLine ("*   @details")
    MODBUS_DCI_H.WriteLine ("*")
    MODBUS_DCI_H.WriteLine ("*   @version")
    MODBUS_DCI_H.WriteLine ("*   C++ Style Guide Version 1.0")
    MODBUS_DCI_H.WriteLine ("*")
    MODBUS_DCI_H.WriteLine ("*   @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    MODBUS_DCI_H.WriteLine ("*")
    MODBUS_DCI_H.WriteLine ("*****************************************************************************************")
    MODBUS_DCI_H.WriteLine ("*/")
    MODBUS_DCI_H.WriteLine ("#ifndef " & upper_case_file_name & "_H")
    MODBUS_DCI_H.WriteLine ("  #define " & upper_case_file_name & "_H")
    MODBUS_DCI_H.WriteLine ("")
    MODBUS_DCI_H.WriteLine ("#include ""DCI.h""")
    MODBUS_DCI_H.WriteLine ("#include ""Modbus_DCI.h""")
    MODBUS_DCI_H.WriteLine ("")
    
'****************************************************************************************************************************
'******     Sort the modbus register list
'****************************************************************************************************************************
    end_of_data = Begin_Ignored_Row(Worksheets(sheet_name)) - 1
    
    Rows(MODBUS_DATA_START_ROW & ":" & end_of_data).Sort Key1:=Cells(MODBUS_DATA_START_ROW, MODBUS_DATA_REG_COL), _
        Order1:=xlAscending, Header:=xlGuess, OrderCustom:=1, MatchCase:=False, Orientation:=xlTopToBottom

'****************************************************************************************************************************
'******     Gotta calculate the total number of Modbus registers we are going to need given a certain length.
'****************************************************************************************************************************
    num_registers = 0
    total_modbus_registers = 0
    counter = MODBUS_DATA_START_ROW
    While (Cells(counter, MODBUS_DATA_DEFINE_COL).text <> Empty) And (Cells(counter, MODBUS_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, MODBUS_DATA_LENGTH_COL).value Mod 2) = 1 Then
            num_registers = num_registers + Cells(counter, MODBUS_DATA_LENGTH_COL).value + 1
        Else
            num_registers = num_registers + Cells(counter, MODBUS_DATA_LENGTH_COL).value
        End If
        num_unique_data = num_unique_data + 1
        counter = counter + 1
    Wend
    num_registers = num_registers / 2

'****************************************************************************************************************************
'******     Create the header and associated file structure.  Get ready to plop the data down.
'****************************************************************************************************************************
    MODBUS_DCI_H.WriteLine ("//*******************************************************")
    MODBUS_DCI_H.WriteLine ("//******     Modbus structure and size follow.")
    MODBUS_DCI_H.WriteLine ("//*******************************************************")
    MODBUS_DCI_H.WriteLine ("")
    MODBUS_DCI_H.WriteLine ("extern const MODBUS_TARGET_INFO_ST_TD " & lower_case_file_name & "_target_info;")
    MODBUS_DCI_H.WriteLine ("")
    MODBUS_DCI_H.WriteLine ("")
    MODBUS_DCI_H.WriteLine ("#endif")



'****************************************************************************************************************************
'******     Start Creating the .C file header
'****************************************************************************************************************************
    MODBUS_DCI_C.WriteLine ("/**")
    MODBUS_DCI_C.WriteLine ("*****************************************************************************************")
    MODBUS_DCI_C.WriteLine ("*   @file")
    MODBUS_DCI_C.WriteLine ("*   @details See header file for module overview.")
    MODBUS_DCI_C.WriteLine ("*   @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    MODBUS_DCI_C.WriteLine ("*")
    MODBUS_DCI_C.WriteLine ("*****************************************************************************************")
    MODBUS_DCI_C.WriteLine ("*/")
    MODBUS_DCI_C.WriteLine ("#include ""Includes.h""")
    MODBUS_DCI_C.WriteLine ("#include """ & file_name & ".h""")
    MODBUS_DCI_C.WriteLine ("")
    MODBUS_DCI_C.WriteLine ("")
    
'****************************************************************************************************************************
'******     Generate the Modbus Object identity information.
'****************************************************************************************************************************
    MODBUS_DCI_C.WriteLine ("//*******************************************************")
    MODBUS_DCI_C.WriteLine ("//******     Modbus Object Defines.")
    MODBUS_DCI_C.WriteLine ("//*******************************************************")
    MODBUS_DCI_C.WriteLine ("")

    'The following items add the Device Identity Object required by the Modbus Identity object definition.
    If Cells(DEVICE_ID_VENDOR_NAME_DCID_ROW, DEVICE_ID_VENDOR_NAME_DCID_COL).text <> Empty Then
        MODBUS_DCI_C.WriteLine ("static const DCI_ID_TD MODBUS_DEV_ID_VENDOR_ID = " & _
                        Cells(DEVICE_ID_VENDOR_NAME_DCID_ROW, DEVICE_ID_VENDOR_NAME_DCID_COL).text & "_DCID;")
        MODBUS_DCI_C.WriteLine ("static const DCI_ID_TD MODBUS_DEV_ID_PROD_CODE_ID = " & _
                        Cells(DEVICE_ID_PROD_CODE_DCID_ROW, DEVICE_ID_PROD_CODE_DCID_COL).text & "_DCID;")
        MODBUS_DCI_C.WriteLine ("static const DCI_ID_TD MODBUS_DEV_ID_FW_REV_ID = " & _
                        Cells(DEVICE_ID_FW_REV_DCID_ROW, DEVICE_ID_FW_REV_DCID_COL).text & "_DCID;")
        MODBUS_DCI_C.WriteLine ("static const DCI_ID_TD MODBUS_DEV_ID_VENDOR_URL_ID = " & _
                        Cells(DEVICE_ID_VENDOR_URL_DCID_ROW, DEVICE_ID_VENDOR_URL_DCID_COL).text & "_DCID;")
        MODBUS_DCI_C.WriteLine ("static const DCI_ID_TD MODBUS_DEV_ID_PROD_NAME_ID = " & _
                        Cells(DEVICE_ID_PROD_NAME_DCID_ROW, DEVICE_ID_PROD_NAME_DCID_COL).text & "_DCID;")
        MODBUS_DCI_C.WriteLine ("static const DCI_ID_TD MODBUS_DEV_ID_MODEL_NAME_ID = " & _
                        Cells(DEVICE_ID_MODEL_NAME_DCID_ROW, DEVICE_ID_MODEL_NAME_DCID_COL).text & "_DCID;")
        MODBUS_DCI_C.WriteLine ("static const DCI_ID_TD MODBUS_DEV_ID_USER_APP_NAME_ID = " & _
                        Cells(DEVICE_ID_APP_NAME_DCID_ROW, DEVICE_ID_APP_NAME_DCID_COL).text & "_DCID;")
    Else
        MODBUS_DCI_C.WriteLine ("static const DCI_ID_TD MODBUS_DEV_ID_VENDOR_ID = DCI_ID_UNDEFINED;")
        MODBUS_DCI_C.WriteLine ("static const DCI_ID_TD MODBUS_DEV_ID_PROD_CODE_ID = DCI_ID_UNDEFINED;")
        MODBUS_DCI_C.WriteLine ("static const DCI_ID_TD MODBUS_DEV_ID_FW_REV_ID = DCI_ID_UNDEFINED;")
        MODBUS_DCI_C.WriteLine ("static const DCI_ID_TD MODBUS_DEV_ID_VENDOR_URL_ID = DCI_ID_UNDEFINED;")
        MODBUS_DCI_C.WriteLine ("static const DCI_ID_TD MODBUS_DEV_ID_PROD_NAME_ID = DCI_ID_UNDEFINED;")
        MODBUS_DCI_C.WriteLine ("static const DCI_ID_TD MODBUS_DEV_ID_MODEL_NAME_ID = DCI_ID_UNDEFINED;")
        MODBUS_DCI_C.WriteLine ("static const DCI_ID_TD MODBUS_DEV_ID_USER_APP_NAME_ID = DCI_ID_UNDEFINED;")
    End If
    MODBUS_DCI_C.WriteLine ("")

'****************************************************************************************************************************
'******     Generate the Modbus Register table.
'****************************************************************************************************************************
    MODBUS_DCI_C.WriteLine ("")
    MODBUS_DCI_C.WriteLine ("//*******************************************************")
    MODBUS_DCI_C.WriteLine ("//******     Modbus Register List")
    MODBUS_DCI_C.WriteLine ("//*******************************************************")
    MODBUS_DCI_C.WriteLine ("")
    row_counter = MODBUS_DATA_START_ROW
    While (Cells(row_counter, MODBUS_DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, MODBUS_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        MODBUS_DCI_C.WriteLine ("static const modbus_reg_t " & Cells(row_counter, MODBUS_DATA_DEFINE_COL).text & "_MODBUS_REG = " & _
                                Cells(row_counter, MODBUS_DATA_REG_COL).text & "U;")
        row_counter = row_counter + 1
    Wend


'****************************************************************************************************************************
'******     Generate the straight modbus to DCI lookup table.
'****************************************************************************************************************************
    
    MODBUS_DCI_C.WriteLine ("")
    MODBUS_DCI_C.WriteLine ("//*******************************************************")
    MODBUS_DCI_C.WriteLine ("//******     The Modbus Register Counts.")
    MODBUS_DCI_C.WriteLine ("//*******************************************************")
    MODBUS_DCI_C.WriteLine ("static const modbus_reg_t " & upper_case_file_name & "_TOTAL_REGISTERS = " & num_registers & "U;")
    MODBUS_DCI_C.WriteLine ("static const DCI_ID_TD " & upper_case_file_name & "_TOTAL_DCIDS = " & num_unique_data & "U;")
    MODBUS_DCI_C.WriteLine ("")
    MODBUS_DCI_C.WriteLine ("")
    MODBUS_DCI_C.WriteLine ("//*******************************************************")
    MODBUS_DCI_C.WriteLine ("//******     The Modbus Register Cross Reference Table.")
    MODBUS_DCI_C.WriteLine ("//*******************************************************")
    MODBUS_DCI_C.WriteLine ("")
    row_counter = MODBUS_DATA_START_ROW
    
    MODBUS_DCI_C.WriteLine ("static const DCI_MBUS_TO_DCID_LKUP_ST_TD " & _
                lower_case_file_name & "_mbus_to_dcid[" & upper_case_file_name & "_TOTAL_REGISTERS] = ")
    MODBUS_DCI_C.WriteLine ("{")
    While (Cells(row_counter, MODBUS_DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, MODBUS_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        reg_dup_counter = 0
        While (reg_dup_counter * 2) < Cells(row_counter, MODBUS_DATA_LENGTH_COL).value    'Run until the reg count compensates for the length
            modbus_register_counter = Cells(row_counter, MODBUS_DATA_REG_COL).value
            modbus_register_counter = modbus_register_counter + reg_dup_counter
            MODBUS_DCI_C.WriteLine ("    {        // " & Cells(row_counter, MODBUS_DESCRIP_COL).text & " + " & reg_dup_counter)
            If (reg_dup_counter = 0) Then
                MODBUS_DCI_C.WriteLine ("        " & Cells(row_counter, MODBUS_DATA_DEFINE_COL).text & "_MODBUS_REG,")
            Else
                MODBUS_DCI_C.WriteLine ("        " & Cells(row_counter, MODBUS_DATA_DEFINE_COL).text & "_MODBUS_REG + " & reg_dup_counter & "U,")
            End If
            MODBUS_DCI_C.WriteLine ("        " & reg_dup_counter * 2 & "U,")
            MODBUS_DCI_C.WriteLine ("        " & Cells(row_counter, MODBUS_DATA_DEFINE_COL).text & "_DCID")
            reg_dup_counter = reg_dup_counter + 1
            If (Cells(row_counter + 1, MODBUS_DATA_DEFINE_COL).text = Empty) And ((reg_dup_counter * 2) >= Cells(row_counter, MODBUS_DATA_LENGTH_COL).value) Then
                MODBUS_DCI_C.WriteLine ("    } ")
            Else
                MODBUS_DCI_C.WriteLine ("    }, ")
            End If
        Wend
        row_counter = row_counter + 1
    Wend
    MODBUS_DCI_C.WriteLine ("};")




'****************************************************************************************************************************
'******     Generate the reverse lookup modbus to DCI lookup table.
'****************************************************************************************************************************

'****************************************************************************************************************************
'******     Takes the modbus list and reverses it back to the DCI values.
'******     This should eventually create the same sorted list as modbus to use the CB search.
'****************************************************************************************************************************
    MODBUS_DCI_C.WriteLine ("")
    MODBUS_DCI_C.WriteLine ("")
    
    If (Cells(MODBUS_ATOMIC_FILENAME_ROW, MODBUS_ATOMIC_FILENAME_COL).text <> "") Then
        dci_defines_row_ctr = DCI_DEFINES_START_ROW
        MODBUS_DCI_C.WriteLine ("static const DCI_MBUS_FROM_DCID_LKUP_ST_TD " & _
                            lower_case_file_name & "_dcid_to_mbus[" & upper_case_file_name & "_TOTAL_DCIDS] = ")
        MODBUS_DCI_C.WriteLine ("{")
        While Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> Empty
            DCI_STRING = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text
            row_counter = MODBUS_DATA_START_ROW
            While (Cells(row_counter, MODBUS_DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, MODBUS_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA) _
                        And (Cells(row_counter, MODBUS_DATA_DEFINE_COL).text <> DCI_STRING)
                row_counter = row_counter + 1
            Wend
            If Cells(row_counter, MODBUS_DATA_DEFINE_COL).text = DCI_STRING Then
                MODBUS_DCI_C.WriteLine ("    {        // " & Cells(row_counter, MODBUS_DESCRIP_COL).text)
                MODBUS_DCI_C.WriteLine ("        " & DCI_STRING & "_DCID,")     'Cells(row_counter, MODBUS_DATA_REG_COL).Value & ",")
                MODBUS_DCI_C.WriteLine ("        " & Cells(row_counter, MODBUS_DATA_LENGTH_COL).value & "U,")
                MODBUS_DCI_C.WriteLine ("        " & Cells(row_counter, MODBUS_DATA_DEFINE_COL).text & "_MODBUS_REG")
                If (Cells(row_counter + 1, MODBUS_DATA_DEFINE_COL).text = Empty) Then
                    MODBUS_DCI_C.WriteLine ("    } ")
                Else
                    MODBUS_DCI_C.WriteLine ("    }, ")
                End If
            End If
            dci_defines_row_ctr = dci_defines_row_ctr + 1
        Wend
        MODBUS_DCI_C.WriteLine ("};")
    Else
        MODBUS_DCI_C.WriteLine ("//The reverse lookup behavior was not needed so it was removed and replaced with a null.")
        MODBUS_DCI_C.WriteLine ("static const DCI_MBUS_FROM_DCID_LKUP_ST_TD* const " & _
            lower_case_file_name & "_dcid_to_mbus = nullptr;")
    End If
    
    MODBUS_DCI_C.WriteLine ("")
    MODBUS_DCI_C.WriteLine ("")
    MODBUS_DCI_C.WriteLine ("//*******************************************************")
    MODBUS_DCI_C.WriteLine ("//******     The Modbus Object Definition.")
    MODBUS_DCI_C.WriteLine ("//*******************************************************")
    MODBUS_DCI_C.WriteLine ("")
    MODBUS_DCI_C.WriteLine ("static const DCI_ID_TD " & lower_case_file_name & "_mbus_obj[MB_DEV_ID_MAX_OBJECTS] = ")
    MODBUS_DCI_C.WriteLine ("    { MODBUS_DEV_ID_VENDOR_ID, MODBUS_DEV_ID_PROD_CODE_ID, MODBUS_DEV_ID_FW_REV_ID, ")
    MODBUS_DCI_C.WriteLine ("        MODBUS_DEV_ID_VENDOR_URL_ID, MODBUS_DEV_ID_PROD_NAME_ID, MODBUS_DEV_ID_MODEL_NAME_ID, ")
    MODBUS_DCI_C.WriteLine ("        MODBUS_DEV_ID_USER_APP_NAME_ID };")
    MODBUS_DCI_C.WriteLine ("")
    MODBUS_DCI_C.WriteLine ("")

    MODBUS_DCI_C.WriteLine ("//*******************************************************")
    MODBUS_DCI_C.WriteLine ("//******     The Modbus Target Definition Structure.")
    MODBUS_DCI_C.WriteLine ("//*******************************************************")
    MODBUS_DCI_C.WriteLine ("")
    MODBUS_DCI_C.WriteLine ("const MODBUS_TARGET_INFO_ST_TD " & lower_case_file_name & "_target_info =")
    MODBUS_DCI_C.WriteLine ("{")
    MODBUS_DCI_C.WriteLine ("    MB_DEV_ID_MAX_OBJECTS,        //uint8_t total_mbus_objs")
    MODBUS_DCI_C.WriteLine ("    " & lower_case_file_name & "_mbus_obj,        //const DCI_ID_TD* mbus_obj_list")
    MODBUS_DCI_C.WriteLine ("    " & upper_case_file_name & "_TOTAL_REGISTERS,        //MODBUS_REG_TD total_registers")
    MODBUS_DCI_C.WriteLine ("    " & lower_case_file_name & "_mbus_to_dcid,        //const DCI_MBUS_TO_DCID_LKUP_ST_TD* mbus_to_dcid")
    MODBUS_DCI_C.WriteLine ("    " & upper_case_file_name & "_TOTAL_DCIDS,        //DCI_ID_TD total_dcids")
    MODBUS_DCI_C.WriteLine ("    " & lower_case_file_name & "_dcid_to_mbus        //const DCI_MBUS_FROM_DCID_LKUP_ST_TD* dcid_to_mbus")
    MODBUS_DCI_C.WriteLine ("};")
    MODBUS_DCI_C.WriteLine ("")
    MODBUS_DCI_C.WriteLine ("")

    MODBUS_DCI_C.Close
    MODBUS_DCI_H.Close

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

Public Sub Verify_Modbus_List(Optional fill_in_ignored As Boolean = True)

'*********************
'******     Constants
'*********************
    Application.Calculation = xlCalculationManual
    Call Modbus_Restore_Cell_Color
    
    Dim sheet_name As String

    DATA_DESC_MISSING_COL = MODBUS_DESCRIP_COL
    DATA_DEFINE_MISSING_COL = MODBUS_DATA_DEFINE_COL
    DATA_LENGTH_MISSING_COL = MODBUS_DATA_LENGTH_COL
    
'*********************
'******     Begin
'*********************
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select

'****************************************************************************************************************************
'******     Find where we should add the unadded items.
'****************************************************************************************************************************
    Dim end_row As Integer
    
    end_row = Last_Modbus_Row(Worksheets(sheet_name))
    Set c = Worksheets(sheet_name).Range( _
        Cells(MODBUS_DATA_START_ROW, MODBUS_DATA_DEFINE_COL), _
        Cells(end_row, MODBUS_DATA_DEFINE_COL)).Find(BEGIN_IGNORED_DATA, LookAt:=xlWhole)
    If c Is Nothing Then
        MsgBox ("Error: Can't Find BEGIN_IGNORED_DATA!")
    End If
    missing_row_counter = c.row + 1 'Get the cell after the begin ignored here.

    fill_in_ignored = False
    
    If (fill_in_ignored = True) Then
'****************************************************************************************************************************
'******     Find the missing modbus data items
'****************************************************************************************************************************
        ' Clear the place where we are going to put the Modbus Data which is missing.
        Worksheets(sheet_name).Range(Cells(missing_row_counter, MODBUS_DESCRIP_COL), Cells(end_row + 1, MODBUS_USER_DOC_IGNORE_COL)).Delete (xlShiftUp)
        dci_defines_row_ctr = DCI_DEFINES_START_ROW
        While Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> Empty
            lookup_string = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text
            Set c = Worksheets(sheet_name).Range( _
                Cells(MODBUS_DATA_START_ROW, MODBUS_DATA_DEFINE_COL), _
                Cells(end_row, MODBUS_DATA_DEFINE_COL)).Find(lookup_string, LookAt:=xlWhole)
            If c Is Nothing Then
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
    End If

    Sheets(sheet_name).Select
'****************************************************************************************************************************
'******     Find the Data items that have been removed from the main list
'******     This will go through the main list and make the lengths correct in the slave sheets
'****************************************************************************************************************************
    modbus_row_ctr = MODBUS_DATA_START_ROW
    dci_end_row = Last_DCI_Row()
    While Cells(modbus_row_ctr, MODBUS_DATA_DEFINE_COL).text <> Empty
        lookup_string = Cells(modbus_row_ctr, MODBUS_DATA_DEFINE_COL).text
        If lookup_string <> BEGIN_IGNORED_DATA Then
            Set c = Find_DCID_Cell(lookup_string)
            If c Is Nothing Then
                Cells(modbus_row_ctr, MODBUS_DATA_DEFINE_COL).Interior.Color = RGB(255, 0, 0)
                Cells(modbus_row_ctr, MODBUS_DESCRIP_COL).Interior.Color = RGB(255, 0, 0)
            Else    ' If found then we go through and fix the length and the descriptions.
                Cells(modbus_row_ctr, MODBUS_DESCRIP_COL).value = c.offset(0, DCI_DESCRIPTORS_COL - DCI_DEFINES_COL).text
                Cells(modbus_row_ctr, MODBUS_DATA_LENGTH_COL).value = c.offset(0, DCI_LENGTH_COL - DCI_DEFINES_COL).text
            End If
        End If
        modbus_row_ctr = modbus_row_ctr + 1
    Wend

    Application.Calculation = xlCalculationAutomatic
End Sub


'****************************************************************************************************************************
'
'   Generates the Modbus Register Documentation.
'   Note: generate_all parameter is historical vestage that can be omitted
'   (but corresponding changes must be made to ButtonLinks and MacroRunner in all product workbooks).
'
'****************************************************************************************************************************

Sub Create_Modbus_Documentation(Optional generate_all As Boolean = False)
    If generate_all = False Then
        Modbus_HTML_PUBLIC_Documentation
        Modbus_HTML_INTERNAL_Documentation
        Create_Modbus_LUA_Definitions
    End If
End Sub


'****************************************************************************************************************************
'******     Will return the correct suffix for a const value.
'****************************************************************************************************************************
Public Function Last_Modbus_Row(Workbook) As Integer

    Last_Modbus_Row = Workbook.Cells(MODBUS_DATA_START_ROW, MODBUS_DATA_DEFINE_COL).End(xlDown).row

End Function

'****************************************************************************************************************************
'******     Will return the correct suffix for a const value.
'****************************************************************************************************************************
Public Function Begin_Ignored_Row(Workbook) As Integer

    end_row = Last_Modbus_Row(Workbook)
    Set c = Workbook.Range( _
        Cells(MODBUS_DATA_START_ROW, MODBUS_DATA_DEFINE_COL), _
        Cells(end_row, MODBUS_DATA_DEFINE_COL)).Find(BEGIN_IGNORED_DATA, LookAt:=xlWhole)
    If c Is Nothing Then
        MsgBox ("Error: Can't Find BEGIN_IGNORED_DATA!")
    End If
    Begin_Ignored_Row = c.row 'Get the cell after the begin ignored here.

End Function

'****************************************************************************************************************************
'
'   Restores interior color of cells to normal.
'
'****************************************************************************************************************************

Sub Modbus_Restore_Cell_Color()
    Dim row As Long
    row = MODBUS_DATA_START_ROW
    While (Cells(row, MODBUS_DATA_DEFINE_COL).text <> Empty) And (Cells(row, MODBUS_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        Rows(row).Interior.ColorIndex = 0
        row = row + 1
    Wend
End Sub

