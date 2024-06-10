VERSION 5.00
Begin {C62A69F0-16DC-11CE-9E98-00AA00574A4F} ProgressIndicator 
   Caption         =   "Progress"
   ClientHeight    =   1476
   ClientLeft      =   36
   ClientTop       =   360
   ClientWidth     =   5664
   OleObjectBlob   =   "ProgressIndicator.frx":0000
   StartUpPosition =   1  'CenterOwner
End
Attribute VB_Name = "ProgressIndicator"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Public Function Progress_Bar_Load(ByVal Title As String)
    Me.Caption = Title
    Me.Show vbModeless
End Function

Public Function Progress_Bar_UnLoad()
    Unload Me
End Function

Public Function Progress_Update(ByVal PercentageComplete As Integer, ByVal CurrentActivity As String)

If (PercentageComplete > 100) Then
Unload Me
End If

General_ProgressBar.value = PercentageComplete
Percentage_Label.Caption = PercentageComplete & "%"
CurrentActivity_Label.Caption = CurrentActivity
Me.Repaint

End Function

