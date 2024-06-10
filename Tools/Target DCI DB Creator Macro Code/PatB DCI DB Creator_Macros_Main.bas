VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "ThisWorkbook"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = True
Private Sub Workbook_BeforeSave(ByVal SaveAsUI As Boolean, Cancel As Boolean)
 
    Application.Calculation = xlCalculationManual
    
    On Error GoTo Errorcatch    ' To hopefully catch some badness that occasionally happens with the macro.
    
    If Sheets("Usage Notes").Cells(3, 2).Text <> Empty Then
        Call Export_Worksheets
    End If
 
    file_path = ThisWorkbook.Path & "\Target DCI DB Creator Macro Code\"
    
    For i = 1 To ThisWorkbook.VBProject.VBComponents.Count
        If ThisWorkbook.VBProject.VBComponents.Item(i).Name = "ThisWorkbook" Then
            ThisWorkbook.VBProject.VBComponents.Item(i).Export (file_path & _
                        Left(ThisWorkbook.Name, Len(ThisWorkbook.Name) - 4) & "_Macros_Main.bas")
        ElseIf Left(ThisWorkbook.VBProject.VBComponents.Item(i).Name, 5) = "Sheet" Then
            ThisWorkbook.VBProject.VBComponents.Item(i).Export (file_path & _
                        ThisWorkbook.VBProject.VBComponents.Item(i).Name & "_Macro.bas")
        Else
            ThisWorkbook.VBProject.VBComponents.Item(i).Export (file_path & _
                        ThisWorkbook.VBProject.VBComponents.Item(i).Name & ".bas")
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

