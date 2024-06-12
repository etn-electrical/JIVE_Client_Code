Attribute VB_Name = "EtherNetIP"
'**************************
'**************************
'Ethernet Construction sheet.
'**************************
'**************************
Type MY_HTML_COL_DEF
    width As String
    align As String
    valign As String
End Type

'***************************************************************
'Sheet File Indexing.
Private Const START_ROW = 9
Private Const DESCRIP_COL = 1
Private Const DATA_DEFINE_COL = 2
Private Const DATA_CLASS_COL = 3
Private Const DATA_INSTANCE_COL = 4
Private Const DATA_ATTRIBUTE_COL = 5
Private Const DATA_INDEX_COL = 6
Private Const DATA_ALTERNATE_LEN_COL = 7
Private Const DATA_CIA_SORT_COL = 8
Private Const DATA_BITWISE_BOOL_COL = 27
Private Const DATA_CIP_SHORT_STRING_COL = 28
Private Const DATA_CLASS_REVISION_COL = 53
Private Const DATA_ASSEMBLY_PRODUCED_COL = 79
Private Const DATA_ASSEMBLY_ITEM_INDEX_COL = 80
Private Const DATA_ASSEMBLY_ALIGNMENT_COL = 81
Private Const DATA_CIP_DEVICE_TYPE_ROW = 1
Private Const DATA_CIP_DEVICE_TYPE_COL = 31
Private Const DATA_CLASS_NAME_COL = 105
Private Const DATA_UNPUBLISHED_COL = 106
Private Const DATA_ALT_SHORT_NAME_COL = 131
Private Const DATA_ALT_DESCRIPTION_COL = 132
Private Const DATA_ALT_DATATYPE_COL = 133

Private Const DATA_DESC_MISSING_COL = DESCRIP_COL
Private Const DATA_DEFINE_MISSING_COL = DATA_DEFINE_COL


'***************************************************************
'***************************************************************

'-----#k:-------------START-------- Added for Dynamic Assembly--------------------------------------------------------------
'-----#k: New definitions---------------------------
Public Const DYNAMIC_ASSEMBLY_START_ROW = 9
Private Const IN_D_ASSEMBLY_DEFINE_COL = "EF:EF" 'Columns("EF:EF").Column
Private Const IN_D_ASSEMBLY_VALUE_COL = "EG:EG"

Private Const OUT_D_ASSEMBLY_DEFINE_COL = "EI:EI"
Private Const OUT_D_ASSEMBLY_VALUE_COL = "EJ:EJ"
    


Sub Create_EtherNetIP_DynamicAssembly_DCI_Xfer()
        
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select

    file_name = Cells(2, Columns("EF:EF").column).value
    
    lower_case_file_name = LCase(file_name)
    upper_case_file_name = UCase(file_name)
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\" & Cells(3, Columns("EF:EF").column).value
    Set DCI_FILE_C = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(4, Columns("EF:EF").column).value
    Set DCI_FILE_H = fs.CreateTextFile(file_path, True)
    
    'Making font type and size consistent.
    Worksheets(sheet_name).Range("A:EJ").Font.name = "Arial"
    Worksheets(sheet_name).Range("A:EJ").Font.Size = 10

'****************************************************************************************************************************
'******     Start Creating the .H file header
'****************************************************************************************************************************
    DCI_FILE_H.WriteLine ("/**")
    DCI_FILE_H.WriteLine ("*****************************************************************************************")
    DCI_FILE_H.WriteLine ("*")
    DCI_FILE_H.WriteLine ("*   @file")
    DCI_FILE_H.WriteLine ("*")
    DCI_FILE_H.WriteLine ("*   @brief")
    DCI_FILE_H.WriteLine ("*")
    DCI_FILE_H.WriteLine ("*   @details")
    DCI_FILE_H.WriteLine ("*")
    DCI_FILE_H.WriteLine ("*   @version")
    DCI_FILE_H.WriteLine ("*   C++ Style Guide Version 1.0")
    DCI_FILE_H.WriteLine ("*")
    DCI_FILE_H.WriteLine ("*   @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    DCI_FILE_H.WriteLine ("*")
    DCI_FILE_H.WriteLine ("*****************************************************************************************")
    DCI_FILE_H.WriteLine ("*/")
    DCI_FILE_H.WriteLine ("#ifndef " & upper_case_file_name & "_H")
    DCI_FILE_H.WriteLine ("  #define " & upper_case_file_name & "_H")
    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("#include ""DCI.h""")
        DCI_FILE_H.WriteLine ("#include ""DCI_Owner.h""")
    DCI_FILE_H.WriteLine ("")

    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("//*******************************************************")
    DCI_FILE_H.WriteLine ("//******Dynamic Assembly structure and size follow.")
    DCI_FILE_H.WriteLine ("//*******************************************************")
    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("typedef uint16_t DYNAMIC_ASSEMBLY_PID_TD;")
    DCI_FILE_H.WriteLine ("typedef uint16_t dynamic_assembly_pid_t;")
    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("")
    '**************** Dynamic i/p Assembly*******************************************
    DCI_FILE_H.WriteLine ("typedef struct DCI_D_IN_ASM_TO_DCID_LKUP_ST_TD")
    DCI_FILE_H.WriteLine ("{")
    DCI_FILE_H.WriteLine ("     DYNAMIC_ASSEMBLY_PID_TD assembly_id;")
    DCI_FILE_H.WriteLine ("     DCI_ID_TD dcid;")
    DCI_FILE_H.WriteLine ("} DCI_D_IN_ASM_TO_DCID_LKUP_ST_TD;")
    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("")
    
    DCI_FILE_H.WriteLine ("typedef struct DCI_DCID_TO_D_IN_ASM_LKUP_ST_TD")
    DCI_FILE_H.WriteLine ("{")
    DCI_FILE_H.WriteLine ("     DCI_ID_TD dcid;")
    DCI_FILE_H.WriteLine ("     DYNAMIC_ASSEMBLY_PID_TD assembly_id;")
    DCI_FILE_H.WriteLine ("} DCI_DCID_TO_D_IN_ASM_LKUP_ST_TD;")
    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("")
    
    DCI_FILE_H.WriteLine ("typedef struct D_IN_ASM_TARGET_INFO_ST_TD")
    DCI_FILE_H.WriteLine ("{")
    DCI_FILE_H.WriteLine ("     DYNAMIC_ASSEMBLY_PID_TD total_d_in_assemblies;")
    DCI_FILE_H.WriteLine ("     const DCI_D_IN_ASM_TO_DCID_LKUP_ST_TD* asmin_to_dcid;")
    DCI_FILE_H.WriteLine ("     DCI_ID_TD total_dcids;")
    DCI_FILE_H.WriteLine ("     const DCI_DCID_TO_D_IN_ASM_LKUP_ST_TD* dcid_to_asmin;")
    DCI_FILE_H.WriteLine ("} D_IN_ASM_TARGET_INFO_ST_TD;")
    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("")
    
'    '**************** Dynamic o/p Assembly*******************************************
     DCI_FILE_H.WriteLine ("typedef struct DCI_D_OUT_ASM_TO_DCID_LKUP_ST_TD")
    DCI_FILE_H.WriteLine ("{")
    DCI_FILE_H.WriteLine ("     DYNAMIC_ASSEMBLY_PID_TD assembly_id;")
    DCI_FILE_H.WriteLine ("     DCI_ID_TD dcid;")
    DCI_FILE_H.WriteLine ("} DCI_D_OUT_ASM_TO_DCID_LKUP_ST_TD;")
    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("")

    DCI_FILE_H.WriteLine ("typedef struct DCI_DCID_TO_D_OUT_ASM_LKUP_ST_TD")
    DCI_FILE_H.WriteLine ("{")
    DCI_FILE_H.WriteLine ("     DCI_ID_TD dcid;")
    DCI_FILE_H.WriteLine ("     DYNAMIC_ASSEMBLY_PID_TD assembly_id;")
    DCI_FILE_H.WriteLine ("} DCI_DCID_TO_D_OUT_ASM_LKUP_ST_TD;")
    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("")

    DCI_FILE_H.WriteLine ("typedef struct D_OUT_ASM_TARGET_INFO_ST_TD")
    DCI_FILE_H.WriteLine ("{")
    DCI_FILE_H.WriteLine ("     DYNAMIC_ASSEMBLY_PID_TD total_d_in_assemblies;")
    DCI_FILE_H.WriteLine ("     const DCI_D_OUT_ASM_TO_DCID_LKUP_ST_TD* asmout_to_dcid;")
    DCI_FILE_H.WriteLine ("     DCI_ID_TD total_dcids;")
    DCI_FILE_H.WriteLine ("     const DCI_DCID_TO_D_OUT_ASM_LKUP_ST_TD* dcid_to_asmin;")
    DCI_FILE_H.WriteLine ("} D_OUT_ASM_TARGET_INFO_ST_TD;")
    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("")
    
    '**************** Dynamic i/p Assembly *******************************************
    DCI_FILE_H.WriteLine ("extern const D_IN_ASM_TARGET_INFO_ST_TD d_in_asm_dci_data_target_info;")
    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("static const DCI_D_IN_ASM_TO_DCID_LKUP_ST_TD* const LOOKUP_D_IN_ASM_TO_DCI_NOT_FOUND=NULL;")
    DCI_FILE_H.WriteLine ("static const DCI_DCID_TO_D_IN_ASM_LKUP_ST_TD* const LOOKUP_DCI_TO_D_IN_ASM_NOT_FOUND=NULL;")
    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("")

'    '**************** Dynamic o/p Assembly *******************************************
    DCI_FILE_H.WriteLine ("extern const D_OUT_ASM_TARGET_INFO_ST_TD d_out_asm_dci_data_target_info;")
    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("static const DCI_D_OUT_ASM_TO_DCID_LKUP_ST_TD* const LOOKUP_D_OUT_ASM_TO_DCI_NOT_FOUND=NULL;")
    DCI_FILE_H.WriteLine ("static const DCI_DCID_TO_D_OUT_ASM_LKUP_ST_TD* const LOOKUP_DCI_TO_D_OUT_ASM_NOT_FOUND=NULL;")
        DCI_FILE_H.WriteLine ("")
        assembly_number_of_ip = 0
        assembly_number_of_op = 0
        row_counter = START_ROW
        While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
                If ((Cells(row_counter, DATA_CLASS_NAME_COL).text <> Empty) And (Cells(row_counter, DATA_CLASS_NAME_COL).text = "Dynamic Output Assembly")) Then
                If ((Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).text <> Empty) And (Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).text <> 0)) Then
                assembly_number_of_op = assembly_number_of_op + 1
                End If
                End If
                row_counter = row_counter + 1
        Wend
        
        row_counter = START_ROW
        While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
                If ((Cells(row_counter, DATA_CLASS_NAME_COL).text <> Empty) And (Cells(row_counter, DATA_CLASS_NAME_COL).text = "Dynamic Input Assembly")) Then
                If ((Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).text <> Empty) And (Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).text <> 0)) Then
                assembly_number_of_ip = assembly_number_of_ip + 1
                End If
                End If
                row_counter = row_counter + 1
        Wend
        
        DCI_FILE_H.WriteLine ("static const uint8_t D_IN_ASM_MAX =   " & assembly_number_of_ip & "U;")
        DCI_FILE_H.WriteLine ("static const uint8_t D_OUT_ASM_MAX =   " & assembly_number_of_op & "U;")
        
        
        DCI_FILE_H.WriteLine ("void init_dyn_input_asm_dcids (DCI_ID_TD * dyn_in_asm_dcid, DCI_Owner * m_d_in_Asm_Selection[]);")
        DCI_FILE_H.WriteLine ("void init_dyn_output_asm_dcids(DCI_ID_TD * dyn_out_asm_dcid, DCI_Owner * m_d_out_Asm_Selection[]);")

'   '**************** End of File *******************************************
    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("#endif")

