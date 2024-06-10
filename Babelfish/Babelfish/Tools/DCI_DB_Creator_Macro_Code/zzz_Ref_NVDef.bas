Attribute VB_Name = "zzz_Ref_NVDef"
Sub Generate_NV_Default_Files()
    
    Dim counter
    
    Set fs = CreateObject("Scripting.FileSystemObject")
    Set NV_DEFAULT_C = fs.CreateTextFile(Cells(3, 2).value, True)
    Set NV_DEFAULT_H = fs.CreateTextFile(Cells(4, 2).value, True)
    
    Dim temp_string As String
    
'****************************************************************************************************************************
'******     Start Creating the .H file header
'****************************************************************************************************************************
    NV_DEFAULT_H.WriteLine ("/*")
    NV_DEFAULT_H.WriteLine ("*****************************************************************************************")
    NV_DEFAULT_H.WriteLine ("*       $Workfile:$")
    NV_DEFAULT_H.WriteLine ("*")
    NV_DEFAULT_H.WriteLine ("*       $Author:$")
    NV_DEFAULT_H.WriteLine ("*       $Date:$")
    NV_DEFAULT_H.WriteLine ("*       Copyright © Eaton Corporation. All Rights Reserved.")
    NV_DEFAULT_H.WriteLine ("*       Creator: Mark A Verheyen")
    NV_DEFAULT_H.WriteLine ("*")
    NV_DEFAULT_H.WriteLine ("*       Description:")
    NV_DEFAULT_H.WriteLine ("*")
    NV_DEFAULT_H.WriteLine ("*")
    NV_DEFAULT_H.WriteLine ("*       Code Inspection Date: ")
    NV_DEFAULT_H.WriteLine ("*")
    NV_DEFAULT_H.WriteLine ("*       $Header:$")
    NV_DEFAULT_H.WriteLine ("*****************************************************************************************")
    NV_DEFAULT_H.WriteLine ("*/")
    NV_DEFAULT_H.WriteLine ("#ifndef EEPROM_DEFAULT_VALS_H")
    NV_DEFAULT_H.WriteLine ("    #define EEPROM_DEFAULT_VALS_H")
    NV_DEFAULT_H.WriteLine ("")
    NV_DEFAULT_H.WriteLine ("")

'****************************************************************************************************************************
'******     Start Creating the .C file header
'****************************************************************************************************************************
    NV_DEFAULT_C.WriteLine ("/*")
    NV_DEFAULT_C.WriteLine ("*****************************************************************************************")
    NV_DEFAULT_C.WriteLine ("*       $Workfile:")
    NV_DEFAULT_C.WriteLine ("*")
    NV_DEFAULT_C.WriteLine ("*       $Author:$")
    NV_DEFAULT_C.WriteLine ("*       $Date:$")
    NV_DEFAULT_C.WriteLine ("*       Copyright© 2001-2004, Eaton Corporation. All Rights Reserved.")
    NV_DEFAULT_C.WriteLine ("*")
    NV_DEFAULT_C.WriteLine ("*       $Header:$")
    NV_DEFAULT_C.WriteLine ("*****************************************************************************************")
    NV_DEFAULT_C.WriteLine ("*/")
    NV_DEFAULT_C.WriteLine ("#include ""includes.h""")
    NV_DEFAULT_C.WriteLine ("#include ""NV_Default_Vals.h""")
    NV_DEFAULT_C.WriteLine ("#include ""NV_Address.h""")
    NV_DEFAULT_C.WriteLine ("")
    NV_DEFAULT_C.WriteLine ("")

'****************************************************************************************************************************
'******     Setup the struct stuff
'****************************************************************************************************************************

    NV_DEFAULT_H.WriteLine ("typedef struct")
    NV_DEFAULT_H.WriteLine ("{")
    NV_DEFAULT_H.WriteLine ("    uint16_t address;")
    NV_DEFAULT_H.WriteLine ("    void flash * value;")
    NV_DEFAULT_H.WriteLine ("    uint8_t size;")
    NV_DEFAULT_H.WriteLine ("} NV_CHECK_VALS_STRUCT_TYPE;")



