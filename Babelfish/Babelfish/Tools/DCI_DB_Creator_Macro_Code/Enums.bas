Attribute VB_Name = "Enums"
Const DCI_DESCRIPTORS_SHEET = "DCI Descriptors"
Const ENUM_CODEGEN_FILENAME = "DCI_Enums"
Const CODE_GEN_FOLDER_PATH_COL = "B"
Const CODE_GEN_FOLDER_PATH_ROW = 1
Const PND_IFNDEF = "#ifndef __"
Const PND_DEFINE = "#define __"
Const PND_ENDIF = "#endif"
Const H_FILE_EXTN = ".h"
Const DCI_DESCRIPTORS_START_ROW = 9
Const ENUMS_COL = "Z"
Const DCI_CODE_DEFINE_COLUMN = "B"
Const DATATYPE_COL = "C"
Public Const ITEM_DELIMITER = ";"
Public Const SUB_ITEM_DELIMITER = ","
Public Const SCALE_START_DELIMITER = "("
Public Const SCALE_END_DELIMITER = ")"

Const DCI_DTYPE_BOOL = "DCI_DTYPE_BOOL"
Const DCI_DTYPE_UINT8 = "DCI_DTYPE_UINT8"
Const DCI_DTYPE_SINT8 = "DCI_DTYPE_BYTE"
Const DCI_DTYPE_UINT16 = "DCI_DTYPE_UINT16"
Const DCI_DTYPE_SINT16 = "DCI_DTYPE_SINT16"
Const DCI_DTYPE_UINT32 = "DCI_DTYPE_UINT32"
Const DCI_DTYPE_UINT64 = "DCI_DTYPE_UINT64"
Const DCI_DTYPE_SINT32 = "DCI_DTYPE_SINT32"
Const DCI_DTYPE_SINT64 = "DCI_DTYPE_SINT64"
Const DCI_DTYPE_FLOAT = "DCI_DTYPE_FLOAT"
Const DCI_DTYPE_DFLOAT = "DCI_DTYPE_DFLOAT"

Const DCI_ENUM_BOOL = "dci_enum_bool_t"
Const DCI_ENUM_UINT8 = "dci_enum_uint8_t"
Const DCI_ENUM_SINT8 = "dci_enum_int8_t"
Const DCI_ENUM_UINT16 = "dci_enum_uint16_t"
Const DCI_ENUM_SINT16 = "dci_enum_int16_t"
Const DCI_ENUM_UINT32 = "dci_enum_uint32_t"
Const DCI_ENUM_UINT64 = "dci_enum_uint64_t"
Const DCI_ENUM_SINT32 = "dci_enum_int32_t"
Const DCI_ENUM_SINT64 = "dci_enum_int64_t"
Const DCI_ENUM_FLOAT = "dci_enum_float32_t"
Const DCI_ENUM_DFLOAT = "dci_enum_float64_t"

Const TYPE_BOOL = "BF_Lib::bool_t"
Const TYPE_UINT8 = "uint8_t"
Const TYPE_SINT8 = "int8_t"
Const TYPE_UINT16 = "uint16_t"
Const TYPE_SINT16 = "int16_t"
Const TYPE_UINT32 = "uint32_t"
Const TYPE_UINT64 = "uint64_t"
Const TYPE_SINT32 = "int32_t"
Const TYPE_SINT64 = "int64_t"
Const TYPE_FLOAT = "float32_t"
Const TYPE_DFLOAT = "float64_t"

Sub EnumFormShow()
    EnumForm.Show
End Sub

Public Sub Gen_DCI_Enum_Code()
Dim fs As FileSystemObject
Dim datatype As String
Dim currentrow As Integer
Dim current_dci As String

