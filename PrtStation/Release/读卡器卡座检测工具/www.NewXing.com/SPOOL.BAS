Attribute VB_Name = "MSpool"
Option Explicit
'
' Win32 API Calls
'
Private Declare Function GetProfileString Lib "kernel32" Alias "GetProfileStringA" (ByVal lpAppName As String, ByVal lpKeyName As Any, ByVal lpDefault As String, ByVal lpReturnedString As String, ByVal nSize As Long) As Long
Private Declare Function OpenPrinter Lib "winspool.drv" Alias "OpenPrinterA" (ByVal pPrinterName As String, phPrn As Long, pDefault As Any) As Long
Private Declare Function StartDocPrinter Lib "winspool.drv" Alias "StartDocPrinterA" (ByVal hPrn As Long, ByVal Level As Long, pDocInfo As DOC_INFO_1) As Long
Private Declare Function StartPagePrinter Lib "winspool.drv" (ByVal hPrn As Long) As Long
Private Declare Function WritePrinter Lib "winspool.drv" (ByVal hPrn As Long, pBuf As Any, ByVal cdBuf As Long, pcWritten As Long) As Long
Private Declare Function EndPagePrinter Lib "winspool.drv" (ByVal hPrn As Long) As Long
Private Declare Function EndDocPrinter Lib "winspool.drv" (ByVal hPrn As Long) As Long
Private Declare Function ClosePrinter Lib "winspool.drv" (ByVal hPrn As Long) As Long
' Download by http://www.NewXing.com
' Structure required by StartDocPrinter
'
Private Type DOC_INFO_1
   pDocName As String
   pOutputFile As String
   pDatatype As String
End Type

Public Sub SelectDefaultPrinter(Lst As ComboBox)
   Dim sRet As String
   Dim nRet As Integer
   Dim i As Integer
   '
   ' Look for default printer in WIN.INI
   '
   sRet = Space(255)
   nRet = GetProfileString("Windows", ByVal "device", "", _
                           sRet, Len(sRet))
   '
   ' Truncate default printer name.
   '
   If nRet Then
      sRet = UCase(Left(sRet, InStr(sRet, ",") - 1))
      '
      ' Cycle list looking for matching entry.
      '
      For i = 0 To Lst.ListCount
         If Left(UCase(Lst.List(i)), Len(sRet)) = sRet Then
            '
            ' Found it. Set index and bail.
            '
            Lst.ListIndex = i
            Exit For
         End If
      Next i
   End If
End Sub

Public Sub SpoolFile(sFile As String, PrnName As String, Optional AppName As String = "")
   Dim hPrn As Long
   Dim Buffer() As Byte
   Dim hFile As Integer
   Dim Written As Long
   Dim di As DOC_INFO_1
   Dim i As Long
   Const BufSize As Long = &H4000
   '
   ' Extract filename from passed spec, and build job name.
   ' Fill remainder of DOC_INFO_1 structure.
   '
   If InStr(sFile, "\") Then
      For i = Len(sFile) To 1 Step -1
         If Mid(sFile, i, 1) = "\" Then Exit For
         di.pDocName = Mid(sFile, i, 1) & di.pDocName
      Next i
   Else
      di.pDocName = sFile
   End If
   If Len(AppName) Then
      di.pDocName = AppName & ": " & di.pDocName
   End If
   di.pOutputFile = vbNullString
   di.pDatatype = "RAW"
   '
   ' Open printer for output to obtain handle.
   ' Set it up to begin recieving raw data.
   '
   Call OpenPrinter(PrnName, hPrn, vbNullString)
   Call StartDocPrinter(hPrn, 1, di)
   Call StartPagePrinter(hPrn)
   '
   ' Open file and pump it to the printer.
   '
   hFile = FreeFile
   Open sFile For Binary Access Read As hFile
      '
      ' Read in 16K buffers and spool.
      '
      ReDim Buffer(1 To BufSize) As Byte
      For i = 1 To LOF(hFile) \ BufSize
         Get #hFile, , Buffer
         Call WritePrinter(hPrn, Buffer(1), BufSize, Written)
      Next i
      '
      ' Get last chunk of file if it doesn't
      ' fit evenly into a 16K buffer.
      '
      If LOF(hFile) Mod BufSize Then
         ReDim Buffer(1 To (LOF(hFile) Mod BufSize)) As Byte
         Get #hFile, , Buffer
         Call WritePrinter(hPrn, Buffer(1), UBound(Buffer), Written)
      End If
   Close #hFile
   '
   ' Shut down spooling process.
   '
   Call EndPagePrinter(hPrn)
   Call EndDocPrinter(hPrn)
   Call ClosePrinter(hPrn)
End Sub

Public Function IsFile(SpecIn As String) As Boolean
   Dim Attr As Byte
   '
   ' Guard against bad SpecIn by ignoring errors.
   '
   On Error Resume Next
   '
   ' Get attribute of SpecIn.
   '
   Attr = GetAttr(SpecIn)
   If Err = 0 Then
      '
      ' No error, so something was found.
      ' If Directory attribute set, then not a file.
      '
      If (Attr And vbDirectory) = vbDirectory Then
         IsFile = False
      Else
         IsFile = True
      End If
   End If
End Function

