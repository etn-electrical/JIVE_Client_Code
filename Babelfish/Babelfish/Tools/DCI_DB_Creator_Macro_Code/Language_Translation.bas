Attribute VB_Name = "Language_Translation"
'***************************************************************
' Constants used in update and verify dci list function.
'***************************************************************
Public Const SHORT_DESC_STRING = "_SHORT_DESC"
Public Const LONG_DESC_STRING = "_LONG_DESC"
Public Const UNITS_DESC_STRING = "_UNITS"
Public Const ENUM_DESC_STRING = "_ENUM_"
Public Const BIT_DESC_STRING = "_BIT_"
Public Const SHORT_NAME_STRING = "short name"
Public Const LONG_NAME_STRING = "long name"
Public Const UNITS_NAME_STRING = "units"
Public Const ENUM_NAME_STRING = "enum"
Public Const BITFIELD_NAME_STRING = "bitfield"
Public Const DCI_DESCRIPTOR_SHEET = "DCI Descriptors"
Public Const LANG_ENGLISH_COL = 4

'***************************************************************
' Update language DCI list.
'***************************************************************
Sub Update_Lang_List()
Dim desc_string As String
Dim ListItems() As String
Dim ParsedItems() As String
Dim TotalEnums As Integer
Dim index As Integer
Dim RawString As String
Dim dcid_found As Boolean
Dim lang_row_counter As Integer
Dim dci_strings_start_row As Integer

counter = DCI_DEFINES_START_ROW
LANG_CHINESE_COL = 5
LANG_FRENCH_COL = 6
LANG_GERMAN_COL = 7
desc_string = ""
Const ENUM_COL = "Z"
Const BITFIELD_COL = "K"
Application.Calculation = xlCalculationManual
Call Language_Column_Size

'Get row number from where DCI strings starts
dci_strings_start_row = LANG_SUPPORT_ROW + 1
While (Worksheets(LANG_TRANSLATION_SHEET).Cells(dci_strings_start_row, LANG_DESC_COL).text <> DCI_STRING_ROW_SPEC)
    dci_strings_start_row = dci_strings_start_row + 1
Wend
dci_strings_start_row = dci_strings_start_row + 1
'*******************************************************************
' Check if DCID present in language list and if not update the list.
'*******************************************************************
    While Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, DCI_DEFINES_COL).text <> Empty
        dcid_found = False
        lang_row_counter = dci_strings_start_row
        While (Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_DESC_COL).text <> Empty)
            If (Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, DCI_DEFINES_COL).text = Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_DESC_COL).text) Then
                 dcid_found = True
            End If
            lang_row_counter = lang_row_counter + 1
        Wend
