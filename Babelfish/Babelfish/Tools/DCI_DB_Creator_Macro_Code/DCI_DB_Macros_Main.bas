VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "ThisWorkbook"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = True
Dim module_name_list() As String
Dim file_path As String
#If VBA7 Then
    Private Declare PtrSafe Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)
#Else
    Private Declare Sub Sleep Lib "kernel32" (ByVal dwMilliseconds As Long)
#End If

Sub CheckTrustAccess()
Dim strStatus, strOpp, strCheck As String
Dim bEnabled As Boolean
If Not VBAIsTrusted Then
'ask the user if they want to try to programatically toggle trust access. If failed, give them directions.
    bEnabled = False
    strStatus = "DISABLE"
    strOpp = "ENABLE"
    V = MsgBox("Trust Access to the VBA Project Object Model is DISABLED." & Chr(10) & Chr(10) & _
       "This access is required to run macros in DCI workbook" & Chr(10) & Chr(10) & _
       "Would you like to ENABLE it automatically?" & Chr(10) & Chr(10) & _
       "To Enable/Disable this setting MANUALLY follow below steps" & Chr(10) & _
       Space(5) & "1) Click " & Chr(145) & "File-> Options-> Trust Center-> Trust Center Settings" & Chr(146) & Chr(10) & _
       Space(5) & "2) Click Macro Settings" & Chr(10) & _
       Space(5) & "3) Toggle the check box next to ""Trust Access to the VBA project object model""" _
       , vbYesNo, " ENABLE Trust Access")
     
    'try to toggle trust
    If V = 6 Then
        Call ToggleTrust(bEnabled)
        If VBAIsTrusted = bEnabled Then
            'if ToggleTrust fails to programatically toggle trust
            MsgBox "Failed to ENABLE Trust Access." & Chr(10) & Chr(10), vbOKOnly, " Failed"
            End
        Else
            MsgBox "Successfully ENABLED Trust Access." & Chr(10) & Chr(10) _
            , vbOKOnly
        End If
    Else
        End
    End If
End If
End Sub

Private Function VBAIsTrusted() As Boolean
Dim a1 As Integer
On Error GoTo Label1
a1 = ActiveWorkbook.VBProject.VBComponents.count
VBAIsTrusted = True
Exit Function
Label1:
VBAIsTrusted = False
End Function

Private Sub ToggleTrust(bEnabled As Boolean)
Dim b1 As Integer, i As Integer
Dim strkeys As String
On Error Resume Next
    Do While i <= 2 'try to sendkeys 3 times
        Sleep 100
    strkeys = "%tms%v{ENTER}"
        Call SendKeys(Trim(strkeys)) 'ST%V{ENTER}")
        DoEvents
        If VBAIsTrusted <> bEnabled Then Exit Do 'successfully toggled trust
        Sleep (100)
        i = i + 1
    Loop
End Sub

Private Sub Workbook_BeforeClose(Cancel As Boolean)
    'Commented (for now) removal algorithm of module list to supress error condition when User press "Cancel" button on save dialoag box
    'Call Remove_Module
End Sub

Private Sub Workbook_BeforeSave(ByVal SaveAsUI As Boolean, Cancel As Boolean)

    Application.Calculation = xlCalculationManual
    
    On Error GoTo Errorcatch
    If file_path = Empty Then
    file_path = ThisWorkbook.Path & "\DCI_DB_Creator_Macro_Code\"
    End If
    For i = 1 To ThisWorkbook.VBProject.VBComponents.count
        If ThisWorkbook.VBProject.VBComponents.Item(i).name = "ThisWorkbook" Then
            ThisWorkbook.VBProject.VBComponents.Item(i).Export (file_path & "DCI_DB_Macros_Main.bas")
        ElseIf Left(ThisWorkbook.VBProject.VBComponents.Item(i).name, 5) = "Sheet" Then
            ThisWorkbook.VBProject.VBComponents.Item(i).Export (file_path & ThisWorkbook.VBProject.VBComponents.Item(i).name & "_Macro.bas")
        Else
            ThisWorkbook.VBProject.VBComponents.Item(i).Export (file_path & ThisWorkbook.VBProject.VBComponents.Item(i).name & ".bas")
        End If
    Next

    Application.Calculation = xlCalculationAutomatic

Exit Sub
Errorcatch:
    MsgBox Err.DESCRIPTION

End Sub

Private Sub Workbook_Open()
    Dim ref_found As Boolean
    Call CheckTrustAccess
    
    For i = 1 To ThisWorkbook.VBProject.References.count
        If ThisWorkbook.VBProject.References.Item(i).GUID = "{0002E157-0000-0000-C000-000000000046}" Then
        ref_found = True
        End If
    Next
    
    If ref_found = False Then
      'MsgBox ("Reference to Microsoft Visual Basic for Applications Extensibility 5.3 missing. In VB Editor, Goto Tool>>Reference and check the same")
       ThisWorkbook.VBProject.References.AddFromGuid _
       GUID:="{0002E157-0000-0000-C000-000000000046}", Major:=5, Minor:=3
    End If

    file_path = ThisWorkbook.Path & "\DCI_DB_Creator_Macro_Code\"
            
    Call Remove_Module
    Call Load_Module
End Sub

Sub Load_Module()
    Dim VBAEditor As VBIDE.VBE
    Dim VBProj As VBIDE.VBProject
    Dim VBComp As VBIDE.VBComponent
    Dim CodeMod As VBIDE.CodeModule
    
    Set VBAEditor = Application.VBE
    Set VBProj = VBAEditor.ActiveVBProject

    module_name_list = Get_Module_List()
    For i = 0 To UBound(module_name_list)
        Application.VBE.ActiveVBProject.VBComponents.Import (file_path & CStr(module_name_list(i)) & ".bas")
    Next
End Sub


Sub Remove_Module()
        Dim VBAEditor As VBIDE.VBE
        Dim VBProj As VBIDE.VBProject
        Dim VBComp As VBIDE.VBComponent
        Dim CodeMod As VBIDE.CodeModule
        
        Set VBAEditor = Application.VBE
        Set VBProj = VBAEditor.ActiveVBProject
        
        module_name_list = Get_Module_List()
        For Each VBComp In VBProj.VBComponents
            For i = 0 To UBound(module_name_list)
                If VBComp.name = module_name_list(i) Then
                    Application.VBE.ActiveVBProject.VBComponents.Remove VBComp
                    Exit For
                End If
            Next
        Next
End Sub

Function Get_Module_List()

    Dim i As Integer
    Dim strFileName As String
    Dim module_list() As String
    
    file_path = ThisWorkbook.Path & "\DCI_DB_Creator_Macro_Code\"
    strFileName = file_path & "ModuleList.txt"
    Open strFileName For Input As #1
    
     ' -------- read from txt file to module_list -------- '
    
     i = 0
     Do Until EOF(1)
        ReDim Preserve module_list(i)
        Line Input #1, module_list(i)
        i = i + 1
     Loop
     Close #1
     Get_Module_List = module_list
End Function



