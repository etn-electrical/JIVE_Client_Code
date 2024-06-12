Attribute VB_Name = "Create_J1939_Definition"
'**************************
'**************************
'J1939_MSG to DCI Construction sheet.
'**************************
'**************************
'**************************************************************

Sub J1939_MSG_Definition()

    If (Verify_J1939_MSG) Then
        
    ''*********************
    '******     Constants
    '*********************
        'J1939 Sheet Indexing.
        START_ROW = 8
        
        'These constants are from DCI descriptors Sheet
        DCI_COL = 2
        DATATYPE_COL = 3
        
        'These constants are from J1939 Sheet
        DESCRIPTION_COL = 1
        DCID_COL = 2
        SPN_NUMBER_COL = 3
        PGN_COL = 4
        INDEX_COL = 5
        TX_RATE_COL = 6
        PRIORITY_COL = 7
        OVERRIDE_COL = 8
        BIT_LENGTH_COL = 9
        BIT_START_COL = 10
        Dim BEGIN_IGNORED_DATA As String
        BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
        Application.Calculation = xlCalculationManual
        
        Dim file_name As String
        Dim lower_case_file_name As String
        Dim upper_case_file_name As String
        
        Dim range_string As String
        
        file_name = Cells(2, 2).value
        lower_case_file_name = LCase(file_name)
        upper_case_file_name = UCase(file_name)
        end_row = ActiveWorkbook.Sheets("J1939").Range("B" & Rows.count).End(xlUp).row
        
        'Making font type and size consistent.
        Worksheets("J1939").Range("A:J").Font.name = "Arial"
        Worksheets("J1939").Range("A:J").Font.Size = 10
         
        Worksheets("J1939").Range("C8").Locked = True
    
    '******************************************
    '******     Begin with the file creation
    '******************************************
        sheet_name = ActiveSheet.name
        Sheets(sheet_name).Select
        Set fs = CreateObject("Scripting.FileSystemObject")
        file_path = ActiveWorkbook.Path & "\" & Cells(3, 2).value
        Set FILE_DCI_C = fs.CreateTextFile(file_path, True)
        file_path = ActiveWorkbook.Path & "\" & Cells(4, 2).value
        Set FILE_DCI_H = fs.CreateTextFile(file_path, True)
    
    '****************************************************************************************************************************
    '******     Start Creating the .H file header
    '****************************************************************************************************************************
        FILE_DCI_H.WriteLine ("/**")
        FILE_DCI_H.WriteLine ("*****************************************************************************************")
        FILE_DCI_H.WriteLine ("*   @file")
        FILE_DCI_H.WriteLine ("*")
        FILE_DCI_H.WriteLine ("*   @brief")
        FILE_DCI_H.WriteLine ("*")
        FILE_DCI_H.WriteLine ("*   @details")
        FILE_DCI_H.WriteLine ("*")
        FILE_DCI_H.WriteLine ("*   @version")
        FILE_DCI_H.WriteLine ("*   C++ Style Guide Version 1.0")
        FILE_DCI_H.WriteLine ("*   Last Modified Date: " & MyDate)
        FILE_DCI_H.WriteLine ("*   @copyright 2019 Eaton Corporation. All Rights Reserved.")
        FILE_DCI_H.WriteLine ("*")
        FILE_DCI_H.WriteLine ("*****************************************************************************************")
        FILE_DCI_H.WriteLine ("*/")
        FILE_DCI_H.WriteLine ("")
        FILE_DCI_H.WriteLine ("#ifndef " & upper_case_file_name & "_H")
        FILE_DCI_H.WriteLine ("    #define " & upper_case_file_name & "_H")
        FILE_DCI_H.WriteLine ("")
        FILE_DCI_H.WriteLine ("#include ""DCI.h""")
        FILE_DCI_H.WriteLine ("#include ""J1939_DCI.h""")
        FILE_DCI_H.WriteLine ("")
        FILE_DCI_H.WriteLine ("//*******************************************************")
        FILE_DCI_H.WriteLine ("//******     J1939 message DCI structure and size follow.")
        FILE_DCI_H.WriteLine ("//*******************************************************")
        FILE_DCI_H.WriteLine ("")
        FILE_DCI_H.WriteLine ("extern const uint16_t TOTAL_J1939_MESSAGES;")
        FILE_DCI_H.WriteLine ("extern const J1939_DCI_STRUCT base_dci_j1939_msg_list[];")
        FILE_DCI_H.WriteLine ("")
        FILE_DCI_H.WriteLine ("")
        FILE_DCI_H.WriteLine ("#endif")
        
    '****************************************************************************************************************************
    '******     Start Creating the .C file header
    '****************************************************************************************************************************
        
        row_counter = START_ROW
        row_count = START_ROW ' this is used to format the .cpp file with proper number of lines
        Dim Format_Count As Integer ' this is used to format the .cpp file with proper number of lines
        
        no_of_pgn = 0
        Dim PGN_Array() As Variant
        Dim TOTAL_J1939_MESSAGES As Integer
        Dim DataRange As Range
        Dim Cell As Range
        Dim x As Long
        Dim Msg_Array_Count As Integer
        
        Dim Total_Msg_Size As Integer
        Dim New_Line_Counter As Integer
        Dim Msg_Array_String As String
        Dim Signal_Length_String As String
        Dim J1939_Struct_String As String
        Dim PGN_Length_Dict As Object
        Set PGN_Length_Dict = CreateObject("Scripting.Dictionary")
        Dim J1939_Struct_Dict As Object
        Set J1939_Struct_Dict = CreateObject("Scripting.Dictionary")
        
        PGN_Array = Find_Unique_PGN
        TOTAL_J1939_MESSAGES = UBound(PGN_Array)
    
        FILE_DCI_C.WriteLine ("/**")
        FILE_DCI_C.WriteLine ("*****************************************************************************************")
        FILE_DCI_C.WriteLine ("*   @file")
        FILE_DCI_C.WriteLine ("*   @details See header file for module overview.")
        FILE_DCI_C.WriteLine ("*   @copyright 2019 Eaton Corporation. All Rights Reserved.")
        FILE_DCI_C.WriteLine ("*")
        FILE_DCI_C.WriteLine ("*****************************************************************************************")
        FILE_DCI_C.WriteLine ("*/")
        FILE_DCI_C.WriteLine ("#include ""Includes.h""")
        FILE_DCI_C.WriteLine ("#include ""J1939_DCI_Data.h""")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        FILE_DCI_C.WriteLine ("//******     J1939 message parameter.")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("const uint16_t TOTAL_J1939_MESSAGES = " & TOTAL_J1939_MESSAGES & "U;")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        FILE_DCI_C.WriteLine ("//******     J1939 PGN signal list")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        
        Dim DCID_Datatype_Dict As Object
        Set DCID_Datatype_Dict = CreateObject("Scripting.Dictionary")
        Dim Datatype_Dict As Object
        Set Datatype_Dict = CreateObject("Scripting.Dictionary")
        
        'The dictionary created here is used to store corresponding bit size for each datatype
        Datatype_Dict("DCI_DTYPE_BOOL") = 1
        Datatype_Dict("DCI_DTYPE_SINT8") = 8
        Datatype_Dict("DCI_DTYPE_SINT16") = 16
        Datatype_Dict("DCI_DTYPE_SINT32") = 32
        Datatype_Dict("DCI_DTYPE_SINT64") = 64
        Datatype_Dict("DCI_DTYPE_UINT8") = 8
        Datatype_Dict("DCI_DTYPE_UINT16") = 16
        Datatype_Dict("DCI_DTYPE_UINT32") = 32
        Datatype_Dict("DCI_DTYPE_UINT64") = 64
        Datatype_Dict("DCI_DTYPE_FLOAT") = 32
        Datatype_Dict("DCI_DTYPE_DFLOAT") = 64
        Datatype_Dict("DCI_DTYPE_BYTE") = 8
        Datatype_Dict("DCI_DTYPE_WORD") = 16
        Datatype_Dict("DCI_DTYPE_DWORD") = 64
        Datatype_Dict("DCI_DTYPE_STRING8") = 8
        
        Worksheets("DCI Descriptors").Activate
        end_row = ActiveWorkbook.Sheets("DCI Descriptors").Range("B" & Rows.count).End(xlUp).row
        row_counter = START_ROW
        
        For i = row_counter To end_row Step 1
            DCID_Datatype_Dict.Add Cells(i, DCI_COL).text, Cells(i, DATATYPE_COL).text
            row_counter = row_counter + 1
        Next i
        
        Worksheets("J1939").Activate
        end_row = ActiveWorkbook.Sheets("J1939").Range("B" & Rows.count).End(xlUp).row
        
        row_counter = START_ROW
        x = LBound(PGN_Array)
     
        While (Cells(row_counter, DCID_COL).value <> BEGIN_IGNORED_DATA)
            object_type_DCID = Cells(row_counter, DCID_COL).value
            Msg_Array_Count = 0
            Format_Count = 0
            Total_Msg_Size = 0
            New_Line_Counter = 0
            Msg_Array_String = "{ "
            Signal_Length_String = "{ "
            J1939_Struct_String = Chr(9) & "{   "
            
           'need this count to format the code in .cpp file
            While (Cells(row_count, PGN_COL).value = PGN_Array(x))
                Format_Count = Format_Count + 1
                row_count = row_count + 1
            Wend
            
            While (Cells(row_counter, PGN_COL).value = PGN_Array(x))
                    New_Line_Counter = New_Line_Counter + 1
                    
                    If (Cells(row_counter, DCID_COL).value <> Empty) Then
                        Msg_Array_String = Msg_Array_String & Cells(row_counter, DCID_COL).value & "_DCID, "
                    End If
                    
                    'Cells(row_counter, BIT_LENGTH_COL).value = Datatype_Dict(DCID_Datatype_Dict(Cells(row_counter, DCID_COL).value))
                    
                    If (Cells(row_counter, OVERRIDE_COL).value <> Empty) Then
                        Signal_Length_String = Signal_Length_String & Cells(row_counter, OVERRIDE_COL).value & "U, "
                    Else
                        Signal_Length_String = Signal_Length_String & Cells(row_counter, BIT_LENGTH_COL).value & "U, "
                    End If
                    
                    'writing bit Start Position in the sheet, by adding bitlengths
                    Cells(row_counter, BIT_START_COL).value = Total_Msg_Size
                    If (Cells(row_counter, OVERRIDE_COL).value <> Empty) Then
                        Total_Msg_Size = Total_Msg_Size + Cells(row_counter, OVERRIDE_COL).value
                    Else
                        Total_Msg_Size = Total_Msg_Size + Cells(row_counter, BIT_LENGTH_COL).value
                    End If
                    row_counter = row_counter + 1
                    Msg_Array_Count = Msg_Array_Count + 1
                    
                    'add tabs and Newline to maintain the length of PGN signal list in .cpp file
                    If New_Line_Counter = 4 And Msg_Array_Count < Format_Count Then
                        Msg_Array_String = Msg_Array_String & Chr(13) & Chr(10) & Chr(9) & Chr(9) & Chr(9) & _
                        Chr(9) & Chr(9) & Chr(9) & Chr(9) & Chr(9) & Chr(9) & Chr(9) & Chr(9) & Chr(9)
                        New_Line_Counter = 0
                    End If
            Wend
           
           'remove the last ", " from the string
           Msg_Array_String = Left(Msg_Array_String, Len(Msg_Array_String) - 2)
           Signal_Length_String = Left(Signal_Length_String, Len(Signal_Length_String) - 2)
           
           Msg_Array_String = Msg_Array_String & " };"
           Signal_Length_String = Signal_Length_String & " };"
           FILE_DCI_C.WriteLine ("static const DCI_ID_TD J1939_MSG_PGN_" & PGN_Array(x) & "[" & Msg_Array_Count & "U] = " & Msg_Array_String)
           
           Signal_Length_String = "static const uint8_t J1939_MSG_PGN_" & PGN_Array(x) & "_BITLENGTH[" & Msg_Array_Count & "U] = " & Signal_Length_String
           PGN_Length_Dict.Add PGN_Array(x), Signal_Length_String
           
            J1939_Struct_String = J1939_Struct_String & "/* Information for PGN number " & PGN_Array(x) & " */" & Chr(13) & Chr(10) & Chr(9) & Chr(9) & PGN_Array(x) & "," & Chr(13) & Chr(10) & Chr(9) & Chr(9) & Msg_Array_Count & "U," & Chr(13) & Chr(10) & Chr(9) & Chr(9) & "8U," & Chr(13) & Chr(10)
            J1939_Struct_String = J1939_Struct_String & Chr(9) & Chr(9) & "J1939_MSG_PGN_" & PGN_Array(x) & "_BITLENGTH," & Chr(13) & Chr(10) & Chr(9) & Chr(9) & "J1939_MSG_PGN_" & PGN_Array(x) & Chr(13) & Chr(10) & Chr(9) & "},"
            J1939_Struct_Dict.Add PGN_Array(x), J1939_Struct_String
                         
           x = x + 1
        Wend
        
        
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        FILE_DCI_C.WriteLine ("//******     PGN Signal length definition")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("")
        
        For Each key In PGN_Length_Dict.keys
            FILE_DCI_C.WriteLine (PGN_Length_Dict(key))
        Next key
        
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        FILE_DCI_C.WriteLine ("//******     J1939 message info table.")
        FILE_DCI_C.WriteLine ("//*******************************************************")
        FILE_DCI_C.WriteLine ("")
        FILE_DCI_C.WriteLine ("const J1939_DCI_STRUCT base_dci_j1939_msg_list[TOTAL_J1939_MESSAGES]")
        FILE_DCI_C.WriteLine ("{")
        
        For Each key In J1939_Struct_Dict.keys
            FILE_DCI_C.WriteLine (J1939_Struct_Dict(key))
        Next key
        
        FILE_DCI_C.WriteLine ("};")
        
    Else
        
    '*********************
    '******     Creating File with error meassage
    '*********************
        sheet_name = ActiveSheet.name
        Sheets(sheet_name).Select
        Set fs = CreateObject("Scripting.FileSystemObject")
        file_path = ActiveWorkbook.Path & "\" & Cells(3, 2).value
        Set FILE_DCI_C = fs.CreateTextFile(file_path, True)
        file_path = ActiveWorkbook.Path & "\" & Cells(4, 2).value
        Set FILE_DCI_H = fs.CreateTextFile(file_path, True)
        FILE_DCI_H.WriteLine ("//ERROR DETECTED")
        FILE_DCI_H.WriteLine ("//Please check DCI sheet")
        FILE_DCI_C.WriteLine ("//ERROR DETECTED")
        FILE_DCI_C.WriteLine ("//Please check DCI sheet")
    
    End If
    
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