'*******************************************************************
' update the list.
'*******************************************************************
        If (dcid_found = False) Then
            desc_string = Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, DCI_DEFINES_COL).text
            Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_DESC_COL) = desc_string
            '*******************************************************************
            ' update short description
            '*******************************************************************
            
                'update extension column
                Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_EXTEN_COL) = SHORT_DESC_STRING
                'update type column
                Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_TYPE_COL) = SHORT_NAME_STRING
                'update English column
                index = Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, DCI_SHORT_DESC_LEN_COL).text
                If (index > 0) Then
                    desc_string = Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, DCI_SHORT_DESC_COL).text
                    Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL).Interior.ColorIndex = 0
                    Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL) = desc_string
                Else
                    Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_ATTENTION_COLOR
                End If
        
                lang_row_counter = lang_row_counter + 1
                '*******************************************************************
                ' update long description
                '*******************************************************************
                
                desc_string = Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, DCI_DEFINES_COL).text
                Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_DESC_COL) = desc_string
                'update extension column
                Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_EXTEN_COL) = LONG_DESC_STRING
                'update type column
                Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_TYPE_COL) = LONG_NAME_STRING
                'update English column
                index = Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, DCI_LONG_DESC_LEN_COL).text
                If (index > 0) Then
                    desc_string = Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, DCI_LONG_DESC_COL).text
                    Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL).Interior.ColorIndex = 0
                    Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL) = desc_string
                Else
                    Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_ATTENTION_COLOR
                End If
        
                lang_row_counter = lang_row_counter + 1
                '*******************************************************************
                '  update units
                '*******************************************************************
                
                index = Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, DCI_UNITS_DESC_LEN_COL).text
                If (index > 0) Then
                    desc_string = Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, DCI_DEFINES_COL).text
                    Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_DESC_COL) = desc_string
                    'update extension column
                    Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_EXTEN_COL) = UNITS_DESC_STRING
                    'update type column
                    Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_TYPE_COL) = UNITS_NAME_STRING
                    'update English column
                    
                    desc_string = Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, DCI_UNITS_DESC_COL).text
                    Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL) = desc_string
                    lang_row_counter = lang_row_counter + 1
                End If
                
                '*******************************************************************
                '  update enum
                '*******************************************************************
                
                If (Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, ENUM_ATTRIB_COL).text <> Empty) Then
                    desc_string = Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, DCI_DEFINES_COL).text
                    RawString = Trim(Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, ENUM_COL))
                    ListItems = Split(RawString, ITEM_DELIMITER)
                    'First item denotes the number of bitfields for this parameter
                    TotalEnums = CInt(ListItems(0))
                    For index = 1 To UBound(ListItems) Step 1
                    If "" <> Trim(ListItems(index)) Then
                        ParsedItems = Split(ListItems(index), SUB_ITEM_DELIMITER)
                        'Remove line feeds
                        ParsedItems(0) = Replace(ParsedItems(0), vbLf, "")
                        ParsedItems(1) = Replace(ParsedItems(1), vbLf, "")
                        ParsedItems(2) = Replace(ParsedItems(2), vbLf, "")
                        Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_DESC_COL) = desc_string
                        'update extension column
                        Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_EXTEN_COL) = ENUM_DESC_STRING & ParsedItems(0)
                        'update type column
                        Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_TYPE_COL) = ENUM_NAME_STRING
                        'update English column
                        If (ParsedItems(2) <> Empty) Then
                            Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL).Interior.ColorIndex = 0
                            Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL) = ParsedItems(2)
                        Else
                            Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_ATTENTION_COLOR
                        End If
                        lang_row_counter = lang_row_counter + 1
                    End If
                    Next index
                End If
                '*******************************************************************
                '  update bitfield
                '*******************************************************************
                
                If (Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, DATA_BITFIELD_COL).text <> Empty) Then
                    desc_string = Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, DCI_DEFINES_COL).text
                    RawString = Trim(Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, BITFIELD_COL))
                    ListItems = Split(RawString, ITEM_DELIMITER)
                    
                    For index = 1 To UBound(ListItems) Step 1
                    If "" <> Trim(ListItems(index)) Then
                        
                        'Get total number of parameters e.g TotalParam is 3 when bitfield details has index, name, decription.
                        TotalParam = UBound(Split(ListItems(index), SUB_ITEM_DELIMITER)) + 1
                        
                        ParsedItems = Split(ListItems(index), SUB_ITEM_DELIMITER)
                        'Remove line feeds
                        ParsedItems(0) = Replace(ParsedItems(0), vbLf, "")
                        ParsedItems(1) = Replace(ParsedItems(1), vbLf, "")
                        If TotalParam = 3 Then
                            ParsedItems(2) = Replace(ParsedItems(2), vbLf, "")
                        End If
                        Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_DESC_COL) = desc_string
                        'update extension column
                        Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_EXTEN_COL) = BIT_DESC_STRING & ParsedItems(0)
                        'update type column
                        Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_TYPE_COL) = BITFIELD_NAME_STRING
                        'update English column
                        If TotalParam = 2 Then
                            If (ParsedItems(1) <> Empty) Then
                                Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL).Interior.ColorIndex = 0
                                Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL) = ParsedItems(1)
                            Else
                                Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_ATTENTION_COLOR
                            End If
                        Else
                            If (ParsedItems(2) <> Empty) Then
                                Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL).Interior.ColorIndex = 0
                                Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL) = ParsedItems(2)
                            Else
                                Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_ATTENTION_COLOR
                            End If
                        End If

                        lang_row_counter = lang_row_counter + 1
                    End If
                    Next index
                End If
            End If
        counter = counter + 1
    Wend
    Call Check_Language_Header_Info

End Sub
Sub Language_Column_Size()
    Columns("A:A").EntireColumn.AutoFit
    Columns("B:B").EntireColumn.AutoFit
    Columns("C:C").EntireColumn.AutoFit
    Columns("D:D").WrapText = True
    Columns("E:E").WrapText = True
    Columns("F:F").WrapText = True
    Columns("G:G").WrapText = True
    Columns("H:H").WrapText = True
    'Making font type and size consistent.
    Worksheets("Lang Translation").Range("A:K").Font.name = "Arial"
    Worksheets("Lang Translation").Range("A:K").Font.Size = 10
End Sub

'*******************************************************************
' find missing dci from dci descriptor sheet
'*******************************************************************
Sub Missing_DCI_Cell_Alert(dci_row_number As Integer)
Dim counter As Integer
Dim inner_counter As Integer
Dim dcid_found As Boolean
Dim LANG_END_COL As Integer
Const DCI_DESCRIPTOR_SHEET = "DCI Descriptors"
counter = dci_row_number              'initialization
dcid_found = False
LANG_END_COL = LANG_DESC_COL
While (Worksheets(LANG_TRANSLATION_SHEET).Cells(dci_row_number, LANG_END_COL).text <> Empty)
    LANG_END_COL = LANG_END_COL + 1
