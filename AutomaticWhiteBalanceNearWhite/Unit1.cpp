//---------------------------------------------------------------------------

// A Novel Automatic White Balance Method
// For Digital Still Cameras
// Ching-Chih Weng1, Homer Chen1,2, and Chiou-Shann Fuh3
// Department of Electrical Engineering, 2Graduate Institute of Communication Engineering
// Department of Computer Science and Information Engineering
// National Taiwan University, Taipei, Taiwan
// Email: b89901135@ntu.edu.tw, homer@cc.ee.ntu.edu.tw, fuh@csie.ntu.edu.tw

#include <vcl.h>
#pragma hdrstop

#include <math.h>

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

#define	NumberOfPixels	512 * 512

#define	NearWhiteRegionPercent	10.0 / 100.0

unsigned char **ImageDataBlock;

unsigned char ** YCbCrDataBlock;

struct NearWhiteStruct			// YCbCr
{
	
	unsigned char Y;
	unsigned char Cb;
	unsigned char Cr;
	
};

NearWhiteStruct * NearWhite;


BitMapFileHeader header1;
BitMapInfoHeader header2;

struct MacroBlock
{
	
	unsigned char Mb;	// mean of Cb
	unsigned char Mr;	// mean of Cr
	unsigned char Db;	// difference of Cb
	unsigned char Dr;	// difference of Cr
	
};

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
	// It will be assumed 512 bytes by the hardware, and then it has to minus 2 bytes.
	fread( &header1, 14, 1, fp );
	
	// read BitMapInfoHeader
	fread( &header2, 40, 1, fp );
	
	// according to width and height. Read Image Data Block in order to make RGB buffer
	
	for( int i = 0; i < imageHeight; i ++ )
		fread( ImageDataBlock[ i ], 3 * imageWidth, 1, fp );
		
	fclose( fp );
}

void SwapAddress( NearWhiteStruct * small, NearWhiteStruct * big )
{
	
	NearWhiteStruct temp;
	temp = *small;
	*small = *big;
	*big = temp;
	
}

void Sort( NearWhiteStruct NearWhite[], unsigned int len )
{
	
	// from big to small
	for( unsigned int i = 0; i < len - 1; i ++ )
		for( unsigned int j = i + 1; j < len; j ++ )
			if( NearWhite[ i ].Y < NearWhite[ j ].Y )
				SwapAddress( &NearWhite[ i ], &NearWhite[ j ] );
	
}

