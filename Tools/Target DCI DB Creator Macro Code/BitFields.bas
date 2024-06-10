Attribute VB_Name = "BitFields"
Sub DCI_Descriptors_Bitfields()
If ActiveCell.Row <= 8 Then
MsgBox "Please select a valid DCID cell first", vbCritical, "Invalid Selection"
ElseIf ActiveSheet.Cells(ActiveCell.Row, "F").Value = "" Then
MsgBox "The selected DCI Parameter has an invalid Total Length", vbCritical, "Invalid Length"
Else
BitFieldsForm.Show
End If
End Sub
