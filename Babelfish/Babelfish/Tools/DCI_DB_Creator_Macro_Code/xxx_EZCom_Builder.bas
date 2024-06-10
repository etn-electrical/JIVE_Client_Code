Attribute VB_Name = "xxx_EZCom_Builder"
'**************************
'**************************
'EZBus Construction sheet.
'**************************
'**************************

Sub Create_EZCom_DCI_Xfer()

'*********************
'******     Constants
'*********************
    
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    
    START_ROW = 9
    
    DESCRIP_COL = 1
    DATA_DEFINE_COL = 2
    DATA_LENGTH_COL = 3
    DATA_REG_COL = 4

'*********************
'******     Begin
'*********************
    Sheets("EZCom").Select
    Set fs = CreateObject("Scripting.FileSystemObject")
    Set FILE_DCI_C = fs.CreateTextFile(Cells(3, 2).value, True)
    Set FILE_DCI_H = fs.CreateTextFile(Cells(4, 2).value, True)


'****************************************************************************************************************************
'******     Start Creating the .H file header
'****************************************************************************************************************************
    FILE_DCI_H.WriteLine ("/*")
    FILE_DCI_H.WriteLine ("*****************************************************************************************")
    FILE_DCI_H.WriteLine ("*       $Workfile:$")
    FILE_DCI_H.WriteLine ("*")
    FILE_DCI_H.WriteLine ("*       $Author:$")
    FILE_DCI_H.WriteLine ("*       $Date:$")
    FILE_DCI_H.WriteLine ("*       Copyright © Eaton Corporation. All Rights Reserved.")
    FILE_DCI_H.WriteLine ("*       Creator: Mark A Verheyen")
    FILE_DCI_H.WriteLine ("*")
    FILE_DCI_H.WriteLine ("*       Description:")
    FILE_DCI_H.WriteLine ("*")
    FILE_DCI_H.WriteLine ("*")
    FILE_DCI_H.WriteLine ("*       Code Inspection Date: ")
    FILE_DCI_H.WriteLine ("*")
    FILE_DCI_H.WriteLine ("*       $Header:$")
    FILE_DCI_H.WriteLine ("*****************************************************************************************")
    FILE_DCI_H.WriteLine ("*/")
    FILE_DCI_H.WriteLine ("#ifndef EZCOM_DCI_XREF_H")
    FILE_DCI_H.WriteLine ("  #define EZCOM_DCI_XREF_H")
    FILE_DCI_H.WriteLine ("")
    FILE_DCI_H.WriteLine ("#include ""DCI.h""")
    FILE_DCI_H.WriteLine ("")
    
'****************************************************************************************************************************
'******     Sort the Register list
'****************************************************************************************************************************
    Rows("9:250").Select
    Selection.Sort Key1:=Range("D9"), Order1:=xlAscending, _
        Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
        Orientation:=xlTopToBottom
    Range("A9").Select

