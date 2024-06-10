VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "SeedUI"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = True
Option Explicit

'**************************
'**************************
'Modbus Construction sheet.
'**************************
'**************************
Private Type Modbus
    reg As Integer
    length As Integer
    userdoc As String
End Type

Private Type param
    define As String
    pid As Integer
    r As Integer
    w As Integer
    format As String
    datatype As String
    count As Integer
    name As String
    desc As String
    units As String
    min As Variant
    max As Variant
    enums As Variant
    enums_desc() As String
    mb() As Modbus
End Type

Private Type Assembly
    id As Integer
    name As String
    params() As Integer
End Type

Private Type UIInfo
    infoType As uiInfoType
    name As String
    param As String
    layout As String
    icon As String
    url As String
    file_name As String
End Type

Private Type GroupInfo
    name As String
    icon As String
    params() As String
End Type

Private Type TabbarInfo
    name As String
    icon As String
    layout As String
    url As String
    groups() As GroupInfo
End Type

Private Type NavbarInfo
    name As String
    icon As String
    url As String
    tabs() As TabbarInfo
End Type

Private Enum uiInfoType
    uiNavbar = 1
    uiTab
    uiSection
    uiGroup
    uiSidebar
    uiParam
    uiAction
End Enum

Private Const ENCODE_STRING = "abcdefghijklmpqrstuvwxyzABCDEFGHIJKLMPQRSTUVWXYZ"

'Update the dropdowns based on the values in the row
Private Sub Worksheet_Change(ByVal Target As Range)
    Dim row As Long
    Dim i As Long
    row = Target.row
    
    Dim webSheet As Worksheet
    Dim webDefines As Worksheet
    Set webSheet = Worksheets("Seed UI")
    Set webDefines = Worksheets("Seed UI Defines")
    
    Dim t
    
    Dim iconCol As Integer
    Dim iconCount As Integer
    iconCol = 2
    iconCount = 1
    While webDefines.Cells(iconCount + 2, iconCol) <> Empty
        iconCount = iconCount + 1
    Wend

    
    For i = 0 To Target.Rows.count - 1
            
        row = Target.row + i
        
        'Update the icon dropdown
        With webSheet.Cells(row, 4).Validation
            .Delete
            .Add Type:=xlValidateList, AlertStyle:=xlValidAlertStop, _
            Operator:=xlBetween, Formula1:="='" & webDefines.name & "'!B$2:B$" & (1 + iconCount)
            .IgnoreBlank = True
            .InCellDropdown = True
            .InputTitle = ""
            .ErrorTitle = ""
            .InputMessage = ""
            .ErrorMessage = ""
            .ShowInput = True
            .ShowError = False
        End With
        
        'Update Layout dropdown
        Set t = webDefines.Columns(1).Find(what:=webSheet.Cells(row, 1).value, lookat:=xlWhole)
        If Not t Is Nothing Then
            Dim col As Integer
            Dim colChar As String
            Dim count As Integer
            
            col = t.row + 1
            colChar = Chr$(Asc("A") - 1 + col)
            count = 0
            While webDefines.Cells(count + 2, col) <> Empty
                count = count + 1
            Wend
            
            'the reference string to the source range
            Dim strSourceRange As String
            
            'strSourceRange = "=Sheet1!A1:A" + Strings.Trim(str(intRow))
            With webSheet.Cells(row, 3).Validation
                .Delete
                If count > 0 Then
                    .Add Type:=xlValidateList, AlertStyle:=xlValidAlertStop, _
                    Operator:=xlBetween, Formula1:="='" & webDefines.name & "'!" & colChar & "2:" & colChar & (1 + count)
                    .IgnoreBlank = True
                    .InCellDropdown = True
                    .InputTitle = ""
                    .ErrorTitle = ""
                    .InputMessage = ""
                    .ErrorMessage = ""
                    .ShowInput = True
                    .ShowError = False
                End If
            End With
        
        
        End If
    Next i
    
    
End Sub


Sub Create_Web_UI()


