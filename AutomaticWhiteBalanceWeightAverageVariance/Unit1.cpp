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
	double DCa;
	double DCb;
	double WeightL;
	double Weighta;
	double Weightb;
	double WeightFinal;
};

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
			
			char MeanWeightFile[ 150 ];
			
			sprintf( MeanWeightFile, "%s_MeanWeight.emf", ExtractFileName( ExtractFileDir( FileListBox1 -> FileName ) ) );
			
			if( !FileExists( MeanWeightFile ) )
			{
				
				FILE *WeightData = fopen( "WeightData.dat", "w" );
				FILE *MeanData = fopen( "MeanData.dat", "w" );
				
				char filename[ 150 ];
				
				int i, j, k, m, n;
				
				MacroBlock block[ imageHeight / 16 ][ imageWidth / 16 ];
				
				for( n = 0; n < FileListBox1 -> Items -> Count; n ++ )
				{
					
					memset( block, 0, sizeof( block ) );
					
					sprintf( filename, "%s%s", ExtractFilePath( FileListBox1 -> FileName ), FileListBox1 -> Items -> Strings[ n ].c_str() );
					
					ReadBMPData( filename );
					
					double LSum = 0;
					double aSum = 0;
					double bSum = 0;
					
					double Totala = 0;
					double Totalb = 0;
					
					for( i = 0; i < imageHeight / 16; i ++ )
						for( j = 0; j < imageWidth / 16; LSum = 0, aSum = 0, bSum = 0, j ++ )
						{
							for( k = i * 16; k <= ( i * 16 + 15 ); k ++ )
								for( m = j * 16 * 3; m <= ( j * 16 + 15 ) * 3; m += 3 )
								{
									RGB2Lab( ImageDataBlock[ k ][ m + 0 ], ImageDataBlock[ k ][ m + 1 ], ImageDataBlock[ k ][ m + 2 ],
										&Lab_buf[ k ][ m + 0 ], &Lab_buf[ k ][ m + 1 ], &Lab_buf[ k ][ m + 2 ] );
									
									LSum += Lab_buf[ k ][ m + 0 ];
									aSum += Lab_buf[ k ][ m + 1 ];
									bSum += Lab_buf[ k ][ m + 2 ];
									
								}
							
							block[ i ][ j ].DCL = LSum / 256.0;
							block[ i ][ j ].DCa = aSum / 256.0;
							block[ i ][ j ].DCb = bSum / 256.0;
							
							Totala += aSum;
							Totalb += bSum;
							
						}
					
					double TotalMeana = Totala / ( imageHeight * imageWidth );
					double TotalMeanb = Totalb / ( imageHeight * imageWidth  );
					
					fprintf( MeanData, "%f %f\n", TotalMeana, TotalMeanb );
					
					double WeightAveragea = 0;
					double WeightAverageb = 0;
					
					double WeightTotalFinal = 0;
					
					for( i = 1; i < imageHeight / 16 - 1; i ++ )
						for( j = 1; j < imageWidth / 16 - 1; j ++ )
						{
							
							block[ i ][ j ].WeightL
								
								= pow( block[ i ][ j ].DCL - block[ i - 1 ][ j - 1 ].DCL, 2 )
								+ pow( block[ i ][ j ].DCL - block[ i - 1 ][ j     ].DCL, 2 )
								+ pow( block[ i ][ j ].DCL - block[ i - 1 ][ j + 1 ].DCL, 2 )
								
								+ pow( block[ i ][ j ].DCL - block[ i     ][ j - 1 ].DCL, 2 )
								+ pow( block[ i ][ j ].DCL - block[ i     ][ j + 1 ].DCL, 2 )
								
								+ pow( block[ i ][ j ].DCL - block[ i + 1 ][ j - 1 ].DCL, 2 )
								+ pow( block[ i ][ j ].DCL - block[ i + 1 ][ j     ].DCL, 2 )
								+ pow( block[ i ][ j ].DCL - block[ i + 1 ][ j + 1 ].DCL, 2 );
							
							block[ i ][ j ].Weighta                                            
								
								= pow( block[ i ][ j ].DCa - block[ i - 1 ][ j - 1 ].DCa, 2 )
								+ pow( block[ i ][ j ].DCa - block[ i - 1 ][ j     ].DCa, 2 )
								+ pow( block[ i ][ j ].DCa - block[ i - 1 ][ j + 1 ].DCa, 2 )
								
								+ pow( block[ i ][ j ].DCa - block[ i     ][ j - 1 ].DCa, 2 )
								+ pow( block[ i ][ j ].DCa - block[ i     ][ j + 1 ].DCa, 2 )
								
								+ pow( block[ i ][ j ].DCa - block[ i + 1 ][ j - 1 ].DCa, 2 )
								+ pow( block[ i ][ j ].DCa - block[ i + 1 ][ j     ].DCa, 2 )
								+ pow( block[ i ][ j ].DCa - block[ i + 1 ][ j + 1 ].DCa, 2 );
							
							block[ i ][ j ].Weightb                                            
								
								= pow( block[ i ][ j ].DCb - block[ i - 1 ][ j - 1 ].DCb, 2 )
								+ pow( block[ i ][ j ].DCb - block[ i - 1 ][ j     ].DCb, 2 )
								+ pow( block[ i ][ j ].DCb - block[ i - 1 ][ j + 1 ].DCb, 2 )
								
								+ pow( block[ i ][ j ].DCb - block[ i     ][ j - 1 ].DCb, 2 )
								+ pow( block[ i ][ j ].DCb - block[ i     ][ j + 1 ].DCb, 2 )
								
								+ pow( block[ i ][ j ].DCb - block[ i + 1 ][ j - 1 ].DCb, 2 )
								+ pow( block[ i ][ j ].DCb - block[ i + 1 ][ j     ].DCb, 2 )
								+ pow( block[ i ][ j ].DCb - block[ i + 1 ][ j + 1 ].DCb, 2 );
							
							block[ i ][ j ].WeightFinal = pow( block[ i ][ j ].WeightL * block[ i ][ j ].Weighta * block[ i ][ j ].Weightb, 0.5 );
							
							WeightTotalFinal += block[ i ][ j ].WeightFinal;
							
							WeightAveragea += block[ i ][ j ].DCa * block[ i ][ j ].WeightFinal;
							WeightAverageb += block[ i ][ j ].DCb * block[ i ][ j ].WeightFinal;
							
						}
						
					WeightAveragea /= WeightTotalFinal;
					WeightAverageb /= WeightTotalFinal;
					
					fprintf( WeightData, "%f %f\n", WeightAveragea, WeightAverageb );
					
					for( int i = 0; i < imageHeight; i ++ )
						for( int j = 0; j < imageWidth * 3; j += 3 )
						{
							
							Lab_buf[ i ][ j + 1 ] -= WeightAveragea;
							Lab_buf[ i ][ j + 2 ] -= WeightAverageb;
							
							Lab2RGB( Lab_buf[ i ][ j + 0 ], Lab_buf[ i ][ j + 1 ], Lab_buf[ i ][ j + 2 ],
									&ImageDataBlock[ i ][ j + 0 ], &ImageDataBlock[ i ][ j + 1 ], &ImageDataBlock[ i ][ j + 2 ] );
							
						}
					
					char CorrectFile[ 150 ];
					char *temp;
					temp = strrchr( filename, '.' );
					temp[ 0 ] = 0;
					
					sprintf( CorrectFile, "%s_correction.bmp", filename );
					
					FILE *TestImage = fopen( CorrectFile, "wb" );
					
					fwrite( &header1, sizeof( BitMapFileHeader ) - 2, 1, TestImage );
					fwrite( &header2, sizeof( BitMapInfoHeader ), 1, TestImage );
					
					for( int i = 0; i < imageHeight; i ++ )
						fwrite( ImageDataBlock[ i ], 3 * imageWidth, 1, TestImage );
					
					fclose( TestImage );
					
				}
				
				fclose( MeanData );
				fclose( WeightData );
				
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
				
				fprintf( fp_domain_script, "set output '%s_MeanWeight.emf'\n", ExtractFileName( ExtractFileDir( FileListBox1 -> FileName ) ) );
				fprintf( fp_domain_script, "plot 'MeanData.dat' title '%s' with points 1 4, 'WeightData.dat' title '%s' with points 3 7\n",
											"MeanData.dat", "WeightData.dat" );
				
				fclose( fp_domain_script );
				
				char command[ 200 ];
				sprintf( command, "%s\\gnuplot\\bin\\wgnuplot.exe histogram.dem", ExtractFileDrive( FileListBox1 -> FileName ) );
				
				system( command );
				
				remove( "histogram.dem" );
				remove( "WeightData.dat" );
				remove( "MeanData.dat" );
				
			}
			
			Image4 -> Picture -> LoadFromFile( MeanWeightFile );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
		}
		
	}
}
//---------------------------------------------------------------------------

