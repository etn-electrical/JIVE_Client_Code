VERSION 5.00
Begin {C62A69F0-16DC-11CE-9E98-00AA00574A4F} BitFieldsForm 
   Caption         =   "BitFields"
   ClientHeight    =   5616
   ClientLeft      =   36
   ClientTop       =   360
   ClientWidth     =   9000
   OleObjectBlob   =   "BitFieldsForm.frx":0000
   StartUpPosition =   1  'CenterOwner
End
Attribute VB_Name = "BitFieldsForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Const BITFIELDS_COL = "K"
Const TOTALLENGTH_COL = "F"
Const DCI_DESCRIPTION_COLUMN = "A"
Const DCI_CODE_DEFINE_COLUMN = "B"
Const BITS_IN_BYTE = 8
Const ITEM_DELIMITER = ";"
Const SUB_ITEM_DELIMITER = ","

Private Sub Apply_Button_Click()
    Call Applychanges
    Apply_Button.Enabled = False
End Sub

Private Sub Applychanges()
    Dim OutputString As String
    If 0 = Items_ListView.ListItems.Count Then
      OutputString = ""
    Else
        OutputString = Items_ListView.ListItems.Count & ";" & vbLf
        For Index = 1 To Items_ListView.ListItems.Count Step 1
            OutputString = OutputString & Items_ListView.ListItems.Item(Index).Text & "," & _
            Items_ListView.ListItems(Index).SubItems(1) & ","
            If Items_ListView.ListItems.Count = Index Then
                OutputString = OutputString & Items_ListView.ListItems(Index).SubItems(2) & vbLf
             Else
              OutputString = OutputString & Items_ListView.ListItems(Index).SubItems(2) & ";" & vbLf
             End If
        Next Index
    End If
    ActiveSheet.Cells(ActiveCell.Row, BITFIELDS_COL).Value = OutputString
End Sub

Private Sub ApplyClose_Button_Click()
    Call Applychanges
    Unload Me
End Sub

Private Sub Bit_ComboBox_Change()
    Call NewSelection
End Sub

Private Sub Close_Button_Click()
    Dim YesNo As String
    If Apply_Button.Enabled = True Then
        YesNo = MsgBox("Unsaved Changes will be lost. Continue exiting and lose changes ?", vbYesNo + vbCritical, "Unsaved Changes!")
        If YesNo = vbNo Then
            Exit Sub
        End If
    End If
    Unload Me
End Sub


Private Sub Description_TextBox_Change()
    Dim Index As Integer
    For Index = 1 To Items_ListView.ListItems.Count Step 1
        If CInt(Bit_ComboBox.List(Bit_ComboBox.ListIndex)) = CInt(Items_ListView.ListItems.Item(Index).Text) Then
            NewUpdate_Button.Enabled = True
            NewUpdate_Button.Caption = "Update"
            Exit For
        End If
    Next Index
    'If we did not find it in the list, enable the "Add" button
    If Index > Items_ListView.ListItems.Count Then
        NewUpdate_Button.Enabled = True
        NewUpdate_Button.Caption = "Add"
    End If
End Sub

Private Sub Description_TextBox_KeyPress(ByVal KeyAscii As MSForms.ReturnInteger)
    If Bit_ComboBox.ListIndex < 0 Then
        KeyAscii = 0
        Exit Sub
    End If
    If KeyAscii = 44 Or KeyAscii = 59 Then
        KeyAscii = 0
    End If
End Sub

Private Sub Items_ListView_ItemClick(ByVal Item As MSComctlLib.ListItem)
    Bit_ComboBox.ListIndex = CInt(Item.Text)
    Call NewSelection
End Sub

Private Sub NewSelection()
    Dim Index As Integer
    For Index = 1 To Items_ListView.ListItems.Count Step 1
        If CInt(Bit_ComboBox.List(Bit_ComboBox.ListIndex)) = CInt(Items_ListView.ListItems.Item(Index).Text) Then
            CodeDefine_TextBox.Text = Items_ListView.ListItems(Index).SubItems(1)
            Description_TextBox = Items_ListView.ListItems(Index).SubItems(2)
            Remove_Button.Enabled = True
            Items_ListView.ListItems(Index).Selected = True
            Exit For
        End If
    Next Index
    'If we did not find it in the list, enable the "Add" button
    If Index > Items_ListView.ListItems.Count Then
        NewUpdate_Button.Enabled = True
        NewUpdate_Button.Caption = "Add"
        Remove_Button.Enabled = False
    Else
        NewUpdate_Button.Enabled = False
    End If
End Sub

Private Sub CodeDefine_TextBox_Change()
    Dim Index As Integer
    For Index = 1 To Items_ListView.ListItems.Count Step 1
        If CInt(Bit_ComboBox.List(Bit_ComboBox.ListIndex)) = CInt(Items_ListView.ListItems.Item(Index).Text) Then
            NewUpdate_Button.Enabled = True
            NewUpdate_Button.Caption = "Update"
            Exit For
        End If
    Next Index
    'If we did not find it in the list, enable the "Add" button
    If Index > Items_ListView.ListItems.Count Then
        NewUpdate_Button.Enabled = True
        NewUpdate_Button.Caption = "Add"
    End If
End Sub

Private Sub CodeDefine_TextBox_KeyPress(ByVal KeyAscii As MSForms.ReturnInteger)
    If Bit_ComboBox.ListIndex < 0 Then
        KeyAscii = 0
        Exit Sub
    End If
    If KeyAscii >= 48 And KeyAscii <= 57 Or KeyAscii = 95 Or KeyAscii >= 65 And KeyAscii <= 90 Then
        'Do Nothing
    ElseIf KeyAscii >= 97 And KeyAscii <= 122 Then 'a to z, then change to A to Z
        KeyAscii = KeyAscii - 32
    Else
        KeyAscii = 0
    End If
