//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <math.h>

#include "Unit1.h"
#include "RGBCIELAB.h"
#include "bmp.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------

#define	imageWidth	2048
#define	imageHeight	1536

#define	aMin	-86.181258
#define	aMax	98.235151

#define	bMin	-107.861747
#define	bMax	94.475782

#define	aStep	( aMax - aMin ) / 2048	// x axis
#define	bStep	( bMax - bMin ) / 1536	// y axis

#define	Radius60	60
#define	Radius50	50
#define	Radius40	40
#define	Radius30	30
#define	Radius20	20
#define	Radius10	10

unsigned char **ImageDataBlock;
double **Lab_buf;

BitMapFileHeader header1;
BitMapInfoHeader header2;

__fastcall TForm1::TForm1(TComponent* Owner): TForm(Owner)
{
	
	ImageDataBlock = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	Lab_buf = ( double ** )calloc( imageHeight, sizeof( double * ) );
	
	for ( int i = 0 ; i < imageHeight ; ++ i )
	{
		
		ImageDataBlock[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );
		Lab_buf[ i ] = ( double * )calloc( imageWidth * 3, sizeof( double ) );
		
	}
	
	// 2048 x 1536
	header1.bfType = 0x4D42;			// 2 bytes
	header1.bfSize = 0x00900036;			// 4 bytes
	header1.bfReserved1 = 0x0000;			// 2 bytes
	header1.bfReserved2 = 0x0000;			// 2 bytes
	header1.bfOffBits = 0x00000036;		// 4 bytes
	
	// 40 bytes
	header2.biSize = 0x00000028;			// 4 bytes
	header2.biWidth = 0x00000800;			// 4 bytes
	header2.biHeight = 0x00000600;		// 4 bytes
	header2.biPlanes = 0x0001;			// 2 bytes
	header2.biBitCount = 0x0018;			// 2 bytes
	header2.biCompression = 0x00000000;		// 4 bytes
	header2.biSizeImage = 0x00900000;		// 4 bytes
	header2.biXPelsPerMeter = 0x000000B13;		// 4 bytes
	header2.biYpelsPerMeter = 0x000000B13;		// 4 bytes
	header2.biClrUsed = 0x00000000;		// 4 bytes
	header2.biClrImportant = 0x00000000;		// 4 bytes
	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FileListBox1Click(TObject *Sender)
{
	TabControl1Change( Sender );
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TabControl1Change(TObject *Sender)
{
	
	switch( TabControl1 -> TabIndex )
	{
		
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		{
			
			char CIELabChartFileName[ 150 ];
			int LValue;
			
			switch( TabControl1 -> TabIndex )
			{
				
				case 0:	strcpy( CIELabChartFileName, "CIELabChartL0.BMP" ); LValue = 0; break;
				case 1:	strcpy( CIELabChartFileName, "CIELabChartL25.BMP" ); LValue = 25; break;
				case 2:	strcpy( CIELabChartFileName, "CIELabChartL50.BMP" ); LValue = 50; break;
				case 3:	strcpy( CIELabChartFileName, "CIELabChartL75.BMP" ); LValue = 75; break;
				case 4:	strcpy( CIELabChartFileName, "CIELabChartL90.BMP" ); LValue = 90; break;
				case 5:	strcpy( CIELabChartFileName, "CIELabChartL100.BMP" ); LValue = 100; break;
				
			}
			
			//FILE *test = fopen( "test.dat", "w" );
			
			if( !FileExists( CIELabChartFileName ) )
			{
				
				double aValue = aMin;
				double bValue = bMax;
				
				for( int i = 0; i < imageHeight; i ++, bValue -= bStep, aValue = aMin )
					for( int j = 0; j < imageWidth * 3; j += 3, aValue += aStep )
					{
						
						Lab_buf[ i ][ j + 0 ] = LValue;	// L
						Lab_buf[ i ][ j + 1 ] = aValue;	// a
						Lab_buf[ i ][ j + 2 ] = bValue;	// b
						
						Lab2RGB( Lab_buf[ i ][ j + 0 ], Lab_buf[ i ][ j + 1 ], Lab_buf[ i ][ j + 2 ],
							&ImageDataBlock[ i ][ j + 0 ], &ImageDataBlock[ i ][ j + 1 ], &ImageDataBlock[ i ][ j + 2 ] );
						
						//fprintf( test, "a = %f, b = %f\n", Lab_buf[ i ][ j + 1 ], Lab_buf[ i ][ j + 2 ] );
						
					}
				
				//fclose( test );
				
				double aZeroValue = 100;	// dumb value
				double bZeroValue = 100;	// dumb value
				
				int bZero;
				int aZero;
				
				// b zero point
				for( int i = 0; i < imageHeight; i ++ )
					if( fabs( Lab_buf[ i ][ 2 ] ) < fabs( bZeroValue ) )
					{
						bZeroValue = Lab_buf[ i ][ 2 ];
						bZero = i;
					}
				
				// a zero point
				for( int j = 0; j < imageWidth; j ++ )
					if( fabs( Lab_buf[ 0 ][ j * 3 + 1 ] ) < fabs( aZeroValue ) )
					{
						aZeroValue = Lab_buf[ 0 ][ j * 3 + 1 ];
						aZero = j;
					}
				
				// pad white to outside range of circle
				for( int i = 0; i < imageHeight; i ++ )
					for( int j = 0; j < imageWidth ; j ++ )
						if( pow( pow( j - aZero, 2 ) + pow( i - bZero, 2 ), 0.5 ) > Radius60 / ( aMax - aMin ) * imageWidth )
						{
							ImageDataBlock[ i ][ j * 3 + 0 ] = 255;	// B element
							ImageDataBlock[ i ][ j * 3 + 1 ] = 255;	// G element
							ImageDataBlock[ i ][ j * 3 + 2 ] = 255;	// R element
						}
				
				// plot black ring inside circle, radius 50
				for( int i = 0; i < imageHeight; i ++ )
					for( int j = 0; j < imageWidth ; j ++ )
						if( ( pow( pow( j - aZero, 2 ) + pow( i - bZero, 2 ), 0.5 ) - Radius50 / ( aMax - aMin ) * imageWidth ) > -0.5
						&& ( pow( pow( j - aZero, 2 ) + pow( i - bZero, 2 ), 0.5 ) - Radius50 / ( aMax - aMin ) * imageWidth ) < 0.5 )
						{
							ImageDataBlock[ i ][ j * 3 + 0 ] = 0;	// B element
							ImageDataBlock[ i ][ j * 3 + 1 ] = 0;	// G element
							ImageDataBlock[ i ][ j * 3 + 2 ] = 0;	// R element
						}
				
				// plot black ring inside circle, radius 40
				for( int i = 0; i < imageHeight; i ++ )
					for( int j = 0; j < imageWidth ; j ++ )
						if( ( pow( pow( j - aZero, 2 ) + pow( i - bZero, 2 ), 0.5 ) - Radius40 / ( aMax - aMin ) * imageWidth ) > -0.5
						&& ( pow( pow( j - aZero, 2 ) + pow( i - bZero, 2 ), 0.5 ) - Radius40 / ( aMax - aMin ) * imageWidth ) < 0.5 )
						{
							ImageDataBlock[ i ][ j * 3 + 0 ] = 0;	// B element
							ImageDataBlock[ i ][ j * 3 + 1 ] = 0;	// G element
							ImageDataBlock[ i ][ j * 3 + 2 ] = 0;	// R element
						}
				
				// plot black ring inside circle, radius 30
				for( int i = 0; i < imageHeight; i ++ )
					for( int j = 0; j < imageWidth ; j ++ )
						if( ( pow( pow( j - aZero, 2 ) + pow( i - bZero, 2 ), 0.5 ) - Radius30 / ( aMax - aMin ) * imageWidth ) > -0.5
						&& ( pow( pow( j - aZero, 2 ) + pow( i - bZero, 2 ), 0.5 ) - Radius30 / ( aMax - aMin ) * imageWidth ) < 0.5 )
						{
							ImageDataBlock[ i ][ j * 3 + 0 ] = 0;	// B element
							ImageDataBlock[ i ][ j * 3 + 1 ] = 0;	// G element
							ImageDataBlock[ i ][ j * 3 + 2 ] = 0;	// R element
						}
				
				// plot black ring inside circle, radius 20
				for( int i = 0; i < imageHeight; i ++ )
					for( int j = 0; j < imageWidth ; j ++ )
						if( ( pow( pow( j - aZero, 2 ) + pow( i - bZero, 2 ), 0.5 ) - Radius20 / ( aMax - aMin ) * imageWidth ) > -0.5
						&& ( pow( pow( j - aZero, 2 ) + pow( i - bZero, 2 ), 0.5 ) - Radius20 / ( aMax - aMin ) * imageWidth ) < 0.5 )
						{
							ImageDataBlock[ i ][ j * 3 + 0 ] = 0;	// B element
							ImageDataBlock[ i ][ j * 3 + 1 ] = 0;	// G element
							ImageDataBlock[ i ][ j * 3 + 2 ] = 0;	// R element
						}
				
				// plot black ring inside circle, radius 10
				for( int i = 0; i < imageHeight; i ++ )
					for( int j = 0; j < imageWidth ; j ++ )
						if( ( pow( pow( j - aZero, 2 ) + pow( i - bZero, 2 ), 0.5 ) - Radius10 / ( aMax - aMin ) * imageWidth ) > -0.5
						&& ( pow( pow( j - aZero, 2 ) + pow( i - bZero, 2 ), 0.5 ) - Radius10 / ( aMax - aMin ) * imageWidth ) < 0.5 )
						{
							ImageDataBlock[ i ][ j * 3 + 0 ] = 0;	// B element
							ImageDataBlock[ i ][ j * 3 + 1 ] = 0;	// G element
							ImageDataBlock[ i ][ j * 3 + 2 ] = 0;	// R element
						}
				
				// plot a axis
				for( int j = 0; j < imageWidth * 3; j += 3 )
				{
					ImageDataBlock[ bZero ][ j + 0 ] = 0;	// B element
					ImageDataBlock[ bZero ][ j + 1 ] = 0;	// G element
					ImageDataBlock[ bZero ][ j + 2 ] = 0;	// R element
				}
				
				// plot b axis
				for( int i = 0; i < imageHeight; i ++ )
				{
					ImageDataBlock[ i ][ aZero * 3 + 0 ] = 0;	// B element
					ImageDataBlock[ i ][ aZero * 3 + 1 ] = 0;	// G element
					ImageDataBlock[ i ][ aZero * 3 + 2 ] = 0;	// R element
				}
				
				FILE *fp = fopen( CIELabChartFileName, "wb" );
				
				fwrite( &header1.bfType, 2, 1, fp );
				fwrite( &header1.bfSize, 4, 1, fp );
				fwrite( &header1.bfReserved1, 2, 1, fp );
				fwrite( &header1.bfReserved2, 2, 1, fp );
				fwrite( &header1.bfOffBits, 4, 1, fp );
				
				fwrite( &header2, sizeof( BitMapInfoHeader ), 1, fp );
				
				for( int i = imageHeight - 1; i >= 0; i -- )
					fwrite( ImageDataBlock[ i ], 3 * imageWidth, 1, fp );
				
				fclose( fp );
				
			}
			
			TabControl1 -> Tabs -> Strings[ TabControl1 -> TabIndex ] = ExtractFileName( CIELabChartFileName );
			Image4 -> Picture -> LoadFromFile( CIELabChartFileName );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
			
		}
		
	}
}
//---------------------------------------------------------------------------

