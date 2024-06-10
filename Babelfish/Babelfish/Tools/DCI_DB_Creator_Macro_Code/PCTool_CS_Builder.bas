Attribute VB_Name = "PCTool_CS_Builder"
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
'*    FUNCTION HEADER
'****************************************************************
'****************************************************************
'****************************************************************
'****************************************************************
Sub Create_PCTool_Stuct_Builder()
    ' ****** Turn auto calcs off
    Application.Calculation = xlCalculationManual
    
    Dim sheet_name As String
    Dim counter
    Dim temp_default_str As String
    Dim indent
    Dim tempStr
    indent = 4
    sheet_name = "PC Tool"
    Sheets(sheet_name).Select
    
    Verify_PCTool_List
    
    Set fs = CreateObject("Scripting.FileSystemObject")
    cs_file_path = ActiveWorkbook.Path & "\" & Cells(3, 2).value
    Set DCI_DATA_C = fs.CreateTextFile(cs_file_path, True)
    xml_file_path = ActiveWorkbook.Path & "\" & Cells(4, 2).value
    Set DCI_DATA_XML = fs.CreateTextFile(xml_file_path, True)
   
    DATA_START_ROW = 9

    DATA_DESCRIPTION_COL = 1
    DATA_DEFINE_COL = 2
    DATA_PARMID_COL = 3
    DATA_USERCLASSRD_COL = 4
    DATA_USERCLASSWR_COL = 5
    DATA_DATATYPE_COL = 6
    DATA_DATATYPE_SIZE = 7
    DATA_ARRAY_COUNT_COL = 8
    DATA_TOTAL_LENGTH_COL = 9
    DATA_DEFAULT_COL = 10
    DATA_MIN_COL = 11
    DATA_MAX_COL = 12
    DATA_ENUM_COL = 13
    DATA_OBSERVER_COL = 14
    DATA_OPERATOR_COL = 15
    DATA_PLANNING_COL = 16
    DATA_EXPERIENCE_COL = 17
    DATA_CATEGORIES_COL = 18
    DATA_OFFLINE_COL = 19
    DATA_ONLINE_COL = 20
    
    READ_ONLY_VAL_ATTRIB_COL = 27
    INITIAL_VAL_ATTRIB_COL = 28
    DEFAULT_VAL_ATTRIB_COL = 29
    RW_DEFAULT_VAL_ATTRIB_COL = 30
    RANGE_ATTRIB_COL = 31
    RW_RANGE_ATTRIB_COL = 32
    CALLBACK_ATTRIB_COL = 33
    RW_LENGTH_ATTRIB_COL = 34
    ENUM_ATTRIB_COL = 35
    APP_ONLY_ATTRIB_COL = 36
    
    PATRON_RAM_VAL_WRITEABLE_COL = 53
    PATRON_INIT_VAL_READ_ACCESS_COL = 54
    PATRON_INIT_VAL_WRITEABLE_ACCESS_COL = 55
    PATRON_DEFAULT_VAL_READ_ACCESS_COL = 56
    PATRON_RANGE_VAL_ACCESS_COL = 57
    PATRON_ENUM_VAL_ACCESS_COL = 58
    
    DCI_SHORT_DESC_COL = 79
    DCI_SHORT_DESC_LEN_COL = 80
    DCI_LONG_DESC_COL = 81
    DCI_LONG_DESC_LEN_COL = 82
    DCI_UNITS_DESC_COL = 83


    ' ==============================================================
    
    
    
    '****************************************************************************************************************************
    '******     Create the resource
    '****************************************************************************************************************************
    Dim dict, dictk, dicti, rescounter, resid
    Dim desc, units
    Set dict = CreateObject("Scripting.Dictionary")
    rescounter = 1000
    counter = DATA_START_ROW
    While Cells(counter, DATA_DEFINE_COL).text <> Empty And Cells(counter, DATA_DESCRIPTION_COL).text <> Empty
        '*************************************************
        ' Look up the data from the DCI Descriptions sheet
        lookup_string = Cells(counter, DATA_DEFINE_COL).text
        Set C = Worksheets("DCI Descriptors").Range("B9:AZ1000").Find(lookup_string, LookAt:=xlWhole)
        desc = ""
        units = ""
        If Not (C Is Nothing) Then
            desc = Worksheets("DCI Descriptors").Range("A1").Cells(C.row, DCI_SHORT_DESC_COL).text
            desc = Replace(desc, Chr(13), " ")
            desc = Replace(desc, Chr(10), " ")
            units = Worksheets("DCI Descriptors").Range("A1").Cells(C.row, DCI_UNITS_DESC_COL).text
        End If
    
        If Not dict.Exists(desc) Then
            resid = "R" & rescounter
            rescounter = rescounter + 1
            dict.Add desc, resid
        End If
        
        If units <> "" And Not dict.Exists(units) Then
            resid = "R" & rescounter
            rescounter = rescounter + 1
            dict.Add units, resid
        End If
        
        counter = counter + 1
    Wend
    
    DCI_DATA_XML.WriteLine ("<?xml version=" & Chr(34) & "1.0" & Chr(34) & " encoding=" & Chr(34) & "utf-8" & Chr(34) & " ?>")
    DCI_DATA_XML.WriteLine ("<!-- Generated file -->")
    
    DCI_DATA_XML.WriteLine "<ALL>"
    indent = 4
    dictk = dict.keys              'Get the keys
    dicti = dict.Items
    For idx = 0 To dict.Count - 1 'Iterate the array
        DCI_DATA_XML.Write Space(indent)
        DCI_DATA_XML.Write "<" + dicti(idx) + ">"
        DCI_DATA_XML.Write dictk(idx)
        DCI_DATA_XML.Write "</" & dicti(idx) & ">"
        DCI_DATA_XML.WriteLine ("")
    Next
    DCI_DATA_XML.WriteLine "</ALL>"
    DCI_DATA_XML.Close

    '****************************************************************************************************************************
    '******     Output the file header
    '****************************************************************************************************************************
    DCI_DATA_C.WriteLine ("//////////////////////////////////////////////////////////////////////////////////////////////")
    DCI_DATA_C.WriteLine ("// <auto-generated>")
    DCI_DATA_C.WriteLine ("//     This code was generated by DCI DB Creator Macros.xls.")
    DCI_DATA_C.WriteLine ("//     Revision: 2.005")
    DCI_DATA_C.WriteLine ("//     " & Date & " " & Time)
    DCI_DATA_C.WriteLine ("//     Changes to this file may cause incorrect behavior and will be lost if")
    DCI_DATA_C.WriteLine ("//     the code is regenerated.")
    DCI_DATA_C.WriteLine ("// </auto-generated>")
    
    '****************************************************************************************************************************
    '******     Start the namespace
    '****************************************************************************************************************************
    DCI_DATA_C.WriteLine ("namespace Eaton.C445.Dtm.Shared")
    DCI_DATA_C.WriteLine ("{")
    
    '****************************************************************************************************************************
    '******     Start the class
    '****************************************************************************************************************************
    indent = 4
    DCI_DATA_C.WriteLine (Space(indent) & "public partial class ParameterData")
    DCI_DATA_C.WriteLine (Space(indent) & "{")
    DCI_DATA_C.WriteLine ("#region enumerations")
    indent = indent + 4
    DCI_DATA_C.WriteLine (Space(indent) & "public enum RWaccess : int")
    DCI_DATA_C.WriteLine (Space(indent) & "{")
    indent = indent + 4
    DCI_DATA_C.WriteLine (Space(indent) & "NONE,")
    DCI_DATA_C.WriteLine (Space(indent) & "READ,")
    DCI_DATA_C.WriteLine (Space(indent) & "WRITE")
    indent = indent - 4
    DCI_DATA_C.WriteLine (Space(indent) & "}")
    DCI_DATA_C.WriteLine (Space(indent) & "")
    DCI_DATA_C.WriteLine (Space(indent) & "public enum ParamID : int")
    DCI_DATA_C.WriteLine (Space(indent) & "{")

    '****************************************************************************************************************************
    '******     Output the Enumerations for the data
    '****************************************************************************************************************************
    indent = indent + 4
    DCI_DATA_C.WriteLine (Space(indent) & "/// <summary>")
    DCI_DATA_C.WriteLine (Space(indent) & "/// ParamID list sourced from BCM DCI DB Creator.xls")
    DCI_DATA_C.WriteLine (Space(indent) & "/// </summary>")
    counter = DATA_START_ROW
    While Cells(counter, DATA_DEFINE_COL).text <> Empty And Cells(counter, DATA_DESCRIPTION_COL).text <> Empty
        DCI_DATA_C.WriteLine (Space(indent) & "/// <summary>")
        DCI_DATA_C.WriteLine (Space(indent) & "/// " & Cells(counter, DATA_DESCRIPTION_COL).text)
        DCI_DATA_C.WriteLine (Space(indent) & "/// </summary>")
        DCI_DATA_C.WriteLine (Space(indent) & Cells(counter, DATA_DEFINE_COL).text & " = " & Cells(counter, DATA_PARMID_COL).text) & ","
        counter = counter + 1
    Wend
    indent = indent - 4
    DCI_DATA_C.WriteLine (Space(indent) & "}")
    DCI_DATA_C.WriteLine ("#endregion enumerations")
    DCI_DATA_C.WriteLine ("")
    
    '****************************************************************************************************************************
    '******     Create the Init function
    '****************************************************************************************************************************
    DCI_DATA_C.WriteLine (Space(indent) & "/// <summary>")
    DCI_DATA_C.WriteLine (Space(indent) & "/// Initializes the parameter list from BCM DCI DB Creator.xls")
    DCI_DATA_C.WriteLine (Space(indent) & "/// </summary>")
    DCI_DATA_C.WriteLine (Space(indent) & "public void Init()")
    DCI_DATA_C.WriteLine (Space(indent) & "{")
    indent = indent + 4
    
    '****************************************************************************************************************************
    '******     Loop through the PC Tool sheet and output the data. Lookup DCI Descriptions when needed
    '****************************************************************************************************************************
    counter = DATA_START_ROW
    While Cells(counter, DATA_DEFINE_COL).text <> Empty And Cells(counter, DATA_DESCRIPTION_COL).text <> Empty
        DCI_DATA_C.WriteLine (Space(indent) & "// " & Cells(counter, DATA_DESCRIPTION_COL).text)
        DCI_DATA_C.Write (Space(indent))
        DCI_DATA_C.WriteLine ("AddParameterItem(")
        indent = indent + 4
        DCI_DATA_C.WriteLine (Space(indent) & "ParamID." & Cells(counter, DATA_DEFINE_COL).value & ",")
        DCI_DATA_C.WriteLine (Space(indent) & "UserClass." & Cells(counter, DATA_USERCLASSRD_COL).value & ",")
        DCI_DATA_C.WriteLine (Space(indent) & "UserClass." & Cells(counter, DATA_USERCLASSWR_COL).value & ",")
            
        '********************************
        ' Define the Datatype
        DCI_DATA_C.WriteLine (Space(indent) & "DataType." & Cells(counter, DATA_DATATYPE_COL).value & ",")

        '********************************
        ' This is the datatype size.
        DCI_DATA_C.Write (Space(indent))
        If (Cells(counter, DATA_DATATYPE_SIZE).text <> Empty) Then
            DCI_DATA_C.Write (Cells(counter, DATA_DATATYPE_SIZE).text)
        Else
            Select Case Cells(counter, DATA_DATATYPE_COL).value
                Case "DCI_DTYPE_BYTE"
                DCI_DATA_C.Write ("1")
            End Select
        End If
        DCI_DATA_C.WriteLine (",")
            
        '********************************
        ' This is the array size.
        DCI_DATA_C.Write (Space(indent))
        If (Cells(counter, DATA_ARRAY_COUNT_COL).text <> Empty) Then
            DCI_DATA_C.Write (Space(indent) & Cells(counter, DATA_ARRAY_COUNT_COL).text)
        Else
            DCI_DATA_C.Write ("0")
        End If
        DCI_DATA_C.WriteLine (",")
        
        '*************************************************
        ' Look up the data from the DCI Descriptions sheet
        lookup_string = Cells(counter, DATA_DEFINE_COL).text
        Set C = Worksheets("DCI Descriptors").Range("B9:AZ1000").Find(lookup_string, LookAt:=xlWhole)
        If Not (C Is Nothing) Then
            '*************************************
            ' Short Description
            DCI_DATA_C.Write (Space(indent))
            If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, DCI_SHORT_DESC_COL).text <> Empty) Then
                tempStr = Worksheets("DCI Descriptors").Range("A1").Cells(C.row, DCI_SHORT_DESC_COL).text
                DCI_DATA_C.Write Chr(34)
                If dict.Exists(tempStr) Then
                    DCI_DATA_C.Write "|" + dict(tempStr)
                Else
                    DCI_DATA_C.Write tempStr
                End If
                DCI_DATA_C.Write Chr(34)
            Else
                DCI_DATA_C.Write ("null")
            End If
            DCI_DATA_C.WriteLine (",")
    
            '*************************************
            ' Long Description
            DCI_DATA_C.Write (Space(indent))
            If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, DCI_LONG_DESC_COL).text <> Empty) Then
                tempStr = Worksheets("DCI Descriptors").Range("A1").Cells(C.row, DCI_LONG_DESC_COL).text
                tempStr = Replace(tempStr, Chr(13), " ")
                tempStr = Replace(tempStr, Chr(10), " ")
                DCI_DATA_C.Write (Chr(34) + tempStr + Chr(34))
            Else
                DCI_DATA_C.Write ("null")
            End If
            DCI_DATA_C.WriteLine (",")
        
            '*************************************
            ' Units
            DCI_DATA_C.Write (Space(indent))
            If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, DCI_UNITS_DESC_COL).text <> Empty) Then
                tempStr = Worksheets("DCI Descriptors").Range("A1").Cells(C.row, DCI_UNITS_DESC_COL).text
                DCI_DATA_C.Write Chr(34)
                If dict.Exists(tempStr) Then
                    DCI_DATA_C.Write "|" + dict(tempStr)
                Else
                    DCI_DATA_C.Write tempStr
                End If
                DCI_DATA_C.Write Chr(34)
            Else
                DCI_DATA_C.Write ("null")
            End If
            DCI_DATA_C.WriteLine (",")
        
            '**************************************
            ' Define the Owner Attribute Info Bits.
            AIA_string = ""
    
            add_an_OR = False
            none_defined = True
            
            If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, READ_ONLY_VAL_ATTRIB_COL).text <> Empty) Then
                AIA_string = AIA_string & "DCI_OWNER_ATTRIB_BITS_ENUM.DCI_OWNER_ATTRIB_INFO_READ_ONLY"
                add_an_OR = True
                none_defined = False
            End If
    
            '********************************
            ' Initial Value
            If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, INITIAL_VAL_ATTRIB_COL).text <> Empty) Then
                If add_an_OR = True Then
                    AIA_string = AIA_string & " | "
                    DCI_DATA_C.WriteLine (Space(indent) & AIA_string)
                   AIA_string = ""
                End If
                AIA_string = AIA_string & "DCI_OWNER_ATTRIB_BITS_ENUM.DCI_OWNER_ATTRIB_INFO_INIT_VAL"
                add_an_OR = True
                none_defined = False
            End If

            '********************************
            ' Default Value
            If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, DEFAULT_VAL_ATTRIB_COL).text <> Empty) Then
                If add_an_OR = True Then
                    AIA_string = AIA_string & " | "
                    DCI_DATA_C.WriteLine (Space(indent) & AIA_string)
                   AIA_string = ""
                End If
                AIA_string = AIA_string & "DCI_OWNER_ATTRIB_BITS_ENUM.DCI_OWNER_ATTRIB_INFO_DEFAULT_VAL"
                add_an_OR = True
                none_defined = False
            End If
    
            '********************************
            ' Default Value Attribute
            If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, RW_DEFAULT_VAL_ATTRIB_COL).text <> Empty) Then
                If add_an_OR = True Then
                    AIA_string = AIA_string & " | "
                    DCI_DATA_C.WriteLine (Space(indent) & AIA_string)
                   AIA_string = ""
                End If
                AIA_string = AIA_string & "DCI_OWNER_ATTRIB_BITS_ENUM.DCI_OWNER_ATTRIB_INFO_RW_DEFAULT_VAL"
                add_an_OR = True
                none_defined = False
            End If
    
            '********************************
            ' Range Attribute
            If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, RANGE_ATTRIB_COL).text <> Empty) Then
                If add_an_OR = True Then
                    AIA_string = AIA_string & " | "
                    DCI_DATA_C.WriteLine (Space(indent) & AIA_string)
                   AIA_string = ""
                End If
                AIA_string = AIA_string & "DCI_OWNER_ATTRIB_BITS_ENUM.DCI_OWNER_ATTRIB_INFO_RANGE_VAL"
                add_an_OR = True
                none_defined = False
            End If
            
            '********************************
            ' Enumeration Attribute
            If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, ENUM_ATTRIB_COL).text <> Empty) Then
                If add_an_OR = True Then
                    AIA_string = AIA_string & " | "
                    DCI_DATA_C.WriteLine (Space(indent) & AIA_string)
                   AIA_string = ""
                End If
                AIA_string = AIA_string & "DCI_OWNER_ATTRIB_BITS_ENUM.DCI_OWNER_ATTRIB_INFO_ENUM_VAL"
                add_an_OR = True
                none_defined = False
            End If
    
            '********************************
            ' Range Attribute
            If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, RW_RANGE_ATTRIB_COL).text <> Empty) Then
                If add_an_OR = True Then
                    AIA_string = AIA_string & " | "
                    DCI_DATA_C.WriteLine (Space(indent) & AIA_string)
                   AIA_string = ""
                End If
                AIA_string = AIA_string & "DCI_OWNER_ATTRIB_BITS_ENUM.DCI_OWNER_ATTRIB_INFO_RW_RANGE_VAL"
                add_an_OR = True
                none_defined = False
            End If
            
            '********************************
            ' Callback Attribute
            If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, CALLBACK_ATTRIB_COL).text <> Empty) Then
               If add_an_OR = True Then
                   AIA_string = AIA_string & " | "
                   DCI_DATA_C.WriteLine (Space(indent) & AIA_string)
                  AIA_string = ""
               End If
               AIA_string = AIA_string & "DCI_OWNER_ATTRIB_BITS_ENUM.DCI_OWNER_ATTRIB_INFO_CALLBACK"
               add_an_OR = True
               none_defined = False
            End If
            
            '********************************
            ' Length Attribute
            If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, RW_LENGTH_ATTRIB_COL).text <> Empty) Then
               If add_an_OR = True Then
                   AIA_string = AIA_string & " | "
                   DCI_DATA_C.WriteLine (Space(indent) & AIA_string)
                  AIA_string = ""
               End If
               AIA_string = AIA_string & "DCI_OWNER_ATTRIB_BITS_ENUM.DCI_OWNER_ATTRIB_INFO_RW_LENGTH"
               add_an_OR = True
               none_defined = False
            End If
            
            '********************************
            ' App Only Attribute
            If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, APP_ONLY_ATTRIB_COL).text <> Empty) Then
               If add_an_OR = True Then
                   AIA_string = AIA_string & " | "
                   DCI_DATA_C.WriteLine (Space(indent) & AIA_string)
                  AIA_string = ""
               End If
               AIA_string = AIA_string & "DCI_OWNER_ATTRIB_BITS_ENUM.DCI_OWNER_ATTRIB_INFO_APP_PARAM"
               add_an_OR = True
               none_defined = False
            End If
            
            DCI_DATA_C.Write (Space(indent))
            If none_defined = True Then
                DCI_DATA_C.Write ("0")
            Else
                DCI_DATA_C.Write (AIA_string)
            End If
            DCI_DATA_C.WriteLine (",")
    
            '***************************************
            ' Define the Patron Attribute Info Bits.
            AIA_string = ""
    
            add_an_OR = False
            none_defined = True
            If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, PATRON_RAM_VAL_WRITEABLE_COL).text <> Empty) And _
                    (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, READ_ONLY_VAL_ATTRIB_COL).text = Empty) Then
                AIA_string = AIA_string & "DCI_PATRON_ATTRIB_BITS_ENUM.DCI_PATRON_ATTRIB_RAM_WR_DATA"
                add_an_OR = True
                none_defined = False
            End If
    
            '********************************
            ' Patron Init Val Read Access
            If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, PATRON_INIT_VAL_READ_ACCESS_COL).text <> Empty) And _
                    (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, INITIAL_VAL_ATTRIB_COL).text <> Empty) Then
                If add_an_OR = True Then
                    AIA_string = AIA_string & " | "
                    DCI_DATA_C.WriteLine (Space(indent) & AIA_string)
                   AIA_string = ""
                End If
                AIA_string = AIA_string & "DCI_PATRON_ATTRIB_BITS_ENUM.DCI_PATRON_ATTRIB_NVMEM_DATA"
                add_an_OR = True
                none_defined = False
            End If
    
            '********************************
            ' Patron Init Val Write Access
            If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, PATRON_INIT_VAL_WRITEABLE_ACCESS_COL).text <> Empty) And _
                    (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, INITIAL_VAL_ATTRIB_COL).text <> Empty) Then
                If add_an_OR = True Then
                    AIA_string = AIA_string & " | "
                    DCI_DATA_C.WriteLine (Space(indent) & AIA_string)
                   AIA_string = ""
                End If
                AIA_string = AIA_string & "DCI_PATRON_ATTRIB_BITS_ENUM.DCI_PATRON_ATTRIB_NVMEM_WR_DATA"
                add_an_OR = True
                none_defined = False
            End If
    
            '********************************
            ' Patron Default Read Access
            If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, PATRON_DEFAULT_VAL_READ_ACCESS_COL).text <> Empty) And _
                    (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, DEFAULT_VAL_ATTRIB_COL).text <> Empty) Then
                If add_an_OR = True Then
                    AIA_string = AIA_string & " | "
                    DCI_DATA_C.WriteLine (Space(indent) & AIA_string)
                   AIA_string = ""
                End If
                AIA_string = AIA_string & "DCI_PATRON_ATTRIB_BITS_ENUM.DCI_PATRON_ATTRIB_DEFAULT_DATA"
                add_an_OR = True
                none_defined = False
            End If
    
            '********************************
            ' Patron Range Value Access
            If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, PATRON_RANGE_VAL_ACCESS_COL).text <> Empty) And _
                    (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, RANGE_ATTRIB_COL).text <> Empty) Then
                If add_an_OR = True Then
                    AIA_string = AIA_string & " | "
                    DCI_DATA_C.WriteLine (Space(indent) & AIA_string)
                   AIA_string = ""
                End If
                AIA_string = AIA_string & "DCI_PATRON_ATTRIB_BITS_ENUM.DCI_PATRON_ATTRIB_RANGE_DATA"
                add_an_OR = True
                none_defined = False
            End If
            
            '********************************
            ' Patron Enum Value Access
            If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, PATRON_ENUM_VAL_ACCESS_COL).text <> Empty) And _
                    (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, ENUM_ATTRIB_COL).text <> Empty) Then
                If add_an_OR = True Then
                    AIA_string = AIA_string & " | "
                    DCI_DATA_C.WriteLine (Space(indent) & AIA_string)
                   AIA_string = ""
                End If
                AIA_string = AIA_string & "DCI_PATRON_ATTRIB_BITS_ENUM.DCI_PATRON_ATTRIB_ENUM_DATA"
                add_an_OR = True
                none_defined = False
            End If
            
            DCI_DATA_C.Write (Space(indent))
            If none_defined = True Then
                DCI_DATA_C.Write ("0")
            Else
                DCI_DATA_C.Write (AIA_string)
            End If
        Else
            DCI_DATA_C.Write ("0,0")
        End If

        DCI_DATA_C.WriteLine (",")

        '***********************************
        ' Define the Range definition block.
        DCI_DATA_C.Write (Space(indent))
        If (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, RANGE_ATTRIB_COL).text <> Empty) Or (Worksheets("DCI Descriptors").Range("A1").Cells(C.row, ENUM_ATTRIB_COL).text <> Empty) Then
            
            DCI_DATA_C.Write ("new DCI_RANGE_BLOCK(")
            
            '***********************************
            ' Write out the Min data
            If (Cells(counter, DATA_MIN_COL).text) <> Empty Then
                DCI_DATA_C.Write (Cells(counter, DATA_MIN_COL).text)
            Else
                DCI_DATA_C.Write ("null")
            End If
            DCI_DATA_C.Write (", ")
            
            '***********************************
            ' Write out the Max data
            If (Cells(counter, DATA_MAX_COL).text) <> Empty Then
                DCI_DATA_C.Write (Cells(counter, DATA_MAX_COL).text)
            Else
                DCI_DATA_C.Write ("null")
            End If
            DCI_DATA_C.Write (", ")
            
            '***********************************
            ' Write out the enum data that should be comma delimited
            If (Cells(counter, DATA_ENUM_COL).text) <> Empty Then
                DCI_DATA_C.Write (Cells(counter, DATA_ENUM_COL).text)
            Else
                DCI_DATA_C.Write ("null")
            End If
            
            DCI_DATA_C.Write (")")
        Else
            DCI_DATA_C.Write ("null")
        End If
        DCI_DATA_C.WriteLine (",")
        
        '*************************************
        ' Define the Default definition block.
        DCI_DATA_C.Write (Space(indent))
        If (Cells(counter, DATA_DEFAULT_COL).text <> Empty) Then
            tempStr = Cells(counter, DATA_DEFAULT_COL).text
            If (tempStr = "TRUE" Or tempStr = "FALSE") Then tempStr = LCase(tempStr)
            If (Left(tempStr, 1) = "{" And Right(tempStr, 1) = "}") Then
                tempStr = Mid(tempStr, 2, Len(tempStr) - 2)
            End If
            If (InStr(tempStr, ",") <> 0) Then
                If (Left(tempStr, 1) <> Chr(34)) Then
                    tempStr = Chr(34) + tempStr + Chr(34)
                End If
            End If
            
            DCI_DATA_C.Write (tempStr)
        Else
            DCI_DATA_C.Write ("null")
        End If
        DCI_DATA_C.WriteLine (",")
        
        '*************************************
        ' Observer rights
        DCI_DATA_C.Write (Space(indent))
        If (Cells(counter, DATA_OBSERVER_COL).text <> Empty) Then
            tempStr = Cells(counter, DATA_OBSERVER_COL).text
            DCI_DATA_C.Write ("RWaccess.")
            Select Case tempStr
                Case "R"
                    DCI_DATA_C.Write ("READ")
                Case "W"
                    DCI_DATA_C.Write ("WRITE")
                Case "-"
                    DCI_DATA_C.Write ("NONE")
            End Select
        Else
            DCI_DATA_C.Write ("RWaccess.NONE")
        End If
        DCI_DATA_C.WriteLine (",")

        '*************************************
        ' Operator rights
        DCI_DATA_C.Write (Space(indent))
        If (Cells(counter, DATA_OPERATOR_COL).text <> Empty) Then
            tempStr = Cells(counter, DATA_OPERATOR_COL).text
            DCI_DATA_C.Write ("RWaccess.")
            Select Case tempStr
                Case "R"
                    DCI_DATA_C.Write ("READ")
                Case "W"
                    DCI_DATA_C.Write ("WRITE")
                Case "-"
                    DCI_DATA_C.Write ("NONE")
            End Select
        Else
            DCI_DATA_C.Write ("RWaccess.NONE")
        End If
        DCI_DATA_C.WriteLine (",")

        '*************************************
        ' Planning rights
        DCI_DATA_C.Write (Space(indent))
        If (Cells(counter, DATA_PLANNING_COL).text <> Empty) Then
            tempStr = Cells(counter, DATA_PLANNING_COL).text
            DCI_DATA_C.Write ("RWaccess.")
            Select Case tempStr
                Case "R"
                    DCI_DATA_C.Write ("READ")
                Case "W"
                    DCI_DATA_C.Write ("WRITE")
                Case "-"
                    DCI_DATA_C.Write ("NONE")
            End Select
        Else
            DCI_DATA_C.Write ("RWaccess.NONE")
        End If
        DCI_DATA_C.WriteLine (",")

        '*************************************
        ' Experiance
        DCI_DATA_C.Write (Space(indent))
        If (Cells(counter, DATA_EXPERIENCE_COL).text <> Empty) Then
            tempStr = Cells(counter, DATA_EXPERIENCE_COL).text
            DCI_DATA_C.Write ("ExperienceLevel." + tempStr)
        Else
            DCI_DATA_C.Write ("ExperienceLevel.None")
        End If
        DCI_DATA_C.WriteLine (",")
        
        '*************************************
        ' Category
        DCI_DATA_C.Write (Space(indent))
        If (Cells(counter, DATA_CATEGORIES_COL).text <> Empty) Then
            tempStr = Cells(counter, DATA_CATEGORIES_COL).text
            DCI_DATA_C.Write (Chr(34) + tempStr + Chr(34))
        Else
            DCI_DATA_C.Write ("null")
        End If
        DCI_DATA_C.WriteLine (",")
        
        '*************************************
        ' Offline availability
        DCI_DATA_C.Write (Space(indent))
        If (Cells(counter, DATA_OFFLINE_COL).text <> Empty) Then
            tempStr = Cells(counter, DATA_OFFLINE_COL).text
            Select Case tempStr
                Case "T"
                    DCI_DATA_C.Write ("true")
                Case "F"
                    DCI_DATA_C.Write ("false")
            End Select
        Else
            DCI_DATA_C.Write ("null")
        End If
        DCI_DATA_C.WriteLine (",")
        
        '*************************************
        ' Online availability
        DCI_DATA_C.Write (Space(indent))
        If (Cells(counter, DATA_ONLINE_COL).text <> Empty) Then
            tempStr = Cells(counter, DATA_ONLINE_COL).text
            Select Case tempStr
                Case "T"
                    DCI_DATA_C.Write ("true")
                Case "F"
                    DCI_DATA_C.Write ("false")
            End Select
        Else
            DCI_DATA_C.Write ("null")
        End If
    
        DCI_DATA_C.WriteLine ("")

        indent = indent - 4
        DCI_DATA_C.WriteLine (Space(indent) & ");")

        counter = counter + 1
    Wend
    
    ' Close the Init Function
    indent = indent - 4
    DCI_DATA_C.WriteLine (Space(indent) & "}")
    ' ==============================================================
    indent = indent - 4
    ' Close the class
    DCI_DATA_C.WriteLine (Space(indent) & "}")
    
    indent = indent - 4
    ' Close the namespace
    DCI_DATA_C.WriteLine (Space(indent) & "}")
    
    ' Close the file
    DCI_DATA_C.Close
    
  
    ' ****** Turn auto calcs back on
    Application.Calculation = xlCalculationAutomatic
