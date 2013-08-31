//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "bmp.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------

#define	imageWidth	2048
#define	imageHeight	1536

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
		// original image data buffer
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
			
			char LargeBlockFileName[ 150 ];
			strcpy( LargeBlockFileName, FileListBox1 -> FileName.c_str() );
			
			sprintf( strrchr( LargeBlockFileName, '.' ), "_LargeBlock.bmp" );
			
			if( !FileExists( LargeBlockFileName ) )
			{
				
				ReadBMPData( FileListBox1 -> FileName.c_str() );
				
				double RightSumB = 0;
				double RightSumG = 0;
				double RightSumR = 0;
				
				double LeftSumB = 0;
				double LeftSumG = 0;
				double LeftSumR = 0;
				
				double deviation;
				
				// 0: not edge, 1: edge
				char EdgeFlag = 0;
				
				unsigned char ImageDataBlockRecovery[ 16 ][ 16 * 3 ];	// in case of some needed to recover block
				
			//	FILE *WeightEdgeFile = fopen( "WeightEdge.dat", "w" );
				
				for( int i = 0; i < imageHeight / 16; LeftSumB = 0, LeftSumG = 0, LeftSumR = 0, i ++ )
				{
					
					for( int j = 0; j < imageWidth / 16; RightSumB = 0, RightSumG = 0, RightSumR = 0, j ++ )
					{
						
						for( int k = i * 16; k <= ( i * 16 + 15 ); k ++ )
							for( int m = j * 16 * 3; m <= ( j * 16 + 15 ) * 3; m += 3 )
							{
								
								RightSumB += ImageDataBlock[ k ][ m + 0 ];
								RightSumG += ImageDataBlock[ k ][ m + 1 ];
								RightSumR += ImageDataBlock[ k ][ m + 2 ];
								
							}
						
						deviation = ( pow( RightSumR - LeftSumR, 2 ) + pow( RightSumG - LeftSumG, 2 ) + pow( RightSumB - LeftSumB, 2 ) ) 
						/ ( pow( ( RightSumR + LeftSumR ) / 2, 2 ) + pow( ( RightSumG + LeftSumG ) / 2, 2 ) + pow( ( RightSumB + LeftSumB ) / 2, 2 ) );
						
					//	fprintf( WeightEdgeFile, "%f ", deviation );
						
						if( j == 0 || deviation < 0.10 )	// not edge, space
						{
							
							for( int k = i * 16, p = 0; k <= ( i * 16 + 15 ); k ++, p ++ )
								for( int m = j * 16 * 3, q = 0; m <= ( j * 16 + 15 ) * 3; m += 3, q += 3 )
								{
									
									ImageDataBlockRecovery[ p ][ q + 0 ] = ImageDataBlock[ k ][ m + 0 ];
									ImageDataBlockRecovery[ p ][ q + 1 ] = ImageDataBlock[ k ][ m + 1 ];
									ImageDataBlockRecovery[ p ][ q + 2 ] = ImageDataBlock[ k ][ m + 2 ];
									
									ImageDataBlock[ k ][ m + 0 ] = 255;
									ImageDataBlock[ k ][ m + 1 ] = 255;
									ImageDataBlock[ k ][ m + 2 ] = 255;
									
								}
							
							EdgeFlag = 0;	// not edge, space
							
						}
						else	// edge == 1
						{
							
							if( 0 == EdgeFlag )	// save the last edge
							{
								
								for( int k = i * 16, p = 0; k <= ( i * 16 + 15 ); k ++, p ++ )
									for( int m = ( j - 1 ) * 16 * 3, q = 0; m <= ( ( j - 1 ) * 16 + 15 ) * 3; m += 3, q += 3 )
									{
										
										ImageDataBlock[ k ][ m + 0 ] = ImageDataBlockRecovery[ p ][ q + 0 ];
										ImageDataBlock[ k ][ m + 1 ] = ImageDataBlockRecovery[ p ][ q + 1 ];
										ImageDataBlock[ k ][ m + 2 ] = ImageDataBlockRecovery[ p ][ q + 2 ];
										
									}
								
							}
							
							EdgeFlag = 1;	// edge
							
						}
						
						LeftSumB = RightSumB;
						LeftSumG = RightSumG;
						LeftSumR = RightSumR;
						
					}
					
				//	fprintf( WeightEdgeFile, "\n" );
					
				}
				
			//	fclose( WeightEdgeFile );
				
				FILE *fp = fopen( LargeBlockFileName, "wb" );
				
				fwrite( &header1, 14, 1, fp );
				
				fwrite( &header2, 40, 1, fp );
				
				for( int i = 0; i < imageHeight; i ++ )
					for( int j = 0; j < imageWidth * 3; j ++ )
						fwrite( &ImageDataBlock[ i ][ j ], 1, 1, fp );
				
				fclose( fp );
				
			}
			
			Image4 -> Picture -> LoadFromFile( LargeBlockFileName );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			TabControl1 -> Tabs -> Strings[ TabControl1 -> TabIndex ] = LargeBlockFileName;
			
			break;
			
		}
		
	}
}
//---------------------------------------------------------------------------

