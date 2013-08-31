//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include <iostream.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <Dialogs.hpp>
#include <FileCtrl.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <jpeg.hpp>

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TDriveComboBox *DriveComboBox1;
        TDirectoryListBox *DirectoryListBox1;
        TFileListBox *FileListBox1;
        TFilterComboBox *FilterComboBox1;
        TTabControl *TabControl1;
        TImage *Image4;
        TCheckBox *EdgeDetectionSwitch;
        TCheckBox *NoiseFilterSwitch;
        TCheckBox *EdgeEnhancementSwitch;
        TCheckBox *WhiteBalanceSwitch;
        TCheckBox *SaturationEnhancementSwitch;
        TCheckBox *GammaCorrectionSwitch;
        TCheckBox *AutoLevelSwitch;
        TCheckBox *ColorCorrectionSwitch;
        TRadioGroup *WhiteBalanceGroup;
        TCheckBox *ToneReproductionSwitch;
        TButton *RunOne;
        TButton *RunAll;
        void __fastcall FileListBox1Click(TObject *Sender);
        void __fastcall TabControl1Change(TObject *Sender);
        void __fastcall WhiteBalanceSwitchClick(TObject *Sender);
        void __fastcall RunOneClick(TObject *Sender);
        void __fastcall RunAllClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
};

//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
