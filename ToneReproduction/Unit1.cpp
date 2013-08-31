//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "RGBCIELAB.h"
#include "bmp.h"
#include <math.h>

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

void swap_by_addr(double *v1, double *v2)
{
	double temp;
	temp = *v1;
	*v1 = *v2;
	*v2 = temp;
}

void exchange_sort(double A[], int len)
{
	
	for( int r = 0; r <= len - 2; r ++ )
		for( int s = r + 1; s <= len - 1; s ++ )
			if( A[ r ] > A[ s ] )
				swap_by_addr( &A[ r ], &A[ s ] );
	
}

#define	imageWidth	640
#define	imageHeight	480
#define	Len_pri		( ( imageHeight / 16 ) - 2 ) * ( ( imageWidth / 16 ) - 2 )

unsigned char **ImageDataBlock;
unsigned char **ucImageOut;		//image data block output, every pixel exists three colors ( RGB )
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
	
	ucImageOut = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	ImageDataBlock = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	Lab_buf = ( double ** )calloc( imageHeight, sizeof( double * ) );
	
	for ( int i = 0 ; i < imageHeight ; ++ i )
	{
		
		ucImageOut[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );
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
			
			char ToneOldFile[ 150 ];
			
			strcpy( ToneOldFile, FileListBox1 -> FileName.c_str() );
			
			sprintf( strrchr( ToneOldFile, '.' ), "_ToneOld.emf" );
			
			if( !FileExists( ToneOldFile ) )
			{
				
				char filename[ 150 ];
				
				int LDomain[ 100 ];
				
				int L_index;
				
				memset( LDomain, 0, sizeof( LDomain ) );
				
				strcpy( filename,  FileListBox1 -> FileName.c_str() );
				
				ReadBMPData( filename );
				
				for( int i = 0 ; i < imageHeight; i ++ )
					for( int j = 0; j < imageWidth * 3; j += 3 )
					{
						
						RGB2Lab( ImageDataBlock[ i ][ j + 0 ], ImageDataBlock[ i ][ j + 1 ], ImageDataBlock[ i ][ j + 2 ],
							&Lab_buf[ i ][ j + 0 ], &Lab_buf[ i ][ j + 1 ], &Lab_buf[ i ][ j + 2 ] );
						
						L_index = ( int )( Lab_buf[ i ][ j + 0 ] + 0.5 );
						
						LDomain[ L_index ] ++;
						
					}
				
				FILE *ToneOldData = fopen( "ToneOldData.dat", "w" );
				
				// write L domain value in data file
				for( int i = 0; i <= 99; i ++ )
					//if( LDomain[ i ] >= 300 )
						fprintf( ToneOldData, "%d %d\n", i, LDomain[ i ] );
					
				fclose( ToneOldData );
				
				char PresentWorkingDirectory[ 150 ];
				
				sprintf( PresentWorkingDirectory, "%s", ExtractFileDir( FileListBox1 -> FileName ) );
				
				FILE *fp_domain_script = fopen( "histogram.dem", "w" );
				fprintf( fp_domain_script, "cd '%s'\n", PresentWorkingDirectory );
				
				fprintf( fp_domain_script, "set terminal emf\n" );
				fprintf( fp_domain_script, "set xrange [ 0 : 100 ]\n" );
				fprintf( fp_domain_script, "set xlabel 'L'\n" );
				fprintf( fp_domain_script, "set ylabel 'intensity'\n" );
				
				fprintf( fp_domain_script, "set grid\n" );
				
				fprintf( fp_domain_script, "set output '%s'\n", ToneOldFile );
				
				fprintf( fp_domain_script, "plot 'ToneOldData.dat' title '%s' with impulses 1 4\n", "ToneOld.dat" );
				
				fclose( fp_domain_script );
				
				char command[ 200 ];
				sprintf( command, "%s\\gnuplot\\bin\\wgnuplot.exe histogram.dem", ExtractFileDrive( FileListBox1 -> FileName ) );
				
				system( command );
				
				remove( "histogram.dem" );
				remove( "ToneOldData.dat" );
				
			}
			
			Image4 -> Picture -> LoadFromFile( ToneOldFile );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
		}
		
		case 2:
		{
			
			char ToneOutPutFile[ 150 ];
			
			strcpy( ToneOutPutFile, FileListBox1 -> FileName.c_str() );
			
			sprintf( strrchr( ToneOutPutFile, '.' ), "_tone.bmp" );
			
			if( !FileExists( ToneOutPutFile ) )
			{
				
				char filename[ 150 ];
				
				strcpy( filename,  FileListBox1 -> FileName.c_str() );
				
				ReadBMPData( filename );
				
				MacroBlock block[ imageHeight / 16 ][ imageWidth / 16 ];
				
				memset( block, 0, sizeof( block ) );
				
				double LSum = 0;
				double aSum = 0;
				double bSum = 0;
				
				for( int i = 0; i < imageHeight / 16; i ++ )
					for( int j = 0; j < imageWidth / 16; LSum = 0, aSum = 0, bSum = 0, j ++ )
					{
						for( int k = i * 16; k <= ( i * 16 + 15 ); k ++ )
							for( int m = j * 16 * 3; m <= ( j * 16 + 15 ) * 3; m += 3 )
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
					
					}
				
				int num = 0;
				
				double L_sort[ Len_pri ];
				
				//  make Macroblock in one dimension
				for( int i = 1; i < imageHeight / 16 - 1; i ++ )
					for( int j = 1; j < imageWidth / 16 - 1; j ++, num ++ )
						L_sort[ num ] = block[ i ][ j ].DCL;
				
				// sort L from small to big
				exchange_sort( L_sort, Len_pri );
				
				double WeightTotalFinal = 0;
				
				for( int i = 1; i < imageHeight / 16 - 1; i ++ )
					for( int j = 1; j < imageWidth / 16 - 1; j ++ )
					{
						
						block[ i ][ j ].WeightL
						
							= pow( block[ i ][ j ].DCL - block[ i - 1 ][ j - 1 ].DCL, 2 )
							+ pow( block[ i ][ j ].DCL - block[ i - 1 ][ j      ].DCL, 2 )
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
					
					}
				
				double tone_L1;		// every L of control point ( 0 ~ 100 )
				double tone_L2;
				double tone_L3;
				double tone_L4;
				
				double tone_c1x = 10;	// MacroBlock DCL weight sum accumulation ratio
				double tone_c2x = 30;
				double tone_c3x = 70;
				double tone_c4x = 90;
				
				double tone_wL1 = 0;	// accumulate every DCL weight sum of control pointªºDCL weight sum, aim at final value
				double tone_wL2 = 0;
				double tone_wL3 = 0;
				double tone_wL4 = 0;
				
				// find out four ratios of WeightFinal, calculate corresponding DCL
				for( int p = 0; p < Len_pri; p ++ )
					for( int i = 1; i < imageHeight / 16 - 1; i ++ )
						for( int j = 1; j < imageWidth / 16 - 1; j ++ )
							if( block[ i ][ j ].DCL == L_sort[ p ] )
							{
								
								if( tone_wL1 < tone_c1x / 100 )
								{
									tone_wL1 +=  block[ i ][ j ].WeightFinal / WeightTotalFinal;
									if( tone_wL1 >= tone_c1x / 100 )
										tone_L1 = block[ i ][ j ].DCL;
									tone_wL2 += block[ i ][ j ].WeightFinal / WeightTotalFinal;
									tone_wL3 += block[ i ][ j ].WeightFinal / WeightTotalFinal;							
									tone_wL4 += block[ i ][ j ].WeightFinal / WeightTotalFinal;							
								}
								else if( tone_wL1 >= tone_c1x / 100 && tone_wL2 < tone_c2x / 100 )
								{
									tone_wL2 += block[ i ][ j ].WeightFinal / WeightTotalFinal;
									if( tone_wL2 >= tone_c2x / 100 )
										tone_L2 = block[ i ][ j ].DCL;
									tone_wL3 += block[ i ][ j ].WeightFinal / WeightTotalFinal;
									tone_wL4 += block[ i ][ j ].WeightFinal / WeightTotalFinal;							
								}
								else if( tone_wL2 >= tone_c2x / 100 && tone_wL3 < tone_c3x / 100 )
								{
									tone_wL3 += block[ i ][ j ].WeightFinal / WeightTotalFinal;
									if( tone_wL3 >= tone_c3x / 100 )
										tone_L3 = block[ i ][ j ].DCL;
									tone_wL4 += block[ i ][ j ].WeightFinal / WeightTotalFinal;							
								}
								else if( tone_wL3 >= tone_c3x / 100 && tone_wL4 < tone_c4x / 100 )
								{
									tone_wL4 += block[ i ][ j ].WeightFinal / WeightTotalFinal;							
									if( tone_wL4 >= tone_c4x / 100 )
										tone_L4 = block[ i ][ j ].DCL;
								}
								
							}
				
				double tone_c1y = 20;	// every L of control point should be adjust to value( 0 ~100 )
				double tone_c2y = 40;
				double tone_c3y = 60;
				double tone_c4y = 80;
				
				// Tone Reproduction on L domain
				for( int p = 0; p < imageHeight; p ++ )
					for( int q = 0; q < ( 3 * imageWidth ); q +=3 )
					{
						
						if( Lab_buf[ p ][ q ] >= 0 && Lab_buf[ p ][ q ] < tone_L1 )
							Lab_buf[ p ][ q ] = Lab_buf[ p ][ q ] * tone_c1y / tone_L1;
						else if( Lab_buf[ p ][ q ] >= tone_L1 && Lab_buf[ p ][ q ] < tone_L2 )
							Lab_buf[ p ][ q ] = tone_c1y + ( Lab_buf[ p ][ q ] - tone_L1 ) * ( tone_c2y - tone_c1y ) / ( tone_L2 - tone_L1 );
						else if( Lab_buf[ p ][ q ] >= tone_L2 && Lab_buf[ p ][ q ] < tone_L3)
							Lab_buf[ p ][ q ] =  tone_c2y + ( Lab_buf[ p ][ q ] - tone_L2 ) * ( tone_c3y - tone_c2y ) / ( tone_L3 - tone_L2 );
						else if( Lab_buf[ p ][ q ] >= tone_L3 && Lab_buf[ p ][ q ] < tone_L4 )
							Lab_buf[ p ][ q ] =  tone_c3y + ( Lab_buf[ p ][ q ] - tone_L3 ) * ( tone_c4y - tone_c3y ) / ( tone_L4 - tone_L3 );
						else
							Lab_buf[ p ][ q ] =  tone_c4y + ( Lab_buf[ p ][ q ] - tone_L4 ) * ( 100 - tone_c4y ) / ( 100 - tone_L4 );
						
						// Lab to RGB
						Lab2RGB( Lab_buf[ p ][ q + 0 ], Lab_buf[ p ][ q + 1 ], Lab_buf[ p ][ q + 2 ],
							&ucImageOut[ p ][ q + 0 ], &ucImageOut[ p ][ q + 1 ], &ucImageOut[ p ][ q + 2 ] );
						
					}
				
				FILE *outfp = fopen( ToneOutPutFile, "wb" );
				
				fwrite( &header1, 14, 1, outfp );
				fwrite( &header2, 40, 1, outfp );
				
				for( int i = 0; i < imageHeight; i ++ )
					fwrite( ucImageOut[ i ], 3 * imageWidth, 1, outfp );
				
				fclose( outfp );
				
			}
			
			Image4 -> Picture -> LoadFromFile( ToneOutPutFile );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
			
		}
	}
	
}
//---------------------------------------------------------------------------

