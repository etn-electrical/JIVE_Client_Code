VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Sheet31"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = True
Sub Create_EtherNetIP_DCI_Btn()
    Application.Run "'DCI DB Creator Macros.xls'!Create_EtherNetIP_DCI_Xfer"
End Sub

Sub Verify_EtherNetIP_Data_Items_Btn()
    Application.Run "'DCI DB Creator Macros.xls'!Verify_EtherNetIP_List"
End Sub

