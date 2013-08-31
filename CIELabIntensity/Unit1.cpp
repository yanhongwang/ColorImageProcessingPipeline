//---------------------------------------------------------------------------

// F. Gasparini, R. Schettini, "Color Correction for Digital Photographs",
// IEEE Proc. of the 12th International Conference on Image Analysis and Processing
// (ICIAP¡¦03),pp. 646-651, 2003

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

#define	imageWidth	2048
#define	imageHeight	1536

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



__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
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
		case 2:
		{
			
			char histogram_file[ 100 ];
			char ec_file[ 100 ];
			char PresentWorkingDirectory[ 100 ];
			
			strcpy( histogram_file, FileListBox1 -> FileName.c_str() );
			strcpy( ec_file, FileListBox1 -> FileName.c_str() );
			
			sprintf( PresentWorkingDirectory, "%s", ExtractFileDir( FileListBox1 -> FileName ) );
			
			if( TabControl1 -> TabIndex == 1 )
			{
				
				sprintf( strrchr( histogram_file, '.' ), "_histogram.emf" );

				
				if( FileExists( histogram_file ) )
				{
					
					Image4 -> Picture -> LoadFromFile( histogram_file );
					TabControl1->Width=Image4->Picture->Width;
					TabControl1->Height=Image4->Picture->Height;
					
					break;
				}
			}
			else	// TabControl1 -> TabIndex == 2
			{
				
				sprintf( strrchr( ec_file, '.' ), "_ec.emf" );
				
				if( FileExists( ec_file ) )
				{
					Image4 -> Picture -> LoadFromFile( ec_file );
					TabControl1->Width=Image4->Picture->Width;
					TabControl1->Height=Image4->Picture->Height;
					
					break;
				}
			}
			
			FILE *fp_domain_data;
			FILE *fp_domain_script;
			
			int abDomain[ 185 ][ 203 ];
			
			unsigned int a_index;
			unsigned int b_index;
			
			double aSum = 0;
			double bSum = 0;
			
			double aMean;
			double bMean;
			double offset;
			
			double aVariance;
			double bVariance;
			double radius;
			
			double intensity;
			
			memset( abDomain, 0, sizeof( abDomain ) );
			
			ReadBMPData( FileListBox1 -> FileName.c_str() );
			
			for( int i = 0; i < imageHeight; i++ )
				for( int j = 0; j < imageWidth * 3; j += 3 )
				{
					
					RGB2Lab( ImageDataBlock[ i ][ j + 0 ], ImageDataBlock[ i ][ j + 1 ], ImageDataBlock[ i ][ j + 2 ],
							&Lab_buf[ i ][ j + 0 ], &Lab_buf[ i ][ j + 1 ], &Lab_buf[ i ][ j + 2 ] );
					
					aSum += Lab_buf[ i ][ j + 1 ];
					bSum += Lab_buf[ i ][ j + 2 ];
					
					a_index = ( int )( Lab_buf[ i ][ j + 1 ] + 0.5 ) + 86;
					b_index = ( int )( Lab_buf[ i ][ j + 2 ] + 0.5 ) + 108;
					
					abDomain[ a_index ][ b_index ]++;
					
				}
			
			fp_domain_data = fopen( "histogram.dat", "w" );
			
			// write ab domain value in data file
			for( int i = 0; i <= 184; i ++ )
				for( int j = 0; j <= 202; j ++ )
					if( abDomain[ i ][ j ] != 0 )
						fprintf( fp_domain_data, "%d %d %d\n", i - 86, j - 108, abDomain[ i ][ j ] );
			
			fclose( fp_domain_data );
			
			fp_domain_script = fopen( "histogram.dem", "w" );
			fprintf( fp_domain_script, "cd '%s'\n", PresentWorkingDirectory );
			
			fprintf( fp_domain_script, "set terminal emf color\n" );
			fprintf( fp_domain_script, "set xrange [ -86 : 98 ]\n" );
			fprintf( fp_domain_script, "set yrange [ -108 : 94 ]\n" );
			fprintf( fp_domain_script, "set xlabel 'a'\n" );
			fprintf( fp_domain_script, "set ylabel 'b'\n" );
			fprintf( fp_domain_script, "set grid\n" );
			
			if( TabControl1 -> TabIndex == 1 )
			{
				
				fprintf( fp_domain_script, "set zlabel 'intensity'\n\n" );
				
				fprintf( fp_domain_script, "set title '2D histogram in the ab plane'\n" );
				fprintf( fp_domain_script, "set ticslevel 0\n" );
				
				fprintf( fp_domain_script, "set output '%s'\n", histogram_file );
				
				fprintf( fp_domain_script, "splot 'histogram.dat' title '%s' with lines\n\n", histogram_file );

			}
			else	// TabControl1 -> TabIndex == 2
			{
				aMean = aSum / ( imageHeight * imageWidth );
				bMean = bSum / ( imageHeight * imageWidth );				
				
				offset = pow( aMean * aMean + bMean * bMean, 0.5 );
				
				aSum = 0;
				bSum = 0;
				
				for( int i = 0; i <= 184; i ++ )
					for( int j = 0; j <= 202; j ++ )
						if( abDomain[ i ][ j ] != 0 )
						{
							aSum += pow( aMean - ( i -  86 ), 2 ) * abDomain[ i ][ j ];
							bSum += pow( bMean - ( j - 108 ), 2 ) * abDomain[ i ][ j ];
						}
				
				aVariance = aSum / ( imageHeight * imageWidth );
				bVariance = bSum / ( imageHeight * imageWidth );
				
				radius = pow( aVariance + bVariance, 0.5 );
				
				intensity = ( offset - radius ) / radius;
				
				fprintf( fp_domain_script, "set parametric\n" );
				fprintf( fp_domain_script, "set output '%s'\n", ec_file );
				
				fprintf( fp_domain_script, "set arrow to %f, %f\n", aMean, bMean );
				fprintf( fp_domain_script, "set title 'Equivalent circle'\n" );
				
				if( aMean >= 0 && bMean >= 0 )		// first quadrant
				{
					fprintf( fp_domain_script, "set label 'offset = %f' at -10, -10\n", offset );
					fprintf( fp_domain_script, "set label 'radius = %f' at -10, -20\n", radius );
					fprintf( fp_domain_script, "set label 'intensity = %f' at -10, -30\n", intensity );
				}
				else if( aMean <= 0 && bMean >= 0 )	// second quadrant
				{
					fprintf( fp_domain_script, "set label 'offset = %f' at 10, -10\n", offset );
					fprintf( fp_domain_script, "set label 'radius = %f' at 10, -20\n", radius );
					fprintf( fp_domain_script, "set label 'intensity = %f' at 10, -30\n", intensity );
				}
				else if( aMean <= 0 && bMean <= 0 )	// third quadrant
				{
					fprintf( fp_domain_script, "set label 'offset = %f' at 10, 30\n", offset );
					fprintf( fp_domain_script, "set label 'radius = %f' at 10, 20\n", radius );
					fprintf( fp_domain_script, "set label 'intensity = %f' at 10, 10\n", intensity );
				}
				else					// fourth quadrant
				{
					fprintf( fp_domain_script, "set label 'offset = %f' at -10, 30\n", offset );
					fprintf( fp_domain_script, "set label 'radius = %f' at -10, 20\n", radius );
					fprintf( fp_domain_script, "set label 'intensity = %f' at -10, 10\n", intensity );
				}
				
				fprintf( fp_domain_script, "plot [ 0 : 2 * pi ] %f * sin( t ) + %f, %f * cos( t ) + %f title '%s'\n", radius, aMean, radius, bMean, ec_file );

			}
			
			fclose( fp_domain_data );
			
			char command[ 200 ];
			sprintf( command, "%s\\gnuplot\\bin\\wgnuplot.exe %s\\histogram.dem", ExtractFileDrive( FileListBox1 -> FileName ), PresentWorkingDirectory );
			system( command );
			
			if( TabControl1 -> TabIndex == 1 )
				Image4 -> Picture -> LoadFromFile( histogram_file );
			else	// TabControl1 -> TabIndex == 2
				Image4 -> Picture -> LoadFromFile( ec_file );
			
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			remove( "histogram.dat" );
			remove( "histogram.dem" );
			
			break;
		}
		
	}
}
//---------------------------------------------------------------------------


