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

//1:red
//2:green
//3:blue
//4:magenta, purple, violet
//5:cyan
//6:brown
//7:orange
//8:coral

//enum color{ red = 1, orange = , yellow = , green = 2, blue = 3, indigo =, violet = 4, brown = 6 };
enum color{ gray = 0, red = 1, green = 2, blue = 3, violet = 4, brown = 6, black = 8 };

struct MacroBlock
{
	double DCa;
	double DCb;
};

#define	imageWidth	2048
#define	imageHeight	1536

BitMapFileHeader header1;
BitMapInfoHeader header2;

__fastcall TForm1::TForm1(TComponent* Owner): TForm(Owner)
{
	
	ImageDataBlockPretty = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	ImageDataBlockUgly = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	
	Lab_buf_Pretty = ( double ** )calloc( imageHeight, sizeof( double * ) );
	Lab_buf_Ugly = ( double ** )calloc( imageHeight, sizeof( double * ) );
	
	for ( int i = 0 ; i < imageHeight ; ++ i )
	{
		
		ImageDataBlockPretty[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );
		ImageDataBlockUgly[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );
		Lab_buf_Pretty[ i ] = ( double * )calloc( imageWidth * 3, sizeof( double ) );
		Lab_buf_Ugly[ i ] = ( double * )calloc( imageWidth * 3, sizeof( double ) );
		
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
			
			for( int n = 0; n < FileListBox1 -> Items -> Count; n ++ )
			{
				
				char RelationEMF[ 150 ];
				
				char UglyFileName[ 150 ];
				
				char PrettyFileName[ 150 ];
				
				char *Checker;
				
				sprintf( RelationEMF, "%s%s", ExtractFilePath( FileListBox1 -> FileName ), FileListBox1 -> Items -> Strings[ n ].c_str() );
				
				Checker = strrchr( RelationEMF, '.' );
				
				if( *( Checker - 5 ) != '_' )
					continue;
				else if( *( Checker - 10 ) != '_' )
					continue;
				else if( *( Checker - 15 ) != '_' )
					continue;
				
				strcpy( UglyFileName, RelationEMF );
				sprintf( Checker - 15, ".bmp" );
				strcpy( PrettyFileName, RelationEMF );
				
				sprintf( strrchr( RelationEMF, '.' ), "_Relation.emf" );
				
				if( !FileExists( RelationEMF ) )
				{
					
					char PresentWorkingDirectory[ 150 ];
					
					sprintf( PresentWorkingDirectory, "%s", ExtractFileDir( FileListBox1 -> FileName ) );
					
					FILE *fp_domain_script = fopen( "histogram.dem", "w" );
					fprintf( fp_domain_script, "cd '%s'\n", PresentWorkingDirectory );
					
					fprintf( fp_domain_script, "set terminal emf\n" );
					fprintf( fp_domain_script, "set xrange [ -35 : 35 ]\n" );
					fprintf( fp_domain_script, "set yrange [ -35 : 35 ]\n" );
					
					fprintf( fp_domain_script, "set xlabel 'a'\n" );
					fprintf( fp_domain_script, "set ylabel 'b'\n" );
					
					fprintf( fp_domain_script, "set title 'Relation conncection in the ab plane'\n" );
					fprintf( fp_domain_script, "set grid\n" );
					
					fprintf( fp_domain_script, "set output '%s'\n", RelationEMF );
					
					MacroBlock blockPretty[ imageHeight / 16 ][ imageWidth / 16 ];
					MacroBlock blockUgly[ imageHeight / 16 ][ imageWidth / 16 ];
					
					memset( blockPretty, 0, sizeof( blockPretty ) );
					memset( blockUgly, 0, sizeof( blockUgly ) );
					
					FILE *fpPretty;
					FILE *fpUgly;
					
					// open test file
					fpPretty = fopen( PrettyFileName, "rb" );
					fpUgly = fopen( UglyFileName, "rb" );
					
					// read BitMapFileHeader
					// struct "BitMapFileHeader" is read 4 bytes by hardware but size of struct "BitMapFileHeader" is 14 bytes,
					// It will be assumed 16 bytes by the hardware, and then it has to minus 2 bytes.
					fread( &header1, 14, 1, fpPretty );
					fread( &header1, 14, 1, fpUgly );	// dumb
					
					// read BitMapInfoHeader
					fread( &header2, 40, 1, fpPretty );
					fread( &header2, 40, 1, fpUgly );	// dumb
                        		
					// according to width and height. Read Image Data Block in order to make RGB imagedatablockfer
					
					for( int i = 0; i < imageHeight; i ++ )
					{	
						
						fread( ImageDataBlockPretty[ i ], 3 * imageWidth, 1, fpPretty );
						fread( ImageDataBlockUgly[ i ], 3 * imageWidth, 1, fpUgly );
						
					}
					
					fclose( fpPretty );
					fclose( fpUgly );
					
					double aSumPretty = 0;
					double bSumPretty = 0;
					
					double aSumUgly = 0;
					double bSumUgly = 0;
					
					//for( int i = 0; i < imageHeight / 16; i ++ )
					for( int i = 0; i < imageHeight / 16; i += 3 )	// for 40 * 10 = 400 blocks
						for( int j = 0; j < imageWidth / 16; aSumPretty = 0, bSumPretty = 0, aSumUgly = 0, bSumUgly = 0, j ++ )
						{
							
							for( int k = i * 16; k <= ( i * 16 + 15 ); k ++ )
								for( int m = j * 16 * 3; m <= ( j * 16 + 15 ) * 3; m += 3 )
								{
									
									RGB2Lab( ImageDataBlockPretty[ k ][ m + 0 ], ImageDataBlockPretty[ k ][ m + 1 ], ImageDataBlockPretty[ k ][ m + 2 ],
										&Lab_buf_Pretty[ k ][ m + 0 ], &Lab_buf_Pretty[ k ][ m + 1 ], &Lab_buf_Pretty[ k ][ m + 2 ] );
									
									RGB2Lab( ImageDataBlockUgly[ k ][ m + 0 ], ImageDataBlockUgly[ k ][ m + 1 ], ImageDataBlockUgly[ k ][ m + 2 ],
										&Lab_buf_Ugly[ k ][ m + 0 ], &Lab_buf_Ugly[ k ][ m + 1 ], &Lab_buf_Ugly[ k ][ m + 2 ] );
									
									aSumPretty += Lab_buf_Pretty[ k ][ m + 1 ];
									bSumPretty += Lab_buf_Pretty[ k ][ m + 2 ];
									
									aSumUgly += Lab_buf_Ugly[ k ][ m + 1 ];
									bSumUgly += Lab_buf_Ugly[ k ][ m + 2 ];
									
								}
							
							blockPretty[ i ][ j ].DCa = aSumPretty / ( 16 * 16 );
							blockPretty[ i ][ j ].DCb = bSumPretty / ( 16 * 16 );
							
							blockUgly[ i ][ j ].DCa = aSumUgly / ( 16 * 16 );
							blockUgly[ i ][ j ].DCb = bSumUgly / ( 16 * 16 );
							
							double distance = pow
								(
									pow( blockUgly[ i ][ j ].DCa - blockPretty[ i ][ j ].DCa, 2 ) +
									pow( blockUgly[ i ][ j ].DCb - blockPretty[ i ][ j ].DCb, 2 ), 0.5
								);
							
							if( distance < 3 )
								fprintf( fp_domain_script, "set arrow from %f, %f to %f, %f head lt %d lw 1\n",
								blockPretty[ i ][ j ].DCa, blockPretty[ i ][ j ].DCb, blockUgly[ i ][ j ].DCa, blockUgly[ i ][ j ].DCb, gray );
							else if( distance < 5 )
								fprintf( fp_domain_script, "set arrow from %f, %f to %f, %f head lt %d lw 1\n",
								blockPretty[ i ][ j ].DCa, blockPretty[ i ][ j ].DCb, blockUgly[ i ][ j ].DCa, blockUgly[ i ][ j ].DCb, red );
							else if( distance < 7 )
								fprintf( fp_domain_script, "set arrow from %f, %f to %f, %f head lt %d lw %d\n",
								blockPretty[ i ][ j ].DCa, blockPretty[ i ][ j ].DCb, blockUgly[ i ][ j ].DCa, blockUgly[ i ][ j ].DCb, green );
							else if( distance < 10 )
								fprintf( fp_domain_script, "set arrow from %f, %f to %f, %f head lt %d lw %d\n",
								blockPretty[ i ][ j ].DCa, blockPretty[ i ][ j ].DCb, blockUgly[ i ][ j ].DCa, blockUgly[ i ][ j ].DCb, blue );
							else if( distance < 13 )
								fprintf( fp_domain_script, "set arrow from %f, %f to %f, %f head lt %d lw %d\n",
								blockPretty[ i ][ j ].DCa, blockPretty[ i ][ j ].DCb, blockUgly[ i ][ j ].DCa, blockUgly[ i ][ j ].DCb, violet );
							else if( distance < 15 )
								fprintf( fp_domain_script, "set arrow from %f, %f to %f, %f head lt %d lw %d\n",
								blockPretty[ i ][ j ].DCa, blockPretty[ i ][ j ].DCb, blockUgly[ i ][ j ].DCa, blockUgly[ i ][ j ].DCb, brown );
							else if( distance > 15 )
								fprintf( fp_domain_script, "set arrow from %f, %f to %f, %f head lt %d lw %d\n",
								blockPretty[ i ][ j ].DCa, blockPretty[ i ][ j ].DCb, blockUgly[ i ][ j ].DCa, blockUgly[ i ][ j ].DCb, black );	
							
						}
					
					fprintf( fp_domain_script, "plot 0\n" );	// no meaning, only for show previous result
					
					fclose( fp_domain_script );
					
					char command[ 200 ];
					sprintf( command, "%s\\gnuplot\\bin\\wgnuplot.exe histogram.dem", ExtractFileDrive( FileListBox1 -> FileName ) );
					
					system( command );
					
					remove( "histogram.dem" );
				}
				
			}
			
			//Image4 -> Picture -> LoadFromFile( RelationEMF );
			
			break;
			
		}
		
	}
}
//---------------------------------------------------------------------------

