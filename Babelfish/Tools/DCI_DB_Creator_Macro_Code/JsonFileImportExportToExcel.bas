Attribute VB_Name = "JsonFileImportExportToExcel"
'******************************************************************************************************
' DCI JSON IMPORT / EXPORT MODULE.
'******************************************************************************************************
Option Explicit
Const REVISION_NO = "1.1"
Const DCI_START_ADDR = 8
Const DCI_HEAD_LIST_MAX = 40
Const MAX_DCI_MSGBOX_HOLD_SIZE = 20
Const MAX_DCI_LIST = 1000
Dim lTotRowNum As Long
Public DCI_HEAD_LIST(1 To DCI_HEAD_LIST_MAX) As String
'******************************************************************************************************
' Clear the formats for code define column in DCI descriptor sheet
'******************************************************************************************************
Public Function ColumnFormatClear()
    Dim sRowColAdd As String
    sRowColAdd = sRowColAdd & "B" & DCI_START_ADDR & ":B" & Last_DCI_Row
    Sheets("DCI Descriptors").Range(sRowColAdd).ClearFormats
End Function
'******************************************************************************************************
'   Copy DCI Column Header names to array list
'******************************************************************************************************
Private Function LoadColumnHeaderNameToArrayList()
    DCI_HEAD_LIST(1) = "Description"
    DCI_HEAD_LIST(2) = "Code Define"
    DCI_HEAD_LIST(3) = "Datatype"
    DCI_HEAD_LIST(4) = "Datatype Length"
    DCI_HEAD_LIST(5) = "Array Cnt"
    DCI_HEAD_LIST(6) = "Total Length"
    DCI_HEAD_LIST(7) = "Default(RO Val)"        'Renamed header name
    DCI_HEAD_LIST(8) = "Min"
    DCI_HEAD_LIST(9) = "Max"
    DCI_HEAD_LIST(10) = "Enum"
    DCI_HEAD_LIST(11) = "Bitfield Details"
    DCI_HEAD_LIST(12) = "Enum Details"
    DCI_HEAD_LIST(13) = "Read Only Value"
    DCI_HEAD_LIST(14) = "Init Val Support"
    DCI_HEAD_LIST(15) = "Default Support"
    DCI_HEAD_LIST(16) = "RW Default"
    DCI_HEAD_LIST(17) = "Range Support"
    DCI_HEAD_LIST(18) = "RW Range"
    DCI_HEAD_LIST(19) = "Callback Support"
    DCI_HEAD_LIST(20) = "RW Length"
    DCI_HEAD_LIST(21) = "Enum Support"
    DCI_HEAD_LIST(22) = "Application Data"
    DCI_HEAD_LIST(23) = "Ram Val Write Access"
    DCI_HEAD_LIST(24) = "Init Val Read Access"
    DCI_HEAD_LIST(25) = "Init Val Write Access"
    DCI_HEAD_LIST(26) = "Default Read Access"
    DCI_HEAD_LIST(27) = "Range Access"
    DCI_HEAD_LIST(28) = "Enum Access"
    DCI_HEAD_LIST(29) = "Tags"
    DCI_HEAD_LIST(30) = "Short Name"
    DCI_HEAD_LIST(31) = "Short Name Length"
    DCI_HEAD_LIST(32) = "Long Description"           'Renamed header name
    DCI_HEAD_LIST(33) = "Long Description Length"    'Renamed header name
    DCI_HEAD_LIST(34) = "Units"
    DCI_HEAD_LIST(35) = "Units Length"
    DCI_HEAD_LIST(36) = "Enum Description"
    DCI_HEAD_LIST(37) = "Length"
    DCI_HEAD_LIST(38) = "Format"
    DCI_HEAD_LIST(39) = "Internal Description"
    DCI_HEAD_LIST(40) = "Errors/Warnings/Notes"
End Function
'******************************************************************************************************
'   Return DCI last ROW number
'******************************************************************************************************
Private Function Last_DCI_Row() As Integer
    Last_DCI_Row = Worksheets("DCI Descriptors").Cells(Rows.count, "A").End(xlUp).row
End Function
'******************************************************************************************************
'   Return DCI last Column number
'******************************************************************************************************
Private Function Last_DCI_Column() As Integer
    Last_DCI_Column = Worksheets("DCI Descriptors").Cells(DCI_START_ADDR, Columns.count).End(xlToLeft).column
