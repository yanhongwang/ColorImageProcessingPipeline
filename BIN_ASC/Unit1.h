//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include <stdio.h>

#include <ComCtrls.hpp>
#include <FileCtrl.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TDriveComboBox *DriveComboBox1;
        TDirectoryListBox *DirectoryListBox1;
        TFileListBox *FileListBox1;
        TButton *BIN2ALLASC;
        TButton *BIN2RGBASC;
        TDirectoryListBox *DirectoryListBox2;
        TDriveComboBox *DriveComboBox2;
        TFileListBox *FileListBox2;
        TButton *RGBASC2BIN;
        TRadioGroup *ResolutionGroup;
        void __fastcall BIN2ALLASCClick(TObject *Sender);
        void __fastcall BIN2RGBASCClick(TObject *Sender);
        void __fastcall RGBASC2BINClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
