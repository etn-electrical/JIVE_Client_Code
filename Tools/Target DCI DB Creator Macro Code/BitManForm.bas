VERSION 5.00
Begin {C62A69F0-16DC-11CE-9E98-00AA00574A4F} BitManForm 
   Caption         =   "BitMan Source DCIDs"
   ClientHeight    =   9075
   ClientLeft      =   36
   ClientTop       =   360
   ClientWidth     =   9624
   OleObjectBlob   =   "BitManForm.frx":0000
   StartUpPosition =   1  'CenterOwner
End
Attribute VB_Name = "BitManForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'DCI Descriptors sheet
Const DCI_DESCRIPTORS_SHEET = "DCI Descriptors"
Const BITFIELDS_COL = "K"
Const TOTALLENGTH_COL = "F"
Const DCI_CODE_DEFINE_COLUMN = "B"
Const DCI_DESCRIPTORS_START_ROW = 9
Const ITEM_DELIMITER = ";"
Const SUB_ITEM_DELIMITER = ","
Const BITS_IN_BYTE = 8

'Bitman sheet
Const BITMAN_SHEET = "BitMan"
Const BITMAN_DATA_START_ROW = 8
Const BITMAN_DATA_START_COL = 4
Const BITMAN_NAME_ROW = 7
Const BITMAN_SINK_DCI_PARAMS_COL = "B"
Const BITMAN_SINK_BITFIELDS_COL = "C"
Const PACKAGE_NAME_ROW = "1"
Const PACKAGE_NAME_COL = "C"
Const FOLDER_PATH_ROW = "2"
Const FOLDER_PATH_COL = "C"
Const PROFILE_NAME_ROW = 7

'Profile Data sheet
Const PROFILE_DATA_SHEET = "__Profile_Data"
Const PROFILE_DATA_SOURCE_DCID_COL = "A"
Const PROFILE_DATA_SOURCE_BITFILED_COL = "B"
Const PROFILE_DATA_SINK_DCID_COL = "C"
Const PROFILE_DATA_SOURCE_VALIDATION_COL = "D"
Const PROFILE_DATA_START_ROW = 4

'CodeGen
Const DCI_DATA_INCLUDE = "#include ""DCI_Data.h"""
Const BITFIELDS_DATATYPE = "const BITMAN_TD "
Const DCID_DATATYPE = "const DCI_ID_TD "
Const SOURCE_PREFIX = "SOURCE_"
Const SINK_PREFIX = "SINK_"
Const BITMAN_BITFIELDS_FILE_NAME = "Bitman_Bitfields"
Const BITMAN_BASE_FILE_NAME = "BitMan_Base"
Const CPP_FILE_EXTN = ".cpp"
Const H_FILE_EXTN = ".h"
Const PND_IFNDEF = "#ifndef __"
Const PND_DEFINE = "#define __"
Const PND_ENDIF = "#endif"
Const NAMESPACE = "Logic"
Const BITMAN_DT_INCLUDE = "#include ""BitMan_Defines.h"""
Const source = 1
Const SINK = 2
Const BITMAN_BASE_STRUCT_NAME = "BitMan_Base_Structure"

'Log
Dim Global_Log As String


Private Sub Cancel_Button_Click()
    Unload Me
End Sub



Private Sub Source_Add_Button_Click()
Source_Used_ListView.ListItems.Add , , Source_Available_ListView.SelectedItem.text
Source_Available_ListView.ListItems.Remove (Source_Available_ListView.SelectedItem.index)
End Sub

Private Sub Source_Remove_Button_Click()
Source_Available_ListView.ListItems.Add , , Source_Used_ListView.SelectedItem.text
Source_Used_ListView.ListItems.Remove (Source_Used_ListView.SelectedItem.index)
End Sub

Private Sub Sink_Add_Button_Click()
Sink_Used_ListView.ListItems.Add , , Sink_Available_ListView.SelectedItem.text
Sink_Available_ListView.ListItems.Remove (Sink_Available_ListView.SelectedItem.index)
End Sub

Private Sub Sink_Remove_Button_Click()
Sink_Available_ListView.ListItems.Add , , Sink_Used_ListView.SelectedItem.text
Sink_Used_ListView.ListItems.Remove (Sink_Used_ListView.SelectedItem.index)
End Sub

Private Sub Source_SaveAndExit()
Dim CurParam As String
Dim ItemFound As Boolean
Dim Profile_Source_Row As Integer
    'Add new parameters
    For index = 1 To Source_Used_ListView.ListItems.count Step 1
        CurParam = Trim(Source_Used_ListView.ListItems.Item(index).text)
        Profile_Source_Row = PROFILE_DATA_START_ROW
        Do While True
            If (CurParam = Trim(Worksheets(PROFILE_DATA_SHEET).Cells(Profile_Source_Row, PROFILE_DATA_SOURCE_DCID_COL).value)) Then
                Exit Do
            ElseIf ("" = Trim(Worksheets(PROFILE_DATA_SHEET).Cells(Profile_Source_Row, PROFILE_DATA_SOURCE_DCID_COL).value)) Then
                'Reached the end in the source column and did not find the parameter, so add it
                Worksheets(PROFILE_DATA_SHEET).Cells(Profile_Source_Row, PROFILE_DATA_SOURCE_DCID_COL).value = CurParam
                'TODO add it to the dropdown in Profiles sheet
                'And Exit the loop
                Exit Do
            Else
                Profile_Source_Row = Profile_Source_Row + 1
            End If
        Loop
    Next index
    
    'Remove unused parameters
    Profile_Source_Row = PROFILE_DATA_START_ROW
    Do While True
        CurParam = Trim(Worksheets(PROFILE_DATA_SHEET).Cells(Profile_Source_Row, PROFILE_DATA_SOURCE_DCID_COL).value)
        If (CurParam = "") Then
            Exit Do
        End If
        ItemFound = False
        For index = 1 To Source_Used_ListView.ListItems.count Step 1
            If (Source_Used_ListView.ListItems.Item(index).text = CurParam) Then
                ItemFound = True
                Exit For
            End If
        Next index
        If (ItemFound = False) Then
            'Remove the Item
            Worksheets(PROFILE_DATA_SHEET).Cells(Profile_Source_Row, PROFILE_DATA_SOURCE_DCID_COL).Delete Shift:=xlUp
        Else
            Profile_Source_Row = Profile_Source_Row + 1
        End If
    Loop
End Sub

Private Sub Sink_SaveAndExit()
Dim CurParam As String
Dim ItemFound As Boolean
Dim Profile_Sink_Row As Integer
    'Add new parameters
    For index = 1 To Sink_Used_ListView.ListItems.count Step 1
        CurParam = Trim(Sink_Used_ListView.ListItems.Item(index).text)
        Profile_Sink_Row = PROFILE_DATA_START_ROW
        Do While True
            If (CurParam = Trim(Worksheets(PROFILE_DATA_SHEET).Cells(Profile_Sink_Row, PROFILE_DATA_SINK_DCID_COL).value)) Then
                Exit Do
            ElseIf ("" = Trim(Worksheets(PROFILE_DATA_SHEET).Cells(Profile_Sink_Row, PROFILE_DATA_SINK_DCID_COL).value)) Then
                'Reached the end in the Sink column and did not find the parameter, so add it
                Worksheets(PROFILE_DATA_SHEET).Cells(Profile_Sink_Row, PROFILE_DATA_SINK_DCID_COL).value = CurParam
                'TODO add it to the dropdown in Profiles sheet
                'And Exit the loop
                Exit Do
            Else
                Profile_Sink_Row = Profile_Sink_Row + 1
            End If
        Loop
    Next index
    
    'Remove unused parameters
    Profile_Sink_Row = PROFILE_DATA_START_ROW
    Do While True
        CurParam = Trim(Worksheets(PROFILE_DATA_SHEET).Cells(Profile_Sink_Row, PROFILE_DATA_SINK_DCID_COL).value)
        If (CurParam = "") Then
            Exit Do
        End If
        ItemFound = False
        For index = 1 To Sink_Used_ListView.ListItems.count Step 1
            If (Sink_Used_ListView.ListItems.Item(index).text = CurParam) Then
                ItemFound = True
                Exit For
            End If
        Next index
        If (ItemFound = False) Then
            'Remove the Item
            Worksheets(PROFILE_DATA_SHEET).Cells(Profile_Sink_Row, PROFILE_DATA_SINK_DCID_COL).Delete Shift:=xlUp
        Else
           Profile_Sink_Row = Profile_Sink_Row + 1
        End If
    Loop
