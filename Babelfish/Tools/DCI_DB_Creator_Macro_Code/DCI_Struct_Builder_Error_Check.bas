Attribute VB_Name = "DCI_Struct_Builder_Error_Check"
'***************************************************************
' Do some default and range checked checks.
'***************************************************************
Sub DCI_Struct_Builder_Sheet_Sanity_Check()
   
    Dim Default_Value As Variant
    Dim min_value As Variant
    Dim max_value As Variant
    Dim Enum_Value As String
    Dim data_type As String
    Dim check_error As Boolean
    Dim Bit_range As String
    Dim Range_check As Long
    Dim Arr As Variant
    Dim Total_len As Long
    
    Dim row_error As Boolean
    Dim row_warning As Boolean
    Dim row_attention As Boolean
    Dim error_output As String
    
    Dim counter As Integer
    Dim precision_count As Integer
    Dim error_counter As Integer
    Dim error_bit_field_cell As Integer
    
    counter = DCI_DEFINES_START_ROW         ' Initialization
    error_counter = 0
    error_bit_field_cell = 0
    
    Call Restore_Cell_Colors
    
    nv_missing_counter = NV_MISSING_START_ROW
    
    'Clear the missing definition column
    Worksheets("NVM Address").Range(Worksheets("NVM Address").Cells(NV_MISSING_START_ROW, NV_MISSING_COL), _
    Worksheets("NVM Address").Cells(NV_MEMORY_LAST_POSSIBLE_ADDRESS_ROW, NV_MISSING_COL)).Clear
        
    While ((Cells(counter, DCI_DEFINES_COL).text <> Empty))
    
        error_output = ""
        check_error = False
        
        row_error = False
        row_warning = False
        row_attention = False

		' Check to see if the default value fits in the total length
        If ((Cells(counter, DATA_DEFAULT_COL).text <> Empty) And (Cells(counter, TOTAL_LENGTH_COL).text <> Empty) And (Cells(counter, DCI_DATATYPE_COL).text = "DCI_DTYPE_STRING8") And (InStr(Cells(counter, DATA_DEFAULT_COL).Value2, """"))) Then
			Dim string_len As Long
            Dim string_value As String
            error_val = False
                        
            Total_len = Cells(counter, TOTAL_LENGTH_COL).Value2
            string_value = Cells(counter, DATA_DEFAULT_COL).Value2
            string_value = Replace(string_value, """", "")
            string_len = Len(string_value)
            
            If (string_len >= Total_len) Then
                Cells(counter, DATA_DEFAULT_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                error_output = error_output & "Error: Length of Default value is more than allocated. ; "
            End If
        End If
		
        ' Check to see if the default fits in the range if range is supported.
        If ((Cells(counter, RANGE_ATTRIB_COL).text <> Empty) And (Cells(counter, DEFAULT_VAL_ATTRIB_COL).text <> Empty)) Then
            Dim parse_range_string() As String
            Dim Default_Value_Min As Variant
            Dim Default_Value_Max As Variant
            
            min_value = Cells(counter, DATA_MIN_COL).Value2
            max_value = Cells(counter, DATA_MAX_COL).Value2
            
            Default_Value = Cells(counter, DATA_DEFAULT_COL).value
            Default_Value = Replace(Default_Value, " ", "")
            parse_range_string() = Split(Default_Value, ",")

            If (IsNumeric(min_value) And IsNumeric(max_value) And ((Cells(counter, DCI_DATATYPE_COL).text = "DCI_DTYPE_FLOAT") Or (Cells(counter, DCI_DATATYPE_COL).text = "DCI_DTYPE_DFLOAT"))) Then
            
                Dim precision_format As String
             
                precision_count = val(Worksheets("Usage Notes").Range("A1").Cells(FLOAT_PRECISION_DISPLAY_ROW, FLOAT_PRECISION_DISPLAY_COL).text)
                
                Select Case precision_count
                    Case 1
                        precision_format = "0.0"
                    Case 3
                        precision_format = "0.000"
                    Case 4
                        precision_format = "0.0000"
                    Case 5
                        precision_format = "0.00000"
                    Case 6
                        precision_format = "0.000000"
                    Case Else
                        precision_format = "0.00"
                End Select
                
                'Adds precision of 2 digits to Cell values.
                Cells(counter, DATA_DEFAULT_COL).NumberFormat = precision_format
                Cells(counter, DATA_MIN_COL).NumberFormat = precision_format
                Cells(counter, DATA_MAX_COL).NumberFormat = precision_format
            
                Default_Value_Min = val(parse_range_string(0))
                Default_Value_Max = val(parse_range_string(0))
            
                'Find Min and Max from default range values.
                For cnt = 0 To UBound(parse_range_string)
                    If (val(parse_range_string(cnt)) < Default_Value_Min) Then
                        Default_Value_Min = val(parse_range_string(cnt))
                    ElseIf (val(parse_range_string(cnt)) > Default_Value_Max) Then
                        Default_Value_Max = val(parse_range_string(cnt))
                    End If
                Next
            
                If (Default_Value_Min < min_value) Then
                    Cells(counter, DATA_DEFAULT_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                    check_error = True
                ElseIf (Default_Value_Max > max_value) Then
                    Cells(counter, DATA_DEFAULT_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                    check_error = True
                End If
            ElseIf (IsNumeric(min_value) And IsNumeric(max_value) And (Cells(counter, DCI_DATATYPE_COL).text <> "DCI_DTYPE_BOOL")) Then
            
                'This is done because if datatype is changed from float to other then formating of cell needs to be restored.
                Cells(counter, DATA_DEFAULT_COL).NumberFormat = "0"
                Cells(counter, DATA_MIN_COL).NumberFormat = "0"
                Cells(counter, DATA_MAX_COL).NumberFormat = "0"
            
                Default_Value_Min = CLng(parse_range_string(0))
                Default_Value_Max = CLng(parse_range_string(0))
            
                'Find Min and Max from default range values.
                For cnt = 0 To UBound(parse_range_string)
                    If (CLng(parse_range_string(cnt)) < Default_Value_Min) Then
                        Default_Value_Min = CLng(parse_range_string(cnt))
                    ElseIf (CLng(parse_range_string(cnt)) > Default_Value_Max) Then
                        Default_Value_Max = CLng(parse_range_string(cnt))
                    End If
                Next
            
                If (Default_Value_Min < min_value) Then
                    Cells(counter, DATA_DEFAULT_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                    check_error = True
                ElseIf (Default_Value_Max > max_value) Then
                    Cells(counter, DATA_DEFAULT_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                    check_error = True
                End If
            End If
            If (check_error = True) Then
                error_output = error_output & "Error: Default is out of range. ; "
                check_error = False
                row_error = True
            End If
        End If
        
        ' Check to see if the default value fits in the datatype range
        If ((Cells(counter, DATA_DEFAULT_COL).text <> Empty) And (Cells(counter, DCI_DATATYPE_COL).text <> "DCI_DTYPE_STRING8")) Then
            error_val = False
            If (Cells(counter, DCI_ARRAY_COUNT_COL).text <> Empty) Then
                Dim parse_string() As String
                
                array_value = Cells(counter, DATA_DEFAULT_COL).Value2
                array_value = Replace(array_value, " ", "")
                parse_string() = Split(array_value, ",")
                For cnt = 0 To UBound(parse_string)
                    If (Check_Datatype_Range(Cells(counter, DCI_DATATYPE_COL).text, parse_string(cnt)) = True) Then
                        error_val = True
                    End If
                Next
            Else
                error_val = Check_Datatype_Range(Cells(counter, DCI_DATATYPE_COL).text, Cells(counter, DATA_DEFAULT_COL).Value2)
            End If
            
            If (error_val = True) Then
                Cells(counter, DATA_DEFAULT_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                error_output = error_output & "Error: Default value is out of datatype range. ; "
            End If
        End If

        ' Set any cell which is not X to red.  To indicate that this cell is not reading correctly.
        For iColumn = READ_ONLY_VAL_ATTRIB_COL To PATRON_ENUM_VAL_ACCESS_COL
            If ((Cells(counter, iColumn).text <> "x") And _
                    (Cells(counter, iColumn).text <> "X")) Then
                If (Cells(counter, iColumn).text <> Empty) Then
                    Cells(counter, iColumn).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_WARNING_COLOR
                    row_error = True
                    error_output = error_output & "Warn: Cell is non-empty ; "
                End If
            End If
        Next iColumn

        ' Check for an initial value without a default.  This is for information only.
        If (Cells(counter, INITIAL_VAL_ATTRIB_COL).text <> Empty) Then
            If (Cells(counter, DEFAULT_VAL_ATTRIB_COL).text = Empty) Then
                Cells(counter, DEFAULT_VAL_ATTRIB_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_ATTENTION_COLOR
                error_output = error_output & "Note: Init Val Without Factory Default ; "
            End If
        End If

        ' Check to see if a value has a bitfield or not.  If it does then the datatype should be byte/word/dword.  This is for information only.
        If (Cells(counter, DATA_BITFIELD_COL).text <> Empty) Then
            If (Cells(counter, DCI_DATATYPE_COL).text <> DCI_DESCRIPTORS_DTYPE_BYTE_STRING) And _
                    (Cells(counter, DCI_DATATYPE_COL).text <> DCI_DESCRIPTORS_DTYPE_BOOL_STRING) And _
                    (Cells(counter, DCI_DATATYPE_COL).text <> DCI_DESCRIPTORS_DTYPE_WORD_STRING) And _
                    (Cells(counter, DCI_DATATYPE_COL).text <> DCI_DESCRIPTORS_DTYPE_DWORD_STRING) Then
                Cells(counter, DCI_DATATYPE_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_ATTENTION_COLOR
                Cells(counter, DATA_BITFIELD_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_ATTENTION_COLOR
                error_output = error_output & "Note: DCIDs with Bitfield support must have datatype as BYTE, WORD or DWORD only. ; "
                error_bit_field_cell = counter
                error_counter = error_counter + 1
                Cells(counter, DCI_DATATYPE_COL).Font.Color = RGB(255, 0, 0)
                Cells(counter, DATA_BITFIELD_COL).Font.Color = RGB(255, 0, 0)
            Else
        ' get the bitfield count and check whether it satisfies the total length range
                Total_len = Cells(counter, TOTAL_LENGTH_COL).Value2
                Bit_range = Cells(counter, DATA_BITFIELD_COL).text
                Arr = Split(Bit_range, ";")
                If IsNumeric(Arr(0)) Then
	                Range_check = CInt(Arr(0))
                End If
                If (Range_check > (MIN_DATATYPE_SIZE * Total_len)) Then
                    Cells(counter, DCI_DATATYPE_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                    Cells(counter, DATA_BITFIELD_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                    error_output = error_output & "Note: Total length used is not sufficient to hold the bitfield data.Consider increaing total length"
                    MsgBox "Note: Total length used is not sufficient to hold the bitfield data.Consider increasing total length"
                End If
            End If
        Else
            If (Cells(counter, DCI_DATATYPE_COL).text = DCI_DESCRIPTORS_DTYPE_BYTE_STRING) Or _
                    (Cells(counter, DCI_DATATYPE_COL).text = DCI_DESCRIPTORS_DTYPE_WORD_STRING) Or _
                    (Cells(counter, DCI_DATATYPE_COL).text = DCI_DESCRIPTORS_DTYPE_DWORD_STRING) Then
                Cells(counter, DCI_DATATYPE_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_ATTENTION_COLOR
                error_output = error_output & "Note: This datatype is typically associated with a bitfield. Consider adding definition for the bitfield. ; "
            End If
        End If
        
        ' Check to see if defaults values are populated appropriately.
        If (Cells(counter, DEFAULT_VAL_ATTRIB_COL).text <> Empty) Then
            If (Cells(counter, DATA_DEFAULT_COL).text = Empty) Then
                Cells(counter, DATA_DEFAULT_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                row_error = True
                error_output = error_output & "Fault: Default Value Missing ; "
            End If
        End If
        If (Cells(counter, DATA_DEFAULT_COL).text <> Empty) Then
            If (Cells(counter, DEFAULT_VAL_ATTRIB_COL).text = Empty) And (Cells(counter, READ_ONLY_VAL_ATTRIB_COL).text = Empty) Then
                Cells(counter, DEFAULT_VAL_ATTRIB_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                row_error = True
                error_output = error_output & "Fault: Default Support Unchecked OR Default Value Without Default Support ; "
            End If
        End If

        ' If range is checked then verify that we don't have an empty range value.
        If (Cells(counter, RANGE_ATTRIB_COL).text <> Empty) Then
            If (Cells(counter, DATA_MIN_COL).text = Empty) Then
                Cells(counter, DATA_MIN_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                row_error = True
                error_output = error_output & "Fault: Range Value Missing ; "
            End If
            If (Cells(counter, DATA_MAX_COL).text = "") Then
                Cells(counter, DATA_MAX_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                row_error = True
                error_output = error_output & "Fault: Range Value Missing ; "
            End If
        End If

        ' If range values exist and the cell is checked then error.
        If ((Cells(counter, DATA_MIN_COL).value <> "") Or (Cells(counter, DATA_MAX_COL).value <> "")) Then
            If (Cells(counter, RANGE_ATTRIB_COL).text <> "x") Then
                Cells(counter, RANGE_ATTRIB_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                row_error = True
                error_output = error_output & "Fault: Range Value Missing ; "
            End If
        End If

        ' Check to see if defaults are populated appropriately.
        If (Cells(counter, ENUM_ATTRIB_COL).text <> Empty) Then
            If (Cells(counter, DATA_ENUM_COL).text = Empty) Then
                Cells(counter, DATA_ENUM_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                row_error = True
                error_output = error_output & "Fault: Enum Value Missing ; "
            ' Check if an enum is defined without enum details.
            ElseIf (Cells(counter, DATA_ENUMLIST_COL).text = Empty) Then
                Cells(counter, DATA_ENUMLIST_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                error_output = error_output & "Fault: Enum Details Missing ; "
            End If
        End If
        If ((Cells(counter, DATA_ENUM_COL).text <> Empty) Or (Cells(counter, DATA_ENUMLIST_COL).text <> Empty)) Then
            If (Cells(counter, ENUM_ATTRIB_COL).text = Empty) Then
                If (Cells(counter, DATA_ENUM_COL).text <> Empty) Then
                    Cells(counter, DATA_ENUM_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                End If
                If (Cells(counter, DATA_ENUMLIST_COL).text <> Empty) Then
                    Cells(counter, DATA_ENUMLIST_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                End If
                Cells(counter, ENUM_ATTRIB_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                row_error = True
                error_output = error_output & "Fault: Enum Support Not Available ; "
            End If
        End If
        
        
        'Check to see if we have a patron access checked without a corresponding owner defined.
        If ((Cells(counter, INITIAL_VAL_ATTRIB_COL).text = Empty) And (Cells(counter, PATRON_INIT_VAL_READ_ACCESS_COL).text <> Empty)) Or _
                ((Cells(counter, INITIAL_VAL_ATTRIB_COL).text = Empty) And (Cells(counter, PATRON_INIT_VAL_WRITEABLE_ACCESS_COL).text <> Empty)) Then
            Cells(counter, PATRON_INIT_VAL_READ_ACCESS_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
            check_error = True
        End If
        If (Cells(counter, DEFAULT_VAL_ATTRIB_COL).text = Empty) And (Cells(counter, PATRON_DEFAULT_VAL_READ_ACCESS_COL).text <> Empty) Then
            Cells(counter, PATRON_DEFAULT_VAL_READ_ACCESS_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
            check_error = True
        End If
        If (Cells(counter, RANGE_ATTRIB_COL).text = Empty) And (Cells(counter, PATRON_RANGE_VAL_ACCESS_COL).text <> Empty) Then
            Cells(counter, PATRON_RANGE_VAL_ACCESS_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
            check_error = True
        End If
        If (Cells(counter, ENUM_ATTRIB_COL).text = Empty) And (Cells(counter, PATRON_ENUM_VAL_ACCESS_COL).text <> Empty) Then
            Cells(counter, PATRON_ENUM_VAL_ACCESS_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
            check_error = True
        End If
        If (Cells(counter, READ_ONLY_VAL_ATTRIB_COL).text <> Empty) And (Cells(counter, PATRON_RAM_VAL_WRITEABLE_COL).text <> Empty) Then
            Cells(counter, PATRON_RAM_VAL_WRITEABLE_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
            check_error = True
        End If

        If (check_error = True) Then
            error_output = error_output & "Fault: Patron Checked Attribute Access Checked Without Owner Attribute Support. ; "
            row_error = True
            check_error = False
        End If

        ' Do some checks on owner RW versus support.
        If (Cells(counter, DEFAULT_VAL_ATTRIB_COL).text = Empty) And (Cells(counter, RW_DEFAULT_VAL_ATTRIB_COL).text <> Empty) Then
            Cells(counter, RW_DEFAULT_VAL_ATTRIB_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
            error_output = error_output & "Fault: RW Default without general support. ; "
        End If
        If (Cells(counter, RANGE_ATTRIB_COL).text = Empty) And (Cells(counter, RW_RANGE_ATTRIB_COL).text <> Empty) Then
            Cells(counter, RW_RANGE_ATTRIB_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
            error_output = error_output & "Fault: RW Range without general support. ; "
        End If
        If (Cells(counter, ENUM_ATTRIB_COL).text = Empty) And (Cells(counter, RW_ENUM_ATTRIB_COL).text <> Empty) Then
            Cells(counter, RW_ENUM_ATTRIB_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
            error_output = error_output & "Fault: RW Enum without general support. ; "
        End If

        ' Check to see if we have an RO value with initial or default support.
        If (Cells(counter, READ_ONLY_VAL_ATTRIB_COL).text <> Empty) Then
            If (Cells(counter, INITIAL_VAL_ATTRIB_COL).text <> Empty) Or (Cells(counter, DEFAULT_VAL_ATTRIB_COL).text <> Empty) Or _
                    (Cells(counter, RW_DEFAULT_VAL_ATTRIB_COL).text <> Empty) Or (Cells(counter, RANGE_ATTRIB_COL).text <> Empty) Or _
                    (Cells(counter, RW_RANGE_ATTRIB_COL).text <> Empty) Or (Cells(counter, ENUM_ATTRIB_COL).text <> Empty) Or _
                    (Cells(counter, PATRON_RAM_VAL_WRITEABLE_COL).text <> Empty) Or (Cells(counter, PATRON_INIT_VAL_READ_ACCESS_COL).text <> Empty) Or _
                    (Cells(counter, PATRON_INIT_VAL_WRITEABLE_ACCESS_COL).text <> Empty) Or (Cells(counter, PATRON_DEFAULT_VAL_READ_ACCESS_COL).text <> Empty) Or _
                    (Cells(counter, PATRON_RANGE_VAL_ACCESS_COL).text <> Empty) Or (Cells(counter, PATRON_ENUM_VAL_ACCESS_COL).text <> Empty) Then
                Cells(counter, READ_ONLY_VAL_ATTRIB_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                error_output = error_output & "Fault: Read only with other support. ; "
                row_error = True
            End If
        End If

        ' Throw an attention flag if you have an owner with an option selected and not a corresponding Patron access
        If (Cells(counter, INITIAL_VAL_ATTRIB_COL).text <> Empty) And (Cells(counter, PATRON_INIT_VAL_READ_ACCESS_COL).text = Empty) Then
            Cells(counter, PATRON_INIT_VAL_READ_ACCESS_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_ATTENTION_COLOR
            check_error = True
        End If
        If (Cells(counter, DEFAULT_VAL_ATTRIB_COL).text <> Empty) And (Cells(counter, PATRON_DEFAULT_VAL_READ_ACCESS_COL).text = Empty) Then
            Cells(counter, PATRON_DEFAULT_VAL_READ_ACCESS_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_ATTENTION_COLOR
            check_error = True
        End If
        If (Cells(counter, RANGE_ATTRIB_COL).text <> Empty) And (Cells(counter, PATRON_RANGE_VAL_ACCESS_COL).text = Empty) Then
            Cells(counter, PATRON_RANGE_VAL_ACCESS_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_ATTENTION_COLOR
            check_error = True
        End If
        If (Cells(counter, ENUM_ATTRIB_COL).text <> Empty) And (Cells(counter, PATRON_ENUM_VAL_ACCESS_COL).text = Empty) Then
            Cells(counter, PATRON_ENUM_VAL_ACCESS_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_ATTENTION_COLOR
            check_error = True
        End If
        If (check_error = True) Then
            error_output = error_output & "Note: Owner attributes are checked without a matching patron access. ; "
            check_error = False
        End If
        
'****************************************************************************************************************************
'******     Check the NV addresses - Essentially add the missing values to the NV Sheet.
'****************************************************************************************************************************
        
        lookup_string = Cells(counter, DCI_DEFINES_COL).text
        ' Find the parameter in the NV memory sheet.
        Set C = Worksheets("NVM Address").Range(Worksheets("NVM Address").Cells(NV_MEM_DATA_START_ROW, NV_DEFINES_COL), _
            Worksheets("NVM Address").Cells(NV_MEMORY_LAST_POSSIBLE_ADDRESS_ROW, NV_MEMORY_MAX_COLUMN)).Find(lookup_string, LookAt:=xlWhole)
        If (Cells(counter, INITIAL_VAL_ATTRIB_COL).text <> Empty) Then  'Is there an NV memory location supposed to exist?
            If C Is Nothing Then    'If the value was not found in the NV memory sheet then we need to add it to the missing column
                Worksheets("NVM Address").Range("A1").Cells(nv_missing_counter, NV_MISSING_COL).value = Cells(counter, DCI_DEFINES_COL).text
                Worksheets("NVM Address").Range("A1").Cells(nv_missing_counter, NV_MISSING_COL).Interior.ColorIndex = NV_MEM_DCI_COVERED_PARAM_COLOR
                new_value_size = Cells(counter, TOTAL_LENGTH_COL).value - 1
                nv_missing_counter = nv_missing_counter + 1
                While new_value_size > 0
                    Worksheets("NVM Address").Range("A1").Cells(nv_missing_counter, NV_MISSING_COL).Interior.ColorIndex = NV_MEM_DCI_COVERED_PARAM_COLOR
                    new_value_size = new_value_size - 1
                    nv_missing_counter = nv_missing_counter + 1
                Wend
                Worksheets("NVM Address").Range("A1").Cells(nv_missing_counter, NV_MISSING_COL).value = NV_MEM_CHECKSUM_STRING
                Worksheets("NVM Address").Range("A1").Cells(nv_missing_counter, NV_MISSING_COL).Interior.ColorIndex = NV_MEM_NON_DCI_COVERED_CHECKSUM_COLOR
                nv_missing_counter = nv_missing_counter + 1
                Worksheets("NVM Address").Range("A1").Cells(nv_missing_counter, NV_MISSING_COL).value = ""
                Worksheets("NVM Address").Range("A1").Cells(nv_missing_counter, NV_MISSING_COL).Interior.ColorIndex = NV_MEM_NON_DCI_COVERED_CHECKSUM_COLOR
                nv_missing_counter = nv_missing_counter + 1        '1
                
                Cells(counter, INITIAL_VAL_ATTRIB_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                error_output = error_output & "Fault: Initial value space is needed in the NV Memory sheet. ; "
                row_error = True
            Else    'If it was found - we might as well check the length matches.
                offset_counter = 0
                While ((C.offset(offset_counter, 0).text <> NV_MEM_CHECKSUM_STRING) And (offset_counter < NV_MEMORY_LAST_POSSIBLE_ADDRESS_ROW))
                    C.offset(offset_counter, 0).Interior.ColorIndex = NV_MEM_DCI_COVERED_PARAM_COLOR
                    offset_counter = offset_counter + 1
                Wend
                C.offset(offset_counter, 0).Interior.ColorIndex = NV_MEM_DCI_COVERED_CHECKSUM_COLOR
                C.offset(offset_counter + 1, 0).Interior.ColorIndex = NV_MEM_DCI_COVERED_CHECKSUM_COLOR
                If (offset_counter <> Cells(counter, TOTAL_LENGTH_COL).value) Then  'If the length assigned in NV does not match the actual then error.
                    Cells(counter, INITIAL_VAL_ATTRIB_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
                    error_output = error_output & "Fault: The NV space allocated does not match the parameter size. ; "
                    row_error = True
                    C.offset(offset_counter, 0).Interior.ColorIndex = NV_MEM_DCI_COVERED_CHECKSUM_COLOR
                    C.offset(offset_counter + 1, 0).Interior.ColorIndex = NV_MEM_DCI_COVERED_CHECKSUM_COLOR
                Else    ' The length is correct and we will fix the checksum color.
                    C.offset(offset_counter, 0).Interior.ColorIndex = NV_MEM_DCI_COVERED_CHECKSUM_COLOR
                    C.offset(offset_counter + 1, 0).Interior.ColorIndex = NV_MEM_DCI_COVERED_CHECKSUM_COLOR
                End If
                
            End If
            If nv_max_max_size < Cells(counter, TOTAL_LENGTH_COL).value Then
                nv_max_max_size = Cells(counter, TOTAL_LENGTH_COL).value
                Worksheets("NVM Address").Range("A1").Cells(NV_MEM_SHEET_MAX_VAL_SIZE_ROW, NV_MEM_SHEET_MAX_VAL_SIZE_COL).value = nv_max_max_size
            End If
        Else    ' If we don't have an NV Value we should probably check to see if there is a remnant in the NV Space which needs to be cleaned up or pointed out.
            If Not (C Is Nothing) Then
                Cells(counter, INITIAL_VAL_ATTRIB_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_WARNING_COLOR
                error_output = error_output & "Warning: Unused initial value is present in the NV mem sheet. ; "
                row_warning = True
                offset_counter = 0
                While ((C.offset(offset_counter, 0).text <> NV_MEM_CHECKSUM_STRING) And (offset_counter < NV_MEMORY_LAST_POSSIBLE_ADDRESS_ROW))
                    C.offset(offset_counter, 0).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_NV_MEM_SHEET_DELETE
                    offset_counter = offset_counter + 1
                Wend
                'We catch the checksum with a bad color as well.
                C.offset(offset_counter, 0).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_NV_MEM_SHEET_CHECKSUM_DELETE
                C.offset(offset_counter + 1, 0).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_NV_MEM_SHEET_CHECKSUM_DELETE
            End If
        End If
        
        Cells(counter, DCI_DESCRIPTORS_ERROR_CHECK_OUTPUT_COL).value = error_output

        If (row_error = True) Then
            Cells(counter, DCI_DEFINES_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_FAULT_COLOR
        ElseIf (row_warning = True) Then
            Cells(counter, DCI_DEFINES_COL).Interior.ColorIndex = DCI_DESCRIPTORS_ERROR_CHECK_WARNING_COLOR
        End If

        counter = counter + 1
        
        If (error_counter <> "0") Then
            MsgBox "Error: DCIDs with Bitfield support must have datatype as BYTE, WORD or DWORD only.", vbCritical, "Invalid Datatype"
            Cells(error_bit_field_cell, DATA_BITFIELD_COL).Select
            Exit Sub
        End If
    Wend

End Sub

'***************************************************************
' Fix the colors and erase the errors.
'***************************************************************
Sub Restore_Cell_Colors()
    Dim counter As Integer
    
    counter = DCI_DEFINES_START_ROW                'initialization
     
    While ((Cells(counter, DCI_DEFINES_COL).text <> Empty))
        Rows(counter).Interior.ColorIndex = 0
        Cells(counter, DATATYPE_SIZE).Interior.ColorIndex = DCI_DESCRIPTORS_LENGTH_COL_COLOR
        Cells(counter, TOTAL_LENGTH_COL).Interior.ColorIndex = DCI_DESCRIPTORS_LENGTH_COL_COLOR
        Cells(counter, DCI_DATATYPE_COL).Font.Color = RGB(0, 0, 0)
        Cells(counter, DATA_BITFIELD_COL).Font.Color = RGB(0, 0, 0)
        counter = counter + 1
    Wend

End Sub

'****************************************************************************************************************************
'******     Check if the value is in the datatype range
'****************************************************************************************************************************
Public Function Check_Datatype_Range(datatype As String, data_value) As Boolean
    Dim return_val As Boolean
    Dim min_val As Double
    Dim max_val As Double
    Dim float_error As Boolean
    float_error = False
    
    If (datatype = "DCI_DTYPE_BOOL") Then
        If ((data_value = True) Or (data_value = False) Or (data_value = 1) Or (data_value = 0)) Then
            return_val = False
        Else
            return_val = True
        End If
    ElseIf (IsNumeric(data_value) = True) Or (Left(data_value, 2) = "0x") Then
        If (Left(data_value, 2) = "0x") Then
            'CLng() only support conversion of 16bit hexadecimal number to Decimal.
            'CLngLng() support conversion of 64bit hexadecimal number but supported only in 64Bit system.
            'data_value = CLng("&H" & Replace(data_value, "0x", ""))
            'Hex2Dec convert hexadecimal number to decimal upto 10 characters( 40bit). Supported in both 64bit and 32bit system.
            'Trim() is used to remove any whitespace before passing the value to Hex2Dec()
            data_value = Application.WorksheetFunction.Hex2Dec(Trim(Replace(data_value, "0x", "")))
        ElseIf ((InStr(data_value, ".") <> 0) And ((datatype <> "DCI_DTYPE_FLOAT") Or (datatype <> "DCI_DTYPE_DFLOAT"))) Then
            'Generate error when a non float/double datatype variable is assigned a float/double value.
            float_error = True
        End If
        
        If (float_error <> True) Then
            Select Case datatype
                Case "DCI_DTYPE_UINT8", "DCI_DTYPE_BYTE"
                    min_val = 0
                    max_val = 255
                Case "DCI_DTYPE_SINT8"
                    min_val = -128
                    max_val = 127
                Case "DCI_DTYPE_UINT16"
                    min_val = 0
                    max_val = 65535
                Case "DCI_DTYPE_SINT16", "DCI_DTYPE_WORD"
                    min_val = -32768
                    max_val = 32767
                Case "DCI_DTYPE_UINT32", "DCI_DTYPE_DWORD"
                    min_val = 0
                    max_val = 4294967295#
                Case "DCI_DTYPE_SINT32"
                    min_val = -2147483648#
                    max_val = 2147483647
                Case "DCI_DTYPE_UINT64"
                    min_val = 0
                    max_val = 1.84467440737096E+19
                Case "DCI_DTYPE_SINT64"
                    min_val = -9.22337203685478E+18
                    max_val = 9.22337203685478E+18
            End Select
        End If
        
        If ((datatype <> "DCI_DTYPE_FLOAT") And (datatype <> "DCI_DTYPE_DFLOAT")) Then
            If ((float_error <> True) And ((Int(data_value) >= min_val) And (Int(data_value) <= max_val))) Then
                return_val = False
            Else
                return_val = True
            End If
        End If
    End If
    
    Check_Datatype_Range = return_val
End Function