End Function
'******************************************************************************************************
'  Copy paste the Column format DCI starting row to current row
'******************************************************************************************************
Private Function CopyFormat(lRowCount As Long)
    Dim sRefRowAddress, sCurrentRowAddress As String
    
    'Update the Datatype Length for new ROW D column
    sRefRowAddress = "D" & DCI_DEFINES_START_ROW
    sCurrentRowAddress = "D" & lRowCount
    Sheets("DCI Descriptors").Range(sRefRowAddress).Copy
    Sheets("DCI Descriptors").Paste Destination:=Range(sCurrentRowAddress)
    
    'Update the Total Length for new ROW F column
    sRefRowAddress = "F" & DCI_DEFINES_START_ROW
    sCurrentRowAddress = "F" & lRowCount
    Sheets("DCI Descriptors").Range(sRefRowAddress).Copy
    Sheets("DCI Descriptors").Paste Destination:=Range(sCurrentRowAddress)
           
End Function
'******************************************************************************************************
'  Return the selected folder name for DCI save function with the help of browse option
'******************************************************************************************************
Private Function JsonFolderBrowseOption()

    On Error GoTo err
    Dim fileExplorer As FileDialog
    Set fileExplorer = Application.FileDialog(msoFileDialogFolderPicker)
    Dim folderPath As String

    'To allow or disable to multi select
    fileExplorer.AllowMultiSelect = False

    With fileExplorer
        If .Show = -1 Then 'Any folder is selected
            folderPath = .SelectedItems.item(1)

        Else ' else dialog is cancelled
            MsgBox "You have cancelled the dialogue"
            folderPath = "NONE" ' when cancelled set blank as file path.
        End If
    End With
err:

JsonFolderBrowseOption = folderPath

End Function
'******************************************************************************************************
'  Return the selected file name for DCI Import function with the help of browse option
'******************************************************************************************************
Private Function JsonFileBrowseOption()
    Dim fldr As FileDialog
    Dim sItem As String
    
    Set fldr = Application.FileDialog(msoFileDialogFilePicker)
    With fldr
        .Title = "Select a File"
        .Filters.Add "JSON Files", "*.json?", 1 'Display only JSON files
        .AllowMultiSelect = False
        .InitialFileName = ThisWorkbook.path
    If .Show <> -1 Then GoTo NextCode
        sItem = .SelectedItems(1)
    End With
NextCode:
    Set fldr = Nothing
    JsonFileBrowseOption = sItem