End Sub


Private Sub SaveAndExit_Button_Click()
    Call Source_SaveAndExit
    Call Sink_SaveAndExit
    Call Refresh_Bitman_Sink_Params
    Call Refresh_Source_Bitfields
    Call Populate_Source_Validation
    Unload Me
End Sub

Private Sub Source_Init()

Dim DCI_Desc_Row_Index As Integer
DCI_Desc_Row_Index = DCI_DESCRIPTORS_START_ROW
Dim Cur_DCI_Code_Define As String
Dim Cur_Bitfields As String
Dim Cur_Profile_DCID As String
Dim Profile_Data_Row As Integer
Dim Found_In_Source_List As Boolean

Do While True
    Cur_DCI_Code_Define = Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(DCI_Desc_Row_Index, DCI_CODE_DEFINE_COLUMN).value)
    Cur_Bitfields_Val = Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(DCI_Desc_Row_Index, BITFIELDS_COL).value)
    If (Cur_DCI_Code_Define <> "") Then
        If ((Cur_Bitfields_Val <> "") And (Cur_Bitfields_Val <> "0;")) Then
            Found_In_Source_List = False
            Profile_Data_Row = PROFILE_DATA_START_ROW
            'Search for an entry in the source DCIDs in the Profile data sheet
            Do While True
                Cur_Profile_DCID = Trim(Worksheets(PROFILE_DATA_SHEET).Cells(Profile_Data_Row, PROFILE_DATA_SOURCE_DCID_COL).value)
                If (Cur_Profile_DCID = Cur_DCI_Code_Define) Then
                    Found_In_Source_List = True
                    Exit Do
                ElseIf (Cur_Profile_DCID = "") Then
                    Exit Do
                Else
                    Profile_Data_Row = Profile_Data_Row + 1
                End If
            Loop
            
            If (Found_In_Source_List = True) Then
                'Already used, add it to the used list
                Source_Used_ListView.ListItems.Add , , Cur_DCI_Code_Define
            Else
                'Available for use
                Source_Available_ListView.ListItems.Add , , Cur_DCI_Code_Define
            End If
            
        End If
    Else
        Exit Do
    End If
    DCI_Desc_Row_Index = DCI_Desc_Row_Index + 1
Loop

End Sub


Private Sub Sink_Init()

Dim DCI_Desc_Row_Index As Integer
DCI_Desc_Row_Index = DCI_DESCRIPTORS_START_ROW
Dim Cur_DCI_Code_Define As String
Dim Cur_Bitfields As String
Dim Cur_Profile_DCID As String
Dim Profile_Data_Row As Integer
Dim Found_In_Sink_List As Boolean

Do While True
    Cur_DCI_Code_Define = Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(DCI_Desc_Row_Index, DCI_CODE_DEFINE_COLUMN).value)
    Cur_Bitfields_Val = Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(DCI_Desc_Row_Index, BITFIELDS_COL).value)
    If (Cur_DCI_Code_Define <> "") Then
        If ((Cur_Bitfields_Val <> "") And (Cur_Bitfields_Val <> "0;")) Then
            Found_In_Sink_List = False
            Profile_Data_Row = PROFILE_DATA_START_ROW
            'Search for an entry in the Sink DCIDs in the Profile data sheet
            Do While True
                Cur_Profile_DCID = Trim(Worksheets(PROFILE_DATA_SHEET).Cells(Profile_Data_Row, PROFILE_DATA_SINK_DCID_COL).value)
                If (Cur_Profile_DCID = Cur_DCI_Code_Define) Then
                    Found_In_Sink_List = True
                    Exit Do
                ElseIf (Cur_Profile_DCID = "") Then
                    Exit Do
                Else
                    Profile_Data_Row = Profile_Data_Row + 1
                End If
            Loop
            
            If (Found_In_Sink_List = True) Then
                'Already used, add it to the used list
                Sink_Used_ListView.ListItems.Add , , Cur_DCI_Code_Define
            Else
                'Available for use
                Sink_Available_ListView.ListItems.Add , , Cur_DCI_Code_Define
            End If
            
        End If
    Else
        Exit Do
    End If
    DCI_Desc_Row_Index = DCI_Desc_Row_Index + 1
Loop

End Sub

''''''''''Profile Data Sheet Helpers''''''''''

Private Function Generate_Source_Bitfields(ByVal Bitfield_Row_Index As Integer, ByVal Source_DCID As String) As Integer
Dim DCI_Desc_Row_Index As Integer
Dim ListItems() As String
Dim ParsedItems() As String
Dim RawString As String
Dim Source_DCID_Bitfield_String As String

    DCI_Desc_Row_Index = Find_DCID_Row(Source_DCID)
    If (0 <> DCI_Desc_Row_Index) Then
        'DCID found, parse the Bitfields and create the profile data source bitfields
        RawString = Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(DCI_Desc_Row_Index, BITFIELDS_COL).value)
        ListItems = Split(RawString, ITEM_DELIMITER)
        'First item denotes the number of bitfields for this parameter
        'this is for display only
        For index = 1 To UBound(ListItems) Step 1
            ParsedItems = Split(ListItems(index), SUB_ITEM_DELIMITER)
            'Remove line feeds
            ParsedItems(0) = Replace(ParsedItems(0), vbLf, "") 'Bit number
            ParsedItems(1) = Replace(ParsedItems(1), vbLf, "") 'Code Define
            ParsedItems(2) = Replace(ParsedItems(2), vbLf, "") 'Description
            'Add items to the listview
            Source_DCID_Bitfield_String = Source_DCID & "__" & ParsedItems(1) & "__" & ParsedItems(0)
            Worksheets(PROFILE_DATA_SHEET).Cells(Bitfield_Row_Index, PROFILE_DATA_SOURCE_BITFILED_COL).value = Application.Clean(Source_DCID_Bitfield_String)
            'Move to next Row
            Bitfield_Row_Index = Bitfield_Row_Index + 1
        Next index
    Else
        'Did not find the DCID, this is bad, exit with an error
        MsgBox "Unable to find a DCID from the Profile_Data source list in DCI Descriptor sheet!", vbCritical, "Unable to find DCID"
        Unload Me
    End If
    Generate_Source_Bitfields = Bitfield_Row_Index
End Function

Private Sub Refresh_Source_Bitfields()
Dim Profile_Data_Row As Integer
Dim BitMan_Data_row As Integer
Dim Cur_DCID As String