End Sub


Private Sub NewUpdate_Button_Click()
    Dim Index As Integer

    If Trim(CodeDefine_TextBox.Text) = "" Then
        MsgBox "Code Define field empty", vbCritical, "Missing Field"
        Exit Sub
    End If
    
    If NewUpdate_Button.Caption = "Add" Then
        'Check for duplicates
        For Index = 1 To Items_ListView.ListItems.Count Step 1
            If Trim(CodeDefine_TextBox.Text) = Items_ListView.ListItems(Index).SubItems(1) Then
                MsgBox "Duplicate Code Define found at Bit# " & Items_ListView.ListItems.Item(Index).Text, vbCritical, "Duplicate entry"
                Exit Sub
            End If
        Next Index
        Index = Items_ListView.ListItems.Count
        Index = Index + 1
        Items_ListView.ListItems.Add Index, , Bit_ComboBox.List(Bit_ComboBox.ListIndex)
        Items_ListView.ListItems(Index).SubItems(1) = Trim(CodeDefine_TextBox.Text)
        Items_ListView.ListItems(Index).SubItems(2) = Trim(Description_TextBox.Text)
        'Enable sorting
        Items_ListView.Sorted = True
        'Refresh the list so that it gets sorted
        Items_ListView.Refresh
        'Disable sorting, so that it's easy to add a new item next time
        Items_ListView.Sorted = False
        NewUpdate_Button.Enabled = False
    Else
        'Check for duplicates
        For Index = 1 To Items_ListView.ListItems.Count Step 1
            If Trim(CodeDefine_TextBox.Text) = Items_ListView.ListItems(Index).SubItems(1) And _
            CInt(Bit_ComboBox.List(Bit_ComboBox.ListIndex)) <> CInt(Items_ListView.ListItems.Item(Index).Text) Then
                MsgBox "Duplicate Code Define found at Bit# " & Items_ListView.ListItems.Item(Index).Text, vbCritical, "Duplicate entry"
                Exit Sub
            End If
        Next Index
        'Update the item
        For Index = 1 To Items_ListView.ListItems.Count Step 1
            If CInt(Bit_ComboBox.List(Bit_ComboBox.ListIndex)) = CInt(Items_ListView.ListItems.Item(Index).Text) Then
                Items_ListView.ListItems(Index).SubItems(1) = Trim(CodeDefine_TextBox.Text)
                Items_ListView.ListItems(Index).SubItems(2) = Trim(Description_TextBox.Text)
                NewUpdate_Button.Enabled = False
            End If
        Next Index
    End If
    Apply_Button.Enabled = True
    ApplyClose_Button.Enabled = True
End Sub

Private Sub Remove_Button_Click()
    'Find the item
    For Index = 1 To Items_ListView.ListItems.Count Step 1
        If CInt(Bit_ComboBox.List(Bit_ComboBox.ListIndex)) = CInt(Items_ListView.ListItems.Item(Index).Text) Then
            Items_ListView.ListItems.Remove (Index)
            'Enable sorting
            Items_ListView.Sorted = True
            'Refresh the list so that it gets sorted
            Items_ListView.Refresh
            'Disable sorting, so that it's easy to add a new item next time
            Items_ListView.Sorted = False
            NewUpdate_Button.Enabled = False
            Exit For
        End If
    Next Index
    NewUpdate_Button.Caption = "Add"
    NewUpdate_Button.Enabled = True
    Apply_Button.Enabled = True
    ApplyClose_Button.Enabled = True
End Sub

Private Sub UserForm_Initialize()
    Dim RawString As String
    Dim TotalLength As Integer
    Dim Index As Integer
    Dim StrPos1 As Integer
    Dim ListItems() As String
    Dim ParsedItems() As String
    
    BitFieldsForm.Caption = "BitFields for " & ActiveSheet.Cells(ActiveCell.Row, DCI_DESCRIPTION_COLUMN).Value & _
    " (" & ActiveSheet.Cells(ActiveCell.Row, DCI_CODE_DEFINE_COLUMN).Value & ")"
    
    RawString = ActiveSheet.Cells(ActiveCell.Row, BITFIELDS_COL).Value
    
    TotalLength = ActiveSheet.Cells(ActiveCell.Row, TOTALLENGTH_COL).Value
    'Add bits to the BitList Combo Box
    For Index = 0 To (TotalLength * BITS_IN_BYTE) - 1 Step 1
        Bit_ComboBox.AddItem Index, Index
    Next Index
    
    If RawString = "" Then
        'No entries
    Else
        ListItems = Split(RawString, ITEM_DELIMITER)
        'First item denotes the number of bitfields for this parameter
        'this is for display only
        For Index = 1 To UBound(ListItems) Step 1
            ParsedItems = Split(ListItems(Index), SUB_ITEM_DELIMITER)
            'Remove line feeds
            ParsedItems(0) = Replace(ParsedItems(0), vbLf, "")
            ParsedItems(1) = Replace(ParsedItems(1), vbLf, "")
            ParsedItems(2) = Replace(ParsedItems(2), vbLf, "")
            'Add items to the listview
            Items_ListView.ListItems.Add Index, , Trim(ParsedItems(0))
            Items_ListView.ListItems(Index).SubItems(1) = Trim(ParsedItems(1))
            Items_ListView.ListItems(Index).SubItems(2) = Trim(ParsedItems(2))
        Next Index
    End If
End Sub
