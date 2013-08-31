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

//#define	imageWidth	2048
//#define	imageHeight	1536

#define	imageWidth	200
#define	imageHeight	210

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
	/*
	// 2048 x 1536
	header1.bfType = 0x4D42;			// 2 bytes
	header1.bfSize = 0x00900000;			// 4 bytes
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
	*/
	
	// 200 x 210
	header1.bfType = 0x4D42;			// 2 bytes
	header1.bfSize = 0x0001EC66;			// 4 bytes
	header1.bfReserved1 = 0x0000;			// 2 bytes
	header1.bfReserved2 = 0x0000;			// 2 bytes
	header1.bfOffBits = 0x00000036;		// 4 bytes
	
	// 40 bytes
	header2.biSize = 0x00000028;			// 4 bytes
	header2.biWidth = 0x000000C8;		// 4 bytes
	header2.biHeight = 0x000000D2;		// 4 bytes
	header2.biPlanes = 0x0001;			// 2 bytes
	header2.biBitCount = 0x0018;			// 2 bytes
	header2.biCompression = 0x00000000;		// 4 bytes
	header2.biSizeImage = 0x0001EC30;		// 4 bytes
	header2.biXPelsPerMeter = 0x000000000;		// 4 bytes
	header2.biYpelsPerMeter = 0x000000000;		// 4 bytes
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
			
			if( !FileExists( CIELabChartFileName ) )
			{
			
				for( int i = 0; i < imageHeight; i ++ )
					for( int j = 0; j < imageWidth * 3; j += 3 )
					{
						
						ImageDataBlock[ i ][ j + 0 ] = 255;
						ImageDataBlock[ i ][ j + 1 ] = 255;
						ImageDataBlock[ i ][ j + 2 ] = 255;
						
					}
				
				double L;
				double a;
				double b;
				
				//FILE *test = fopen( "test.dat", "w" );
				
				for( int  R = 0; R <= 255; R ++ )
					for( int G = 0; G <= 255; G ++ )
						for( int B = 0; B <= 255; B ++ )
						{
							
							RGB2Lab( ( unsigned char )B, ( unsigned char )G, ( unsigned char )R, &L, &a, &b );
							
							if( L >= LValue - 1 && L <= LValue + 1 )
							{
								
								//fprintf( test, "R = %d, G = %d, B = %d, L = %f, a = %f, b = %f\n", R, G, B, L, a, b );
								
								ImageDataBlock[ ( int )( b + 107 + 0.5 ) ][ ( int )( a + 86 + 0.5 ) * 3 + 0  ] = ( unsigned char )B;
								ImageDataBlock[ ( int )( b + 107 + 0.5 ) ][ ( int )( a + 86 + 0.5 ) * 3 + 1  ] = ( unsigned char )G;
								ImageDataBlock[ ( int )( b + 107 + 0.5 ) ][ ( int )( a + 86 + 0.5 ) * 3 + 2  ] = ( unsigned char )R;
								
							}
							
						}
				
				//	fclose( test );
				
				// plot a axis
				for( int j = 0; j < imageWidth * 3; j += 3 )
				{
					
					ImageDataBlock[ 107 ][ j + 0 ] = 0;	// B element
					ImageDataBlock[ 107 ][ j + 1 ] = 0;	// G element
					ImageDataBlock[ 107 ][ j + 2 ] = 0;	// R element
					
				}
				
				// plot b axis
				for( int i = 0; i < imageHeight; i ++ )
				{
					
					ImageDataBlock[ i ][ 86 * 3 + 0 ] = 0;	// B element
					ImageDataBlock[ i ][ 86 * 3 + 1 ] = 0;	// G element
					ImageDataBlock[ i ][ 86 * 3 + 2 ] = 0;	// R element
					
				}
				
				FILE *fp = fopen( CIELabChartFileName, "wb" );
				
				fwrite( &header1.bfType, 2, 1, fp );
				fwrite( &header1.bfSize, 4, 1, fp );
				fwrite( &header1.bfReserved1, 2, 1, fp );
				fwrite( &header1.bfReserved2, 2, 1, fp );
				fwrite( &header1.bfOffBits, 4, 1, fp );
				
				fwrite( &header2, sizeof( BitMapInfoHeader ), 1, fp );
				
				for( int i = 0; i < imageHeight; i ++ )
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