'****************************************************************************************************************************
'******     Setup the enum in the H file
'****************************************************************************************************************************
    EEPROM_ADDRESS_DEFINE_START_ROW = 7
    EEPROM_DEFINE_COLUMN = 1
    EEPROM_LENGTH_COLUMN = 4
    EEPROM_DEFAULT_COLUMN = 3
    EEPROM_DEFAULT_DATATYPE = 2

    Sheets("NV Defaults").Select

    NV_DEFAULT_H.WriteLine ("")
    NV_DEFAULT_H.WriteLine ("enum")
    NV_DEFAULT_H.WriteLine ("{")
    
    val_counter = 0
    row_counter = EEPROM_ADDRESS_DEFINE_START_ROW
    While Cells(row_counter, EEPROM_DEFINE_COLUMN).text <> Empty
        If Cells(row_counter, EEPROM_DEFAULT_COLUMN).text <> Empty Then
            NV_DEFAULT_H.WriteLine ("    EEPROM_DEFAULT_" & Cells(row_counter, EEPROM_DEFINE_COLUMN).text & ",")
            val_counter = val_counter + 1
        End If
        row_counter = row_counter + 1
    Wend
    NV_DEFAULT_H.WriteLine ("    TOTAL_EEPROM_DEFAULT_VALS")
    NV_DEFAULT_H.WriteLine ("};")

    If (val_counter > 0) Then

'****************************************************************************************************************************
'******     Setup the struct flash data
'****************************************************************************************************************************
        NV_DEFAULT_H.WriteLine ("")
        NV_DEFAULT_H.WriteLine ("extern flash NV_CHECK_VALS_STRUCT_TYPE eeprom_defaults[];")
        NV_DEFAULT_H.WriteLine ("")
        

'****************************************************************************************************************************
'******     Setup the constants in the C file
'****************************************************************************************************************************
    
        row_counter = EEPROM_ADDRESS_DEFINE_START_ROW
        While Cells(row_counter, EEPROM_DEFINE_COLUMN).text <> Empty
            If Cells(row_counter, EEPROM_DEFAULT_COLUMN).text <> Empty Then
                NV_DEFAULT_C.WriteLine ("flash " & Cells(row_counter, EEPROM_DEFAULT_DATATYPE).text & " " & _
                        Cells(row_counter, EEPROM_DEFINE_COLUMN).text & "_NVDEFAULT_VAL = " & _
                        Cells(row_counter, EEPROM_DEFAULT_COLUMN).text & ";")
                val_counter = val_counter + 1
            End If
            row_counter = row_counter + 1
        Wend
  
    
'****************************************************************************************************************************
'******     Setup the struct in the C file
'****************************************************************************************************************************
    
    
        NV_DEFAULT_C.WriteLine ("")
        NV_DEFAULT_C.WriteLine ("flash NV_CHECK_VALS_STRUCT_TYPE eeprom_defaults[TOTAL_EEPROM_DEFAULT_VALS] =")
        NV_DEFAULT_C.WriteLine ("{")
        
        row_counter = EEPROM_ADDRESS_DEFINE_START_ROW
        While Cells(row_counter, EEPROM_DEFINE_COLUMN).text <> Empty
            If Cells(row_counter, EEPROM_DEFAULT_COLUMN).text <> Empty Then
                NV_DEFAULT_C.WriteLine ("    {")
                NV_DEFAULT_C.WriteLine ("        " & Cells(row_counter, EEPROM_DEFINE_COLUMN).text & "_NVADD,")
                NV_DEFAULT_C.WriteLine ("        &" & Cells(row_counter, EEPROM_DEFINE_COLUMN).text & "_NVDEFAULT_VAL,")
                NV_DEFAULT_C.WriteLine ("        sizeof( " & Cells(row_counter, EEPROM_DEFINE_COLUMN).text & "_NVDEFAULT_VAL )")
                NV_DEFAULT_C.WriteLine ("    },")
            End If
            row_counter = row_counter + 1
        Wend
        NV_DEFAULT_C.WriteLine ("};")
    
    End If