'clean the source bitfields column
    Worksheets(PROFILE_DATA_SHEET).Range( _
    Worksheets(PROFILE_DATA_SHEET).Cells(PROFILE_DATA_START_ROW, PROFILE_DATA_SOURCE_BITFILED_COL), _
    Worksheets(PROFILE_DATA_SHEET).Cells(Worksheets(PROFILE_DATA_SHEET).Cells(PROFILE_DATA_START_ROW, PROFILE_DATA_SOURCE_BITFILED_COL).End(xlDown).row _
                                            , PROFILE_DATA_SOURCE_BITFILED_COL)).value = ""
                                            
    Profile_Data_Row = PROFILE_DATA_START_ROW
    BitMan_Data_row = PROFILE_DATA_START_ROW
    Application.Calculation = xlCalculationManual
    Do While True
        Cur_DCID = Trim(Worksheets(PROFILE_DATA_SHEET).Cells(Profile_Data_Row, PROFILE_DATA_SOURCE_DCID_COL).value)
        If (Cur_DCID <> "") Then
            BitMan_Data_row = Generate_Source_Bitfields(BitMan_Data_row, Cur_DCID)
            Profile_Data_Row = Profile_Data_Row + 1
        Else
            Exit Do
        End If
    Loop
    Application.Calculation = xlCalculationAutomatic
End Sub

Private Function Profile_Data_Find_Sink_Param(ByVal DCID As String) As Integer
Dim Profile_Data_Row As Integer
    Profile_Data_Row = PROFILE_DATA_START_ROW
    Do While True
        If (DCID = Trim(Worksheets(PROFILE_DATA_SHEET).Cells(Profile_Data_Row, PROFILE_DATA_SINK_DCID_COL).value)) Then
            Profile_Data_Find_Sink_Param = Profile_Data_Row
            Exit Do
        ElseIf ("" = Trim(Worksheets(PROFILE_DATA_SHEET).Cells(Profile_Data_Row, PROFILE_DATA_SINK_DCID_COL).value)) Then
            Profile_Data_Find_Sink_Param = -1
            Exit Do
        Else
            Profile_Data_Row = Profile_Data_Row + 1
        End If
    Loop
End Function


''''''''''BitMan Sheet Helpers''''''''''

Private Function Generate_BitMan_Sink_Data(ByVal Bitfield_Row_Index As Integer, ByVal Source_DCID As String)
Dim DCI_Desc_Row_Index As Integer
Dim ListItems() As String
Dim ParsedItems() As String
Dim RawString As String
Dim Sink_BitField As String

    DCI_Desc_Row_Index = DCI_DESCRIPTORS_START_ROW
    Do While True
        If (Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(DCI_Desc_Row_Index, DCI_CODE_DEFINE_COLUMN).value) = Source_DCID) Then
            'DCID found, parse the Bitfields and create the profile data source bitfields
            RawString = Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(DCI_Desc_Row_Index, BITFIELDS_COL).value)
            ListItems = Split(RawString, ITEM_DELIMITER)
            'First item denotes the number of bitfields for this parameter
            'this is for display only
            For index = 1 To UBound(ListItems) Step 1
                ParsedItems = Split(ListItems(index), SUB_ITEM_DELIMITER)
                'Remove line feeds
                ParsedItems(0) = Replace(ParsedItems(0), vbLf, "") 'Bit number
                ParsedItems(1) = Replace(ParsedItems(1), vbLf, "") 'Code Define
                ParsedItems(2) = Replace(ParsedItems(2), vbLf, "") 'Description
                'Add items to the listview
                Sink_BitField = ParsedItems(1) & "__" & ParsedItems(0)
                Worksheets(BITMAN_SHEET).Cells(Bitfield_Row_Index, BITMAN_SINK_DCI_PARAMS_COL).value = Source_DCID
                Worksheets(BITMAN_SHEET).Cells(Bitfield_Row_Index, BITMAN_SINK_BITFIELDS_COL).value = Sink_BitField
                'Move to next Row
                Bitfield_Row_Index = Bitfield_Row_Index + 1
            Next index
            Exit Do
        ElseIf (Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(DCI_Desc_Row_Index, DCI_CODE_DEFINE_COLUMN).value) = "") Then
            'Reached the end but did not find the DCID, this is bad, exit with an error
            MsgBox "Unable to find a DCID from the Profile_Data source list in DCI Descriptor sheet!", vbCritical, "Unable to find DCID"
            Unload Me
        Else
            DCI_Desc_Row_Index = DCI_Desc_Row_Index + 1
        End If
        
    Loop
End Function

Private Function Bitman_Remove_Sink_Param_By_DCID(ByVal DCID As String)
Dim BitMan_Row_Index As Integer
    BitMan_Row_Index = BITMAN_DATA_START_ROW
    Do While True
        If (DCID = Trim(Worksheets(BITMAN_SHEET).Cells(BitMan_Row_Index, BITMAN_SINK_DCI_PARAMS_COL).value)) Then
            Worksheets(BITMAN_SHEET).Rows(BitMan_Row_Index).Delete Shift:=xlUp
        ElseIf ("" = Trim(Worksheets(BITMAN_SHEET).Cells(BitMan_Row_Index, BITMAN_SINK_DCI_PARAMS_COL).value)) Then
            Exit Do
        Else
            BitMan_Row_Index = BitMan_Row_Index + 1
        End If
    Loop
End Function

Private Function Bitman_Remove_Sink_Bitfield(ByVal DCID As String, ByVal Bitfield As String)
Dim BitMan_Row_Index As Integer
Dim Temp_Bitfield() As String
    BitMan_Row_Index = BITMAN_DATA_START_ROW
    Do While True
        Temp_Bitfield = Split(Trim(Worksheets(BITMAN_SHEET).Cells(BitMan_Row_Index, BITMAN_SINK_BITFIELDS_COL).value), "__")
        If (DCID = Trim(Worksheets(BITMAN_SHEET).Cells(BitMan_Row_Index, BITMAN_SINK_DCI_PARAMS_COL).value) And _
        Bitfield = Temp_Bitfield(0)) Then
            Worksheets(BITMAN_SHEET).Rows(BitMan_Row_Index).Delete Shift:=xlUp
            'PushLog ("Deleted Sink DCID " & DCID & " Bitfield " & Bitfield)
            Exit Do
        ElseIf ("" = Trim(Worksheets(BITMAN_SHEET).Cells(BitMan_Row_Index, BITMAN_SINK_DCI_PARAMS_COL).value)) Then
            MsgBox "Unable to find Bitfield from Bitman sink list to delete!", vbCritical, "Unable to find Bitfield"
            Exit Do
        Else
            BitMan_Row_Index = BitMan_Row_Index + 1
        End If
    Loop
End Function

Private Function Bitman_Insert_Sink_Bitfield(ByVal DCID As String, ByVal Bitfield As String)
Dim Row_Index As Integer
Row_Index = BITMAN_DATA_START_ROW
 Do While True
    If (DCID = Trim(Worksheets(BITMAN_SHEET).Cells(Row_Index, BITMAN_SINK_DCI_PARAMS_COL).value)) Then
        'Insert a row
        Row_Index = Row_Index + 1
        Worksheets(BITMAN_SHEET).Rows(Row_Index).Insert Shift:=xlDown
        Worksheets(BITMAN_SHEET).Cells(Row_Index, BITMAN_SINK_DCI_PARAMS_COL).value = DCID
        Worksheets(BITMAN_SHEET).Cells(Row_Index, BITMAN_SINK_BITFIELDS_COL).value = Bitfield
        Exit Do
    ElseIf ("" = Trim(Worksheets(BITMAN_SHEET).Cells(Row_Index, BITMAN_SINK_DCI_PARAMS_COL).value)) Then
        MsgBox "Unable to find DCI parameter in the bitman sheet to insert!", vbCritical, "Unable to find DCID"
        Exit Do
    Else
        Row_Index = Row_Index + 1
    End If
 Loop

