//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <math.h>

#include "Unit1.h"
#include "RGBCIELAB.h"
#include "bmp.h"
#include "AutoLevel.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------

#define	imageWidth	2048
#define	imageHeight	1536
#define	C		1
#define	LightGamma	0.45

unsigned char **ImageDataBlock;
double **Lab_buf;

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
	
	for ( int i = 0 ; i < imageHeight ; ++ i )
	{
		
		ImageDataBlock[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );
		Lab_buf[ i ] = ( double * )calloc( imageWidth * 3, sizeof( double ) );
		
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
			
			char RGBAutoLevelFileName[ 150 ];
			strcpy( RGBAutoLevelFileName, FileListBox1 -> FileName.c_str() );
			
			sprintf( strrchr( RGBAutoLevelFileName, '.' ), "_RGBAutoLevel.bmp" );
			
			if( !FileExists( RGBAutoLevelFileName ) )
			{
				
				ReadBMPData( FileListBox1 -> FileName.c_str() );
				
				int RLowThreshold = 0;
				int GLowThreshold = 0;
				int BLowThreshold = 0;
				
				int RHighThreshold = 255;
				int GHighThreshold = 255;
				int BHighThreshold = 255;
				
				RGBThreshold( ImageDataBlock, imageHeight, imageWidth, &RLowThreshold, &GLowThreshold, &BLowThreshold, &RHighThreshold, &GHighThreshold, &BHighThreshold );
				
				for( int i = 0; i < imageHeight; i ++ )
					for( int j = 0; j < imageWidth * 3; j += 3 )
					{
						
						// B element
						if( ( ImageDataBlock[ i ][ j + 0 ] > BLowThreshold ) && ( ImageDataBlock[ i ][ j + 0 ] < BHighThreshold ) )	
						{
							ImageDataBlock[ i ][ j + 0 ] = 256 * ( ImageDataBlock[ i ][ j + 0 ] - BLowThreshold ) / ( BHighThreshold - BLowThreshold );
							ImageDataBlock[ i ][ j + 0 ] = C * pow( ImageDataBlock[ i ][ j + 0 ] / 256.0, LightGamma ) * 256.0;
						}
						else if( ImageDataBlock[ i ][ j + 0 ] <= BLowThreshold )
							ImageDataBlock[ i ][ j + 0 ] = 0;
						else	// ImageDataBlock[ i ][ j + 0 ] > BHighThreshold
							ImageDataBlock[ i ][ j + 0 ] = 255;
						
						// G element
						if( ( ImageDataBlock[ i ][ j + 1 ] > GLowThreshold ) && ( ImageDataBlock[ i ][ j + 1 ] < GHighThreshold ) )	
						{
							ImageDataBlock[ i ][ j + 1 ] = 256 * ( ImageDataBlock[ i ][ j + 1 ] - GLowThreshold ) / ( GHighThreshold - GLowThreshold );
							ImageDataBlock[ i ][ j + 1 ] = C * pow( ImageDataBlock[ i ][ j + 1 ] / 256.0, LightGamma ) * 256.0;
						}
						else if( ImageDataBlock[ i ][ j + 1 ] <= GLowThreshold )
							ImageDataBlock[ i ][ j + 1 ] = 0;
						else	// ImageDataBlock[ i ][ j + 1 ] > GHighThreshold
							ImageDataBlock[ i ][ j + 1 ] = 255;
						
						// R element
						if( ( ImageDataBlock[ i ][ j + 2 ] > RLowThreshold ) && ( ImageDataBlock[ i ][ j + 2 ] < RHighThreshold ) )	
						{
							ImageDataBlock[ i ][ j + 2 ] = 256 * ( ImageDataBlock[ i ][ j + 2 ] - RLowThreshold ) / ( RHighThreshold - RLowThreshold );
							ImageDataBlock[ i ][ j + 2 ] = C * pow( ImageDataBlock[ i ][ j + 2 ] / 256.0, LightGamma ) * 256.0;
						}
						else if( ImageDataBlock[ i ][ j + 2 ] <= RLowThreshold )
							ImageDataBlock[ i ][ j + 2 ] = 0;
						else	// ImageDataBlock[ i ][ j + 2 ] > RHighThreshold
							ImageDataBlock[ i ][ j + 2 ] = 255;
						
					}
				
				FILE *fp = fopen( RGBAutoLevelFileName, "wb" );
				
				fwrite( &header1, sizeof( BitMapFileHeader ) - 2, 1, fp );
				fwrite( &header2, sizeof( BitMapInfoHeader ), 1, fp );
				
				for( int i = 0; i < imageHeight; i ++ )
					fwrite( ImageDataBlock[ i ], 3 * imageWidth, 1, fp );
				
				fclose( fp );
				
			}
			
			TabControl1 -> Tabs -> Strings[ TabControl1 -> TabIndex ] = ExtractFileName( RGBAutoLevelFileName );
			Image4 -> Picture -> LoadFromFile( RGBAutoLevelFileName );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
			
		}
		
		case 2:
		{
			
			char LABAutoLevelFileName[ 150 ];
			strcpy( LABAutoLevelFileName, FileListBox1 -> FileName.c_str() );
			
			sprintf( strrchr( LABAutoLevelFileName, '.' ), "_LABAutoLevel.bmp" );
			
			if( !FileExists( LABAutoLevelFileName ) )
			{
				
				ReadBMPData( FileListBox1 -> FileName.c_str() );
				
				for( int i = 0; i < imageHeight; i ++ )
					for( int j = 0; j < imageWidth * 3; j += 3 )
						RGB2Lab( ImageDataBlock[ i ][ j + 0 ], ImageDataBlock[ i ][ j + 1 ], ImageDataBlock[ i ][ j + 2 ],
							&Lab_buf[ i ][ j + 0 ], &Lab_buf[ i ][ j + 1 ], &Lab_buf[ i ][ j + 2 ] );
				
				int LLowThreshold = 0;
				int LHighThreshold = 100;
				
				LThreshold( Lab_buf, imageHeight, imageWidth, &LLowThreshold, &LHighThreshold );
				
				for( int i = 0; i < imageHeight; i ++ )
					for( int j = 0; j < imageWidth * 3; j += 3 )
					{
						
						if( ( Lab_buf[ i ][ j ] > LLowThreshold ) && ( Lab_buf[ i ][ j ] < LHighThreshold ) )	
							Lab_buf[ i ][ j ] = 100 * ( Lab_buf[ i ][ j ] - LLowThreshold ) / ( LHighThreshold - LLowThreshold );
						else if( Lab_buf[ i ][ j ] <= LLowThreshold )
							Lab_buf[ i ][ j ] = 0;
						else	// Lab_buf[ i ][ j ] > LHighThreshold
							Lab_buf[ i ][ j ] = 100;
						
						Lab2RGB( Lab_buf[ i ][ j + 0 ], Lab_buf[ i ][ j + 1 ], Lab_buf[ i ][ j + 2 ],
							&ImageDataBlock[ i ][ j + 0 ], &ImageDataBlock[ i ][ j + 1 ], &ImageDataBlock[ i ][ j + 2 ] );
						
					}
				
				FILE *fp = fopen( LABAutoLevelFileName, "wb" );
				
				fwrite( &header1, sizeof( BitMapFileHeader ) - 2, 1, fp );
				fwrite( &header2, sizeof( BitMapInfoHeader ), 1, fp );
				
				for( int i = 0; i < imageHeight; i ++ )
					fwrite( ImageDataBlock[ i ], 3 * imageWidth, 1, fp );
				
				fclose( fp );
				
			}
			
			TabControl1 -> Tabs -> Strings[ TabControl1 -> TabIndex ] = ExtractFileName( LABAutoLevelFileName );
			Image4 -> Picture -> LoadFromFile( LABAutoLevelFileName );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
			
		}
		
		case 3:
		case 4:
		case 5:
		{
			
			char RGBHistogramFileName[ 150 ];
			
			strcpy( RGBHistogramFileName, FileListBox1 -> FileName.c_str() );
			
			if( TabControl1 -> TabIndex == 3 )
				sprintf( strrchr( RGBHistogramFileName, '.' ), "_RHistogram.emf" );
			else if( TabControl1 -> TabIndex == 4 )
				sprintf( strrchr( RGBHistogramFileName, '.' ), "_GHistogram.emf" );
			else if( TabControl1 -> TabIndex == 5 )
				sprintf( strrchr( RGBHistogramFileName, '.' ), "_BHistogram.emf" );
			
			if ( FileExists( RGBHistogramFileName ) )
			{
				
				Image4 -> Picture -> LoadFromFile( RGBHistogramFileName );
				return;
				
			}
				
			ReadBMPData( FileListBox1 -> FileName.c_str() );
			
			int RHistogram[ 256 ];
			int GHistogram[ 256 ];
			int BHistogram[ 256 ];
			
			memset( RHistogram, 0, sizeof( RHistogram ) );
			memset( GHistogram, 0, sizeof( GHistogram ) );
			memset( BHistogram, 0, sizeof( BHistogram ) );
			
			for( int i = 0; i < imageHeight; i ++ )
				for( int j = 0; j < imageWidth * 3; j += 3 )
				{
					
					BHistogram[ ImageDataBlock[ i ][ j + 0 ] ] ++;
					GHistogram[ ImageDataBlock[ i ][ j + 1 ] ] ++;
					RHistogram[ ImageDataBlock[ i ][ j + 2 ] ] ++;
					
				}
			
			FILE *RGBHistogramData = fopen( "RGBHistogramData.dat", "w" );
			
			// write  R, G, or B domain value in data file
			if( TabControl1 -> TabIndex == 3 )
				for( int i = 0; i <= 255; i ++ )
					fprintf( RGBHistogramData, "%d %d\n", i, RHistogram[ i ] );
			else if( TabControl1 -> TabIndex == 4 )
				for( int i = 0; i <= 255; i ++ )
					fprintf( RGBHistogramData, "%d %d\n", i, GHistogram[ i ] );
			else if( TabControl1 -> TabIndex == 5 )
				for( int i = 0; i <= 255; i ++ )
					fprintf( RGBHistogramData, "%d %d\n", i, BHistogram[ i ] );
			
			fclose( RGBHistogramData );
			
			char PresentWorkingDirectory[ 150 ];
			
			sprintf( PresentWorkingDirectory, "%s", ExtractFileDir( FileListBox1 -> FileName ) );
			
			FILE *fp_domain_script = fopen( "histogram.dem", "w" );
			fprintf( fp_domain_script, "cd '%s'\n", PresentWorkingDirectory );
			
			fprintf( fp_domain_script, "set terminal emf\n" );
			fprintf( fp_domain_script, "set xrange [ 0 : 255 ]\n" );
			
			if( TabControl1 -> TabIndex == 3 )
				fprintf( fp_domain_script, "set xlabel 'R Level'\n" );
			else if( TabControl1 -> TabIndex == 4 )
				fprintf( fp_domain_script, "set xlabel 'G Level'\n" );
			else if( TabControl1 -> TabIndex == 5 )
				fprintf( fp_domain_script, "set xlabel 'B Level'\n" );
			
			fprintf( fp_domain_script, "set ylabel 'Intensity'\n" );
			
			fprintf( fp_domain_script, "set grid\n" );
			fprintf( fp_domain_script, "set output '%s'\n", RGBHistogramFileName );
			fprintf( fp_domain_script, "plot 'RGBHistogramData.dat' title '%s' with impulses 1 4\n", "RGBHistogramData.dat" );
			
			fclose( fp_domain_script );
			
			char command[ 200 ];
			sprintf( command, "%s\\gnuplot\\bin\\wgnuplot.exe histogram.dem", ExtractFileDrive( FileListBox1 -> FileName ) );
			
			system( command );
			
			remove( "histogram.dem" );
			remove( "RGBHistogramData.dat" );
			
			Image4 -> Picture -> LoadFromFile( RGBHistogramFileName );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
			
		}
		
	}
}
//---------------------------------------------------------------------------

