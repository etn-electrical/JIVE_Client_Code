Attribute VB_Name = "HelperFuncs"
'****************************************************************************************************************************
'******     Helps find and create the namespace for all generated files.
'****************************************************************************************************************************
Public Function Get_Gen_Data_Namespace() As String
    ' The namespace associated with the generated data.  This will reference the Usage Notes page.  If usage notes is not filled in with the
    ' namespace then a default one will be assigned (Gen_Data::)
    Dim gen_data_namespace As String
    Dim gen_data_namespace_colons As String

    Get_Gen_Data_Namespace = Worksheets("Usage Notes").Range("A1").Cells(GENERATED_DOCS_DATA_NAMESPACE_ROW, GENERATED_DOCS_DATA_NAMESPACE_COL).text
    If (Get_Gen_Data_Namespace = "") Then
        Get_Gen_Data_Namespace = "Gen_Data"
    End If

End Function


'****************************************************************************************************************************
'******     Will return the correct suffix for a const value.
'****************************************************************************************************************************
Public Function Get_Suffixed_Const(Datatype As String, data_value As String) As String

    Dim return_string As String
    
    data_value = Replace(data_value, " ", "")

    If (IsNumeric(data_value) = True) Or (Left(data_value, 2) = "0x") Then
        If (Datatype = "DCI_DTYPE_BOOL") Then
            return_string = "U"
        ElseIf (Datatype = "DCI_DTYPE_UINT8") Or (Datatype = "DCI_DTYPE_BYTE") Then
            return_string = "U"
        ElseIf (Datatype = "DCI_DTYPE_SINT8") Or (Datatype = "DCI_DTYPE_STRING8") Then
            return_string = ""
        ElseIf (Datatype = "DCI_DTYPE_UINT16") Or (Datatype = "DCI_DTYPE_WORD") Then
            return_string = "U"
        ElseIf (Datatype = "DCI_DTYPE_SINT16") Then
            return_string = ""
        ElseIf (Datatype = "DCI_DTYPE_UINT32") Or (Datatype = "DCI_DTYPE_DWORD") Then
            return_string = "U"
        ElseIf (Datatype = "DCI_DTYPE_UINT64") Then
            return_string = "UL"
        ElseIf (Datatype = "DCI_DTYPE_SINT32") Then
            return_string = ""
        ElseIf (Datatype = "DCI_DTYPE_SINT64") Then
            return_string = ""
        ElseIf (Datatype = "DCI_DTYPE_FLOAT") Then
            Dim decimal_string As String
            decimal_string = Replace(data_value, ".", "")
            If (Len(decimal_string) < Len(data_value)) Then
                return_string = "F"
            Else
                return_string = ".0F"
            End If
        ElseIf (Datatype = "DCI_DTYPE_DFLOAT") Then
            return_string = ""
        End If
        return_string = data_value & return_string
    Else
        return_string = data_value
    End If
    
    Get_Suffixed_Const = return_string
End Function

'****************************************************************************************************************************
'******     Will return the correct suffix for a const value.
'****************************************************************************************************************************
Public Function Suffixed_String_Test()

    Dim test_string As String
    Dim result_string As String

    test_string = "{ 1, 2, 3, 4 }"
    result_string = Suffixed_String("DCI_DTYPE_UINT8", test_string)

    test_string = "1, 2, 3, 4"
    result_string = Suffixed_String("DCI_DTYPE_UINT8", test_string)
    
    test_string = "1"
    result_string = Suffixed_String("DCI_DTYPE_UINT8", test_string)
    
    test_string = "{ 1, BIG_VAL, NO_VAL, 2 }"
    result_string = Suffixed_String("DCI_DTYPE_UINT8", test_string)

    test_string = "{ 0x1010 }"
    result_string = Suffixed_String("DCI_DTYPE_UINT8", test_string)

    test_string = "{ 0x1111, 0x2222   }"
    result_string = Suffixed_String("DCI_DTYPE_UINT8", test_string)

    test_string = "{ 1, 1.5, NO_VAL, 2 }"
    result_string = Suffixed_String("DCI_DTYPE_FLOAT", test_string)

    test_string = "{ ""Test"" }"
    result_string = Suffixed_String("DCI_DTYPE_FLOAT", test_string)
    
    test_string = """Test"""
    result_string = Suffixed_String("DCI_DTYPE_FLOAT", test_string)

    test_string = """Test"", ""Test2"""
    result_string = Suffixed_String("DCI_DTYPE_FLOAT", test_string)

End Function