End Function

Private Function Bitman_Remove_Sink_Param(ByVal Row_Index As Integer)
Dim DCI_Param As String
 DCI_Param = Trim(Worksheets(BITMAN_SHEET).Cells(Row_Index, BITMAN_SINK_DCI_PARAMS_COL).value)
 Do While True
    If (DCI_Param = Trim(Worksheets(BITMAN_SHEET).Cells(Row_Index, BITMAN_SINK_DCI_PARAMS_COL).value)) Then
        'Delete the row
        Worksheets(BITMAN_SHEET).Rows(Row_Index).Delete Shift:=xlUp
    Else
        'Reached the end of the sheet or deleted all the Sink params
        Exit Do
    End If
 Loop

End Function

Public Sub Populate_Source_Validation()
Dim List_Index As Integer
Dim Source_BitFields_List() As String
Dim Validation_List As String
Dim cur_bitfield As String
Dim Profile_Data_Row As Integer

Dim BitMan_Cur_Row As Integer
Dim BitMan_Cur_Col As Integer
Dim Temp_Selection As String
Dim Search_Result As Variant

Dim Total_Sink_Bitfields As Integer
Dim Last_Row As Integer

Application.Calculation = xlCalculationManual

Worksheets(PROFILE_DATA_SHEET).Cells(PROFILE_DATA_START_ROW, PROFILE_DATA_SOURCE_VALIDATION_COL).value = Chr(160) 'Adding an empty item to be able to not map anything

'Get the total number of source bitfields
Profile_Data_Row = PROFILE_DATA_START_ROW
Last_Row = Worksheets(PROFILE_DATA_SHEET).Cells(PROFILE_DATA_START_ROW, PROFILE_DATA_SOURCE_VALIDATION_COL).End(xlDown).row
Total_Sink_Bitfields = Last_Row - Profile_Data_Row + 1

ReDim Source_BitFields_List(Total_Sink_Bitfields)

'Clear out old validation data
Worksheets(PROFILE_DATA_SHEET).Range( _
Worksheets(PROFILE_DATA_SHEET).Cells(PROFILE_DATA_START_ROW + 1, PROFILE_DATA_SOURCE_VALIDATION_COL), _
Worksheets(PROFILE_DATA_SHEET).Cells(Last_Row, PROFILE_DATA_SOURCE_VALIDATION_COL)).value = ""

Profile_Data_Row = PROFILE_DATA_START_ROW
List_Index = 0

Do While True
    cur_bitfield = Trim(Worksheets(PROFILE_DATA_SHEET).Cells(Profile_Data_Row, PROFILE_DATA_SOURCE_BITFILED_COL).value)
    If ("" <> cur_bitfield) Then
        Source_BitFields_List(List_Index) = cur_bitfield
        Worksheets(PROFILE_DATA_SHEET).Cells(Profile_Data_Row + 1, PROFILE_DATA_SOURCE_VALIDATION_COL).value = cur_bitfield
        Profile_Data_Row = Profile_Data_Row + 1
        List_Index = List_Index + 1
    Else
        Exit Do
    End If
Loop
Validation_List = "=" & PROFILE_DATA_SHEET & "!" & "$" & PROFILE_DATA_SOURCE_VALIDATION_COL & "$" & PROFILE_DATA_START_ROW & ":" & "$" & PROFILE_DATA_SOURCE_VALIDATION_COL & "$" & Profile_Data_Row

BitMan_Cur_Col = BITMAN_DATA_START_COL

Do While True
    If ("" <> Trim(Worksheets(BITMAN_SHEET).Cells(BITMAN_NAME_ROW, BitMan_Cur_Col).value)) Then
        'There is a valid profile
        BitMan_Cur_Row = BITMAN_DATA_START_ROW
        Do While True
            If ("" <> Trim(Worksheets(BITMAN_SHEET).Cells(BitMan_Cur_Row, BITMAN_SINK_DCI_PARAMS_COL).value)) Then
                Temp_Selection = Trim(Worksheets(BITMAN_SHEET).Cells(BitMan_Cur_Row, BitMan_Cur_Col).value)
                'Add the validation
                 With Worksheets(BITMAN_SHEET).Cells(BitMan_Cur_Row, BitMan_Cur_Col).Validation
                    .Delete
                    .Add Type:=xlValidateList, AlertStyle:=xlValidAlertStop, Operator:= _
                     xlBetween, Formula1:=Validation_List
                    .IgnoreBlank = False
                    .InCellDropdown = True
                    .InputTitle = ""
                    .ErrorTitle = ""
                    .InputMessage = ""
                    .ErrorMessage = ""
                    .ShowInput = True
                    .ShowError = True
                End With
                If IsError(Application.Match(Temp_Selection, Source_BitFields_List, 0)) < 0 And Temp_Selection <> Chr(160) Then
                    Worksheets(BITMAN_SHEET).Cells(BitMan_Cur_Row, BitMan_Cur_Col).value = " "
                    PushLog ("Warning: Removing linkage at Row,Col " & BitMan_Cur_Row & "," & BitMan_Cur_Col)
                Else
                    Worksheets(BITMAN_SHEET).Cells(BitMan_Cur_Row, BitMan_Cur_Col).value = Temp_Selection
                End If
            Else
                Exit Do
            End If
            BitMan_Cur_Row = BitMan_Cur_Row + 1
        Loop
    Else
        Exit Do
    End If
    BitMan_Cur_Col = BitMan_Cur_Col + 1
Loop

Application.Calculation = xlCalculationAutomatic
    
End Sub

Private Sub Refresh_Bitman_Sink_Params()
Dim Profile_Data_Row As Integer
Dim Bitman_Search_Row As Integer
Dim Cur_DCID As String

    Profile_Data_Row = PROFILE_DATA_START_ROW
    
    Do While True
        Cur_DCID = Trim(Worksheets(PROFILE_DATA_SHEET).Cells(Profile_Data_Row, PROFILE_DATA_SINK_DCID_COL).value)
        If (Cur_DCID <> "") Then
            'Check if the parameter already exists in the BitMan sheet
            Bitman_Search_Row = BITMAN_DATA_START_ROW
            Do While True
                If (Trim(Worksheets(BITMAN_SHEET).Cells(Bitman_Search_Row, BITMAN_SINK_DCI_PARAMS_COL).value) = Cur_DCID) Then
                    'the parameter already exists, we don't have to do anything, so exit the loop
                    Exit Do
                ElseIf (Trim(Worksheets(BITMAN_SHEET).Cells(Bitman_Search_Row, BITMAN_SINK_DCI_PARAMS_COL).value) = "") Then
                    'Reached the end but did not find the parameter, so add it
                    Call Generate_BitMan_Sink_Data(Bitman_Search_Row, Cur_DCID)
                    Exit Do
                Else
                    'Move to the next row
                    Bitman_Search_Row = Bitman_Search_Row + 1
                End If
            Loop
        Else
            Exit Do
        End If
        Profile_Data_Row = Profile_Data_Row + 1
    Loop
    
    'Now do the opposite. Remove any sink DCIDs in the bitman sheet that are unused
    Bitman_Search_Row = BITMAN_DATA_START_ROW
    Do While True
        Cur_DCID = Trim(Worksheets(BITMAN_SHEET).Cells(Bitman_Search_Row, BITMAN_SINK_DCI_PARAMS_COL).value)
        If (Cur_DCID <> "") Then
            If (-1 = Profile_Data_Find_Sink_Param(Cur_DCID)) Then
                Bitman_Remove_Sink_Param (Bitman_Search_Row)
            Else
                'Skip the current parameter
                Do While True
                    Bitman_Search_Row = Bitman_Search_Row + 1
                    If (Cur_DCID <> Trim(Worksheets(BITMAN_SHEET).Cells(Bitman_Search_Row, BITMAN_SINK_DCI_PARAMS_COL).value)) Then
                        Exit Do
                    End If
                Loop
            End If
        Else
            Exit Do
        End If
    Loop
