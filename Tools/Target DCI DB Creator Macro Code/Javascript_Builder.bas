Attribute VB_Name = "Javascript_Builder"
Option Explicit
'**************************
'**************************
'Modbus Construction sheet.
'**************************
'**************************

Sub Create_JS_DCI()


'*********************
'******     Constants
'*********************
    Const START_ROW = 9
    
    Const DESCRIP_COL = 1
    Const DATA_DEFINE_COL = 2
    Const PARAM_ID_COL = 3
    Const R_COL = 4
    Const W_COL = 5
    Const FORMAT_COL = 6
    Const JS_COL = 9
    
    Const BEGIN_IGNORED_DATA As String = "BEGIN_IGNORED_DATA"
    Dim sheet_name As String
    Dim description_string As String
    
    Dim DCIRows As New Dictionary
    
    sheet_name = ActiveSheet.name
    Sheets(sheet_name).Select

    Dim fs As New FileSystemObject
    Dim file_path As String
    Dim REST_DCI_JS As Scripting.TextStream
    file_path = ActiveWorkbook.path & "\" & Cells(5, 2).value
    Set REST_DCI_JS = fs.CreateTextFile(file_path, True)

'****************************************************************************************************************************
'******     Create the object heirarchy
'****************************************************************************************************************************
    Dim obj As New Dictionary
    Dim counter As Integer

    counter = START_ROW
    While (Cells(counter, DATA_DEFINE_COL).text <> Empty) And (Cells(counter, DATA_DEFINE_COL).text <> BEGIN_IGNORED_DATA)
        If Len(Cells(counter, JS_COL).value) > 0 Then
            Dim N As Integer, i As Integer
            Dim dict As Dictionary
            Dim arr() As String
            
            arr = Split(Cells(counter, JS_COL).value, ".")
            N = UBound(arr)
            Set dict = obj
            For i = 0 To N - 1
                Set dict = getDictionary(dict, arr(i))
            Next i
            
            Call getDictionary(dict, arr(i), counter)
        End If
        counter = counter + 1
    Wend
    
    Dim k
    For Each k In obj.keys
        Call CompressDictionary(obj, k)
    Next
    
    Call WriteObjects(REST_DCI_JS, obj, "")
    
End Sub

'Returns the value for a given key in the given dictionary.
'If the key doesn't exist, the given value is placed there and returned.
'If no new value is given, then a new Dictionary is placed there.
Function getDictionary(ByRef dict As Dictionary, ByRef key As String, Optional ByRef value As Variant)
    Dim bracketIndex
    bracketIndex = InStr(key, "[")
    If bracketIndex > 0 And Right(key, 1) = "]" Then
        'This is a fully qualified array
        'Split it into "array[" and "#]" and recurse
        Dim arrKey As String, arrIndex As String
        arrKey = Left(key, bracketIndex)
        arrIndex = Right(key, Len(key) - bracketIndex)
        
        If IsMissing(value) Then
            Set getDictionary = getDictionary(getDictionary(dict, arrKey), arrIndex)
        Else
            getDictionary = getDictionary(getDictionary(dict, arrKey), arrIndex, value)
        End If
        Exit Function
    End If

    Const DEFAULT_KEY As String = "DEFAULT_KEY"
    If Not dict.Exists(key) Then
        If IsMissing(value) Then
            dict.Add key, New Dictionary
        Else
            dict.Add key, value
        End If
    ElseIf TypeName(dict.Item(key)) <> "Dictionary" Then
        'This is the oddball case of having both a value and a dictionary at the same place.
        If IsMissing(value) Then
            dict.Add DEFAULT_KEY, dict.Item(key)
            dict.Add key, New Dictionary
        Else
            dict.Add DEFAULT_KEY, value
        End If
    Else
        'Value already exists, don't overwrite
    End If
    
    'Return the [newly added] dictionary or value
    If IsMissing(value) Then
        Set getDictionary = dict.Item(key)
    Else
        getDictionary = value
    End If
End Function

Sub CompressDictionary(ByRef dict As Dictionary, k)
    'The Dictionary is compressible if the key points to another dictionary AND that dictionary has only 1 element
    If TypeName(dict.Item(k)) = "Dictionary" Then
        Dim d As Dictionary
        Set d = dict.Item(k)
        If d.count = 1 Then
            'Dictionary is compressible, remove the original and replace it
            Dim newKey
            newKey = k & "_" & d.keys(0)
            dict.Remove k
            dict.Add newKey, d.Item(d.keys(0))
        End If
        
        'Try to compress any other dictionaries in the target
        Dim N
        For Each N In d.keys
            If TypeName(d.Item(N)) = "Dictionary" Then Call CompressDictionary(d, N)
        Next
        
    End If
    
End Sub

