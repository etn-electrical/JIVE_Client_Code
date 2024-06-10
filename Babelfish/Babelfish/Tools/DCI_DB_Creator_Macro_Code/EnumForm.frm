VERSION 5.00
Begin {C62A69F0-16DC-11CE-9E98-00AA00574A4F} EnumForm 
   Caption         =   "Enums"
   ClientHeight    =   5610
   ClientLeft      =   30
   ClientTop       =   360
   ClientWidth     =   9000
   OleObjectBlob   =   "EnumForm.frx":0000
   StartUpPosition =   1  'CenterOwner
End
Attribute VB_Name = "EnumForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False




Const ENUMS_COL = "Z"
Const TOTALLENGTH_COL = "F"
Const DCI_DESCRIPTION_COLUMN = "A"
Const DCI_CODE_DEFINE_COLUMN = "B"
Const BITS_IN_BYTE = 8
Const ITEM_DELIMITER = ";"
Const SUB_ITEM_DELIMITER = ","
Const ROW_HEIGHT = 13.2


'Globals
Dim MaxEnumVal As Double

Private Sub Apply_Button_Click()
    Call Applychanges
    Apply_Button.Enabled = False
End Sub

Function PadLeft(text As Variant, totalLength As Integer, padCharacter As String) As String
    PadLeft = String(totalLength - Len(CStr(text)), padCharacter) & CStr(text)
End Function

'TODO - please remove me
Private Sub UpdateOldColumns()
    Dim RawString As String
    Dim index As Integer
    Dim ListItems() As String
    Dim ParsedItems() As String
    Dim EnumColText As String
  
    RawString = ActiveSheet.Cells(ActiveCell.row, ENUMS_COL).value
    
    If RawString = "" Then
        'No entries
    Else
        
        ListItems = Split(RawString, ITEM_DELIMITER)
        'First item denotes the number of bitfields for this parameter
        'this is for display only
        For index = 1 To UBound(ListItems) Step 1
            ParsedItems = Split(ListItems(index), SUB_ITEM_DELIMITER)
            'Remove line feeds
            ParsedItems(0) = Replace(ParsedItems(0), vbLf, "")
            If EnumColText <> "" Then
                EnumColText = EnumColText & ","
            End If
            'Build the Enum Col String
            EnumColText = EnumColText & Trim(ParsedItems(0))
        Next index
    End If
    ActiveSheet.Cells(ActiveCell.row, "J").value = EnumColText
End Sub

Private Sub Applychanges()
    Dim OutputString As String
    If 0 = Items_ListView.ListItems.Count Then
      OutputString = ""
    Else
        OutputString = Items_ListView.ListItems.Count & ";" & vbLf
        For index = 1 To Items_ListView.ListItems.Count Step 1
            OutputString = OutputString & Trim(Items_ListView.ListItems.Item(index).text) & "," & _
            Items_ListView.ListItems(index).SubItems(1) & ","
            If Items_ListView.ListItems.Count = index Then
                OutputString = OutputString & Items_ListView.ListItems(index).SubItems(2) & vbLf
             Else
              OutputString = OutputString & Items_ListView.ListItems(index).SubItems(2) & ";" & vbLf
             End If
        Next index
    End If
    ActiveSheet.Cells(ActiveCell.row, ENUMS_COL).value = OutputString
    Rows(ActiveCell.row).RowHeight = ROW_HEIGHT
    Call UpdateOldColumns
End Sub

Private Sub ApplyClose_Button_Click()
    Call Applychanges
    Unload Me
End Sub

Private Sub EnumNumber_TextBox_Change()
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
    Dim index As Integer
    For index = 1 To Items_ListView.ListItems.Count Step 1
        If EnumNumber_TextBox.value = Val(Items_ListView.ListItems.Item(index).text) Then
            NewUpdate_Button.Enabled = True
            NewUpdate_Button.caption = "Update"
            Exit For
        End If
    Next index
    'If we did not find it in the list, enable the "Add" button
    If index > Items_ListView.ListItems.Count Then
        NewUpdate_Button.Enabled = True
        NewUpdate_Button.caption = "Add"
    End If