End Sub

Public Sub Sync_With_DCI_Desc_sheet()
    Global_Log = ""
    Call sync_Params_With_DCI_Descriptors_Sheet
    Call sync_Bitfields_With_DCI_Descriptors_Sheet
    Call Refresh_Source_Bitfields
    Call Populate_Source_Validation
    If ("" <> Global_Log) Then
        Call LogWindow.ShowLog(Global_Log, "BitMan sync log")
    End If
End Sub

Private Sub sync_Bitfields_With_DCI_Descriptors_Sheet()
'Common
Dim Cur_DCID As String
Dim index As Integer
Dim Index1 As Integer
Dim Search_Result As Variant
Dim Temp_Bitlist() As String

'DCI Descriptor sheet
Dim DCI_Desc_BitList() As String
Dim ListItems() As String
Dim ParsedItems() As String
Dim RawString As String
Dim DCI_Desc_Row_Idx As Integer

'Bitman Sheet
Dim Bitman_BitList() As String
Dim Bitman_Row_Idx As Integer
Dim Bitman_Row_Temp_Idx As Integer
Dim Bitman_Cur_Bitfields_Len As Integer

Bitman_Row_Idx = BITMAN_DATA_START_ROW
Refresh_The_Bitman_Source_Bitlist = False
Do While True
    Cur_DCID = Trim(Worksheets(BITMAN_SHEET).Cells(Bitman_Row_Idx, BITMAN_SINK_DCI_PARAMS_COL).value)
    If ("" <> Cur_DCID) Then
        Bitman_Row_Temp_Idx = Bitman_Row_Idx
        Bitman_Cur_Bitfields_Len = 0
        'Determine the total number of bitfields in the Bitman sheet
        Do While True
            If (Cur_DCID = Trim(Worksheets(BITMAN_SHEET).Cells(Bitman_Row_Temp_Idx, BITMAN_SINK_DCI_PARAMS_COL).value)) Then
                Bitman_Cur_Bitfields_Len = Bitman_Cur_Bitfields_Len + 1
                Bitman_Row_Temp_Idx = Bitman_Row_Temp_Idx + 1
            Else
                Exit Do
            End If
        Loop
        
        Bitman_Row_Temp_Idx = Bitman_Row_Idx
        ReDim Bitman_BitList(Bitman_Cur_Bitfields_Len)
        'Add the bitfields to the list
        For index = LBound(Bitman_BitList) To UBound(Bitman_BitList) - 1 Step 1
            Bitman_BitList(index) = Trim(Worksheets(BITMAN_SHEET).Cells(Bitman_Row_Temp_Idx, BITMAN_SINK_BITFIELDS_COL).value)
            Temp_Bitlist = Split(Bitman_BitList(index), "__")
            Bitman_BitList(index) = Temp_Bitlist(0)
            Bitman_Row_Temp_Idx = Bitman_Row_Temp_Idx + 1
        Next index
        
        DCI_Desc_Row_Idx = DCI_Descriptor_Find_DCID(Cur_DCID)
        If (0 = DCI_Desc_Row_Idx) Then
            MsgBox "Unable to find a DCID from the Bitman sink list in DCI Descriptor sheet!", vbCritical, "Unable to find DCID"
        End If
        
        'Moving to DCI Descriptor sheet to get the bitfields
        RawString = Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(DCI_Desc_Row_Idx, BITFIELDS_COL).value)
        ListItems = Split(RawString, ITEM_DELIMITER)
        ReDim DCI_Desc_BitList(CInt(ListItems(0)))
        'First item denotes the number of bitfields for this parameter
        'this is for display only
        For index = 1 To UBound(ListItems) Step 1
            ParsedItems = Split(ListItems(index), SUB_ITEM_DELIMITER)
            'Remove line feeds
            DCI_Desc_BitList(index - 1) = Replace(ParsedItems(1), vbLf, "") 'Code Define
        Next index
        
        'Check if all the bitfields in the Bitman Sheet is present in the DCI Descriptor sheet
        For index = 0 To UBound(Bitman_BitList) Step 1
            If IsError(Application.Match(Bitman_BitList(index), DCI_Desc_BitList, 0)) Then
                'Remove the bitfield row from the Bitfields Sheet
                Call Bitman_Remove_Sink_Bitfield(Cur_DCID, Bitman_BitList(index))
                PushLog ("Warning Removing bitfield " & Bitman_BitList(index) & " for Sink DCID " & Cur_DCID)
            End If
            
        Next index
        
        'Check if all the bitfields in the DCI Descriptor sheet is present in the Bitman sheet
        For index = 0 To UBound(DCI_Desc_BitList) Step 1
            If IsError(Application.Match(DCI_Desc_BitList(index), Bitman_BitList, 0)) Then
                'Find out the bitfield number
                For Index1 = 1 To UBound(ListItems) Step 1
                    ParsedItems = Split(ListItems(Index1), SUB_ITEM_DELIMITER)
                    'Remove line feeds
                    ParsedItems(0) = Replace(ParsedItems(0), vbLf, "") 'Code Define
                    ParsedItems(1) = Replace(ParsedItems(1), vbLf, "") 'Code Define
                    If (ParsedItems(1) = DCI_Desc_BitList(index)) Then
                        DCI_Desc_BitList(index) = DCI_Desc_BitList(index) & "__" & ParsedItems(0)
                        Exit For
                    End If
                Next Index1
                'Insert the bitfield row in the Bitfields Sheet
                Call Bitman_Insert_Sink_Bitfield(Cur_DCID, DCI_Desc_BitList(index))
                PushLog ("Added new bitfield " & DCI_Desc_BitList(index) & " for Source DCID " & Cur_DCID)
            End If
        Next index
        
        'Done with Current Sink parameter, skip all the bitfields
        Do While True
            If (Cur_DCID = Trim(Worksheets(BITMAN_SHEET).Cells(Bitman_Row_Idx, BITMAN_SINK_DCI_PARAMS_COL).value)) Then
                Bitman_Row_Idx = Bitman_Row_Idx + 1
            Else
                Exit Do
            End If
        Loop
    Else
        Exit Do
    End If
Loop

End Sub

Private Function DCI_Descriptor_Get_Bitfield_Count(ByVal DCID As String) As Integer
Dim DCI_Desc_Row_Idx As Integer
Dim RawString As String
Dim ListItems() As String
DCI_Desc_Row_Idx = DCI_DESCRIPTORS_START_ROW
    Do While True
        If (DCID = Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(DCI_Desc_Row_Idx, DCI_CODE_DEFINE_COLUMN).value)) Then
            'Found it
            RawString = Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(DCI_Desc_Row_Idx, BITFIELDS_COL).value)
            ListItems = Split(RawString, ITEM_DELIMITER)
            If ("" = RawString) Then
                DCI_Descriptor_Get_Bitfield_Count = 0
            Else
                DCI_Descriptor_Get_Bitfield_Count = CInt(ListItems(0))
            End If
            Exit Do
        ElseIf ("" = Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(DCI_Desc_Row_Idx, DCI_CODE_DEFINE_COLUMN).value)) Then
            'We hit the end but did not find the DCI Parameter
            DCI_Descriptor_Get_Bitfield_Count = 0
            Exit Do
        Else
            'Move to the row
            DCI_Desc_Row_Idx = DCI_Desc_Row_Idx + 1
        End If
    Loop
