//---------------------------------------------------------------------------

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

#define	GammaValue0004	00.04
#define	GammaValue0010	00.10
#define	GammaValue0020	00.20
#define	GammaValue0040	00.40
#define	GammaValue0045	00.45
#define	GammaValue0067	00.67
#define	GammaValue0100	01.00
#define	GammaValue0150	01.50
#define	GammaValue0222	02.22
#define	GammaValue0250	02.50
#define	GammaValue0500	05.00
#define	GammaValue1000	10.00
#define	GammaValue2500	25.00

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
			
			char GammaCorrectionFileName[ 150 ];
			
			strcpy( GammaCorrectionFileName, FileListBox1 -> FileName.c_str() );
			
			sprintf( strrchr( GammaCorrectionFileName, '.' ), "_standard_gamma_045.bmp" );
			
			if( !FileExists( GammaCorrectionFileName ) )
			{
				
				int EndPointX = 10;
				
				float EndPointY = pow( EndPointX / 256.0, GammaValue0045 ) * 256.0;
				
				float slope = EndPointY / EndPointX;
				
				ReadBMPData( FileListBox1 -> FileName.c_str() );
				
				for( int i = 0; i < imageHeight; i ++ )
					for( int j = 0; j < imageWidth * 3; j += 3 )
					{
						
						if( ImageDataBlock[ i ][ j + 0 ] >= EndPointX )
							ImageDataBlock[ i ][ j + 0 ] = pow( ImageDataBlock[ i ][ j + 0 ] / 256.0, GammaValue0045 ) * 256.0;
						else
							ImageDataBlock[ i ][ j + 0 ] *= slope;
						
						if( ImageDataBlock[ i ][ j + 1 ] >= EndPointX )
							ImageDataBlock[ i ][ j + 1 ] = pow( ImageDataBlock[ i ][ j + 1 ] / 256.0, GammaValue0045 ) * 256.0;
						else
							ImageDataBlock[ i ][ j + 1 ] *= slope;
						
						if( ImageDataBlock[ i ][ j + 2 ] >= EndPointX )
							ImageDataBlock[ i ][ j + 2 ] = pow( ImageDataBlock[ i ][ j + 2 ] / 256.0, GammaValue0045 ) * 256.0;
						else
							ImageDataBlock[ i ][ j + 2 ] *= slope;
						
					}
				
				FILE *fp = fopen( GammaCorrectionFileName, "wb" );
				
				fwrite( &header1, sizeof( BitMapFileHeader ) - 2, 1, fp );
				fwrite( &header2, sizeof( BitMapInfoHeader ), 1, fp );
				
				for( int i = 0; i < imageHeight; i ++ )
					fwrite( ImageDataBlock[ i ], 3 * imageWidth, 1, fp );
				
				fclose( fp );
				
			}
			
			Image4 -> Picture -> LoadFromFile( GammaCorrectionFileName );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
			
		}
		
		case 2:
		{
			
			char GammaCurveFileName[ 150 ];
			strcpy( GammaCurveFileName, "GammaCurve.emf" );
			
			if( !FileExists( GammaCurveFileName ) )
			{
				
				FILE *GammaCurveData = fopen( "GammaCurveData.dat", "w" );
				
				float	GammaCurveValue0004;
				float	GammaCurveValue0010;
				float	GammaCurveValue0020;
				float	GammaCurveValue0040;
				float	GammaCurveValue0045;
				float	GammaCurveValue0067;
				float	GammaCurveValue0100;
				float	GammaCurveValue0150;
				float	GammaCurveValue0222;
				float	GammaCurveValue0250;
				float	GammaCurveValue0500;
				float	GammaCurveValue1000;
				float	GammaCurveValue2500;
				/*
				int EndPointX = 10;
				
				float EndPointY = pow( EndPointX / 256.0, GammaValue0045 ) * 256.0;
				
				float slope = EndPointY / EndPointX;
				*/
				for( int i = 0; i <= 255; i ++ )
				{
					
					GammaCurveValue0004 = pow( i / 256.0, GammaValue0004 ) * 256.0;
					GammaCurveValue0010 = pow( i / 256.0, GammaValue0010 ) * 256.0;
					GammaCurveValue0020 = pow( i / 256.0, GammaValue0020 ) * 256.0;
					GammaCurveValue0040 = pow( i / 256.0, GammaValue0040 ) * 256.0;
					
					//if( i >= EndPointX )
						GammaCurveValue0045 = pow( i / 256.0, GammaValue0045 ) * 256.0;
					//else
					//	GammaCurveValue0045 = i * slope;
					
					GammaCurveValue0067 = pow( i / 256.0, GammaValue0067 ) * 256.0;
					GammaCurveValue0100 = pow( i / 256.0, GammaValue0100 ) * 256.0;
					GammaCurveValue0150 = pow( i / 256.0, GammaValue0150 ) * 256.0;
					GammaCurveValue0222 = pow( i / 256.0, GammaValue0222 ) * 256.0;
					GammaCurveValue0250 = pow( i / 256.0, GammaValue0250 ) * 256.0;
					GammaCurveValue0500 = pow( i / 256.0, GammaValue0500 ) * 256.0;
					GammaCurveValue1000 = pow( i / 256.0, GammaValue1000 ) * 256.0;
					GammaCurveValue2500 = pow( i / 256.0, GammaValue2500 ) * 256.0;
					
				//	fprintf( GammaCurveData, "%f %f\n", i / 256.0, GammaCurveValue0045 );
					fprintf( GammaCurveData, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f\n", i / 256.0
								, GammaCurveValue0004
								, GammaCurveValue0010
								, GammaCurveValue0020
								, GammaCurveValue0040
								, GammaCurveValue0045
								, GammaCurveValue0067
								, GammaCurveValue0100
								, GammaCurveValue0150
								, GammaCurveValue0222
								, GammaCurveValue0250
								, GammaCurveValue0500
								, GammaCurveValue1000
								, GammaCurveValue2500
								
						);                
					
				}                                 
				                                  
				fclose( GammaCurveData );
				
				char PresentWorkingDirectory[ 150 ];
				
				sprintf( PresentWorkingDirectory, "%s", ExtractFileDir( FileListBox1 -> FileName ) );
				
				FILE *fp_domain_script = fopen( "histogram.dem", "w" );
				fprintf( fp_domain_script, "cd '%s'\n", PresentWorkingDirectory );
				
				fprintf( fp_domain_script, "set terminal emf\n" );
				fprintf( fp_domain_script, "set xrange [ 0 : 1 ]\n" );
				
				fprintf( fp_domain_script, "set xlabel 'Input Gray Level'\n" );
				
				fprintf( fp_domain_script, "set ylabel 'Output Gray Level'\n" );
				
				fprintf( fp_domain_script, "set grid\n" );
				fprintf( fp_domain_script, "set output '%s'\n", GammaCurveFileName );
			//	fprintf( fp_domain_script, "plot 'GammaCurveData.dat' title '%s' with lines 1 4\n", "GammaCurveData.dat" );
				fprintf( fp_domain_script, "plot 'GammaCurveData.dat' using 1 :  2 notitle with lines 1 4\
								,'GammaCurveData.dat' using 1 :  3 notitle with lines 1 4\
								,'GammaCurveData.dat' using 1 :  4 notitle with lines 1 4\
								,'GammaCurveData.dat' using 1 :  5 notitle with lines 1 4\
								,'GammaCurveData.dat' using 1 :  6 notitle with lines 1 4\
								,'GammaCurveData.dat' using 1 :  7 notitle with lines 1 4\
								,'GammaCurveData.dat' using 1 :  8 notitle with lines 1 4\
								,'GammaCurveData.dat' using 1 :  9 notitle with lines 1 4\
								,'GammaCurveData.dat' using 1 : 10 notitle with lines 1 4\
								,'GammaCurveData.dat' using 1 : 11 notitle with lines 1 4\
								,'GammaCurveData.dat' using 1 : 12 notitle with lines 1 4\
								,'GammaCurveData.dat' using 1 : 13 notitle with lines 1 4\
								,'GammaCurveData.dat' using 1 : 14 notitle with lines 1 4\n"
					);
				
				fclose( fp_domain_script );
				
				char command[ 200 ];
				sprintf( command, "d:\\gnuplot\\bin\\wgnuplot.exe histogram.dem" );
				
				system( command );
				
				remove( "histogram.dem" );
				remove( "GammaCurveData.dat" );
				
			}
			
			Image4 -> Picture -> LoadFromFile( GammaCurveFileName );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
			
		}
	}
}
//---------------------------------------------------------------------------