End Sub

Private Sub Description_TextBox_KeyPress(ByVal KeyAscii As MSForms.ReturnInteger)
    If EnumNumber_TextBox.value < 0 Then
        KeyAscii = 0
        Exit Sub
    End If
    If KeyAscii = 44 Or KeyAscii = 59 Then
        KeyAscii = 0
    End If
End Sub

Private Sub EnumNumber_TextBox_KeyPress(ByVal KeyAscii As MSForms.ReturnInteger)
    If KeyAscii >= 48 And KeyAscii <= 57 Then
        If Val(EnumNumber_TextBox.text & Chr(KeyAscii)) > MaxEnumVal Then
            MsgBox "Entered Enum# " & Val(EnumNumber_TextBox.text & Chr(KeyAscii)) & " is greater than the maximum allowed value " & MaxEnumVal, vbCritical, "Invalid Enum#"
            KeyAscii = 0
            Exit Sub
        End If
    Else
        KeyAscii = 0
    End If
End Sub

Private Sub Items_ListView_ItemClick(ByVal Item As MSComctlLib.ListItem)
    EnumNumber_TextBox.text = Item.text
    Call NewSelection
End Sub

Private Sub NewSelection()
    Dim index As Integer
    For index = 1 To Items_ListView.ListItems.Count Step 1
        If EnumNumber_TextBox.value = Val(Items_ListView.ListItems.Item(index).text) Then
            CodeDefine_TextBox.text = Items_ListView.ListItems(index).SubItems(1)
            Description_TextBox = Items_ListView.ListItems(index).SubItems(2)
            Remove_Button.Enabled = True
            Items_ListView.ListItems(index).Selected = True
            Exit For
        End If
    Next index
    'If we did not find it in the list, enable the "Add" button
    If index > Items_ListView.ListItems.Count Then
        NewUpdate_Button.Enabled = True
        NewUpdate_Button.caption = "Add"
        Remove_Button.Enabled = False
    Else
        NewUpdate_Button.Enabled = False
    End If
End Sub

Private Sub CodeDefine_TextBox_Change()
    Dim index As Integer
    For index = 1 To Items_ListView.ListItems.Count Step 1
        If EnumNumber_TextBox.value = Val(Items_ListView.ListItems.Item(index).text) Then
            NewUpdate_Button.Enabled = True
            NewUpdate_Button.caption = "Update"
            Exit For
        End If
    Next index
    'If we did not find it in the list, enable the "Add" button
    If index > Items_ListView.ListItems.Count Then
        NewUpdate_Button.Enabled = True
        NewUpdate_Button.caption = "Add"
    End If
End Sub