Function Verify_J1939_MSG() As Boolean
'*********************
'******     Constants
'*********************
    Application.Calculation = xlCalculationManual
    
    'J1939 Sheet Indexing.
    START_ROW = 8
    
    'These constants are from DCI discriptors Sheet
    DCI_COL = 2
    DATATYPE_COL = 3
    DESP_COL = 81
    
    'These constants are from J1939 Sheet
    DESCRIPTION_COL = 1
    DCID_COL = 2
    SPN_NUMBER_COL = 3
    PGN_COL = 4
    INDEX_COL = 5
    TX_RATE_COL = 6
    PRIORITY_COL = 7
    OVERRIDE_COL = 8
    BIT_LENGTH_COL = 9
    BIT_START_COL = 10
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    Application.Calculation = xlCalculationManual
    Dim sheet_name As String
    Dim DCID_Temp As String
    Dim Source As Range
    Dim end_row As Integer
    Dim row_counter As Integer
    Dim count As Integer
    Dim counter As Integer
    Dim DCID_Array As Variant ' initial storage array to take values
    Dim DCID_Present As Variant
    Dim Column_Check As Variant
    Dim i As Integer
    Dim j As Integer
    Dim x As Long
    Dim y As Long
    Dim DataRange As Range
    Dim arr_size As Integer
    Dim Cell As Range
    Dim Datatype_Dict As Object
    Set Datatype_Dict = CreateObject("Scripting.Dictionary")
    Dim DCID_Datatype_Dict As Object
    Set DCID_Datatype_Dict = CreateObject("Scripting.Dictionary")
    Dim DCID_Description_Dict As Object
    Set DCID_Description_Dict = CreateObject("Scripting.Dictionary")
    Dim Verified As Boolean
    Verified = True
     
    Datatype_Dict("DCI_DTYPE_BOOL") = 1
    Datatype_Dict("DCI_DTYPE_SINT8") = 8
    Datatype_Dict("DCI_DTYPE_SINT16") = 16
    Datatype_Dict("DCI_DTYPE_SINT32") = 32
    Datatype_Dict("DCI_DTYPE_SINT64") = 64
    Datatype_Dict("DCI_DTYPE_UINT8") = 8
    Datatype_Dict("DCI_DTYPE_UINT16") = 16
    Datatype_Dict("DCI_DTYPE_UINT32") = 32
    Datatype_Dict("DCI_DTYPE_UINT64") = 64
    Datatype_Dict("DCI_DTYPE_FLOAT") = 32
    Datatype_Dict("DCI_DTYPE_DFLOAT") = 64
    Datatype_Dict("DCI_DTYPE_BYTE") = 8
    Datatype_Dict("DCI_DTYPE_WORD") = 16
    Datatype_Dict("DCI_DTYPE_DWORD") = 64
    Datatype_Dict("DCI_DTYPE_STRING8") = 8
     
    no_of_pgn = 0
    Dim PGN_Array() As Variant
    Dim Correct_PGN_Array() As Variant

    