End Function
'******************************************************************************************************
'   Import DCIs JSON file to excel file
'******************************************************************************************************
Public Function DCI_ImportToExcel()
    Dim FSO As New FileSystemObject
    Dim JsonTS As TextStream
    Dim jsonObject As Object, item As Object
    Dim Count_I, Count_J, Count_K, flag, TempCount, DCI_Counter, DCI_SkipCounter As Long
    Dim jsonText, sFilePath, sFileName, aDciList(MAX_DCI_LIST), aSkippedList(MAX_DCI_LIST), aSkippedListData(MAX_DCI_LIST) As String
       
    'Initialize the Column header names
    Call LoadColumnHeaderNameToArrayList
    
    'Browse the Import JSON file
    sFilePath = JsonFileBrowseOption
        
    'Validate the Import JSON file path
    If sFilePath = "" Then
        MsgBox "Please select proper Import JSON file", vbInformation
        Exit Function
    End If
    
    'JSON file opened in read mode
    Set JsonTS = FSO.OpenTextFile(sFilePath, ForReading)
    jsonText = JsonTS.ReadAll
    JsonTS.Close
    
    'JSON file shall parse and return in object format which will help to add excel cells
    Set jsonObject = JsonToExcel(jsonText)
    
    Count_I = Last_DCI_Row + 1
        
    'This block will validate the Revision no between JSON file and JsonFileImportExport module,
    'If Revision no matches,Import functionality will continue or else it will exit.
    If flag = 0 Then
        For Count_J = DCI_START_ADDR To Count_I
            flag = 1
            For Each item In jsonObject
                If item("Description") = "REVISION_NO" Then
                    If CStr(REVISION_NO) = CStr(item("Code Define")) Then
                        flag = 4
                    Else
                        flag = 3
                    End If
                End If
            Next
        Next Count_J
       
        If flag <> 4 Then
           MsgBox "Revision No mismatch ", vbCritical
           Exit Function
        End If
    End If
    
    '1. This block shall find the duplicate DCIs and based on sheet "usage note" cell "B27" configuration,__
    '   If sheet "usage note" cell "B27" is empty(default) then DCI will replace or else DCI will skip.
    '2. During DCI import adjust the column no with the help of TempCount variable,__
    '   Since The "DCI Descriptors" sheet columns are non continuous.
    '3. If there are no DCI duplicates, new DCI will get added at the end of "DCI Descriptors" sheet.
    '4. Capture the imported DCIs list and Replaced or Skipped DCI list based on configuration.
    DCI_Counter = 0
    DCI_SkipCounter = 0
    For Each item In jsonObject
        flag = 0
        If Count_I > DCI_START_ADDR Then
            For Count_J = DCI_START_ADDR To Count_I
                If Sheets("DCI Descriptors").Cells(Count_J, 2) = item("Code Define") Then
                    flag = 2
                    For Count_K = 1 To DCI_HEAD_LIST_MAX
                        TempCount = 0
                        If Count_K >= 12 And Count_K < 23 Then
                            TempCount = 14
                        ElseIf Count_K >= 23 And Count_K < 29 Then
                            TempCount = 30
                        ElseIf Count_K >= 29 And Count_K < DCI_HEAD_LIST_MAX Then
                            TempCount = 49
                        ElseIf Count_K = DCI_HEAD_LIST_MAX Then
                            TempCount = 65
                        End If
                        If Sheets("Usage Notes").Range("B27") = "x" Then
                            If CStr(Sheets("DCI Descriptors").Cells(Count_J, (Count_K + TempCount)).value) <> CStr(item(DCI_HEAD_LIST(Count_K))) Then
                                flag = 3
                            End If
                        ElseIf Sheets("Usage Notes").Range("B27") = "" Then
                            If (Count_K + TempCount) = 4 Or (Count_K + TempCount) = 6 Then
                                If (Count_J + TempCount) = 4 Then
                                    CopyFormat (Count_J)
                                End If
                            Else
                                Sheets("DCI Descriptors").Cells(Count_J, (Count_K + TempCount)).value = item(DCI_HEAD_LIST(Count_K))
                            End If
                                flag = 4
                        End If
                    Next Count_K
                End If
            Next Count_J
        End If
        If item("Description") = "REVISION_NO" Then
            Exit For
        End If
        If flag = 0 Then
            For Count_J = 1 To DCI_HEAD_LIST_MAX
                TempCount = 0
                If Count_J >= 12 And Count_J < 23 Then
                    TempCount = 14
                ElseIf Count_J >= 23 And Count_J < 29 Then
                    TempCount = 30
                ElseIf Count_J >= 29 And Count_J < DCI_HEAD_LIST_MAX Then
                    TempCount = 49
                ElseIf Count_J = DCI_HEAD_LIST_MAX Then
                    TempCount = 65
                End If
                
                If (Count_J + TempCount) = 4 Or (Count_J + TempCount) = 6 Then
                    If (Count_J + TempCount) = 4 Then
                        CopyFormat (Count_I)
                    End If
                Else
                    Sheets("DCI Descriptors").Cells(Count_I, (Count_J + TempCount)).value = item(DCI_HEAD_LIST(Count_J))
                End If
            Next Count_J
            Count_I = Count_I + 1
            DCI_Counter = DCI_Counter + 1
            If DCI_Counter > MAX_DCI_LIST Then
                MsgBox "Array size is crossed the Maximum limit, Kindly increase the array size(aDciList)", vbCritical
                Exit Function
            End If
            aDciList(DCI_Counter) = item(DCI_HEAD_LIST(2))
            aSkippedListData(1) = flag
        ElseIf flag = 2 Or flag = 3 Or flag = 4 Then
            DCI_SkipCounter = DCI_SkipCounter + 1
            If DCI_SkipCounter > MAX_DCI_LIST Then
                MsgBox "Array is crossed the Maximum limit, Kindly increase the array size(aSkippedList,aSkippedListData)", vbCritical
                Exit Function
            End If
            aSkippedList(DCI_SkipCounter) = item(DCI_HEAD_LIST(2))
            aSkippedListData(DCI_SkipCounter) = flag
        End If
    Next
        
    'Print the DCI Import summary
    Dim sPathArray() As String
    Dim sDCI_DATA, sDciCntPrint As String
    
    'Based on configuration, (Replaced or Skipped) DCI list will update the DCI import summary variable,__
    'Incase Replaced or Skipped DCI counts are zero will skip to dsiplay from import summary
    If aSkippedListData(1) = 4 And DCI_SkipCounter <> 0 Then
        sDCI_DATA = "Below DCIs already exists in workbook - Replaced (" & DCI_SkipCounter & ") : "
        sDciCntPrint = "DCIs already exists in workbook - Replaced : " & DCI_SkipCounter
    ElseIf DCI_SkipCounter <> 0 Then
        sDCI_DATA = "Below DCIs already exists in workbook - Skipped (" & DCI_SkipCounter & ") : "
        sDciCntPrint = "DCIs already exists in workbook - Skipped : " & DCI_SkipCounter
    End If
    
    For Count_I = 1 To DCI_SkipCounter
        sDCI_DATA = sDCI_DATA & vbCr & Count_I & " . " & aSkippedList(Count_I)
    Next Count_I
    If DCI_Counter <> 0 Then
        sDCI_DATA = sDCI_DATA & vbCr & "Imported DCI list (" & DCI_Counter & ") : "
        sDciCntPrint = sDciCntPrint & vbCr & "Imported DCI list : " & DCI_Counter
    End If
    
    For Count_I = 1 To DCI_Counter
        sDCI_DATA = sDCI_DATA & vbCr & Count_I & " . " & aDciList(Count_I)
    Next Count_I
    
    'Create import summary text file and update the import summary details
    sPathArray() = Split(sFilePath, "\")
    For Count_I = 0 To UBound(sPathArray)
        sFileName = sPathArray(Count_I)
    Next Count_I
    sFileName = Replace(sFileName, ".json", "")
    
    sFilePath = ThisWorkbook.path & "\" & sFileName & "_ImpSummary.txt"
    Set JsonTS = FSO.OpenTextFile(sFilePath, ForWriting, True, TristateFalse)
    JsonTS.Write (sDCI_DATA)
    JsonTS.Close
             
    'Import summary count and (Skiped or Replaced) count is 20 or less then __
    'Import summary will display in msgbox or else Import summary path will display in msgbox
    If ((DCI_Counter + DCI_SkipCounter) <= MAX_DCI_MSGBOX_HOLD_SIZE) Then
        MsgBox "" & sDCI_DATA, vbInformation, "DCI Import Summary"
    Else
        If vbYes = MsgBox(sDciCntPrint & vbCr & "DCI Import Summary file path :" & vbCr & sFilePath & vbCr & vbCr & "Do you want to open DCI Import Summary  file ?", vbQuestion + vbYesNo, "DCI Import Summary") Then
            VBA.Shell "C:\Windows\notepad.exe " & sFilePath, vbNormalFocus
        End If
    End If
        
End Function

'******************************************************************************************************
' Copy paste the DCI selected rows from DCI descriptor sheet to new sheet for save DCI to JSON file
'******************************************************************************************************
Private Function FilteredByHighlightedRows()
    Dim ACell As Range
    Dim WSNew As Worksheet
    Dim Rng, Rng1 As Range
    Dim ActiveCellInTable As Boolean
    Dim sTotalRowNum, sSectedRowCount, sRowColAdd As String
    Dim rA, cA As Long
    With Application
        .ScreenUpdating = False
        .EnableEvents = False
    End With

    'Delete the sheet ExportDCI if it exists
    On Error Resume Next
    Application.DisplayAlerts = False
    Sheets("ExportDCI").Delete
    Application.DisplayAlerts = True
    On Error GoTo 0

    'Select the DCI header row for Export DCI
    Sheets("DCI Descriptors").Range("B" & DCI_START_ADDR).Interior.Color = RGB(210, 210, 210)
    Set ACell = Sheets("DCI Descriptors").Range("B" & DCI_START_ADDR)

    On Error Resume Next
    ActiveCellInTable = (ACell.ListObject.name <> "")
    On Error GoTo 0
        
    'Set the Filter range
    If ActiveCellInTable = False Then
        rA = CInt(Last_DCI_Row)
        sTotalRowNum = "B" & DCI_START_ADDR & ":B" & rA
        Set Rng = Range(sTotalRowNum)
        lTotRowNum = rA
        Rng.Select
        ACell.Activate
    End If

    'Call the built-in filter option to filter on selected sell
    Application.CommandBars("Cell").FindControl(id:=12233, Recursive:=True).Execute

    ACell.Select
    sRowColAdd = Replace(Cells(Last_DCI_Row, Last_DCI_Column).Address, "$", "")
    sRowColAdd = "A" & DCI_START_ADDR & ":" & sRowColAdd
    
    Sheets("DCI Descriptors").Range(sRowColAdd).SpecialCells(xlCellTypeVisible).Copy
        
    'Add a new worksheet to copy the filter results in
    Set WSNew = Worksheets.Add
    WSNew.name = "ExportDCI"

    With WSNew.Range("A1")
        .PasteSpecial xlPasteColumnWidths
        .PasteSpecial xlPasteValues
        .PasteSpecial xlPasteFormats
        Application.CutCopyMode = False
        .Select
    End With

    'Close AutoFilter
    ACell.AutoFilter

    With Application
        .ScreenUpdating = True
        .EnableEvents = True
    End With
    ColumnFormatClear
End Function

'******************************************************************************************************
' Export DCI from DCI descriptor sheet to JSON file
'******************************************************************************************************
Public Function DCI_ExportToJSON()
    Dim FSO As FileSystemObject, JsonTS As TextStream
    Set FSO = New FileSystemObject
    Dim Rng As Range
    Dim sRowColAdd, sRevNo, sFilePath, sFileName, sJsonData As String
    Dim DCI_Counter, Count_I As Long
    Dim bFullDciExpFlag As Boolean: bFullDciExpFlag = False
    sFileName = "Export DCI"
       
    'Find the selected Export area
    FilteredByHighlightedRows
    
    sRowColAdd = Replace(Sheets("ExportDCI").Cells(Sheets("ExportDCI").Cells(Rows.count, 1).End(xlUp).row, Sheets("ExportDCI").Cells(1, Columns.count).End(xlToLeft).column).Address, "$", "")
    sRowColAdd = "A1:" & sRowColAdd
    Set Rng = Range(sRowColAdd)
    'Export Full file, If there are no DCIs got selected
    If Rng.Rows.count <= 1 Or Rng.Rows.count >= (lTotRowNum - DCI_START_ADDR) Then
        On Error Resume Next
        Application.DisplayAlerts = False
        Sheets("ExportDCI").Delete
        Application.DisplayAlerts = True
        On Error GoTo 0
        If vbYes = MsgBox("There are no DCIs got selected, Do you want to export all DCIs?", vbQuestion + vbYesNo, "MsgBox") Then
            sRowColAdd = Replace(Sheets("DCI Descriptors").Cells(Sheets("DCI Descriptors").Cells(Rows.count, 1).End(xlUp).row, Sheets("DCI Descriptors").Cells(DCI_START_ADDR, Columns.count).End(xlToLeft).column).Address, "$", "")
            sRowColAdd = "A" & DCI_START_ADDR & ":" & sRowColAdd
            Set Rng = Range(sRowColAdd)
            bFullDciExpFlag = True
        Else
            Exit Function
        End If
    End If
    
    'Browse the export file
    sFilePath = JsonFolderBrowseOption
    If sFilePath = "NONE" Then
        Exit Function
    End If
    'Take Excel data and return JSON data with revision no
    sJsonData = ExcelToJson(Rng, REVISION_NO)
    
    'Based on user input, either save the JSON file with default file name or modified file name.
    sFileName = InputBox("Do you want to save default file? Press OK,or else rename the file and then Press OK. Note: Provide the file name without extension", "Save DCI Export JSON file ", sFileName)
    
    If sFileName <> "" Then
        sFilePath = sFilePath & "\" & sFileName & ".json"
        Set JsonTS = FSO.OpenTextFile(sFilePath, ForWriting, True, TristateFalse)
        JsonTS.Write (sJsonData)
        JsonTS.Close
        DCI_Counter = Rng.Rows.count - 1
    Else
        DCI_Counter = 0
        MsgBox "DCI Export function got cancelled ", vbInformation
    End If
    
    'Print the Export summary
    sRowColAdd = "Exported DCI list (" & DCI_Counter & ") : "
    If bFullDciExpFlag = False Then
        For Count_I = 2 To (1 + DCI_Counter)
            sRowColAdd = sRowColAdd & vbCr & (Count_I - 1) & " . " & Sheets("ExportDCI").Cells(Count_I, 2).value
        Next Count_I
    Else
        For Count_I = (DCI_START_ADDR + 1) To (DCI_Counter + DCI_START_ADDR)
            sRowColAdd = sRowColAdd & vbCr & (Count_I - DCI_START_ADDR) & " . " & Sheets("DCI Descriptors").Cells(Count_I, 2).value
        Next Count_I
    End If
    sFilePath = ThisWorkbook.path & "\" & sFileName & "_ExpSummary.txt"
    Set JsonTS = FSO.OpenTextFile(sFilePath, ForWriting, True, TristateFalse)
    JsonTS.Write (sRowColAdd)
    JsonTS.Close
    
    'Export summary and Skip count is 20 or less, Export summary will display in msgbox or else
    'Export summary path will display in msgbox
    If (DCI_Counter <= MAX_DCI_MSGBOX_HOLD_SIZE) Then
        MsgBox "" & sRowColAdd, vbInformation, "DCI Export Summary"
    Else
        If vbYes = MsgBox("Exported DCI : " & DCI_Counter & vbCr & "DCI Export Summary file path :" & vbCr & sFilePath & vbCr & vbCr & "Do you want to open DCI Export Summary  file ?", vbQuestion + vbYesNo, "DCI Export Summary") Then
            VBA.Shell "C:\Windows\notepad.exe " & sFilePath, vbNormalFocus
        End If
    End If
    
    'Delete the ExportDCI sheet, hence the export DCI operation got completed
    On Error Resume Next
    Application.DisplayAlerts = False
    Sheets("ExportDCI").Delete
    Application.DisplayAlerts = True
    On Error GoTo 0
        
End Function
'******************************************************************************************************
'   DCI convertion from Excel file format to JSON file
'******************************************************************************************************
Private Function ExcelToJson(Rng As Range, RevNo As String) As String
    If Rng.Columns.count < DCI_HEAD_LIST_MAX Then
        err.Raise 13, "JsonFileImportExportToExcel", "Type Mismatch(Total Coloum count) : " & Rng.Columns.count
        Exit Function
    End If
    Dim dataLoop, headerLoop, headerStart As Long
    Dim headerRange As Range: Set headerRange = Range(Rng.Rows(1).Address)
    Dim colCount As Long: colCount = headerRange.Columns.count
    Dim sData As String
    
    'Format the excel data to JSON string
    Dim JSON As String: JSON = "["
    For dataLoop = 1 To Rng.Rows.count
        If dataLoop > 1 Then
            If IsEmpty(Rng.Value2(dataLoop, 1)) Then
                Exit For
            End If
            Dim jsonData As String: jsonData = vbCr & "{" & vbCr
            For headerLoop = 1 To colCount
                If IsEmpty(headerRange.Value2(1, headerLoop)) = False Then
                    jsonData = jsonData & "    """ & headerRange.Value2(1, headerLoop) & """" & ":"
                    sData = Rng.Value2(dataLoop, headerLoop)
                    If headerRange.Value2(1, headerLoop) = "Default(RO Val)" And Rng.Value2(dataLoop, headerLoop) <> "" And CountOccurrences(sData, """") = 2 Then
                        sData = Replace(sData, """", "")
                        sData = "\" & """" & sData & "\"""
                        jsonData = jsonData & """" & sData & """"
                    Else
                        jsonData = jsonData & """" & Rng.Value2(dataLoop, headerLoop) & """"
                    End If
                    jsonData = jsonData & "," & vbCr
                End If
            Next headerLoop
            jsonData = Left(jsonData, Len(jsonData) - 2)
            JSON = JSON & jsonData & vbCr & "},"
        End If
    Next
    
    'Add Revision No in JSON string
    JSON = JSON & vbCr & "{" & vbCr
    For headerLoop = 1 To colCount
    If IsEmpty(headerRange.Value2(1, headerLoop)) = False Then
         JSON = JSON & "    """ & headerRange.Value2(1, headerLoop) & """" & ":"
         If headerLoop = 1 Then
            JSON = JSON & """REVISION_NO"""
         ElseIf headerLoop = 2 Then
            JSON = JSON & """" & RevNo & """"
         Else
            JSON = JSON & """"""
        End If
        JSON = JSON & "," & vbCr
    End If
    Next headerLoop
    JSON = Left(JSON, Len(JSON) - 2)
    JSON = JSON & vbCr & "},"
    
    JSON = Left(JSON, Len(JSON) - 1)
    JSON = JSON & vbCr & "]"
    ExcelToJson = JSON
    
End Function
'******************************************************************************************************
'  DCI convertion from JSON file to Excel file format
'******************************************************************************************************
Private Function JsonToExcel(ByVal pString As String) As Object
    Dim Index As Long
    Index = 1
    
    'Remove vbCr, vbLf, and vbTab from pString
    pString = Replace(Replace(Replace(pString, vbCr, ""), vbLf, ""), vbTab, "")

    pSkipSpaces pString, Index
    Select Case Mid$(pString, Index, 1)
    Case "{"
        Set JsonToExcel = ParseObject(pString, Index)
    Case "["
        Set JsonToExcel = ParseArray(pString, Index)
    Case Else
          'Error: Invalid JSON format
          err.Raise 91, "JsonFileImportExportToExcel", DebugPrint(pString, Index, "Object variable or With block variable not set: Expecting '{' or '['")
    End Select
End Function

'******************************************************************************************************
'   DCI Import: Object Parser function
'******************************************************************************************************
Private Function ParseObject(pString As String, ByRef pIndex As Long) As Dictionary
    Dim pKey As String
    Dim pNextChar As String
    Set ParseObject = New Dictionary
    pSkipSpaces pString, pIndex
    
    If Mid$(pString, pIndex, 1) <> "{" Then
       err.Raise 78, "JsonFileImportExportToExcel", DebugPrint(pString, pIndex, "Expecting '{'")
    Else
        pIndex = pIndex + 1

        Do
            pSkipSpaces pString, pIndex
            If Mid$(pString, pIndex, 1) = "}" Then
                pIndex = pIndex + 1
                Exit Function
            ElseIf Mid$(pString, pIndex, 1) = "," Then
                pIndex = pIndex + 1
                pSkipSpaces pString, pIndex
            End If
                        
            pKey = ParseKey(pString, pIndex)
            
            pSkipSpaces pString, pIndex
            pNextChar = Mid$(pString, pIndex, 1)
            
            If pNextChar = "[" Or pNextChar = "{" Then
                Set ParseObject.item(pKey) = ParseValue(pString, pIndex)
            Else
                ParseObject.item(pKey) = ParseValue(pString, pIndex)
            End If
        Loop
    End If
End Function
'******************************************************************************************************
'   DCI Import: Array Parser function
'******************************************************************************************************
Private Function ParseArray(pString As String, ByRef pIndex As Long) As Collection
    Set ParseArray = New Collection
    pSkipSpaces pString, pIndex
    If Mid$(pString, pIndex, 1) <> "[" Then
        err.Raise 78, "JsonFileImportExportToExcel", DebugPrint(pString, pIndex, "Expecting '['")
    Else
        pIndex = pIndex + 1
        Do
            pSkipSpaces pString, pIndex
            If Mid$(pString, pIndex, 1) = "]" Then
                pIndex = pIndex + 1
                Exit Function
            ElseIf Mid$(pString, pIndex, 1) = "," Then
                pIndex = pIndex + 1
                pSkipSpaces pString, pIndex
            End If
            ParseArray.Add ParseValue(pString, pIndex)
        Loop
    End If
End Function
'******************************************************************************************************
'   DCI Import: String Parser function
'******************************************************************************************************
Private Function ParseString(pString As String, ByRef pIndex As Long) As String
    Dim pQuote As String
    Dim pChar As String
    Dim pCode As String
    Dim pBuffer As String
    Dim pBufPos As Long
    Dim pBufferLength As Long

    pSkipSpaces pString, pIndex
    
    'Store opening quote to look for matching closing quote in string
    pQuote = Mid$(pString, pIndex, 1)
    pIndex = pIndex + 1

    Do While pIndex > 0 And pIndex <= Len(pString)
        pChar = Mid$(pString, pIndex, 1)

        Select Case pChar
        Case "\"
            'Escaped parse string, \\, or \/
            pIndex = pIndex + 1
            pChar = Mid$(pString, pIndex, 1)

            Select Case pChar
            Case """", "\", "/", "'"
                BufferAppend pBuffer, pChar, pBufPos, pBufferLength
                pIndex = pIndex + 1
            Case "b"
                BufferAppend pBuffer, vbBack, pBufPos, pBufferLength
                pIndex = pIndex + 1
            Case "f"
                BufferAppend pBuffer, vbFormFeed, pBufPos, pBufferLength
                pIndex = pIndex + 1
            Case "n"
                BufferAppend pBuffer, vbCrLf, pBufPos, pBufferLength
                pIndex = pIndex + 1
            Case "r"
                BufferAppend pBuffer, vbCr, pBufPos, pBufferLength
                pIndex = pIndex + 1
            Case "t"
                BufferAppend pBuffer, vbTab, pBufPos, pBufferLength
                pIndex = pIndex + 1
            Case "u"
                'Parse Unicode character escape(eg. \u00a9 = Copyright)
                pIndex = pIndex + 1
                pCode = Mid$(pString, pIndex, 4)
                BufferAppend pBuffer, VBA.ChrW(VBA.val("&h" + pCode)), pBufPos, pBufferLength
                pIndex = pIndex + 4
            End Select
        Case pQuote
            If pBufPos > 0 Then
                ParseString = VBA.Left$(pBuffer, pBufPos)
            End If
            pIndex = pIndex + 1
            Exit Function
        Case Else
            BufferAppend pBuffer, pChar, pBufPos, pBufferLength
            pIndex = pIndex + 1
        End Select
    Loop
End Function
'******************************************************************************************************
'   DCI Import: Value Parser function
'******************************************************************************************************
Private Function ParseValue(pString As String, ByRef pIndex As Long) As Variant
    pSkipSpaces pString, pIndex
    Select Case Mid$(pString, pIndex, 1)
    Case "{"
        Set ParseValue = ParseObject(pString, pIndex)
    Case "["
        Set ParseValue = ParseArray(pString, pIndex)
    Case """", "'"
        ParseValue = ParseString(pString, pIndex)
    Case Else
        If Mid$(pString, pIndex, 4) = "true" Then
            ParseValue = True
            pIndex = pIndex + 4
        ElseIf Mid$(pString, pIndex, 5) = "false" Then
            ParseValue = False
            pIndex = pIndex + 5
        ElseIf Mid$(pString, pIndex, 4) = "null" Then
            ParseValue = Null
            pIndex = pIndex + 4
        Else
             err.Raise 78, "JsonFileImportExportToExcel", DebugPrint(pString, pIndex, "Expecting 'STRING', 'NUMBER', null, true, false, '{', or '['")
        End If
    End Select
End Function
'******************************************************************************************************
'   DCI Import: Key Parser function
'******************************************************************************************************
Private Function ParseKey(pString As String, ByRef pIndex As Long) As String
    'Parse key with single or double quotes
    If Mid$(pString, pIndex, 1) = """" Or Mid$(pString, pIndex, 1) = "'" Then
        ParseKey = ParseString(pString, pIndex)
    Else
         err.Raise 78, "JsonFileImportExportToExcel", DebugPrint(pString, pIndex, "Expecting '""' or '''")
    End If
    pSkipSpaces pString, pIndex
    If Mid$(pString, pIndex, 1) <> ":" Then
         err.Raise 78, "JsonFileImportExportToExcel", DebugPrint(pString, pIndex, "Expecting ':'")
    Else
        pIndex = pIndex + 1
    End If
    
End Function
'******************************************************************************************************
'   DCI Import: Error debug print
'******************************************************************************************************
Private Function DebugPrint(pString As String, ByRef pIndex As Long, ErrorMessage As String)
   
    'Provide detailed Debug message, including details of where and what occurred
    '
    'Example:
    'Debug Message:
    '{"abcde":True}
    '          ^
    'Expecting 'STRING', 'NUMBER', null, true, false, '{', or '['
    Dim pStartIndex As Long
    Dim pStopIndex As Long

   'if possible Include 10 characters before and after error
    pStartIndex = pIndex - 10
    pStopIndex = pIndex + 10
    If pStartIndex <= 0 Then
        pStartIndex = 1
    End If
    If pStopIndex > Len(pString) Then
        pStopIndex = Len(pString)
    End If

    DebugPrint = "Debug Message:" & VBA.vbNewLine & _
                             Mid$(pString, pStartIndex, pStopIndex - pStartIndex + 1) & vbNewLine & _
                             Space$(pIndex - pStartIndex) & "^" & vbNewLine & _
                             ErrorMessage
End Function
'******************************************************************************************************
'   DCI Import: String append
'******************************************************************************************************
Private Sub BufferAppend(ByRef pBuffer As String, ByRef pAppend As Variant, ByRef pBufPos As Long, ByRef pBufferLength As Long)
    'VBA can be slow to append strings due to allocating a new string for each append
    'Instead of using the traditional append, allocate a large empty string and then copy string at append position
    Dim pAppendLength As Long
    Dim pLengthPlusPosition As Long

    pAppendLength = VBA.Len(pAppend)
    pLengthPlusPosition = pAppendLength + pBufPos

    If pLengthPlusPosition > pBufferLength Then
        'Appending would overflow buffer, add chunk(double buffer length or append length,whichever is bigger)
        Dim pAddedLength As Long
        pAddedLength = IIf(pAppendLength > pBufferLength, pAppendLength, pBufferLength)

        pBuffer = pBuffer & VBA.Space$(pAddedLength)
        pBufferLength = pBufferLength + pAddedLength
    End If

    Mid$(pBuffer, pBufPos + 1, pAppendLength) = CStr(pAppend)
    pBufPos = pBufPos + pAppendLength
End Sub
'******************************************************************************************************
'   DCI Import: Return string occurrences count
'******************************************************************************************************
Private Function CountOccurrences(myTxt As String, countTxt As String) As Long
    CountOccurrences = (Len(myTxt) - Len(Replace(myTxt, countTxt, ""))) / Len(countTxt)
End Function
'******************************************************************************************************
'   DCI Import: White Space Parser function
'******************************************************************************************************
Private Sub pSkipSpaces(pString As String, ByRef pIndex As Long)
    Do While pIndex > 0 And pIndex <= VBA.Len(pString) And Mid$(pString, pIndex, 1) = " "
        pIndex = pIndex + 1
    Loop
End Sub
'******************************************************************************************************
' End of DCI JSON IMPORT/EXPORT MODULE.
'******************************************************************************************************