'****************************************************************************************************************************
'******     Start Creating the .C file header
'****************************************************************************************************************************
    DCI_FILE_C.WriteLine ("/**")
    DCI_FILE_C.WriteLine ("*****************************************************************************************")
    DCI_FILE_C.WriteLine ("*   @file")
    DCI_FILE_C.WriteLine ("*   @details See header file for module overview.")
    DCI_FILE_C.WriteLine ("*   @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    DCI_FILE_C.WriteLine ("*")
    DCI_FILE_C.WriteLine ("*****************************************************************************************")
    DCI_FILE_C.WriteLine ("*/")
    DCI_FILE_C.WriteLine ("#include ""Includes.h""")
    DCI_FILE_C.WriteLine ("#include """ & file_name & ".h""")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("")
    

'****************************************************************************************************************************
'******     Create the Dynamic input assembly Count.
'****************************************************************************************************************************

    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("//******  Dynamic input Assembly List")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("")
    row_counter = DYNAMIC_ASSEMBLY_START_ROW
    in_assembly_counter = 0
     While (Cells(row_counter, Columns(IN_D_ASSEMBLY_DEFINE_COL).column).text <> Empty) And (Cells(row_counter, Columns(IN_D_ASSEMBLY_DEFINE_COL).column).text <> "BEGIN_IGNORED_DATA")
        DCI_FILE_C.WriteLine ("static const dynamic_assembly_pid_t " & Cells(row_counter, Columns(IN_D_ASSEMBLY_DEFINE_COL).column).text & "_IN_DASM_ID = " & _
                                Cells(row_counter, Columns(IN_D_ASSEMBLY_VALUE_COL).column).text & "U;")
        row_counter = row_counter + 1
        in_assembly_counter = in_assembly_counter + 1
    Wend
    
'******   Total i/p Assemblies  ******************************************************
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("//****** The Dynamic i/p Assembly Counts.")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("static const dynamic_assembly_pid_t DYNAMIC_IN_ASSEMBLY_DATA_TOTAL_IDS = " & in_assembly_counter & "U;")
    DCI_FILE_C.WriteLine ("static DCI_ID_TD DYNAMIC_IN_ASSEMBLY_DCI_DATA_TOTAL_DCIDS = " & in_assembly_counter & "U;")
    DCI_FILE_C.WriteLine ("")

'****************************************************************************************************************************
'******     Create the Dynamic output assembly Count.
'****************************************************************************************************************************

   DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("//****** Dynamic output Assembly List")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("")
    row_counter = DYNAMIC_ASSEMBLY_START_ROW
    out_assembly_counter = 0
     While (Cells(row_counter, Columns(OUT_D_ASSEMBLY_DEFINE_COL).column).text <> Empty) And (Cells(row_counter, Columns(OUT_D_ASSEMBLY_DEFINE_COL).column).text <> "BEGIN_IGNORED_DATA")
        DCI_FILE_C.WriteLine ("static const dynamic_assembly_pid_t " & Cells(row_counter, Columns(OUT_D_ASSEMBLY_DEFINE_COL).column).text & "_OUT_DASM_ID = " & _
                                Cells(row_counter, Columns(OUT_D_ASSEMBLY_VALUE_COL).column).text & "U;")
        row_counter = row_counter + 1
        out_assembly_counter = out_assembly_counter + 1
    Wend

'******   Total o/p Assemblies  ******************************************************
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("//****** The Dynamic o/p Assembly Counts.")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("static const dynamic_assembly_pid_t DYNAMIC_OUT_ASSEMBLY_DATA_TOTAL_IDS = " & out_assembly_counter & "U;")
    DCI_FILE_C.WriteLine ("static DCI_ID_TD DYNAMIC_OUT_ASSEMBLY_DCI_DATA_TOTAL_DCIDS = " & out_assembly_counter & "U;")
    DCI_FILE_C.WriteLine ("")


'****************************************************************************************************************************
'******     Generate the straight Dynamic i/p Assembly to DCI lookup table.
'****************************************************************************************************************************
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("//****    The Dynamic i/p Assembly Cross Reference Table.")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("")
        
    row_counter = DYNAMIC_ASSEMBLY_START_ROW
    DCI_FILE_C.WriteLine ("static const DCI_D_IN_ASM_TO_DCID_LKUP_ST_TD dynamic_in_assem_dci_data_id_to_dcid[DYNAMIC_IN_ASSEMBLY_DATA_TOTAL_IDS] = ")
    DCI_FILE_C.WriteLine ("{")
    While (Cells(row_counter, IN_D_ASSEMBLY_DEFINE_COL).text <> Empty) And (Cells(row_counter, IN_D_ASSEMBLY_DEFINE_COL).text <> "BEGIN_IGNORED_DATA")
        
        DCI_FILE_C.WriteLine ("    { ")
        DCI_FILE_C.WriteLine ("        " & Cells(row_counter, Columns(IN_D_ASSEMBLY_DEFINE_COL).column).text & "_IN_DASM_ID,")
        DCI_FILE_C.WriteLine ("        " & Cells(row_counter, Columns(IN_D_ASSEMBLY_DEFINE_COL).column).text & "_DCID")

            If (Cells(row_counter + 1, Columns(IN_D_ASSEMBLY_DEFINE_COL).column).text = Empty) And (Cells(row_counter, IN_D_ASSEMBLY_DEFINE_COL).text <> "BEGIN_IGNORED_DATA") Then
                DCI_FILE_C.WriteLine ("    } ")
            Else
                DCI_FILE_C.WriteLine ("    }, ")
            End If

        row_counter = row_counter + 1
    Wend
    DCI_FILE_C.WriteLine ("};")
'-------------------------------------------------------------------------------

    row_counter = DYNAMIC_ASSEMBLY_START_ROW
    DCI_FILE_C.WriteLine ("const DCI_DCID_TO_D_IN_ASM_LKUP_ST_TD dynamic_in_asm_dci_data_dcid_to_id[DYNAMIC_IN_ASSEMBLY_DATA_TOTAL_IDS] = ")
    DCI_FILE_C.WriteLine ("{")
    While (Cells(row_counter, IN_D_ASSEMBLY_DEFINE_COL).text <> Empty) And (Cells(row_counter, IN_D_ASSEMBLY_DEFINE_COL).text <> "BEGIN_IGNORED_DATA")
        
        DCI_FILE_C.WriteLine ("    { ")
        DCI_FILE_C.WriteLine ("        " & Cells(row_counter, Columns(IN_D_ASSEMBLY_DEFINE_COL).column).text & "_DCID,")
        DCI_FILE_C.WriteLine ("        " & Cells(row_counter, Columns(IN_D_ASSEMBLY_DEFINE_COL).column).text & "_IN_DASM_ID")
        
            If (Cells(row_counter + 1, Columns(IN_D_ASSEMBLY_DEFINE_COL).column).text = Empty) And (Cells(row_counter, IN_D_ASSEMBLY_DEFINE_COL).text <> "BEGIN_IGNORED_DATA") Then
                DCI_FILE_C.WriteLine ("    } ")
            Else
                DCI_FILE_C.WriteLine ("    }, ")
            End If

        row_counter = row_counter + 1
    Wend
    DCI_FILE_C.WriteLine ("};")

    DCI_FILE_C.WriteLine ("")
    
'****************************************************************************************************************************
'******     Generate the straight Dynamic o/p Assembly to DCI lookup table.
'****************************************************************************************************************************

    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("//****    The Dynamic o/p Assembly Cross Reference Table.")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("")
    row_counter = DYNAMIC_ASSEMBLY_START_ROW

    DCI_FILE_C.WriteLine ("static const DCI_D_OUT_ASM_TO_DCID_LKUP_ST_TD dynamic_out_assem_dci_data_id_to_dcid[DYNAMIC_OUT_ASSEMBLY_DATA_TOTAL_IDS] = ")
    DCI_FILE_C.WriteLine ("{")
    While (Cells(row_counter, OUT_D_ASSEMBLY_DEFINE_COL).text <> Empty) And (Cells(row_counter, OUT_D_ASSEMBLY_DEFINE_COL).text <> "BEGIN_IGNORED_DATA")

        DCI_FILE_C.WriteLine ("    { ")
        DCI_FILE_C.WriteLine ("        " & Cells(row_counter, Columns(OUT_D_ASSEMBLY_DEFINE_COL).column).text & "_OUT_DASM_ID,")
        DCI_FILE_C.WriteLine ("        " & Cells(row_counter, Columns(OUT_D_ASSEMBLY_DEFINE_COL).column).text & "_DCID")

            If (Cells(row_counter + 1, Columns(OUT_D_ASSEMBLY_DEFINE_COL).column).text = Empty) And (Cells(row_counter, OUT_D_ASSEMBLY_DEFINE_COL).text <> "BEGIN_IGNORED_DATA") Then
                DCI_FILE_C.WriteLine ("    } ")
            Else
                DCI_FILE_C.WriteLine ("    }, ")
            End If

        row_counter = row_counter + 1
    Wend
    DCI_FILE_C.WriteLine ("};")
'-------------------------------------------------------------------------------

    row_counter = DYNAMIC_ASSEMBLY_START_ROW
    DCI_FILE_C.WriteLine ("const DCI_DCID_TO_D_OUT_ASM_LKUP_ST_TD dynamic_out_asm_dci_data_dcid_to_id[DYNAMIC_OUT_ASSEMBLY_DATA_TOTAL_IDS] = ")
    DCI_FILE_C.WriteLine ("{")
    While (Cells(row_counter, OUT_D_ASSEMBLY_DEFINE_COL).text <> Empty) And (Cells(row_counter, OUT_D_ASSEMBLY_DEFINE_COL).text <> "BEGIN_IGNORED_DATA")

        DCI_FILE_C.WriteLine ("    { ")
        DCI_FILE_C.WriteLine ("        " & Cells(row_counter, Columns(OUT_D_ASSEMBLY_DEFINE_COL).column).text & "_DCID,")
        DCI_FILE_C.WriteLine ("        " & Cells(row_counter, Columns(OUT_D_ASSEMBLY_DEFINE_COL).column).text & "_OUT_DASM_ID")

            If (Cells(row_counter + 1, Columns(OUT_D_ASSEMBLY_DEFINE_COL).column).text = Empty) And (Cells(row_counter, OUT_D_ASSEMBLY_DEFINE_COL).text <> "BEGIN_IGNORED_DATA") Then
                DCI_FILE_C.WriteLine ("    } ")
            Else
                DCI_FILE_C.WriteLine ("    }, ")
            End If

        row_counter = row_counter + 1
    Wend
    DCI_FILE_C.WriteLine ("};")

    DCI_FILE_C.WriteLine ("")
    
'****************************************************************************************************************************
'******     Create the i/p Assembly Structure.
'****************************************************************************************************************************
     DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("//** The Dynamic i/p Assembly Target Definition Structure.")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("const D_IN_ASM_TARGET_INFO_ST_TD  d_in_asm_dci_data_target_info = ")
    DCI_FILE_C.WriteLine ("{")
    DCI_FILE_C.WriteLine ("    DYNAMIC_IN_ASSEMBLY_DATA_TOTAL_IDS,        //DYNAMIC_ASSEMBLY_PID_TD total_d_in_assemblies")
    DCI_FILE_C.WriteLine ("    dynamic_in_assem_dci_data_id_to_dcid,      //const DCI_D_IN_ASM_TO_DCID_LKUP_ST_TD* asmin_to_dcid")
    DCI_FILE_C.WriteLine ("    DYNAMIC_IN_ASSEMBLY_DCI_DATA_TOTAL_DCIDS,  //DCI_ID_TD total_dcids")
    DCI_FILE_C.WriteLine ("    dynamic_in_asm_dci_data_dcid_to_id       //const DCI_DCID_TO_D_IN_ASM_LKUP_ST_TD* dcid_to_asmin")
    DCI_FILE_C.WriteLine ("};")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("")

'****************************************************************************************************************************
'******     Create the o/p Assembly Structure.
'****************************************************************************************************************************
     DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("//** The Dynamic o/p Assembly Target Definition Structure.")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("const D_OUT_ASM_TARGET_INFO_ST_TD  d_out_asm_dci_data_target_info = ")
    DCI_FILE_C.WriteLine ("{")
    DCI_FILE_C.WriteLine ("    DYNAMIC_OUT_ASSEMBLY_DATA_TOTAL_IDS,        //DYNAMIC_ASSEMBLY_PID_TD total_d_out_assemblies")
    DCI_FILE_C.WriteLine ("    dynamic_out_assem_dci_data_id_to_dcid,      //const DCI_D_OUT_ASM_TO_DCID_LKUP_ST_TD* asmout_to_dcid")
    DCI_FILE_C.WriteLine ("    DYNAMIC_OUT_ASSEMBLY_DCI_DATA_TOTAL_DCIDS,  //DCI_ID_TD total_dcids")
    DCI_FILE_C.WriteLine ("    dynamic_out_asm_dci_data_dcid_to_id       //const DCI_DCID_TO_D_IN_ASM_LKUP_ST_TD* dcid_to_asmout")
    DCI_FILE_C.WriteLine ("};")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("")

'****************************************************************************************************************************
'******     Init Dynamic Input ASM IDs
'****************************************************************************************************************************
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("//** Init Dynamic i/p Assembly IDs.")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("void init_dyn_input_asm_dcids( DCI_ID_TD * dyn_in_asm_dcid, DCI_Owner * m_d_in_Asm_Selection[])")
    DCI_FILE_C.WriteLine ("{")
    DCI_FILE_C.WriteLine ("    uint8_t temp_uint8 = 0U;")
    DCI_FILE_C.WriteLine ("")
        
        Dim asm_index As Variant
        asm_index = 0
        row_counter = START_ROW
        my_counter = 0
        instance = 0
        my_flag = 0
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
                my_flag = 0
                If ((Cells(row_counter, DATA_CLASS_NAME_COL).text <> Empty) And (Cells(row_counter, DATA_CLASS_NAME_COL).text = "Dynamic Input Assembly")) Then
                If ((Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).text <> Empty) And (Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).text <> 0)) Then
                DCI_FILE_C.WriteLine ("    new DCI_Owner( " & Cells(row_counter, DATA_DEFINE_COL).text & "_DCID );")
                asm_index = Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).value
                instance = Cells(row_counter, DATA_INSTANCE_COL).value
                my_flag = 1
                row_counter1 = START_ROW
                While (Cells(row_counter1, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter1, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
                If ((Cells(row_counter1, DATA_CLASS_NAME_COL).text <> Empty) And (Cells(row_counter1, DATA_CLASS_NAME_COL).text = "Dynamic Input Assembly Interface")) Then
                If ((Cells(row_counter1, DATA_ATTRIBUTE_COL).text <> Empty) And (Cells(row_counter1, DATA_ATTRIBUTE_COL).value = asm_index) And (Cells(row_counter1, DATA_CLASS_COL).value = instance)) Then
                DCI_FILE_C.WriteLine ("    dyn_in_asm_dcid[" & my_counter & "] = " & Cells(row_counter1, DATA_DEFINE_COL).text & "_DCID;")
                my_counter = my_counter + 1
                my_flag = 0
                End If
                End If
                row_counter1 = row_counter1 + 1
                Wend
                
                End If
                End If
                If (my_flag <> 0) Then
                MsgBox "Error In Input Assembly Data Generation. Please Check EIP Sheet"
                End If
        row_counter = row_counter + 1
    Wend
        
    DCI_FILE_C.WriteLine ("    for ( temp_uint8=0U; temp_uint8 < D_IN_ASM_MAX; temp_uint8++)")
    DCI_FILE_C.WriteLine ("    {")
    DCI_FILE_C.WriteLine ("        m_d_in_Asm_Selection[temp_uint8] = new DCI_Owner( dyn_in_asm_dcid[temp_uint8]);")
    DCI_FILE_C.WriteLine ("    }")
    DCI_FILE_C.WriteLine ("}")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("")
        
        
'****************************************************************************************************************************
'******     Init Dynamic Output ASM IDs
'****************************************************************************************************************************
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("//** Init Dynamic o/p Assembly IDs.")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("void init_dyn_output_asm_dcids( DCI_ID_TD * dyn_out_asm_dcid, DCI_Owner * m_d_out_Asm_Selection[])")
    DCI_FILE_C.WriteLine ("{")
    DCI_FILE_C.WriteLine ("    uint8_t temp_uint8 = 0U;")
    DCI_FILE_C.WriteLine ("")
        
        'Dim asm_index As Variant
        asm_index = 0
        instance = 0
        row_counter = START_ROW
        my_counter = 0
        my_flag = 0

    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
                my_flag = 0
                If ((Cells(row_counter, DATA_CLASS_NAME_COL).text <> Empty) And (Cells(row_counter, DATA_CLASS_NAME_COL).text = "Dynamic Output Assembly")) Then
                If ((Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).text <> Empty) And (Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).text <> 0)) Then
                DCI_FILE_C.WriteLine ("    new DCI_Owner( " & Cells(row_counter, DATA_DEFINE_COL).text & "_DCID );")
                asm_index = Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).value
                instance = Cells(row_counter, DATA_INSTANCE_COL).value
                my_flag = 1

                row_counter1 = START_ROW
                While (Cells(row_counter1, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter1, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
                If ((Cells(row_counter1, DATA_CLASS_NAME_COL).text <> Empty) And (Cells(row_counter1, DATA_CLASS_NAME_COL).text = "Dynamic Output Assembly Interface")) Then
                If ((Cells(row_counter1, DATA_ATTRIBUTE_COL).text <> Empty) And (Cells(row_counter1, DATA_ATTRIBUTE_COL).value = asm_index) And (Cells(row_counter1, DATA_CLASS_COL).value = instance)) Then
                DCI_FILE_C.WriteLine ("    dyn_out_asm_dcid[" & my_counter & "] = " & Cells(row_counter1, DATA_DEFINE_COL).text & "_DCID;")
                my_counter = my_counter + 1
                my_flag = 0
                End If
                End If
                row_counter1 = row_counter1 + 1
                Wend
                
                End If
                End If
                If (my_flag <> 0) Then
                MsgBox "Error In Output Assembly Data Generation. Please Check EIP Sheet"
                End If
        row_counter = row_counter + 1
    Wend
        
    DCI_FILE_C.WriteLine ("    for ( temp_uint8=0U; temp_uint8 < D_OUT_ASM_MAX; temp_uint8++)")
    DCI_FILE_C.WriteLine ("    {")
    DCI_FILE_C.WriteLine ("        m_d_out_Asm_Selection[temp_uint8] = new DCI_Owner( dyn_out_asm_dcid[temp_uint8]);")
    DCI_FILE_C.WriteLine ("    }")
    DCI_FILE_C.WriteLine ("}")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("")

'****************************************************************************************************************************
'******     Close the files.
'****************************************************************************************************************************

    DCI_FILE_C.Close
    DCI_FILE_H.Close

'-----#k:-------------END-------- Added for Dynamic Assembly-------------------------------------------------------------


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
Sub Create_EtherNetIP_DCI_Xfer()

'*********************
'******     Constants
'*********************
    Call Create_EIP_Documentation(False)
    Call Create_EIP_Documentation(True)
    Call Clear_EIP_Ignored_Items

    Application.Calculation = xlCalculationManual
    
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    Dim sheet_name As String

    Dim DCI_STRING As String
    Dim num_unique_data As Integer
    Dim file_name As String
    Dim lower_case_file_name As String
    Dim cia_num As Long
    Dim special_str As String
    Dim cia_str As String
    Dim temp_string As String
    Dim string_print As String
    Dim base_string As String

    Dim class_byte_size As Integer
    Dim instance_byte_size As Integer
    Dim attribute_byte_size As Integer

    Dim present_class_num As Long   ' We will use this to track the current class number being worked on.
    Dim class_counter As Long       ' We will use this to track how many classes have been created.
    
    Dim present_instance_num As Long   ' We will use this to track the current class number being worked on.
    Dim instance_counter As Long
    Dim num_instances As Long
    Dim max_instance_num As Long
    Dim instance_0_rev As Long
    
    Dim present_attrib_num As Long   ' We will use this to track the current class number being worked on.
    Dim attrib_counter As Long       ' We will use this to track how many classes have been created.
    

    Dim NUM_ALIGN_POS As Integer
    NUM_ALIGN_POS = 65

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
    Set DCI_FILE_C = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(4, 2).value
    Set DCI_FILE_H = fs.CreateTextFile(file_path, True)
    
    'Making font type and size consistent.
    Worksheets(sheet_name).Range("A:EJ").Font.name = "Arial"
    Worksheets(sheet_name).Range("A:EJ").Font.Size = 10

'****************************************************************************************************************************
'******     Start Creating the .H file header
'****************************************************************************************************************************
    DCI_FILE_H.WriteLine ("/**")
    DCI_FILE_H.WriteLine ("*****************************************************************************************")
    DCI_FILE_H.WriteLine ("*")
    DCI_FILE_H.WriteLine ("*   @file")
    DCI_FILE_H.WriteLine ("*")
    DCI_FILE_H.WriteLine ("*   @brief")
    DCI_FILE_H.WriteLine ("*")
    DCI_FILE_H.WriteLine ("*   @details")
    DCI_FILE_H.WriteLine ("*")
    DCI_FILE_H.WriteLine ("*   @version")
    DCI_FILE_H.WriteLine ("*   C++ Style Guide Version 1.0")
    DCI_FILE_H.WriteLine ("*")
    DCI_FILE_H.WriteLine ("*   @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    DCI_FILE_H.WriteLine ("*")
    DCI_FILE_H.WriteLine ("*****************************************************************************************")
    DCI_FILE_H.WriteLine ("*/")
    DCI_FILE_H.WriteLine ("#ifndef " & upper_case_file_name & "_H")
    DCI_FILE_H.WriteLine ("  #define " & upper_case_file_name & "_H")
    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("#include ""DCI.h""")
    DCI_FILE_H.WriteLine ("#include ""EtherNetIP_DCI.h""")
    DCI_FILE_H.WriteLine ("")
    
'****************************************************************************************************************************
'******     Sort the list by first creating a sorting column.
'****************************************************************************************************************************
    'Rows("9:499").Select
    ' Sort by Class, Attribute then instance.
    
    Columns("H:H").Clear
    row_counter = START_ROW
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        Cells(row_counter, DATA_CIA_SORT_COL).value = Cells(row_counter, DATA_CLASS_COL).value * (2 ^ 32) + _
                    Cells(row_counter, DATA_INSTANCE_COL).value * 2 ^ 16 + _
                    Cells(row_counter, DATA_ATTRIBUTE_COL).value
        row_counter = row_counter + 1
    Wend

    Rows("9:1000").Sort Key1:=Range("H9"), Key2:=Range("CB9"), _
        Order1:=xlAscending, Order2:=xlAscending, _
        Header:=xlGuess, OrderCustom:=1, MatchCase:=False, Orientation:=xlTopToBottom

    Columns("H:H").Clear

'****************************************************************************************************************************
'******     Gotta calculate the total number of items we are going to create.
'****************************************************************************************************************************
    use_64bit_cia = False
    num_entries = 0
     
    class_byte_size = 1
    instance_byte_size = 1
    attribute_byte_size = 1
    row_counter = START_ROW
    class_counter = 0
    present_class_num = 0
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(row_counter, DATA_CLASS_COL).text >= 256) Then
            class_byte_size = 2
        End If
        
        If (Cells(row_counter, DATA_INSTANCE_COL).text >= 256) Then
            instance_byte_size = 2
        End If
        
        If (Cells(row_counter, DATA_ATTRIBUTE_COL).text >= 256) Then
            attribute_byte_size = 2
        End If
        If (present_class_num <> Cells(row_counter, DATA_CLASS_COL).value) Then
            present_class_num = Cells(row_counter, DATA_CLASS_COL).text
            class_counter = class_counter + 1
        End If
        num_entries = num_entries + 1
        row_counter = row_counter + 1
    Wend
    
    If ((class_is_16bit + instance_is_16bit + attribute_is_16bit) > 1) Then
        use_64bit_cia = True
    End If

'****************************************************************************************************************************
'******     Create the header and associated file structure.  Get ready to plop the data down.
'****************************************************************************************************************************
'    DCI_FILE_H.WriteLine ("//*******************************************************")
'    DCI_FILE_H.WriteLine ("//******     If the CIA goes into a 64bit range we")
'    DCI_FILE_H.WriteLine ("//******     switch our search to be 64bit compatible.")
'    DCI_FILE_H.WriteLine ("//*******************************************************")
'    DCI_FILE_H.WriteLine ("")
'    If (use_64bit_cia = True) Then
'        DCI_FILE_H.WriteLine ("#define CIP_TO_DCI_USE_64BIT_CIA         TRUE")
'    Else
'        DCI_FILE_H.WriteLine ("#define CIP_TO_DCI_USE_64BIT_CIA         FALSE")
'    End If
    DCI_FILE_H.WriteLine ("static const uint8_t CIP_TO_DCI_CLASS_BYTE_SIZE =     " & class_byte_size & "U;")
    DCI_FILE_H.WriteLine ("static const uint8_t CIP_TO_DCI_INSTANCE_BYTE_SIZE =  " & instance_byte_size & "U;")
    DCI_FILE_H.WriteLine ("static const uint8_t CIP_TO_DCI_ATTRIBUTE_BYTE_SIZE = " & attribute_byte_size & "U;")

    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("//*******************************************************")
    DCI_FILE_H.WriteLine ("//******     CIP structure and size follow.")
    DCI_FILE_H.WriteLine ("//*******************************************************")
    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("extern const EIP_TARGET_INFO_ST_TD " & lower_case_file_name & "_target_info;")
    '----------#k: New Start---------------------------------------------------------------
    DCI_FILE_H.WriteLine ("extern const DCI_ID_TD DCI_DYN_IN_ASM_INTERFACE_INST_0x96_ASM_DCID_LIST[8];")
    DCI_FILE_H.WriteLine ("extern const DCI_ID_TD DCI_DYN_OUT_ASM_INTERFACE_INST_0x97_ASM_DCID_LIST[4];")
    DCI_FILE_H.WriteLine ("/** Above lines are temporarily hardcoded for Dynamic Assembly.")
    DCI_FILE_H.WriteLine ("Hardcode will be removed later on.")
    DCI_FILE_H.WriteLine ("'DCI_DYN_IN_ASM_INTERFACE' & 'DCI_DYN_OUT_ASM_INTERFACE' are Headers")
    DCI_FILE_H.WriteLine ("for Dynamic Assembly instance 150 & 151.")
    DCI_FILE_H.WriteLine ("So Dynamic Assembly name & Length will require to change if in")
    DCI_FILE_H.WriteLine ("future these are changed in 'EtherNetIP' tab in Spreadsheet.")
    DCI_FILE_H.WriteLine ("*/")
    '----------#k: New End---------------------------------------------------------------
    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("")
    DCI_FILE_H.WriteLine ("#endif")



'****************************************************************************************************************************
'******     Start Creating the .C file header
'****************************************************************************************************************************
    DCI_FILE_C.WriteLine ("/**")
    DCI_FILE_C.WriteLine ("*****************************************************************************************")
    DCI_FILE_C.WriteLine ("*   @file")
    DCI_FILE_C.WriteLine ("*   @details See header file for module overview.")
    DCI_FILE_C.WriteLine ("*   @copyright " & Get_Year() & " Eaton Corporation. All Rights Reserved.")
    DCI_FILE_C.WriteLine ("*")
    DCI_FILE_C.WriteLine ("*****************************************************************************************")
    DCI_FILE_C.WriteLine ("*/")
    DCI_FILE_C.WriteLine ("#include ""Includes.h""")
    DCI_FILE_C.WriteLine ("#include """ & file_name & ".h""")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("")
    

'****************************************************************************************************************************
'******     Create the Class, Instances, and Attributes Count.  We spin through the list once to set the class information.
'****************************************************************************************************************************

    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("//******     Identity Object Device Type Definition")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("")
    string_print = "static const uint16_t " & upper_case_file_name & "_DEVICE_TYPE ="
    DCI_FILE_C.WriteLine (string_print & Create_Space(string_print, NUM_ALIGN_POS) & _
                Cells(DATA_CIP_DEVICE_TYPE_ROW, DATA_CIP_DEVICE_TYPE_COL).text & "U;")

    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("//******     Class, Instance, Attribute Item Counts")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("///Total number of classes.")
    string_print = "static const uint16_t " & upper_case_file_name & "_TOTAL_CLASSES="
    DCI_FILE_C.WriteLine (string_print & Create_Space(string_print, NUM_ALIGN_POS) & class_counter & "U;")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("///Total number of attributes and instances per class.")
    row_counter = START_ROW
    present_class_num = 0
    present_instance_num = 0
    instance_counter = 0
    attribute_counter = 0
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        present_class_num = Cells(row_counter, DATA_CLASS_COL).text
        If (present_instance_num <> Cells(row_counter, DATA_INSTANCE_COL).text) Then
            present_instance_num = Cells(row_counter, DATA_INSTANCE_COL).value
            instance_counter = instance_counter + 1
        End If
        If (instance_counter = 1) Then
            attribute_counter = attribute_counter + 1
        End If
        row_counter = row_counter + 1
        If (present_class_num <> Cells(row_counter, DATA_CLASS_COL).text) Then
            temp_string = "static const uint8_t " & upper_case_file_name & "_CLASS_0x" & _
                                Hexify(present_class_num, class_byte_size) & "_NUM_INSTANCES ="
            DCI_FILE_C.WriteLine (temp_string & Create_Space(temp_string, NUM_ALIGN_POS) & instance_counter & "U;")
            temp_string = "static const uint8_t " & upper_case_file_name & "_CLASS_0x" & _
                                Hexify(present_class_num, class_byte_size) & "_NUM_ATTRIBUTES ="
            DCI_FILE_C.WriteLine (temp_string & Create_Space(temp_string, NUM_ALIGN_POS) & attribute_counter & "U;")
            instance_counter = 0
            present_instance_num = 0
            attribute_counter = 0
        End If
    Wend
    DCI_FILE_C.WriteLine ("")
            

'****************************************************************************************************************************
'******     Generate the Attribute Lists first.
'****************************************************************************************************************************
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("//******     Define Attribute Structure Lists")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("")
    present_instance_num = 0
    present_class_num = 0
    row_counter = START_ROW
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
    If ((Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).value = 0) Or _
                (Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).value = Empty)) Then
        If (present_class_num <> Cells(row_counter, DATA_CLASS_COL).text) Or (present_instance_num <> Cells(row_counter, DATA_INSTANCE_COL).text) Then
            If (present_class_num <> 0) Then
                DCI_FILE_C.WriteLine ("};")
                DCI_FILE_C.WriteLine ("")
            End If
                present_class_num = Cells(row_counter, DATA_CLASS_COL).text
                present_instance_num = Cells(row_counter, DATA_INSTANCE_COL).text
                DCI_FILE_C.WriteLine ("const CIP_ATTRIB_ST_TD " & _
                        lower_case_file_name & "_C0x" & Hexify(present_class_num, class_byte_size) & _
                        "_INST_0x" & Hexify(present_instance_num, instance_byte_size) & _
                        "_attrib_list[" & upper_case_file_name & "_CLASS_0x" & Hexify(present_class_num, class_byte_size) & _
                        "_NUM_ATTRIBUTES] = ")
                DCI_FILE_C.WriteLine ("{")
        End If
        cia_str = "0x" & Hexify(Cells(row_counter, DATA_ATTRIBUTE_COL).text, attribute_byte_size)
        
        '********************************************************************
        ' This defines the Beginning of the structure and the DCID.
        DCI_FILE_C.WriteLine ("    {        // C: 0x" & Hexify(present_class_num, class_byte_size) & _
                        ", I: 0x" & Hexify(present_instance_num, instance_byte_size) & _
                        ", A: 0x" & Hexify(Cells(row_counter, DATA_ATTRIBUTE_COL).text, attribute_byte_size) & _
                        " - " & Cells(row_counter, DESCRIP_COL).text & " Attrib" & Space(4) & cia_str)
        DCI_FILE_C.WriteLine ("        " & cia_str & "U,")
        DCI_FILE_C.WriteLine ("        " & Cells(row_counter, DATA_DEFINE_COL).text & "_DCID,")
        
        '********************************************************************
        ' This defines the Index section (whether it is bitwise or bytewise.
        If (Cells(row_counter, DATA_INDEX_COL).text <> Empty) Then
            DCI_FILE_C.WriteLine ("        " & Cells(row_counter, DATA_INDEX_COL).text & "U,")
        Else
            DCI_FILE_C.WriteLine ("        0U,")
        End If
        
        '********************************************************************
        ' This defines the alternate length which may be required.
        If (Cells(row_counter, DATA_ALTERNATE_LEN_COL).text <> Empty) Then
            DCI_FILE_C.WriteLine ("        " & Cells(row_counter, DATA_ALTERNATE_LEN_COL).text & "U,")
        Else
            DCI_FILE_C.WriteLine ("        0U,")
        End If

        '********************************************************************
        ' This defines the bitwise structure associated with special behaviors.
        DCI_FILE_C.WriteLine ("        ( ")
        special_str = ""
        
        If (Cells(row_counter, DATA_INDEX_COL).text <> Empty) Then
            If Len(special_str) > 0 Then
                DCI_FILE_C.WriteLine ("            " & special_str & " |")
            End If
            special_str = "static_cast<uint8_t>(1U<<EIP_SPECIAL_USE_DATATYPE_LEN )"
        End If
        
        If (Cells(row_counter, DATA_BITWISE_BOOL_COL).text <> Empty) Then
            If Len(special_str) > 0 Then
                DCI_FILE_C.WriteLine ("            " & special_str & " |")
            End If
            special_str = "static_cast<uint8_t>(1U<<EIP_SPECIAL_BITWISE_TO_BOOL )"
        End If

        If (Cells(row_counter, DATA_CIP_SHORT_STRING_COL).text <> Empty) Then
            If Len(special_str) > 0 Then
                DCI_FILE_C.WriteLine ("            " & special_str & " |")
            End If
            special_str = "static_cast<uint8_t>(1U<<EIP_SPECIAL_SHORT_STRING_DTYPE )"
        End If

        If Len(special_str) > 0 Then
            DCI_FILE_C.WriteLine ("            " & special_str)
        Else
            DCI_FILE_C.WriteLine ("            0U")
        End If
        DCI_FILE_C.WriteLine ("        )")

        ' End the bitwise stuff and wrap up the single structure definition.
        '**********************************************************************
        
        If (Cells(row_counter + 1, DATA_DEFINE_COL).text = Empty) Or (Cells(row_counter + 1, DATA_DEFINE_COL).text = BEGIN_IGNORED_DATA) Then
            DCI_FILE_C.WriteLine ("    }")
        Else
            DCI_FILE_C.WriteLine ("    },")
        End If
    End If
        row_counter = row_counter + 1
    Wend
    DCI_FILE_C.WriteLine ("};")
    DCI_FILE_C.WriteLine ("")
        
    
    