End Sub













Sub Verify_PCTool_List()
    '*********************
    '******     Constants
    '*********************
    
    ' ****** Turn auto calcs off
    Application.Calculation = xlCalculationManual
    
    Dim sheet_name As String
    Dim BEGIN_IGNORED_DATA As String
    BEGIN_IGNORED_DATA = "BEGIN_IGNORED_DATA"
    
    START_ROW = 9
    DCI_DEFINES_START_ROW = 9
    DCI_DESCRIPTORS_COL = 1
    DCI_DEFINES_COL = 2
    DCI_DATATYPE_COL = 3
    DCI_LENGTH_COL = 4
    DCI_ARRAY_LENGTH_COL = 5
    DCI_TOTAL_LENGTH_COL = 6
    DCI_DEFAULT_COL = 7
    DCI_MIN_COL = 8
    DCI_MAX_COL = 9
    DCI_ENUM_COL = 10

    DESCRIP_COL = 1
    DATA_DEFINE_COL = 2
    DATA_DESC_COL = DESCRIP_COL
    DATA_PARAMID = 3
    DATA_DATATYPE_COL = 6
    DATA_LENGTH_COL = 7
    DATA_ARRAY_LENGTH_COL = 8
    DATA_TOTAL_LENGTH_COL = 9
    DATA_DEFAULT_COL = 10
    DATA_MIN_COL = 11
    DATA_MAX_COL = 12
    DATA_ENUM_COL = 13
    
    MAX_DATA_SIZE_ROW = 6
    MAX_DATA_SIZE_COL = 3
    
    '*********************
    '******     Begin
    '*********************
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select
    
    '****************************************************************************************************************************
    '******     Fill in the proper Param IDs as they would be generated by the DCI_Struct_Builder
    '****************************************************************************************************************************
    counter = DCI_DEFINES_START_ROW
    While Worksheets("DCI Descriptors").Cells(counter, DCI_DEFINES_COL).text <> Empty
        lookup_string = Worksheets("DCI Descriptors").Cells(counter, DCI_DEFINES_COL).text
        If lookup_string <> BEGIN_IGNORED_DATA Then
            Set C = Worksheets(sheet_name).Range("B9:AZ1000").Find(lookup_string, LookAt:=xlWhole)
            If Not (C Is Nothing) Then
                Cells(C.row, DATA_PARAMID).value = counter
            End If
        End If
        counter = counter + 1
    Wend
    
    '****************************************************************************************************************************
    '******     Fill in data from the DCI Descriptions sheet that we will use. We don't need all the flags duplicated
    '****************************************************************************************************************************
    dci_defines_row_ctr = DCI_DEFINES_START_ROW
    While Worksheets(sheet_name).Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text <> Empty
        Cells(dci_defines_row_ctr, DATA_DEFINE_COL).Interior.Color = RGB(255, 255, 255)
        lookup_string = Worksheets(sheet_name).Range("A1").Cells(dci_defines_row_ctr, DCI_DEFINES_COL).text
        If lookup_string <> BEGIN_IGNORED_DATA Then
            Set C = Worksheets("DCI Descriptors").Range("B9:AZ1000").Find(lookup_string, LookAt:=xlWhole)
            If Not (C Is Nothing) Then
                Cells(dci_defines_row_ctr, DATA_DEFINE_COL).Interior.Color = RGB(255, 255, 255)
                Cells(dci_defines_row_ctr, DATA_DESC_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(C.row, DCI_DESCRIPTORS_COL).text
                Cells(dci_defines_row_ctr, DATA_DATATYPE_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(C.row, DCI_DATATYPE_COL).text
                Cells(dci_defines_row_ctr, DATA_LENGTH_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(C.row, DCI_LENGTH_COL).text
                Cells(dci_defines_row_ctr, DATA_ARRAY_LENGTH_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(C.row, DCI_ARRAY_LENGTH_COL).text
                Cells(dci_defines_row_ctr, DATA_TOTAL_LENGTH_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(C.row, DCI_TOTAL_LENGTH_COL).text
                Cells(dci_defines_row_ctr, DATA_DEFAULT_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(C.row, DCI_DEFAULT_COL).text
                
                Cells(dci_defines_row_ctr, DATA_MIN_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(C.row, DCI_MIN_COL).text
                Cells(dci_defines_row_ctr, DATA_MAX_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(C.row, DCI_MAX_COL).text
                Cells(dci_defines_row_ctr, DATA_ENUM_COL).value = Worksheets("DCI Descriptors").Range("A1").Cells(C.row, DCI_ENUM_COL).text
            Else
                Cells(dci_defines_row_ctr, DATA_DEFINE_COL).Interior.Color = RGB(255, 0, 0)
            End If
        End If
        dci_defines_row_ctr = dci_defines_row_ctr + 1
    Wend

    ' ****** Turn auto calcs back on
    Application.Calculation = xlCalculationAutomatic
End Sub







