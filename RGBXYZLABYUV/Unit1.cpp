//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"

#include "RGBCIELAB.h"
#include "YCbCrRGB.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RGBSpaceClick(TObject *Sender)
{
	
	double CIEX;
	double CIEY;
	double CIEZ;
	
	double L;
	double a;
	double b;
	
	unsigned char Y;
	unsigned char Cb;
	unsigned char Cr;
	
	RGB2XYZ( BValue -> Text.ToInt(), GValue -> Text.ToInt(), RValue -> Text.ToInt(), &CIEX, &CIEY, &CIEZ );
	
	XYZ2Lab( CIEX, CIEY, CIEZ, &L, &a, &b );
	
	CIEXValue -> Text = CIEX;
	CIEYValue -> Text = CIEY;
	CIEZValue -> Text = CIEZ;
	
	CIELValue -> Text = L;
	CIEaValue -> Text = a;
	CIEbValue -> Text = b;
	
	RGB2YCbCr( BValue -> Text.ToInt(), GValue -> Text.ToInt(), RValue -> Text.ToInt(), &Y, &Cb, &Cr );
	
	YValue -> Text = Y;
	UValue -> Text = Cb;
	VValue -> Text = Cr;
	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::XYZSpaceClick(TObject *Sender)
{
	
	unsigned char B;
	unsigned char G;
	unsigned char R;
	
	double L;
	double a;
	double b;
	
	unsigned char Y;
	unsigned char Cb;
	unsigned char Cr;
	
	XYZ2RGB( CIEXValue->Text.ToDouble(), CIEYValue->Text.ToDouble(), CIEZValue->Text.ToDouble(), &B, &G, &R );
	
	XYZ2Lab( CIEXValue->Text.ToDouble(), CIEYValue->Text.ToDouble(), CIEZValue->Text.ToDouble(), &L, &a, &b );
	
	RValue -> Text = R;
	GValue -> Text = G;
	BValue -> Text = B;
	
	CIELValue -> Text = L;
	CIEaValue -> Text = a;
	CIEbValue -> Text = b;
	
	RGB2YCbCr( B, G, R, &Y, &Cb, &Cr );
	
	YValue -> Text = Y;
	UValue -> Text = Cb;
	VValue -> Text = Cr;
	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LabSpaceClick(TObject *Sender)
{
	
	unsigned char B;
	unsigned char G;
	unsigned char R;
	
	double CIEX;
	double CIEY;
	double CIEZ;
	
	unsigned char Y;
	unsigned char Cb;
	unsigned char Cr;
	
	Lab2XYZ( CIELValue -> Text.ToDouble(), CIEaValue -> Text.ToDouble(), CIEbValue -> Text.ToDouble(), &CIEX, &CIEY, &CIEZ );
	
	XYZ2RGB( CIEX, CIEY, CIEZ, &B, &G, &R );
	
	RValue -> Text = R;
	GValue -> Text = G;
	BValue -> Text = B;
	
	CIEXValue -> Text = CIEX;
	CIEYValue -> Text = CIEY;
	CIEZValue -> Text = CIEZ;
	
	RGB2YCbCr( B, G, R, &Y, &Cb, &Cr );
	
	YValue -> Text = Y;
	UValue -> Text = Cb;
	VValue -> Text = Cr;
	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::YUVSpaceClick(TObject *Sender)
{
	
	unsigned char R;
	unsigned char G;
	unsigned char B;
	
	double CIEX;
	double CIEY;
	double CIEZ;
	
	double L;
	double a;
	double b;
	
	YCbCr2RGB( YValue->Text.ToDouble(), UValue->Text.ToDouble(), VValue->Text.ToDouble(), &B, &G, &R );
	
	RValue -> Text = R;
	GValue -> Text = G;
	BValue -> Text = B;
	
	RGB2XYZ( B, G, R, &CIEX, &CIEY, &CIEZ );
	
	CIEXValue -> Text = CIEX;
	CIEYValue -> Text = CIEY;
	CIEZValue -> Text = CIEZ;
	
	XYZ2Lab( CIEX, CIEY, CIEZ, &L, &a, &b );
	
	CIELValue -> Text = L;
	CIEaValue -> Text = a;
	CIEbValue -> Text = b;
	
}
//---------------------------------------------------------------------------