'****************************************************************************************************************************
'******     Generate the Instance Lists.
'****************************************************************************************************************************
 
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("//******     Define Instance Structure Lists")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("")
    present_class_num = 0
    present_instance_num = 0
    row_counter = START_ROW
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (present_class_num <> Cells(row_counter, DATA_CLASS_COL).text) Then
            If (present_class_num <> 0) Then
                DCI_FILE_C.WriteLine ("};")
                DCI_FILE_C.WriteLine ("")
            End If
            present_class_num = Cells(row_counter, DATA_CLASS_COL).text
            present_instance_num = Cells(row_counter, DATA_INSTANCE_COL).text
            DCI_FILE_C.WriteLine ("const CIP_INSTANCE_ST_TD " & _
                    lower_case_file_name & "_C0x" & Hexify(present_class_num, class_byte_size) & _
                    "_instance_list[" & upper_case_file_name & "_CLASS_0x" & Hexify(present_class_num, class_byte_size) & _
                    "_NUM_INSTANCES] = ")
            DCI_FILE_C.WriteLine ("{")
            DCI_FILE_C.WriteLine ("    {")
            DCI_FILE_C.WriteLine ("        " & present_instance_num & "U,")
            DCI_FILE_C.WriteLine ("        " & lower_case_file_name & "_C0x" & Hexify(present_class_num, class_byte_size) & _
                                "_INST_0x" & Hexify(present_instance_num, instance_byte_size) & _
                                "_attrib_list")
            DCI_FILE_C.WriteLine ("    },")
        End If
        If (present_instance_num <> Cells(row_counter, DATA_INSTANCE_COL).text) Then
            present_instance_num = Cells(row_counter, DATA_INSTANCE_COL).text
            DCI_FILE_C.WriteLine ("    {")
            DCI_FILE_C.WriteLine ("        " & present_instance_num & "U,")
            DCI_FILE_C.WriteLine ("        " & lower_case_file_name & "_C0x" & Hexify(present_class_num, class_byte_size) & _
                                "_INST_0x" & Hexify(present_instance_num, instance_byte_size) & _
                                "_attrib_list")
            DCI_FILE_C.WriteLine ("    },")
        End If
        row_counter = row_counter + 1
    Wend
    DCI_FILE_C.WriteLine ("};")
    DCI_FILE_C.WriteLine ("")
 
    


'****************************************************************************************************************************
'******     Create the class instance 0 attribute list.  We spin through the list once to set the class information.
'****************************************************************************************************************************

    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("//******      Define the Class Structure List")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("")
    class_counter = 0
    present_class_num = 0
    row_counter = START_ROW
    DCI_FILE_C.WriteLine ("const CIP_CLASS_ST_TD " & _
                lower_case_file_name & "_cip_class_list[" & upper_case_file_name & "_TOTAL_CLASSES] = ")
    DCI_FILE_C.WriteLine ("{")
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (present_class_num <> Cells(row_counter, DATA_CLASS_COL).text) Then
            present_class_num = Cells(row_counter, DATA_CLASS_COL).value
            max_instance_num = 0
            num_instances = 0
            instance_0_rev = 1
            While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And _
                        (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA) And _
                        (present_class_num = Cells(row_counter, DATA_CLASS_COL).text)
                If (Cells(row_counter, DATA_CLASS_REVISION_COL).text <> Empty) Then
                    If (Cells(row_counter, DATA_CLASS_REVISION_COL).text > instance_0_rev) Then
                        instance_0_rev = Cells(row_counter, DATA_CLASS_REVISION_COL).text
                    End If
                End If
'                If (Cells(row_counter, DATA_INSTANCE_COL).Text > max_instance_num) Then
'                    max_instance_num = Cells(row_counter, DATA_INSTANCE_COL).Text
'                    num_instances = num_instances + 1
'                End If
                row_counter = row_counter + 1
            Wend

            DCI_FILE_C.WriteLine ("    {        // C: " & present_class_num & " Index: " & class_counter)
            DCI_FILE_C.WriteLine ("        0x" & Hexify(present_class_num, class_byte_size) & "U,")
            DCI_FILE_C.WriteLine ("        " & lower_case_file_name & "_C0x" & _
                    Hexify(present_class_num, class_byte_size) & "_instance_list,")
            DCI_FILE_C.WriteLine ("        " & instance_0_rev & "U,")
'            DCI_FILE_C.WriteLine ("        " & max_instance_num & ",   ///Max number of instances.")
            DCI_FILE_C.WriteLine ("        " & upper_case_file_name & "_CLASS_0x" & _
                                Hexify(present_class_num, class_byte_size) & "_NUM_INSTANCES,")
            DCI_FILE_C.WriteLine ("        " & upper_case_file_name & "_CLASS_0x" & _
                                Hexify(present_class_num, class_byte_size) & "_NUM_ATTRIBUTES")
            DCI_FILE_C.WriteLine ("    },")
            class_counter = class_counter + 1
        Else
            row_counter = row_counter + 1
        End If
    Wend
    DCI_FILE_C.WriteLine ("};")
    DCI_FILE_C.WriteLine ("")


'****************************************************************************************************************************
'******     Generate the Message Router Object attribute 1 data.
'****************************************************************************************************************************
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("//******     Message Router Object Attribute 1 Definition")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("const uint16_t " & _
                lower_case_file_name & "_msg_rtr_attrib1[" & upper_case_file_name & "_TOTAL_CLASSES + 1U] = ")
    DCI_FILE_C.WriteLine ("{")
    DCI_FILE_C.WriteLine ("    " & upper_case_file_name & "_TOTAL_CLASSES,")
    present_class_num = 0
    row_counter = START_ROW
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (present_class_num <> Cells(row_counter, DATA_CLASS_COL).text) Then
            present_class_num = Cells(row_counter, DATA_CLASS_COL).value
            DCI_FILE_C.WriteLine ("    0x" & Hexify(present_class_num, 2) & "U,")
        End If
        row_counter = row_counter + 1
    Wend
    DCI_FILE_C.WriteLine ("};")



