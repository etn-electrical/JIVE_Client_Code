VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "ThisWorkbook"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = True
#If VBA7 Then
    Private Declare PtrSafe Sub ExitProcess Lib "kernel32" (ByVal exitCode As Long)
#Else
    Private Declare Sub ExitProcess Lib "kernel32" (ByVal exitCode As Long)
#End If

Private Sub Workbook_Open()

    'Application.EnableCancelKey = xlDisabled  'Turn off the Esc key
    Call Check_compatibility
    
    Dim environString As String
    environString = Environ("MegaMacroRun")
    
    If environString = "True" Then
        Mega_Macro_Run  'Perform the magic!
        ExitProcess (0)  'This acts as a Quit command.  Must be the VERY last command otherwise processes could hang!
    End If
End Sub

Private Sub Workbook_BeforeSave(ByVal SaveAsUI As Boolean, Cancel As Boolean)
    'Exit Sub
    Application.Calculation = xlCalculationManual
    
    On Error GoTo Errorcatch    ' To hopefully catch some badness that occasionally happens with the macro.
    
    If Sheets("Usage Notes").Cells(3, 2).text <> Empty Then
        Call Export_Worksheets
    End If
    
    file_path = ThisWorkbook.path & "\Target DCI DB Creator Macro Code\"
    
    For i = 1 To ThisWorkbook.VBProject.VBComponents.count
        If ThisWorkbook.VBProject.VBComponents.Item(i).name = "ThisWorkbook" Then
            ThisWorkbook.VBProject.VBComponents.Item(i).Export (file_path & _
                        Left(ThisWorkbook.name, Len(ThisWorkbook.name) - 4) & "_Macros_Main.bas")
        ElseIf Left(ThisWorkbook.VBProject.VBComponents.Item(i).name, 5) = "Sheet" Then
            ThisWorkbook.VBProject.VBComponents.Item(i).Export (file_path & _
                        ThisWorkbook.VBProject.VBComponents.Item(i).name & "_Macro.bas")
        Else
            ThisWorkbook.VBProject.VBComponents.Item(i).Export (file_path & _
                        ThisWorkbook.VBProject.VBComponents.Item(i).name & ".bas")
        End If
    Next
    
    Application.Calculation = xlCalculationAutomatic
    
    Exit Sub

Errorcatch:
    Select Case Err.Number
        Case 76, 50035, 50012     ' Make a new folder
            MkDir "Target DCI DB Creator Macro Code"
            Err.Clear
            Resume
        Case Else
            MsgBox Err.Description
    End Select

End Sub

Private Sub Check_compatibility()
    Dim file_name, line, DCI_readme_file, macro_readme_file As String
    Dim return_val, macro_compatibility_num, DCI_compatibility_num As Integer
    Dim log_present, DCI_readme_found, macro_readme_found As Boolean
    log_present = False
    DCI_readme_found = False
    macro_readme_found = False
    return_val = 0
    
    ' Define README file names and their paths
    DCI_readme_file_name = "RTK_DCI_README.md"
    Macro_readme_file_name = "DCI_MACRO_README.md"
    DCI_file_path = ActiveWorkbook.path & "\"
    macro_file_path = ActiveWorkbook.path & Worksheets("DCI Descriptors").Cells(1, 8).text
    DCI_readme_file = DCI_file_path & DCI_readme_file_name
    macro_readme_file = macro_file_path & Macro_readme_file_name
    
    'Open DCID's README file and read the COMPATIBILITY_NUMBER
    If Len(Dir(DCI_readme_file)) = 0 Then
        MsgBox ("RTK_DCI_README.md file does not exist at RTK_Example\Tools folder.")
    Else
        DCI_readme_found = True
        Open DCI_readme_file For Input As #1
        Line Input #1, line
        line_num = InStr(line, "$ COMPATIBILITY_NUMBER")
        While ((line_num = 0) And (Not EOF(1)))
            Line Input #1, line
            line_num = InStr(line, "$ COMPATIBILITY_NUMBER")
        Wend
        If line_num <> 0 Then
            DCI_compatibility_num = CInt(Mid(line, (InStr(line, "=") + 2)))
        End If
        Close #1
    End If
    
    'Open Macro's README file and read the COMPATIBILITY_NUMBER
    If Len(Dir(macro_readme_file)) = 0 Then
        MsgBox ("DCI_MACRO_README.md file does not exist at RTK_Example\Babelfish\Tools folder.")
    Else
        macro_readme_found = True
        Open macro_readme_file For Input As #2
        Line Input #2, line
        line_num = InStr(line, "$ COMPATIBILITY_NUMBER")
        While ((line_num = 0) And (Not EOF(2)))
            Line Input #2, line
            line_num = InStr(line, "$ COMPATIBILITY_NUMBER")
        Wend
        If line_num <> 0 Then
            macro_compatibility_num = Mid(line, (InStr(line, "=") + 2))
        End If
        Close #2
    End If
    
    
    If (macro_readme_found = True) And (DCI_readme_found = True) And _
    (macro_compatibility_num <> DCI_compatibility_num) Then
        If (macro_compatibility_num > DCI_compatibility_num) Then
            log_present = True
        End If
        return_val = MsgBox("The RTK DCI DB Creator workbook is NOT compatible with the DCI DB Creator Macro workbook. " & Chr(10) & Chr(10) & _
                "Compatibility versions are" & Chr(10) & _
                "RTK DCI DB Creator: " & vbTab & vbTab & DCI_compatibility_num & Chr(10) & _
                "DCI DB Creator Macro: " & vbTab & macro_compatibility_num & Chr(10) & Chr(10) & _
                "Would you like to see the change log?", vbYesNo)
    End If
    
    If ((return_val = 6) And (log_present = True)) Then
        MyTxtFile = Shell("notepad " & macro_readme_file, vbNormalFocus)
    ElseIf ((return_val = 6) And (log_present = False)) Then
        MsgBox "Log Not Available."
    End If
End Sub

' This macro must be run independently from here.  It is not associated with any automatic or button based triggers.
' It clears all the styles in the workbook.  We don't use custom styles but over time the workbook has gathered up a bunch of
' bad styles which are redundant and confusing.  This removes them.
Sub StyleKiller()
    Dim N As Long, i As Long

    With ActiveWorkbook
        N = .Styles.count
        For i = N To 1 Step -1
            If Not .Styles(i).BuiltIn Then .Styles(i).Delete
        Next i
    End With
End Sub
