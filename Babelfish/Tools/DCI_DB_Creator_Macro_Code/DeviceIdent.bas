Attribute VB_Name = "DeviceIdent"
Sub Create_DeviceIdentity_Def()

'*********************
'******     Constants
'*********************
    
    Application.Calculation = xlCalculationManual
    
    Dim sheet_name As String

    Dim file_name As String
    Dim lower_case_file_name As String
    Dim upper_case_file_name As String

    
    'Sheet File Indexing.
    START_ROW = 9
    PROD_DESCRIP_COL = 1
    PROD_CODE_DEFINE_COL = 2
    PROD_CODE_NUMBER_COL = 3
    PROD_CODE_ASCII_NUM_COL = 4
    PROD_ASCII_DESC_COL = 5

    PROD_DEV_IO_SEL = 27
    PROD_DEV_COM_SEL = 28

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
    Set DEV_FILE_C = fs.CreateTextFile(file_path, True)
    file_path = ActiveWorkbook.Path & "\" & Cells(4, 2).value
    Set DEV_FILE_H = fs.CreateTextFile(file_path, True)

'****************************************************************************************************************************
'******     Start Creating the .H file header
'****************************************************************************************************************************
    DEV_FILE_H.WriteLine ("/*")
    DEV_FILE_H.WriteLine ("*****************************************************************************************")
    DEV_FILE_H.WriteLine ("*")
    DEV_FILE_H.WriteLine ("*       Copyright © Eaton Corporation. All Rights Reserved.")
    DEV_FILE_H.WriteLine ("*       Creator: Microsoft Excel - Macro Generated")
    DEV_FILE_H.WriteLine ("*")
    DEV_FILE_H.WriteLine ("*       Description:")
    DEV_FILE_H.WriteLine ("*")
    DEV_FILE_H.WriteLine ("*")
    DEV_FILE_H.WriteLine ("*")
    DEV_FILE_H.WriteLine ("*****************************************************************************************")
    DEV_FILE_H.WriteLine ("*/")
    DEV_FILE_H.WriteLine ("#ifndef " & upper_case_file_name & "_H")
    DEV_FILE_H.WriteLine ("  #define " & upper_case_file_name & "_H")
    DEV_FILE_H.WriteLine ("")

    DEV_FILE_H.WriteLine ("#include ""Device_Identity.h""")
    DEV_FILE_H.WriteLine ("")
    DEV_FILE_H.WriteLine ("")
    DEV_FILE_H.WriteLine ("//*******************************************************")
    DEV_FILE_H.WriteLine ("//******     Create the Prod Def Structure.")
    DEV_FILE_H.WriteLine ("//*******************************************************")
    DEV_FILE_H.WriteLine ("")
    DEV_FILE_H.WriteLine ("extern const DEV_ID_DEF_PROD_DESC_SEL_ST " & lower_case_file_name & "_prod_def;")
    DEV_FILE_H.WriteLine ("")
    DEV_FILE_H.WriteLine ("")
    DEV_FILE_H.WriteLine ("#endif")



'****************************************************************************************************************************
'******     Start Creating the .C file header
'****************************************************************************************************************************
    DEV_FILE_C.WriteLine ("/*")
    DEV_FILE_C.WriteLine ("*****************************************************************************************")
    DEV_FILE_C.WriteLine ("*       Auto Generated EIP DCI interface file.")
    DEV_FILE_C.WriteLine ("*")
    DEV_FILE_C.WriteLine ("*")
    DEV_FILE_C.WriteLine ("*       Copyright© 2011, Eaton Corporation. All Rights Reserved.")
    DEV_FILE_C.WriteLine ("*")
    DEV_FILE_C.WriteLine ("*****************************************************************************************")
    DEV_FILE_C.WriteLine ("*/")
    DEV_FILE_C.WriteLine ("#include ""Includes.h""")
    DEV_FILE_C.WriteLine ("#include """ & file_name & ".h""")
    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("")