'****************************************************************************************************************************
'******     Generate the DCID to assembly instance lookup
'******     Takes the assembly items and reverses it back to the DCI values.
'******     This will also create a linked list if the same DCI value is used in the same assembly.
'****************************************************************************************************************************
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("")

    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("//******     The EIP Assembly Object Definition.")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("")

'******     Find the total number of assembly instance entries that we will need to create.
'******
    asm_instance_count = 0
    align_size = 0
    asm_instance_dci_offset = 0
    present_instance = 0
    row_counter = START_ROW
    dci_row = 0
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(row_counter, DATA_CLASS_COL).value = 4) And (Cells(row_counter, DATA_ATTRIBUTE_COL).text = 3) Then
            If (present_instance <> Cells(row_counter, DATA_INSTANCE_COL).value) Then
                present_instance = Cells(row_counter, DATA_INSTANCE_COL).value
                asm_instance_dci_offset = 0
            End If
            base_string = Cells(row_counter, DATA_DEFINE_COL).text & "_INST_" & _
                                "0x" & Hexify(Cells(row_counter, DATA_INSTANCE_COL).text, instance_byte_size)
            If (Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).value > 0) Then
                string_print = "static const uint8_t " & base_string & "_ASM_OFFSET="
                DCI_FILE_C.WriteLine (string_print & Create_Space(string_print, NUM_ALIGN_POS) & asm_instance_dci_offset & "U;")
                dci_row = Get_DCI_Row(Cells(row_counter, DATA_DEFINE_COL).text)
                asm_instance_dci_offset = asm_instance_dci_offset + Worksheets("DCI Descriptors").Range("A1").Cells(dci_row, DCI_LENGTH_COL).value
                asm_instance_dci_offset = asm_instance_dci_offset + (Worksheets("DCI Descriptors").Range("A1").Cells(dci_row, DCI_LENGTH_COL).value Mod align_size)
            Else
                If (Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).text <> Empty) Then
                    asm_offset_ctr = row_counter + 1
                    While (present_instance = Cells(asm_offset_ctr, DATA_INSTANCE_COL).value) And _
                                    (Cells(row_counter, DATA_CLASS_COL).value = Cells(asm_offset_ctr, DATA_CLASS_COL).value)
                         asm_offset_ctr = asm_offset_ctr + 1
                    Wend
                    string_print = "static const uint8_t " & base_string & "_ASM_DCID_LIST_LEN = "
                    DCI_FILE_C.WriteLine (string_print & Create_Space(string_print, NUM_ALIGN_POS) & (asm_offset_ctr - row_counter - 1) & "U;")
                    string_print = base_string & "_ASM_DCID_LIST"
                    string_print = "const DCI_ID_TD " & string_print & "[" & string_print & "_LEN] ="
                    DCI_FILE_C.WriteLine (string_print)
                    DCI_FILE_C.WriteLine ("{")
                    asm_offset_ctr = row_counter + 1
                    While (present_instance = Cells(asm_offset_ctr, DATA_INSTANCE_COL).value) And _
                                    (Cells(row_counter, DATA_CLASS_COL).value = Cells(asm_offset_ctr, DATA_CLASS_COL).value)
                        DCI_FILE_C.WriteLine ("    " & Cells(asm_offset_ctr, DATA_DEFINE_COL).text & "_DCID,")
                        asm_offset_ctr = asm_offset_ctr + 1
                    Wend
                    DCI_FILE_C.WriteLine ("};")
                    string_print = "static const uint8_t " & Cells(row_counter, DATA_DEFINE_COL).text & "_INST_" & _
                                    "0x" & Hexify(Cells(row_counter, DATA_INSTANCE_COL).text, instance_byte_size) & "_ASM_OFFSET ="
                    If (Cells(row_counter, DATA_ASSEMBLY_ALIGNMENT_COL).text <> Empty) Then
                        align_size = Cells(row_counter, DATA_ASSEMBLY_ALIGNMENT_COL).value
                    Else
                        align_size = 1
                    End If
                    DCI_FILE_C.WriteLine (string_print & Create_Space(string_print, NUM_ALIGN_POS) & _
                                align_size & "U;" & "    //This is the alignment size for this assembly head.")
                Else
                    string_print = "static const uint8_t " & Cells(row_counter, DATA_DEFINE_COL).text & "_INST_" & _
                                    "0x" & Hexify(Cells(row_counter, DATA_INSTANCE_COL).text, instance_byte_size) & "_ASM_OFFSET ="
                    DCI_FILE_C.WriteLine (string_print & Create_Space(string_print, NUM_ALIGN_POS) & "0U;")
                End If
            End If
            asm_instance_count = asm_instance_count + 1
        End If
        row_counter = row_counter + 1
    Wend
    DCI_FILE_C.WriteLine ("")
    string_print = "static const uint16_t " & upper_case_file_name & "_TOTAL_ASM_ITEMS ="
    DCI_FILE_C.WriteLine (string_print & Create_Space(string_print, NUM_ALIGN_POS) & asm_instance_count & "U;")
    DCI_FILE_C.WriteLine ("")


'******     First I need to scan the list to look for redundant DCI IDs.  That way we can create a linked list lookup of them.
    max_asm_data_len = 0
    dci_defines_row_ctr = DCI_DEFINES_START_ROW
    DCI_FILE_C.WriteLine ("const CIP_ASM_STRUCT_TD " & _
                        lower_case_file_name & "_dcid_to_asm[" & upper_case_file_name & "_TOTAL_ASM_ITEMS] = ")
    DCI_FILE_C.WriteLine ("{")
    While Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> Empty
        DCI_STRING = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text
        dci_occurence_counter = 0
        row_counter = START_ROW
        While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
            If (Cells(row_counter, DATA_DEFINE_COL).text = DCI_STRING) And _
                                        (Cells(row_counter, DATA_CLASS_COL).text = 4) And _
                                        (Cells(row_counter, DATA_ATTRIBUTE_COL).text = 3) Then
                If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_LENGTH_COL).text > max_asm_data_len) Then
                    max_asm_data_len = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_LENGTH_COL).text
                End If
                DCI_FILE_C.WriteLine ("    {        // C: 0x" & Hexify(Cells(row_counter, DATA_CLASS_COL).text, class_byte_size) & _
                                                    ", I: 0x" & Hexify(Cells(row_counter, DATA_INSTANCE_COL).text, instance_byte_size) & _
                                                    ", A: 0x" & Hexify(Cells(row_counter, DATA_ATTRIBUTE_COL).text, attribute_byte_size) & _
                                                    " - " & Cells(row_counter, DESCRIP_COL).text)
                DCI_FILE_C.WriteLine ("        " & DCI_STRING & "_DCID,")
                DCI_FILE_C.WriteLine ("        " & dci_occurence_counter & "U,    //This is the number of times this DCI exists.  How many to go back to get to the first one.")
                DCI_FILE_C.WriteLine ("        " & Cells(row_counter, DATA_INSTANCE_COL).text & "U,")
                If (Cells(row_counter, DATA_ASSEMBLY_PRODUCED_COL).text <> Empty) Then
                    DCI_FILE_C.WriteLine ("        true,")
                Else
                    DCI_FILE_C.WriteLine ("        false,")
                End If
                
                If (Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).text = Empty) Then
                    DCI_FILE_C.WriteLine ("        CIP_ASM_TYPE_STANDALONE,")
                ElseIf (Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).value = 0) Then
                    DCI_FILE_C.WriteLine ("        CIP_ASM_TYPE_LIST_HEAD,")
                Else
                    DCI_FILE_C.WriteLine ("        CIP_ASM_TYPE_LIST_MEMBER,")
                End If
                
                base_string = Cells(row_counter, DATA_DEFINE_COL).text & "_INST_" & _
                                "0x" & Hexify(Cells(row_counter, DATA_INSTANCE_COL).text, instance_byte_size)
                DCI_FILE_C.WriteLine ("        " & base_string & "_ASM_OFFSET,    //This is the number of bytes offset into the assembly it is.")
                If (Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).text <> Empty) And _
                        (Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).value = 0) Then
                    DCI_FILE_C.WriteLine ("        " & base_string & "_ASM_DCID_LIST,")
                    DCI_FILE_C.WriteLine ("        " & base_string & "_ASM_DCID_LIST_LEN")
                Else
                    DCI_FILE_C.WriteLine ("        reinterpret_cast<DCI_ID_TD*>(NULL),")
                    DCI_FILE_C.WriteLine ("        static_cast<uint8_t>(NULL)")
                End If
                DCI_FILE_C.WriteLine ("    },")
                dci_occurence_counter = dci_occurence_counter + 1
            End If
            row_counter = row_counter + 1
        Wend
        dci_defines_row_ctr = dci_defines_row_ctr + 1
    Wend
    DCI_FILE_C.WriteLine ("};")


'****************************************************************************************************************************
'******     Generate the target info.
'****************************************************************************************************************************
    DCI_FILE_C.WriteLine ("")

    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("//******     The Target Definition Structure.")
    DCI_FILE_C.WriteLine ("//*******************************************************")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("const EIP_TARGET_INFO_ST_TD " & lower_case_file_name & "_target_info =")
    DCI_FILE_C.WriteLine ("{")
    DCI_FILE_C.WriteLine ("    " & upper_case_file_name & "_DEVICE_TYPE,   //uint16_t identity_dev_type;")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("    " & upper_case_file_name & "_TOTAL_CLASSES, //uint16_t total_items;")
    DCI_FILE_C.WriteLine ("    " & lower_case_file_name & "_cip_class_list,//const DCI_CIP_TO_DCID_LKUP_ST_TD* cip_to_dcid")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("    " & lower_case_file_name & "_msg_rtr_attrib1,//uint16_t const* msg_rtr_attrib1")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("    " & upper_case_file_name & "_TOTAL_ASM_ITEMS,//uint16_t total_assemblies;")
    DCI_FILE_C.WriteLine ("    " & max_asm_data_len & "U,                   //uint16_t asm_max_data_len;")
    DCI_FILE_C.WriteLine ("    " & lower_case_file_name & "_dcid_to_asm     //CIP_ASM_STRUCT_TD const* asm_list;")
    DCI_FILE_C.WriteLine ("};")
    DCI_FILE_C.WriteLine ("")
    DCI_FILE_C.WriteLine ("")


    DCI_FILE_C.Close
    DCI_FILE_H.Close

    Columns("H:H").Clear
    row_counter = START_ROW
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        Cells(row_counter, DATA_CIA_SORT_COL).value = Cells(row_counter, DATA_CLASS_COL).value * (2 ^ 32) + _
                    Cells(row_counter, DATA_ATTRIBUTE_COL).value * 2 ^ 16 + _
                    Cells(row_counter, DATA_INSTANCE_COL).value
        row_counter = row_counter + 1
    Wend

    Rows("9:1000").Sort Key1:=Range("H9"), Key2:=Range("CB9"), _
        Order1:=xlAscending, Order2:=xlAscending, _
        Header:=xlGuess, OrderCustom:=1, MatchCase:=False, Orientation:=xlTopToBottom

    Columns("H:H").Clear

'    Rows("9:499").Sort Key1:=Range("C9"), Key2:=Range("E9"), Key3:=Range("D9"), Order1:=xlAscending, _
        Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
        Orientation:=xlTopToBottom

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
Sub Create_EtherNetIP_EDS_Stubs()

'*********************
'******     Constants
'*********************
    
    Application.Calculation = xlCalculationManual
    
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    Dim sheet_name As String

    Dim DCI_STRING As String
    Dim num_unique_data As Integer
    Dim file_name As String
    Dim lower_case_file_name As String
    Dim cia_num As Long
    Dim special_str As String
    Dim cia_str As String
    Dim temp_string As String
    Dim row_counter As Long
    Dim inst_row_counter As Long
    Dim class_row_counter As Long
    
    Dim class_byte_size As Integer
    Dim instance_byte_size As Integer
    Dim attribute_byte_size As Integer

    Dim present_class_num As Long   ' We will use this to track the current class number being worked on.
    Dim class_counter As Long       ' We will use this to track how many classes have been created.
    
    Dim present_instance_num As Long   ' We will use this to track the current class number being worked on.
    Dim instance_counter As Long
    Dim num_instances As Long
    Dim max_instance_num As Long
    Dim instance_0_rev As Long
    
    Dim present_attrib_num As Long   ' We will use this to track the current class number being worked on.
    Dim attribe_counter As Long       ' We will use this to track how many classes have been created.
    
    NUM_ALIGN_POS = 65

    Dim COMMENT_ALIGN_POS As Integer
    COMMENT_ALIGN_POS = 40
    

'*********************
'******     Begin
'*********************
    
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select

    file_name = Cells(2, 2).value & "_EDS_Stub.txt"
    lower_case_file_name = LCase(file_name)
    upper_case_file_name = UCase(file_name)
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\"
    file_path = file_path & Worksheets("Usage Notes").Range("A1").Cells(GENERATED_DOCS_PATH_ROW, GENERATED_DOCS_PATH_COL).text
    Set EDS_OUT_FILE = fs.CreateTextFile(file_path & file_name, True)

'****************************************************************************************************************************
'******     Start Creating the .H file header
'****************************************************************************************************************************
    EDS_OUT_FILE.WriteLine ("$****************************************************************************************")
    EDS_OUT_FILE.WriteLine ("$*")
    EDS_OUT_FILE.WriteLine ("$*       Generated EDS Data:")
    EDS_OUT_FILE.WriteLine ("$*")
    EDS_OUT_FILE.WriteLine ("$****************************************************************************************")
    EDS_OUT_FILE.WriteLine ("")
    EDS_OUT_FILE.WriteLine ("")
    
'****************************************************************************************************************************
'******     Sort the list
'****************************************************************************************************************************
    Rows("9:1000").Sort Key1:=Range("C9"), Key2:=Range("D9"), Key3:=Range("E9"), Order1:=xlAscending, _
        Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
        Orientation:=xlTopToBottom

'****************************************************************************************************************************
'******     Gotta calculate the total number of items we are going to create.
'****************************************************************************************************************************
    use_64bit_cia = False
    num_entries = 0
     
    class_byte_size = 1
    instance_byte_size = 1
    attribute_byte_size = 1
    row_counter = START_ROW
    class_counter = 0
    present_class_num = 0
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(row_counter, DATA_UNPUBLISHED_COL).text = Empty) Then
            If (Cells(row_counter, DATA_CLASS_COL).text >= 256) Then
                class_byte_size = 2
            End If
            
            If (Cells(row_counter, DATA_INSTANCE_COL).text >= 256) Then
                instance_byte_size = 2
            End If
            
            If (Cells(row_counter, DATA_ATTRIBUTE_COL).text >= 256) Then
                attribute_byte_size = 2
            End If
            If (present_class_num <> Cells(row_counter, DATA_CLASS_COL).value) Then
                present_class_num = Cells(row_counter, DATA_CLASS_COL).text
                class_counter = class_counter + 1
            End If
            num_entries = num_entries + 1
        End If
        row_counter = row_counter + 1
    Wend
    
    If ((class_is_16bit + instance_is_16bit + attribute_is_16bit) > 1) Then
        use_64bit_cia = True
    End If

'****************************************************************************************************************************
'******     Create the header and associated file structure.  Get ready to plop the data down.
'****************************************************************************************************************************
    Dim FIRST_INDENT As Integer
    Dim SECOND_INDENT As Integer
    FIRST_INDENT = 4
    SECOND_INDENT = 12
    Dim param_string As String
    Dim desc_int As Long
    Dim index_string As String
    
