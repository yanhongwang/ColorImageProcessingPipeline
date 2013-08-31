//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "RGBCIELAB.h"
#include "bmp.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------

struct MacroBlock
{
	
	double DCL;
	
};

#define	imageWidth	640
#define	imageHeight	480

double ***AllDCL;

unsigned char **ImageDataBlock;
double **Lab_buf;

float **stddev;

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
	Lab_buf = ( double ** )calloc( imageHeight, sizeof( double * ) );
	stddev =  ( float ** )calloc( imageHeight, sizeof( float * ) );
	
	AllDCL = ( double *** ) calloc( 100, sizeof( double ** ) );
				
	for( int i = 0; i < 100; i ++ )
		AllDCL[ i ] = ( double ** ) calloc( imageHeight, sizeof( double * ) );
	
	for( int i = 0; i < 100; i ++ )
		for( int j = 0; j < imageHeight; j ++ )
			AllDCL[ i ][ j ] = ( double * ) calloc( imageWidth, sizeof( double ) );
				
	for ( int i = 0 ; i < imageHeight ; ++ i )
	{
		
		ImageDataBlock[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );
		Lab_buf[ i ] = ( double * )calloc( imageWidth * 3, sizeof( double ) );
		stddev[ i ] = ( float * )calloc( imageWidth, sizeof( float ) );
		
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
			
			if( !FileExists( "MeanFile.txt" ) || !FileExists( "standardDeviation.txt" ) )
			{
				
				char filename[ 150 ];
				
				MacroBlock MeanBlock[ imageHeight / 16 ][ imageWidth / 16 ];
				
				MacroBlock block[ imageHeight / 16 ][ imageWidth / 16 ];
				
				for( int n = 0; n < FileListBox1 -> Items -> Count; n ++ )
				{
					
					memset( block, 0, sizeof( block ) );
					memset( MeanBlock, 0, sizeof( MeanBlock ) );
					
					sprintf( filename, "%s%s", ExtractFilePath( FileListBox1 -> FileName ), FileListBox1 -> Items -> Strings[ n ].c_str() );
					
					ReadBMPData( filename );
					
					double LSum = 0;
					
					for( int i = 0; i < imageHeight / 16; i ++ )
						for( int j = 0; j < imageWidth / 16; LSum = 0, j ++ )
						{
							for( int k = i * 16; k <= ( i * 16 + 15 ); k ++ )
								for( int m = j * 16 * 3; m <= ( j * 16 + 15 ) * 3; m += 3 )
								{
									RGB2Lab( ImageDataBlock[ k ][ m + 0 ], ImageDataBlock[ k ][ m + 1 ], ImageDataBlock[ k ][ m + 2 ],
										&Lab_buf[ k ][ m + 0 ], &Lab_buf[ k ][ m + 1 ], &Lab_buf[ k ][ m + 2 ] );
									
									LSum += Lab_buf[ k ][ m + 0 ];
									
								}
							
							block[ i ][ j ].DCL = LSum / 256.0;
							
							MeanBlock[ i ][ j ].DCL += block[ i ][ j ].DCL;
							
							AllDCL[ n ][ i ][ j ] = block[ i ][ j ].DCL;
							
						}
					
				}
				
				for( int i = 0; i < imageHeight / 16; i ++ )
					for( int j = 0; j < imageWidth / 16; j ++ )
						MeanBlock[ i ][ j ].DCL /= FileListBox1 -> Items -> Count;
				
				FILE *MeanFile = fopen( "MeanFile.txt", "w" );
				
				for( int i = 0; i < imageHeight / 16; i ++ )
				{
					for( int j = 0; j < imageWidth / 16; j ++ )
						fprintf( MeanFile, "%f ", MeanBlock[ i ][ j ].DCL );
					
					fprintf( MeanFile, "\n" );
				}
				
				fclose( MeanFile );
				
				for( int i = 0; i < 100; i ++ )
					for( int j = 0; j < imageHeight / 16; j ++ )
						for( int k = 0; k < imageWidth / 16; k ++ )
							stddev[ j ][ k ] += pow( AllDCL[ i ][ j ][ k ] - MeanBlock[ j ][ k ].DCL, 2 );
				
				for( int i = 0; i < imageHeight / 16; i ++ )
					for( int j = 0; j < imageWidth / 16; j ++ )
						stddev[ i ][ j ] = pow( stddev[ i ][ j ] / ( FileListBox1 -> Items -> Count - 1 ), 0.5 );
				
				FILE *StandardDeviation = fopen( "standardDeviation.txt", "w" );
				
				for( int i = 0; i < imageHeight / 16; i ++ )
				{
					for( int j = 0; j < imageWidth / 16; j ++ )
						fprintf( StandardDeviation, "%f ", stddev[ i ][ j ] );
					
					fprintf( StandardDeviation, "\n" );
				}
				
				fclose( StandardDeviation );
				
			}
			
			break;
		}
		
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	free( AllDCL );
}
//---------------------------------------------------------------------------

