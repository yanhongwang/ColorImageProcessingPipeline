object Form1: TForm1
  Left = 503
  Top = 745
  Width = 483
  Height = 179
  Caption = 'RGBXYZLAB'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object RValue: TEdit
    Left = 88
    Top = 8
    Width = 121
    Height = 21
    TabOrder = 0
  end
  object GValue: TEdit
    Left = 216
    Top = 8
    Width = 121
    Height = 21
    TabOrder = 1
  end
  object BValue: TEdit
    Left = 344
    Top = 8
    Width = 121
    Height = 21
    TabOrder = 2
  end
  object CIEXValue: TEdit
    Left = 88
    Top = 40
    Width = 121
    Height = 21
    TabOrder = 3
  end
  object CIEYValue: TEdit
    Left = 216
    Top = 40
    Width = 121
    Height = 21
    TabOrder = 4
  end
  object CIEZValue: TEdit
    Left = 344
    Top = 40
    Width = 121
    Height = 21
    TabOrder = 5
  end
  object CIELValue: TEdit
    Left = 88
    Top = 72
    Width = 121
    Height = 21
    TabOrder = 6
  end
  object CIEaValue: TEdit
    Left = 216
    Top = 72
    Width = 121
    Height = 21
    TabOrder = 7
  end
  object CIEbValue: TEdit
    Left = 344
    Top = 72
    Width = 121
    Height = 21
    TabOrder = 8
  end
  object RGBSpace: TButton
    Left = 8
    Top = 8
    Width = 75
    Height = 25
    Caption = 'RGB'
    TabOrder = 9
    OnClick = RGBSpaceClick
  end
  object XYZSpace: TButton
    Left = 8
    Top = 40
    Width = 75
    Height = 25
    Caption = 'XYZ'
    TabOrder = 10
    OnClick = XYZSpaceClick
  end
  object LabSpace: TButton
    Left = 8
    Top = 72
    Width = 75
    Height = 25
    Caption = 'Lab'
    TabOrder = 11
    OnClick = LabSpaceClick
  end
  object YUVSpace: TButton
    Left = 8
    Top = 104
    Width = 75
    Height = 25
    Caption = 'YUV'
    TabOrder = 12
    OnClick = YUVSpaceClick
  end
  object YValue: TEdit
    Left = 88
    Top = 104
    Width = 121
    Height = 21
    TabOrder = 13
  end
  object UValue: TEdit
    Left = 216
    Top = 104
    Width = 121
    Height = 21
    TabOrder = 14
  end
  object VValue: TEdit
    Left = 344
    Top = 104
    Width = 121
    Height = 21
    TabOrder = 15
  end
end
