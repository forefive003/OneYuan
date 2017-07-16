VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.Form frmSpool 
   BorderStyle     =   4  'Fixed ToolWindow
   Caption         =   "发送脱机打印任务"
   ClientHeight    =   2115
   ClientLeft      =   1275
   ClientTop       =   1560
   ClientWidth     =   5490
   Icon            =   "SPOOL.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   2115
   ScaleWidth      =   5490
   ShowInTaskbar   =   0   'False
   Begin VB.ComboBox Combo1 
      Height          =   300
      Left            =   180
      Style           =   2  'Dropdown List
      TabIndex        =   4
      Top             =   1080
      Width           =   4395
   End
   Begin VB.CommandButton cmdClose 
      Cancel          =   -1  'True
      Caption         =   "关闭(&C)"
      Height          =   315
      Left            =   3180
      TabIndex        =   6
      Top             =   1560
      Width           =   1395
   End
   Begin VB.CommandButton cmdPrint 
      Caption         =   "打印(&P)"
      Height          =   315
      Left            =   1560
      TabIndex        =   5
      Top             =   1560
      Width           =   1395
   End
   Begin VB.TextBox txtFile 
      Height          =   315
      Left            =   180
      TabIndex        =   1
      Text            =   "txtFile"
      Top             =   420
      Width           =   3975
   End
   Begin VB.CommandButton cmdFile 
      Caption         =   "..."
      Height          =   315
      Left            =   4200
      TabIndex        =   2
      Top             =   420
      Width           =   375
   End
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   4860
      Top             =   120
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
      CancelError     =   -1  'True
      DialogTitle     =   "要打印的文件"
   End
   Begin VB.Label Label2 
      AutoSize        =   -1  'True
      Caption         =   "可用的打印机(&P):"
      Height          =   180
      Left            =   240
      TabIndex        =   3
      Top             =   840
      Width           =   1440
   End
   Begin VB.Label Label1 
      AutoSize        =   -1  'True
      Caption         =   "要打印的文件(&F):"
      Height          =   180
      Left            =   240
      TabIndex        =   0
      Top             =   180
      Width           =   1440
   End
End
Attribute VB_Name = "frmSpool"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
' Download by http://www.NewXing.com
Private Sub cmdClose_Click()
   '
   ' All Done
   '
   Unload Me
End Sub

Private Sub cmdFile_Click()
   With CommonDialog1
      '
      ' Set dialog properties
      '
      .Flags = cdlOFNFileMustExist Or cdlOFNHideReadOnly
      .Filter = "Print Files (*.prn)|*.PRN|AllFiles (*.*)|*.*"
      '
      ' Get filename. Allow user to cancel.
      '
      On Error Resume Next
      .ShowOpen
      '
      ' Place selected filename into textbox.
      '
      If Err = 0 Then
         txtFile = .FileName
      End If
   End With
End Sub

Private Sub cmdPrint_Click()
   Dim Submit As String
   Dim prn As Printer
   '
   ' Warn user if file doesn't exist.
   '
   Submit = UCase(Trim(txtFile))
   If Not IsFile(Submit) Then
      MsgBox "Can't find file: " & Submit, vbExclamation, "Error"
      Exit Sub
   End If
   '
   ' Submit file to spooler after extracting name.
   '
   For Each prn In Printers
      If InStr(Combo1, prn.DeviceName) = 1 _
         And Right(Combo1, Len(prn.Port)) = prn.Port Then
            Call SpoolFile(Submit, prn.DeviceName)
            Exit For
      End If
   Next prn
End Sub

Private Sub Form_Initialize()
   '
   ' Make sure there are printers installed!
   ' No point in proceeding if not.
   '
   If Printers.Count = 0 Then
      MsgBox "No printers are installed. Can't continue.", _
             vbCritical, "Fatal Error"
      End
   End If
End Sub

Private Sub Form_Load()
   Dim prn As Printer
   '
   ' Fill combo box with available printers, and
   ' select default printer.
   '
   For Each prn In Printers
      Combo1.AddItem prn.DeviceName & " on " & prn.Port
   Next prn
   '
   ' Select default printer.
   '
   SelectDefaultPrinter Combo1
   '
   ' Size and position form; clear textbox.
   '
   With Me
      .Width = Combo1.Width + Combo1.Left * 2 + _
               (.Width - Me.ScaleWidth)
      .Height = cmdPrint.Top + cmdPrint.Height + Label1.Top + _
               (.Height - .ScaleHeight)
      .Move (Screen.Width - .Width) \ 2, _
            (Screen.Height - .Height) \ 2
   End With
   txtFile = ""
End Sub