'****************************************************************************************************************************
'******     Create the Params section.
    EDS_OUT_FILE.WriteLine ("[Params]")

    row_counter = START_ROW
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And _
             (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        dci_defines_row_ctr = Find_DCID_Row(Cells(row_counter, DATA_DEFINE_COL).text)
        
        If (dci_defines_row_ctr <> 0) And (Cells(row_counter, DATA_UNPUBLISHED_COL).text = Empty) Then
            present_class_num = Cells(row_counter, DATA_CLASS_COL).text
            present_instance_num = Cells(row_counter, DATA_INSTANCE_COL).text
            present_attrib_num = Cells(row_counter, DATA_ATTRIBUTE_COL).text

'*************************************************
'******     Create the Param Header
            param_string = Spacify("", FIRST_INDENT)
            param_string = param_string & "Param" & Get_ID(row_counter) & " = "
'            If (present_class_num <> 4) Then
'                param_string = param_string & "Param" & _
'                            Create_Dec_CIA(present_class_num, present_instance_num, present_attrib) & " = "
'                param_id_counter = param_id_counter + 1
'            Else
'                param_string = param_string & "Param" & asm_param_id_counter & " = "
'                asm_param_id_counter = asm_param_id_counter + 1
'            End If
            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
            param_string = param_string & "$ C: 0x" & Hexify(present_class_num, class_byte_size) & _
                        ", I: 0x" & Hexify(present_instance_num, instance_byte_size) & _
                        ", A: 0x" & Hexify(present_attrib_num, attribute_byte_size) & _
                        " - " & Cells(row_counter, DESCRIP_COL).text
            EDS_OUT_FILE.WriteLine (param_string)

'*************************************************
'******     Null Field
            param_string = Spacify("", SECOND_INDENT)
            param_string = param_string & "0,"
            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
            param_string = param_string & "$ First field shall equal 0"
            EDS_OUT_FILE.WriteLine (param_string)
    
'*************************************************
'******     Link Path and size.
            If (Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).value > 0) Then
                param_string = Spacify("", SECOND_INDENT)
                param_string = param_string & ",,"
            Else
                param_string = Spacify("", SECOND_INDENT)
                param_string = param_string & "6, ""20 " & Hexify(present_class_num, 1) _
                                                & " 24 " & Hexify(present_instance_num, 1) _
                                                & " 30 " & Hexify(present_attrib_num, 1) & ""","
            End If
            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
            param_string = param_string & "$ Link Path Size, Link Path (Class, Inst, Attrib)"
            EDS_OUT_FILE.WriteLine (param_string)
            
            
'*************************************************
'******     Descriptor
            desc_int = 0
            If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, PATRON_RAM_VAL_WRITEABLE_COL).text = Empty) Then
                '******     Read only bit 4.
                desc_int = desc_int + (2 ^ 4)
            End If
            
            If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, PATRON_INIT_VAL_WRITEABLE_ACCESS_COL).text = Empty) Then
                '******     Monitor bit 5.
                desc_int = desc_int + (2 ^ 5)
            End If
            
            If (Cells(row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).value > 0) Then
                '******     This parameter should not be displayed.  It is part of an assembly.  Removed until someone says to make it so.
                'desc_int = desc_int + (2 ^ 10)
            End If
            param_string = Spacify("", SECOND_INDENT)
            param_string = param_string & "0x" & Hexify(desc_int, 2) & ","
            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
            param_string = param_string & "$ Descriptor Bits"
            EDS_OUT_FILE.WriteLine (param_string)

'*************************************************
'******     Data Type
            param_string = Spacify("", SECOND_INDENT)
            If (Cells(row_counter, DATA_BITWISE_BOOL_COL).text <> Empty) Then
                param_string = param_string & _
                            Get_CIP_Datatype("DCI_DTYPE_BOOL")
            ElseIf (Cells(row_counter, DATA_CIP_SHORT_STRING_COL).text <> Empty) Then
                param_string = param_string & "0xDA"
            ElseIf (Cells(row_counter, DATA_ALTERNATE_LEN_COL).text <> Empty) Then
                param_string = param_string & _
                            Get_CIP_Datatype(Get_Datatype_With_Alt_Len(Cells(row_counter, DATA_ALTERNATE_LEN_COL).value, _
                                    Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DATATYPE_COL).text))
            Else
                param_string = param_string & _
                            Get_CIP_Datatype(Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DATATYPE_COL).text)
            End If
            param_string = param_string & ","
            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
            param_string = param_string & "$ Data Type" & " - " & _
                            Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DATATYPE_COL).text
            EDS_OUT_FILE.WriteLine (param_string)
            
'*************************************************
'******     Data Size
            param_string = Spacify("", SECOND_INDENT)
            If (Cells(row_counter, DATA_ALTERNATE_LEN_COL).text <> Empty) Then
                param_string = param_string & Cells(row_counter, DATA_ALTERNATE_LEN_COL).text & ","
            ElseIf (Cells(row_counter, DATA_BITWISE_BOOL_COL).text <> Empty) Or _
                    (Cells(row_counter, DATA_CIP_SHORT_STRING_COL).text <> Empty) Then
                param_string = param_string & "1,"
            ElseIf (Cells(row_counter, DATA_INDEX_COL).text <> Empty) Then
                param_string = param_string & _
                        Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DATATYPE_SIZE).text & ","
            Else
                param_string = param_string & _
                        Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, TOTAL_LENGTH_COL).text & ","
            End If
            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
            param_string = param_string & "$ Data Size in Bytes"
            EDS_OUT_FILE.WriteLine (param_string)
            
'*************************************************
'******     Data Name - textual description
            param_string = Spacify("", SECOND_INDENT)
            index_string = ""
            If (Cells(row_counter, DATA_INDEX_COL).text <> Empty) Then
                index_string = " " & (Cells(row_counter, DATA_INDEX_COL).value + 1)
            End If
            If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_SHORT_DESC_COL).text <> Empty) Then
                param_string = param_string & """" & _
                        Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_SHORT_DESC_COL).text & _
                        index_string & ""","
            Else
                param_string = param_string & """" & _
                        Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DATA_DESCRIPTION_COL).text & _
                        index_string & ""","
            End If
            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
            param_string = param_string & "$ Data Name"
            EDS_OUT_FILE.WriteLine (param_string)
            
'*************************************************
'******     Units
            param_string = Spacify("", SECOND_INDENT)
            param_string = param_string & """"","
            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
            param_string = param_string & "$ Units"
            EDS_OUT_FILE.WriteLine (param_string)

'*************************************************
'******     Help string
            param_string = Spacify("", SECOND_INDENT)
            param_string = param_string & """" & _
                        Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_SHORT_DESC_COL).text & ""","
            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
            param_string = param_string & "$ Help String"
            EDS_OUT_FILE.WriteLine (param_string)
            
'*************************************************
'******     Min Value
            param_string = Spacify("", SECOND_INDENT)
            If (Cells(row_counter, DATA_BITWISE_BOOL_COL).text <> Empty) Then
                param_string = param_string & "0"
            ElseIf (Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, RANGE_ATTRIB_COL).text <> Empty) Then
                param_string = param_string & _
                        Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DATA_MIN_COL).text
            ElseIf (Cells(row_counter, DATA_ALTERNATE_LEN_COL).text <> Empty) Then
                param_string = param_string & _
                            Get_CIP_Datatype_Min_String(Get_Datatype_With_Alt_Len(Cells(row_counter, DATA_ALTERNATE_LEN_COL).value, _
                                    Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DATATYPE_COL).text))
            Else
                param_string = param_string & _
                        Get_CIP_Datatype_Min_String(Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DATATYPE_COL).text)
            End If
            param_string = param_string & ","
            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
            param_string = param_string & "$ Min Value"
            EDS_OUT_FILE.WriteLine (param_string)
            
'*************************************************
'******     Max Value
            param_string = Spacify("", SECOND_INDENT)
            If (Cells(row_counter, DATA_BITWISE_BOOL_COL).text <> Empty) Then
                param_string = param_string & "1"
            ElseIf (Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, RANGE_ATTRIB_COL).text <> Empty) Then
                param_string = param_string & _
                        Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DATA_MAX_COL).text
            ElseIf (Cells(row_counter, DATA_ALTERNATE_LEN_COL).text <> Empty) Then
                param_string = param_string & _
                            Get_CIP_Datatype_Max_String(Get_Datatype_With_Alt_Len(Cells(row_counter, DATA_ALTERNATE_LEN_COL).value, _
                                    Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DATATYPE_COL).text))
            Else
                param_string = param_string & _
                        Get_CIP_Datatype_Max_String(Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DATATYPE_COL).text)
            End If
            param_string = param_string & ","
            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
            param_string = param_string & "$ Max Value"
            EDS_OUT_FILE.WriteLine (param_string)
            
'*************************************************
'******     Default Value
            param_string = Spacify("", SECOND_INDENT)
            If (Cells(row_counter, DATA_CIP_SHORT_STRING_COL).text <> Empty) Then
                param_string = param_string & """ """
            ElseIf (Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DEFAULT_VAL_ATTRIB_COL).text <> Empty) Then
                param_string = param_string & _
                        Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DATA_DEFAULT_COL).text
            Else
                param_string = param_string & "0"
            End If
            param_string = param_string & ","
            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
            param_string = param_string & "$ Default Value"
            EDS_OUT_FILE.WriteLine (param_string)

'*************************************************
'******     Mult Div Base Offset Scaling
            param_string = Spacify("", SECOND_INDENT)
            param_string = param_string & ",,,,"
            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
            param_string = param_string & "$ Mult, Div, Base, Offset Scaling"
            EDS_OUT_FILE.WriteLine (param_string)

'*************************************************
'******     Mult Div Base Offset Links
            param_string = Spacify("", SECOND_INDENT)
            param_string = param_string & ",,,,"
            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
            param_string = param_string & "$ Mult, Div, Base, Offset Links"
            EDS_OUT_FILE.WriteLine (param_string)
            
 '*************************************************
'******     Mult Div Base Offset Links
            param_string = Spacify("", SECOND_INDENT)
            param_string = param_string & ";"
            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
            param_string = param_string & "$ Decimal Places"
            EDS_OUT_FILE.WriteLine (param_string)
           
            EDS_OUT_FILE.WriteLine ("")
        End If
        row_counter = row_counter + 1
        param_id_counter = param_id_counter + 1
    Wend


'****************************************************************************************************************************
'****************************************************************************************************************************
'******     Create the Groups section.
    EDS_OUT_FILE.WriteLine ("")
    EDS_OUT_FILE.WriteLine ("[Groups]")
    
    row_counter = START_ROW
    group_id_counter = 1
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And _
             (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)

        present_class_num = Cells(row_counter, DATA_CLASS_COL).text
        present_instance_num = Cells(row_counter, DATA_INSTANCE_COL).text
        present_attrib_num = Cells(row_counter, DATA_ATTRIBUTE_COL).text

        If (present_class_num <> 1) And (present_class_num <> 4) And (Cells(row_counter, DATA_UNPUBLISHED_COL).text = Empty) Then
'*************************************************
'******     Create the Group Header
            param_string = Spacify("", FIRST_INDENT)
            param_string = param_string & "Group" & group_id_counter & " = "
            group_id_counter = group_id_counter + 1
            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
            param_string = param_string & "$ C: 0x" & Hexify(present_class_num, class_byte_size) & _
                            " - " & Cells(row_counter, DATA_CLASS_NAME_COL).text
            EDS_OUT_FILE.WriteLine (param_string)
            
'*************************************************
'******     Create the Class Description
            param_string = Spacify("", SECOND_INDENT)
            param_string = param_string & """" & Cells(row_counter, DATA_CLASS_NAME_COL).text & ""","
            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
            param_string = param_string & "$ Class Description"
            EDS_OUT_FILE.WriteLine (param_string)
            