End Function

Private Function DCI_Descriptor_Find_DCID(ByVal DCID As String) As Integer
Dim DCI_Desc_Row_Idx As Integer
DCI_Desc_Row_Idx = DCI_DESCRIPTORS_START_ROW
    Do While True
        If (DCID = Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(DCI_Desc_Row_Idx, DCI_CODE_DEFINE_COLUMN).value)) Then
            'Found it
            DCI_Descriptor_Find_DCID = DCI_Desc_Row_Idx
            Exit Do
        ElseIf ("" = Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(DCI_Desc_Row_Idx, DCI_CODE_DEFINE_COLUMN).value)) Then
            'We hit the end but did not find the DCI Parameter
            DCI_Descriptor_Find_DCID = 0
            Exit Do
        Else
            'Move to the row
            DCI_Desc_Row_Idx = DCI_Desc_Row_Idx + 1
        End If
    Loop
End Function

'This method checks to see if all the source and sink params in the __Profile_Data sheet & Bitman Sheet are
'present in the DCI Descriptors sheet. If not delete them
Private Sub sync_Params_With_DCI_Descriptors_Sheet()
Dim Cur_DCID As String
Dim Cur_Profile_Data_row As Integer


'doing the sinks first
Cur_Profile_Data_row = PROFILE_DATA_START_ROW
 Do While True
    Cur_DCID = Trim(Worksheets(PROFILE_DATA_SHEET).Cells(Cur_Profile_Data_row, PROFILE_DATA_SINK_DCID_COL).value)
    If ("" <> Cur_DCID) Then
        If ((0 = DCI_Descriptor_Find_DCID(Cur_DCID)) Or (0 = DCI_Descriptor_Get_Bitfield_Count(Cur_DCID))) Then
            'Delete the parameter
            Worksheets(PROFILE_DATA_SHEET).Cells(Cur_Profile_Data_row, PROFILE_DATA_SINK_DCID_COL).Delete Shift:=xlUp
            Bitman_Remove_Sink_Param_By_DCID (Cur_DCID)
            Call PushLog("Removing Sink DCID " & Cur_DCID)
        Else
            'move on
            Cur_Profile_Data_row = Cur_Profile_Data_row + 1
        End If
    Else
        Exit Do
    End If
Loop

'now the source
Cur_Profile_Data_row = PROFILE_DATA_START_ROW
Source_List_Changed = False
 Do While True
    Cur_DCID = Trim(Worksheets(PROFILE_DATA_SHEET).Cells(Cur_Profile_Data_row, PROFILE_DATA_SOURCE_DCID_COL).value)
    If ("" <> Cur_DCID) Then
        If ((0 = DCI_Descriptor_Find_DCID(Cur_DCID)) Or (0 = DCI_Descriptor_Get_Bitfield_Count(Cur_DCID))) Then
            'Delete the parameter
            Worksheets(PROFILE_DATA_SHEET).Cells(Cur_Profile_Data_row, PROFILE_DATA_SOURCE_DCID_COL).Delete Shift:=xlUp
            Call PushLog("Removing Source DCID " & Cur_DCID)
        Else
            'move on
            Cur_Profile_Data_row = Cur_Profile_Data_row + 1
        End If
    Else
        Exit Do
    End If
Loop

End Sub

Private Sub PushLog(ByVal LogString As String)
    Global_Log = Global_Log & LogString & vbLf
End Sub

Private Sub UserForm_Initialize()
    Call Source_Init
    Call Sink_Init
End Sub