void MeanNearWhiteRegion
(
	
	NearWhiteStruct * NearWhite,
	int count,
	float ratio,
	unsigned char * FinalY,
	unsigned char * FinalCb,
	unsigned char * FinalCr
	
)
{
	
	float TotalY = 0;
	float TotalCb = 0;
	float TotalCr = 0;
	
	int n = count * ratio;
	
	for( int i = 0; i < n; i ++ )
	{
		
		TotalY += NearWhite[ i ].Y;
		TotalCb += NearWhite[ i ].Cb;
		TotalCr += NearWhite[ i ].Cr;
		
	}
	
	* FinalY = ( unsigned char )( TotalY / n );
	* FinalCb = ( unsigned char )( TotalCb / n );
	* FinalCr = ( unsigned char )( TotalCr / n );
	
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
	
	// allocate large memory in case of memory lacking problem
	NearWhite = ( NearWhiteStruct * )calloc( imageHeight * imageWidth * 3, sizeof( NearWhiteStruct ) );
	
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
			
			char NearWhiteFileName[ 150 ];
			
			strcpy( NearWhiteFileName, FileListBox1 -> FileName.c_str() );
			
			sprintf( strrchr( NearWhiteFileName, '.' ), "_nearwhite.bmp" );
			
			if( !FileExists( NearWhiteFileName ) )
			{
				
				MacroBlock block[ imageHeight / 512 ][ imageWidth / 512 ];
				
				ReadBMPData( FileListBox1 -> FileName.c_str() );
				
				int i;
				int j;
				int k;
				int m;
				
				double TotalCb = 0;
				double TotalCr = 0;
				
				double TotalCbDeviation = 0;
				double TotalCrDeviation = 0;
				
				double FinalMb = 0;
				double FinalMr = 0;
				double FinalDb = 0;
				double FinalDr = 0;
				
				//FILE * testFile1 = fopen( "test1.dat", "w" );
				for( i = 0; i < imageHeight / 512; i ++ )
					for( j = 0; j < imageWidth / 512; TotalCb = 0, TotalCr = 0, TotalCbDeviation = 0, TotalCrDeviation = 0, j ++ )
					{
						
						// calculate total CbCr of every block
						for( k = i * 512; k <= ( i * 512 + 511 ); k ++ )
							for( m = j * 512 * 3; m <= ( j * 512 + 511 ) * 3; m += 3 )
							{
								
								RGB2YCbCr( ImageDataBlock[ k ][ m + 0 ], ImageDataBlock[ k ][ m + 1 ], ImageDataBlock[ k ][ m + 2 ],
									&YCbCrDataBlock[ k ][ m + 0 ], &YCbCrDataBlock[ k ][ m + 1 ], &YCbCrDataBlock[ k ][ m + 2 ] );
								
								TotalCb += YCbCrDataBlock[ k ][ m + 1 ];		// Cb
								TotalCr += YCbCrDataBlock[ k ][ m + 2 ];		// Cr
								
							}
						
						// calculate mean CbCr of every block
						block[ i ][ j ].Mb = ( unsigned char )( TotalCb / NumberOfPixels );		// mean of Cb
						block[ i ][ j ].Mr = ( unsigned char )( TotalCr / NumberOfPixels );			// mean of Cr
						
						// calculate total of absolute differences
						for( k = i * 512; k <= ( i * 512 + 511 ); k ++ )
							for( m = j * 512 * 3; m <= ( j * 512 + 511 ) * 3; m += 3 )
							{
								//fprintf( testFile1, "%d\t%d\t%d\t%d\n", i, j, k, m );
								TotalCbDeviation += fabs( YCbCrDataBlock[ k ][ m + 1 ] - block[ i ][ j ].Mb );
								TotalCrDeviation += fabs( YCbCrDataBlock[ k ][ m + 2 ] - block[ i ][ j ].Mr );
								
							}
						
						// calculate average absolute differences Db and Dr
						block[ i ][ j ].Db = ( unsigned char )( TotalCbDeviation / NumberOfPixels );
						block[ i ][ j ].Dr = ( unsigned char )( TotalCrDeviation / NumberOfPixels );
						
						// calculate total of final Mb, Mr, Db and Dr
						FinalMb += block[ i ][ j ].Mb;
						FinalMr += block[ i ][ j ].Mr;
						FinalDb += block[ i ][ j ].Db;
						FinalDr += block[ i ][ j ].Dr;
						
					}
				//fclose( testFile1 );
				int amount = ( imageWidth / 512 ) * ( imageHeight / 512 );	// 4 * 3
				
				// calculate final Mb, Mr, Db and Dr
				FinalMb /= amount;
				FinalMr /= amount;
				FinalDb /= amount;
				FinalDr /= amount;
				
				int signMb;
				int signMr;
				
				if( FinalMb > 0 )
					signMb = 1;
				else
					signMb = -1;
				
				if( FinalMr > 0 )
					signMr = 1;
				else
					signMr = -1;
				
				double CbSubtrahend = FinalMb + FinalDb * signMb;
				double CrSubtrahend = 1.5 * FinalMr + FinalDr * signMr;
				
				unsigned int count = 0;
				
				unsigned char Ymax = 0;
				
				for( int i = 0; i < imageHeight; i ++ )
					for( int j = 0; j < imageWidth * 3; j += 3 )
					{
						
						if(
							( fabs( YCbCrDataBlock[ i ][ j + 1 ] - CbSubtrahend ) < 1.5 * FinalDb ) &&
							( fabs( YCbCrDataBlock[ i ][ j + 2 ] - CrSubtrahend ) < 1.5 * FinalDr )
						)
						{
							
							// fill out all near white point region
							NearWhite[ count ].Y = YCbCrDataBlock[ i ][ j + 0 ];		// Y
							NearWhite[ count ].Cb = YCbCrDataBlock[ i ][ j + 1 ];	// Cb
							NearWhite[ count ].Cr = YCbCrDataBlock[ i ][ j + 2 ];	// Cr
							
							count ++;
							
						}
						
						// find maximum Y
						if( Ymax < YCbCrDataBlock[ i ][ j + 0 ] )
							Ymax = YCbCrDataBlock[ i ][ j + 0 ];
						
					}
				
				//ShowMessage( count );
				
				// from big to small
				Sort( NearWhite, count );
				/*
				FILE * testFile2 = fopen( "test2.dat", "w" );
				for( unsigned int i = 0; i < count; i ++ )
				{
					
					unsigned char R;
					unsigned char G;
					unsigned char B;
					
					YCbCr2RGB( NearWhite[ i ].Y, NearWhite[ i ].Cb, NearWhite[ i ].Cr, &B, &G, &R );
					fprintf( testFile2, "%d\tY = %d\tCb = %d\tCr = %d\tR = %d\tG = %d\tB = %d\n",
					i, NearWhite[ i ].Y, NearWhite[ i ].Cb, NearWhite[ i ].Cr, R, G, B );
					
				}
				fclose( testFile2 );
				*/
				unsigned char FinalY = 0;
				unsigned char FinalCb = 0;
				unsigned char FinalCr = 0;
				
				MeanNearWhiteRegion( NearWhite, count, NearWhiteRegionPercent, &FinalY, &FinalCb, &FinalCr );
				
				unsigned char Raver;
				unsigned char Gaver;
				unsigned char Baver;
				
				YCbCr2RGB( FinalY, FinalCb, FinalCr, &Baver, &Gaver, &Raver );
				
				double Rgain = ( double )Ymax / ( double )Raver;
				
				double Ggain = ( double )Ymax / ( double )Gaver;
				
				double Bgain = ( double )Ymax / ( double )Baver;
				
				double tempValue;
				
				for( int i = 0; i < imageHeight; i++ )
					for( int j = 0; j < imageWidth * 3; j += 3 )
					{
						
						// channel B
						tempValue = ImageDataBlock[ i ][ j + 0 ] * Bgain;
						ImageDataBlock[ i ][ j + 0 ] = ( unsigned char )( tempValue < 0 ? 0 : tempValue >  255 ? 255 : tempValue );
						
						// channel G
						tempValue = ImageDataBlock[ i ][ j + 1 ] * Ggain;
						ImageDataBlock[ i ][ j + 1 ] = ( unsigned char )( tempValue < 0 ? 0 : tempValue >  255 ? 255 : tempValue );
						
						// channel R
						tempValue = ImageDataBlock[ i ][ j + 2 ] * Rgain;
						ImageDataBlock[ i ][ j + 2 ] = ( unsigned char )( tempValue < 0 ? 0 : tempValue >  255 ? 255 : tempValue );
						
					}
				
				FILE *fp = fopen( NearWhiteFileName, "wb" );
				
				fwrite( &header1, sizeof( BitMapFileHeader ) - 2, 1, fp );
				fwrite( &header2, sizeof( BitMapInfoHeader ), 1, fp );
				
				for( int i = 0; i < imageHeight; i ++ )
					fwrite( ImageDataBlock[ i ], 3 * imageWidth, 1, fp );
				
				fclose( fp );
				
			}
			
			Image4 -> Picture -> LoadFromFile( NearWhiteFileName );
			TabControl1 -> Tabs -> Strings[ TabControl1 -> TabIndex ] = ExtractFileName( NearWhiteFileName );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
			
		}

	}
}
//---------------------------------------------------------------------------
