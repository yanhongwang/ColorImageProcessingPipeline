object Form1: TForm1
  Left = 334
  Top = 85
  Width = 1233
  Height = 858
  Caption = 'Color Image Processing Pipeline Simulation'
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
    Height = 121
    FileList = FileListBox1
    ItemHeight = 16
    TabOrder = 1
  end
  object FileListBox1: TFileListBox
    Left = 8
    Top = 168
    Width = 177
    Height = 105
    ItemHeight = 13
    Mask = '*.raw'
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
      'All In One')
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
  object EdgeDetectionSwitch: TCheckBox
    Left = 8
    Top = 592
    Width = 97
    Height = 17
    Caption = 'Edge Detection'
    TabOrder = 5
  end
  object NoiseFilterSwitch: TCheckBox
    Left = 8
    Top = 624
    Width = 73
    Height = 17
    Caption = 'Noise Filter'
    TabOrder = 6
  end
  object EdgeEnhancementSwitch: TCheckBox
    Left = 8
    Top = 688
    Width = 113
    Height = 17
    Caption = 'Edge Enhancement'
    TabOrder = 7
  end
  object WhiteBalanceSwitch: TCheckBox
    Left = 8
    Top = 360
    Width = 97
    Height = 17
    Caption = 'White Balance'
    TabOrder = 8
    OnClick = WhiteBalanceSwitchClick
  end
  object SaturationEnhancementSwitch: TCheckBox
    Left = 8
    Top = 720
    Width = 137
    Height = 17
    Caption = 'Saturationi Enhancement'
    TabOrder = 9
  end
  object GammaCorrectionSwitch: TCheckBox
    Left = 8
    Top = 784
    Width = 105
    Height = 17
    Caption = 'Gamma Correction'
    TabOrder = 10
  end
  object AutoLevelSwitch: TCheckBox
    Left = 8
    Top = 328
    Width = 73
    Height = 17
    Caption = 'Auto Level'
    TabOrder = 11
  end
  object ColorCorrectionSwitch: TCheckBox
    Left = 8
    Top = 656
    Width = 97
    Height = 17
    Caption = 'Color Correction'
    TabOrder = 12
  end
  object WhiteBalanceGroup: TRadioGroup
    Left = 8
    Top = 392
    Width = 161
    Height = 185
    Caption = 'Method'
    Items.Strings = (
      'Gray World'
      'White Point'
      'Quadratic Mapping'
      'Color Temperature Curve'
      'CTC + Large Block')
    TabOrder = 13
  end
  object ToneReproductionSwitch: TCheckBox
    Left = 8
    Top = 752
    Width = 113
    Height = 17
    Caption = 'ToneReproduction'
    TabOrder = 14
  end
  object RunOne: TButton
    Left = 8
    Top = 288
    Width = 81
    Height = 25
    Caption = 'RunOne'
    TabOrder = 15
    OnClick = RunOneClick
  end
  object RunAll: TButton
    Left = 96
    Top = 288
    Width = 81
    Height = 25
    Caption = 'RunAll'
    TabOrder = 16
    OnClick = RunAllClick
  end
end