'****************************************************************************************************************************
'******     Generate the Attribute Lists first.
'****************************************************************************************************************************
    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("//*******************************************************")
    DEV_FILE_C.WriteLine ("//******     Define Prod Code and Desc Structures")
    DEV_FILE_C.WriteLine ("//*******************************************************")
    DEV_FILE_C.WriteLine ("")
    row_counter = START_ROW
    total_values = 0
    While (Cells(row_counter, PROD_CODE_DEFINE_COL).text <> Empty)
        DEV_FILE_C.WriteLine ("")
        DEV_FILE_C.WriteLine ("//" & Cells(row_counter, PROD_DESCRIP_COL).text)
        DEV_FILE_C.WriteLine ("const uint8_t " & Cells(row_counter, PROD_CODE_DEFINE_COL).text & "_PROD_CODE_ASCII[] = ")
        DEV_FILE_C.WriteLine ("        { " & Cells(row_counter, PROD_CODE_ASCII_NUM_COL).text & " };")
        DEV_FILE_C.WriteLine ("const uint8_t " & Cells(row_counter, PROD_CODE_DEFINE_COL).text & "_PROD_DESC[] = ")
        DEV_FILE_C.WriteLine ("        { """ & Cells(row_counter, PROD_ASCII_DESC_COL).text & """ };")
        DEV_FILE_C.WriteLine ("const DEV_ID_DEF_PROD_DESC_ST " & Cells(row_counter, PROD_CODE_DEFINE_COL).text & "_PROD_CODE_DESC =")
        DEV_FILE_C.WriteLine ("{")
        DEV_FILE_C.WriteLine ("    " & Cells(row_counter, PROD_CODE_NUMBER_COL).text & ",")
        DEV_FILE_C.WriteLine ("    " & Cells(row_counter, PROD_CODE_DEFINE_COL).text & "_PROD_CODE_ASCII,")
        DEV_FILE_C.WriteLine ("    " & Cells(row_counter, PROD_CODE_DEFINE_COL).text & "_PROD_DESC")
        DEV_FILE_C.WriteLine ("};")
        row_counter = row_counter + 1
        total_values = total_values + 1
    Wend

    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("//*******************************************************")
    DEV_FILE_C.WriteLine ("//******     Define Prod Select List")
    DEV_FILE_C.WriteLine ("//*******************************************************")
    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("#define " & upper_case_file_name & "_TOTAL_SELECTIONS        " & total_values)
    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("const DEV_ID_DEF_SEL_ST " & lower_case_file_name & "_prod_sel_list[" & upper_case_file_name & "_TOTAL_SELECTIONS] =")
    DEV_FILE_C.WriteLine ("{")
    
    row_counter = START_ROW
    While (Cells(row_counter, PROD_CODE_DEFINE_COL).text <> Empty)

        DEV_FILE_C.WriteLine ("    {   //" & Cells(row_counter, PROD_DESCRIP_COL).text)
        DEV_FILE_C.WriteLine ("        " & Cells(row_counter, PROD_DEV_IO_SEL).text & ",")
        DEV_FILE_C.WriteLine ("        " & Cells(row_counter, PROD_DEV_COM_SEL).text & ",")
        DEV_FILE_C.WriteLine ("        &" & Cells(row_counter, PROD_CODE_DEFINE_COL).text & "_PROD_CODE_DESC")
        DEV_FILE_C.WriteLine ("    },")
        row_counter = row_counter + 1
    Wend
    DEV_FILE_C.WriteLine ("};")

    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("//*******************************************************")
    DEV_FILE_C.WriteLine ("//******     Define List Definition")
    DEV_FILE_C.WriteLine ("//*******************************************************")
    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("const DEV_ID_DEF_PROD_DESC_SEL_ST " & lower_case_file_name & "_prod_def =")
    DEV_FILE_C.WriteLine ("{")
    DEV_FILE_C.WriteLine ("    " & upper_case_file_name & "_TOTAL_SELECTIONS,")
    DEV_FILE_C.WriteLine ("    " & lower_case_file_name & "_prod_sel_list")
    DEV_FILE_C.WriteLine ("};")
        
    DEV_FILE_C.Close
    DEV_FILE_H.Close

    Application.Calculation = xlCalculationAutomatic

End Sub

Sub Dead_Code()


'****************************************************************************************************************************
'******     Create the Class, Instances, and Attributes Count.  We spin through the list once to set the class information.
'****************************************************************************************************************************

    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("//*******************************************************")
    DEV_FILE_C.WriteLine ("//******     Class, Instance, Attribute Item Counts")
    DEV_FILE_C.WriteLine ("//*******************************************************")
    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("///Total number of classes.")
    DEV_FILE_C.WriteLine ("#define " & upper_case_file_name & "_TOTAL_CLASSES" & _
                        Space(NUM_ALIGN_POS - Len("#define " & upper_case_file_name & "_TOTAL_CLASSES") + 1) & class_counter)
    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("///Total number of attributes and instances per class.")
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
            temp_string = "#define " & upper_case_file_name & "_CLASS_0x" & _
                                Hexify(present_class_num, class_byte_size) & "_NUM_INSTANCES"
            DEV_FILE_C.WriteLine (temp_string & Space(NUM_ALIGN_POS - Len(temp_string) + 1) & instance_counter)
            temp_string = "#define " & upper_case_file_name & "_CLASS_0x" & _
                                Hexify(present_class_num, class_byte_size) & "_NUM_ATTRIBUTES"
            DEV_FILE_C.WriteLine (temp_string & Space(NUM_ALIGN_POS - Len(temp_string) + 1) & attribute_counter)
            instance_counter = 0
            present_instance_num = 0
            attribute_counter = 0
        End If
    Wend
    DEV_FILE_C.WriteLine ("")
            

'****************************************************************************************************************************
'******     Generate the Attribute Lists first.
'****************************************************************************************************************************
    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("//*******************************************************")
    DEV_FILE_C.WriteLine ("//******     Define Attribute Structure Lists")
    DEV_FILE_C.WriteLine ("//*******************************************************")
    DEV_FILE_C.WriteLine ("")
    present_instance_num = 0
    present_class_num = 0
    row_counter = START_ROW
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (present_class_num <> Cells(row_counter, DATA_CLASS_COL).text) Or (present_instance_num <> Cells(row_counter, DATA_INSTANCE_COL).text) Then
            If (present_class_num <> 0) Then
                DEV_FILE_C.WriteLine ("};")
                DEV_FILE_C.WriteLine ("")
            End If
            present_class_num = Cells(row_counter, DATA_CLASS_COL).text
            present_instance_num = Cells(row_counter, DATA_INSTANCE_COL).text
            DEV_FILE_C.WriteLine ("const CIP_ATTRIB_ST_TD " & _
                    lower_case_file_name & "_C0x" & Hexify(present_class_num, class_byte_size) & _
                    "_I0x" & Hexify(present_instance_num, instance_byte_size) & _
                    "_attrib_list[" & upper_case_file_name & "_CLASS_0x" & Hexify(present_class_num, class_byte_size) & _
                    "_NUM_ATTRIBUTES] = ")
            DEV_FILE_C.WriteLine ("{")
        End If
        cia_str = "0x" & Hexify(Cells(row_counter, DATA_ATTRIBUTE_COL).text, attribute_byte_size)
        
        '********************************************************************
        ' This defines the Beginning of the structure and the DCID.
        DEV_FILE_C.WriteLine ("    {        // C: 0x" & Hexify(present_class_num, class_byte_size) & _
                        ", I: 0x" & Hexify(present_instance_num, instance_byte_size) & _
                        ", A: 0x" & Hexify(Cells(row_counter, DATA_ATTRIBUTE_COL).text, attribute_byte_size) & _
                        " - " & Cells(row_counter, DESCRIP_COL).text & " Attrib" & Space(4) & cia_str)
        DEV_FILE_C.WriteLine ("        " & cia_str & ",")
        DEV_FILE_C.WriteLine ("        " & Cells(row_counter, DATA_DEFINE_COL).text & "_DCID,")
        
        '********************************************************************
        ' This defines the Index section (whether it is bitwise or bytewise.
        If (Cells(row_counter, DATA_INDEX_COL).text <> Empty) Then
            DEV_FILE_C.WriteLine ("        " & Cells(row_counter, DATA_INDEX_COL).text & ",")
        Else
            DEV_FILE_C.WriteLine ("        0,")
        End If
        
        '********************************************************************
        ' This defines the alternate length which may be required.
        If (Cells(row_counter, DATA_ALTERNATE_LEN_COL).text <> Empty) Then
            DEV_FILE_C.WriteLine ("        " & Cells(row_counter, DATA_ALTERNATE_LEN_COL).text & ",")
        Else
            DEV_FILE_C.WriteLine ("        0,")
        End If

        '********************************************************************
        ' This defines the bitwise structure associated with special behaviors.
        DEV_FILE_C.WriteLine ("        ( ")
        special_str = ""
        If (Cells(row_counter, DATA_BITWISE_BOOL_COL).text <> Empty) Then
            If Len(special_str) > 0 Then
                DEV_FILE_C.WriteLine ("            " & special_str & " |")
            End If
            special_str = "( 1<<EIP_SPECIAL_BITWISE_TO_BOOL )"
        End If
        
        If (Cells(row_counter, DATA_BOOLIZE_COL).text <> Empty) Then
            If Len(special_str) > 0 Then
                DEV_FILE_C.WriteLine ("            " & special_str & " |")
            End If
            special_str = "( 1<<EIP_SPECIAL_CONVERT_TO_BOOL )"
        End If

        If Len(special_str) > 0 Then
            DEV_FILE_C.WriteLine ("            " & special_str)
        Else
            DEV_FILE_C.WriteLine ("            0")
        End If
        DEV_FILE_C.WriteLine ("        )")

        ' End the bitwise stuff and wrap up the single structure definition.
        '**********************************************************************
        
        If (Cells(row_counter + 1, DATA_DEFINE_COL).text = Empty) Or (Cells(row_counter + 1, DATA_DEFINE_COL).text = BEGIN_IGNORED_DATA) Then
            DEV_FILE_C.WriteLine ("    }")
        Else
            DEV_FILE_C.WriteLine ("    },")
        End If
        row_counter = row_counter + 1
    Wend
    DEV_FILE_C.WriteLine ("};")
    DEV_FILE_C.WriteLine ("")
        
    
    
'****************************************************************************************************************************
'******     Generate the Instance Lists.
'****************************************************************************************************************************
 
    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("//*******************************************************")
    DEV_FILE_C.WriteLine ("//******     Define Instance Structure Lists")
    DEV_FILE_C.WriteLine ("//*******************************************************")
    DEV_FILE_C.WriteLine ("")
    present_class_num = 0
    present_instance_num = 0
    row_counter = START_ROW
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (present_class_num <> Cells(row_counter, DATA_CLASS_COL).text) Then
            If (present_class_num <> 0) Then
                DEV_FILE_C.WriteLine ("};")
                DEV_FILE_C.WriteLine ("")
            End If
            present_class_num = Cells(row_counter, DATA_CLASS_COL).text
            present_instance_num = Cells(row_counter, DATA_INSTANCE_COL).text
            DEV_FILE_C.WriteLine ("const CIP_INSTANCE_ST_TD " & _
                    lower_case_file_name & "_C0x" & Hexify(present_class_num, class_byte_size) & _
                    "_instance_list[" & upper_case_file_name & "_CLASS_0x" & Hexify(present_class_num, class_byte_size) & _
                    "_NUM_INSTANCES] = ")
            DEV_FILE_C.WriteLine ("{")
            DEV_FILE_C.WriteLine ("    {")
            DEV_FILE_C.WriteLine ("        " & lower_case_file_name & "_C0x" & Hexify(present_class_num, class_byte_size) & _
                                "_I0x" & Hexify(present_instance_num, instance_byte_size) & _
                                "_attrib_list")
            DEV_FILE_C.WriteLine ("    },")
        End If
        If (present_instance_num <> Cells(row_counter, DATA_INSTANCE_COL).text) Then
            present_instance_num = Cells(row_counter, DATA_INSTANCE_COL).text
            DEV_FILE_C.WriteLine ("    {")
            DEV_FILE_C.WriteLine ("        " & lower_case_file_name & "_C0x" & Hexify(present_class_num, class_byte_size) & _
                                "_I0x" & Hexify(present_instance_num, instance_byte_size) & _
                                "_attrib_list")
            DEV_FILE_C.WriteLine ("    },")
        End If
        row_counter = row_counter + 1
    Wend
    DEV_FILE_C.WriteLine ("};")
    DEV_FILE_C.WriteLine ("")
 
    


'****************************************************************************************************************************
'******     Create the class instance 0 attribute list.  We spin through the list once to set the class information.
'****************************************************************************************************************************

    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("//*******************************************************")
    DEV_FILE_C.WriteLine ("//******      Define the Class Structure List")
    DEV_FILE_C.WriteLine ("//*******************************************************")
    DEV_FILE_C.WriteLine ("")
    class_counter = 0
    present_class_num = 0
    row_counter = START_ROW
    DEV_FILE_C.WriteLine ("const CIP_CLASS_ST_TD " & _
                lower_case_file_name & "_cip_class_list[" & upper_case_file_name & "_TOTAL_CLASSES] = ")
    DEV_FILE_C.WriteLine ("{")
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

            DEV_FILE_C.WriteLine ("    {        // C: " & present_class_num & " Index: " & class_counter)
            DEV_FILE_C.WriteLine ("        0x" & Hexify(present_class_num, class_byte_size) & ",")
            DEV_FILE_C.WriteLine ("        " & lower_case_file_name & "_C0x" & _
                    Hexify(present_class_num, class_byte_size) & "_instance_list,")
            DEV_FILE_C.WriteLine ("        " & instance_0_rev & ",")
'            DEV_FILE_C.WriteLine ("        " & max_instance_num & ",   ///Max number of instances.")
            DEV_FILE_C.WriteLine ("        " & upper_case_file_name & "_CLASS_0x" & _
                                Hexify(present_class_num, class_byte_size) & "_NUM_INSTANCES,")
            DEV_FILE_C.WriteLine ("        " & upper_case_file_name & "_CLASS_0x" & _
                                Hexify(present_class_num, class_byte_size) & "_NUM_ATTRIBUTES")
            DEV_FILE_C.WriteLine ("    },")
            class_counter = class_counter + 1
        Else
            row_counter = row_counter + 1
        End If
    Wend
    DEV_FILE_C.WriteLine ("};")
    DEV_FILE_C.WriteLine ("")


'****************************************************************************************************************************
'******     Generate the Message Router Object attribute 1 data.
'****************************************************************************************************************************
    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("//*******************************************************")
    DEV_FILE_C.WriteLine ("//******     Message Router Object Attribute 1 Definition")
    DEV_FILE_C.WriteLine ("//*******************************************************")
    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("const uint16_t " & _
                lower_case_file_name & "_msg_rtr_attrib1[" & upper_case_file_name & "_TOTAL_CLASSES + 1] = ")
    DEV_FILE_C.WriteLine ("{")
    DEV_FILE_C.WriteLine ("    " & upper_case_file_name & "_TOTAL_CLASSES,")
    present_class_num = 0
    row_counter = START_ROW
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (present_class_num <> Cells(row_counter, DATA_CLASS_COL).text) Then
            present_class_num = Cells(row_counter, DATA_CLASS_COL).value
            DEV_FILE_C.WriteLine ("    0x" & Hexify(present_class_num, 2) & ",")
        End If
        row_counter = row_counter + 1
    Wend
    DEV_FILE_C.WriteLine ("};")


    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("//*******************************************************")
    DEV_FILE_C.WriteLine ("//******     The Target Definition Structure.")
    DEV_FILE_C.WriteLine ("//*******************************************************")
    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("const EIP_TARGET_INFO_ST_TD " & lower_case_file_name & "_target_info =")
    DEV_FILE_C.WriteLine ("{")
    DEV_FILE_C.WriteLine ("    " & upper_case_file_name & "_TOTAL_CLASSES,      //uint16_t total_items;")
    DEV_FILE_C.WriteLine ("    " & lower_case_file_name & "_cip_class_list,      //const DCI_CIP_TO_DCID_LKUP_ST_TD* cip_to_dcid")
    DEV_FILE_C.WriteLine ("")
'    DEV_FILE_C.WriteLine ("    " & upper_case_file_name & "_TOTAL_CLASSES,        //uint16_t total_class_inst0_defs")
'    DEV_FILE_C.WriteLine ("    &" & lower_case_file_name & "_cip_class_inst0,       //const CIP_INSTANCE_0_EN_TD* class_inst0_def")
    DEV_FILE_C.WriteLine ("    " & lower_case_file_name & "_msg_rtr_attrib1,       //uint16_t const* msg_rtr_attrib1")
'    DEV_FILE_C.WriteLine ("    " & upper_case_file_name & "_TOTAL_DCIDS,        //DCI_ID_TD total_dcids")
'    DEV_FILE_C.WriteLine ("    " & lower_case_file_name & "_dcid_to_mbus        //const DCI_MBUS_FROM_DCID_LKUP_ST_TD* dcid_to_mbus")
    DEV_FILE_C.WriteLine ("};")
    DEV_FILE_C.WriteLine ("")
    DEV_FILE_C.WriteLine ("")
End Sub