'*********************
'******     Begin
'*********************
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select
               
'****************************************************************************************************************************
'******     Sort the J1939 list
'*************************************************************************************************************
        
    counter = START_ROW
    While (Cells(counter, DCID_COL).text <> Empty) And (Cells(counter, DCID_COL).text <> BEGIN_IGNORED_DATA)
        counter = counter + 1
    Wend
       
    Dim strDataRange As Range
    Set strDataRange = Range(Cells(START_ROW, DESCRIPTION_COL), Cells(counter, BIT_START_COL))
    
    'Sorting on basis of Index
    strDataRange.Sort Key1:=Range("E8"), Order1:=xlAscending, _
    Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
    Orientation:=xlTopToBottom
    
    'Sorting for PGN
    strDataRange.Sort Key1:=Range("D8"), Order1:=xlAscending, _
    Header:=xlGuess, OrderCustom:=1, MatchCase:=False, _
    Orientation:=xlTopToBottom
    
'****************************************************************************************************************************
'******     Checking for Blanks
'****************************************************************************************************************************
    
    end_row = ActiveWorkbook.Sheets("J1939").Range("A" & Rows.count).End(xlUp).row
    x = 0
    row_counter = START_ROW
    
    'clearing all the errors before validation
    Range("A8:K" & end_row).Interior.Color = xlNone
    Range("J8:J" & end_row).ClearContents
    Range("I8:I" & end_row).ClearContents

    counter = START_ROW
    While (Cells(counter, DCID_COL).text <> BEGIN_IGNORED_DATA)
        counter = counter + 1
    Wend
    
    'Add Columns here to check for blanks
    Column_Check = Array("B", "D", "E")
    
    For i = 0 To UBound(Column_Check) Step 1
        For j = row_counter To end_row Step 1
            If (Cells(j, Column_Check(i)).text = Empty) Then
                Cells(j, Column_Check(i)).Interior.Color = RGB(255, 0, 0)
                Verified = False
            End If
        Next j
    Next i

