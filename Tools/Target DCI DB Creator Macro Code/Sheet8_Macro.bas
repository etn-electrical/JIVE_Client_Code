VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Sheet8"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = True

Private Sub Create_GSD_File_Click()
    Application.Run "'DCI DB Creator Macros.xlsm'!Create_Profibus_GSD_File"
End Sub