'****************************************************************************************************************************
'******     Setup the struct flash data
'****************************************************************************************************************************
    NV_DEFAULT_H.WriteLine ("")
    NV_DEFAULT_H.WriteLine ("extern flash NV_CHECK_VALS_STRUCT_TYPE fram_defaults[];")
    NV_DEFAULT_H.WriteLine ("")


'****************************************************************************************************************************
'******     Setup the enum in the H file
'****************************************************************************************************************************
    EEPROM_ADDRESS_DEFINE_START_ROW = 7
    EEPROM_DEFINE_COLUMN = 9
    EEPROM_LENGTH_COLUMN = 12
    EEPROM_DEFAULT_COLUMN = 11
    EEPROM_DEFAULT_DATATYPE = 10

    Sheets("NV Defaults").Select

    NV_DEFAULT_H.WriteLine ("")
    NV_DEFAULT_H.WriteLine ("enum")
    NV_DEFAULT_H.WriteLine ("{")
    
    val_counter = 0
    row_counter = EEPROM_ADDRESS_DEFINE_START_ROW
    While Cells(row_counter, EEPROM_DEFINE_COLUMN).text <> Empty
        If Cells(row_counter, EEPROM_DEFAULT_COLUMN).text <> Empty Then
            NV_DEFAULT_H.WriteLine ("    FRAM_DEFAULT_" & Cells(row_counter, EEPROM_DEFINE_COLUMN).text & ",")
            val_counter = val_counter + 1
        End If
        row_counter = row_counter + 1
    Wend
    NV_DEFAULT_H.WriteLine ("    TOTAL_FRAM_DEFAULT_VALS")
    NV_DEFAULT_H.WriteLine ("};")

    
    If (val_counter > 0) Then

'****************************************************************************************************************************
'******     Setup the constants in the C file
'****************************************************************************************************************************
        row_counter = EEPROM_ADDRESS_DEFINE_START_ROW
        While Cells(row_counter, EEPROM_DEFINE_COLUMN).text <> Empty
            If Cells(row_counter, EEPROM_DEFAULT_COLUMN).text <> Empty Then
                NV_DEFAULT_C.WriteLine ("flash " & Cells(row_counter, EEPROM_DEFAULT_DATATYPE).text & " " & _
                        Cells(row_counter, EEPROM_DEFINE_COLUMN).text & "_NVDEFAULT_VAL = " & _
                        Cells(row_counter, EEPROM_DEFAULT_COLUMN).text & ";")
            End If
            row_counter = row_counter + 1
        Wend
   
    
'****************************************************************************************************************************
'******     Setup the struct in the C file
'****************************************************************************************************************************
    
        NV_DEFAULT_C.WriteLine ("")
        NV_DEFAULT_C.WriteLine ("flash FRAM_CHECK_VALS_STRUCT_TYPE fram_defaults[TOTAL_FRAM_DEFAULT_VALS] =")
        NV_DEFAULT_C.WriteLine ("{")
        
        row_counter = EEPROM_ADDRESS_DEFINE_START_ROW
        While Cells(row_counter, EEPROM_DEFINE_COLUMN).text <> Empty
            If Cells(row_counter, EEPROM_DEFAULT_COLUMN).text <> Empty Then
                NV_DEFAULT_C.WriteLine ("    {")
                NV_DEFAULT_C.WriteLine ("        " & Cells(row_counter, EEPROM_DEFINE_COLUMN).text & "_NVADD,")
                NV_DEFAULT_C.WriteLine ("        &" & Cells(row_counter, EEPROM_DEFINE_COLUMN).text & "_NVDEFAULT_VAL,")
                NV_DEFAULT_C.WriteLine ("        sizeof( " & Cells(row_counter, EEPROM_DEFINE_COLUMN).text & "_NVDEFAULT_VAL )")
                NV_DEFAULT_C.WriteLine ("    },")
            End If
            row_counter = row_counter + 1
        Wend
        NV_DEFAULT_C.WriteLine ("};")
        NV_DEFAULT_C.WriteLine ("")
    End If


    NV_DEFAULT_H.WriteLine ("")
    NV_DEFAULT_H.WriteLine ("")
    NV_DEFAULT_H.WriteLine ("#endif")
    
    NV_DEFAULT_C.Close
    NV_DEFAULT_H.Close
    
    
