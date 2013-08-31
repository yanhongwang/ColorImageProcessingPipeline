//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------

FILE * ColorCheckerError;

static double xyValue[][ 2 ] =
{
	
	{ 0.400, 0.350 }, { 0.377, 0.345 }, { 0.247, 0.251 }, { 0.337, 0.422 }, { 0.265, 0.240 }, { 0.261, 0.343 },
	{ 0.506, 0.407 }, { 0.211, 0.175 }, { 0.453, 0.306 }, { 0.285, 0.202 }, { 0.380, 0.489 }, { 0.473, 0.438 },
	{ 0.187, 0.129 }, { 0.305, 0.478 }, { 0.539, 0.313 }, { 0.448, 0.470 }, { 0.364, 0.233 }, { 0.196, 0.252 },
	{ 0.310, 0.316 }, { 0.310, 0.316 }, { 0.310, 0.316 }, { 0.310, 0.316 }, { 0.310, 0.316 }, { 0.310, 0.316 }
	
};

__fastcall TForm1::TForm1(TComponent* Owner): TForm(Owner)
{
	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FileListBox1Click(TObject *Sender)
{
	
	TabControl1Change( Sender );
	
}
//---------------------------------------------------------------------------
/*
void RGB2XYZ( double B, double G, double R, double *X, double *Y, double *Z )
{
	
	float r;
	float g;
	float b;
	
	if( R / 256 <= 0.04045 )
		r = ( R / 256 ) / 12.92;
	else
		r = pow( ( R / 256 + 0.055 ) / 1.055, 2.4 );
	
	if( G / 256 <= 0.04045 )
		g = ( G / 256 ) / 12.92;
	else
		g = pow( ( G / 256 + 0.055 ) / 1.055, 2.4 );
	
	if( B / 256 <= 0.04045 )
		b = ( B / 256 ) / 12.92;
	else
		b = pow( ( B / 256 + 0.055 ) / 1.055, 2.4 );
	
	*X = 0.412453 * r + 0.357580 * g + 0.180423 * b;
	*Y = 0.212671 * r + 0.715160 * g + 0.072169 * b;
	*Z = 0.019334 * r + 0.119193 * g + 0.950227 * b;
	
}
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void RGB2XYZ( short B, short G, short R, double *X, double *Y, double *Z )
{
	*X = ( double )( 0.412453 * R ) + ( double )( 0.357580 * G ) + ( double )( 0.180423 * B );
	*Y = ( double )( 0.212671 * R ) + ( double )( 0.715160 * G ) + ( double )( 0.072169 * B );
	*Z = ( double )( 0.019334 * R ) + ( double )( 0.119193 * G ) + ( double )( 0.950227 * B );
}

void XYZ2Lab( double X, double Y, double Z, double *L, double *a, double *b )
{
	
	double fX, fY, fZ;
	
	// with the D65 reference white: Xn = 0.9505; Yn = 1.00; Zn = 1.0891;
	
	X /= ( 256 * 0.950456 );
	Y /=  256;
	Z /= ( 256 * 1.088754 );
	
	if( Y > 0.008856 )
		fY = pow( Y, 1.0 / 3.0 );
	else
		fY = 7.787 * Y + 16.0 / 116.0;
	
	*L = 116.0 * fY - 16.0;
	
	if( X > 0.008856 )
		fX = pow( X, 1.0 / 3.0 );
	else
		fX = 7.787 * X + 16.0 / 116.0;
	
	if( Z > 0.008856 )
		fZ = pow( Z, 1.0 / 3.0 );
	else
		fZ = 7.787 * Z + 16.0 / 116.0;
	
	*a = 500.0 * ( fX - fY );
	*b = 200.0 * ( fY - fZ );
	
}

void RGB2Lab( short B, short G, short R, double *L, double *a, double *b )
{
	
	double X, Y, Z;
	
	RGB2XYZ( B, G, R, &X, &Y, &Z );
	
	XYZ2Lab( X, Y, Z, L, a, b );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void __fastcall TForm1::TabControl1Change(TObject *Sender)
{
	
	switch( TabControl1 -> TabIndex )
	{
		
		case 0:
		{
			
			TabControl1 -> Tabs -> Strings[ TabControl1 -> TabIndex ] = ExtractFileName( FileListBox1 -> FileName );
			Image4 -> Picture -> LoadFromFile( FileListBox1 -> FileName );
			
			Form1 -> Caption = FileListBox1 -> FileName;
			
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height + 25;	// 25 is top frame boarder of tabcontrol
			/*
			char ColorCheckerErrorFileName[ 150 ];
			strcpy( ColorCheckerErrorFileName, FileListBox1 -> FileName.c_str() );
			
			sprintf( strrchr( ColorCheckerErrorFileName, '.' ), ".dat" );
			ColorCheckerError = fopen( ColorCheckerErrorFileName, "w" );
			*/
			break;
			
		}
		
	}
	
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Image4MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	
	static char flag = 0;
	static int FirstX;
	static int FirstY;
	static int SecondX;
	static int SecondY;
	//ShowMessage( X );ShowMessage( Y );
	if( flag == 0 )	// first time
	{
		
		FirstX = X;
		FirstY = Y;
		
		flag = 1;
		
	}
	else		// second time
	{
		
		SecondX = X;
		SecondY = Y;
		
		int LeftX;
		int LeftY;
		int RightX;
		int RightY;
		
		if( FirstX > SecondX )
		{
			
			RightX = FirstX;
			LeftX = SecondX;
			
		}
		else	// FirstX < SecondX
		{
			
			RightX = SecondX;
			LeftX = FirstX;
			
		}
		
		if( FirstY > SecondY )
		{
			
			RightY = FirstY;
			LeftY = SecondY;
			
		}
		else	// FirstY < SecondY
		{
			
			RightY = SecondY;
			LeftY = FirstY;
			
		}
		
		float AverageRValue = 0;
		float AverageGValue = 0;
		float AverageBValue = 0;
		
		for( int XIndex = LeftX; XIndex < RightX; XIndex ++ )
			for( int YIndex = LeftY; YIndex < RightY; YIndex ++ )
			{
				
				AverageRValue += GetRValue( Image4 -> Canvas -> Pixels[ XIndex ][ YIndex ] );
				AverageGValue += GetGValue( Image4 -> Canvas -> Pixels[ XIndex ][ YIndex ] );
				AverageBValue += GetBValue( Image4 -> Canvas -> Pixels[ XIndex ][ YIndex ] );
				
			}
		
		int TotalPixels = ( RightX - LeftX ) * ( RightY - LeftY );
		
		AverageRValue /= TotalPixels;
		AverageGValue /= TotalPixels;
		AverageBValue /= TotalPixels;
		
		char information[ 300 ];
		/*
		double X;
		double Y;
		double Z;
		
		RGB2XYZ( ( short )AverageBValue, ( short )AverageGValue, ( short )AverageRValue, &X, &Y, &Z );
		
		sprintf( information, "R = %f\nG = %f\nB = %f\n\nx = %f\ny = %f\nY = %f", 
				AverageRValue, AverageGValue, AverageBValue, X / ( X + Y + Z ), Y / ( X + Y + Z ), Y * 256 );
		*/
		
		double L;
		double a;
		double b;
		
		RGB2Lab( AverageBValue, AverageGValue, AverageRValue, &L, &a, &b );
		
		sprintf( information, "R = %f\nG = %f\nB = %f\n\nL = %f\na = %f\nb = %f\n\nError Distance = %f", 
				AverageRValue, AverageGValue, AverageBValue, L, a, b,  pow( a * a + b * b, 0.5 ) ) ;
		
		ShowMessage( information );
		
		flag = 0;
		
	}
	
	
	/*
	static int count = 0;
	static int FirstX;
	static int FirstY;
	static int SecondX;
	static int SecondY;
	
	if( count < 48 )
	{
		
		if( count % 2 == 0 )	// first time
		{
			
			FirstX = X;
			FirstY = Y;
			
		}
		else		// second time
		{
			
			SecondX = X;
			SecondY = Y;
			
			int LeftX;
			int LeftY;
			int RightX;
			int RightY;
			
			if( FirstX > SecondX )
			{
				
				RightX = FirstX;
				LeftX = SecondX;
				
			}
			else	// FirstX < SecondX
			{
				
				RightX = SecondX;
				LeftX = FirstX;
				
			}
			
			if( FirstY > SecondY )
			{
				
				RightY = FirstY;
				LeftY = SecondY;
				
			}
			else	// FirstY < SecondY
			{
				
				RightY = SecondY;
				LeftY = FirstY;
				
			}
			
			float AverageRValue = 0;
			float AverageGValue = 0;
			float AverageBValue = 0;
			
			for( int XIndex = LeftX; XIndex < RightX; XIndex ++ )
				for( int YIndex = LeftY; YIndex < RightY; YIndex ++ )
				{
					
					AverageRValue += GetRValue( Image4 -> Canvas -> Pixels[ XIndex ][ YIndex ] );
					AverageGValue += GetGValue( Image4 -> Canvas -> Pixels[ XIndex ][ YIndex ] );
					AverageBValue += GetBValue( Image4 -> Canvas -> Pixels[ XIndex ][ YIndex ] );
					
				}
			
			int TotalPixels = ( RightX - LeftX ) * ( RightY - LeftY );
			
			AverageRValue /= TotalPixels;
			AverageGValue /= TotalPixels;
			AverageBValue /= TotalPixels;
			
			float X;
			float Y;
			float Z;
			
			RGB2XYZ( AverageBValue, AverageGValue, AverageRValue, &X, &Y, &Z );
			
			double x = X / ( X + Y + Z );
			double y = Y / ( X + Y + Z );
			
			fprintf( ColorCheckerError, "%d %f %f\n", count / 2, ( x - xyValue[ count / 2 ][ 0 ] ) / xyValue[ count / 2 ][ 0 ], ( y  - xyValue[ count / 2 ][ 1 ] ) / xyValue[ count / 2 ][ 1 ] );
			
		}
		
		count ++;
			
	}
	else	// count > 48
	{
		fclose( ColorCheckerError );
		count = 0;
		ShowMessage( "Done!" );
	}
	*/
	
}
//---------------------------------------------------------------------------

