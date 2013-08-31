//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "bmp.h"
#include "YCbCrRGB.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------

#define	imageWidth	2048
#define	imageHeight	1536

unsigned char **ImageDataBlock;
unsigned char **YCbCrDataBlock;

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
	YCbCrDataBlock = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	
	for ( int i = 0 ; i < imageHeight ; ++ i )
	{
		
		ImageDataBlock[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );
		YCbCrDataBlock[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );
		
	}

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
			
			char ColorSaturationFileName[ 150 ];
			strcpy( ColorSaturationFileName, FileListBox1 -> FileName.c_str() );
			
			sprintf( strrchr( ColorSaturationFileName, '.' ), "_YCbCr.bmp" );
			
			if( !FileExists( ColorSaturationFileName ) )
			{
				
				ReadBMPData( FileListBox1 -> FileName.c_str() );
				
				for( int i = 0; i < imageHeight; i ++ )
					for( int j = 0; j < imageWidth * 3; j += 3 )
					{
						
						RGB2YCbCr( ImageDataBlock[ i ][ j + 0 ], ImageDataBlock[ i ][ j + 1 ], ImageDataBlock[ i ][ j + 2 ],
							&YCbCrDataBlock[ i ][ j + 0 ], &YCbCrDataBlock[ i ][ j + 1 ], &YCbCrDataBlock[ i ][ j + 2 ] );
						
						YCbCrDataBlock[ i ][ j + 1 ] *= 1.1;
						YCbCrDataBlock[ i ][ j + 2 ] *= 1.1;
						
						YCbCr2RGB( YCbCrDataBlock[ i ][ j + 0 ], YCbCrDataBlock[ i ][ j + 1 ], YCbCrDataBlock[ i ][ j + 2 ],
							&ImageDataBlock[ i ][ j + 0 ], &ImageDataBlock[ i ][ j + 1 ], &ImageDataBlock[ i ][ j + 2 ] );
						
					}
				
				FILE *fp = fopen( ColorSaturationFileName, "wb" );
				
				fwrite( &header1, sizeof( BitMapFileHeader ) - 2, 1, fp );
				fwrite( &header2, sizeof( BitMapInfoHeader ), 1, fp );
				
				for( int i = 0; i < imageHeight; i ++ )
					fwrite( ImageDataBlock[ i ], 3 * imageWidth, 1, fp );
				
				fclose( fp );
				
			}
			
			Image4 -> Picture -> LoadFromFile( ColorSaturationFileName );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
			
		}
		
	}
}
//---------------------------------------------------------------------------

