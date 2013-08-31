//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "RGBCIELAB.h"
#include "bmp.h"
#include "edge.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------

#define	imageWidth	640
#define	imageHeight	480

unsigned char **ImageDataBlock;
double **Lab_buf;

unsigned char **edg_Bbuf;
unsigned char **edg_Rembuf;	//"Rem prefiltering buffer" is used to remember which pixels are edge and the value of edge is not 0

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

__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
	
	ImageDataBlock = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	Lab_buf = ( double ** )calloc( imageHeight, sizeof( double * ) );
	
	edg_Bbuf = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	edg_Rembuf = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	
	for ( int i = 0 ; i < imageHeight ; ++ i )
	{
		
		ImageDataBlock[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );
		Lab_buf[ i ] = ( double * )calloc( imageWidth * 3, sizeof( double ) );
		
		edg_Bbuf[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );
		edg_Rembuf[ i ] = ( unsigned char * )calloc( imageWidth, sizeof( unsigned char ) );
		
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
			
			int i, j, m, n, count;
			int intApre = 6;
			int intEdgeDet = 20;			
			
			char MeanWeight[ 150 ];
			
			sprintf( MeanWeight, "%s_MeanWeight_Lab.emf", ExtractFileName( ExtractFileDir( FileListBox1 -> FileName ) ) );
			
			if( !FileExists( MeanWeight ) )
			{
				
				FILE *testMeanData = fopen( "testMeanData.dat", "w" );
				
				char filename[ 150 ];
				
				for( n = 0; n < FileListBox1 -> Items -> Count; n ++ )
				{
					
					sprintf( filename, "%s%s", ExtractFilePath( FileListBox1 -> FileName ), FileListBox1 -> Items -> Strings[ n ].c_str() );
					
					ReadBMPData( filename );
					
					double aSum = 0;
					double bSum = 0;
					
					for( i = 0; i < imageHeight; i ++ )
						for( j = 0; j < imageWidth * 3; j += 3 )
							RGB2Lab( ImageDataBlock[ i ][ j + 0 ], ImageDataBlock[ i ][ j + 1 ], ImageDataBlock[ i ][ j + 2 ],
							&Lab_buf[ i ][ j + 0 ], &Lab_buf[ i ][ j + 1 ], &Lab_buf[ i ][ j + 2 ] );
					
					
					edge_prefiltering_Lab( imageHeight , imageWidth , Lab_buf , edg_Bbuf, intApre );
					edge_detection( imageHeight , imageWidth , edg_Bbuf , edg_Rembuf , intEdgeDet );
					
					count = 0;
					
					for( i = 0; i < imageHeight; i ++ )
						for( j = 0, m = 0; j <= imageWidth; j ++, m += 3 )
							if( edg_Rembuf[ i ][ j ] == 1 )
							{
								aSum += Lab_buf[ i ][ m + 1 ];
								bSum += Lab_buf[ i ][ m + 2 ];
								count ++;
							}
					
					if( count != 0 )
					{
						aSum /= count;
						bSum /= count;
					
						fprintf( testMeanData, "%f %f\n", aSum, bSum );
					}
					
				}
				
				fclose( testMeanData );
				
				char PresentWorkingDirectory[ 150 ];
				
				sprintf( PresentWorkingDirectory, "%s", ExtractFileDir( FileListBox1 -> FileName ) );
				
				FILE *fp_domain_script = fopen( "histogram.dem", "w" );
				fprintf( fp_domain_script, "cd '%s'\n", PresentWorkingDirectory );
				
				fprintf( fp_domain_script, "set terminal emf\n" );
				fprintf( fp_domain_script, "set xrange [ -86 : 98 ]\n" );
				fprintf( fp_domain_script, "set yrange [ -108 : 94 ]\n" );
				fprintf( fp_domain_script, "set xlabel 'a'\n" );
				fprintf( fp_domain_script, "set ylabel 'b'\n" );
				
				fprintf( fp_domain_script, "set title '%d photos weight histogram in the ab plane'\n", FileListBox1 -> Items -> Count );
				fprintf( fp_domain_script, "set grid\n" );
				
				fprintf( fp_domain_script, "set output '%s_MeanWeight_Lab.emf'\n", ExtractFileName( ExtractFileDir( FileListBox1 -> FileName ) ) );
				fprintf( fp_domain_script, "plot 'testMeanData.dat' with points 1 4\n" );
				
				fclose( fp_domain_script );
				
				char command[ 200 ];
				sprintf( command, "%s\\gnuplot\\bin\\wgnuplot.exe histogram.dem", ExtractFileDrive( FileListBox1 -> FileName ) );
				
				system( command );
				
				remove( "testMeanData.dat" );
				remove( "histogram.dem" );
				
			}
			
			Image4 -> Picture -> LoadFromFile( MeanWeight );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
		}
		
		case 2:
		{
			
			int i, j, m, n, count;
			int intApre = 6;
			int intEdgeDet = 20;			
			
			char MeanWeight[ 150 ];
			
			sprintf( MeanWeight, "%s_MeanWeight_rgb.emf", ExtractFileName( ExtractFileDir( FileListBox1 -> FileName ) ) );
			
			if( !FileExists( MeanWeight ) )
			{
				
				FILE *testMeanData = fopen( "testMeanData.dat", "w" );
				
				char filename[ 150 ];
				
				for( n = 0; n < FileListBox1 -> Items -> Count; n ++ )
				{
					
					sprintf( filename, "%s%s", ExtractFilePath( FileListBox1 -> FileName ), FileListBox1 -> Items -> Strings[ n ].c_str() );
					
					ReadBMPData( filename );
					
					double aSum = 0;
					double bSum = 0;
					
					edge_prefiltering_rgb( imageHeight , imageWidth , ImageDataBlock , edg_Bbuf, intApre );
					
					edge_detection( imageHeight , imageWidth , edg_Bbuf , edg_Rembuf , intEdgeDet );
					
					for( i = 0; i < imageHeight; i ++ )
						for( j = 0; j < imageWidth * 3; j += 3 )
							RGB2Lab( ImageDataBlock[ i ][ j + 0 ], ImageDataBlock[ i ][ j + 1 ], ImageDataBlock[ i ][ j + 2 ],
							&Lab_buf[ i ][ j + 0 ], &Lab_buf[ i ][ j + 1 ], &Lab_buf[ i ][ j + 2 ] );
					
					count = 0;
					
					for( i = 0; i < imageHeight; i ++ )
						for( j = 0, m = 0; j <= imageWidth; j ++, m += 3 )
							if( edg_Rembuf[ i ][ j ] == 1 )
							{
								aSum += Lab_buf[ i ][ m + 1 ];
								bSum += Lab_buf[ i ][ m + 2 ];
								count ++;
							}
					
					aSum /= count;
					bSum /= count;
					
					fprintf( testMeanData, "%f %f\n", aSum, bSum );
					
				}
				
				fclose( testMeanData );
				
				char PresentWorkingDirectory[ 150 ];
				
				sprintf( PresentWorkingDirectory, "%s", ExtractFileDir( FileListBox1 -> FileName ) );
				
				FILE *fp_domain_script = fopen( "histogram.dem", "w" );
				fprintf( fp_domain_script, "cd '%s'\n", PresentWorkingDirectory );
				
				fprintf( fp_domain_script, "set terminal emf\n" );
				fprintf( fp_domain_script, "set xrange [ -86 : 98 ]\n" );
				fprintf( fp_domain_script, "set yrange [ -108 : 94 ]\n" );
				fprintf( fp_domain_script, "set xlabel 'a'\n" );
				fprintf( fp_domain_script, "set ylabel 'b'\n" );
				
				fprintf( fp_domain_script, "set title '%d photos weight histogram in the ab plane'\n", FileListBox1 -> Items -> Count );
				fprintf( fp_domain_script, "set grid\n" );
				
				fprintf( fp_domain_script, "set output '%s_MeanWeight_rgb.emf'\n", ExtractFileName( ExtractFileDir( FileListBox1 -> FileName ) ) );
				fprintf( fp_domain_script, "plot 'testMeanData.dat' with points 1 4\n" );
				
				fclose( fp_domain_script );
				
				char command[ 200 ];
				sprintf( command, "%s\\gnuplot\\bin\\wgnuplot.exe histogram.dem", ExtractFileDrive( FileListBox1 -> FileName ) );
				
				system( command );
				
				remove( "histogram.dem" );
				remove( "testMeanData.dat");
				
			}
			
			Image4 -> Picture -> LoadFromFile( MeanWeight );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
		}
		
	}
}
//---------------------------------------------------------------------------
