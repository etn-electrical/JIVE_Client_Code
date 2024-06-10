Attribute VB_Name = "ModbusMap_Builder"
Option Explicit
'**************************
'**************************
'Modbus Construction sheet.
'**************************
'**************************

Sub Create_Modbus_Map()


'*********************
'******     Constants
'*********************
    Const START_ROW = 9
    
    Const DESCRIP_COL = 1
    Const DATA_DEFINE_COL = 2
    Const DATA_LENGTH_COL = 3
    Const REGISTER_COL = 4
    Const DATA_START_COL = 8
    Const USER_DOC_IGNORE_COL = 27
    
    Const BEGIN_IGNORED_DATA As String = "BEGIN_IGNORED_DATA"
    Dim sheet_name As String
    Dim description_string As String
    
    Dim DCIRows As New Dictionary
    
    sheet_name = "Modbus"
    Dim sh As Sheet14
    Set sh = Sheets(sheet_name)

    Dim fs As New FileSystemObject
    Dim file_path As String
    Dim REST_DCI_JS As Scripting.TextStream
    file_path = ActiveWorkbook.path & "\" & "..\Code\Web\txt2hex2\www\js\ModbusMap.js"
    Set REST_DCI_JS = fs.CreateTextFile(file_path, True)

    'Create two dictionaries
    '   One that holds the description for each modbus register, keying on title
    '   One that holds each part of the description, keying on the part of the description
    
    Dim obj As New Dictionary       'String -> (description)
    Dim literals As New Dictionary  'String -> count
    Dim counter As Integer
    
    Dim types(0 To 10) As String
    types(0) = "STRING"
    types(1) = "INT16"
    types(2) = "INT32"
    types(3) = "FLOAT"
    

    counter = START_ROW
    While (sh.Cells(counter, DATA_DEFINE_COL).text <> Empty) And (sh.Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If Len(sh.Cells(counter, DESCRIP_COL).value) > 0 And sh.Cells(counter, USER_DOC_IGNORE_COL).value = "" Then
            Dim arr() As Variant
            Dim key As String
            Dim ub As Integer
            key = sh.Cells(counter, DESCRIP_COL).value
            
            If obj.Exists(key) Then
                arr = obj.Item(key)
                obj.Remove key
                ub = UBound(arr)
            Else
            
                Dim litVal As Integer
                Dim s
                For Each s In splitString(key)
                    If literals.Exists(s) Then
                        litVal = literals.Item(s)
                        literals.Remove s
                        literals.Add s, litVal + 1
                    Else
                        literals.Add s, 1
                    End If
                Next s
                
                ReDim arr(0)
                ub = -1
            End If
            
            ReDim Preserve arr(ub + 3)
            
            arr(ub + 1) = sh.Cells(counter, REGISTER_COL).value
            arr(ub + 2) = sh.Cells(counter, DATA_LENGTH_COL).value
            arr(ub + 3) = getDataType(sh.Cells(counter, DATA_DEFINE_COL).value)
            If (arr(ub + 3) = "") Then
                arr(ub + 3) = 0
            End If
            
            obj.Add key, arr
        End If
        counter = counter + 1
    Wend
    
    Dim txtArray() As String
    txtArray = convertToArray(literals)
    
    REST_DCI_JS.WriteLine ("function MM() {")
    REST_DCI_JS.WriteLine ("//this.o is an array of modbus values.")
    REST_DCI_JS.WriteLine ("// [0] Description")
    REST_DCI_JS.WriteLine ("// [n+1] n'th Modbus Address")
    REST_DCI_JS.WriteLine ("// [n+2] n'th Byte Length")
    REST_DCI_JS.WriteLine ("// [n+3] n'th Data Type [STRING, BOOL, BYTE, UINT8, UINT16, UINT32, SINT32, FLOAT]")
    
    REST_DCI_JS.WriteLine ("var m = [""" & Join(txtArray, """,""") & """];")
    
    Dim k, val(), line As String
    
    REST_DCI_JS.WriteLine ("this.o = [")
    For Each k In obj.keys
        Dim i As Integer
        val = obj.Item(k)
        line = "[" & convertToRefs(k, literals) & "," & Join(val, ",") & "],"
        REST_DCI_JS.WriteLine (line)
    Next
    REST_DCI_JS.WriteLine ("];")
    
    REST_DCI_JS.WriteLine ("}")
    
    
End Sub

' Converts the String->count dictionary into a String->index dictionary
'   and returns the new array
'   Strings that have a count of 1 are discarded
Function convertToArray(ByRef literals As Dictionary) As String()
    Dim s, litVal As Integer
    Dim ret() As String
    ReDim ret(0 To literals.count - 1)
    
    Dim i As Integer
    Dim count As Integer
    i = 0
    count = 0
    
    Set literals = SortDictionary(literals)
    
    For Each s In literals.keys
        litVal = literals.Item(s)
        literals.Remove s
        If (litVal > 1 And Len(s) >= 7) Then
            count = count + litVal
            literals.Add s, i
            ret(i) = s
            i = i + 1
        End If
    Next s
    
    ReDim Preserve ret(0 To literals.count - 1)
    convertToArray = ret

End Function

Function convertToRefs(str, ByRef literals As Dictionary) As String
    Dim pieces() As String
    Dim arr() As Variant
    Dim lb As Integer, ub As Integer
    Dim i As Integer
    
    
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
        ret = """" & arr(lb)
        lastWasString = True
    Else
        ret = "m[" & arr(lb) & "]"
        lastWasString = False
    End If
    
    For i = lb + 1 To ub
        If TypeName(arr(i)) = "String" Then
            If Not lastWasString Then
                ret = ret & "+"""
            End If
            ret = ret & arr(i)
            lastWasString = True
        Else
            If lastWasString Then
                ret = ret & """"
            End If
            ret = ret & "+m[" & arr(i) & "]"
            lastWasString = False
        End If
    Next i
    
    If lastWasString Then
        ret = Trim(ret) & """"
    End If
    
    convertToRefs = ret
End Function

'Special function to split a string into somewhat large chunks
Function splitString(str) As String()
    Dim pieces() As String
    Dim ret() As String
    Dim i As Integer
    Dim lastS
    Dim s
    
    pieces = Split(str)
    ReDim ret(LBound(pieces) To UBound(pieces))
    i = LBound(pieces)  'i is the next available index
    
    For Each s In Split(str)
        If lastS <> "" Then
            s = lastS & " " & s
        End If
        
        If Len(s) < 6 Then
            lastS = s
        Else
            ret(i) = s & " "
            i = i + 1
            lastS = ""
        End If
    Next s
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

Function getDataType(define) As Integer
    'Look up the DATATYPE
    Dim r, row
    
    Set r = Worksheets("DCI Descriptors").Range("B:B")
    Set row = r.Find(what:=define, lookat:=xlWhole)
    If row Is Nothing Then
        getDataType = 0
    Else
        Dim value
        value = r.Cells(row.row, 2).value
        Select Case value
            Case "DCI_DTYPE_STRING8":
                getDataType = 0
            Case "DCI_DTYPE_BOOL":
                getDataType = 1
            Case "DCI_DTYPE_BYTE":
                getDataType = 2
            Case "DCI_DTYPE_UINT8":
                getDataType = 3
            Case "DCI_DTYPE_UINT16":
                getDataType = 4
            Case "DCI_DTYPE_UINT32":
                getDataType = 5
            Case "DCI_DTYPE_SINT32":
                getDataType = 6
            Case "DCI_DTYPE_FLOAT":
                getDataType = 7
            Case Else
                getDataType = 9
        End Select
    End If
                
            
            
End Function
