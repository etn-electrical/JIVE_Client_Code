Attribute VB_Name = "BACnet_Builder"
'**************************
'**************************
'BACnet Construction sheet.
'**************************
'**************************


Sub Create_BACnet_DCI_Xfer()

'*********************
'******     Constants
'*********************
    
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    
    START_ROW = 9
    
    DESCRIP_COL = 1
    DATA_DEFINE_COL = 2
    DATA_LENGTH_COL = 3
    DATA_BACNET_REG_COL = 4

'*********************
'******     Begin
'*********************
    Sheets("BACnet").Select
    Set fs = CreateObject("Scripting.FileSystemObject")
    Set BACNET_DCI_C = fs.CreateTextFile(Cells(3, 2).value, True)
    Set BACNET_DCI_H = fs.CreateTextFile(Cells(4, 2).value, True)
    
    'Making font type and size consistent.
    Worksheets("BACnet").Range("A:S").Font.name = "Arial"
    Worksheets("BACnet").Range("A:S").Font.Size = 10


'****************************************************************************************************************************
'******     Start Creating the .H file header
'****************************************************************************************************************************
    BACNET_DCI_H.WriteLine ("/*")
    BACNET_DCI_H.WriteLine ("*****************************************************************************************")
    BACNET_DCI_H.WriteLine ("*       $Workfile:$")
    BACNET_DCI_H.WriteLine ("*")
    BACNET_DCI_H.WriteLine ("*       $Author:$")
    BACNET_DCI_H.WriteLine ("*       $Date:$")
    BACNET_DCI_H.WriteLine ("*       Copyright © Eaton Corporation. All Rights Reserved.")
    BACNET_DCI_H.WriteLine ("*       Creator: Mark A Verheyen")
    BACNET_DCI_H.WriteLine ("*")
    BACNET_DCI_H.WriteLine ("*       Description:")
    BACNET_DCI_H.WriteLine ("*")
    BACNET_DCI_H.WriteLine ("*")
    BACNET_DCI_H.WriteLine ("*       Code Inspection Date: ")
    BACNET_DCI_H.WriteLine ("*")
    BACNET_DCI_H.WriteLine ("*       $Header:$")
    BACNET_DCI_H.WriteLine ("*****************************************************************************************")
    BACNET_DCI_H.WriteLine ("*/")
    BACNET_DCI_H.WriteLine ("#ifndef BACNET_DCI_XREF_H")
    BACNET_DCI_H.WriteLine ("  #define BACNET_DCI_XREF_H")
    BACNET_DCI_H.WriteLine ("")
    BACNET_DCI_H.WriteLine ("#include ""DCI.h""")
    BACNET_DCI_H.WriteLine ("")
    
'****************************************************************************************************************************
'******     Sort the BACNET register list
'****************************************************************************************************************************
    Rows("9:250").Select
    Selection.Sort Key1:=Range("D9"), Order1:=xlAscending, _
        Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
        Orientation:=xlTopToBottom
    Range("A9").Select

'****************************************************************************************************************************
'******     Gotta calculate the total number of BACNET registers we are going to need given a certain length.
'****************************************************************************************************************************
    num_registers = 0
    total_modbus_registers = 0
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If (Cells(counter, DATA_LENGTH_COL).value Mod 2) = 1 Then
            num_registers = num_registers + Cells(counter, DATA_LENGTH_COL).value + 1
        Else
            num_registers = num_registers + Cells(counter, DATA_LENGTH_COL).value
        End If
        counter = counter + 1
    Wend
    num_registers = num_registers / 2
    
    BACNET_DCI_H.WriteLine ("//*******************************************************")
    BACNET_DCI_H.WriteLine ("//******     Modbus structure and size follow.")
    BACNET_DCI_H.WriteLine ("//*******************************************************")
    BACNET_DCI_H.WriteLine ("")
    BACNET_DCI_H.WriteLine ("typedef UINT16         BACNET_REG_TD;")
    BACNET_DCI_H.WriteLine ("typedef struct")
    BACNET_DCI_H.WriteLine ("{")
    BACNET_DCI_H.WriteLine ("    BACNET_REG_TD modbus_reg;")
    BACNET_DCI_H.WriteLine ("    DCI_LENGTH_TD offset;")
    BACNET_DCI_H.WriteLine ("    DCI_ID_TD dci_id;")
    BACNET_DCI_H.WriteLine ("}BACNET_DCI_XREF_ST_TD;")
    BACNET_DCI_H.WriteLine ("")
    BACNET_DCI_H.WriteLine ("#define BACNET_DCI_XREF_TOTAL_REGISTERS        " & num_registers)
    BACNET_DCI_H.WriteLine ("extern const BACNET_DCI_XREF_ST_TD modbus_xref[BACNET_DCI_XREF_TOTAL_REGISTERS];")
    BACNET_DCI_H.WriteLine ("")
    BACNET_DCI_H.WriteLine ("")
    
    BACNET_DCI_H.WriteLine ("//*******************************************************")
    BACNET_DCI_H.WriteLine ("//******     Modbus Register List")
    BACNET_DCI_H.WriteLine ("//*******************************************************")
    BACNET_DCI_H.WriteLine ("")
    row_counter = START_ROW
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        BACNET_DCI_H.WriteLine ("#define " & Cells(row_counter, DATA_DEFINE_COL).text & "_BACNET_REG            " & _
                                Cells(row_counter, DATA_BACNET_REG_COL).text)
        row_counter = row_counter + 1
    Wend
    BACNET_DCI_H.WriteLine ("")
    BACNET_DCI_H.WriteLine ("")
    BACNET_DCI_H.WriteLine ("#endif")
    