Sub WriteObjects(ByRef REST_DCI_JS As TextStream, ByRef dict As Dictionary, path As String)
    Const DESCRIP_COL = 1
    Const DATA_DEFINE_COL = 2
    Const PARAM_ID_COL = 3
    Const READ_LEVEL_COL = 4
    Const WRITE_LEVEL_COL = 5
    
    Dim k, V
    Dim i As Integer
    Dim d As Dictionary
    Dim newPath As String
    Dim dictCount
    Dim dicts(50) As String
    
    For Each k In dict.keys
        If TypeName(dict.Item(k)) <> "Dictionary" Then
            'Add the line item and move on
            V = dict.Item(k)
            Dim id, name, define, r As Range, row As Range, modbus1, modbus2
            Dim size, delta, readLevel, writeLevel
            Dim dci As Range, dRow As Range
            id = Cells(V, PARAM_ID_COL).value
            name = Cells(V, DESCRIP_COL).value
            define = Cells(V, DATA_DEFINE_COL).value
            readLevel = Cells(V, READ_LEVEL_COL).value
            writeLevel = Cells(V, WRITE_LEVEL_COL).value
            
            'Find the DCI row
            Set dci = Worksheets("DCI Descriptors").Range("B:B")
            Set dRow = dci.Find(what:=define, lookat:=xlWhole)
            'Determine size per element
            If (dci.Cells(dRow.row, 2) = "DCI_DTYPE_STRING8") Then
                'String size equals the array count
                size = dci.Cells(dRow.row, 4)
            Else
                size = dci.Cells(dRow.row, 3)
            End If
            
            'Inflate writeLevel if writing is disabled
            Dim tempValue
            tempValue = dci.Cells(dRow.row, 52).value
            If Len(Trim(tempValue)) = 0 Then
                writeLevel = 255
            Else
                writeLevel = writeLevel
            End If
            
            
            'Find the modbus values
            Set r = Worksheets("Modbus").Range("B:B")
            Set row = r.Find(what:=define, lookat:=xlWhole)
            delta = "null"
            modbus1 = "null"
            modbus2 = "null"
            Do Until (row Is Nothing)
                If define = "DCI_MM_1_CT_ID" Then
                    define = define
                End If
                
                Dim value, prevRow As Integer, dataStartPosition
                value = r.Cells(row.row, 3).value
                
                dataStartPosition = r.Cells(row.row, 9).value
                
                If dataStartPosition = 0 Or dataStartPosition = Empty Then
                    If "null" = modbus1 Then
                        modbus1 = value
                    ElseIf value <> modbus1 Then
                        If value < modbus1 Then
                            modbus2 = modbus1
                            modbus1 = value
                        Else
                            modbus2 = value
                        End If
                        Exit Do
                    End If
                ElseIf "null" <> modbus1 Then
                    If delta = "null" Or delta > Abs(value - modbus1) Then
                        delta = Abs(value - modbus1)
                    End If
                End If
                
                prevRow = row.row
                Set row = r.Find(what:=define, lookat:=xlWhole, After:=row)
                If row.row <= prevRow Then
                    Exit Do
                End If
            Loop
            
            'Assign the path
            If (id <> 0) Then
                If Len(path) = 0 Then
                    newPath = k
                ElseIf Right(k, 1) = "]" Then
                    newPath = path & k
                Else
                    newPath = path & "." & k
                End If
                REST_DCI_JS.Write (newPath & " = new Param(" & id & ", """ & name & """, " & readLevel & ", " & writeLevel)
                
                Dim flag
                If delta <> "null" Then
                    flag = 4
                ElseIf size <> "null" And modbus1 <> "null" Then
                    flag = 3
                ElseIf modbus2 <> "null" Then
                    flag = 2
                ElseIf modbus1 <> "null" Then
                    flag = 1
                Else
                    flag = 0
                End If
                    
                If flag >= 1 Then
                    REST_DCI_JS.Write (", " & modbus1)
                    If flag >= 2 Then
                        REST_DCI_JS.Write (", " & modbus2)
                        If flag >= 3 Then
                            REST_DCI_JS.Write (", " & size)
                            If flag >= 4 Then
                                REST_DCI_JS.Write (", " & delta)
                            End If
                        End If
                    End If
                End If
                
                REST_DCI_JS.WriteLine (");")
            End If
        Else
            'This is a dictionary, add it to the queue to process later
            dictCount = dictCount + 1
            dicts(dictCount) = k
        End If
    Next
    
    For i = 1 To dictCount
        'This is a dictionary, create the new object and recurse
        k = dicts(i)
        If Len(path) = 0 Then
            newPath = k
        ElseIf Right(k, 1) = "]" Then
            newPath = path & k
        Else
            newPath = path & "." & k
        End If
        
        If Right(k, 1) = "[" Then
            'This is an array, create the array name
            REST_DCI_JS.WriteLine ("")
            REST_DCI_JS.WriteLine (Left(newPath, Len(newPath) - 1) & " = [];")
        Else
            'This is an object, create the object name
            REST_DCI_JS.WriteLine ("")
            REST_DCI_JS.WriteLine (newPath & " = {};")
        End If
        
        Set d = dict.Item(k)
        Call WriteObjects(REST_DCI_JS, d, newPath)
    Next
End Sub



