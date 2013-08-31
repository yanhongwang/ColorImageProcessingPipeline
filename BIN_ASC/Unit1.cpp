//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "bmp.h"

unsigned char **ImageDataBlock;		// RGB pixel storage

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
	
	ResolutionGroup -> ItemIndex = 0;
	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BIN2ALLASCClick(TObject *Sender)
{
	
	FILE *BinaryFile;
	FILE *DataFile;
	
	BitMapFileHeader header1;
	
	BitMapInfoHeader header2;
	
	unsigned char DataBlock;		//RGB buffer
	
	char FileName[ 150 ];
	
	int TotalMemory;
	
	for( int m = 0; m < FileListBox1 -> Items -> Count; m ++ )
	{
		
		sprintf( FileName, "%s%s", ExtractFilePath( FileListBox1 -> FileName ), FileListBox1 -> Items -> Strings[ m ].c_str() );
		
		BinaryFile = fopen( FileName, "rb" );
		
		fread( &header1, 14, 1, BinaryFile );
		
		// read BitMapInfoHeader
		fread( &header2, 40, 1, BinaryFile );
		
		// calculate whole file size
		TotalMemory = 14 + 40 + header2.biWidth * header2.biHeight * 3;
		
		fclose( BinaryFile );
		
		BinaryFile = fopen( FileName, "rb" );
		
		sprintf( strrchr( FileName, '.' ), ".dat" );	// result image file name
		
		DataFile = fopen( FileName, "w" );
		
		for( int i = 0; i < TotalMemory; i ++ )
		{
			
			fread( &DataBlock, 1, 1, BinaryFile );
		
			if( i % 16 == 0 )
				fprintf( DataFile, "\n" );
				
			fprintf( DataFile, "%02X ", DataBlock );
			
		}
		
		fclose( BinaryFile );
		
		fclose( DataFile );
		
	}
	
}
//---------------------------------------------------------------------------
void __fastcall TForm1::BIN2RGBASCClick(TObject *Sender)
{
	
	FILE *BinaryFile;
	
	FILE *DataFileR;
	FILE *DataFileG;
	FILE *DataFileB;
	
	unsigned char DataBlock[ 3 ];		//RGB buffer
	
	int RTotal = 0;
	
	int GTotal = 0;
	
	int BTotal = 0;
	
	char FileName[ 150 ];
	
	char FileNameR[ 150 ];
	
	char FileNameG[ 150 ];
	
	char FileNameB[ 150 ];
	
	char ReportMessage[ 150 ];
	
	BitMapFileHeader header1;
	
	BitMapInfoHeader header2;
	
	int imageWidth;
	
	int imageHeight;
	
	for( int m = 0; m < FileListBox1 -> Items -> Count; m ++, RTotal = 0, GTotal = 0, BTotal = 0 )
	{
		
		sprintf( FileName, "%s%s", ExtractFilePath( FileListBox1 -> FileName ), FileListBox1 -> Items -> Strings[ m ].c_str() );
		
		BinaryFile = fopen( FileName, "rb" );
		
		fread( &header1, 14, 1, BinaryFile );
		
		// read BitMapInfoHeader
		fread( &header2, 40, 1, BinaryFile );
		
		imageWidth = header2.biWidth;
		
		imageHeight = header2.biHeight;
		
		strcpy( FileNameR, FileName );
		
		strcpy( FileNameG, FileName );
		
		strcpy( FileNameB, FileName );
		
		sprintf( strrchr( FileNameR, '.' ), "R.dat" );	// R image data file name
		
		sprintf( strrchr( FileNameG, '.' ), "G.dat" );	// G image data file name
		
		sprintf( strrchr( FileNameB, '.' ), "B.dat" );	// B image data file name
		
		DataFileR = fopen( FileNameR, "w" );
		
		DataFileG = fopen( FileNameG, "w" );
		
		DataFileB = fopen( FileNameB, "w" );
		
		for( int i = 0; i < imageWidth * imageHeight; i ++ )
		{
			
			fread( DataBlock, 3, 1, BinaryFile );
			
			if( i % 16 == 0 )
			{
				
				fprintf( DataFileB, "\n" );
				fprintf( DataFileG, "\n" );
				fprintf( DataFileR, "\n" );
				
			}
			
			fprintf( DataFileB, "%02X ", DataBlock[ 0 ] ); BTotal += DataBlock[ 0 ];
			fprintf( DataFileG, "%02X ", DataBlock[ 1 ] ); GTotal += DataBlock[ 1 ];
			fprintf( DataFileR, "%02X ", DataBlock[ 2 ] ); RTotal += DataBlock[ 2 ];
			
		}
		
		fclose( BinaryFile );
		
		fclose( DataFileR );
		fclose( DataFileG );
		fclose( DataFileB );
		
		sprintf( ReportMessage, "R Total = %d\nG Total = %d\nB Total = %d", RTotal, GTotal, BTotal );
		
		ShowMessage( ReportMessage );
		
	}
	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RGBASC2BINClick(TObject *Sender)
{
	
	if( FileListBox2 -> Items -> Count < 3 )
	{
		
		ShowMessage( "Some files are lacked" );
		return;
		
	}
	
	int	imageWidth;
	int	imageHeight;
	
	BitMapFileHeader header1;				// bmp format header1
	BitMapInfoHeader header2;				// bmp format header2
	
	switch( ResolutionGroup -> ItemIndex )
	{
		
		// 2048 x 1536
		case 0:
		{
			
			imageWidth = 2048;
			imageHeight = 1536;
			
			header1.bfType = 0x4D42;		// 2 bytes
			header1.bfSize = 0x00900036;		// 4 bytes
			header1.bfReserved1 = 0x0000;		// 2 bytes
			header1.bfReserved2 = 0x0000;		// 2 bytes
			header1.bfOffBits = 0x00000036;		// 4 bytes
			
			// 40 bytes
			header2.biSize = 0x00000028;		// 4 bytes
			header2.biWidth = 0x00000800;		// 4 bytes
			header2.biHeight = 0x00000600;		// 4 bytes
			header2.biPlanes = 0x0001;		// 2 bytes
			header2.biBitCount = 0x0018;		// 2 bytes
			header2.biCompression = 0x00000000;	// 4 bytes
			header2.biSizeImage = 0x00000000;	// 4 bytes
			header2.biXPelsPerMeter = 0x00000EC4;	// 4 bytes
			header2.biYpelsPerMeter = 0x00000EC4;	// 4 bytes
			header2.biClrUsed = 0x00000000;		// 4 bytes
			header2.biClrImportant = 0x00000000;	// 4 bytes
			
			break;
			
		}
		
		// 320 x 240
		case 1:
		{
			
			imageWidth = 320;
			imageHeight = 240;
			
			header1.bfType = 0x4D42;		// 2 bytes
			header1.bfSize = 0x00038436;		// 4 bytes
			header1.bfReserved1 = 0x0000;		// 2 bytes
			header1.bfReserved2 = 0x0000;		// 2 bytes
			header1.bfOffBits = 0x00000036;		// 4 bytes
			
			// 40 bytes
			header2.biSize = 0x00000028;		// 4 bytes
			header2.biWidth = 0x00000140;		// 4 bytes
			header2.biHeight = 0x000000F0;		// 4 bytes
			header2.biPlanes = 0x0001;		// 2 bytes
			header2.biBitCount = 0x0018;		// 2 bytes
			header2.biCompression = 0x00000000;	// 4 bytes
			header2.biSizeImage = 0x00038400;	// 4 bytes
			header2.biXPelsPerMeter = 0x00000EC4;	// 4 bytes
			header2.biYpelsPerMeter = 0x00000EC4;	// 4 bytes
			header2.biClrUsed = 0x00000000;		// 4 bytes
			header2.biClrImportant = 0x00000000;	// 4 bytes
			
			break;
			
		}
		
		// 20 x 15
		case 2:
		{
			
			imageWidth = 20;
			imageHeight = 15;
			
			header1.bfType = 0x4D42;		// 2 bytes
			header1.bfSize = 0x000003BA;		// 4 bytes
			header1.bfReserved1 = 0x0000;		// 2 bytes
			header1.bfReserved2 = 0x0000;		// 2 bytes
			header1.bfOffBits = 0x00000036;		// 4 bytes
			
			// 40 bytes
			header2.biSize = 0x00000028;		// 4 bytes
			header2.biWidth = 0x00000014;		// 4 bytes
			header2.biHeight = 0x0000000F;		// 4 bytes
			header2.biPlanes = 0x0001;		// 2 bytes
			header2.biBitCount = 0x0018;		// 2 bytes
			header2.biCompression = 0x00000000;	// 4 bytes
			header2.biSizeImage = 0x00000384;	// 4 bytes
			header2.biXPelsPerMeter = 0x00000EC4;	// 4 bytes
			header2.biYpelsPerMeter = 0x00000EC4;	// 4 bytes
			header2.biClrUsed = 0x00000000;		// 4 bytes
			header2.biClrImportant = 0x00000000;	// 4 bytes
			
			break;
			
		}
		
	}
	
	ImageDataBlock = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	
	for ( int i = 0 ; i < imageHeight ; ++ i )
		ImageDataBlock[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );
	
	char FileName[ 150 ];
	
	char FileNameR[ 150 ];
	
	char FileNameG[ 150 ];
	
	char FileNameB[ 150 ];
	
	for( int i = 0; i < FileListBox2 -> Items -> Count; i += 3 )
	{
		
		sprintf( FileNameB, "%s%s", ExtractFilePath( FileListBox2 -> FileName ), FileListBox2 -> Items -> Strings[ i + 0 ].c_str() );
		sprintf( FileNameG, "%s%s", ExtractFilePath( FileListBox2 -> FileName ), FileListBox2 -> Items -> Strings[ i + 1 ].c_str() );
		sprintf( FileNameR, "%s%s", ExtractFilePath( FileListBox2 -> FileName ), FileListBox2 -> Items -> Strings[ i + 2 ].c_str() );
		
		strcpy( FileName, FileNameB );
		sprintf( strrchr( FileName, '.' ) - 1, ".bmp" );	// result image file name
		
		// These three channel cannot be merge into one block, I don't know why
		// ------------------------------------------
		unsigned char B;
		
		FILE *DataFileB = fopen( FileNameB, "r" );
		
		for( int i = 0; i < imageHeight; i ++ )
			for( int j = 0; j < imageWidth * 3; j += 3 )
			{
				
				fscanf( DataFileB, "%X", &B );
				ImageDataBlock[ i ][ j + 0 ] = B;
				
			}
		
		fclose( DataFileB );
		
		// ------------------------------------------
		unsigned char G;
		
		FILE *DataFileG = fopen( FileNameG, "r" );
		
		for( int i = 0; i < imageHeight; i ++ )
			for( int j = 0; j < imageWidth * 3; j += 3 )
			{
				
				fscanf( DataFileG, "%X", &G );
				ImageDataBlock[ i ][ j + 1 ] = G;
				
			}
		
		fclose( DataFileG );
		
		// ------------------------------------------
		unsigned char R;
		
		FILE *DataFileR = fopen( FileNameR, "r" );
		
		for( int i = 0; i < imageHeight; i ++ )
			for( int j = 0; j < imageWidth * 3; j += 3 )
			{
				
				fscanf( DataFileR, "%X", &R );
				ImageDataBlock[ i ][ j + 2 ] = R;
				
			}
		
		fclose( DataFileR );
		
		// -----------------------------------------
		
		FILE *BinaryFile = fopen( FileName, "wb");
		
		fwrite( &header1.bfType, 2, 1, BinaryFile );
		fwrite( &header1.bfSize, 4, 1, BinaryFile );
		fwrite( &header1.bfReserved1, 2, 1, BinaryFile );
		fwrite( &header1.bfReserved2, 2, 1, BinaryFile );
		fwrite( &header1.bfOffBits, 4, 1, BinaryFile );
		
		fwrite( &header2, 40, 1, BinaryFile );
		
		for( int i = 0; i < imageHeight; i ++ )
			for( int j = 0; j < imageWidth * 3; j ++ )
				fwrite( &ImageDataBlock[ i ][ j ], 1, 1, BinaryFile );
		
		fclose( BinaryFile );
		
	}
	
	for( int i = 0; i < imageHeight; i ++ )
		free( ImageDataBlock[ i ] );
	
	free( ImageDataBlock );
	
}
//---------------------------------------------------------------------------
