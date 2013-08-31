object Form1: TForm1
  Left = 174
  Top = 101
  Width = 1233
  Height = 865
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object DriveComboBox1: TDriveComboBox
    Left = 8
    Top = 8
    Width = 73
    Height = 19
    DirList = DirectoryListBox1
    TabOrder = 0
  end
  object DirectoryListBox1: TDirectoryListBox
    Left = 8
    Top = 32
    Width = 177
    Height = 169
    FileList = FileListBox1
    ItemHeight = 16
    TabOrder = 1
  end
  object FileListBox1: TFileListBox
    Left = 8
    Top = 208
    Width = 177
    Height = 561
    ItemHeight = 13
    Mask = '*.bmp'
    TabOrder = 2
    OnClick = FileListBox1Click
  end
  object FilterComboBox1: TFilterComboBox
    Left = 80
    Top = 8
    Width = 105
    Height = 21
    FileList = FileListBox1
    Filter = '24 '#20301#20803#40670#38499#22294#65288'*.bmp'#65289'|*.bmp'
    TabOrder = 3
  end
  object TabControl1: TTabControl
    Left = 184
    Top = 8
    Width = 1033
    Height = 801
    TabOrder = 4
    Tabs.Strings = (
      'Image'
      'Color Correction')
    TabIndex = 0
    OnChange = TabControl1Change
    object Image4: TImage
      Left = 0
      Top = 24
      Width = 1024
      Height = 768
      AutoSize = True
    end
  end
end
