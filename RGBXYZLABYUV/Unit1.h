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
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TEdit *RValue;
        TEdit *GValue;
        TEdit *BValue;
        TEdit *CIEXValue;
        TEdit *CIEYValue;
        TEdit *CIEZValue;
        TEdit *CIELValue;
        TEdit *CIEaValue;
        TEdit *CIEbValue;
        TButton *RGBSpace;
        TButton *XYZSpace;
        TButton *LabSpace;
        TButton *YUVSpace;
        TEdit *YValue;
        TEdit *UValue;
        TEdit *VValue;
        void __fastcall RGBSpaceClick(TObject *Sender);
        void __fastcall XYZSpaceClick(TObject *Sender);
        void __fastcall LabSpaceClick(TObject *Sender);
        void __fastcall YUVSpaceClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
