//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include <stdio.h>

#include "bmp.h"
#include "RGBCIELAB.h"

#include <ComCtrls.hpp>
#include <FileCtrl.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TButton *calculate;
        TLabel *Label7;
        TLabel *LValue;
        TLabel *Label8;
        TLabel *aValue;
        TLabel *Label9;
        TLabel *bValue;
        TEdit *RMin;
        TUpDown *UpDown1;
        TLabel *Label1;
        TLabel *Label2;
        TEdit *GMin;
        TUpDown *UpDown2;
        TLabel *Label3;
        TEdit *BMin;
        TUpDown *UpDown3;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TEdit *RMax;
        TEdit *GMax;
        TEdit *BMax;
        TUpDown *UpDown4;
        TUpDown *UpDown5;
        TUpDown *UpDown6;
        TDriveComboBox *DriveComboBox1;
        TDirectoryListBox *DirectoryListBox1;
        TFileListBox *FileListBox1;
        TButton *RGB2RAW;
        TButton *RGB2LABTable;
        TButton *LAB2RGBTable;
        TButton *RandomRGB;
        TEdit *RHigh;
        TEdit *RLow;
        TLabel *Label10;
        TEdit *GHigh;
        TEdit *GLow;
        TLabel *Label11;
        TEdit *BHigh;
        TEdit *BLow;
        TLabel *Label12;
        TButton *calculate_xy;
        TLabel *Label13;
        TLabel *xValue;
        TLabel *Label15;
        TLabel *yValue;
        TPanel *Panel1;
        TRadioGroup *MathOperationGroup;
        TButton *MathTableGenerate;
        TEdit *MathMinBoundary;
        TLabel *Label14;
        TEdit *MathMaxBoundary;
        TLabel *Label16;
        TEdit *ScaleNumber;
        TLabel *Label17;
        TLabel *Label18;
        TLabel *Label19;
        TLabel *CIEXValue;
        TLabel *CIEYValue;
        TLabel *CIEZValue;
        TButton *calculateXYZ;
        TLabel *Label20;
        TEdit *Argument;
        TButton *uglyRGB2XYZ;
        TButton *RGB2XYZTable;
        TButton *uglyCTC;
        TButton *uglyGammaCorrection;
        void __fastcall calculateClick(TObject *Sender);
        void __fastcall RGB2RAWClick(TObject *Sender);
        void __fastcall RGB2LABTableClick(TObject *Sender);
        void __fastcall LAB2RGBTableClick(TObject *Sender);
        void __fastcall RandomRGBClick(TObject *Sender);
        void __fastcall calculate_xyClick(TObject *Sender);
        void __fastcall MathTableGenerateClick(TObject *Sender);
        void __fastcall calculateXYZClick(TObject *Sender);
        void __fastcall uglyRGB2XYZClick(TObject *Sender);
        void __fastcall RGB2XYZTableClick(TObject *Sender);
        void __fastcall uglyCTCClick(TObject *Sender);
        void __fastcall uglyGammaCorrectionClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