'****************************************************************************************************************************
'******     Will return the correct suffix for a const value.
'****************************************************************************************************************************
Public Function Suffixed_String(Datatype As String, data_value As String) As String

    Dim measure_string As String
    Dim work_string As String
    Dim parse_string() As String
    Dim return_string As String
    Dim has_brackets As Boolean
    Dim has_commas As Boolean
    Dim has_quotes As Boolean
    Dim comma_string As String
    
    has_brackets = False
    has_commas = False
    has_quotes = False
        
    ' First check for valid datatype and enum value
    If ((Len(data_value) > 0) And (Len(datatype) > 0)) Then
          
        measure_string = Replace(data_value, ",", "")
        If (Len(measure_string) < Len(data_value)) Then
            has_commas = True
        End If
        
        measure_string = Replace(data_value, "{", "")
        measure_string = Replace(data_value, "}", "")
        If (Len(measure_string) < Len(data_value)) Then
            has_brackets = True
        End If
        
        measure_string = Replace(data_value, """", "")
        If (Len(measure_string) < Len(data_value)) Then
            has_quotes = True
        End If
            
        work_string = Replace(data_value, "{", "")
        work_string = Replace(work_string, "}", "")
        
        If (has_quotes = False) Then
            work_string = Replace(work_string, " ", "")
        End If
        
        parse_string() = Split(work_string, ",")

        If (has_brackets = True) Or (has_commas = True) Then
            comma_string = ""
            return_string = "{ "
            
            For cnt = 0 To UBound(parse_string)
                If (IsNumeric(parse_string(cnt)) = True) Or (Left(parse_string(cnt), 2) = "0x") Then
                return_string = return_string & comma_string & Get_Suffixed_Const(Datatype, parse_string(cnt))
                Else
                    return_string = return_string & comma_string & parse_string(cnt)
                End If
                comma_string = ", "
            Next
            return_string = return_string & " }"
        Else
            If (IsNumeric(parse_string(0)) = True) Or (Left(parse_string(0), 2) = "0x") Then
            return_string = Get_Suffixed_Const(Datatype, parse_string(0))
            ElseIf (has_quotes = True) Then
                return_string = "{ " & parse_string(0) & " }"
            Else
                return_string = parse_string(0)
            End If
        End If
    
    End If
    
    Suffixed_String = return_string

End Function

'****************************************************************************************************************************
'******     Will return the current year.
'****************************************************************************************************************************
Public Function Get_Year() As String
    Get_Year = Year(Date)
End Function


'****************************************************************************************************************************
'******     Will return the correct suffix for a const value.
'****************************************************************************************************************************
Public Function Last_DCI_Row() As Integer

    Last_DCI_Row = Worksheets("DCI Descriptors").Cells(DCI_DEFINES_START_ROW, DCI_DEFINES_COL).End(xlDown).row

End Function

'****************************************************************************************************************************
'******     Will return the correct suffix for a const value.
'****************************************************************************************************************************
Public Function Find_DCID_Cell(dcid_define) As Range
    Set Find_DCID_Cell = Worksheets("DCI Descriptors").Range( _
        Worksheets("DCI Descriptors").Cells(DCI_DEFINES_START_ROW, DCI_DEFINES_COL), _
        Worksheets("DCI Descriptors").Cells(Last_DCI_Row(), DCI_DEFINES_COL)).Find(dcid_define, LookAt:=xlWhole)
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
Public Function Find_DCID_Row(search_string As String) As Integer

    Set DCID_Cell = Find_DCID_Cell(search_string)
    
    If DCID_Cell Is Nothing Then
        Find_DCID_Row = 0
    Else
        Find_DCID_Row = DCID_Cell.row
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
Public Function Find_DCID_Row_Search_Desc_Col(search_string As String) As Integer
   
    dci_defines_row_ctr = DCI_DEFINES_START_ROW

    Set DCID_Cell = Worksheets("DCI Descriptors").Range( _
        Worksheets("DCI Descriptors").Cells(DCI_DEFINES_START_ROW, DCI_DESCRIPTORS_COL), _
        Worksheets("DCI Descriptors").Cells(Last_DCI_Row(), DCI_DESCRIPTORS_COL)).Find(search_string, LookAt:=xlWhole)

    If DCID_Cell Is Nothing Then
        Find_DCID_Row_Search_Desc_Col = 0
    Else
        Find_DCID_Row_Search_Desc_Col = DCID_Cell.row
    End If

End Function

'****************************************************************
'****************************************************************
'*    FUNCTION HEADER
'****************************************************************
'****************************************************************
Sub SaveAll()
    Dim Wkb As Workbook
    
    For Each Wkb In Workbooks
        If Not Wkb.ReadOnly And Windows(Wkb.name).Visible Then
            Wkb.Save
            Wkb
        End If
    Next
End Sub

Function Tokenize(ByVal TokenString, ByRef TokenSeparators())

    Dim NumWords, a()
    NumWords = 0
    
    Dim NumSeps
    NumSeps = UBound(TokenSeparators)
    
    Do
        Dim SepIndex, SepPosition
        SepPosition = 0
        SepIndex = -1
        
        For i = 0 To NumSeps - 1
        
            ' Find location of separator in the string
            Dim pos
            pos = InStr(TokenString, TokenSeparators(i))
            
            ' Is the separator present, and is it closest to the beginning of the string?
            If pos > 0 And ((SepPosition = 0) Or (pos < SepPosition)) Then
                SepPosition = pos
                SepIndex = i
            End If
            
        Next

        ' Did we find any separators?
        If SepIndex < 0 Then

            ' None found - so the token is the remaining string
            ReDim Preserve a(NumWords + 1)
            a(NumWords) = TokenString
            
        Else

            ' Found a token - pull out the substring
            Dim substr
            substr = Trim(Left(TokenString, SepPosition - 1))
    
            ' Add the token to the list
            ReDim Preserve a(NumWords + 1)
            a(NumWords) = substr
        
            ' Cutoff the token we just found
            Dim TrimPosition
            TrimPosition = SepPosition + Len(TokenSeparators(SepIndex))
            TokenString = Trim(Mid(TokenString, TrimPosition))
                        
        End If
        
        NumWords = NumWords + 1
    Loop While (SepIndex >= 0)
    
    Tokenize = a
    
End Function