'*************************************************
'******     Count and Write out the Attribute Count
            class_row_counter = row_counter
            class_item_counter = 0
            While (Cells(class_row_counter, DATA_DEFINE_COL).text <> Empty) And _
                    (Cells(class_row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA) And _
                    (present_class_num = Cells(class_row_counter, DATA_CLASS_COL).text)
                class_item_counter = class_item_counter + 1
                class_row_counter = class_row_counter + 1
            Wend

            param_string = Spacify("", SECOND_INDENT)
            param_string = param_string & class_item_counter & ","
            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
            param_string = param_string & "$ Class Attribute Count"
            EDS_OUT_FILE.WriteLine (param_string)
            
                        
'*************************************************
'******     Write out the Attributes
            MAX_ATTRIBUTES_PER_ROW = 4
            attribute_print_counter = 0
            class_row_counter = row_counter
            param_string = Spacify("", SECOND_INDENT)
            While (Cells(class_row_counter, DATA_DEFINE_COL).text <> Empty) And _
                            (Cells(class_row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA) And _
                            (present_class_num = Cells(class_row_counter, DATA_CLASS_COL).text)
'                If (Cells(class_row_counter, DATA_INSTANCE_COL).Text = present_instance_num) Then
                    If (attribute_print_counter = 0) Then
                        param_string = param_string & Get_ID(class_row_counter)
                    Else
                        param_string = param_string & ", " & Get_ID(class_row_counter)
                    End If

                    attribute_print_counter = attribute_print_counter + 1
                    If (attribute_print_counter >= MAX_ATTRIBUTES_PER_ROW) And _
                            (Cells(class_row_counter + 1, DATA_DEFINE_COL).text <> Empty) And _
                            (Cells(class_row_counter + 1, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA) And _
                            (present_class_num = Cells(class_row_counter + 1, DATA_CLASS_COL).text) Then
                        param_string = param_string & ","
                        param_string = Spacify(param_string, COMMENT_ALIGN_POS)
                        param_string = param_string & "$ Class Attribute List"
                        EDS_OUT_FILE.WriteLine (param_string)
                        param_string = Spacify("", SECOND_INDENT)
                        attribute_print_counter = 0
                    End If
'                End If
                class_row_counter = class_row_counter + 1
            Wend
            param_string = param_string & ";"
            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
            param_string = param_string & "$ Class Attribute List"
            EDS_OUT_FILE.WriteLine (param_string)
            
            EDS_OUT_FILE.WriteLine ("")
            row_counter = class_row_counter
        Else
            row_counter = row_counter + 1
        End If
    Wend


'****************************************************************************************************************************
'****************************************************************************************************************************
'******     Create the Assemblies section.
    EDS_OUT_FILE.WriteLine ("")
    EDS_OUT_FILE.WriteLine ("[Assembly]")
    Dim data_size_string As String

    row_counter = START_ROW
    asm_param_id_counter = 4000
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And _
             (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        dci_defines_row_ctr = DCI_DEFINES_START_ROW

        present_class_num = Cells(row_counter, DATA_CLASS_COL).text
        If (present_class_num = 4) And (Cells(row_counter, DATA_UNPUBLISHED_COL).text = Empty) Then
        
            dci_defines_row_ctr = Find_DCID_Row(Cells(row_counter, DATA_DEFINE_COL).text)
        
            If (dci_defines_row_ctr <> 0) Then
                present_instance_num = Cells(row_counter, DATA_INSTANCE_COL).text
                present_attrib_num = Cells(row_counter, DATA_ATTRIBUTE_COL).text
           
'*************************************************
'******     Create the Assem Header
                param_string = Spacify("", FIRST_INDENT)
                param_string = param_string & "Assem" & present_instance_num & " = "
                param_string = Spacify(param_string, COMMENT_ALIGN_POS)
                param_string = param_string & "$ Assembly Instance #:" & present_instance_num _
                                & " (" & Hexify(present_instance_num, class_byte_size) & _
                                ") - " & Cells(row_counter, DATA_CLASS_NAME_COL).text
                EDS_OUT_FILE.WriteLine (param_string)

'*************************************************
'******     Create the Assembly Description
                param_string = Spacify("", SECOND_INDENT)
                index_string = ""
                If (Cells(row_counter, DATA_INDEX_COL).text <> Empty) Then
                    index_string = " " & (Cells(row_counter, DATA_INDEX_COL).value + 1)
                End If
                If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_SHORT_DESC_COL).text <> Empty) Then
                    param_string = param_string & """" & _
                            Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_SHORT_DESC_COL).text & _
                            index_string & ""","
                Else
                    param_string = param_string & """" & _
                            Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DATA_DESCRIPTION_COL).text & _
                            index_string & ""","
                End If
                param_string = Spacify(param_string, COMMENT_ALIGN_POS)
                param_string = param_string & "$ Data Name"
                EDS_OUT_FILE.WriteLine (param_string)

'*************************************************
'******     Create the Path
                param_string = Spacify("", SECOND_INDENT)
                param_string = param_string & """"","
                param_string = Spacify(param_string, COMMENT_ALIGN_POS)
                param_string = param_string & "$ Path"
                EDS_OUT_FILE.WriteLine (param_string)
    
'*************************************************
'******     Data Size

'*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+
                    
                '***********************************************************************************
                '****** This is where we calculate the assembly size.
                prev_instance = 0
                offset_counter = 0
                inst_row_counter = row_counter
                If (Cells(row_counter, DATA_ASSEMBLY_ALIGNMENT_COL).text <> Empty) Then
                    align_size = Cells(row_counter, DATA_ASSEMBLY_ALIGNMENT_COL).value
                Else
                    align_size = 1
                End If
                present_instance_num = Cells(inst_row_counter, DATA_INSTANCE_COL).text
                While (Cells(inst_row_counter, DATA_INSTANCE_COL).text = present_instance_num)
                    desc_sheet_row = Find_DCID_Row(Cells(inst_row_counter, DATA_DEFINE_COL).text)
                    If (desc_sheet_row <> 0) Then
                        If ((Cells(inst_row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).text = Empty) Or _
                                      (Cells(inst_row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).value > 0)) Then
                            offset_counter = offset_counter + Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, TOTAL_LENGTH_COL).text
                            offset_counter = offset_counter + (Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, TOTAL_LENGTH_COL).text Mod align_size)
                        End If
                    End If
                    inst_row_counter = inst_row_counter + 1
                Wend
    
'*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+
                param_string = Spacify("", SECOND_INDENT)
                asm_data_size = offset_counter
If (0) Then
            If (Cells(row_counter, DATA_ALTERNATE_LEN_COL).text <> Empty) Then
                asm_data_size = param_string & Cells(row_counter, DATA_ALTERNATE_LEN_COL).text
                'param_string = param_string & Cells(row_counter, DATA_ALTERNATE_LEN_COL).Text & ","
            ElseIf (Cells(row_counter, DATA_BITWISE_BOOL_COL).text <> Empty) Or _
                    (Cells(row_counter, DATA_CIP_SHORT_STRING_COL).text <> Empty) Then
                asm_data_size = 1
'                    param_string = param_string & "1,"
            ElseIf (Cells(row_counter, DATA_INDEX_COL).text <> Empty) Then
                asm_data_size = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DATATYPE_SIZE).text
'                    param_string = param_string & _
                        Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DATATYPE_SIZE).Text & ","
            Else
                asm_data_size = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, TOTAL_LENGTH_COL).text
'                    param_string = param_string & _
                        Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, TOTAL_LENGTH_COL).Text & ","
            End If
End If
                param_string = param_string & asm_data_size & ","
                param_string = Spacify(param_string, COMMENT_ALIGN_POS)
                param_string = param_string & "$ Data Size (in bytes)"
                EDS_OUT_FILE.WriteLine (param_string)

'*************************************************
'******     Create the Descriptor
                param_string = Spacify("", SECOND_INDENT)
                param_string = param_string & "0x0000,"
                param_string = Spacify(param_string, COMMENT_ALIGN_POS)
                param_string = param_string & "$ Descriptor"
                EDS_OUT_FILE.WriteLine (param_string)

'*************************************************
'******     Create the Reserved Space
                param_string = Spacify("", SECOND_INDENT)
                param_string = param_string & ",,"
                param_string = Spacify(param_string, COMMENT_ALIGN_POS)
                param_string = param_string & "$ Reserved - Must Be Empty"
                EDS_OUT_FILE.WriteLine (param_string)
    
'*************************************************
'******     Create the Member Reference
'******     Create the Member Size.  Needs to be the bit size.

'*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+
                prev_instance = 0
                data_size = 0
                inst_row_counter = row_counter
                If (Cells(row_counter, DATA_ASSEMBLY_ALIGNMENT_COL).text <> Empty) Then
                    align_size = Cells(row_counter, DATA_ASSEMBLY_ALIGNMENT_COL).value
                Else
                    align_size = 1
                End If
                present_instance_num = Cells(inst_row_counter, DATA_INSTANCE_COL).text
                While (Cells(inst_row_counter, DATA_INSTANCE_COL).text = present_instance_num)
                    desc_sheet_row = Find_DCID_Row(Cells(inst_row_counter, DATA_DEFINE_COL).text)
                    If (desc_sheet_row <> 0) Then
                        If ((Cells(inst_row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).text = Empty) Or _
                                      (Cells(inst_row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).value > 0)) Then
                            data_size = Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, TOTAL_LENGTH_COL).text
                            data_size = data_size + (Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, TOTAL_LENGTH_COL).text Mod align_size)
                            asm_data_size = data_size * 8
                            
                            param_string = Spacify("", SECOND_INDENT)
                            param_string = param_string & asm_data_size & ", "
                            If (Cells(inst_row_counter + 1, DATA_INSTANCE_COL).text = present_instance_num) Then
                                param_string = param_string & "Param" & Get_ID(inst_row_counter) & ","
                            Else
                                param_string = param_string & "Param" & Get_ID(inst_row_counter) & ";"
                            End If
                            param_string = Spacify(param_string, COMMENT_ALIGN_POS)
                            param_string = param_string & "$ Member Size (in bits), Member Reference"
                            EDS_OUT_FILE.WriteLine (param_string)
                        End If
                    End If
                    inst_row_counter = inst_row_counter + 1
                Wend
                row_counter = inst_row_counter - 1
'*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+

                EDS_OUT_FILE.WriteLine ("")
            End If
        End If

        row_counter = row_counter + 1
    Wend


    EDS_OUT_FILE.Close

    Rows("9:499").Sort Key1:=Range("C9"), Key2:=Range("E9"), Key3:=Range("D9"), Order1:=xlAscending, _
        Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
        Orientation:=xlTopToBottom

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

Sub Create_EIP_Documentation(Optional gen_unpublished As Boolean = False)

'*********************
'******     Constants
'*********************
    If (gen_unpublished = False) Then
        Call Create_EtherNetIP_EDS_Stubs
    End If
    
    Application.Calculation = xlCalculationManual
    
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    Dim sheet_name As String

    Dim present_class_num As Long   ' We will use this to track the current class number being worked on.
    Dim class_counter As Long       ' We will use this to track how many classes have been created.
    Dim present_attrib_num As Long   ' We will use this to track the current class number being worked on.

    Dim present_instance_num As Long   ' We will use this to track the current class number being worked on.
    Dim instance_counter As Long
    Dim num_instances As Long
    Dim max_instance_num As Long
    Dim instance_0_rev As Long
    Dim align_size As Integer
    align_size = 2

    Dim DCI_STRING As String
    Dim num_unique_data As Integer
    Dim file_name As String
    Dim lower_case_file_name As String

'*****************************************
'******     HTML Cell Column Width and alignment
'*****************************************

    HTML_COL_WIDTH_ATTRIB = """8%"""
    HTML_COL_WIDTH_NV = """8%"""
    HTML_COL_WIDTH_ACCESS = """8%"""
    HTML_COL_WIDTH_DTYPE = """8%"""
    HTML_COL_WIDTH_NAME = """15%"""
    HTML_COL_WIDTH_DESC = """53%"""
    
    HTML_COL_ALIGN_ATTRIB = """center"""
    HTML_COL_ALIGN_NV = """center"""
    HTML_COL_ALIGN_ACCESS = """center"""
    HTML_COL_ALIGN_DTYPE = """center"""
    HTML_COL_ALIGN_NAME = """left"""
    HTML_COL_ALIGN_DESC = """left"""
    
    HTML_COL_VALIGN_ATTRIB = """top"""
    HTML_COL_VALIGN_NV = """top"""
    HTML_COL_VALIGN_ACCESS = """top"""
    HTML_COL_VALIGN_DTYPE = """top"""
    HTML_COL_VALIGN_NAME = """top"""
    HTML_COL_VALIGN_DESC = """top"""
    
    HTML_TEXT_STYLE = """font-family:arial"""
    
'    Dim num_cols As Integer
    Const num_cols = 6
    Dim result_string As String
    Dim col_strings(1 To num_cols) As String
    col_strings(1) = "Test1"
    col_strings(2) = "Test2"
    col_strings(3) = "Test3"
    col_strings(4) = "Test4"
    col_strings(5) = "Test5"
    col_strings(6) = "Test6"
    Dim col_def(1 To num_cols) As MY_HTML_COL_DEF
    col_def(1).width = HTML_COL_WIDTH_ATTRIB
    col_def(1).align = HTML_COL_ALIGN_ATTRIB
    col_def(1).valign = HTML_COL_VALIGN_ATTRIB
    col_def(2).width = HTML_COL_WIDTH_NV
    col_def(2).align = HTML_COL_ALIGN_NV
    col_def(2).valign = HTML_COL_VALIGN_NV
    col_def(3).width = HTML_COL_WIDTH_ACCESS
    col_def(3).align = HTML_COL_ALIGN_ACCESS
    col_def(3).valign = HTML_COL_VALIGN_ACCESS
    col_def(4).width = HTML_COL_WIDTH_DTYPE
    col_def(4).align = HTML_COL_ALIGN_DTYPE
    col_def(4).valign = HTML_COL_VALIGN_DTYPE
    col_def(5).width = HTML_COL_WIDTH_NAME
    col_def(5).align = HTML_COL_ALIGN_NAME
    col_def(5).valign = HTML_COL_VALIGN_NAME
    col_def(6).width = HTML_COL_WIDTH_DESC
    col_def(6).align = HTML_COL_ALIGN_DESC
    col_def(6).valign = HTML_COL_VALIGN_DESC


    Const num_asm_inst_cols = 4
    Dim asm_inst_cols_def(1 To num_asm_inst_cols) As MY_HTML_COL_DEF
    asm_inst_cols_def(1).width = """10%"""    'Type
    asm_inst_cols_def(1).align = """center"""
    asm_inst_cols_def(1).valign = """top"""
    asm_inst_cols_def(2).width = """10%"""        'Instance
    asm_inst_cols_def(2).align = """center"""
    asm_inst_cols_def(2).valign = """top"""
    asm_inst_cols_def(3).width = """10%"""    'Usage
    asm_inst_cols_def(3).align = """center"""
    asm_inst_cols_def(3).valign = """top"""
    asm_inst_cols_def(4).width = """70%"""     'Name
    asm_inst_cols_def(4).align = """left"""
    asm_inst_cols_def(4).valign = """top"""

    Const num_asm_def_cols = 5
    Dim asm_def_cols_def(1 To num_asm_def_cols) As MY_HTML_COL_DEF
    asm_def_cols_def(1).width = """8%"""    'Byte Offset
    asm_def_cols_def(1).align = """center"""
    asm_def_cols_def(1).valign = """top"""
    asm_def_cols_def(2).width = """8%"""        'Word Offset
    asm_def_cols_def(2).align = """center"""
    asm_def_cols_def(2).valign = """top"""
    asm_def_cols_def(3).width = """8%"""    'Byte size
    asm_def_cols_def(3).align = """center"""
    asm_def_cols_def(3).valign = """top"""
    asm_def_cols_def(4).width = """16%"""    'Name
    asm_def_cols_def(4).align = """left"""
    asm_def_cols_def(4).valign = """top"""
    asm_def_cols_def(5).width = """60%"""     'Description
    asm_def_cols_def(5).align = """left"""
    asm_def_cols_def(5).valign = """top"""
    
    
    Const num_class_inst0_def_cols = 5
    Dim class_inst0_def_strings(1 To num_class_inst0_def_cols) As String
    class_inst0_def_strings(1) = "Test1"
    class_inst0_def_strings(2) = "Test2"
    class_inst0_def_strings(3) = "Test3"
    class_inst0_def_strings(4) = "Test4"
    class_inst0_def_strings(5) = "Test5"
    Dim class_inst0_def(1 To num_class_inst0_def_cols) As MY_HTML_COL_DEF
    class_inst0_def(1).width = """8%"""    'Attribute ID
    class_inst0_def(1).align = """center"""
    class_inst0_def(1).valign = """top"""
    class_inst0_def(2).width = """8%"""        'Access Rule
    class_inst0_def(2).align = """center"""
    class_inst0_def(2).valign = """top"""
    class_inst0_def(3).width = """8%"""    'Datatype
    class_inst0_def(3).align = """center"""
    class_inst0_def(3).valign = """top"""
    class_inst0_def(4).width = """16%"""    'Name
    class_inst0_def(4).align = """left"""
    class_inst0_def(4).valign = """top"""
    class_inst0_def(5).width = """60%"""     'Description
    class_inst0_def(5).align = """left"""
    class_inst0_def(5).valign = """top"""



'*********************
'******     Begin
'*********************
    
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select

    file_name = Cells(2, 2).value
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\"
    file_path = file_path & Worksheets("Usage Notes").Range("A1").Cells(GENERATED_DOCS_PATH_ROW, GENERATED_DOCS_PATH_COL).text
    If (gen_unpublished = True) Then
        file_name = file_name & "_UNPUB"
    End If
    file_path = file_path & file_name & ".html"
    Set GENED_DOC = fs.CreateTextFile(file_path, True)
    
    
'****************************************************************************************************************************
'******     Sort the list
'****************************************************************************************************************************
    Rows("9:1000").Sort Key1:=Range("C9"), Key2:=Range("D9"), Key3:=Range("E9"), Order1:=xlAscending, _
                Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
                Orientation:=xlTopToBottom

'****************************************************************************************************************************
'******     Start Creating the body surrounding the table.
'****************************************************************************************************************************
    GENED_DOC.WriteLine ("<html><body>")
    GENED_DOC.WriteLine ("")
    
    GENED_DOC.WriteLine ("<h1 style=" & HTML_TEXT_STYLE & ">")
    GENED_DOC.WriteLine (vbTab & "Filename: " & file_name)
    GENED_DOC.WriteLine ("</h1>")
    GENED_DOC.WriteLine ("<br/>")

'****************************************************************************************************************************
'******     First describe the general Class Instance 0 definition.
'****************************************************************************************************************************
    GENED_DOC.WriteLine ("<h1 style=" & HTML_TEXT_STYLE & "><li>")
    GENED_DOC.WriteLine (vbTab & " - Device Specific Objects Instance 0 Support")
    GENED_DOC.WriteLine ("</li></h1>")
    
    '********************************************
    '****** Create the table column headers
    GENED_DOC.WriteLine ("<table class=""reference"" cellspacing=""0"" cellpadding=""2"" border=""1"" width=""100%"" style=" & HTML_TEXT_STYLE & ">")
    class_inst0_def_strings(1) = "Attrib"
    class_inst0_def_strings(2) = "Access"
    class_inst0_def_strings(3) = "Data Type"
    class_inst0_def_strings(4) = "Name"
    class_inst0_def_strings(5) = "Description"
    GENED_DOC.WriteLine (Create_Table_Entry(class_inst0_def_strings, class_inst0_def, num_class_inst0_def_cols, True))
        
    class_inst0_def_strings(1) = "1"
    class_inst0_def_strings(2) = "Get"
    class_inst0_def_strings(3) = "UINT"
    
    class_inst0_def_strings(4) = "Revision"
    class_inst0_def_strings(5) = "Revision of the Object"
    GENED_DOC.WriteLine (Create_Table_Entry(class_inst0_def_strings, class_inst0_def, num_class_inst0_def_cols, False))
    
    class_inst0_def_strings(1) = "2"
    class_inst0_def_strings(4) = "Max Instances"
    class_inst0_def_strings(5) = "Maximum instance number of an object."
    GENED_DOC.WriteLine (Create_Table_Entry(class_inst0_def_strings, class_inst0_def, num_class_inst0_def_cols, False))
    
    class_inst0_def_strings(1) = "3"
    class_inst0_def_strings(4) = "Num Instances"
    class_inst0_def_strings(5) = "Number of instances of an object."
    GENED_DOC.WriteLine (Create_Table_Entry(class_inst0_def_strings, class_inst0_def, num_class_inst0_def_cols, False))

    GENED_DOC.WriteLine ("</table>")
    GENED_DOC.WriteLine ("<br/>")
    GENED_DOC.WriteLine ("<br/>")
    GENED_DOC.WriteLine ("")

'****************************************************************************************************************************
'******     Now lets run down the list and build the table.
'****************************************************************************************************************************
    
    Dim desc_sheet_row As Integer
    Dim reg_num As Long
    Dim coil_num As Long
    Dim name_string As String
    Dim desc_string As String
    present_class_num = 0
    Dim instance_string As String
    Dim current_instance As Integer

    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, DATA_UNPUBLISHED_COL).text = Empty) Or (gen_unpublished = True) Then
            desc_sheet_row = Find_DCID_Row(Cells(counter, DATA_DEFINE_COL).text)
            
            If (desc_sheet_row <> 0) Then
                '********************************************
                '****** Create the head of the table.  Only when we are starting a new class
                If (present_class_num <> Cells(counter, DATA_CLASS_COL).text) Then
                
                    present_class_num = Cells(counter, DATA_CLASS_COL).text
                    '********************************************
                    '****** Create the table header
                    GENED_DOC.WriteLine ("<h1 style=" & HTML_TEXT_STYLE & "><li>")
                    GENED_DOC.WriteLine (vbTab & "Class " & present_class_num & " (0x" & _
                                    Hexify(present_class_num, 1) & "): " & Cells(counter, DATA_CLASS_NAME_COL).text & " Object")
                    GENED_DOC.WriteLine ("</li></h1>")
                    
                    '********************************************
                    '****** Create the list of instances we go and count out all the instances and print them out.
                    inst_row_counter = counter
                    current_instance = Cells(inst_row_counter, DATA_INSTANCE_COL).text
                    instance_string = Cells(inst_row_counter, DATA_INSTANCE_COL).text
                    instance_counter = 1
                    While (Cells(inst_row_counter, DATA_CLASS_COL).text = present_class_num)
                        If (current_instance <> Cells(inst_row_counter, DATA_INSTANCE_COL).text) Then
                            instance_string = instance_string & ", " & Cells(inst_row_counter, DATA_INSTANCE_COL).text
                            instance_counter = instance_counter + 1
                            current_instance = Cells(inst_row_counter, DATA_INSTANCE_COL).text
                        End If
                        inst_row_counter = inst_row_counter + 1
                    Wend
                    GENED_DOC.WriteLine ("<p style=" & HTML_TEXT_STYLE & ">")
                    GENED_DOC.WriteLine (vbTab & "Instance Count: " & instance_counter & "<br/>")
                    GENED_DOC.WriteLine (vbTab & "Instance List: " & instance_string & "<br/>")
                    GENED_DOC.WriteLine ("</p>")
        
                    '********************************************
                    '****** Create the table column headers
                    GENED_DOC.WriteLine ("<table class=""reference"" cellspacing=""0"" cellpadding=""2"" border=""1"" width=""100%"" style=" & HTML_TEXT_STYLE & ">")
                    col_strings(1) = "Attrib"
                    col_strings(2) = "NV"
                    col_strings(3) = "Access"
                    col_strings(4) = "Data Type"
                    col_strings(5) = "Name"
                    col_strings(6) = "Description"
                    GENED_DOC.WriteLine (Create_Table_Entry(col_strings, col_def, num_cols, True))
                    
                    '********************************************
                    '****** Create the Assembly template header since it will be defined later.
                    current_instance = Cells(counter, DATA_INSTANCE_COL).text
                    If (present_class_num = 4) Then
                        col_strings(1) = "3"
                        col_strings(2) = "V"
                        col_strings(3) = "Set/Get"
                        col_strings(4) = "Array of BYTE"
                        col_strings(5) = "Data"
                        col_strings(6) = "Assembly Data.  See sections below for instance definition."
                        GENED_DOC.WriteLine (Create_Table_Entry(col_strings, col_def, num_cols))
                        GENED_DOC.WriteLine ("</table>")
                        GENED_DOC.WriteLine ("<br/>")
                        GENED_DOC.WriteLine ("<br/>")
                    End If
                End If
        
                '*********************************************************************************************
                '*********************************************************************************************
                '****** We handle the assembly objects differently
                '*********************************************************************************************
                '*********************************************************************************************
                If (present_class_num = 4) Then
            
                    '********************************************
                    '****** Create the table header
                    GENED_DOC.WriteLine ("<h2 style=" & HTML_TEXT_STYLE & "><li>")
                    GENED_DOC.WriteLine (vbTab & "Assembly Instance List:")
                    GENED_DOC.WriteLine ("</li></h2>")
                    
                    '********************************************
                    '****** Create the list of instances.  This is the summary of assembly instances.
                    GENED_DOC.WriteLine ("<table class=""reference"" cellspacing=""0"" cellpadding=""2"" border=""1"" width=""100%"" style=" & HTML_TEXT_STYLE & ">")
                    col_strings(1) = "Type"
                    col_strings(2) = "Instance"
                    col_strings(3) = "Usage"
                    col_strings(4) = "Name"
                    GENED_DOC.WriteLine (Create_Table_Entry(col_strings, asm_inst_cols_def, num_asm_inst_cols, True))
                    
                    inst_row_counter = counter
                    current_instance = 0
                    While (Cells(inst_row_counter, DATA_CLASS_COL).text = present_class_num)
                        If ((current_instance <> Cells(inst_row_counter, DATA_INSTANCE_COL).text) And _
                                    Not (Cells(inst_row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).value > 0)) Then
                            desc_sheet_row = Find_DCID_Row(Cells(inst_row_counter, DATA_DEFINE_COL).text)
                            If (desc_sheet_row <> 0) Then
                                If (Cells(inst_row_counter, DATA_ASSEMBLY_PRODUCED_COL).text = Empty) Then
                                    col_strings(1) = "Output"
                                Else
                                    col_strings(1) = "Input"
                                End If
                                col_strings(2) = Cells(inst_row_counter, DATA_INSTANCE_COL).text & _
                                                " (0x" & Hexify(Cells(inst_row_counter, DATA_INSTANCE_COL).text, 1) & ")"
                                col_strings(3) = "Poll"
                                If (Cells(inst_row_counter, DATA_ALT_SHORT_NAME_COL).text <> Empty) Then
                                    col_strings(4) = Cells(inst_row_counter, DATA_ALT_SHORT_NAME_COL).text
                                Else
                                    col_strings(4) = Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_SHORT_DESC_COL).text
                                End If
                                GENED_DOC.WriteLine (Create_Table_Entry(col_strings, asm_inst_cols_def, num_asm_inst_cols))
                            End If
                        End If
                        current_instance = Cells(inst_row_counter, DATA_INSTANCE_COL).text
                        inst_row_counter = inst_row_counter + 1
                    Wend
                    
                    GENED_DOC.WriteLine ("</table>")
                    GENED_DOC.WriteLine ("<br/>")
                    GENED_DOC.WriteLine ("<br/>")
                    
                    '***********************************************************************************
                    '****** This is where we actually define the specific details associated with the instances
                    GENED_DOC.WriteLine ("<h2 style=" & HTML_TEXT_STYLE & "><li>")
                    GENED_DOC.WriteLine (vbTab & "Assembly Instance Definitions:")
                    GENED_DOC.WriteLine ("</li></h2>")
                    GENED_DOC.WriteLine ("<p style=" & HTML_TEXT_STYLE & ">")
                    GENED_DOC.WriteLine (vbTab & "All Assembly data is little endian (low byte first).<br/>")
                    GENED_DOC.WriteLine ("</p>")
    
                    prev_instance = 0
                    offset_counter = 0
                    inst_row_counter = counter
                    While (Cells(inst_row_counter, DATA_CLASS_COL).text = present_class_num)
                        desc_sheet_row = Find_DCID_Row(Cells(inst_row_counter, DATA_DEFINE_COL).text)
                        If (desc_sheet_row <> 0) Then
                            If (Cells(inst_row_counter, DATA_ALT_SHORT_NAME_COL).text <> Empty) Then
                                name_string = Cells(inst_row_counter, DATA_ALT_SHORT_NAME_COL).text
                            Else
                                name_string = Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_SHORT_DESC_COL).text
                            End If
                            name_string = Format_String_For_HTML(name_string)
                            
                            '*** Get the description and wrap it up.
                            If (Cells(inst_row_counter, DATA_ALT_DESCRIPTION_COL).text <> Empty) Then
                                desc_string = Cells(inst_row_counter, DATA_ALT_DESCRIPTION_COL).text
                            ElseIf (Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_LONG_DESC_COL).text <> Empty) Then
                                desc_string = Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_LONG_DESC_COL).text
                            Else
                                desc_string = "-"
                            End If

                            '*** Get the units and wrap it up.
                            If (Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_UNITS_DESC_COL).text <> Empty) Then
                                desc_string = desc_string & " (" & _
                                        Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_UNITS_DESC_COL).text & ")"
                            End If
                            desc_string = Format_String_For_HTML(desc_string)
                            
                            If ((prev_instance <> Cells(inst_row_counter, DATA_INSTANCE_COL).text) And _
                                        Not (Cells(inst_row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).value > 0)) Then
                                GENED_DOC.WriteLine ("<h3 style=" & HTML_TEXT_STYLE & "><li>")
                                GENED_DOC.WriteLine (vbTab & "Assembly Instance " & Cells(inst_row_counter, DATA_INSTANCE_COL).text & _
                                                        " (0x" & Hexify(Cells(inst_row_counter, DATA_INSTANCE_COL).value, 1) & "): " & name_string)
                                GENED_DOC.WriteLine ("</li></h3>")
                                
                                asm_head_search_ctr = START_ROW
                                While (Cells(inst_row_counter, DATA_INSTANCE_COL).text <> Cells(asm_head_search_ctr, DATA_INSTANCE_COL).text)
                                    asm_head_search_ctr = asm_head_search_ctr + 1
                                Wend
                                If (Cells(asm_head_search_ctr, DATA_ASSEMBLY_ALIGNMENT_COL).text <> Empty) Then
                                    align_size = Cells(asm_head_search_ctr, DATA_ASSEMBLY_ALIGNMENT_COL).value
                                Else
                                    align_size = 1
                                End If
                                GENED_DOC.WriteLine ("<p style=" & HTML_TEXT_STYLE & ">")
                                GENED_DOC.WriteLine (vbTab & desc_string & "<br/>")
                                If (Cells(inst_row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).text <> Empty) Then
                                    GENED_DOC.WriteLine (vbTab & "Word Size (bytes): " & align_size & "<br/>")
                                End If
                                GENED_DOC.WriteLine ("</p>")
                                GENED_DOC.WriteLine ("<table class=""reference"" cellspacing=""0"" cellpadding=""2"" border=""1"" width=""100%"" style=" & HTML_TEXT_STYLE & ">")
                                col_strings(1) = "Byte Offset"
                                col_strings(2) = "Word Offset"
                                col_strings(3) = "Size (bytes)"
                                col_strings(4) = "Name"
                                col_strings(5) = "Description"
                                GENED_DOC.WriteLine (Create_Table_Entry(col_strings, asm_def_cols_def, num_asm_def_cols, True))
                                offset_counter = 0
                            End If
                            If ((Cells(inst_row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).text = Empty) Or _
                                          (Cells(inst_row_counter, DATA_ASSEMBLY_ITEM_INDEX_COL).value > 0)) Then
                                col_strings(1) = offset_counter         'This is the byte index
                                col_strings(2) = offset_counter / align_size        ' This is the word offset
                                offset_counter = offset_counter + Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, TOTAL_LENGTH_COL).text
                                offset_counter = offset_counter + (Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, TOTAL_LENGTH_COL).text Mod align_size)
                                col_strings(3) = Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, TOTAL_LENGTH_COL).text
                                col_strings(4) = name_string
                                col_strings(5) = desc_string
                                GENED_DOC.WriteLine (Create_Table_Entry(col_strings, asm_def_cols_def, num_asm_def_cols))
                            End If
                            prev_instance = Cells(inst_row_counter, DATA_INSTANCE_COL).text
                            If (Cells(inst_row_counter, DATA_INSTANCE_COL).text <> Cells(inst_row_counter + 1, DATA_INSTANCE_COL).text) Then
                                GENED_DOC.WriteLine ("</table>")
                                GENED_DOC.WriteLine ("<p style=" & HTML_TEXT_STYLE & ">")
                                GENED_DOC.WriteLine (vbTab & "Total Assembly Size (bytes): " & offset_counter & "<br/>")
                                GENED_DOC.WriteLine ("</p>")
                                GENED_DOC.WriteLine ("<br/>")
                                GENED_DOC.WriteLine ("<br/>")
                            End If
                        End If
                        inst_row_counter = inst_row_counter + 1
                    Wend
                    counter = inst_row_counter - 1
                Else
                '*********************************************************************************************
                '*********************************************************************************************
                '****** Here we handle the regular objects.
                '*********************************************************************************************
                '*********************************************************************************************
            
                    present_instance_num = Cells(counter, DATA_INSTANCE_COL).text
                    present_attrib_num = Cells(counter, DATA_ATTRIBUTE_COL).text
        
                    '****** Make sure that we only document one instance.
                    If (present_instance_num = current_instance) Then
                        col_strings(1) = present_attrib_num & "  (0x" & Hexify(present_attrib_num, 1) & ")"
                        
                        If (Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, PATRON_INIT_VAL_WRITEABLE_ACCESS_COL).text <> Empty) Then
                            col_strings(2) = "NV"
                        Else
                            col_strings(2) = "V"
                        End If
                        
                        If (Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, PATRON_RAM_VAL_WRITEABLE_COL).text <> Empty) Then
                            col_strings(3) = "Set/Get"
                        Else
                            col_strings(3) = "Get"
                        End If
                        
                        If (Cells(counter, DATA_ALT_DATATYPE_COL).text <> Empty) Then
                            col_strings(4) = Cells(counter, DATA_ALT_DATATYPE_COL).text
                        ElseIf (Cells(counter, DATA_BITWISE_BOOL_COL).text <> Empty) Then
                            col_strings(4) = Get_CIP_Datatype_String("DCI_DTYPE_BOOL")
                        ElseIf (Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_DATATYPE_COL).text = "DCI_DTYPE_STRING8") And _
                                    (Cells(counter, DATA_CIP_SHORT_STRING_COL).text = Empty) Then
                            ' This makes sure that if we have a DCI string that is not listed as a CIP string we treat it just as a string of UINT8s.
                            col_strings(4) = Get_CIP_Datatype_String("DCI_DTYPE_UINT8")
                        ElseIf (Cells(counter, DATA_ALTERNATE_LEN_COL).text <> Empty) Then
                            col_strings(4) = Get_CIP_Datatype_String(Get_Datatype_With_Alt_Len(Cells(counter, DATA_ALTERNATE_LEN_COL).value, _
                                    Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_DATATYPE_COL).text))
                        Else
                            col_strings(4) = Get_CIP_Datatype_String(Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_DATATYPE_COL).text)
                        End If
                        
                        If (Cells(counter, DATA_ALT_SHORT_NAME_COL).text <> Empty) Then
                            col_strings(5) = Cells(counter, DATA_ALT_SHORT_NAME_COL).text
                        Else
                            col_strings(5) = Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_SHORT_DESC_COL).text
                        End If

                        '*** Get the description and wrap it up.
                        If (Cells(counter, DATA_ALT_DESCRIPTION_COL).text <> Empty) Then
                            col_strings(6) = Cells(counter, DATA_ALT_DESCRIPTION_COL).text
                        ElseIf (Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_LONG_DESC_COL).text <> Empty) Then
                            col_strings(6) = Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_LONG_DESC_COL).text
                        Else
                            col_strings(6) = "-"
                        End If

                        '*** Get the units and wrap it up.
                        If (Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_UNITS_DESC_COL).text <> Empty) Then
                            col_strings(6) = col_strings(6) & " (" & _
                                    Worksheets("DCI Descriptors").Range("A1").Cells(desc_sheet_row, DCI_UNITS_DESC_COL).text & ")"
                        End If
                        col_strings(6) = Format_String_For_HTML(col_strings(6))

                        GENED_DOC.WriteLine (Create_Table_Entry(col_strings, col_def, num_cols))
                    End If
    
                    If (present_class_num <> Cells(counter + 1, DATA_CLASS_COL).text) Then
                        GENED_DOC.WriteLine ("</table>")
                        GENED_DOC.WriteLine ("<br/>")
                        GENED_DOC.WriteLine ("<br/>")
                    End If
                End If
            End If
        End If
        counter = counter + 1
    Wend
    GENED_DOC.WriteLine ("")
    GENED_DOC.WriteLine ("</body></html>")

    Rows("9:499").Sort Key1:=Range("C9"), Key2:=Range("E9"), Key3:=Range("D9"), Order1:=xlAscending, _
        Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
        Orientation:=xlTopToBottom

    GENED_DOC.Close

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
Function Create_Table_Entry(col_strings() As String, col_def() As MY_HTML_COL_DEF, num_cols As Integer, Optional header_row As Boolean = False) As String
    Dim col_ctr As Integer
    Dim tcell As String
    
    If (header_row = False) Then
        tcell = "td"
    Else
        tcell = "th"
    End If
    
    return_string = ""
    return_string = vbTab & "<tr>" & vbCrLf
    col_ctr = 1
    While (col_ctr <= num_cols)
        '****** Create the Column
        return_string = return_string & vbTab & vbTab & "<" & tcell & _
                            " width=" & col_def(col_ctr).width & _
                            " align=" & col_def(col_ctr).align & _
                            " valign=" & col_def(col_ctr).valign & _
                            ">" & vbCrLf
        return_string = return_string & vbTab & vbTab & vbTab & Format_String_For_HTML(col_strings(col_ctr)) & vbCrLf
        return_string = return_string & vbTab & vbTab & "</" & tcell & ">" & vbCrLf
        col_ctr = col_ctr + 1
    Wend
    return_string = return_string & vbTab & "</tr>" & vbCrLf

    Create_Table_Entry = return_string
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
Function Create_Table_Entry1(col0 As String, col1 As String, col2 As String, col3 As String, col4 As String, col5 As String, colt() As String, num As Integer) As String
    
    return_string = ""
    '****** Create the Column 0
    return_string = vbTab & "<tr>" & vbCrLf
    return_string = return_string & vbTab & vbTab & "<td width=" & HTML_COL_WIDTH_ATTRIB & " align=" & HTML_COL_ALIGN_ATTRIB & ">" & vbCrLf
    return_string = return_string & vbTab & vbTab & vbTab & col0 & vbCrLf
    return_string = return_string & vbTab & vbTab & "</td>" & vbCrLf
    '****** Create the Column 1
    return_string = return_string & vbTab & vbTab & "<td width=" & HTML_COL_WIDTH_NV & " align=" & HTML_COL_ALIGN_NV & ">" & vbCrLf
    return_string = return_string & vbTab & vbTab & vbTab & col1 & vbCrLf
    return_string = return_string & vbTab & vbTab & "</td>" & vbCrLf
    '****** Create the Column 2
    return_string = return_string & vbTab & vbTab & "<td width=" & HTML_COL_WIDTH_ACCESS & " align=" & HTML_COL_ALIGN_ACCESS & ">" & vbCrLf
    return_string = return_string & vbTab & vbTab & vbTab & col2 & vbCrLf
    return_string = return_string & vbTab & vbTab & "</td>" & vbCrLf
    '****** Create the Column 3
    return_string = return_string & vbTab & vbTab & "<td width=" & HTML_COL_WIDTH_DTYPE & " align=" & HTML_COL_ALIGN_DTYPE & ">" & vbCrLf
    return_string = return_string & vbTab & vbTab & vbTab & col3 & vbCrLf
    return_string = return_string & vbTab & vbTab & "</td>" & vbCrLf
    '****** Create the Column 4
    return_string = return_string & vbTab & vbTab & "<td width=" & HTML_COL_WIDTH_NAME & " align=" & HTML_COL_ALIGN_NAME & ">" & vbCrLf
    return_string = return_string & vbTab & vbTab & vbTab & col4 & vbCrLf
    return_string = return_string & vbTab & vbTab & "</td>" & vbCrLf
    '****** Create the Column 5
    return_string = return_string & vbTab & vbTab & "<td width=" & HTML_COL_WIDTH_DESC & " align=" & HTML_COL_ALIGN_DESC & ">" & vbCrLf
    return_string = return_string & vbTab & vbTab & vbTab & col5 & vbCrLf
    return_string = return_string & vbTab & vbTab & "</td>" & vbCrLf
    '****** Create the close
    return_string = return_string & vbTab & "</tr>" & vbCrLf

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
Function Format_String_For_HTML(source_str As String) As String
    Dim temp_string As String
    temp_string = source_str

    'This is just in case we happen to run over this twice.
    'We undo what we might have done.
    temp_string = Replace(temp_string, "&amp;", "&")
    temp_string = Replace(temp_string, "&nbsp&nbsp", "  ")
    temp_string = Replace(temp_string, "&lt;", "<")
    temp_string = Replace(temp_string, "&gt;", ">")
    temp_string = Replace(temp_string, "&quot;", """")
    temp_string = Replace(temp_string, "&apos;", "'")
    temp_string = Replace(temp_string, "&#176;", "º")
    temp_string = Replace(temp_string, "<br/>", vbCr & vbLf)
    temp_string = Replace(temp_string, "<br>", vbCr & vbLf)
    

    ' this needs to be the first for any reference replacement
    temp_string = Replace(temp_string, "&", "&amp;")
    
    temp_string = Replace(temp_string, "  ", "&nbsp&nbsp")
    
    temp_string = Replace(temp_string, "<", "&lt;")
    temp_string = Replace(temp_string, ">", "&gt;")
    temp_string = Replace(temp_string, """", "&quot;")
    temp_string = Replace(temp_string, "'", "&apos;")
    
    temp_string = Replace(temp_string, "º", "&#176;")
    
    temp_string = Replace(temp_string, vbCr & vbLf, "<br/>")
    temp_string = Replace(temp_string, vbLf & vbCr, "<br/>")
    temp_string = Replace(temp_string, vbCr, "<br/>")
    temp_string = Replace(temp_string, vbLf, "<br/>")
    
    Format_String_For_HTML = temp_string

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
Public Sub Verify_EtherNetIP_List()

'*********************
'******     Constants
'*********************
    Application.Calculation = xlCalculationManual
    
    Dim BEGIN_IGNORED_DATA As String
    Dim sheet_name As String
    Dim lookup_string As String
    Dim dci_sheet_row As Integer
    
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    
'*********************
'******     Begin
'*********************
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select

'****************************************************************************************************************************
'******     Find where we should add the unadded items.
'****************************************************************************************************************************
    missing_row_counter = START_ROW
    While (Cells(missing_row_counter, DATA_DEFINE_COL).text <> Empty)
        missing_row_counter = missing_row_counter + 1
    Wend

'****************************************************************************************************************************
'******     Find the missing data items
'****************************************************************************************************************************
    dci_defines_row_ctr = DCI_DEFINES_START_ROW
    While Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> Empty
        lookup_string = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text
        Set C = Worksheets(sheet_name).Range("B9:AZ1000").Find(lookup_string, LookAt:=xlWhole)
        If C Is Nothing Then
            Cells(missing_row_counter, DATA_DESC_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DESCRIPTORS_COL).text
            Cells(missing_row_counter, DATA_DEFINE_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text
            missing_row_counter = missing_row_counter + 1
        End If
        dci_defines_row_ctr = dci_defines_row_ctr + 1
    Wend


    Sheets(sheet_name).Select
'****************************************************************************************************************************
'******     Find the Data items that have been removed from the main list
'****************************************************************************************************************************
    dci_sheet_row = 0
    sheet_row_ctr = START_ROW
    missing_counter = START_ROW
    While Cells(sheet_row_ctr, DATA_DEFINE_COL).text <> Empty
        lookup_string = Cells(sheet_row_ctr, DATA_DEFINE_COL).text
        If lookup_string <> BEGIN_IGNORED_DATA Then
            dci_sheet_row = Get_DCI_Row(lookup_string)
            If dci_sheet_row = 0 Then
                Cells(sheet_row_ctr, DATA_DEFINE_COL).Interior.Color = RGB(255, 0, 0)
                Cells(sheet_row_ctr, DESCRIP_COL).Interior.Color = RGB(255, 0, 0)
            Else
                Cells(sheet_row_ctr, DESCRIP_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_sheet_row, DCI_DESCRIPTORS_COL).text
            End If
        End If
        sheet_row_ctr = sheet_row_ctr + 1
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
Sub Clear_EIP_Ignored_Items()
    Application.Calculation = xlCalculationManual
    
    Dim BEGIN_IGNORED_DATA As String
    Dim sheet_name As String
    
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"

    sheet_row_ctr = START_ROW
    While (Cells(sheet_row_ctr, DATA_DEFINE_COL).text <> Empty) And _
        (Cells(sheet_row_ctr, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        sheet_row_ctr = sheet_row_ctr + 1
    Wend
    sheet_row_ctr = sheet_row_ctr + 1
    
    Range(Cells(sheet_row_ctr, 1), Cells(10000, 255)).Delete (xlShiftUp)
    
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

Public Function Hexify(num_to_hex As Long, bytes As Integer) As String
    
    If (bytes = 4) Then
        If (num_to_hex < 16) Then
            Hexify = "0000000" & Hex(num_to_hex)
        ElseIf (num_to_hex < 256) Then
            Hexify = "000000" & Hex(num_to_hex)
        ElseIf (num_to_hex < 4096) Then
            Hexify = "00000" & Hex(num_to_hex)
        ElseIf (num_to_hex < 65536) Then
            Hexify = "0000" & Hex(num_to_hex)
        ElseIf (num_to_hex < 1048576) Then
            Hexify = "000" & Hex(num_to_hex)
        ElseIf (num_to_hex < 16777216) Then
            Hexify = "00" & Hex(num_to_hex)
        ElseIf (num_to_hex < 268435456) Then
            Hexify = "0" & Hex(num_to_hex)
        Else
            Hexify = Hex(num_to_hex)
        End If
    ElseIf (bytes = 2) Then
        If (num_to_hex < 16) Then
            Hexify = "000" & Hex(num_to_hex)
        ElseIf (num_to_hex < 256) Then
            Hexify = "00" & Hex(num_to_hex)
        ElseIf (num_to_hex < 4096) Then
            Hexify = "0" & Hex(num_to_hex)
        Else
            Hexify = Hex(num_to_hex)
        End If
    ElseIf (bytes = 1) Then
        If (num_to_hex < 16) Then
            Hexify = "0" & Hex(num_to_hex)
        Else
            Hexify = Hex(num_to_hex)
        End If
    Else
        Hexify = Hex(num_to_hex)
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
Public Function Create_Dec_CIA(class As Long, instance As Long, attrib As Long) As String
    Dim return_string As String
    
    return_string = class
    If (instance < 10) Then
        return_string = return_string & "00" & instance
    ElseIf (instance < 100) Then
        return_string = return_string & "0" & instance
    Else
        return_string = return_string & instance
    End If
    
    If (attrib < 10) Then
        return_string = return_string & "00" & attrib
    ElseIf (instance < 100) Then
        return_string = return_string & "0" & attrib
    Else
        return_string = return_string & attrib
    End If

    Create_Dec_CIA = return_string
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
    
    Spacify = my_string & Create_Space(my_string, col_pos)
    
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
Public Function Tabify(my_string As String, col_pos As Integer) As String
    
    TAB_SIZE = 4
    Dim string_len As Integer
    Dim tab_cnt As Integer
    Dim tab_cnt_pos As Integer
    Dim num_tabs As Integer
    
    Tabify = my_string
    string_len = Len(my_string)
    
    tab_cnt_pos = 1
    tab_cnt = 0
    While (tab_cnt_pos < string_len) And (tab_cnt_pos <> 0)
        tab_cnt_pos = InStr(tab_cnt_pos, my_string, vbTab, vbBinaryCompare)
        If (tab_cnt_pos <> 0) Then
            tab_cnt = tab_cnt + 1
            tab_cnt_pos = tab_cnt_pos + 1
        End If
    Wend
    string_len = string_len + (tab_cnt * 3)

    If (col_pos > string_len) Then
        num_tabs = WorksheetFunction.Ceiling((col_pos - string_len) / TAB_SIZE, 1)
'        num_tabs = Trunc((col_pos - string_len + (TAB_SIZE - 1)) / TAB_SIZE)
    Else
        num_tabs = 1
    End If
    
    While (num_tabs > 0)
        my_string = my_string & Chr(9)
        num_tabs = num_tabs - 1
    Wend
    
    Tabify = my_string
   
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

Public Function Get_CIP_Datatype(my_s As String) As String
    Dim return_string As String
    
    Select Case my_s
        Case "DCI_DTYPE_BOOL"
            return_string = "0xC1"
        Case "DCI_DTYPE_SINT8"
            return_string = "0xC2"
        Case "DCI_DTYPE_SINT16"
            return_string = "0xC3"
        Case "DCI_DTYPE_SINT32"
            return_string = "0xC4"
        Case "DCI_DTYPE_SINT64"
            return_string = "0xC5"
        Case "DCI_DTYPE_UINT8"
            return_string = "0xC6"   '6
        Case "DCI_DTYPE_UINT16"
            return_string = "0xC7"
        Case "DCI_DTYPE_UINT32"
            return_string = "0xC8"
        Case "DCI_DTYPE_UINT64"
            return_string = "0xC9"
        Case "DCI_DTYPE_FLOAT"
            return_string = "0xCA"
        Case "DCI_DTYPE_LFLOAT"
            return_string = "0xCB"
        Case "DCI_DTYPE_STRING8"
            return_string = "0xD0"
        Case "DCI_DTYPE_BYTE"
            return_string = "0xD1"
        Case "DCI_DTYPE_WORD"
            return_string = "0xD2"
        Case "DCI_DTYPE_DWORD"
            return_string = "0xD3"
        Case Else
            return_string = "0xC1"
    End Select
    
    Get_CIP_Datatype = return_string
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
Public Function Get_Datatype_With_Alt_Len(alt_len As Integer, my_s As String) As String
    Dim type_string As String
    
    Select Case alt_len
        Case 1
            Select Case my_s
                Case "DCI_DTYPE_BOOL"
                    type_string = "DCI_DTYPE_BOOL"
                Case "DCI_DTYPE_BYTE", "DCI_DTYPE_WORD", "DCI_DTYPE_DWORD"
                    type_string = "DCI_DTYPE_BYTE"
                Case "DCI_DTYPE_SINT8", "DCI_DTYPE_SINT16", "DCI_DTYPE_SINT32", "DCI_DTYPE_SINT64"
                    type_string = "DCI_DTYPE_SINT8"
                Case "DCI_DTYPE_UINT8", "DCI_DTYPE_UINT16", "DCI_DTYPE_UINT32", "DCI_DTYPE_UINT64"
                    type_string = "DCI_DTYPE_UINT8"
                Case "DCI_DTYPE_FLOAT", "DCI_DTYPE_LFLOAT"
                    type_string = my_s          '  This is actually an error.
                Case Else
                    type_string = my_s
            End Select
            
        Case 2
            Select Case my_s
                Case "DCI_DTYPE_BOOL"
                    type_string = "DCI_DTYPE_WORD"
                Case "DCI_DTYPE_BYTE", "DCI_DTYPE_WORD", "DCI_DTYPE_DWORD"
                    type_string = "DCI_DTYPE_WORD"
                Case "DCI_DTYPE_SINT8", "DCI_DTYPE_SINT16", "DCI_DTYPE_SINT32", "DCI_DTYPE_SINT64"
                    type_string = "DCI_DTYPE_SINT16"
                Case "DCI_DTYPE_UINT8", "DCI_DTYPE_UINT16", "DCI_DTYPE_UINT32", "DCI_DTYPE_UINT64"
                    type_string = "DCI_DTYPE_UINT16"
                Case "DCI_DTYPE_FLOAT", "DCI_DTYPE_LFLOAT"
                    type_string = my_s          '  This is actually an error.
                Case Else
                    type_string = my_s
            End Select

        Case 4
            Select Case my_s
                Case "DCI_DTYPE_BOOL"
                    type_string = "DCI_DTYPE_DWORD"
                Case "DCI_DTYPE_BYTE", "DCI_DTYPE_WORD", "DCI_DTYPE_DWORD"
                    type_string = "DCI_DTYPE_DWORD"
                Case "DCI_DTYPE_SINT8", "DCI_DTYPE_SINT16", "DCI_DTYPE_SINT32", "DCI_DTYPE_SINT64"
                    type_string = "DCI_DTYPE_SINT32"
                Case "DCI_DTYPE_UINT8", "DCI_DTYPE_UINT16", "DCI_DTYPE_UINT32", "DCI_DTYPE_UINT64"
                    type_string = "DCI_DTYPE_UINT32"
                Case "DCI_DTYPE_FLOAT", "DCI_DTYPE_LFLOAT"
                    type_string = my_s          '  This is actually an error.
                Case Else
                    type_string = my_s
            End Select

        Case Else
            type_string = my_s
    End Select

    Get_Datatype_With_Alt_Len = type_string
    
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
Public Function Get_CIP_Datatype_Min_String(my_s As String) As String
    Dim exit_string As String

    If (my_s = "DCI_DTYPE_BOOL") Then
        exit_string = "0"
    ElseIf (my_s = "DCI_DTYPE_SINT8") Then
        exit_string = "0x80"
    ElseIf (my_s = "DCI_DTYPE_SINT16") Then
        exit_string = "0x8000"
    ElseIf (my_s = "DCI_DTYPE_SINT32") Then
        exit_string = "0x80000000"
    ElseIf (my_s = "DCI_DTYPE_SINT64") Then
        exit_string = "0x8000000000000000"
    ElseIf (my_s = "DCI_DTYPE_UINT8") Then
        exit_string = "0"   '6
    ElseIf (my_s = "DCI_DTYPE_UINT16") Then
        exit_string = "0"
    ElseIf (my_s = "DCI_DTYPE_UINT32") Then
        exit_string = "0"
    ElseIf (my_s = "DCI_DTYPE_UINT64") Then
        exit_string = "0"
    ElseIf (my_s = "DCI_DTYPE_FLOAT") Then
        exit_string = "-8388607"
    ElseIf (my_s = "DCI_DTYPE_LFLOAT") Then
        exit_string = "-4503599627370495"
    ElseIf (my_s = "DCI_DTYPE_STRING8") Then
        exit_string = "0"
    ElseIf (my_s = "DCI_DTYPE_BYTE") Then
        exit_string = "0"
    ElseIf (my_s = "DCI_DTYPE_WORD") Then
        exit_string = "0"
    ElseIf (my_s = "DCI_DTYPE_DWORD") Then
        exit_string = "0"
    End If
    Get_CIP_Datatype_Min_String = exit_string
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
Public Function Get_CIP_Datatype_Max_String(my_s As String) As String
    Dim exit_string As String

    If (my_s = "DCI_DTYPE_BOOL") Then
        exit_string = "1"
    ElseIf (my_s = "DCI_DTYPE_SINT8") Then
        exit_string = "0xFF"
    ElseIf (my_s = "DCI_DTYPE_SINT16") Then
        exit_string = "0x7FFF"
    ElseIf (my_s = "DCI_DTYPE_SINT32") Then
        exit_string = "0x7FFFFFFF"
    ElseIf (my_s = "DCI_DTYPE_SINT64") Then
        exit_string = "0x7FFFFFFFFFFFFFFF"
    ElseIf (my_s = "DCI_DTYPE_UINT8") Then
        exit_string = "0xFF"
    ElseIf (my_s = "DCI_DTYPE_UINT16") Then
        exit_string = "0xFFFF"
    ElseIf (my_s = "DCI_DTYPE_UINT32") Then
        exit_string = "0xFFFFFFFF"
    ElseIf (my_s = "DCI_DTYPE_UINT64") Then
        exit_string = "0xFFFFFFFFFFFFFFFF"
    ElseIf (my_s = "DCI_DTYPE_FLOAT") Then
        exit_string = "8388607"
    ElseIf (my_s = "DCI_DTYPE_LFLOAT") Then
        exit_string = "4503599627370495"
    ElseIf (my_s = "DCI_DTYPE_STRING8") Then
        exit_string = "0xFF"
    ElseIf (my_s = "DCI_DTYPE_BYTE") Then
        exit_string = "0xFF"
    ElseIf (my_s = "DCI_DTYPE_WORD") Then
        exit_string = "0xFFFF"
    ElseIf (my_s = "DCI_DTYPE_DWORD") Then
        exit_string = "0xFFFFFFFF"
    End If
    Get_CIP_Datatype_Max_String = exit_string
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
Public Function Get_ID(row_number As Long) As String
    Dim exit_string As String
    
    exit_string = row_number - 9 + 1    ' Start row + 1 essentially.
    Get_ID = exit_string
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
Public Function Get_DCI_Row(DCI_STRING As String) As Integer

    dci_defines_row_ctr = DCI_DEFINES_START_ROW
    While (Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> Empty) And _
            (Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> DCI_STRING)
        dci_defines_row_ctr = dci_defines_row_ctr + 1
    Wend
    
    If (Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text = Empty) Then
        Get_DCI_Row = 0
    Else
        Get_DCI_Row = dci_defines_row_ctr
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
Public Function Create_Space(my_s As String, align_pos As Integer) As String
    If ((Len(my_s) + 2) >= align_pos) Then
        Create_Space = Space(4)
    Else
        Create_Space = Space(align_pos - Len(my_s))
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
Public Function Get_CIP_Datatype_String(my_s As String) As String
    Dim result_string As String
    
    result_string = ""
    If (my_s = "DCI_DTYPE_BOOL") Then
        result_string = "BOOL"
    ElseIf (my_s = "DCI_DTYPE_SINT8") Then
        result_string = "SINT"
    ElseIf (my_s = "DCI_DTYPE_SINT16") Then
        result_string = "INT"
    ElseIf (my_s = "DCI_DTYPE_SINT32") Then
        result_string = "DINT"
    ElseIf (my_s = "DCI_DTYPE_SINT64") Then
        result_string = "LINT"
    ElseIf (my_s = "DCI_DTYPE_UINT8") Then
        result_string = "USINT"
    ElseIf (my_s = "DCI_DTYPE_UINT16") Then
        result_string = "UINT"
    ElseIf (my_s = "DCI_DTYPE_UINT32") Then
        result_string = "UDINT"
    ElseIf (my_s = "DCI_DTYPE_UINT64") Then
        result_string = "ULINT"
    ElseIf (my_s = "DCI_DTYPE_FLOAT") Then
        result_string = "REAL"
    ElseIf (my_s = "DCI_DTYPE_LFLOAT") Then
        result_string = "LREAL"
    ElseIf (my_s = "DCI_DTYPE_STRING8") Then
        result_string = "SHORT_STRING"
    ElseIf (my_s = "DCI_DTYPE_BYTE") Then
        result_string = "BYTE"
    ElseIf (my_s = "DCI_DTYPE_WORD") Then
        result_string = "WORD"
    ElseIf (my_s = "DCI_DTYPE_DWORD") Then
        result_string = "DWORD"
    End If

    Get_CIP_Datatype_String = result_string

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
Sub TestRead()


'*********************
'******     Constants
'*********************
    Dim read_string As String
    

'*********************
'******     Begin
'*********************
    
    file_name = "readthis.txt"
    lower_case_file_name = LCase(file_name)
    upper_case_file_name = UCase(file_name)
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\"
    file_path = file_path & Worksheets("Usage Notes").Range("A1").Cells(GENERATED_DOCS_PATH_ROW, GENERATED_DOCS_PATH_COL).text
    Set EDS_SRC_FILE = fs.OpenTextFile(file_path & file_name, 1)

    While (EDS_SRC_FILE.AtEndOfStream <> True)
        read_string = EDS_SRC_FILE.ReadLine()
    Wend


End Sub