'****************************************************************************************************************************
'******     Gotta calculate the total number of registers we are going to need given a certain length.
'****************************************************************************************************************************
    num_registers = 0
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        num_registers = num_registers + 1
        counter = counter + 1
    Wend
    
    FILE_DCI_H.WriteLine ("//*******************************************************")
    FILE_DCI_H.WriteLine ("//******     Structure and size follow.")
    FILE_DCI_H.WriteLine ("//*******************************************************")
    FILE_DCI_H.WriteLine ("")
    FILE_DCI_H.WriteLine ("typedef uint16_t         EZCOM_REG_TD;")
    FILE_DCI_H.WriteLine ("typedef struct")
    FILE_DCI_H.WriteLine ("{")
    FILE_DCI_H.WriteLine ("    EZCOM_REG_TD ezcom_reg;")
    FILE_DCI_H.WriteLine ("    DCI_ID_TD dci_id;")
    FILE_DCI_H.WriteLine ("}EZCOM_DCI_XREF_ST_TD;")
    FILE_DCI_H.WriteLine ("")
    FILE_DCI_H.WriteLine ("#define EZCOM_DCI_XREF_TOTAL_REGISTERS        " & num_registers)
    FILE_DCI_H.WriteLine ("extern const EZCOM_DCI_XREF_ST_TD ezcom_xref[EZCOM_DCI_XREF_TOTAL_REGISTERS];")
    FILE_DCI_H.WriteLine ("")
    FILE_DCI_H.WriteLine ("")
    
    FILE_DCI_H.WriteLine ("//*******************************************************")
    FILE_DCI_H.WriteLine ("//******     Register List")
    FILE_DCI_H.WriteLine ("//*******************************************************")
    FILE_DCI_H.WriteLine ("")
    row_counter = START_ROW
    While (Cells(row_counter, DATA_DEFINE_COL).text <> Empty) And (Cells(row_counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        FILE_DCI_H.WriteLine ("#define " & Cells(row_counter, DATA_DEFINE_COL).text & "_EZCOM_REG            " & _
                                Cells(row_counter, DATA_REG_COL).text)
        row_counter = row_counter + 1
    Wend
    FILE_DCI_H.WriteLine ("")
    FILE_DCI_H.WriteLine ("")
    FILE_DCI_H.WriteLine ("#endif")


'****************************************************************************************************************************
'******     Start Creating the .C file header
'****************************************************************************************************************************
    FILE_DCI_C.WriteLine ("/*")
    FILE_DCI_C.WriteLine ("*****************************************************************************************")
    FILE_DCI_C.WriteLine ("*       $Workfile:")
    FILE_DCI_C.WriteLine ("*")
    FILE_DCI_C.WriteLine ("*       $Author:$")
    FILE_DCI_C.WriteLine ("*       $Date:$")
    FILE_DCI_C.WriteLine ("*       Copyright© 2001-2004, Eaton Corporation. All Rights Reserved.")
    FILE_DCI_C.WriteLine ("*")
    FILE_DCI_C.WriteLine ("*       $Header:$")
    FILE_DCI_C.WriteLine ("*****************************************************************************************")
    FILE_DCI_C.WriteLine ("*/")
    FILE_DCI_C.WriteLine ("#include ""Includes.h""")
    FILE_DCI_C.WriteLine ("#include ""EZCom_DCI_XRef.h""")
    FILE_DCI_C.WriteLine ("")

'****************************************************************************************************************************
'******     Create the Ram pointer blocks.
'****************************************************************************************************************************
    
    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("")
    FILE_DCI_C.WriteLine ("//*******************************************************")
    FILE_DCI_C.WriteLine ("//******     The Register Cross Reference Table.")
    FILE_DCI_C.WriteLine ("//*******************************************************")
    FILE_DCI_C.WriteLine ("")
    
    FILE_DCI_C.WriteLine ("static const EZCOM_DCI_XREF_ST_TD ezcom_xref[EZCOM_DCI_XREF_TOTAL_REGISTERS] = ")
    FILE_DCI_C.WriteLine ("{")
    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        FILE_DCI_C.WriteLine ("    {        // " & Cells(counter, DESCRIP_COL).text)
        FILE_DCI_C.WriteLine ("        " & Cells(counter, DATA_DEFINE_COL).text & "_EZCOM_REG,")
        FILE_DCI_C.WriteLine ("        " & Cells(counter, DATA_DEFINE_COL).text & "_DCID")
        If (Cells(counter + 1, DATA_DEFINE_COL).text = Empty) Or (Cells(counter + 1, DATA_DEFINE_COL).text = BEGIN_IGNORED_DATA) Then
            FILE_DCI_C.WriteLine ("    } ")
        Else
            FILE_DCI_C.WriteLine ("    }, ")
        End If
        counter = counter + 1
    Wend
    FILE_DCI_C.WriteLine ("};")

    FILE_DCI_H.Close
    FILE_DCI_C.Close

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

Sub Verify_EZCom_List()

'*********************
'******     Constants
'*********************
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    
    START_ROW = 9
    DCI_DEFINES_START_ROW = 9
    DCI_DESCRIPTORS_COL = 1
    DCI_DEFINES_COL = 2
    DCI_LENGTH_COL = 6

    DESCRIP_COL = 1
    DATA_DEFINE_COL = 2
    DATA_LENGTH_COL = 3
    DATA_DESC_MISSING_COL = DESCRIP_COL
    DATA_DEFINE_MISSING_COL = DATA_DEFINE_COL
    DATA_LENGTH_MISSING_COL = DATA_LENGTH_COL
    


'*********************
'******     Begin
'*********************
    Sheets("EZCom").Select

    
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
        Set C = Worksheets("EZCom").Range("B9:AZ300").Find(lookup_string)
        If C Is Nothing Then '
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


    Sheets("EZCom").Select
'****************************************************************************************************************************
'******     Find the Data items that have been removed from the main list
'****************************************************************************************************************************
    reg_row_ctr = START_ROW
    missing_counter = START_ROW
    While Cells(reg_row_ctr, DATA_DEFINE_COL).text <> Empty
        lookup_string = Cells(reg_row_ctr, DATA_DEFINE_COL).text
        If lookup_string <> BEGIN_IGNORED_DATA Then
            Set C = Worksheets("DCI Descriptors").Range("B9:B500").Find(lookup_string)
            If C Is Nothing Then
                Cells(reg_row_ctr, DATA_DEFINE_COL).Interior.Color = RGB(255, 0, 0)
                Cells(reg_row_ctr, DESCRIP_COL).Interior.Color = RGB(255, 0, 0)
            End If
        End If
        reg_row_ctr = reg_row_ctr + 1
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

End Sub



