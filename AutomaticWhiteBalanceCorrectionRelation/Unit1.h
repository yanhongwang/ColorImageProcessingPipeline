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

#include <Dialogs.hpp>
#include <FileCtrl.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <jpeg.hpp>

unsigned char **ImageDataBlockPretty;
unsigned char **ImageDataBlockUgly;
double **Lab_buf_Pretty;
double **Lab_buf_Ugly;

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
private:	// User declarations
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
	__fastcall ~TForm1(){}
};

void ReadBMPData( char* filename );

//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
