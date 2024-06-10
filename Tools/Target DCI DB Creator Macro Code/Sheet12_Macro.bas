VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Sheet12"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = True
Private Sub CommandButton1_Click()
        Application.Run "'DCI DB Creator Macros.xlsm'!Verify_IO_Modules"
End Sub

Private Sub CommandButton2_Click()
        Application.Run "'DCI DB Creator Macros.xlsm'!Generate_Enum_Values"
End Sub

Private Sub Generate_Output_Bit_Enumeration()
        Application.Run "'DCI DB Creator Macros.xlsm'!Generate_Output_Bit_Enum_Values"
End Sub
'Sub Create_DataEX_Module_Information()
'    Application.Run "'DCI DB Creator Macros.xlsm'!Create_Module_Info_Files"
'End Sub
