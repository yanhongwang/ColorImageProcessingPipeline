//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "bmp.h"
#include "Interpolation.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------

unsigned char **ImageDataBlock;
unsigned char **BayerPattern;
unsigned char **ucImageOut;//image data block output, every pixel has RGB colors

unsigned char **GPlane;//divide pattern into three parts
unsigned char **RPlane;
unsigned char **BPlane;

BitMapFileHeader header1;
BitMapInfoHeader header2;

__fastcall TForm1::TForm1(TComponent* Owner): TForm(Owner)
{
	
	ImageDataBlock = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	BayerPattern = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	ucImageOut = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	GPlane = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );//divide pattern into three parts
	RPlane = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	BPlane = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	
	for ( int i = 0 ; i < imageHeight ; ++ i )
	{
		
		ImageDataBlock[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );
		BayerPattern[ i ] = ( unsigned char * )calloc( imageWidth, sizeof( unsigned char ) );
		ucImageOut[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );
		GPlane[ i ] = ( unsigned char * )calloc( imageWidth, sizeof( unsigned char ) );
		RPlane[ i ] = ( unsigned char * )calloc( imageWidth, sizeof( unsigned char ) );
		BPlane[ i ] = ( unsigned char * )calloc( imageWidth, sizeof( unsigned char ) );
		
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
			
			char CFAPSNR_EMF[ 150 ];
			
			sprintf( CFAPSNR_EMF, "%s_CFAPSNR.emf", ExtractFileName( ExtractFileDir( FileListBox1 -> FileName ) ) );
			
			if( !FileExists( CFAPSNR_EMF ) )
			{
				
				char bmpin[ 150 ];
				
				double PSNRG = 0;
				double PSNRR = 0;
				double PSNRB = 0;
				
				FILE *CFAPSNRDataR = fopen( "CFAPSNRDataR.dat" , "w" );
				FILE *CFAPSNRDataG = fopen( "CFAPSNRDataG.dat" , "w" );
				FILE *CFAPSNRDataB = fopen( "CFAPSNRDataB.dat" , "w" );
				
				char InterpolationFileName[ 150 ];
				
				for( int n = 0; n < FileListBox1 -> Items -> Count; n ++ )
				{
					
					strcpy( InterpolationFileName, FileListBox1 -> Items -> Strings[ n ].c_str() );
					
					sprintf( strrchr( InterpolationFileName, '.' ), "_sobel.bmp" );
					
					strcpy( bmpin, FileListBox1 -> Items -> Strings[ n ].c_str() );
					
					sprintf( strrchr( bmpin, '.' ), ".bmp" );
					
					FILE *filep = fopen( bmpin, "rb" );
					
					fread( &header1, 14, 1, filep ); 
					fread( &header2, 40, 1, filep );
					
					for( int i = 0; i < imageHeight; i ++ )
						fread( ImageDataBlock[ i ], 3 * imageWidth, 1, filep );
					
					fclose( filep );
					
					FILE *fp = fopen( FileListBox1 -> Items -> Strings[ n ].c_str(), "rb" );
					
					for( int i = 0; i < imageHeight; i ++ )
						fread( BayerPattern[ i ], imageWidth, 1, fp );
					
					fclose( fp );
					
					Pattern2RGBPlanes( BayerPattern, GPlane, RPlane, BPlane);
					
					Interpolation_2003_Sobel( BayerPattern, GPlane, RPlane, BPlane );
					
					for( int i = 0; i < imageHeight; i ++ )
					{
						for( int j = 0; j < imageWidth; j ++ )
						{
							
							ucImageOut[ i ][ 3 * j + 0 ] = BPlane[ i ][ j ];
							ucImageOut[ i ][ 3 * j + 1 ] = GPlane[ i ][ j ];
							ucImageOut[ i ][ 3 * j + 2 ] = RPlane[ i ][ j ];
							
						}
					}
					
					fp = fopen( InterpolationFileName, "wb" );
					
					fwrite( &header1, 14, 1, fp );
					
					fwrite( &header2, 40, 1, fp );
					
					for( int i = 0; i < imageHeight; i ++ )
						fwrite( ucImageOut[ i ], 3 * imageWidth, 1, fp );
					
					fclose( fp );
					
					PSNR( ImageDataBlock, ucImageOut, &PSNRG, &PSNRR, &PSNRB );
					
					fprintf( CFAPSNRDataR, "%d %f\n", n, PSNRR );
					
					fprintf( CFAPSNRDataG, "%d %f\n", n, PSNRG );
					
					fprintf( CFAPSNRDataB, "%d %f\n", n, PSNRB );
					
				}
				
				fclose( CFAPSNRDataR );
				fclose( CFAPSNRDataG );
				fclose( CFAPSNRDataB );
				
				FILE *CFAPSNRScript = fopen( "CFAPSNRScript.dem" , "w" );
				
				fprintf( CFAPSNRScript, "set terminal emf color\n" );
				fprintf( CFAPSNRScript, "set xrange [ 0 : %d ]\n", FileListBox1 -> Items -> Count );
				
				fprintf( CFAPSNRScript, "set xlabel 'Picture'\n" );
				fprintf( CFAPSNRScript, "set ylabel 'PSNR'\n" );
				
				fprintf( CFAPSNRScript, "set grid\n" );
				
				fprintf( CFAPSNRScript, "set title 'Sobel Interpolation result in PSNR domain'\n" );
				fprintf( CFAPSNRScript, "set ticslevel 0\n" );
				
				fprintf( CFAPSNRScript, "set output '%s'\n", CFAPSNR_EMF );
				
				fprintf( CFAPSNRScript, "plot 'CFAPSNRDataR.dat' title 'R of PSNR' with linespoints 1 2,\
								'CFAPSNRDataG.dat' title 'G of PSNR' with linespoints 2 3,\
								'CFAPSNRDataB.dat' title 'B of PSNR' with linespoints 3 4\n"
					);
				
				fclose( CFAPSNRScript );
				
				char command[ 200 ];
				sprintf( command, "%s\\gnuplot\\bin\\wgnuplot.exe CFAPSNRScript.dem", ExtractFileDrive( FileListBox1 -> FileName ) );
				
				system( command );
				
				remove( "CFAPSNRDataR.dat" );
				remove( "CFAPSNRDataG.dat" );
				remove( "CFAPSNRDataB.dat" );
				remove( "CFAPSNRScript.dem" );
				
			}
			
			Image4 -> Picture -> LoadFromFile( CFAPSNR_EMF );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
			
		}
		
	}
}
//---------------------------------------------------------------------------