base_file_path = ActiveWorkbook.Path & "\" & Worksheets(DCI_DESCRIPTORS_SHEET).Cells(CODE_GEN_FOLDER_PATH_ROW, CODE_GEN_FOLDER_PATH_COL).text
enum_codegen_file = base_file_path & "\" & ENUM_CODEGEN_FILENAME & H_FILE_EXTN
Set fs = New FileSystemObject
Set enum_file = fs.CreateTextFile(enum_codegen_file, True)
enum_file.WriteLine (PND_IFNDEF & UCase(ENUM_CODEGEN_FILENAME))
enum_file.WriteLine (PND_DEFINE & UCase(ENUM_CODEGEN_FILENAME))

enum_file.WriteLine ("")
enum_file.WriteLine ("#include ""Includes.h""")
enum_file.WriteLine ("")

'Generate all the typedefs
enum_file.WriteLine ("typedef " & TYPE_BOOL & " " & DCI_ENUM_BOOL & ";")
enum_file.WriteLine ("typedef " & TYPE_UINT8 & " " & DCI_ENUM_UINT8 & ";")
enum_file.WriteLine ("typedef " & TYPE_SINT8 & " " & DCI_ENUM_SINT8 & ";")
enum_file.WriteLine ("typedef " & TYPE_UINT16 & " " & DCI_ENUM_UINT16 & ";")
enum_file.WriteLine ("typedef " & TYPE_SINT16 & " " & DCI_ENUM_SINT16 & ";")
enum_file.WriteLine ("typedef " & TYPE_UINT32 & " " & DCI_ENUM_UINT32 & ";")
enum_file.WriteLine ("typedef " & TYPE_UINT64 & " " & DCI_ENUM_UINT64 & ";")
enum_file.WriteLine ("typedef " & TYPE_SINT32 & " " & DCI_ENUM_SINT32 & ";")
enum_file.WriteLine ("typedef " & TYPE_SINT64 & " " & DCI_ENUM_SINT64 & ";")
enum_file.WriteLine ("typedef " & TYPE_FLOAT & " " & DCI_ENUM_FLOAT & ";")
enum_file.WriteLine ("typedef " & TYPE_DFLOAT & " " & DCI_ENUM_DFLOAT & ";")
enum_file.WriteLine ("")

currentrow = DCI_DESCRIPTORS_START_ROW

Do While True
    If ("" = Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(currentrow, DCI_CODE_DEFINE_COLUMN))) Then
        Exit Do
    End If
    If ("" <> Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(currentrow, ENUMS_COL))) And _
        ("0;" <> Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(currentrow, ENUMS_COL))) Then
        current_dci = Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(currentrow, DCI_CODE_DEFINE_COLUMN))
        enum_file.WriteLine (vbLf & vbTab & "//Enums for DCID " & current_dci)
        'Determine the datatype
        datatype = Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(currentrow, DATATYPE_COL))
        If (DCI_DTYPE_BOOL = datatype) Then
            datatype = DCI_ENUM_BOOL
        ElseIf (DCI_DTYPE_UINT8 = datatype) Then
            datatype = DCI_ENUM_UINT8
        ElseIf (DCI_DTYPE_SINT8 = datatype) Then
            datatype = DCI_ENUM_SINT8
        ElseIf (DCI_DTYPE_UINT16 = datatype) Then
            datatype = DCI_ENUM_UINT16
        ElseIf (DCI_DTYPE_SINT16 = datatype) Then
            datatype = DCI_ENUM_SINT16
        ElseIf (DCI_DTYPE_UINT32 = datatype) Then
            datatype = DCI_ENUM_UINT32
        ElseIf (DCI_DTYPE_UINT64 = datatype) Then
            datatype = DCI_ENUM_UINT64
        ElseIf (DCI_DTYPE_SINT32 = datatype) Then
            datatype = DCI_ENUM_SINT32
        ElseIf (DCI_DTYPE_SINT64 = datatype) Then
            datatype = DCI_ENUM_SINT64
        ElseIf (DCI_DTYPE_FLOAT = datatype) Then
            datatype = DCI_ENUM_FLOAT
        ElseIf (DCI_DTYPE_DFLOAT = datatype) Then
            datatype = DCI_ENUM_DFLOAT
        Else
            MsgBox ("Invalid datatype " & datatype & " for Enum DCI at row " & currentrow)
            Exit Sub
        End If
        Call Generate_Enums(Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(currentrow, ENUMS_COL)), _
                    current_dci, datatype, enum_file)
    End If
    
    currentrow = currentrow + 1
