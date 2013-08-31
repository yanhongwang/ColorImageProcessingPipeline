object Form1: TForm1
  Left = 450
  Top = 550
  Width = 448
  Height = 337
  Caption = 'BIN&ASC'
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
    Width = 209
    Height = 19
    DirList = DirectoryListBox1
    TabOrder = 0
  end
  object DirectoryListBox1: TDirectoryListBox
    Left = 8
    Top = 32
    Width = 209
    Height = 97
    FileList = FileListBox1
    ItemHeight = 16
    TabOrder = 1
  end
  object FileListBox1: TFileListBox
    Left = 8
    Top = 136
    Width = 209
    Height = 121
    ItemHeight = 13
    Mask = '*.bmp'
    TabOrder = 2
  end
  object BIN2ALLASC: TButton
    Left = 8
    Top = 264
    Width = 97
    Height = 25
    Caption = 'BIN2ALLASC'
    TabOrder = 3
    OnClick = BIN2ALLASCClick
  end
  object BIN2RGBASC: TButton
    Left = 120
    Top = 264
    Width = 97
    Height = 25
    Caption = 'BIN2RGBASC'
    TabOrder = 4
    OnClick = BIN2RGBASCClick
  end
  object DirectoryListBox2: TDirectoryListBox
    Left = 224
    Top = 32
    Width = 209
    Height = 97
    FileList = FileListBox2
    ItemHeight = 16
    TabOrder = 5
  end
  object DriveComboBox2: TDriveComboBox
    Left = 224
    Top = 8
    Width = 209
    Height = 19
    DirList = DirectoryListBox2
    TabOrder = 6
  end
  object FileListBox2: TFileListBox
    Left = 224
    Top = 136
    Width = 113
    Height = 121
    ItemHeight = 13
    Mask = '*.dat'
    TabOrder = 7
  end
  object RGBASC2BIN: TButton
    Left = 224
    Top = 264
    Width = 209
    Height = 25
    Caption = 'RGBASC2BIN'
    TabOrder = 8
    OnClick = RGBASC2BINClick
  end
  object ResolutionGroup: TRadioGroup
    Left = 344
    Top = 136
    Width = 89
    Height = 121
    BiDiMode = bdLeftToRight
    Caption = 'Resolution'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    Items.Strings = (
      '2048 x 1536'
      '320 x 240'
      '20 x 15')
    ParentBiDiMode = False
    ParentFont = False
    TabOrder = 9
  end
end