End Sub

Sub Generate_NV_Default_List()

    Dim temp_string As String
    Dim lookup_string As String
    Dim length As Integer
    Dim checking_length As Boolean
    Dim found_already As Boolean

    EEPROM_SRC_ADDRESS_DEFINE_START_ROW = 7
    EEPROM_SRC_ADDRESS_DEFINE_COLUMN = 1
    EEPROM_SRC_ADDRESS_COLUMN = 2

    EEPROM_DEST_ADDRESS_DEFINE_START_ROW = 7
    EEPROM_DEST_DEFINE_COLUMN = 1
    EEPROM_DEST_LENGTH_COLUMN = 4

    Sheets("NV Defaults").Select

    dest_counter = EEPROM_DEST_ADDRESS_DEFINE_START_ROW
    While Cells(dest_counter, EEPROM_DEST_DEFINE_COLUMN).text <> Empty
        dest_counter = dest_counter + 1
    Wend

    counter = EEPROM_SRC_ADDRESS_DEFINE_START_ROW
    location_length = 1
    While Worksheets("NVM Address").Range("A1").Cells(counter, EEPROM_SRC_ADDRESS_COLUMN).text <> Empty
        lookup_string = Worksheets("NVM Address").Range("A1").Cells(counter, EEPROM_SRC_ADDRESS_DEFINE_COLUMN).text
        If (lookup_string <> Empty) And (StrComp(lookup_string, NV_MEM_CHECKSUM_STRING) <> 0) Then
            found_already_loc = EEPROM_DEST_ADDRESS_DEFINE_START_ROW
            found_already = False
            While (Cells(found_already_loc, EEPROM_DEST_DEFINE_COLUMN).text <> Empty) And (found_already = False)
                If StrComp(Cells(found_already_loc, EEPROM_DEST_DEFINE_COLUMN).text, lookup_string) = 0 Then
                    found_already = True
                Else
                    found_already_loc = found_already_loc + 1
                End If
            Wend
            If found_already = False Then
                Cells(dest_counter, EEPROM_DEST_DEFINE_COLUMN).value = lookup_string
                dest_counter = dest_counter + 1
            End If
            location_length = 1
            checking_length = True
        Else
            location_length = location_length + 1
        End If
        
        counter = counter + 1
    Wend
    
    
    FRAM_SRC_ADDRESS_DEFINE_START_ROW = 7
    FRAM_SRC_ADDRESS_DEFINE_COLUMN = 5
    FRAM_SRC_ADDRESS_COLUMN = 6
    
    FRAM_DEST_ADDRESS_DEFINE_START_ROW = 7
    FRAM_DEST_DEFINE_COLUMN = 9
    FRAM_DEST_LENGTH_COLUMN = 12

    Sheets("NV Defaults").Select

    dest_counter = FRAM_DEST_ADDRESS_DEFINE_START_ROW
    While Cells(dest_counter, FRAM_DEST_DEFINE_COLUMN).text <> Empty
        dest_counter = dest_counter + 1
    Wend

    counter = FRAM_SRC_ADDRESS_DEFINE_START_ROW
    location_length = 1
    While Worksheets("NVM Address").Range("A1").Cells(counter, FRAM_SRC_ADDRESS_COLUMN).text <> Empty
        lookup_string = Worksheets("NVM Address").Range("A1").Cells(counter, FRAM_SRC_ADDRESS_DEFINE_COLUMN).text
        If (lookup_string <> Empty) And (StrComp(lookup_string, NV_MEM_CHECKSUM_STRING) <> 0) Then
            found_already_loc = FRAM_DEST_ADDRESS_DEFINE_START_ROW
            found_already = False
            While (Cells(found_already_loc, FRAM_DEST_DEFINE_COLUMN).text <> Empty) And (found_already = False)
                If StrComp(Cells(found_already_loc, FRAM_DEST_DEFINE_COLUMN).text, lookup_string) = 0 Then
                    found_already = True
                Else
                    found_already_loc = found_already_loc + 1
                End If
            Wend
            If found_already = False Then
                Cells(dest_counter, FRAM_DEST_DEFINE_COLUMN).value = lookup_string
                dest_counter = dest_counter + 1
            End If
            location_length = 1
            checking_length = True
        Else
            location_length = location_length + 1
        End If
        counter = counter + 1
    Wend

