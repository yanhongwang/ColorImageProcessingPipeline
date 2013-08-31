//---------------------------------------------------------------------------

// Combining Gray World and Retinex Theory for
// Automatic White Balance in Digital Photography
// Edmund Y. Lam, Member, IEEE
//

#include <vcl.h>
#pragma hdrstop

#include <math.h>

#include "Unit1.h"
#include "bmp.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------

#define	imageWidth	2048
#define	imageHeight	1536

unsigned char **ImageDataBlock;

BitMapFileHeader header1;
BitMapInfoHeader header2;

// ************************* ReadBMPData **********************************
// * Function: This function is used for extracting Bayer Pattern from a  *
// *           BMP file.                                                  *
// * Input: the name of a BMP file.                                       *
// * Output: bayer pattern                                                *
// ************************************************************************
void ReadBMPData( char* filename )
{
	
	FILE *fp;
	
	// open test file
	fp = fopen( filename, "rb" );
	// read BitMapFileHeader
	// struct "BitMapFileHeader" is read 4 bytes by hardware but size of struct "BitMapFileHeader" is 14 bytes,
	// It will be assumed 16 bytes by the hardware, and then it has to minus 2 bytes.
	fread( &header1, 14, 1, fp );
	
	// read BitMapInfoHeader
	fread( &header2, 40, 1, fp );
	
	// according to width and height. Read Image Data Block in order to make RGB buffer
	
	for( int i = 0; i < imageHeight; i ++ )
		fread( ImageDataBlock[ i ], 3 * imageWidth, 1, fp );
		
	fclose( fp );
}

__fastcall TForm1::TForm1(TComponent* Owner): TForm(Owner)
{
	
	ImageDataBlock = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	
	for ( int i = 0 ; i < imageHeight ; ++ i )
		ImageDataBlock[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );

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
		{
			
			TabControl1 -> Tabs -> Strings[ TabControl1 -> TabIndex ] = ExtractFileName( FileListBox1 -> FileName );
			Image4 -> Picture -> LoadFromFile( FileListBox1 -> FileName );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
			
		}
		
		case 1:
		{
			
			char QuadraticMappingFileName[ 150 ];
			
			strcpy( QuadraticMappingFileName, FileListBox1 -> FileName.c_str() );
			
			sprintf( strrchr( QuadraticMappingFileName, '.' ), "_quadratic.bmp" );
			
			if( !FileExists( QuadraticMappingFileName ) )
			{
				
				ReadBMPData( FileListBox1 -> FileName.c_str() );
				
				// for gray world
				double total_R = 0;
				double total_G = 0;
				double total_B = 0;
				
				double total_R_square = 0;
				double total_B_square = 0;
				
				// for retinex
				double max_r = 0;
				double max_g = 0;
				double max_b = 0;
				
				for( int i = 0; i < imageHeight; i++ )
					for( int j = 0; j < imageWidth * 3; j += 3 )
					{
						
						// for gray world
						total_B += ImageDataBlock[ i ][ j + 0 ];
						total_G += ImageDataBlock[ i ][ j + 1 ];
						total_R += ImageDataBlock[ i ][ j + 2 ];
						total_B_square += ImageDataBlock[ i ][ j + 0 ] * ImageDataBlock[ i ][ j + 0 ];
						total_R_square += ImageDataBlock[ i ][ j + 2 ] * ImageDataBlock[ i ][ j + 2 ];
						
						// for retinex
						if( max_b < ImageDataBlock[ i ][ j + 0 ] )
							max_b = ImageDataBlock[ i ][ j + 0 ];
						if( max_g < ImageDataBlock[ i ][ j + 1 ] )
							max_g = ImageDataBlock[ i ][ j + 1 ];
						if( max_r < ImageDataBlock[ i ][ j + 2 ] )
							max_r = ImageDataBlock[ i ][ j + 2 ];
						
					}
				
				double max_r_square = max_r * max_r;
				
				double max_b_square = max_b * max_b;
				
				// au + bv = x
				// cu + dv = y
				
				// adu + bdv = dx
				// cbu + dbv = by
				// ( ad - cb )u = dx - by
				// u = ( dx - by ) / ( ad - cb )
				
				// acu + bcv = cx
				// cau + dav = ay
				// ( bc - da )v = cx - ay
				// v = ( cx - ay ) / ( bc - da )
				
				// R channel
				// u = ( dx - by ) / ( ad - cb )
				double FactorRU = ( max_r * total_G - total_R * max_g ) / ( total_R_square * max_r - max_r_square * total_R );
				// R channel
				// v = ( cx - ay ) / ( bc - da )
				double FactorRV = ( max_r_square * total_G - total_R_square * max_g ) / ( total_R * max_r_square - max_r * total_R_square );
				
				// B channel
				// u = ( dx - by ) / ( ad - cb )
				double FactorBU = ( max_b * total_G - total_B * max_g ) / ( total_B_square * max_b - max_b_square * total_B );
				// B channel
				// v = ( cx - ay ) / ( bc - da )
				double FactorBV = ( max_b_square * total_G - total_B_square * max_g ) / ( total_B * max_b_square - max_b * total_B_square );
				
				double tempValue;
				
				for( int i = 0; i < imageHeight; i++ )
					for( int j = 0; j < imageWidth * 3; j += 3 )
					{
						
						tempValue = FactorBU * ImageDataBlock[ i ][ j + 0 ] * ImageDataBlock[ i ][ j + 0 ] + FactorBV * ImageDataBlock[ i ][ j + 0 ];
						ImageDataBlock[ i ][ j + 0 ] = ( unsigned char )( tempValue < 0 ? 0 : tempValue >  255 ? 255 : tempValue );
						
						tempValue = FactorRU * ImageDataBlock[ i ][ j + 2 ] * ImageDataBlock[ i ][ j + 2 ] + FactorRV * ImageDataBlock[ i ][ j + 2 ];
						ImageDataBlock[ i ][ j + 2 ] = ( unsigned char )( tempValue < 0 ? 0 : tempValue >  255 ? 255 : tempValue );
						
					}
				
				FILE *fp = fopen( QuadraticMappingFileName, "wb" );
				
				fwrite( &header1, sizeof( BitMapFileHeader ) - 2, 1, fp );
				fwrite( &header2, sizeof( BitMapInfoHeader ), 1, fp );
				
				for( int i = 0; i < imageHeight; i ++ )
					fwrite( ImageDataBlock[ i ], 3 * imageWidth, 1, fp );
				
				fclose( fp );
				
			}
			
			Image4 -> Picture -> LoadFromFile( QuadraticMappingFileName );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
			
		}

	}
}
//---------------------------------------------------------------------------