'****************************************************************************************************************************
'******     Start Creating the .C file header
'****************************************************************************************************************************
    BACNET_DCI_C.WriteLine ("/*")
    BACNET_DCI_C.WriteLine ("*****************************************************************************************")
    BACNET_DCI_C.WriteLine ("*       $Workfile:")
    BACNET_DCI_C.WriteLine ("*")
    BACNET_DCI_C.WriteLine ("*       $Author:$")
    BACNET_DCI_C.WriteLine ("*       $Date:$")
    BACNET_DCI_C.WriteLine ("*       Copyright© 2001-2004, Eaton Corporation. All Rights Reserved.")
    BACNET_DCI_C.WriteLine ("*")
    BACNET_DCI_C.WriteLine ("*       $Header:$")
    BACNET_DCI_C.WriteLine ("*****************************************************************************************")
    BACNET_DCI_C.WriteLine ("*/")
    BACNET_DCI_C.WriteLine ("#include ""Includes.h""")
    BACNET_DCI_C.WriteLine ("#include """ & Cells(2, 2).value & ".h""")
    BACNET_DCI_C.WriteLine ("")

'****************************************************************************************************************************
'******     Create the Ram pointer blocks.
'****************************************************************************************************************************
    
    BACNET_DCI_C.WriteLine ("")
    BACNET_DCI_C.WriteLine ("")
    BACNET_DCI_C.WriteLine ("//*******************************************************")
    BACNET_DCI_C.WriteLine ("//******     The BACnet Register Cross Reference Table.")
    BACNET_DCI_C.WriteLine ("//*******************************************************")
    BACNET_DCI_C.WriteLine ("")
    
    BACNET_DCI_C.WriteLine ("const BACNET_DCI_XREF_ST_TD modbus_xref[BACNET_DCI_XREF_TOTAL_REGISTERS] = ")
    BACNET_DCI_C.WriteLine ("{")
    row_counter = START_ROW
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        reg_dup_counter = 0
        
        While (reg_dup_counter * 2) < Cells(row_counter, DATA_LENGTH_COL).value    'Run until the reg count compensates for the length
            modbus_register_counter = Cells(row_counter, DATA_BACNET_REG_COL).value
            modbus_register_counter = modbus_register_counter + reg_dup_counter
            BACNET_DCI_C.WriteLine ("    {        // " & Cells(row_counter, DESCRIP_COL).text & " + " & reg_dup_counter)
'            BACNET_DCI_C.WriteLine ("        " & Cells(row_counter, DATA_DEFINE_COL).Text & "_BACNET_REG + " & reg_dup_counter * 2 & ",")
            BACNET_DCI_C.WriteLine ("        " & modbus_register_counter & ",")
            BACNET_DCI_C.WriteLine ("        " & reg_dup_counter * 2 & ",")
            BACNET_DCI_C.WriteLine ("        " & Cells(row_counter, DATA_DEFINE_COL).text & "_DCID")
            reg_dup_counter = reg_dup_counter + 1
            If (Cells(row_counter + 1, DATA_DEFINE_COL).text = Empty) And ((reg_dup_counter * 2) >= Cells(row_counter, DATA_LENGTH_COL).value) Then
                BACNET_DCI_C.WriteLine ("    } ")
            Else
                BACNET_DCI_C.WriteLine ("    }, ")
            End If
        Wend
        
        row_counter = row_counter + 1
    Wend
    BACNET_DCI_C.WriteLine ("};")

    BACNET_DCI_C.Close
    BACNET_DCI_H.Close

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

Public Sub Verify_BACnet_List()