'*********************
'******     Constants
'*********************
    Const START_ROW = 9
    
    Const REST_DESCRIP_COL = 1
    Const REST_DATA_DEFINE_COL = 2
    Const REST_PARAM_ID_COL = 3
    Const REST_R_COL = 4
    Const REST_W_COL = 5
    Const REST_FORMAT_COL = 6
    
    Const ASM_ID_COL = 7
    Const ASM_NAME_COL = 8
    Const ASM_PARAM_COL = 9
    
    Const DCI_TYPE_COL = 3
    Const DCI_COUNT_COL = 5
    Const DCI_MIN_COL = 8
    Const DCI_MAX_COL = 9
    Const DCI_ENUM_COL = 10
    Const DCI_READ_ONLY_COL = 27
    Const DCI_WRITE_COL = 53
    Const DCI_NAME_COL = 79
    Const DCI_DESC_COL = 81
    Const DCI_UNITS_COL = 83
    Const DCI_ENUM_DESC_COL = 85
    
    Const MODBUS_DATA_LENGTH_COL = 2
    Const MODBUS_REG_COL = 3
    Const MODBUS_USER_DOC_COL = 26
    
    Const WEB_START_ROW = 5
    Const WEB_TYPE_COL = 1
    Const WEB_NAME_COL = 2
    Const WEB_LAYOUT_COL = 3
    Const WEB_ICON_COL = 4
    Const WEB_URL_COL = 5
    
    Const BEGIN_IGNORED_DATA As String = "BEGIN_IGNORED_DATA"
    Dim description_string As String
    
    Dim DCIRows As New Dictionary
    
    Dim dciSheet As Worksheet
    Dim restSheet As Worksheet
    Dim webSheet As Worksheet
    Dim webDefines As Worksheet
    Dim modbusSheet As Worksheet
    Dim i As Integer, j As Integer, k As Variant
    
    
    Set dciSheet = Worksheets("DCI Descriptors")
    Set restSheet = Worksheets("REST")
    Set webSheet = Worksheets("Seed UI")
    Set webDefines = Worksheets("Seed UI Defines")
    Set modbusSheet = Worksheets("Modbus")
   
    Dim fs As New FileSystemObject
    Dim REST_DCI_JS As Scripting.TextStream
    Dim REST_TABS_JS As Scripting.TextStream
    Dim REST_VIEW_PATH As String
    
    Set REST_DCI_JS = fs.CreateTextFile(ActiveWorkbook.path & "\" & webSheet.Cells(1, 3).value, True)
    Set REST_TABS_JS = fs.CreateTextFile(ActiveWorkbook.path & "\" & webSheet.Cells(2, 3).value, True)
    REST_VIEW_PATH = ActiveWorkbook.path & "\" & webSheet.Cells(3, 3).value
    

'Create a list of all DCI objects found on the REST page, with all their information
    Dim literals As New Dictionary
    Dim counter As Integer
    Dim count As Integer
    
    
    Dim params() As param
    count = Application.WorksheetFunction.CountA(restSheet.Range("B:B"))
    
    ReDim params(0 To count)

    counter = START_ROW
    count = 0
    While (restSheet.Cells(counter, REST_DATA_DEFINE_COL).text <> Empty) And (restSheet.Cells(counter, REST_DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        Dim p As param
        Dim dci As Range, dRow As Range
        Dim ce
        Dim enumArr() As String
        
        'From REST
        p.define = restSheet.Cells(counter, REST_DATA_DEFINE_COL).value
        p.pid = restSheet.Cells(counter, REST_PARAM_ID_COL).value
        p.r = restSheet.Cells(counter, REST_R_COL).value
        p.w = restSheet.Cells(counter, REST_W_COL).value
        p.format = restSheet.Cells(counter, REST_FORMAT_COL).value
        
        'From DCI Descriptors
        Set dRow = getRow("DCI Descriptors", p.define)
        p.datatype = LCase(Mid(dRow.Cells(, DCI_TYPE_COL).value, 11))
        p.count = dRow.Cells(, DCI_COUNT_COL).value
        p.name = escapeString(dRow.Cells(, DCI_NAME_COL).text)
        p.desc = escapeString(dRow.Cells(, DCI_DESC_COL).text)
        p.units = escapeString(dRow.Cells(, DCI_UNITS_COL).text)
        p.min = dRow.Cells(, DCI_MIN_COL).value
        p.max = dRow.Cells(, DCI_MAX_COL).value
        enumArr = Split(dRow.Cells(, DCI_ENUM_COL).text, ",")
        If Len(Join(enumArr)) > 0 Then
            p.enums = Array()
            ReDim p.enums(LBound(enumArr) To UBound(enumArr))
            For i = LBound(enumArr) To UBound(enumArr)
                p.enums(i) = enumArr(i)
            Next i
        Else
            p.enums = Empty
        End If
        p.enums_desc = Split(escapeString(dRow.Cells(, DCI_ENUM_DESC_COL).text), ",")
        
        'Handle the special case where param is not writable
        If Len(Trim(dRow.Cells(, DCI_WRITE_COL).text)) = 0 Or Len(Trim(dRow.Cells(, DCI_READ_ONLY_COL).text)) <> 0 Then
            p.w = 999
        End If
        
        If (p.count) = 0 Then p.count = 1
        Call addLit(p.define, literals)
        Call addLit(p.format, literals)
        Call addLit(p.datatype, literals)
        Call addLit(p.name, literals)
        Call addLit(p.desc, literals)
        Call addLit(p.units, literals)
        Call addLit(p.pid, literals)
        Call addLit(p.r, literals)
        Call addLit(p.w, literals)
        Call addLit(p.count, literals)
        Call addLit(p.min, literals)
        Call addLit(p.max, literals)
        If Not IsEmpty(p.enums) Then
            For j = LBound(p.enums) To UBound(p.enums)
                If p.format <> "string8" Then
                    p.enums(j) = val(p.enums(j))
                End If
                Call addLit(p.enums(j), literals)
            Next j
        End If
        If Len(Join(p.enums_desc)) > 0 Then
            For j = LBound(p.enums_desc) To UBound(p.enums_desc)
                Call addLit(p.enums_desc(j), literals)
            Next j
        End If
        
        'From Modbus
        Set dci = modbusSheet.Range("B:B")
        Set dRow = dci.Find(what:=p.define, lookat:=xlWhole)
        If Not dRow Is Nothing Then
            ReDim p.mb(0 To dRow.count)
            i = 0
            For Each ce In dRow.Cells
                If Len(Trim(dci.Cells(ce.row, MODBUS_USER_DOC_COL).value)) = 0 Then
                    p.mb(i).reg = addLit(dci.Cells(ce.row, MODBUS_REG_COL).value, literals)
                    p.mb(i).length = addLit(dci.Cells(ce.row, MODBUS_DATA_LENGTH_COL).value, literals)
                    i = i + 1
                End If
            Next
            If i = 0 Then
                ReDim p.mb(-1 To -1)
            Else
                ReDim Preserve p.mb(0 To i - 1)
            End If
        Else
            ReDim p.mb(-1 To -1)
        End If
                    
        params(count) = p
        counter = counter + 1
        count = count + 1
    Wend
    ReDim Preserve params(0 To count - 1)

    
'Create a list of all Tabs on the Web UI page
    Dim uiinfos() As UIInfo
    Dim firstRow
    Set dci = webSheet.Range("A:A")
    count = Application.WorksheetFunction.CountA(dci)
    ReDim uiinfos(1 To count - 1)
    
    counter = WEB_START_ROW
    count = 0
    'Find the first row, which is the header
    Set dRow = dci.Find(what:="*", lookat:=xlWhole)
    'Save the header row so we know when we loop around
    firstRow = dRow.row
    'Loop through remaining until none left
    Set dRow = dci.Find(what:="*", After:=dRow, lookat:=xlWhole)
    Do While Not dRow.row = firstRow
        Dim defineRow As Range
        Set defineRow = webDefines.Columns(1).Find(what:=dRow, lookat:=xlWhole)
        If Not defineRow Is Nothing Then
            count = count + 1
            uiinfos(count).infoType = defineRow.row - 1
            uiinfos(count).name = dRow.Cells(, WEB_NAME_COL).value
            uiinfos(count).layout = dRow.Cells(, WEB_LAYOUT_COL).value
            uiinfos(count).icon = dRow.Cells(, WEB_ICON_COL).value
            uiinfos(count).url = dRow.Cells(, WEB_URL_COL).value
                        
            'Split string into [define].[index]
            Dim strs
            strs = Split(uiinfos(count).name, "[")
            If UBound(strs) >= 0 Then
                If Not getRow("REST", CStr(strs(0))) Is Nothing Then
                    strs(0) = getRow("REST", CStr(strs(0))).Cells(, REST_PARAM_ID_COL).value
                    uiinfos(count).param = Join(strs, "[")
                End If
            End If
            If uiinfos(count).url <> "" Then
                uiinfos(count).file_name = "tab-" & Mid$(uiinfos(count).url, InStrRev(uiinfos(count).url, "/") + 1)
            End If
        End If
        
        Set dRow = dci.Find(what:="*", After:=dRow, lookat:=xlWhole)
    Loop
    If (count > 0) Then
        ReDim Preserve uiinfos(1 To count)
    End If

'Create a list of the assemblies and what DCI values they hold
    Dim asms() As Assembly
    Dim pCount As Integer
    
    count = Application.WorksheetFunction.CountA(restSheet.Range("G:G"))
    
    ReDim asms(0 To count)
    
    counter = START_ROW
    count = 0
    Set dci = restSheet.Range("G:G")
    'Find the first row, which is the header
    Set dRow = dci.Find(what:="*", lookat:=xlWhole)
    'Save the header row so we know when we loop around
    firstRow = dRow.row
    'Loop through remaining until none left
    Set dRow = dci.Find(what:="*", After:=dRow, lookat:=xlWhole)
    Do While Not dRow Is Nothing
        If IsNumeric(dRow.value) Then
            asms(count).id = dRow.Cells(, 1).value
            asms(count).name = dRow.Cells(, 2).value
            Call addLit(asms(count).id, literals)
            Call addLit(asms(count).name, literals)
            pCount = 0
            Do
                pCount = pCount + 1
                ReDim Preserve asms(count).params(1 To pCount)
                asms(count).params(pCount) = getRow("REST", dRow.Cells(pCount, 3)).Cells(, REST_PARAM_ID_COL).value
                Call addLit(asms(count).params(pCount), literals)
            Loop While (dRow.Cells(pCount + 1, 1) = Empty And dRow.Cells(pCount + 1, 3) <> Empty)
            
            If (pCount = 0) Then
                MsgBox ("Assembly " & asms(count).name & " has no parameters!")
                End
            Else
                count = count + 1
                
            End If
        End If
        Set dRow = dci.Find(what:="*", After:=dRow, lookat:=xlWhole)
        If dRow.row = firstRow Then Exit Do
    Loop
    If (count > 0) Then
        ReDim Preserve asms(0 To count - 1)
    End If

'****************************************************************************************************************************
'*****      Add JS description of DCI output
'****************************************************************************************************************************
    Dim txtArray() As String
    txtArray = convertToArray(literals)
    
    REST_DCI_JS.WriteLine ("(function(){")
    REST_DCI_JS.WriteLine ("var m = [" & Join(txtArray, ",") & "],")
    REST_DCI_JS.WriteLine ("b = '" & ENCODE_STRING & "', c=b.length,")
    REST_DCI_JS.WriteLine ("i = 0,k = function(x) {")
    REST_DCI_JS.WriteLine ("    if (!x--) return '';")
    REST_DCI_JS.WriteLine ("    return k(Math.floor(x/c))+b[x%c];")
    REST_DCI_JS.WriteLine ("}")
    REST_DCI_JS.WriteLine ("while (m.length) {")
    REST_DCI_JS.WriteLine ("    m[k(++i)] = m.shift(); ")
    REST_DCI_JS.WriteLine ("}")
    
    REST_DCI_JS.WriteLine ("// Using 'with' below to make things MUCH smaller, i.e. a=m[0], b=m[1] ... aa=m[52] ...")
    REST_DCI_JS.WriteLine ("with (m) {")
    
    REST_DCI_JS.WriteLine ("// DCI array contains object with keys: define, pid, r, w, format, datatype, count, name, desc, units, min, max, enum, enum_desc, modbus")
    REST_DCI_JS.WriteLine ("// [modbus] array contains object with keys: reg, length, start")
    REST_DCI_JS.WriteLine ("var _ = function (a, menum, modbus){")
    REST_DCI_JS.WriteLine ("    var obj = {define:a[0],pid:a[1],r:a[2],w:a[3],format:a[4],datatype:a[5],count:a[6],name:a[7],desc:a[8],units:a[9]};")
    REST_DCI_JS.WriteLine ("    if (menum) {")
    REST_DCI_JS.WriteLine ("        if (Array.isArray(menum[0])) {")
    REST_DCI_JS.WriteLine ("            obj.enum=menum[0];")
    REST_DCI_JS.WriteLine ("            obj.enum_desc=menum[1]||menum[0];")
    REST_DCI_JS.WriteLine ("        } else {")
    REST_DCI_JS.WriteLine ("            obj.min=menum[0];")
    REST_DCI_JS.WriteLine ("            obj.max=menum[1];")
    REST_DCI_JS.WriteLine ("        }")
    REST_DCI_JS.WriteLine ("    }")
    REST_DCI_JS.WriteLine ("    if (modbus) {")
    REST_DCI_JS.WriteLine ("        var b=obj.modbus=[];")
    REST_DCI_JS.WriteLine ("        for (var i=0;i<modbus.length;i++) {")
    REST_DCI_JS.WriteLine ("            var c=modbus[i];")
    REST_DCI_JS.WriteLine ("            b[i]={reg:c[0],length:c[1],start:c[2]||0};")
    REST_DCI_JS.WriteLine ("        }")
    REST_DCI_JS.WriteLine ("    }")
    REST_DCI_JS.WriteLine ("    return obj;")
    REST_DCI_JS.WriteLine ("};")
    
    REST_DCI_JS.WriteLine ("window.DCI=[")
    For i = 0 To UBound(params)
        Dim a As String
        Dim menum As String
        Dim mbArg As String
        Dim mbArr() As String
        
        p = params(i)
        menum = ""
        mbArg = ""
        
        Dim x() As String
        a = "[" & Join(Array(convertToRefs(p.define, literals), _
                   convertToRefs(p.pid, literals), _
                   convertToRefs(p.r, literals), _
                   convertToRefs(p.w, literals), _
                   convertToRefs(p.format, literals), _
                   convertToRefs(p.datatype, literals), _
                   convertToRefs(p.count, literals), _
                   convertToRefs(p.name, literals), _
                   convertToRefs(p.desc, literals), _
                   convertToRefs(p.units, literals)), ",") & "]"
        If Not IsEmpty(p.min) Or Not IsEmpty(p.max) Then
            If IsEmpty(p.min) Then
                menum = "[undefined," & convertToRefs(p.max, literals) & "]"
            ElseIf IsEmpty(p.max) Then
                menum = "[" & convertToRefs(p.min, literals) & ",undefined]"
            Else
                menum = "[" & convertToRefs(p.min, literals) & "," & convertToRefs(p.max, literals) & "]"
            End If
        ElseIf Not IsEmpty(p.enums) Then
            For j = LBound(p.enums) To UBound(p.enums)
                p.enums(j) = convertToRefs(p.enums(j), literals)
            Next j
            If Len(Join(p.enums_desc)) > 0 Then
                For j = 0 To UBound(p.enums_desc)
                    p.enums_desc(j) = convertToRefs(p.enums_desc(j), literals)
                Next j
                menum = "[[" & Join(p.enums, ",") & "],[" & Join(p.enums_desc, ",") & "]]"
            Else
                menum = "[[" & Join(p.enums, ",") & "]]"
            End If
        End If
        
        If (UBound(p.mb) >= 0) Then
            ReDim mbArr(0 To UBound(p.mb))
            For j = 0 To UBound(p.mb)
                Dim mb As Modbus
                mb = p.mb(j)
                mbArr(j) = "[" & Join(Array(convertToRefs(mb.reg, literals), convertToRefs(mb.length, literals)), ",") & "]"
            Next j
            mbArg = "[" & Join(mbArr, ",") & "]"
        End If
        
        If Len(mbArg) = 0 Then
            If Len(menum) = 0 Then
                REST_DCI_JS.WriteLine ("_(" & a & "),")
            Else
                REST_DCI_JS.WriteLine ("_(" & a & "," & menum & "),")
            End If
        Else
            If Len(menum) = 0 Then
                REST_DCI_JS.WriteLine ("_(" & a & ",0," & mbArg & "),")
            Else
                REST_DCI_JS.WriteLine ("_(" & a & "," & menum & "," & mbArg & "),")
            End If
        End If
    Next i
    REST_DCI_JS.WriteLine ("];")
    
'****************************************************************************************************************************
'*****      Add JS description of WEBUI output
'****************************************************************************************************************************
    
     Dim t As TabbarInfo
     Dim g As GroupInfo
    
'    REST_DCI_JS.WriteLine ("// WEBUI array containing the following objects, representing the navbar:")
'    REST_DCI_JS.WriteLine ("//  {")
'    REST_DCI_JS.WriteLine ("//        name:")
'    REST_DCI_JS.WriteLine ("//        icon:")
'    REST_DCI_JS.WriteLine ("//        url:")
'    REST_DCI_JS.WriteLine ("//        tabs: [{")
'    REST_DCI_JS.WriteLine ("//              name:")
'    REST_DCI_JS.WriteLine ("//              icon:")
'    REST_DCI_JS.WriteLine ("//              url:")
'    REST_DCI_JS.WriteLine ("//              layout:")
'    REST_DCI_JS.WriteLine ("//              groups: [{")
'    REST_DCI_JS.WriteLine ("//                    name:")
'    REST_DCI_JS.WriteLine ("//                    icon:")
'    REST_DCI_JS.WriteLine ("//                    pid: [pid]")
'    REST_DCI_JS.WriteLine ("//              }]")
'    REST_DCI_JS.WriteLine ("//        }]")
'    REST_DCI_JS.WriteLine ("//  }")
'    REST_DCI_JS.WriteLine ("_ = function (a){")
'    REST_DCI_JS.WriteLine ("    var ret={name:a[2],icon:a[3],url:a[4],layout:a[5]};")
'    REST_DCI_JS.WriteLine ("    ret[a[0]]=a[1];")
'    REST_DCI_JS.WriteLine ("    return ret;")
'    REST_DCI_JS.WriteLine ("};")
'
'    REST_DCI_JS.WriteLine ("window.WEBUI=[")
'    For i = LBound(navs) To UBound(navs)
'        a = "_([" & convertToRefs("tabs", literals) & ",["
'        For j = LBound(navs(i).tabs) To UBound(navs(i).tabs)
'            t = navs(i).tabs(j)
'            a = a & "_([" & convertToRefs("groups", literals) & ",["
'
'            For k = LBound(t.groups) To UBound(t.groups)
'                g = t.groups(k)
'                a = a & "_([" & convertToRefs("pid", literals) & ",["
'                a = a & Join(convertToRefs(g.params, literals), ",")
'                a = a & "]," & Join(Array(convertToRefs(g.name, literals), convertToRefs(g.icon, literals)), ",") & "]),"
'            Next k
'            a = a & "]," & Join(Array(convertToRefs(t.name, literals), convertToRefs(t.icon, literals), convertToRefs(t.url, literals), convertToRefs(t.layout, literals)), ",") & "]),"
'
'        Next j
'        a = a & "]," & Join(Array(convertToRefs(navs(i).name, literals), convertToRefs(navs(i).icon, literals), convertToRefs(navs(i).url, literals)), ",") & "]),"
'        REST_DCI_JS.WriteLine (a)
'    Next i
'    REST_DCI_JS.WriteLine ("];")
    
'****************************************************************************************************************************
'*****      Add JS description of ASSEMBLY output
'****************************************************************************************************************************
    
    
    REST_DCI_JS.WriteLine ("// ASM array contains object with keys: id, name, pid")
    REST_DCI_JS.WriteLine ("// [pid] is an array of DCI pid")
    REST_DCI_JS.WriteLine ("_ = function (a){")
    REST_DCI_JS.WriteLine ("    return {id:a[0],name:a[1],pid:a[2]};")
    REST_DCI_JS.WriteLine ("};")
    
    REST_DCI_JS.WriteLine ("window.ASM=[")
    For i = 0 To UBound(asms)
        a = "[" & Join(Array(asms(i).id, convertToRefs(asms(i).name, literals), "[" & Join(convertToRefs(asms(i).params, literals), ",") & "]"), ",") & "]"
        REST_DCI_JS.WriteLine ("_(" & a & "),")
    Next i
    REST_DCI_JS.WriteLine ("];")
    
    
    REST_DCI_JS.Write ("}")
    REST_DCI_JS.Write ("})();")
    
    
    
    
'****************************************************************************************************************************
'*****      Write tabs.js
'****************************************************************************************************************************
    REST_TABS_JS.WriteLine ("'use strict';")
    REST_TABS_JS.WriteLine ("(function(){")
    REST_TABS_JS.WriteLine ("    //This file was generated by the LTK DCI DB Creator Web UI Tab")
    REST_TABS_JS.WriteLine ("    var nav, tab, section, group, param;")
    REST_TABS_JS.WriteLine ("    ")
    'Write the tab cascade
    For i = LBound(uiinfos) To UBound(uiinfos)
        Dim info As UIInfo
        info = uiinfos(i)
        
        Dim infoArray As Variant
        infoArray = Empty
        
        Select Case info.infoType
            Case uiInfoType.uiNavbar 'Nav
                REST_TABS_JS.Write ("    nav = navbar")
                infoArray = Array(info.name, info.layout, info.icon, info.url)
            Case uiInfoType.uiTab 'Tab
                REST_TABS_JS.Write ("    tab = nav")
                infoArray = Array(info.name, info.file_name, info.icon, info.url)
        End Select
        
        If Not IsEmpty(infoArray) Then
            'infoArray = Array(info.name, info.layout, info.icon, info.url)
            While infoArray(UBound(infoArray)) = Empty
                ReDim Preserve infoArray(LBound(infoArray) To UBound(infoArray) - 1)
            Wend
            
            For j = LBound(infoArray) To UBound(infoArray)
                infoArray(j) = IIf(infoArray(j) = Empty, "undefined", "'" & infoArray(j) & "'")
            Next j
            If Len(info.param) <> 0 Then
                infoArray(LBound(infoArray)) = info.param
            End If
            
            REST_TABS_JS.WriteLine (".create(" & Join(infoArray, ",") & ");")
        End If
    Next i
    
    REST_TABS_JS.WriteLine ("    ")
    REST_TABS_JS.WriteLine ("})();")
    
    
'****************************************************************************************************************************
'*****      Write each tab-view
'****************************************************************************************************************************

    Dim templateHeader As String
    Dim templateFooter As String
    Dim templateLine As String
    
    For i = LBound(uiinfos) To UBound(uiinfos)
        If (uiinfos(i).infoType) = 2 Then
            Dim TAB_FILE As Scripting.TextStream
            Dim TAB_NAME As String
            'TAB_NAME = uiinfos(i).layout
            TAB_NAME = uiinfos(i).file_name
            Set TAB_FILE = fs.CreateTextFile(REST_VIEW_PATH & "\" & TAB_NAME & ".html", True)
            TAB_FILE.WriteLine ("<dom-module id=""" & TAB_NAME & """>")
            TAB_FILE.WriteLine ("    <template>")
            Call writeTag(TAB_FILE, uiinfos, i, 1)
            TAB_FILE.WriteLine ("    </template>")
            TAB_FILE.WriteLine ("    <script>")
            TAB_FILE.WriteLine ("        Polymer({")
            TAB_FILE.WriteLine ("            is: '" & TAB_NAME & "'")
            TAB_FILE.WriteLine ("        });")
            TAB_FILE.WriteLine ("    </script>")
            TAB_FILE.WriteLine ("</dom-module>")
        End If
    Next i
    
    
'    REST_TABS_JS.WriteLine ("    var dummyInit = function(el, data) {")
'    REST_TABS_JS.WriteLine ("        return data;")
'    REST_TABS_JS.WriteLine ("    };")
'
'    'Write all the unique components included
'    Dim layouts() As String
'    ReDim layouts(1 To 1)
'    For i = LBound(uiinfos) To UBound(uiinfos)
'        Dim isNovel As Boolean
'
'        info = uiinfos(i)
'        isNovel = True
'        Select Case info.infoType
'            Case 3, 4, 5 'Section, Group, Param
'                For j = LBound(layouts) To UBound(layouts)
'                    If info.layout = layouts(j) Then
'                        isNovel = False
'                        Exit For
'                    End If
'                Next j
'                If isNovel Then
'                    ReDim Preserve layouts(1 To UBound(layouts) + 1)
'                    layouts(UBound(layouts)) = info.layout
'                    REST_TABS_JS.WriteLine ("    PB.createComponent(""" & info.layout & ", dummyInit);")
'                End If
'        End Select
'
'    Next i
    
    
    
    
    'For Each k In literals.keys
    '    Debug.Print refString(literals.Item(k)), k
    'Next k
    
End Sub

'Write out each tag recursively until we hit a tab or end of array
Private Function writeTag(ByRef file As Scripting.TextStream, ByRef uiinfos() As UIInfo, ByVal index As Integer, ByVal minType As Integer) As Integer
    If index > UBound(uiinfos) Then
        writeTag = 0
        Exit Function
    ElseIf uiinfos(index).infoType <= minType Then
        writeTag = 0
        Exit Function
    End If
    
    Dim info As UIInfo
    info = uiinfos(index)
    
    file.Write (String(4 * (minType + 1), "    ") & "<" & info.layout)
    If Len(info.param) <> 0 Then
        file.Write (" param='" & info.param & "'")
    Else
        If info.name <> "" Then
            file.Write (" label='" & info.name & "'")
        End If
        If info.icon <> "" Then
            file.Write (" icon='" & info.icon & "'")
        End If
    End If
    file.WriteLine (">")
    
    Dim i As Integer
    Dim delta As Integer
    i = 1
    delta = 1
    
    Do While delta <> 0
        If index + i > UBound(uiinfos) Then
            writeTag = 0
            Exit Do
        ElseIf uiinfos(index + i).infoType <= minType Then
        End If
        delta = writeTag(file, uiinfos, index + i, info.infoType)
        i = i + delta
    Loop
    file.WriteLine (String(4 * (minType + 1), "    ") & "</" & uiinfos(index).layout & ">")
    
    writeTag = i
End Function


'Escapes the string from quotes
Function escapeString(ByVal str As String) As String
    escapeString = Replace(Replace(Replace(str, "'", "\'"), vbCr, " "), vbLf, " \n")
End Function

'Returns the reference string, based on its index in the table
Function refString(ByVal x As Integer) As String
    Dim b As String
    Dim c As Integer
    
    b = ENCODE_STRING
    c = Len(b)
    If (0 = x) Then
        refString = ""
    Else
        refString = refString(Int((x - 1) / c)) & Mid(b, ((x - 1) Mod c) + 1, 1)
    End If
End Function

'Adds the string to the dictionary
Function addLit(str, ByRef literals As Dictionary)
    Dim litVal As Integer
    Dim arr
    Dim s
    If VarType(str) = vbString Then
        arr = splitString(str)
    ElseIf VarType(str) = vbEmpty Then
        arr = Array(str)
        Exit Function
    Else
        arr = Array(str)
    End If
    
    For Each s In arr
        If literals.Exists(s) Then
            litVal = literals.Item(s)
            literals.Remove s
            literals.Add s, litVal + 1
        Else
            literals.Add s, 1
        End If
    Next s
    addLit = str
End Function


' Converts the String->count dictionary into a String->index dictionary
'   and returns the new array
'   Strings that have a count of 1 are discarded
Function convertToArray(ByRef literals As Dictionary) As String()
    Dim s, litVal As Integer
    Dim ret() As String
    Dim endsWithDelim As Boolean
    Dim minLen
    ReDim ret(0 To literals.count - 1)
    
    Dim i As Integer
    Dim count As Integer
    i = 0
    count = 0
    
    Set literals = SortDictionary(literals)
    
    For Each s In literals.keys
        litVal = literals.Item(s)
        literals.Remove s
        'Normal: "abcdef"
        'Middle: ""+ab+""
        'End:    ""+ab
        If VarType(s) = vbString Then
            If Len(s) = 0 Then
                'Special case where a single letter is smaller than ''
                minLen = -2
            ElseIf (" " = Right(s, 1) Or "_" = Right(s, 1)) Then
                minLen = 4
            Else
                minLen = 2
            End If
            If (litVal > 1 And minLen + Len("" & refString(i + 1)) <= Len("" & s)) Then
                i = i + 1
                count = count + litVal
                literals.Add s, i
                ret(i - 1) = "'" & s & "'"
            End If
        Else
            minLen = 1
            If (litVal > 1 And minLen + Len("" & refString(i + 1)) <= Len("" & s)) Then
                i = i + 1
                count = count + litVal
                literals.Add s, i
                ret(i - 1) = s
            End If
        End If
        
        
    Next s
    
    ReDim Preserve ret(0 To literals.count - 1)
    convertToArray = ret

End Function

'Converts the string to an element of the literal reference table
'   Also adds quotes if needed
Function convertToRefs(str, ByRef literals As Dictionary)
    Dim i As Integer
    If (VarType(str) And vbArray) > 0 Then
        Dim retArr As Variant
        retArr = Array()
        ReDim retArr(LBound(str) To UBound(str))
        For i = LBound(str) To UBound(str)
            retArr(i) = convertToRefs(str(i), literals)
        Next i
        convertToRefs = retArr
        Exit Function
    ElseIf VarType(str) <> vbString Then
        If literals.Exists(str) Then
            convertToRefs = refString(literals.Item(str))
        Else
            convertToRefs = str
        End If
        Exit Function
    ElseIf Len(str) = 0 Then
        If literals.Exists(str) Then
            convertToRefs = refString(literals.Item(str))
        Else
            convertToRefs = "''"
        End If
        Exit Function
    End If
    
    
    Dim pieces() As String
    Dim arr() As Variant
    Dim lb As Integer, ub As Integer
    
    
    
    'ret = +" "+m[12]
    'ret = +" 123456"
    
    pieces = splitString(str)
    
    lb = LBound(pieces)
    ub = UBound(pieces)
    ReDim arr(lb To ub)
    
    For i = lb To ub
        If literals.Exists(pieces(i)) Then
            arr(i) = literals.Item(pieces(i))
        Else
            arr(i) = pieces(i)
        End If
    Next i
    
    Dim ret As String
    Dim lastWasString As Boolean
    If TypeName(arr(lb)) = "String" Then
        ret = "'" & arr(lb)
        lastWasString = True
    Else
        ret = refString(arr(lb))
        lastWasString = False
    End If
    
    For i = lb + 1 To ub
        If TypeName(arr(i)) = "String" Then
            If Not lastWasString Then
                ret = ret & "+'"
            End If
            ret = ret & arr(i)
            lastWasString = True
        Else
            If lastWasString Then
                ret = ret & "'"
            End If
            ret = ret & "+" & refString(arr(i))
            lastWasString = False
        End If
    Next i
    
    If lastWasString Then
        ret = Trim(ret) & "'"
    End If
    
    convertToRefs = ret
End Function

'Special function to split a string into somewhat large chunks
Function splitString(str) As String()
    Dim pieces() As String
    Dim ret() As String
    Dim i As Integer, j As Integer
    Dim lastS
    Dim s
    Dim delim As String
    delim = " "
    
    If Len(str) = 0 Then
        ReDim ret(0 To 0)
        splitString = ret
        Exit Function
    End If
    
    pieces = Split(str, delim)
    If LBound(pieces) = UBound(pieces) Then
        delim = "_"
        pieces = Split(str, delim)
    End If
    
    ReDim ret(LBound(pieces) To UBound(pieces))
    i = LBound(pieces)  'i is the next available index
    
    For j = LBound(pieces) To UBound(pieces)
        s = pieces(j)
        If lastS <> "" Then
            s = lastS & delim & s
        End If
        
        If Len(s) <= 4 Then
            lastS = s
        Else
            If j = UBound(pieces) Then
                ret(i) = s
            Else
                ret(i) = s & delim
            End If
            i = i + 1
            lastS = ""
        End If
    Next j
    If lastS <> "" Then
        ret(i) = lastS
    Else
        i = i - 1
    End If
        
    ReDim Preserve ret(LBound(pieces) To i)
    splitString = ret
End Function

'Sorts dictionary by converting to an array first
Function SortDictionary(dict As Dictionary)
    Dim arrTemp()
    Dim curKey(1 To 2) As Variant
    Dim itX As Integer
    Dim itY As Integer

    'Populate the array
    ReDim arrTemp(dict.count, 1 To 2)
    itX = 0
    For Each curKey(1) In dict.keys
        arrTemp(itX, 1) = curKey(1)
        arrTemp(itX, 2) = dict.Item(curKey(1))
        itX = itX + 1
    Next

    'Do the sort in the array
    For itX = 0 To (dict.count - 2)
        For itY = (itX + 1) To (dict.count - 1)
            If arrTemp(itX, 2) < arrTemp(itY, 2) Then
                curKey(1) = arrTemp(itY, 1)
                curKey(2) = arrTemp(itY, 2)
                arrTemp(itY, 1) = arrTemp(itX, 1)
                arrTemp(itY, 2) = arrTemp(itX, 2)
                arrTemp(itX, 1) = curKey(1)
                arrTemp(itX, 2) = curKey(2)
            End If
        Next
    Next

    'Create the new dictionary
    Set SortDictionary = New Dictionary
    For itX = 0 To (dict.count - 1)
        SortDictionary.Add arrTemp(itX, 1), arrTemp(itX, 2)
    Next
End Function

Function getRow(ByRef worksheetName As String, ByRef dciName As String)
    Dim grDCI, grRow
    Set grDCI = Worksheets(worksheetName).Range("B:B")
    Set grRow = grDCI.Find(what:=dciName, lookat:=xlWhole)
    If (grRow Is Nothing) Then
        'MsgBox ("Could not find " & dciName & " in REST")
        'Stop
        Set getRow = Nothing
    Else
        Set getRow = grRow.EntireRow
    End If
End Function

    
Private Sub CommandButton1_Click()
    Call Create_Web_UI
End Sub