Loop

enum_file.WriteLine (PND_ENDIF)
enum_file.Close
End Sub

Private Function Generate_Enums(ByVal RawString As String, ByVal DCID As String, ByVal datatype As String, ByVal file As TextStream)
    Dim index As Integer
    Dim ListItems() As String
    Dim ParsedItems() As String
    Dim type_suffix As String
    Dim TotalEnums As Integer
    Dim max_enum_value As Double
    Dim first_itr As Boolean
    first_itr = True
    Dim MaxItem As String
    Dim unsigned_int As Boolean
    
    file.WriteLine (vbTab & "class " & DCID & "_ENUM {")
    file.WriteLine (vbTab & vbTab & "public: ")
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
                           
            If (datatype = DCI_ENUM_UINT8 Or _
                datatype = DCI_ENUM_UINT16 Or _
                datatype = DCI_ENUM_UINT32 Or _
                datatype = DCI_ENUM_UINT64) Then
                type_suffix = "U;"
                unsigned_int = True
            ElseIf (datatype = DCI_ENUM_FLOAT) Then
                'If datatype is float then suffix should be f
                type_suffix = "f;"
            Else
                type_suffix = ";"
            End If
            If (first_itr = True) Then
                max_enum_value = CDbl(ParsedItems(0))
                first_itr = False
            Else
                If (CDbl(ParsedItems(0)) > max_enum_value) Then
                    max_enum_value = CDbl(ParsedItems(0))
                    MaxItem = ParsedItems(0)
                End If
            End If
            'If data type is float use "static constexpr"
            If (datatype = DCI_ENUM_FLOAT) Then
                'If the enum entered by user doesn't have a decimal point then just suffix it with .0
                If ((InStr(ParsedItems(0), ".")) = 0) Then
                    ParsedItems(0) = ParsedItems(0) & ".0"
                End If
                file.WriteLine (vbTab & vbTab & "static constexpr " & datatype & " " & _
                ParsedItems(1) & " = " & ParsedItems(0) & type_suffix & "// " & ParsedItems(2))
            Else
                file.WriteLine (vbTab & vbTab & "static const " & datatype & " " & _
                ParsedItems(1) & " = " & ParsedItems(0) & type_suffix & "// " & ParsedItems(2))
            End If
        End If
    Next index
    If (unsigned_int = True) Then
         file.WriteLine (vbTab & vbTab & "static const " & datatype & " MAX_ENUM " & " = " & (MaxItem) & type_suffix)
    Else
        If (datatype = DCI_ENUM_FLOAT) Then
            'If the max_enum_value doesn't have a decimal point then just suffix it with .0
            'Use "static constexpr " for float constant
            Dim max_enum_value_str As String
            max_enum_value_str = str(max_enum_value)
            If ((InStr(max_enum_value_str, ".")) = 0) Then
                file.WriteLine (vbTab & vbTab & "static constexpr " & datatype & " MAX_ENUM " & " = " & (max_enum_value) & ".0" & type_suffix)
            Else
                file.WriteLine (vbTab & vbTab & "static constexpr " & datatype & " MAX_ENUM " & " = " & (max_enum_value) & type_suffix)
            End If
        Else
            file.WriteLine (vbTab & vbTab & "static const " & datatype & " MAX_ENUM " & " = " & (max_enum_value) & type_suffix)
        End If
        
    End If
    file.WriteLine (vbTab & vbTab & "static const " & TYPE_UINT32 & " TOTAL_ENUMS " & " = " & TotalEnums & "U;")
    file.WriteLine (vbTab & "};")
End Function

