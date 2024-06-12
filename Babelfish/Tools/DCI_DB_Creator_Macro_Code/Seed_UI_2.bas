Attribute VB_Name = "Seed_UI_2"
Private Type UIInfo
    section As String
    name As String
    layout As String
    icon As String
    Route As String
    param As String
    specialCase As String
End Type

'****************************************************************
'Language translation sheet indexing
Public Const LANG_TRANSLATION_SHEET = "Lang Translation"
Public Const LANG_HEADER_DESC_COL = 3
Public Const LANG_COMMON_HEADER_ROW_START = 2
Public Const LANG_COMMON_HEADER_ROW_END = 4
Public Const LANG_COMMON_HEADER_COL = 4
Public Const LANG_HEADER_COMPRESSION_PARAM_ROW = 4
Public Const LANG_HEADER_ROW_START = 8
Public Const LANG_HEADER_ROW_END = 9
Public Const LANG_CODE_ROW = 9
Public Const LANG_SUPPORT_ROW = 10
Public Const LANG_TRANS_COL_START = 4
Public Const LANG_COL_START = 4
Public Const LANG_DESC_COL = 1
Public Const LANG_EXTEN_COL = 2
Public Const LANG_TYPE_COL = 3
Public Const COMPRESSION_PARAM_VAL_0 = 0
Public Const COMPRESSION_PARAM_VAL_1 = 1
Public Const DCI_STRING_ROW_SPEC = "DCI Strings Description"
Public Const NON_DCI_STRING_ROW_SPEC = "Non-DCI Strings Description"

Global lang_attribute_desc_row As Integer

'****************************************************************
'Seed UI 2.0 sheet indexing
Public Const SEED_DESCRIPTOR_SHEET = "Seed UI 2.0"
Public Const SEED_UI_JSON_FILE_PATH_COL = 19
Public Const SEED_UI_JSON_FILE_PATH_ROW = 2
Public Const SEED_UI_DATEPICKER_FILE_PATH_ROW = 2
Public Const SEED_UI_DATEPICKER_FILE_PATH_COL = 21
Public Const SEED_UI_INI_PATH_ROW = 1
Public Const SEED_UI_FILE_PATH_COL = 21
Public Const SEED_UI_GZIP_EXE_PATH_ROW = 1
Public Const SEED_UI_TAB = "Tab"
Public Const SEED_UI_SUBTAB = "Sub Tab"
Public Const SEED_UI_GRP = "Group"
Public Const SEED_UI_TITLE = "Title"
Public Const SEED_UI_ROW_START = 7
Public Const SEED_UI_ROW_TYPE = 6
Public Const SEED_UI_LANG_COL_START = 17
Public Const SEED_UI_TYPE_COL = 1
Public Const SEED_UI_NAME_COL = 2
Public Const SEED_UI_COMMON_JSON_ROW = 3
Public Const SEED_UI_JSON_PATH_ROW = 4

'****************************************************************
'DCI Descriptors sheet indexing
Const DCI_TYPE_COL = 3
Const DCI_LENGTH_COL = 6
Const DCI_MIN_COL = 8
Const DCI_MAX_COL = 9
Const DCI_ENUM_COL = 26
Const DCI_BITFIELD_COL = 11
Const DCI_NAME_COL = 79
Const DCI_DESC_COL = 81
Const DCI_UNITS_COL = 83
Const DCI_FORMAT_COL = "CI"
Const DCI_RAM_WRITE_ACCESS_COL = "BA"
Const DCI_RW_DEFAULT = "AD"
Const DCI_RW_RANGE = "AF"
Const DCI_RW_LENGTH = "AH"
Const DCI_DESCRIPTORS_SHEET = "DCI Descriptors"

'****************************************************************
'REST sheet indexing
Const REST_DEFINE_COL = 2
Const REST_PARAM_ID_COL = 3
Const REST_R_COL = 4
Const REST_W_COL = 5
Const REST_DESC_ROW_START = 10
Const REST_EXPORT_COL = 9
Const REST_IMPORT_COL = 10
Const REST_DESCRIPTOR_SHEET = "REST"

'****************************************************************
'Constants to derive file path and file extensions
Public Const COMMON_JSON_SUFFIX = "\common.json"
Public Const suffix = "\"
Public Const INI_EXTN = ".ini"
Public Const JSON_EXTN = ".json"
Public Const GZ_EXTN = ".gz"
Public Const JS_EXTN = ".js"

'****************************************************************
'Error codes, return values and other constants
Public Const QUOTES = 34
Public Const MINIMIZE_SHELL = 7
'Return codes in Create_Lang_INI function
Public Const CREATE_JSON_FILE = 1
Public Const LANG_HEADER_MISSING = 2
Public Const INI_ERROR = 3
Public Const INVALID_COMPRESSION_METHOD = 3
'Return codes in Create_Lang_JSON function
Public Const JSON_ERROR = 2
Public Const LANG_COL_MISSING = 2
Public Const LANG_COMMON_JSON_MISSING = 3
Public Const LANG_OUT_FOLDER_PATH_INVALID = 4
Public Const LANG_GZIP_TOOL_MISSING = 5
Public Const WS_SHELL_ERROR = 6