Sub CodeGen()

    Application.Calculation = xlCalculationManual

    Dim fs As FileSystemObject
    Dim base_file_path As String
    Dim cpp_file As TextStream
    Dim h_file As TextStream
    Dim Package_Name As String
    Dim Src_Dcid_Len As Integer
    Dim Snk_Dcid_Len As Integer
    Dim Map_Len As Integer
    
    Dim Cur_Profile_Col As Integer
    Dim Cur_Prof_Name As String
    
    base_file_path = ActiveWorkbook.path & "\" & Worksheets(BITMAN_SHEET).Cells(FOLDER_PATH_ROW, FOLDER_PATH_COL).text
    
    Set fs = New FileSystemObject
    
    Package_Name = Worksheets(BITMAN_SHEET).Cells(PACKAGE_NAME_ROW, PACKAGE_NAME_COL).text
    
    'Create the bitfields file
    Set h_file = fs.CreateTextFile(base_file_path & Package_Name & "_" & BITMAN_BITFIELDS_FILE_NAME & H_FILE_EXTN, True)
    Set cpp_file = fs.CreateTextFile(base_file_path & Package_Name & "_" & BITMAN_BITFIELDS_FILE_NAME & CPP_FILE_EXTN, True)
    
    h_file.WriteLine (PND_IFNDEF & UCase(Package_Name & "_" & BITMAN_BITFIELDS_FILE_NAME))
    h_file.WriteLine (PND_DEFINE & UCase(Package_Name & "_" & BITMAN_BITFIELDS_FILE_NAME))
    h_file.WriteLine (vbNewLine & BITMAN_DT_INCLUDE)
    h_file.WriteLine ("namespace " & NAMESPACE & vbNewLine & "{")
    h_file.WriteLine (vbNewLine & vbTab & "/****************************** Source DCID Bitfields ******************************/")
    h_file.WriteLine (vbTab & "class " & Package_Name & "_BitMan_Source" & vbNewLine & vbTab & "{" & vbNewLine & vbTab & "public:")
    
    cpp_file.WriteLine ("#include """ & Package_Name & "_" & BITMAN_BITFIELDS_FILE_NAME & H_FILE_EXTN & """")
    cpp_file.WriteLine ("namespace " & NAMESPACE & vbNewLine & "{")
    cpp_file.WriteLine (vbNewLine & vbTab & "/****************************** Source DCID Bitfields ******************************/")
    Call File_Generate_Bitlist(h_file, cpp_file, Package_Name & "_BitMan_Source", source)
    h_file.WriteLine (vbTab & "};")
    h_file.WriteLine (vbNewLine & vbTab & "/************************** End Of Source DCID Bitfields ***************************/")
    cpp_file.WriteLine (vbTab & "/************************** End Of Source DCID Bitfields ***************************/")
    h_file.WriteLine (vbNewLine & vbTab & "/******************************* Sink DCID Bitfields *******************************/")
    cpp_file.WriteLine (vbNewLine & vbTab & "/******************************* Sink DCID Bitfields *******************************/")
    h_file.WriteLine (vbTab & "class " & Package_Name & "_BitMan_Sink" & vbNewLine & vbTab & "{" & vbNewLine & vbTab & vbTab & "public:")
    Call File_Generate_Bitlist(h_file, cpp_file, Package_Name & "_BitMan_Sink", SINK)
    h_file.WriteLine (vbTab & "};")
    h_file.WriteLine (vbNewLine & vbTab & "/*************************** End Of Sink DCID Bitfields ****************************/")
    h_file.WriteLine ("}")
    cpp_file.WriteLine (vbTab & "/*************************** End Of Sink DCID Bitfields ****************************/")
    cpp_file.WriteLine ("}")
    h_file.WriteLine (PND_ENDIF)
    h_file.Close
    cpp_file.Close
    
    'Create the base file
    Set h_file = fs.CreateTextFile(base_file_path & Package_Name & "_" & BITMAN_BASE_FILE_NAME & H_FILE_EXTN, True)
    h_file.WriteLine (PND_IFNDEF & UCase(Package_Name & "_" & BITMAN_BASE_FILE_NAME))
    h_file.WriteLine (PND_DEFINE & UCase(Package_Name & "_" & BITMAN_BASE_FILE_NAME))
    h_file.WriteLine (vbNewLine & BITMAN_DT_INCLUDE)
    h_file.WriteLine (vbNewLine)
    h_file.WriteLine ("namespace " & NAMESPACE & vbNewLine & "{")
    h_file.WriteLine (vbTab & "class " & Package_Name & "_" & BITMAN_BASE_FILE_NAME & vbNewLine & vbTab & "{" & vbNewLine & vbTab & "public:")
    h_file.WriteLine (vbTab & vbTab & "static const " & BITMAN_BASE_STRUCT_NAME & " s_bitman_base_structure;")
    h_file.WriteLine (vbTab & "private:")
    h_file.WriteLine (vbTab & vbTab & "static " & DCID_DATATYPE & "s_source_dcid_list[];")
    h_file.WriteLine (vbTab & vbTab & "static " & DCID_DATATYPE & "s_sink_dcid_list[];")
    h_file.WriteLine (vbTab & "};" & vbNewLine & "}")
    h_file.WriteLine (PND_ENDIF)
    h_file.Close
    
    Set cpp_file = fs.CreateTextFile(base_file_path & Package_Name & "_" & BITMAN_BASE_FILE_NAME & CPP_FILE_EXTN, True)
    cpp_file.WriteLine (vbNewLine & "#include """ & Package_Name & "_" & BITMAN_BASE_FILE_NAME & H_FILE_EXTN & """")
    cpp_file.WriteLine (DCI_DATA_INCLUDE)
    cpp_file.WriteLine ("namespace " & NAMESPACE & vbNewLine & "{")
    cpp_file.WriteLine (vbTab & DCID_DATATYPE & Package_Name & "_" & BITMAN_BASE_FILE_NAME & "::" & "s_source_dcid_list[] = " & vbNewLine & vbTab & "{")
    Src_Dcid_Len = File_Generate_Dcid_List(cpp_file, source)
    cpp_file.WriteLine (vbTab & "};")
    cpp_file.WriteLine (vbTab & DCID_DATATYPE & Package_Name & "_" & BITMAN_BASE_FILE_NAME & "::" & "s_sink_dcid_list[] = " & vbNewLine & vbTab & "{")
    Snk_Dcid_Len = File_Generate_Dcid_List(cpp_file, SINK)
    cpp_file.WriteLine (vbTab & "};")
    cpp_file.WriteLine (vbTab & "const " & BITMAN_BASE_STRUCT_NAME & " " & Package_Name & "_" & BITMAN_BASE_FILE_NAME & "::" & "s_bitman_base_structure = " & vbNewLine & vbTab & "{")
    cpp_file.WriteLine (vbTab & vbTab & Snk_Dcid_Len & "U")
    cpp_file.WriteLine (vbTab & vbTab & ",&" & Package_Name & "_" & BITMAN_BASE_FILE_NAME & "::s_sink_dcid_list[0]")
    cpp_file.WriteLine (vbTab & vbTab & "," & Src_Dcid_Len & "U")
    cpp_file.WriteLine (vbTab & vbTab & ",&" & Package_Name & "_" & BITMAN_BASE_FILE_NAME & "::s_source_dcid_list[0]")
    cpp_file.WriteLine (vbTab & "};")
    cpp_file.WriteLine ("}")
    cpp_file.Close
      
    Cur_Profile_Col = BITMAN_DATA_START_COL

    'Generate the profiles
    Do While True
        Cur_Prof_Name = Worksheets(BITMAN_SHEET).Cells(PROFILE_NAME_ROW, Cur_Profile_Col).text
        If ("" <> Cur_Prof_Name) Then
            Set h_file = fs.CreateTextFile(base_file_path & Package_Name & "_BitMan_" & Cur_Prof_Name & H_FILE_EXTN, True)
            h_file.WriteLine (PND_IFNDEF & UCase(Package_Name & "_" & Cur_Prof_Name))
            h_file.WriteLine (PND_DEFINE & UCase(Package_Name & "_" & Cur_Prof_Name))
            h_file.WriteLine (vbNewLine & BITMAN_DT_INCLUDE)
            h_file.WriteLine (vbNewLine)
            h_file.WriteLine ("namespace " & NAMESPACE & vbNewLine & "{")
            h_file.WriteLine (vbTab & "class " & Package_Name & "_BitMan_" & Cur_Prof_Name & vbNewLine & vbTab & "{" & vbNewLine & vbTab & "public:")
            h_file.WriteLine (vbTab & vbTab & "static const BitMan_Profile " & "s_profile;")
            h_file.WriteLine (vbTab & "private:")
            h_file.WriteLine (vbTab & vbTab & "static " & "const BitMan_Profile_Map " & "s_profile_map[];")
            h_file.WriteLine (vbTab & "};" & vbNewLine & "}")
            h_file.WriteLine (PND_ENDIF)
            h_file.Close
            
            Set cpp_file = fs.CreateTextFile(base_file_path & Package_Name & "_BitMan_" & Cur_Prof_Name & CPP_FILE_EXTN, True)
            cpp_file.WriteLine ("#include """ & Package_Name & "_BitMan_" & Cur_Prof_Name & H_FILE_EXTN & """")
            cpp_file.WriteLine ("#include """ & Package_Name & "_" & BITMAN_BITFIELDS_FILE_NAME & H_FILE_EXTN & """")
            cpp_file.WriteLine ("namespace " & NAMESPACE & vbNewLine & "{")
            cpp_file.WriteLine (vbTab & "const BitMan_Profile_Map " & Package_Name & "_BitMan_" & Cur_Prof_Name & "::" & "s_profile_map[] = " & vbNewLine & vbTab & "{")
            Map_Len = File_Generate_Profile_Map(cpp_file, Cur_Profile_Col, Package_Name)
            cpp_file.WriteLine (vbNewLine & vbTab & "};")
            cpp_file.WriteLine (vbNewLine & vbTab & "const BitMan_Profile " & Package_Name & "_BitMan_" & Cur_Prof_Name & "::" & "s_profile = ")
            cpp_file.WriteLine (vbTab & "{" & vbNewLine & vbTab & vbTab & "&" & Package_Name & "_BitMan_" & Cur_Prof_Name & "::s_profile_map[0]")
            cpp_file.WriteLine (vbTab & vbTab & "," & Map_Len & "U")
            cpp_file.WriteLine (vbTab & "};")
            cpp_file.WriteLine ("}")
            cpp_file.Close
        Else
            Exit Do
        End If
        'move on to the the next profile
        Cur_Profile_Col = Cur_Profile_Col + 1
    Loop
    
    Application.Calculation = xlCalculationAutomatic

End Sub

Function File_Generate_Profile_Map(ByVal file_handle As TextStream, ByVal profile_col As Integer, ByVal Package As String) As Integer
Dim Cur_Profile_Row As Integer
Dim First_Item As Boolean
Dim ParsedItems() As String
Dim CurSourceBitfield As String
Dim MapLength As Integer

Cur_Profile_Row = BITMAN_DATA_START_ROW
First_Item = True
MapLength = 0

    Do While True
        If ("" <> Worksheets(BITMAN_SHEET).Cells(Cur_Profile_Row, BITMAN_SINK_DCI_PARAMS_COL)) Then
            CurSourceBitfield = Trim(Worksheets(BITMAN_SHEET).Cells(Cur_Profile_Row, profile_col))
            CurSourceBitfield = Replace(CurSourceBitfield, Chr(160), "")
            If ("" <> CurSourceBitfield) Then
                If (First_Item = False) Then
                    file_handle.WriteLine (",")
                Else
                    file_handle.WriteLine ("")
                    First_Item = False
                End If
                ParsedItems = Split(CurSourceBitfield, "__")
                file_handle.Write (vbTab & vbTab & "{ " & Package & "_BitMan_Source" & "::" & ParsedItems(0) & "::" & ParsedItems(1) & "__" & ParsedItems(2))
                file_handle.Write (vbTab & vbTab & "," & Package & "_BitMan_Sink" & "::" & Worksheets(BITMAN_SHEET).Cells(Cur_Profile_Row, BITMAN_SINK_DCI_PARAMS_COL) & "::" & Worksheets(BITMAN_SHEET).Cells(Cur_Profile_Row, BITMAN_SINK_BITFIELDS_COL) & "}")
                MapLength = MapLength + 1
            End If
        Else
            Exit Do
        End If
        Cur_Profile_Row = Cur_Profile_Row + 1
    Loop
    File_Generate_Profile_Map = MapLength
End Function

Function File_Generate_Dcid_List(ByVal file_handle As TextStream, ByVal source_or_sink As Integer) As Integer
Dim Cur_Profile_Data_row As Integer
Dim Cur_Dci_Desc_Dcid_Row As Integer
Dim totalLength As Integer
Dim index As Integer
Dim First_Item As Boolean
Dim List_Len As Integer

Cur_Profile_Data_row = PROFILE_DATA_START_ROW
First_Item = True
List_Len = 0

 Do While True
    If (source_or_sink = source) Then
        Cur_DCID = Trim(Worksheets(PROFILE_DATA_SHEET).Cells(Cur_Profile_Data_row, PROFILE_DATA_SOURCE_DCID_COL).value)
    Else
        Cur_DCID = Trim(Worksheets(PROFILE_DATA_SHEET).Cells(Cur_Profile_Data_row, PROFILE_DATA_SINK_DCID_COL).value)
    End If
    If ("" <> Cur_DCID) Then
        Cur_Dci_Desc_Dcid_Row = DCI_Descriptor_Find_DCID(Cur_DCID)
        If (0 <> Cur_Dci_Desc_Dcid_Row) Then
            totalLength = Worksheets(DCI_DESCRIPTORS_SHEET).Cells(Cur_Dci_Desc_Dcid_Row, TOTALLENGTH_COL).value
            For index = 1 To totalLength Step 1
                If (First_Item = True) Then
                    file_handle.WriteLine (vbTab & vbTab & Worksheets(DCI_DESCRIPTORS_SHEET). _
                            Cells(Cur_Dci_Desc_Dcid_Row, DCI_CODE_DEFINE_COLUMN).value & "_DCID")
                    First_Item = False
                Else
                    file_handle.WriteLine (vbTab & vbTab & "," & Worksheets(DCI_DESCRIPTORS_SHEET). _
                            Cells(Cur_Dci_Desc_Dcid_Row, DCI_CODE_DEFINE_COLUMN).value & "_DCID")
                End If
                List_Len = List_Len + 1
            Next index
        Else
            MsgBox "Unable to find a DCID from the Profile_Data source list in DCI Descriptor sheet!", vbCritical, "Unable to find DCID"
        End If
        'move on
        Cur_Profile_Data_row = Cur_Profile_Data_row + 1
    Else
        Exit Do
    End If
Loop
File_Generate_Dcid_List = List_Len
End Function

Function File_Generate_Bitlist(ByVal h_file As TextStream, ByVal cpp_file As TextStream, ByVal MasterClass As String, ByVal source_or_sink As Integer)
Dim Cur_Profile_Data_row As Integer
Dim Cur_Dci_Desc_Dcid_Row As Integer

Dim totalLength As Integer
Dim RawString As String
Dim ListItems() As String
Dim ParsedItems() As String

Dim dcid_found As Boolean
Dim BitNumber As Integer
Dim index As Integer
Dim rolling_index As Integer
Dim prefix As String
Dim Package_Name As String

rolling_index = 0
Package_Name = Worksheets(BITMAN_SHEET).Cells(PACKAGE_NAME_ROW, PACKAGE_NAME_COL).text

 Cur_Profile_Data_row = PROFILE_DATA_START_ROW
 Do While True
    If (source_or_sink = source) Then
        Cur_DCID = Trim(Worksheets(PROFILE_DATA_SHEET).Cells(Cur_Profile_Data_row, PROFILE_DATA_SOURCE_DCID_COL).value)
    Else
        Cur_DCID = Trim(Worksheets(PROFILE_DATA_SHEET).Cells(Cur_Profile_Data_row, PROFILE_DATA_SINK_DCID_COL).value)
    End If
    If ("" <> Cur_DCID) Then
        Cur_Dci_Desc_Dcid_Row = DCI_Descriptor_Find_DCID(Cur_DCID)
        If (0 <> Cur_Dci_Desc_Dcid_Row) Then
            RawString = Worksheets(DCI_DESCRIPTORS_SHEET).Cells(Cur_Dci_Desc_Dcid_Row, BITFIELDS_COL).value
            totalLength = Worksheets(DCI_DESCRIPTORS_SHEET).Cells(Cur_Dci_Desc_Dcid_Row, TOTALLENGTH_COL).value
            ListItems = Split(RawString, ITEM_DELIMITER)
            h_file.WriteLine (vbTab & vbTab & "class " & Cur_DCID & vbNewLine & vbTab & vbTab & "{" & vbNewLine & vbTab & vbTab & "public:")
            For BitNumber = 0 To (totalLength * BITS_IN_BYTE) - 1 Step 1
                dcid_found = False
                For index = 1 To UBound(ListItems) Step 1
                    ParsedItems = Split(ListItems(index), SUB_ITEM_DELIMITER)
                    If BitNumber = CInt(ParsedItems(0)) Then
                        h_file.WriteLine (vbTab & vbTab & vbTab & "static " & BITFIELDS_DATATYPE & ParsedItems(1) & "__" & BitNumber & ";")
                        cpp_file.WriteLine (vbTab & BITFIELDS_DATATYPE & MasterClass & "::" & Cur_DCID & "::" & ParsedItems(1) & "__" & BitNumber & " = " & rolling_index & "U;")
                        dcid_found = True
                        Exit For
                    End If
                Next index
                If (dcid_found = False) Then
                    h_file.WriteLine (vbTab & vbTab & vbTab & "static " & BITFIELDS_DATATYPE & "BIT__" & BitNumber & ";")
                    cpp_file.WriteLine (vbTab & BITFIELDS_DATATYPE & MasterClass & "::" & Cur_DCID & "::" & "BIT__" & BitNumber & " = " & rolling_index & "U;")
                End If
                rolling_index = rolling_index + 1
            Next BitNumber
        Else
            MsgBox "Unable to find a DCID from the Profile_Data source list in DCI Descriptor sheet!", vbCritical, "Unable to find DCID"
        End If
        'move on
        Cur_Profile_Data_row = Cur_Profile_Data_row + 1
        h_file.WriteLine (vbTab & vbTab & "};")
        cpp_file.WriteLine (vbNewLine)
    Else
        Exit Do
    End If
Loop
End Function