Private Sub CodeDefine_TextBox_KeyPress(ByVal KeyAscii As MSForms.ReturnInteger)
    If EnumNumber_TextBox.value < 0 Then
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
    Dim index As Integer

    If Trim(EnumNumber_TextBox.text) = "" Then
        MsgBox "Enum# field empty", vbCritical, "Missing Field"
        Exit Sub
    End If
    
    If Trim(CodeDefine_TextBox.text) = "" Then
        MsgBox "Code Define field empty", vbCritical, "Missing Field"
        Exit Sub
    End If
    
    If NewUpdate_Button.caption = "Add" Then
        'Check for duplicates
        For index = 1 To Items_ListView.ListItems.Count Step 1
            If Trim(CodeDefine_TextBox.text) = Items_ListView.ListItems(index).SubItems(1) Then
                MsgBox "Duplicate Code Define found at Enum# " & Items_ListView.ListItems.Item(index).text, vbCritical, "Duplicate entry"
                Exit Sub
            End If
        Next index
        index = Items_ListView.ListItems.Count
        index = index + 1
        Items_ListView.ListItems.Add index, , PadLeft(EnumNumber_TextBox.text, 5, " ")
        Items_ListView.ListItems(index).SubItems(1) = Trim(CodeDefine_TextBox.text)
        Items_ListView.ListItems(index).SubItems(2) = Trim(Description_TextBox.text)
        'Enable sorting
        Items_ListView.Sorted = True
        'Refresh the list so that it gets sorted
        Items_ListView.Refresh
        'Disable sorting, so that it's easy to add a new item next time
        Items_ListView.Sorted = False
        NewUpdate_Button.Enabled = False
    Else
        'Check for duplicates
        For index = 1 To Items_ListView.ListItems.Count Step 1
            If Trim(CodeDefine_TextBox.text) = Items_ListView.ListItems(index).SubItems(1) And _
            EnumNumber_TextBox.value <> Val(Items_ListView.ListItems.Item(index).text) Then
                MsgBox "Duplicate Code Define found at Enum# " & Items_ListView.ListItems.Item(index).text, vbCritical, "Duplicate entry"
                Exit Sub
            End If
        Next index
        'Update the item
        For index = 1 To Items_ListView.ListItems.Count Step 1
            If EnumNumber_TextBox.value = Val(Items_ListView.ListItems.Item(index).text) Then
                Items_ListView.ListItems(index).SubItems(1) = Trim(CodeDefine_TextBox.text)
                Items_ListView.ListItems(index).SubItems(2) = Trim(Description_TextBox.text)
                NewUpdate_Button.Enabled = False
            End If
        Next index
    End If
    Apply_Button.Enabled = True
    ApplyClose_Button.Enabled = True
End Sub

Private Sub Remove_Button_Click()
    'Find the item
    For index = 1 To Items_ListView.ListItems.Count Step 1
        If EnumNumber_TextBox.value = Val(Items_ListView.ListItems.Item(index).text) Then
            Items_ListView.ListItems.Remove (index)
            'Enable sorting
            Items_ListView.Sorted = True
            'Refresh the list so that it gets sorted
            Items_ListView.Refresh
            'Disable sorting, so that it's easy to add a new item next time
            Items_ListView.Sorted = False
            NewUpdate_Button.Enabled = False
            Exit For
        End If
    Next index
    NewUpdate_Button.caption = "Add"
    NewUpdate_Button.Enabled = True
    Apply_Button.Enabled = True
    ApplyClose_Button.Enabled = True
End Sub

Private Sub UserForm_Initialize()
    Dim RawString As String
    Dim totalLength As Double
    Dim index As Integer
    Dim ListItems() As String
    Dim ParsedItems() As String
    
    EnumForm.caption = "Enums for " & ActiveSheet.Cells(ActiveCell.row, DCI_DESCRIPTION_COLUMN).value & _
    " (" & ActiveSheet.Cells(ActiveCell.row, DCI_CODE_DEFINE_COLUMN).value & ")"
    
    RawString = ActiveSheet.Cells(ActiveCell.row, ENUMS_COL).value
    
    totalLength = ActiveSheet.Cells(ActiveCell.row, TOTALLENGTH_COL).value
    'Calculate the maximum enum length based on the length
    MaxEnumVal = (2 ^ (totalLength * BITS_IN_BYTE)) - 1

    If RawString = "" Then
        'No entries
    Else
        ListItems = Split(RawString, ITEM_DELIMITER)
        'First item denotes the number of bitfields for this parameter
        'this is for display only
        For index = 1 To UBound(ListItems) Step 1
            If "" <> Trim(ListItems(index)) Then
                ParsedItems = Split(ListItems(index), SUB_ITEM_DELIMITER)
                'Remove line feeds
                ParsedItems(0) = Replace(ParsedItems(0), vbLf, "")
                ParsedItems(1) = Replace(ParsedItems(1), vbLf, "")
                ParsedItems(2) = Replace(ParsedItems(2), vbLf, "")
                'Add items to the listview
                Items_ListView.ListItems.Add index, , PadLeft(Trim(ParsedItems(0)), 5, " ")
                Items_ListView.ListItems(index).SubItems(1) = Trim(ParsedItems(1))
                Items_ListView.ListItems(index).SubItems(2) = Trim(ParsedItems(2))
            End If
        Next index
    End If
End Sub