'**************************
'**************************
'Seed UI 2.0.
'**************************
'**************************
Sub Create_Web_UI_2()
'Constants Created for Seed UI 2.0...
Dim seedUI() As UIInfo
Dim seedUISheet As Worksheet
Dim WEB_START_ROW
Dim WEB_START_COLUMN
Dim WEB_END_Column
Dim START_ROW
Dim APP_PAGES_JS As Scripting.TextStream
Dim PAGE_CONFIG_JS As Scripting.TextStream
Dim DCI_META_JS As Scripting.TextStream
Dim INDEX_FILE As Scripting.TextStream
Dim fs As New FileSystemObject
Dim GroupCount
Dim Navcount
Dim TabRoute
'Object Created for Seed UI 2.0...
Set seedUISheet = Worksheets("Seed UI 2.0")
Set APP_PAGES_JS = fs.CreateTextFile(ActiveWorkbook.Path & "\" & seedUISheet.Cells(1, 3).value, True)
Set PAGE_CONFIG_JS = fs.CreateTextFile(ActiveWorkbook.Path & "\" & seedUISheet.Cells(2, 3).value, True)
Set DCI_META_JS = fs.CreateTextFile(ActiveWorkbook.Path & "\" & seedUISheet.Cells(3, 3).value, True)
Set INDEX_FILE = fs.CreateTextFile(ActiveWorkbook.Path & "\" & seedUISheet.Cells(4, 3).value, True)
WEB_START_ROW = seedUISheet.UsedRange.Rows.count
WEB_START_COLUMN = 1
UI_START_ROW = 8
WEB_END_Column = 4
TabCount = 0
SubTabCount = 0
GroupCount = 0
ParamCount = 0
Navcount = 0

'Making font type and size consistent.
Worksheets("Seed UI 2.0").Range("A:W").Font.name = "Arial"
Worksheets("Seed UI 2.0").Range("A:W").Font.Size = 10
    
ReDim Preserve seedUI(0 To WEB_START_ROW - UI_START_ROW, 0 To WEB_END_Column)
'Data from excel workbook are collected and populated in the seedUI array
 For i = 0 To UBound(seedUI, 1)
     For j = 0 To UBound(seedUI, 2)
     If (j = 0) Then
          If (seedUISheet.Cells(UI_START_ROW, WEB_START_COLUMN).value <> "") Then
               seedUI(i, j).section = seedUISheet.Cells(UI_START_ROW, 1).value
               seedUI(i, j).name = seedUISheet.Cells(UI_START_ROW, 2).value
               seedUI(i, j).layout = seedUISheet.Cells(UI_START_ROW, 3).value
               seedUI(i, j).icon = seedUISheet.Cells(UI_START_ROW, 4).value
               seedUI(i, j).Route = seedUISheet.Cells(UI_START_ROW, 5).value
               seedUI(i, j).specialCase = seedUISheet.Cells(UI_START_ROW, 6).value
               seedUI(i, j).param = getPid(seedUI(i, j).name)
           End If
               WEB_START_COLUMN = WEB_START_COLUMN + 1
      End If
      Next
               WEB_START_COLUMN = 1
               UI_START_ROW = UI_START_ROW + 1
      Next
'*****************************************************************
'*****************************************************************
' Generating the appPages.js file
'*****************************************************************
'*****************************************************************
APP_PAGES_JS.WriteLine ("import * as config from " & """./pageConfig""" & ";")
APP_PAGES_JS.WriteLine ("import * as paramMeta from " & """./dciMeta""" & ";")
APP_PAGES_JS.WriteLine ("")
APP_PAGES_JS.WriteLine ("export const appPages = {")
APP_PAGES_JS.WriteLine ("    config: config,")
APP_PAGES_JS.WriteLine ("    meta: paramMeta,")
APP_PAGES_JS.WriteLine ("    drawerNavGroup: [")
'-Tab Constants----
Dim TabData
Dim PrevType
For i = 0 To UBound(seedUI, 1)
    For j = 0 To UBound(seedUI, 2)
               If (j = 0) Then
                       If (seedUI(i, 0).section = "Tab") Then
                          If (TabCount > 0) And (SubTabCount > 0) Then
                                If (GroupCount >= 1) Then
                                    APP_PAGES_JS.WriteLine ("           ]")
                                    APP_PAGES_JS.WriteLine ("        },")
                                    TabRoute = Empty
                                Else
                                    APP_PAGES_JS.WriteLine ("                }")
                                    APP_PAGES_JS.WriteLine ("            ]")
                                    APP_PAGES_JS.WriteLine ("        },")
                                    TabRoute = Empty
                                End If
                          End If
                       '-Reset subtabcount-
                        SubTabCount = 0
                        TabCount = TabCount + 1
                                   GroupCount = 0
                                   If (TabCount > 1 And PrevType = "Tab") Then
                                   APP_PAGES_JS.WriteLine ("        },")
                                   End If
                                   APP_PAGES_JS.WriteLine ("        {")
                                   APP_PAGES_JS.WriteLine ("            id: " & i & ",")
                                   APP_PAGES_JS.WriteLine ("            name: """ & seedUI(i, j).name & """" & ",")
                                   If (seedUI(i, j).layout <> Empty) Then
                                   APP_PAGES_JS.WriteLine ("            layout: """ & seedUI(i, j).layout & """" & ",")
                                   End If
                                   APP_PAGES_JS.WriteLine ("            icon: """ & seedUI(i, j).icon & """" & ",")
                                   If (seedUI(i, j).Route <> Empty) Then
                                   TabRoute = seedUI(i, j).Route
                                   APP_PAGES_JS.WriteLine ("            route: """ & seedUI(i, j).Route & """" & ",")
                                   Else
                                   MsgBox "Please provide URL for the " & seedUI(i, j).name & " tab"
                                   End If
                                   TabData = getData(seedUI(i, j).name)
                      ElseIf (seedUI(i, j).section = "Group" And SubTabCount = 0) Then
                                   GroupCount = GroupCount + 1
                                   If (GroupCount = 1) Then
                                   APP_PAGES_JS.WriteLine ("            data: config." & TabData)
                                   APP_PAGES_JS.WriteLine ("        },")
                                   End If
                      ElseIf (seedUI(i, j).section = "Param Hidden") And (ParamCount < 1) And (SubTabCount > 0) Then
                       ParamCount = ParamCount + 1
                                If (GroupCount >= 1) Then
                                      APP_PAGES_JS.WriteLine ("           ]")
                                    APP_PAGES_JS.WriteLine ("        },")
                                Else
                                    APP_PAGES_JS.WriteLine ("                }")
                                    APP_PAGES_JS.WriteLine ("            ]")
                                End If
                      End If
                      If (seedUI(i, j).section = "Sub Tab") Then
                                   SubTabCount = SubTabCount + 1
                                   GroupCount = 0
                                   If (SubTabCount > 1 And PrevType = "Sub Tab") Then
                                   APP_PAGES_JS.WriteLine ("                },")
                                   End If
                                   If (SubTabCount = 1) Then
                                   APP_PAGES_JS.WriteLine ("            subTabNavGroup: [")
                                   End If
                                   APP_PAGES_JS.WriteLine ("                {")
                                   APP_PAGES_JS.WriteLine ("                    id: " & i & ",")
                                   APP_PAGES_JS.WriteLine ("                    name: """ & seedUI(i, j).name & """" & ",")
                                   APP_PAGES_JS.WriteLine ("                    layout: """ & seedUI(i, j).layout & """" & ",")
                                   If (seedUI(i, j).Route <> Empty) Then
                                   APP_PAGES_JS.WriteLine ("                    route: """ & TabRoute & seedUI(i, j).Route & """" & ",")
                                   Else
                                   MsgBox "Please provide URL for the " & seedUI(i, j).name & " sub tab"
                                   End If
                                   TabData = getData(seedUI(i, j).name)
                      ElseIf (seedUI(i, j).section = "Group") Then
                                   GroupCount = GroupCount + 1
                                   If (GroupCount = 1) Then
                                   APP_PAGES_JS.WriteLine ("                    data: config." & TabData)
                                   APP_PAGES_JS.WriteLine ("                },")
                                   End If
                      End If
                               PrevType = seedUI(i, j).section
                End If
 Next
 Next
 If (GroupCount >= 1) Then
 APP_PAGES_JS.WriteLine ("    ],")
 APP_PAGES_JS.WriteLine ("}")
 Else
 APP_PAGES_JS.WriteLine ("        }")
 APP_PAGES_JS.WriteLine ("    ],")
 APP_PAGES_JS.WriteLine ("}")
 End If
 
'*****************************************************************
'*****************************************************************
' Generating the dciMeta.js file with DCI Meta information
'*****************************************************************
'*****************************************************************
 Dim dciName() As String
 Dim codeDefine
 Dim dciCount
 dciCount = 1
 Dim rowCount
 rowCount = seedUISheet.UsedRange.Rows.count
 ReDim dciName(1 To rowCount)
 For i = 0 To UBound(seedUI, 1)
    For j = 0 To UBound(seedUI, 2)
       If (j = 0) Then
            If (seedUI(i, j).section = "Param" Or seedUI(i, j).section = "Param Hidden") Then
              dciName(dciCount) = seedUI(i, j).name
              codeDefine = RemoveDuplicateKeys(dciName)
              dciCount = dciCount + 1
            End If
       End If
   Next
 Next
 DCI_META_JS.WriteLine ("export const dciMetaData = {")
 For i = 0 To UBound(codeDefine, 1)
    If (codeDefine(i) <> "") Then
        metaData = getParamMetaData(codeDefine(i))
        DCI_META_JS.WriteLine ("  " & removeDCIPrefix(codeDefine(i)) & ": {")
        DCI_META_JS.WriteLine ("    id: " & """" & metaData(0) & """" & ",")
        'Check if Param meta data needs to be dynamically fetched
        If (metaData(16) = False) Then
            DCI_META_JS.WriteLine ("    name: " & """" & metaData(3) & """" & ",")
            If (metaData(4) <> "") Then
                DCI_META_JS.WriteLine ("    description: " & """" & metaData(4) & """" & ",")
            End If
            If (metaData(5) <> "") Then
                DCI_META_JS.WriteLine ("    units: " & """" & metaData(5) & """" & ",")
            End If
            If (metaData(8) <> "") Then
                DCI_META_JS.WriteLine ("    min: " & metaData(8) & ",")
            End If
            If (metaData(9) <> "") Then
                DCI_META_JS.WriteLine ("    max: " & metaData(9) & ",")
            End If
            DCI_META_JS.WriteLine ("    datatype: " & """" & metaData(6) & """" & ",")
            If (metaData(10) <> "") Then
                DCI_META_JS.WriteLine ("    enums: { " & metaData(10) & "},")
            End If
            If (metaData(11) <> "") Then
                DCI_META_JS.WriteLine ("    bitfields: { " & metaData(11) & "},")
            End If
            DCI_META_JS.WriteLine ("    r: " & metaData(1) & ",")
            DCI_META_JS.WriteLine ("    w: " & metaData(2) & ",")
            DCI_META_JS.WriteLine ("    length: " & metaData(12) & ",")
            DCI_META_JS.WriteLine ("    presentWR: " & metaData(13) & ",")
        Else
            DCI_META_JS.WriteLine ("    isDynamicMeta: true" & ",")
        End If
		If (metaData(7) <> "") Then
			DCI_META_JS.WriteLine ("    format: " & """" & metaData(7) & """" & ",")
		End If        
        If (metaData(14) <> "") Then
            DCI_META_JS.WriteLine ("    export: " & metaData(14) & ",")
        End If
        If (metaData(15) <> "") Then
            DCI_META_JS.WriteLine ("    import: " & metaData(15) & ",")
        End If
        DCI_META_JS.WriteLine ("  },")
    End If
 Next
 DCI_META_JS.WriteLine ("};")
 
'*****************************************************************
'*****************************************************************
'index.HTML
'*****************************************************************
'*****************************************************************
 INDEX_FILE.WriteLine ("<!DOCTYPE html>")
 INDEX_FILE.WriteLine ("<html class=""" & "no-js" & """>")
 INDEX_FILE.WriteLine ("")
 INDEX_FILE.WriteLine ("<head>")
 INDEX_FILE.WriteLine ("    <meta charset=""" & "utf-8" & """>")
 INDEX_FILE.WriteLine ("    <title>" & seedUISheet.Cells(7, 2).value & "</title>")
 INDEX_FILE.WriteLine ("    <meta name=""" & "viewport""" & "" & " content=""" & "width=device-width, initial-scale=1.0""" & "" & ">")
 INDEX_FILE.WriteLine ("    <link rel=""" & "icon""" & "" & " type=""" & "image/png""" & "" & " sizes=""" & "32x32""" & "" & " href=""" & "%PUBLIC_URL%/favicon-32x32.png""" & ">")
 INDEX_FILE.WriteLine ("    <link rel=""" & "icon""" & "" & " type=""" & "image/png""" & "" & " sizes=""" & "16x16""" & "" & " href=""" & "%PUBLIC_URL%/favicon-16x16.png""" & ">")
 INDEX_FILE.WriteLine ("    <link rel=""" & "manifest""" & "" & " href=""" & "%PUBLIC_URL%/site.webmanifest""" & ">")
 INDEX_FILE.WriteLine ("</head>")
 INDEX_FILE.WriteLine ("")
 INDEX_FILE.WriteLine ("<body>")
 INDEX_FILE.WriteLine ("    <div id=""" & "root" & """>" & "</div>")
 INDEX_FILE.WriteLine ("    <div id=""banner"" style=""display: none; text-align: center;"">")
 INDEX_FILE.WriteLine ("   	<span style=""font-size: 20px; color: #ca3c3d""> IE is not supported. Please use Chrome, MS Edge or Firefox. </span>")
 INDEX_FILE.WriteLine ("    </div>")
 INDEX_FILE.WriteLine ("")
 INDEX_FILE.WriteLine ("   <script>")
 INDEX_FILE.WriteLine ("      if (navigator.userAgent.match(/Trident.*rv\:11\./)) {")
 INDEX_FILE.WriteLine ("           document.getElementById(""banner"").style.display = ""block"";")
 INDEX_FILE.WriteLine ("           document.getElementById(""root"").style.display = ""none"";")
 INDEX_FILE.WriteLine ("       }")
 INDEX_FILE.WriteLine ("   </script>")
 INDEX_FILE.WriteLine ("</body>")
 INDEX_FILE.WriteLine ("")
 INDEX_FILE.WriteLine ("</html>")
'*****************************************************************
'*****************************************************************
' Generating the pageConfig.js file
'*****************************************************************
'*****************************************************************
TabCount = 1
SubTabCount = 1
PAGE_CONFIG_JS.WriteLine ("import * as meta from " & """./dciMeta""" & ";")
For i = 0 To UBound(seedUI, 1)
    For j = 0 To UBound(seedUI, 2)
        If (j = 0) Then

            'Exit the loop when there is no entry
            If (seedUI(i, j).section = "") Then
                GroupCount = 0
            End If

            'Increment Groupcount & reset the TabCount
            If (seedUI(i, j).section = "Group") Then
                GroupCount = GroupCount + 1
                TabCount = 0
                SubTabCount = 0
            End If
            
            If (GroupCount = 1 Or seedUI(i, j).section = "Tab" Or seedUI(i, j).section = "Sub Tab") Then
               'Reset subtabname
                If (GroupCount = 0) And (SubTabCount >= 1) Then
                    SubTabName = Empty
                End If
                
                If (seedUI(i, j).section = "Group" Or seedUI(i, j).section = "") Then
                    PAGE_CONFIG_JS.WriteLine ("")
                    If (SubTabName <> Empty) Then
                        If (GroupCount >= 1) Then
                            PAGE_CONFIG_JS.WriteLine ("export const " & SubTabName & " = [")
                            SubTabName = Empty
                        End If
                    Else
                        If (GroupCount >= 1) Then
                            PAGE_CONFIG_JS.WriteLine ("export const " & TabName & " = [")
                            TabName = Empty
                        End If
                    End If
                    
                    PAGE_CONFIG_JS.WriteLine ("  {")
                    PAGE_CONFIG_JS.WriteLine ("    title: """ & seedUI(i, j).name & """" & ",")
                    
                    If (seedUI(i, j).icon <> "") Then
                        PAGE_CONFIG_JS.WriteLine ("    icon: """ & seedUI(i, j).icon & """" & ",")
                    End If
                    
                    If (seedUI(i, j).layout <> Empty) Then
                        PAGE_CONFIG_JS.WriteLine ("    layout: """ & seedUI(i, j).layout & """" & ",")
                    End If
                    
                    'hidden if & disabled if & style if special case
                    If (seedUI(i, j).specialCase <> "") Then
                        Call specialCase(seedUI(i, j).specialCase, PAGE_CONFIG_JS)
                    End If
                    PAGE_CONFIG_JS.WriteLine ("    data: [")
                ElseIf (seedUI(i, j).section = "Param" Or seedUI(i, j).section = "Tab" Or seedUI(i, j).section = "Group" Or seedUI(i, j).section = "Sub Tab") Then
                    If (seedUI(i, j).section = "Param") Then
                        PAGE_CONFIG_JS.WriteLine ("      """ & seedUI(i, j).param & """" & ",")
                    ElseIf seedUI(i, j).section = "Tab" And (TabCount = 0) Then
                        If (GroupCount > 1) Then
                            PAGE_CONFIG_JS.WriteLine ("    ]")
                            PAGE_CONFIG_JS.WriteLine ("  }")
                            PAGE_CONFIG_JS.WriteLine ("];")
                        ElseIf (GroupCount >= 1) And (SubTabCount = 0) Then
                            PAGE_CONFIG_JS.WriteLine ("    ]")
                            PAGE_CONFIG_JS.WriteLine ("  }")
                            PAGE_CONFIG_JS.WriteLine ("];")
                        End If
                    ElseIf seedUI(i, j).section = "Sub Tab" And (SubTabCount = 0) Then
                        If (GroupCount >= 1) Then
                            PAGE_CONFIG_JS.WriteLine ("    ]")
                            PAGE_CONFIG_JS.WriteLine ("  }")
                            PAGE_CONFIG_JS.WriteLine ("];")
                        End If
                    End If
                End If
            ElseIf (GroupCount >= 2) Then
                If (seedUI(i, j).section = "Group" Or seedUI(i, j).section = "") Then
                    PAGE_CONFIG_JS.WriteLine ("    ]")
                    PAGE_CONFIG_JS.WriteLine ("  },")
                    PAGE_CONFIG_JS.WriteLine ("  {")
                    PAGE_CONFIG_JS.WriteLine ("    title: """ & seedUI(i, j).name & """" & ",")
                    If (seedUI(i, j).icon <> "") Then
                        PAGE_CONFIG_JS.WriteLine ("    icon: """ & seedUI(i, j).icon & """" & ",")
                    End If
                    If (seedUI(i, j).layout <> Empty) Then
                        PAGE_CONFIG_JS.WriteLine ("    layout: """ & seedUI(i, j).layout & """" & ",")
                    End If
                    'hidden if & disabled if & style if special case
                    If (seedUI(i, j).specialCase <> "") Then
                        Call specialCase(seedUI(i, j).specialCase, PAGE_CONFIG_JS)
                    End If
                    PAGE_CONFIG_JS.WriteLine ("    data: [")
                ElseIf (seedUI(i, j).section = "Param") Then
                    PAGE_CONFIG_JS.WriteLine ("      """ & seedUI(i, j).param & """" & ",")
                End If
            End If
            'Incrementing the tabCount and storing the tab name and subtabname which has the group and param...
            If (seedUI(i, j).section = "Tab") Then
                TabName = getData(seedUI(i, j).name)
                TabCount = TabCount + 1
                'Reset Groupcount
                GroupCount = 0
            ElseIf (seedUI(i, j).section = "Sub Tab") Then
                SubTabName = getData(seedUI(i, j).name)
                SubTabCount = SubTabCount + 1
                'Reset Groupcount
                GroupCount = 0
            End If
        End If
    Next
Next
If (TabCount = 0) Then
 PAGE_CONFIG_JS.WriteLine ("    ]")
 PAGE_CONFIG_JS.WriteLine ("  }")
 PAGE_CONFIG_JS.WriteLine ("];")
End If
End Sub

'converts the tabname into lowercase by triming thier space in between
Function getData(text As String)
   Dim lower As String
   lower = LCase(text)
   getData = Replace(lower, " ", "")
End Function

'Captures all the DCI param meta information from DCI Descriptor and REST sheets
Function getParamMetaData(name) As Variant
    Dim check() As String
    Dim dci() As String
    count = 0
    Dim dRowRest As Range
    Dim dRowDciDescriptor As Range
    Dim ret_val(0 To 16) As Variant
    Dim rwDefaultVal As String
    Dim rwRangeVal As String
    Dim rwLengthVal As String

    If (name <> "") Then
        check = Split(name, "_")
        If (check(0) = "DCI") Then
            dci = Split(name, "[")

            'Get meta data from REST Sheet
            Set dRowRest = getRow("REST", CStr(dci(i)))
            For i = 0 To UBound(dci, 1)
                If (Not dRowRest Is Nothing) Then
                    ret_val(0) = dRowRest.Cells(, REST_PARAM_ID_COL).value 'param id
                    ret_val(1) = dRowRest.Cells(, REST_R_COL).value 'param read value
                    ret_val(2) = dRowRest.Cells(, REST_W_COL).value 'param write value
                    export = dRowRest.Cells(, REST_EXPORT_COL).value 'param export access
                    If (export = "x") Then
                        ret_val(14) = 1
                    Else
                        ret_val(14) = export
                    End If
                    import = dRowRest.Cells(, REST_IMPORT_COL).value 'param import access
                    If (import = "x") Then
                        ret_val(15) = 1
                    Else
                        ret_val(15) = import
                    End If
                Else
                    MsgBox "Error: One or more Seed UI parameters are not present in the REST sheet"
                End If
            Next i

            'Get meta data from DCI Descriptor Sheet
            Set dRowDciDescriptor = getRow("DCI Descriptors", CStr(dci(j)))
            For j = 0 To UBound(dci, 1)
                If (Not dRowDciDescriptor Is Nothing) Then
                    ret_val(3) = escapeString(dRowDciDescriptor.Cells(, DCI_NAME_COL).text) 'param name
                    ret_val(4) = escapeString(dRowDciDescriptor.Cells(, DCI_DESC_COL).text) 'param description
                    ret_val(5) = escapeString(dRowDciDescriptor.Cells(, DCI_UNITS_COL).text) 'param units
                    ret_val(6) = LCase(Mid(dRowDciDescriptor.Cells(, DCI_TYPE_COL).value, 11)) 'param datatype
                    ret_val(7) = escapeString(dRowDciDescriptor.Cells(, DCI_FORMAT_COL).text) 'param format
                    min = dRowDciDescriptor.Cells(, DCI_MIN_COL).value 'param minimum range
                    If (min <> "") Then
                        If (min = "True") Then
                            ret_val(8) = 1
                        ElseIf (min = "False") Then
                            ret_val(8) = 0
                        Else
                            ret_val(8) = min
                        End If
                    Else
                        ret_val(8) = min
                    End If
                    max = dRowDciDescriptor.Cells(, DCI_MAX_COL).value 'param maximum range
                    If (max <> "") Then
                        If (max = "True") Then
                            ret_val(9) = 1
                        ElseIf (max = "False") Then
                            ret_val(9) = 0
                        Else
                            ret_val(9) = max
                        End If
                    Else
                        ret_val(9) = max
                    End If
                    ret_val(10) = getEnum(name) 'param enums
                    ret_val(11) = getBitfields(name) 'param bitfields
                    ret_val(12) = dRowDciDescriptor.Cells(, DCI_LENGTH_COL).value 'param length
                    presentWR = dRowDciDescriptor.Cells(, DCI_RAM_WRITE_ACCESS_COL).value 'param ram write access
                    If (presentWR = "x") Then
                        ret_val(13) = 1
                    Else
                        ret_val(13) = 0
                    End If
                    rwDefaultVal = escapeString(dRowDciDescriptor.Cells(, DCI_RW_DEFAULT).text) 'RW Default
                    rwRangeVal = escapeString(dRowDciDescriptor.Cells(, DCI_RW_RANGE).text) 'RW Range
                    rwLengthVal = escapeString(dRowDciDescriptor.Cells(, DCI_RW_LENGTH).text) 'RW Length
                    If (rwDefaultVal = "x" Or rwRangeVal = "x" Or rwLengthVal = "x") Then
                        ret_val(16) = True
                    Else
                        ret_val(16) = False
                    End If
                Else
                    MsgBox "Error: One or more Seed UI parameters are not present in the DCI Descriptor sheet"
                End If
            Next j
            'returns the param name
            getParamMetaData = ret_val
        End If
    End If
End Function

'Extract Param Id from the REST sheet
Function getPid(name)
    Dim check() As String
    Dim dci() As String
    Dim index() As String
    count = 0

    If (name <> "") Then
        check = Split(name, "_")
        If (check(0) = "DCI") Then
            dci = Split(name, "[")
            For i = 0 To UBound(dci, 1)
                If (Not getRow("REST", CStr(dci(i))) Is Nothing) Then
                    pid = getRow("REST", CStr(dci(i))).Cells(, REST_PARAM_ID_COL).value
                ElseIf (dci(i) <> Empty) Then
                    index = Split(dci(i), "]")
                Else
                    MsgBox "Error: One or more Seed UI parameters are not present in the REST sheet"
                End If
            Next i
            'returns the pid
            If (Not index) = -1 Then
                getPid = pid
            Else
                getPid = pid & "[" & index(0) & "]"
            End If
        End If
    End If
End Function

'Extract the Enum details from the DCI Desciptor sheet
Function getEnum(name)
    Dim DCI_Enum_Details() As String
    Dim enumInfo As String
    Dim FoundCell As Excel.Range
    Set ws = Worksheets(DCI_DESCRIPTORS_SHEET)
    Dim dci() As String
    dci = Split(name, "[")
    Set FoundCell = ws.Range("B:B").Find(what:=dci(0), lookat:=xlWhole)

    RawString = Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(FoundCell.row, DCI_ENUM_COL))

    If (RawString <> "") Then
        DCI_Enum_Details = Split(RawString, ITEM_DELIMITER)
        enumInfo = ""
        For index = 1 To UBound(DCI_Enum_Details) Step 1
            If ("" <> Trim(DCI_Enum_Details(index))) Then
                ParsedItems = Split(DCI_Enum_Details(index), SUB_ITEM_DELIMITER)
                'Remove line feeds
                ParsedItems(0) = Replace(ParsedItems(0), vbLf, "")
                ParsedItems(1) = Replace(ParsedItems(1), vbLf, "")
                ParsedItems(2) = Replace(ParsedItems(2), vbLf, "")
                        
                'Concatenate dci enum information to form an object
                If (ParsedItems(0) < 0) Then
                    enumInfo = enumInfo & """" & ParsedItems(0) & """" & ": " & """" & ParsedItems(2) & """" & ", "
                Else
                    enumInfo = enumInfo & ParsedItems(0) & ": " & """" & ParsedItems(2) & """" & ", "
                End If
            End If
        Next index
        getEnum = enumInfo
    End If
End Function

'Extract the Bitfield details from the DCI Desciptor sheet
Function getBitfields(name)
    Dim DCI_Bitfields_Details() As String
    Dim bitfieldInfo As String
    Dim FoundCell As Excel.Range
    Set ws = Worksheets(DCI_DESCRIPTORS_SHEET)
    Dim dci() As String
    dci = Split(name, "[")
    Set FoundCell = ws.Range("B:B").Find(what:=dci(0), lookat:=xlWhole)

    RawString = Trim(Worksheets(DCI_DESCRIPTORS_SHEET).Cells(FoundCell.row, DCI_BITFIELD_COL))

    If (RawString <> "") Then
        DCI_Bitfields_Details = Split(RawString, ITEM_DELIMITER)
        bitfieldInfo = ""
        For index = 1 To UBound(DCI_Bitfields_Details) Step 1
            If ("" <> Trim(DCI_Bitfields_Details(index))) Then
                ParsedItems = Split(DCI_Bitfields_Details(index), SUB_ITEM_DELIMITER)
                'Remove line feeds
                ParsedItems(0) = Replace(ParsedItems(0), vbLf, "")
                ParsedItems(1) = Replace(ParsedItems(1), vbLf, "")
                ParsedItems(2) = Replace(ParsedItems(2), vbLf, "")
                        
                'Concatenate dci enum information to form an object
                bitfieldInfo = bitfieldInfo & ParsedItems(0) & ": " & """" & ParsedItems(2) & """" & ", "
            End If
        Next index
        getBitfields = bitfieldInfo
    End If
End Function

'Escapes the string from quotes
Function escapeString(ByVal str As String) As String
    escapeString = Replace(Replace(Replace(str, "'", "\'"), vbCr, " "), vbLf, " \n")
End Function

'which returns the paramid of the codedefines from the Rest sheet..
Function getRow(ByRef worksheetName As String, ByRef dciName As String)
    Dim grDCI, grRow
    Set grDCI = Worksheets(worksheetName).Range("B:B")
    Set grRow = grDCI.Find(what:=dciName, lookat:=xlWhole)
    If (grRow Is Nothing) Then
         Set getRow = Nothing
    Else
         Set getRow = grRow.EntireRow
    End If
End Function

'which removes the 'DCI' prefix from the codedefines..
Function removeDCIPrefix(str)
    Dim text
    Dim name() as String
    text = Mid(str, 5)
    name = Split(text, "[")
    removeDCIPrefix = name(0)
End Function

'which add the hiddenif & disabledif & styledif special case to the pageconfig.js....
Function specialCase(data, PAGE_CONFIG_JS)
    Dim values() As String
    values = Split(data, "}],")
    PAGE_CONFIG_JS.WriteLine (values(0) & ",")
End Function

'Removing the duplicate codedefines from the nameId array
Function RemoveDuplicateKeys(dciName As Variant) As Variant
    Dim i As Long
    Dim d As Scripting.Dictionary
    Set d = New Scripting.Dictionary
    With d
        For i = LBound(dciName) To UBound(dciName)
            If (IsMissing(dciName(i)) = False) Then
               If dciName(i) Like "*[[]*" Then
                 dci = Split(dciName(i), "[")
                .Item(dci(0)) = l
               Else
                 .Item(dciName(i)) = l
               End If
            End If
        Next
        RemoveDuplicateKeys = .keys
    End With
End Function

'*********************************************
'*********************************************
'Generate Language INI and JSON files
'*********************************************
'*********************************************
Sub Generate_Lang_INI_JSON()

LANGUAGE_COUNT = 0       'Total number of languages available in LANG_TRANSLATION_SHEET
Dim lang_counter As Integer
Dim supported_lang_counter As Long
Dim supported_locale_list() As String
Dim lang_col_counter As Integer
Dim header_empty_counter As Integer
Dim file_name As String
Dim json_file_path As String
Dim ini_file_path As String
Dim gz_file_path As String
Dim header_missing_name As String
Dim ret_val_ini As Integer
Dim ret_val_json As Integer
Dim lang_common_hdr_empty As Boolean

lang_counter = LANG_TRANS_COL_START

'Get the row number having language names
lang_attribute_desc_row = LANG_SUPPORT_ROW + 1
While (Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_attribute_desc_row, LANG_DESC_COL).text <> NON_DCI_STRING_ROW_SPEC)
    lang_attribute_desc_row = lang_attribute_desc_row + 1
Wend
        
While (Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_attribute_desc_row, lang_counter).text <> Empty)
    LANGUAGE_COUNT = LANGUAGE_COUNT + 1
    lang_counter = lang_counter + 1
Wend


'Delete old language ini files if present
ini_file_path = ActiveWorkbook.Path & "\" & Cells(SEED_UI_INI_PATH_ROW, SEED_UI_JSON_FILE_PATH_COL).value & "\*.ini*"
If (Dir(ini_file_path) <> "") Then
    Kill ini_file_path
End If
'Delete old language json files if present
json_file_path = ActiveWorkbook.Path & "\" & Cells(SEED_UI_JSON_FILE_PATH_ROW, SEED_UI_JSON_FILE_PATH_COL).value & "\*.json*"
If (Dir(json_file_path) <> "") Then
    Kill json_file_path
End If
'Delete old gzip compressed language files if present
gz_file_path = ActiveWorkbook.Path & "\" & Cells(SEED_UI_JSON_FILE_PATH_ROW, SEED_UI_JSON_FILE_PATH_COL).value & "\*.gz*"
If (Dir(gz_file_path) <> "") Then
    Kill gz_file_path
End If

header_missing_name = ""
header_empty_counter = 0
lang_counter = 0
lang_col_counter = LANG_TRANS_COL_START

'Verify whether all common header fields are present
row_counter = LANG_COMMON_HEADER_ROW_START
While (lang_common_hdr_empty = False And row_counter <= LANG_COMMON_HEADER_ROW_END)
    If (Worksheets(LANG_TRANSLATION_SHEET).Cells(row_counter, LANG_COMMON_HEADER_COL).text = Empty) Then
        lang_common_hdr_empty = True
    End If
    row_counter = row_counter + 1
Wend

If (lang_common_hdr_empty = False) Then

    While (ret_val_ini < INI_ERROR And ret_val_json < JSON_ERROR And lang_counter < LANGUAGE_COUNT)
    
        'Check if language support is enabled
        If (Worksheets(LANG_TRANSLATION_SHEET).Cells(LANG_SUPPORT_ROW, lang_col_counter).text = "x") Then
           
            file_name = Worksheets(LANG_TRANSLATION_SHEET).Cells(LANG_CODE_ROW, lang_col_counter).text
            
            '*******************************************************************************************************************
            ' Updates the localelist that are supported for the dateTimePicker
            '*******************************************************************************************************************
            
            ReDim Preserve supported_locale_list(supported_lang_counter)
            supported_locale_list(supported_lang_counter) = file_name
            supported_lang_counter = supported_lang_counter + 1

            '*******************************************************************************************************************
            ' based on language header information present in Lang Translation sheet create INI file
            '*******************************************************************************************************************
            ret_val_ini = Create_Lang_INI(header_missing_name, header_empty_counter, lang_col_counter, suffix & file_name & INI_EXTN, file_name)
            
            If (ret_val_ini = CREATE_JSON_FILE) Then
        
                '********************************************************************************************************************
                ' Create Language JSON file which includes data in common.json file, Seed UI 2.0 sheet and Lang Translation sheet
                '********************************************************************************************************************
                ret_val_json = Create_Lang_JSON(lang_col_counter, file_name)
                lang_counter = lang_counter
            End If
        End If
        
        lang_counter = lang_counter + 1
        lang_col_counter = lang_col_counter + 1
    Wend
    
    '*********************************************************************************************
    ' Display msg box if language is enabled and header information is missing
    '*********************************************************************************************
    If (header_missing_name <> Empty) Then
        header_missing_name = header_missing_name
        Call MsgBox("Header Information for below language is missing in " & _
                     Chr(QUOTES) & LANG_TRANSLATION_SHEET & Chr(QUOTES) & " worksheet" & vbCr & header_missing_name, _
                     vbExclamation + vbOKOnly, "Language Files")
    End If
    
    '*********************************************************************************************
    ' Display msg box if language column is missing in Seed UI 2.0 worksheet
    '*********************************************************************************************
    If (ret_val_json = LANG_COL_MISSING) Then
        Call MsgBox(Chr(QUOTES) & file_name & Chr(QUOTES) & " language column is missing in " & _
                    Chr(QUOTES) & SEED_DESCRIPTOR_SHEET & Chr(QUOTES) & " worksheet", _
                    vbExclamation + vbOKOnly, "Language Files")
    
    '*********************************************************************************************
    ' Display msg box if common.json file is missing in "WebUI_2.0\locales" folder
    '*********************************************************************************************
    ElseIf (ret_val_json = LANG_COMMON_JSON_MISSING) Then
        Call MsgBox("common.json file is missing for " & Chr(QUOTES) & file_name & Chr(QUOTES) & _
                    " language at location " & Chr(QUOTES) & "WebUI_2.0\locales" & Chr(QUOTES), _
                    vbExclamation + vbOKOnly, "Language Files")
    End If
    
Else
    '*********************************************************************************************
    ' Display msg box if common header information is missing
    '*********************************************************************************************
    Call MsgBox("Common Header Information is missing in " & Chr(QUOTES) & LANG_TRANSLATION_SHEET & Chr(QUOTES) & " worksheet" & _
                vbCr, vbExclamation + vbOKOnly, "Language Files")
End If

If (Len(Join(supported_locale_list)) > 0) Then
   Call Create_dateTimePickerLocaleFile(supported_locale_list)
End If

End Sub

'*********************************************************************************************
' Function to create Language INI file and adding header information in it
'*********************************************************************************************
Function Create_Lang_INI(ByRef header_missing_name As String, ByRef header_empty_counter As Integer, ByVal lang_col As Integer, ByVal ini_name As String, ByVal file_name As String) As Integer

Dim row_counter As Integer
Dim ret_val As Integer
Dim lang_hdr_empty As Boolean
Dim file_path As String

ret_val = 0

'Verify whether all language header fields are present
lang_hdr_empty = False
row_counter = LANG_HEADER_ROW_START
While (lang_hdr_empty = False And row_counter <= LANG_HEADER_ROW_END)
    If (Worksheets(LANG_TRANSLATION_SHEET).Cells(row_counter, lang_col).text = Empty) Then
        lang_hdr_empty = True
        header_empty_counter = header_empty_counter + 1
        header_missing_name = header_missing_name & header_empty_counter & "." & " " & _
                              Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_attribute_desc_row, lang_col).text & vbCr
    End If
    row_counter = row_counter + 1
Wend

If (lang_hdr_empty = False) Then

    'Create INI file
    Sheets(SEED_DESCRIPTOR_SHEET).Select
    Set fs = CreateObject("Scripting.FileSystemObject")
    file_path = ActiveWorkbook.Path & "\" & Cells(SEED_UI_INI_PATH_ROW, SEED_UI_JSON_FILE_PATH_COL).value & ini_name
    Set LANG_INI = fs.CreateTextFile(file_path, True, True)
    row_counter = LANG_COMMON_HEADER_ROW_START
    
    LANG_INI.WriteLine ("[Language Header Information]")
    
    While (row_counter <= LANG_COMMON_HEADER_ROW_END)
        LANG_INI.WriteLine (Worksheets(LANG_TRANSLATION_SHEET).Cells(row_counter, LANG_HEADER_DESC_COL).text & "=" & Worksheets(LANG_TRANSLATION_SHEET).Cells(row_counter, LANG_COMMON_HEADER_COL).text)
        row_counter = row_counter + 1
    Wend
    
    row_counter = LANG_HEADER_ROW_START
    While (row_counter <= LANG_HEADER_ROW_END)
        LANG_INI.WriteLine (Worksheets(LANG_TRANSLATION_SHEET).Cells(row_counter, LANG_HEADER_DESC_COL).text & "=" & Worksheets(LANG_TRANSLATION_SHEET).Cells(row_counter, lang_col).text)
        row_counter = row_counter + 1
    Wend
    
    LANG_INI.WriteLine ("Language_Name=" & Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_attribute_desc_row, lang_col).text)
    
    ret_val = CREATE_JSON_FILE 'INI file created
    
    If (Worksheets(LANG_TRANSLATION_SHEET).Cells(LANG_HEADER_COMPRESSION_PARAM_ROW, LANG_COMMON_HEADER_COL).text = COMPRESSION_PARAM_VAL_0) Then
        LANG_INI.WriteLine ("LANGUAGE_JSON_PATH=" & Cells(SEED_UI_JSON_PATH_ROW, SEED_UI_JSON_FILE_PATH_COL).value & suffix & file_name & JSON_EXTN)
    ElseIf (Worksheets(LANG_TRANSLATION_SHEET).Cells(LANG_HEADER_COMPRESSION_PARAM_ROW, LANG_COMMON_HEADER_COL).text = COMPRESSION_PARAM_VAL_1) Then
         LANG_INI.WriteLine ("LANGUAGE_JSON_PATH=" & Cells(SEED_UI_JSON_PATH_ROW, SEED_UI_JSON_FILE_PATH_COL).value & suffix & file_name & GZ_EXTN)
    Else
        'Compression parameter value in header is invalid
        ret_val = INVALID_COMPRESSION_METHOD
        'Display a warning message
        Call MsgBox("Invalid compression method! " & vbCr & _
                    "Enter valid compression method in common header field in " & _
                    Chr(QUOTES) & LANG_TRANSLATION_SHEET & Chr(QUOTES) & " worksheet", _
                    vbCritical + vbOKOnly, "Language Files")
    End If
    LANG_INI.Close
Else
    ret_val = LANG_HEADER_MISSING 'Langugae header field is missing
End If

Create_Lang_INI = ret_val

End Function

'*********************************************************************************************
' Function to create Language JSON file and adding language translation data in it
'*********************************************************************************************
Function Create_Lang_JSON(ByVal lang_col As Integer, ByVal file_name As String) As Integer
Dim row_counter As Integer
Dim lang_row_counter As Integer
Dim Seed_trans_col As Integer
Dim lang_col_count As Integer
Dim ret_val As Integer
Dim error_code As Integer
Dim dci_strings_start_row As Integer
Dim in_stream As New ADODB.stream
Dim out_stream As New ADODB.stream
Dim bin_stream As New ADODB.stream
Dim readfile As String
Dim lang_json_path As String
Dim common_json_path As String
Dim json_extn_path As String
Dim gz_out_file_path As String
Dim gz_exe_path As String
Dim out_folder_path As String
Dim rest_id As String
Dim index As String
Dim long_desc As String
Dim dcid_found As Boolean
Dim lang_row_empty As Boolean

Const SKIP_BOM_CONTENT = 3

Open_Curly = "{"
Close_Curly = "}"
Dot_Operator = "."
Colon_Operator = ":"
Comma_mark = ","

lang_col_count = LANG_COL_START

lang_col_count = SEED_UI_LANG_COL_START
dcid_found = False
ret_val = 0
 
While (Worksheets(SEED_DESCRIPTOR_SHEET).Cells(SEED_UI_ROW_TYPE, lang_col_count).text <> Empty And dcid_found = False)
    If (Worksheets(SEED_DESCRIPTOR_SHEET).Cells(SEED_UI_ROW_TYPE, lang_col_count).text = file_name) Then
        Seed_trans_col = lang_col_count
    ElseIf (file_name = "en") Then
        Seed_trans_col = SEED_UI_NAME_COL
        dcid_found = True
    End If
    lang_col_count = lang_col_count + 1
Wend

If (Seed_trans_col <> 0) Then
    
    'Set json file path and start of row counter
    Sheets(SEED_DESCRIPTOR_SHEET).Select
    out_folder_path = ActiveWorkbook.Path & "\" & Cells(SEED_UI_JSON_FILE_PATH_ROW, SEED_UI_JSON_FILE_PATH_COL).value & suffix
    
    If (Dir(out_folder_path) <> "") Then
         lang_json_path = out_folder_path & file_name & JSON_EXTN
         row_counter = REST_DESC_ROW_START
         
        'Create the ADO Stream Object to write to json file
        Set out_stream = CreateObject("ADODB.Stream")
        'Set properties
        out_stream.Type = adTypeText
        out_stream.Charset = "UTF-8"
        'Open stream object and write the text
        out_stream.Open
        out_stream.WriteText (Open_Curly & vbCrLf)
        
        'Get row number from where DCI strings starts
        dci_strings_start_row = LANG_SUPPORT_ROW + 1
        While (Worksheets(LANG_TRANSLATION_SHEET).Cells(dci_strings_start_row, LANG_DESC_COL).text <> DCI_STRING_ROW_SPEC)
            dci_strings_start_row = dci_strings_start_row + 1
        Wend
        dci_strings_start_row = dci_strings_start_row + 1
        
        'Write "Lang Translation" sheet data to out_stream
        While (Worksheets(REST_DESCRIPTOR_SHEET).Cells(row_counter, REST_DEFINE_COL).text <> "BEGIN_IGNORED_DATA")
            lang_row_counter = dci_strings_start_row
            dcid_found = False
            While (lang_row_empty = False And Worksheets(REST_DESCRIPTOR_SHEET).Cells(row_counter, REST_DEFINE_COL).text <> Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_DESC_COL).text)
                If (Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_DESC_COL).text <> Empty) Then
                    lang_row_counter = lang_row_counter + 1
                Else
                    lang_row_empty = True
                End If
            Wend
            lang_row_empty = False
            If (Worksheets(REST_DESCRIPTOR_SHEET).Cells(row_counter, REST_DEFINE_COL).text = Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_DESC_COL).text) Then
                dcid_found = True
            End If
            If (dcid_found = True) Then
                rest_id = Worksheets(REST_DESCRIPTOR_SHEET).Cells(row_counter, REST_PARAM_ID_COL).text
                
                While (Worksheets(REST_DESCRIPTOR_SHEET).Cells(row_counter, REST_DEFINE_COL).text = Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_DESC_COL).text)
                    If (Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_TYPE_COL).text = SHORT_NAME_STRING) Then
                    ' check for which language column to be used
                        out_stream.WriteText ("  " & Chr(34) & rest_id & Dot_Operator & "sd" & Chr(34) & Colon_Operator & " " & Chr(34) & Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, lang_col).text & Chr(34) & Comma_mark & vbCrLf)
                    ElseIf (Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_TYPE_COL).text = LONG_NAME_STRING) Then
                        long_desc = Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, lang_col).text
                        long_desc = Replace(long_desc, vbLf, " ")
                        out_stream.WriteText ("  " & Chr(34) & rest_id & Dot_Operator & "ld" & Chr(34) & Colon_Operator & " " & Chr(34) & long_desc & Chr(34) & Comma_mark & vbCrLf)
                    ElseIf (Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_TYPE_COL).text = UNITS_NAME_STRING) Then
                        ' check if NULL then don't add in json
                        If (Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, lang_col).text <> Empty) Then
                            out_stream.WriteText ("  " & Chr(34) & rest_id & Dot_Operator & "u" & Chr(34) & Colon_Operator & " " & Chr(34) & Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, lang_col).text & Chr(34) & Comma_mark & vbCrLf)
                        End If
                    ElseIf (Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_TYPE_COL).text = ENUM_NAME_STRING) Then
                        index = Mid(Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_EXTEN_COL).text, 7, 5)
                        out_stream.WriteText ("  " & Chr(34) & rest_id & Dot_Operator & "e" & Dot_Operator & index & Chr(34) & Colon_Operator & " " & Chr(34) & Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, lang_col).text & Chr(34) & Comma_mark & vbCrLf)
                    ElseIf (Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_TYPE_COL).text = BITFIELD_NAME_STRING) Then
                        index = Mid(Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_EXTEN_COL).text, 6, 5)
                        out_stream.WriteText ("  " & Chr(34) & rest_id & Dot_Operator & "b" & Dot_Operator & index & Chr(34) & Colon_Operator & " " & Chr(34) & Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, lang_col).text & Chr(34) & Comma_mark & vbCrLf)
                    End If
                    lang_row_counter = lang_row_counter + 1
                Wend
            End If
            row_counter = row_counter + 1
        Wend
        
        'Get row number from where Non-DCI strings starts and write Non-DCI strings translations to out_stream
        lang_row_counter = LANG_SUPPORT_ROW + 1
        While (Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_DESC_COL).text <> NON_DCI_STRING_ROW_SPEC)
                lang_row_counter = lang_row_counter + 1
        Wend
        lang_row_counter = lang_row_counter + 1
        While (Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_DESC_COL).text <> Empty)
            out_stream.WriteText ("  " & Chr(34) & Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, LANG_TRANS_COL_START).text & Chr(34) & Colon_Operator & " " & Chr(34) & Worksheets(LANG_TRANSLATION_SHEET).Cells(lang_row_counter, lang_col).text & Chr(34) & Comma_mark & vbCrLf)
            lang_row_counter = lang_row_counter + 1
        Wend
        
        'Write SEED UI 2.0 sheet data to out_stream
        row_counter = SEED_UI_ROW_START
        While (Worksheets(SEED_DESCRIPTOR_SHEET).Cells(row_counter, SEED_UI_TYPE_COL).text <> Empty)
            If (Worksheets(SEED_DESCRIPTOR_SHEET).Cells(row_counter, SEED_UI_TYPE_COL).text = SEED_UI_TAB) Then
                out_stream.WriteText ("  " & Chr(34) & Worksheets(SEED_DESCRIPTOR_SHEET).Cells(row_counter, SEED_UI_NAME_COL).text & Chr(34) & Colon_Operator & " " & Chr(34) & Worksheets(SEED_DESCRIPTOR_SHEET).Cells(row_counter, Seed_trans_col).text & Chr(34) & Comma_mark & vbCrLf)
            ElseIf (Worksheets(SEED_DESCRIPTOR_SHEET).Cells(row_counter, SEED_UI_TYPE_COL).text = SEED_UI_SUBTAB) Then
                out_stream.WriteText ("  " & Chr(34) & Worksheets(SEED_DESCRIPTOR_SHEET).Cells(row_counter, SEED_UI_NAME_COL).text & Chr(34) & Colon_Operator & " " & Chr(34) & Worksheets(SEED_DESCRIPTOR_SHEET).Cells(row_counter, Seed_trans_col).text & Chr(34) & Comma_mark & vbCrLf)
            ElseIf (Worksheets(SEED_DESCRIPTOR_SHEET).Cells(row_counter, SEED_UI_TYPE_COL).text = SEED_UI_GRP) Then
                out_stream.WriteText ("  " & Chr(34) & Worksheets(SEED_DESCRIPTOR_SHEET).Cells(row_counter, SEED_UI_NAME_COL).text & Chr(34) & Colon_Operator & " " & Chr(34) & Worksheets(SEED_DESCRIPTOR_SHEET).Cells(row_counter, Seed_trans_col).text & Chr(34) & Comma_mark & vbCrLf)
            ElseIf (Worksheets(SEED_DESCRIPTOR_SHEET).Cells(row_counter, SEED_UI_TYPE_COL).text = SEED_UI_TITLE) Then
                out_stream.WriteText ("  " & Chr(34) & Worksheets(SEED_DESCRIPTOR_SHEET).Cells(row_counter, SEED_UI_NAME_COL).text & Chr(34) & Colon_Operator & " " & Chr(34) & Worksheets(SEED_DESCRIPTOR_SHEET).Cells(row_counter, Seed_trans_col).text & Chr(34) & Comma_mark & vbCrLf)
            End If
            row_counter = row_counter + 1
        Wend
    
        'Path for common.json of seed ui strings
        common_json_path = ActiveWorkbook.Path & "\" & Cells(SEED_UI_COMMON_JSON_ROW, SEED_UI_JSON_FILE_PATH_COL).value & "\" & file_name & COMMON_JSON_SUFFIX
        
        'Load the data from common json and write to out_stream if file exists
        If (Dir(common_json_path) <> "") Then
            'Create the ADO Stream Object to read common.json
            Set in_stream = CreateObject("ADODB.Stream")
            'Set properties
            in_stream.Type = adTypeText
            in_stream.Charset = "UTF-8"
            'Open stream object and load the text
            in_stream.Open
            in_stream.LoadFromFile (common_json_path)
            readfile = in_stream.ReadText
            index = Mid(readfile, 3)
            out_stream.WriteText (index)
            in_stream.Close
            
            'Create the ADO Stream Object to convert from UTF-8_BOM to UTF-8_Without_BOM
            Set bin_stream = CreateObject("ADODB.Stream")
            'Set properties
            out_stream.Position = SKIP_BOM_CONTENT 'Skip BOM content
            bin_stream.Type = adTypeBinary
            'Open stream object and copy the out_stream bytes to bin_stream
            bin_stream.Open
            out_stream.CopyTo bin_stream
            'Create json file and save the stream data in it. Any number of bits can be stored in a Stream object, limited only by system resources.
            'If the Stream contains more bits than can be represented by a Long value, Size is truncated and therefore does not accurately represent the length of the Stream.
            bin_stream.SaveToFile lang_json_path, adSaveCreateOverWrite
            bin_stream.Close
            
            'Compress language json file in gzip format if compression parameter in header is 1
            If (Worksheets(LANG_TRANSLATION_SHEET).Cells(LANG_HEADER_COMPRESSION_PARAM_ROW, LANG_COMMON_HEADER_COL).text = COMPRESSION_PARAM_VAL_1) Then
                gz_exe_path = ActiveWorkbook.Path & "\" & Cells(SEED_UI_GZIP_EXE_PATH_ROW, SEED_UI_FILE_PATH_COL).value
                'Validate gzip exe path and create the compressed language file
                If (Dir(gz_exe_path) <> "") Then
                    
                    gz_out_file_path = out_folder_path & file_name & GZ_EXTN
                    
                    Dim args_str As String
                    Dim ws_shell As Object
                    Dim waitOnReturn As Boolean
                    Dim windowStyle As Integer
                    
                    waitOnReturn = True 'Wait until the command finishes execution.
                    windowStyle = MINIMIZE_SHELL 'Displays the window as a minimized window. The active window remains active.
                    
                    args_str = Chr(34) & Chr(34) & gz_exe_path & Chr(34) & " " & "-c" & " " & _
                               Chr(34) & lang_json_path & Chr(34) & " " & ">" & " " & Chr(34) & gz_out_file_path & Chr(34) & Chr(34)
                    
                    Set ws_shell = CreateObject("WScript.Shell")
                    'Command line execution returns zero when execution succeeds and non-zero when execution fails
                    error_code = ws_shell.Run("cmd.exe /c" & args_str, windowStyle, waitOnReturn)
                    If error_code = 0 Then
                        'No error, delete the json file after creating compressed(.gz) file
                        If (Dir(lang_json_path) <> "") Then
                            Kill lang_json_path
                        End If
                    Else
                        'Unable to run the shell command
                        ret_val = WS_SHELL_ERROR
                        'Display a warning message
                        Call MsgBox("Unable to run gzip tool through windows shell, cmd.exe exited with error code " & _
                                    Chr(QUOTES) & error_code & Chr(QUOTES) & vbCr, _
                                    vbCritical + vbOKOnly, "Language Files")
                    End If
                Else
                    'gzip tool does not exists
                    ret_val = LANG_GZIP_TOOL_MISSING
                    'Display a error message
                    Call MsgBox(Chr(QUOTES) & gz_exe_path & Chr(QUOTES) & " is not found. " & _
                                "Verify gzip exe path in " & Chr(QUOTES) & SEED_DESCRIPTOR_SHEET & Chr(QUOTES) & " worksheet", _
                                vbCritical + vbOKOnly, "Language Files")
                End If
            End If

        Else
            'common.json file does not exists
            ret_val = LANG_COMMON_JSON_MISSING
        End If
        out_stream.Close
    Else
        'Path of the output folder is not valid
        ret_val = LANG_OUT_FOLDER_PATH_INVALID
        'Display a warning message
        Call MsgBox("Path of the output folder is invalid " & vbCr & _
                    Chr(QUOTES) & out_folder_path & Chr(QUOTES) & vbCr & _
                    "Verify json file path in " & Chr(QUOTES) & SEED_DESCRIPTOR_SHEET & Chr(QUOTES) & " worksheet", _
                    vbCritical + vbOKOnly, "Language Files")
    End If
Else
    'Language column is not present in Seed UI 2.0 sheet
    ret_val = LANG_COL_MISSING
End If

Create_Lang_JSON = ret_val

End Function

'*********************************************************************************************
' Function to create a dateTimePickerLocaleFile...
'*********************************************************************************************
Function Create_dateTimePickerLocaleFile(locale_list)
Dim out_stream As New ADODB.stream
Dim bin_stream As New ADODB.stream
Dim datepicker_js_path As String
Dim chinese_localefile As String
Dim arabic_locale_code As String
Dim out_folder_path As String
Dim file_name As String

file_name = "dateTimePickerLocaleFile"

 Sheets(SEED_DESCRIPTOR_SHEET).Select
    out_folder_path = ActiveWorkbook.Path & "\" & Cells(SEED_UI_DATEPICKER_FILE_PATH_ROW, SEED_UI_DATEPICKER_FILE_PATH_COL).value & suffix
    
    If (Dir(out_folder_path) <> "") Then
    
        datepicker_js_path = out_folder_path & file_name & JS_EXTN
        'Create the ADO Stream Object to write to json file
        Set out_stream = CreateObject("ADODB.Stream")
        'Set properties
        out_stream.Type = adTypeText
        out_stream.Charset = "UTF-8"
        'Open stream object and write the text
        out_stream.Open
        
       For i = 0 To UBound(locale_list, 1)
       
         'Special handling for chinese language
          If locale_list(i) = "zh" Then
             chinese_locale_code = "zh-CN"
             out_stream.WriteText ("import " & locale_list(i) & " from " & """date-fns/locale/" & chinese_locale_code & """;" & vbCrLf)
          ElseIf locale_list(i) = "ar" Then
             arabic_locale_code = "ar-SA"
             out_stream.WriteText ("import " & locale_list(i) & " from " & """date-fns/locale/" & arabic_locale_code & """;" & vbCrLf)
          Else
             out_stream.WriteText ("import " & locale_list(i) & " from " & """date-fns/locale/" & locale_list(i) & """;" & vbCrLf)
          End If
       
       Next
             out_stream.WriteText (vbCrLf)
             out_stream.WriteText ("export const localeMap = {" & vbCrLf)
        
       For i = 0 To UBound(locale_list, 1)
             out_stream.WriteText ("  " & locale_list(i) & ": " & locale_list(i) & "," & vbCrLf)
       Next
             out_stream.WriteText ("}")
             out_stream.SaveToFile datepicker_js_path, adSaveCreateOverWrite
             out_stream.Close
    Else
        
        'Display a warning message
        Call MsgBox("Path of the output folder is invalid " & vbCr & _
                    Chr(QUOTES) & out_folder_path & Chr(QUOTES) & vbCr & _
                    "Verify file path in " & Chr(QUOTES) & SEED_DESCRIPTOR_SHEET & Chr(QUOTES) & " worksheet", _
                    vbCritical + vbOKOnly, "Language Files")
     
   End If
End Function
