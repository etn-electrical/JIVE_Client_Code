Attribute VB_Name = "Export_Sheets"
Public Sub Export_Worksheets()

    Application.Calculation = xlCalculationManual
'    Application.ScreenUpdating = False

    dest_path = ActiveWorkbook.path
    
    Sheets("DCI Descriptors").Select
    Set fs = CreateObject("Scripting.FileSystemObject")
    dest_file_name = (ActiveWorkbook.path & "\" & Left(ActiveWorkbook.name, Len(ActiveWorkbook.name) - 5) & ".txt")
    Set EXPORT_FILE = fs.CreateTextFile(dest_file_name, True)
    
    EXPORT_FILE.WriteLine ("*********************************************************************************")
    EXPORT_FILE.WriteLine ("*********************************************************************************")
    EXPORT_FILE.WriteLine ("*  This is a csv file generated from all sheets inside of the DCI Gen File")
    EXPORT_FILE.WriteLine ("*  It can be used for comparison or backup of spreadsheets")
    EXPORT_FILE.WriteLine ("*********************************************************************************")
    EXPORT_FILE.WriteLine ("*********************************************************************************")
    
    Call ProgressIndicator.Progress_Bar_Load("Exporting")
    Application.DisplayAlerts = False
    
    Dim WS As Excel.Worksheet
    Dim hidden_sheet As Boolean

    total_sheets = 0
    For Each WS In ThisWorkbook.Worksheets
        total_sheets = total_sheets + 1
    Next

    sheet_counter = 0
    For Each WS In ThisWorkbook.Worksheets
        sheet_counter = sheet_counter + 1
    
        Call ProgressIndicator.Progress_Update((sheet_counter / total_sheets) * 100, "Exporting Sheet " & WS.name)

        EXPORT_FILE.WriteLine ("")
        EXPORT_FILE.WriteLine ("")
        EXPORT_FILE.WriteLine ("*********************************************************************************")
        EXPORT_FILE.WriteLine ("*    BEGIN """ & WS.name & """")
        EXPORT_FILE.WriteLine ("VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV")
        EXPORT_FILE.WriteLine ("")

        If (Sheets(WS.name).Visible = True) Then
            hidden_sheet = False
        Else
            hidden_sheet = True
            Sheets(WS.name).Visible = True
        End If
        
        Sheets(WS.name).Copy
        ActiveWorkbook.SaveAs filename:=dest_path & "csv_extract.csv", FileFormat:=xlCSV
        ActiveWorkbook.Close savechanges:=False
        ThisWorkbook.Activate
        
        If (hidden_sheet = True) Then
            Sheets(WS.name).Visible = False
        End If

        Dim source_file As Integer
        Dim read_string As String
        
        source_file = FreeFile()
        Open dest_path & "csv_extract.csv" For Input As source_file
        
        Do While Not EOF(source_file)
            Line Input #source_file, read_string
            EXPORT_FILE.WriteLine (read_string)
        Loop
        Close #source_file
        
        With New FileSystemObject
            If .FileExists(dest_path & "csv_extract.csv") Then
                .DeleteFile dest_path & "csv_extract.csv"
            End If
        End With
' Run the DOS Copy command to append the text files.
'        Shell "command.com /c" & "copy c:\dest.txt+c:\source.txt c:\dest.txt /b"
         
        EXPORT_FILE.WriteLine ("")
        EXPORT_FILE.WriteLine ("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^")
        EXPORT_FILE.WriteLine ("*    END " & WS.name)
        EXPORT_FILE.WriteLine ("*********************************************************************************")
    
    Next
    
    EXPORT_FILE.Close

    Application.DisplayAlerts = True
'    Application.ScreenUpdating = True
    Application.Calculation = xlCalculationAutomatic
    Call ProgressIndicator.Progress_Bar_UnLoad
End Sub


Public Sub The_Old_Solution_Export_Worksheets()

    Application.Calculation = xlCalculationManual
    
    Sheets("DCI Descriptors").Select
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = (ActiveWorkbook.path & "\" & Left(ActiveWorkbook.name, Len(ActiveWorkbook.name) - 4) & ".txt")
    Set EXPORT_FILE = fs.CreateTextFile(file_path, True)
    
    EXPORT_FILE.WriteLine ("*********************************************************************************")
    EXPORT_FILE.WriteLine ("*********************************************************************************")
    EXPORT_FILE.WriteLine ("*  This is a csv file generated from all sheets inside of the DCI Gen File")
    EXPORT_FILE.WriteLine ("*  It can be used for comparison or backup of spreadsheets")
    EXPORT_FILE.WriteLine ("*********************************************************************************")
    EXPORT_FILE.WriteLine ("*********************************************************************************")
    
    Dim export_string As String
    Dim temp_string As String
    Dim data_to_append As Boolean

    Const END_COL_NUM = 256
    Const END_ROW_NUM = 10000
    Const SAMPLE_ROW = 8
    Const SAMPLE_COL = 1
    
    Call ProgressIndicator.Progress_Bar_Load("Exporting")
    
    For i = 1 To Sheets.count
    
        Call ProgressIndicator.Progress_Update((i / Sheets.count) * 100, "Exporting Sheet " & Sheets(i).name)
        'Find the end col of the sheet.
        
        col_end = 1
        For j = 1 To END_COL_NUM
            If (Sheets(i).Cells(SAMPLE_ROW, j).text <> Empty) Then
                col_end = j
            End If
        Next j
        
        'Find the end col of the sheet.
        row_end = 1
        For j = 1 To END_ROW_NUM
            If (Sheets(i).Cells(j, SAMPLE_COL).text <> Empty) Then
                row_end = j
            End If
        Next j
        
        EXPORT_FILE.WriteLine ("")
        EXPORT_FILE.WriteLine ("")
        EXPORT_FILE.WriteLine ("*********************************************************************************")
        EXPORT_FILE.WriteLine ("*    BEGIN """ & Sheets(i).name & """")
        EXPORT_FILE.WriteLine ("VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV")
        EXPORT_FILE.WriteLine ("")
        temp_range = "A:IV"
        If Not (ThisWorkbook.Sheets(i).Range(temp_range).Find(what:="*", SearchOrder:=xlByRows, SearchDirection:=xlPrevious) Is Nothing) Then
            row_end = ThisWorkbook.Sheets(i).Range(temp_range).Find(what:="*", SearchOrder:=xlByRows, SearchDirection:=xlPrevious).row
        Else
            row_end = 1
        End If
        
        If Not (ThisWorkbook.Sheets(i).Range(temp_range).Find(what:="*", SearchOrder:=xlByColumns, SearchDirection:=xlPrevious) Is Nothing) Then
            col_end = ThisWorkbook.Sheets(i).Range(temp_range).Find(what:="*", SearchOrder:=xlByColumns, SearchDirection:=xlPrevious).column
        Else
            col_end = 1
        End If
        
        formula_col_end = 1
        If Not (ThisWorkbook.Sheets(i).Range(temp_range).Find(what:="*", LookIn:=xlValues, SearchOrder:=xlByColumns, SearchDirection:=xlPrevious) Is Nothing) Then
            formula_col_end = ThisWorkbook.Sheets(i).Range(temp_range).Find(what:="*", LookIn:=xlValues, SearchOrder:=xlByColumns, SearchDirection:=xlPrevious).column
        Else
            formula_col_end = 1
        End If
        If formula_col_end > col_end Then
            col_end = formula_col_end
        End If
        
        row_counter = 1
        While (row_counter <= row_end)
         
            col_counter = 1
            temp_string = ""
            export_string = ""
            While (col_counter <= col_end)
                If Sheets(i).Cells(row_counter, col_counter).text <> Empty Then
                    export_string = export_string & temp_string & _
                                    Replace(Sheets(i).Cells(row_counter, col_counter).text, ",", "{{COMMA}}") & ","
                    temp_string = ""
                Else
                    temp_string = temp_string & ","
                End If
                col_counter = col_counter + 1
            Wend
            row_counter = row_counter + 1
            If export_string <> "" Then
                EXPORT_FILE.WriteLine (export_string)
            End If
        Wend

        EXPORT_FILE.WriteLine ("")
        EXPORT_FILE.WriteLine ("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^")
        EXPORT_FILE.WriteLine ("*    END " & Sheets(i).name)
        EXPORT_FILE.WriteLine ("*********************************************************************************")
    
    Next i
    
    EXPORT_FILE.Close

    Application.Calculation = xlCalculationAutomatic
    Call ProgressIndicator.Progress_Bar_UnLoad
End Sub



Function CommonLeft(s1, s2)
Dim l, l1, l2
l = Len(s1)
CommonLeft = s1

l2 = Len(s2)
If l2 > l Then
    l = l2
    CommonLeft = s2
End If

For i = 1 To l
    If Mid(s1, i, 1) <> Mid(s2, i, 1) Then
        CommonLeft = Left(s1, i - 1)
        Exit Function
    End If
Next i

End Function


