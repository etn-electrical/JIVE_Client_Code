Attribute VB_Name = "MacroRunner"
'*************************************************************
'*************************************************************
'*** Runs all the macros you list below.
'*************************************************************
'*************************************************************
Sub Mega_Macro_Run()
    Call ProgressIndicator.Progress_Bar_Load("Magic in progress. Go refill your coffee.")

    Call ProgressIndicator.Progress_Update(0, "Opening DCI Macros")

    Call DCI_Descriptors_Open_DCI_Macros
    
'*************************************************************
'*** DCI Descriptors
    Call ProgressIndicator.Progress_Update(25, "DCI Descriptors")
    Sheets("DCI Descriptors").Select
  
    Call Run_DCI_DB_Macro("Gen_DCI_Enum_Code")
    Call Run_DCI_DB_Macro("Create_DCI_Structs")

'*************************************************************
'*** NV Memory
    Call ProgressIndicator.Progress_Update(50, "NV Memory")
    Sheets("NVM Address").Select
    Call Run_DCI_DB_Macro("Create_NV_Address_File")
    Call Run_DCI_DB_Macro("Create_NV_Defaults_File")

'*************************************************************
'*** Modbus
    Call ProgressIndicator.Progress_Update(75, "Modbus")
    Sheets("Modbus").Select
    Call Run_DCI_DB_Macro("Create_Modbus_DCI_Xfer")

    Call ProgressIndicator.Progress_Update(100, "Done...")
    Call ProgressIndicator.Progress_Bar_UnLoad

End Sub