'**********************************************************
'******     Checking if DCID is present in DCI Descriptors
'**********************************************************
    If (Verified) Then
    
        end_row = ActiveWorkbook.Sheets("DCI Descriptors").Range("B" & Rows.count).End(xlUp).row
        x = 0
        row_counter = START_ROW + 1
        Worksheets("DCI Descriptors").Activate
          
        ReDim DCID_Array(end_row - row_counter)
        
            For i = row_counter To end_row Step 1
                DCID_Array(x) = Worksheets("DCI Descriptors").Cells(i, DCI_COL).text
                DCID_Datatype_Dict.Add Cells(i, DCI_COL).text, Cells(i, DATATYPE_COL).text
                DCID_Description_Dict.Add Cells(i, DCI_COL).text, Cells(i, DESP_COL).text
                x = x + 1
            Next i
            
            arr_size = UBound(DCID_Array)
                
            row_counter = START_ROW
            Worksheets("J1939").Activate
            row_counter = START_ROW
            
        While (Cells(row_counter, DCID_COL).value <> BEGIN_IGNORED_DATA)
            For i = 0 To arr_size Step 1
                If (Cells(row_counter, DCID_COL).value = DCID_Array(i)) Then
                    Cells(row_counter, DESCRIPTION_COL).value = DCID_Description_Dict(DCID_Array(i))
                End If
            Next i
            row_counter = row_counter + 1
        Wend
        
        While (Cells(row_counter, DCID_COL).value <> BEGIN_IGNORED_DATA)
    
            DCID_Present = Application.Match(Cells(row_counter, DCID_COL).value, DCID_Array, 0)
            If IsError(DCID_Present) Then
                Cells(row_counter, DCID_COL).Interior.Color = RGB(255, 0, 0)
                Verified = False
            End If
            row_counter = row_counter + 1
            
        Wend
        
    '****************************************************************************************************************************
    '******     Checking for duplicate Index numbers
    '****************************************************************************************************************************
        row_counter = START_ROW
        ReDim PGN_Array(counter)
        
        While (Cells(row_counter, DCID_COL).value <> BEGIN_IGNORED_DATA)
            object_type = Cells(row_counter + 1, PGN_COL).value
            pre_object_type = Cells(row_counter, PGN_COL).value
            If (pre_object_type <> object_type) Then
                no_of_pgn = no_of_pgn + 1
                PGN_Array(y) = pre_object_type
                y = y + 1
            End If
            row_counter = row_counter + 1
        Wend
        
        Pgn_Size = LBound(PGN_Array)
        row_counter = START_ROW
        count = START_ROW
     
        While (Cells(row_counter, DCID_COL).value <> BEGIN_IGNORED_DATA)
    
            Msg_Array_Count = 0
            
            If (Cells(row_counter, INDEX_COL).value = 0) Then
                Cells(row_counter, INDEX_COL).Interior.Color = RGB(191, 191, 191)
                Cells(row_counter, TX_RATE_COL).Interior.Color = RGB(191, 191, 191)
                Cells(row_counter, PRIORITY_COL).Interior.Color = RGB(191, 191, 191)
            End If
            
            While (Cells(row_counter, PGN_COL).value = PGN_Array(Pgn_Size))
                Msg_Array_Count = Msg_Array_Count + 1
                row_counter = row_counter + 1
            Wend
            
            Set Source = Range(Cells(row_counter - Msg_Array_Count, INDEX_COL), Cells(row_counter - 1, INDEX_COL))
    
            For Each Cell In Source
                If Application.WorksheetFunction.CountIf(Source, Cell) > 1 Then
                    Cell.Interior.Color = RGB(255, 0, 0)
                    Verified = False
                End If
            Next
           Pgn_Size = Pgn_Size + 1
        Wend
    
    '********************************************
    '******     Checking bit length for each PGN
    '********************************************
        
        Pgn_Size = LBound(PGN_Array)
        row_counter = START_ROW
        count = START_ROW
        Total_Bit_Length = 0
        
        While (Cells(row_counter, DCID_COL).value <> BEGIN_IGNORED_DATA)
            
            Msg_Array_Count = 0
            While (Cells(row_counter, PGN_COL).value = PGN_Array(Pgn_Size))
                Msg_Array_Count = Msg_Array_Count + 1
                row_counter = row_counter + 1
            Wend
            
            For i = 1 To Msg_Array_Count Step 1
                Cells(count, BIT_START_COL).value = Total_Bit_Length
                If (Cells(count, OVERRIDE_COL).value <> Empty) Then
                    Cells(count, BIT_LENGTH_COL).value = Cells(count, OVERRIDE_COL).value
                    Cells(count, BIT_LENGTH_COL).Interior.Color = RGB(149, 179, 215)
                    Total_Bit_Length = Total_Bit_Length + Cells(count, OVERRIDE_COL).value
                Else
                    Cells(count, BIT_LENGTH_COL).value = Datatype_Dict(DCID_Datatype_Dict(Cells(count, DCID_COL).value))
                    Cells(count, BIT_LENGTH_COL).Interior.Color = RGB(255, 255, 0)
                    Total_Bit_Length = Total_Bit_Length + Cells(count, BIT_LENGTH_COL).value
                End If
                count = count + 1
            Next i
            
            If (Total_Bit_Length > 64) Then
                Range(Cells(row_counter - Msg_Array_Count, BIT_START_COL), Cells(row_counter - 2, BIT_START_COL)).Interior.Color = RGB(255, 255, 0)
                Cells(row_counter - 1, BIT_START_COL).Interior.Color = RGB(255, 0, 0)
                Verified = False
            Else
                Range(Cells(row_counter - Msg_Array_Count, BIT_START_COL), Cells(row_counter - 1, BIT_START_COL)).Interior.Color = RGB(255, 255, 0)
            End If
            Pgn_Size = Pgn_Size + 1
            Total_Bit_Length = 0
        Wend
        
        '*******************************************
        '****** Validating the datatype of each DCID
        '*******************************************
        
        row_counter = START_ROW
        
        While (Cells(row_counter, DCID_COL).value <> BEGIN_IGNORED_DATA)
          If (Cells(row_counter, OVERRIDE_COL).value > (Datatype_Dict(DCID_Datatype_Dict(Cells(row_counter, DCID_COL).value)))) Then
            Cells(row_counter, OVERRIDE_COL).Interior.Color = RGB(255, 0, 0)
            Cells(row_counter, OVERRIDE_COL).Interior.Color = RGB(255, 0, 0)
            Verified = False
          End If
          row_counter = row_counter + 1
        Wend
    End If
          
    Verify_J1939_MSG = Verified
    
        