Wend
While ((Worksheets(LANG_TRANSLATION_SHEET).Cells(counter, LANG_DESC_COL).text <> Empty))
    inner_counter = DCI_DEFINES_START_ROW
    While (Worksheets(DCI_DESCRIPTOR_SHEET).Cells(inner_counter, DCI_DEFINES_COL).text <> Empty) And (dcid_found = False)
            If (Worksheets(LANG_TRANSLATION_SHEET).Cells(counter, LANG_DESC_COL).text = Worksheets(DCI_DESCRIPTOR_SHEET).Cells(inner_counter, DCI_DEFINES_COL).text) Then
                dcid_found = True
            End If
            inner_counter = inner_counter + 1
    Wend
        If (dcid_found = False) Then
            Range(Cells(counter, LANG_DESC_COL), Cells(counter, LANG_END_COL - 1)).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
        Else
            Range(Cells(counter, LANG_DESC_COL), Cells(counter, LANG_END_COL - 1)).Interior.ColorIndex = 0
            dcid_found = False
        End If
    counter = counter + 1
Wend

End Sub
'*********************************************************************************************
' Color a row for which header information is missing for Language column filled with content
'*********************************************************************************************
Sub Check_Language_Header_Info()
Dim row_counter As Integer
Dim col_counter As Integer
Dim header_counter As Integer
Dim lang_counter As Integer
HEADER_ROW_START = 8
DESC_ROW_START = 14
HEADER_INFO_COUNT = 2
LANGUAGE_COUNT = 0
ATTRIBUTE_DESC_ROW = 13

lang_counter = LANG_ENGLISH_COL
col_counter = LANG_ENGLISH_COL

While (Worksheets(LANG_TRANSLATION_SHEET).Cells(ATTRIBUTE_DESC_ROW, lang_counter).text <> Empty)
    LANGUAGE_COUNT = LANGUAGE_COUNT + 1
    lang_counter = lang_counter + 1
Wend

lang_counter = 0

While (lang_counter < LANGUAGE_COUNT)
    If ((Worksheets(LANG_TRANSLATION_SHEET).Cells(DESC_ROW_START, col_counter).text <> Empty)) Then
        header_counter = 0
        row_counter = HEADER_ROW_START
        While (header_counter < HEADER_INFO_COUNT)
            If (Worksheets(LANG_TRANSLATION_SHEET).Cells(row_counter, col_counter).text = Empty) Then
                Worksheets(LANG_TRANSLATION_SHEET).Cells(row_counter, col_counter).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_ATTENTION_COLOR
            Else
                Worksheets(LANG_TRANSLATION_SHEET).Cells(row_counter, col_counter).Interior.ColorIndex = 0
            End If
            row_counter = row_counter + 1
            header_counter = header_counter + 1
        Wend
    End If
    col_counter = col_counter + 1
    lang_counter = lang_counter + 1
Wend
End Sub
Sub Verify_Lang_List()
Dim short_dci_description As String
Dim long_dci_description As String
Dim unit_dci_description As String
Dim dci_lang_description As String
Dim dci_row_number As Integer

counter = DCI_DEFINES_START_ROW

'Get row number from where DCI strings starts
lang_row_counter = LANG_SUPPORT_ROW + 1
While (Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_DESC_COL).text <> DCI_STRING_ROW_SPEC)
    lang_row_counter = lang_row_counter + 1
Wend
lang_row_counter = lang_row_counter + 1
dci_row_number = lang_row_counter
Call Missing_DCI_Cell_Alert(dci_row_number)
    While Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, DCI_DEFINES_COL).text <> Empty
        '*******************************************************************
        ' Verify the list.
        '*******************************************************************
          While (Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, DCI_DEFINES_COL).text = Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_DESC_COL).text)
               
              dci_lang_description = Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL).text
              '*******************************************************************
              ' verify short description
              '*******************************************************************
                If (SHORT_DESC_STRING = Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_EXTEN_COL).text) Then
                    short_dci_description = Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, DCI_SHORT_DESC_COL).text
                    If (short_dci_description <> dci_lang_description) Then
                     Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL).Interior.ColorIndex = 3
                    Else
                     Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL).Interior.ColorIndex = 0
                    End If
                End If
                
             ''*******************************************************************
             ' verify long description
             '********************************************************************
                If (LONG_DESC_STRING = Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_EXTEN_COL).text) Then
                    long_dci_description = Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, DCI_LONG_DESC_COL).text
                    If (long_dci_description <> dci_lang_description) Then
                     Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL).Interior.ColorIndex = 3
                    Else
                     Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL).Interior.ColorIndex = 0
                    End If
                End If
                
             ''*******************************************************************
             ' verify unit description
             '********************************************************************
              If (UNITS_DESC_STRING = Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_EXTEN_COL).text) Then
                    unit_dci_description = Worksheets(DCI_DESCRIPTOR_SHEET).Cells(counter, DCI_UNITS_DESC_COL).text
                    If (unit_dci_description <> dci_lang_description) Then
                     Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL).Interior.ColorIndex = 3
                    Else
                     Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_ENGLISH_COL).Interior.ColorIndex = 0
                    End If
              End If

         lang_row_counter = lang_row_counter + 1
         Wend
    counter = counter + 1
    Wend
End Sub

