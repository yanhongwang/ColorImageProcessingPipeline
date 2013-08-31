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

//#include "LookUpTableRGB2LAB.h"
//#include "LookUpTableLAB2RGB.h"

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
        void __fastcall FileListBox1Click(TObject *Sender);
        void __fastcall TabControl1Change(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
};

void ReadBMPData( char* filename );

//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
