//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "RGBCIELAB.h"
#include "bmp.h"
#include "edge.h"
#include "PSNR.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------

#define	imageWidth	640
#define	imageHeight	480

unsigned char **ImageDataBlock;
unsigned char **ucImageOut;//image data block output, every pixel has RGB colors
double **Lab_buf;

unsigned char **edg_Bbuf;	// allocate memory to B prefiltering buffer
unsigned char **Rembuf;

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
	ucImageOut = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	Lab_buf = ( double ** )calloc( imageHeight, sizeof( double * ) );
	edg_Bbuf = ( unsigned char ** )calloc( imageHeight * 3, sizeof( unsigned char * ) );	// allocate memory to B prefiltering buffer
	Rembuf = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	
	for ( int i = 0 ; i < imageHeight ; ++ i )
	{
		
		ImageDataBlock[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );
		ucImageOut[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );
		Lab_buf[ i ] = ( double * )calloc( imageWidth * 3, sizeof( double ) );
		edg_Bbuf[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );
		Rembuf[ i ] = ( unsigned char * )calloc( imageWidth, sizeof( unsigned char ) );
		
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
			
			char PSNR_EMF[ 150 ];
			
			sprintf( PSNR_EMF, "%s_PSNR.emf", ExtractFileName( ExtractFileDir( FileListBox1 -> FileName ) ) );
			
			if( !FileExists( PSNR_EMF ) )
			{
				
				int i, j;
				char ImageFileName[ 150 ];
				int intBilSize = 1;
				int intCloseness = 10;
				int intSimilarity = 10;
				int intApre = 6;
				int intEdgeDet = 20;
				
				double PSNRG = 0;
				double PSNRR = 0;
				double PSNRB = 0;
				
				FILE *PSNRDataR = fopen( "PSNRDataR.dat" , "w" );
				FILE *PSNRDataG = fopen( "PSNRDataG.dat" , "w" );
				FILE *PSNRDataB = fopen( "PSNRDataB.dat" , "w" );
				
				for( int n = 0; n < FileListBox1 -> Items -> Count; n ++ )
				{
					
					strcpy( ImageFileName, FileListBox1 -> Items -> Strings[ n ].c_str() );
					
					sprintf( strrchr( ImageFileName, '.' ), "%s", "_bilateral_edge.bmp" );
					
					ReadBMPData( FileListBox1 -> Items -> Strings[ n ].c_str() );
					
					for( i = 0; i < imageHeight; i ++ )
						for( j = 0; j < imageWidth * 3; j ++ )
							ucImageOut[ i ][ j ] = ImageDataBlock[ i ][ j ];
					
					edge_prefiltering( imageHeight, imageWidth, ImageDataBlock, edg_Bbuf, intApre );
					
					edge_detection( imageHeight , imageWidth , edg_Bbuf , intEdgeDet , Rembuf );
					
					bilateral( imageHeight, imageWidth, ImageDataBlock, intBilSize, intCloseness, intSimilarity,  Rembuf );
					
					for( i = 0; i < imageHeight; i ++ )
						for( j = 0; j < imageWidth * 3; j += 3 )
							RGB2Lab( ImageDataBlock[ i ][ j + 0 ], ImageDataBlock[ i ][ j + 1 ], ImageDataBlock[ i ][ j + 2 ],
								&Lab_buf[ i ][ j + 0 ], &Lab_buf[ i ][ j + 1 ], &Lab_buf[ i ][ j + 2 ] );
					
					edge_enhancement( imageHeight , imageWidth , Lab_buf , Rembuf );
					
					for( i = 0; i < imageHeight; i ++ )
						for( j = 0; j < imageWidth * 3; j += 3 )
							Lab2RGB( Lab_buf[ i ][ j + 0 ], Lab_buf[ i ][ j + 1 ], Lab_buf[ i ][ j + 2 ],
								&ImageDataBlock[ i ][ j + 0 ], &ImageDataBlock[ i ][ j + 1 ], &ImageDataBlock[ i ][ j + 2 ] );
					
					PSNR( ImageDataBlock, ucImageOut, &PSNRG, &PSNRR, &PSNRB );
					
					fprintf( PSNRDataR, "%d %f\n", n, PSNRR );
					
					fprintf( PSNRDataG, "%d %f\n", n, PSNRG );
					
					fprintf( PSNRDataB, "%d %f\n", n, PSNRB );
					
					FILE *fp = fopen( ImageFileName, "wb" );
					
					fwrite( &header1, sizeof( BitMapFileHeader ) - 2, 1, fp );
					fwrite( &header2, sizeof( BitMapInfoHeader ), 1, fp );
					
					for( i = 0; i < imageHeight; i ++ )
						fwrite( ImageDataBlock[ i ], 3 * imageWidth, 1, fp );
					
					fclose( fp );
					
				}
				
				fclose( PSNRDataR );
				fclose( PSNRDataG );
				fclose( PSNRDataB );
				
				FILE *PSNRScript = fopen( "PSNRScript.dem" , "w" );
				
				fprintf( PSNRScript, "set terminal emf color\n" );
				fprintf( PSNRScript, "set xrange [ 0 : %d ]\n", FileListBox1 -> Items -> Count );
				
				fprintf( PSNRScript, "set xlabel 'Picture'\n" );
				fprintf( PSNRScript, "set ylabel 'PSNR'\n" );
				
				fprintf( PSNRScript, "set grid\n" );
				
				fprintf( PSNRScript, "set title 'Noise Fileter and Edge Enhancement result in PSNR domain'\n" );
				fprintf( PSNRScript, "set ticslevel 0\n" );
				
				fprintf( PSNRScript, "set output '%s'\n", PSNR_EMF );
				
				fprintf( PSNRScript, "plot 'PSNRDataR.dat' title 'R of PSNR' with linespoints 1 2,\
							'PSNRDataG.dat' title 'G of PSNR' with linespoints 2 3,\
							'PSNRDataB.dat' title 'B of PSNR' with linespoints 3 4\n"
					);
				
				fclose( PSNRScript );
				
				char command[ 200 ];
				sprintf( command, "%s\\gnuplot\\bin\\wgnuplot.exe PSNRScript.dem", ExtractFileDrive( FileListBox1 -> FileName ) );
				
				system( command );
				
				//remove( "PSNRDataR.dat" );
				//remove( "PSNRDataG.dat" );
				//remove( "PSNRDataB.dat" );
				//remove( "PSNRScript.dem" );
				
			}
			
			Image4 -> Picture -> LoadFromFile( PSNR_EMF );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
		}
		
	}
}
//---------------------------------------------------------------------------