End Sub

Sub Find_Dead_NV_Addresses()

    Dim lookup_string As String

    EEPROM_SRC_ADDRESS_DEFINE_START_ROW = 7
    EEPROM_SRC_ADDRESS_DEFINE_COLUMN = 1
    EEPROM_SRC_ADDRESS_COLUMN = 2
    
    EEPROM_DEST_ADDRESS_DEFINE_START_ROW = 7
    EEPROM_DEST_DEFINE_COLUMN = 1
    EEPROM_DEST_LENGTH_COLUMN = 4

    Sheets("NV Defaults").Select

    dest_counter = EEPROM_DEST_ADDRESS_DEFINE_START_ROW
    While Cells(dest_counter, EEPROM_DEST_DEFINE_COLUMN).text <> Empty
        counter = EEPROM_SRC_ADDRESS_DEFINE_START_ROW
        location_length = 1
        match_found = False
        While (Worksheets("NVM Address").Range("A1").Cells(counter, EEPROM_SRC_ADDRESS_COLUMN).text <> Empty) And (match_found = False)
            lookup_string = Worksheets("NVM Address").Range("A1").Cells(counter, EEPROM_SRC_ADDRESS_DEFINE_COLUMN).text
            If lookup_string <> Empty Then
                If StrComp(Cells(dest_counter, EEPROM_DEST_DEFINE_COLUMN).text, lookup_string) = 0 Then
                    match_found = True
                End If
            End If
            counter = counter + 1
        Wend
        If match_found = False Then
            Cells(dest_counter, EEPROM_DEST_DEFINE_COLUMN).Font.Color = RGB(255, 0, 0)
        End If
        dest_counter = dest_counter + 1
    Wend
  
    
    FRAM_SRC_ADDRESS_DEFINE_START_ROW = 7
    FRAM_SRC_ADDRESS_DEFINE_COLUMN = 5
    FRAM_SRC_ADDRESS_COLUMN = 6
    
    FRAM_DEST_ADDRESS_DEFINE_START_ROW = 7
    FRAM_DEST_DEFINE_COLUMN = 9
    FRAM_DEST_LENGTH_COLUMN = 12

    Sheets("NV Defaults").Select

    dest_counter = FRAM_DEST_ADDRESS_DEFINE_START_ROW
    While Cells(dest_counter, FRAM_DEST_DEFINE_COLUMN).text <> Empty
        counter = FRAM_SRC_ADDRESS_DEFINE_START_ROW
        location_length = 1
        match_found = False
        While (Worksheets("NVM Address").Range("A1").Cells(counter, FRAM_SRC_ADDRESS_COLUMN).text <> Empty) And (match_found = False)
            lookup_string = Worksheets("NVM Address").Range("A1").Cells(counter, FRAM_SRC_ADDRESS_DEFINE_COLUMN).text
            If lookup_string <> Empty Then
                If StrComp(Cells(dest_counter, FRAM_DEST_DEFINE_COLUMN).text, lookup_string) = 0 Then
                    match_found = True
                End If
            End If
            counter = counter + 1
        Wend
        If match_found = False Then
            Cells(dest_counter, FRAM_DEST_DEFINE_COLUMN).Font.Color = RGB(255, 0, 0)
        End If
        dest_counter = dest_counter + 1
    Wend


End Sub