'*********************
'******     Constants
'*********************
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    
    Dim BEGIN_NEW_BACNET_OBJECT As String
    BEGIN_NEW_BACNET_OBJECT = "BEGIN_NEW_BACNET_OBJECT"
    
    START_ROW = 9
    DCI_DEFINES_START_ROW = 9
    DCI_DESCRIPTORS_COL = 1
    DCI_DEFINES_COL = 2
    DCI_DATATYPE_COL = 3
    DCI_LENGTH_COL = 6

    DESCRIP_COL = 1
    DATA_DEFINE_COL = 2
    DATA_DATATYPE_COL = 3
    DATA_LENGTH_COL = 4
    DATA_DESC_MISSING_COL = DESCRIP_COL
    DATA_DEFINE_MISSING_COL = DATA_DEFINE_COL
    DATA_LENGTH_MISSING_COL = DATA_LENGTH_COL
    DATA_END_ROW = 300
    BEGIN_NEW_BACNET_OBJECT_ROW_SKIP = 6

'*********************
'******     Begin
'*********************
    Sheets("BACnet").Select

    
    
'****************************************************************************************************************************
'******     Find where we should add the unadded items.
'****************************************************************************************************************************
    missing_row_counter = START_ROW
    While (Cells(missing_row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA) And _
                                        (missing_row_counter < DATA_END_ROW)
        missing_row_counter = missing_row_counter + 1
    Wend
    While (Cells(missing_row_counter, DATA_DEFINE_COL).text <> Empty)
        missing_row_counter = missing_row_counter + 1
    Wend

'****************************************************************************************************************************
'******     Find the missing modbus data items
'****************************************************************************************************************************
    dci_defines_row_ctr = DCI_DEFINES_START_ROW
    While Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> Empty
        lookup_string = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text
        Set C = Worksheets("BACnet").Range("B9:AZ300").Find(lookup_string)
        If C Is Nothing Then
            Cells(missing_row_counter, DATA_DESC_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DESCRIPTORS_COL).text
            Cells(missing_row_counter, DATA_DEFINE_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text
            Cells(missing_row_counter, DATA_DATATYPE_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DATATYPE_COL).text
            ' The following line is for creating a link between the length and the data id.
            'Cells(missing_counter, DATA_LENGTH_MISSING_COL).Value = "='DCI Descriptors'!" & "$F$" & dci_defines_row_ctr
            ' The next line will just copy the size and not correct it.
            Cells(missing_row_counter, DATA_LENGTH_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_LENGTH_COL).value
            missing_row_counter = missing_row_counter + 1
        End If
        dci_defines_row_ctr = dci_defines_row_ctr + 1
    Wend


    Sheets("BACnet").Select
'****************************************************************************************************************************
'******     Find the Data items that have been removed from the main list
'****************************************************************************************************************************
    row_ctr = START_ROW
    missing_counter = START_ROW
    While row_ctr < DATA_END_ROW  'Cells(row_ctr, DATA_DEFINE_COL).Text <> Empty
        If Cells(row_ctr, DATA_DEFINE_COL).text = BEGIN_NEW_BACNET_OBJECT Then
            row_ctr = row_ctr + BEGIN_NEW_BACNET_OBJECT_ROW_SKIP
        Else
            If Cells(row_ctr, DATA_DEFINE_COL).text <> Empty Then
                lookup_string = Cells(row_ctr, DATA_DEFINE_COL).text
                If lookup_string <> BEGIN_IGNORED_DATA Then
                    Set C = Worksheets("DCI Descriptors").Range("B9:B500").Find(lookup_string)
                    If C Is Nothing Then
                        Cells(row_ctr, DATA_DEFINE_COL).Interior.Color = RGB(255, 0, 0)
                        Cells(row_ctr, DESCRIP_COL).Interior.Color = RGB(255, 0, 0)
                    End If
                End If
            End If
            row_ctr = row_ctr + 1
        End If
    Wend


'****************************************************************************************************************************
'******     This will go through the main list and make the lengths correct in the slave sheets
'****************************************************************************************************************************
    dci_defines_row_ctr = DCI_DEFINES_START_ROW
    While Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> Empty
        local_row_ctr = START_ROW
        While (Cells(local_row_ctr, DATA_DEFINE_MISSING_COL).value <> Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text) And _
                        (local_row_ctr < DATA_END_ROW) 'Cells(local_row_ctr, DATA_DEFINE_MISSING_COL).Value <> Empty)
            local_row_ctr = local_row_ctr + 1
        Wend
        If (Cells(local_row_ctr, DATA_DEFINE_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text) Then
            Cells(local_row_ctr, DATA_LENGTH_MISSING_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(dci_defines_row_ctr, DCI_LENGTH_COL).text
        End If
        dci_defines_row_ctr = dci_defines_row_ctr + 1
    Wend


End Sub