End Function

Function Find_Unique_PGN() As Variant
    Dim Unique_PGN_Array() As Variant
    Dim LastRow As Integer
    Dim LastRowAfterCopy As Integer
    Dim TOTAL_J1939_MESSAGES As Integer
    Dim index As Integer
    index = 0
    PGN_COL = 4
    
    With ActiveWorkbook.ActiveSheet
        LastRow = .Cells(.Rows.count, "D").End(xlUp).row
    End With
    
    Set uniqueRng = ActiveWorkbook.ActiveSheet.Range("D7:D" & LastRow)
    uniqueRng.AdvancedFilter Action:=xlFilterCopy, CopyToRange:= _
                ActiveWorkbook.ActiveSheet.Range("D" & LastRow + 10), Unique:=True
    
    With ActiveWorkbook.ActiveSheet
        LastRowAfterCopy = .Cells(.Rows.count, "D").End(xlUp).row
    End With
    
    'we need to subtract 1 as the advancedFilter will take the first row a header
    TOTAL_J1939_MESSAGES = (ActiveWorkbook.ActiveSheet.Range("D" & LastRow + 10 & ":" & "D" & LastRowAfterCopy).count) - 1
    
    'Resize Array prior to loading data
    ReDim Unique_PGN_Array(TOTAL_J1939_MESSAGES)
    
    For i = LastRow + 11 To LastRowAfterCopy Step 1
            Unique_PGN_Array(index) = Cells(i, PGN_COL).value
            index = index + 1
    Next i
    
    ActiveWorkbook.ActiveSheet.Range("D" & LastRow + 10 & ":" & "D" & LastRowAfterCopy).Clear
    
    Find_Unique_PGN = Unique_PGN_Array
    
End Function





