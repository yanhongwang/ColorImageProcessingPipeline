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

// x : 0.15 ~ 0.64		64 - 15 = 49
// y : 0.06 ~ 0.60		60 - 6 = 54

#define	xMin		15	// 0.15
#define	xMax		64	// 0.64

#define	yMin		6	// 0.06
#define	yMax		60	// 0.60

#define	TotalPixel		( xMax - xMin ) * ( yMax - yMin )

unsigned char ** ImageDataBlock;

int ** xyDataBlock;

int ** TotalxyDataBlock;

float ** OldCorrelationMatrix;

float ** NewCorrelationMatrix;

int IndexRow = 1;

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
	
	for ( int i = 0; i < imageHeight; ++ i )
		ImageDataBlock[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );
	
	xyDataBlock = ( int ** )calloc( xMax - xMin, sizeof( int * ) );
	TotalxyDataBlock = ( int ** )calloc( xMax - xMin, sizeof( int * ) );
	
	for( int i = 0; i < xMax - xMin; i ++ )
	{
		
		xyDataBlock[ i ] = ( int * )calloc( yMax - yMin, sizeof( int ) );
		TotalxyDataBlock[ i ] = ( int * )calloc( yMax - yMin, sizeof( int ) );
		
	}
	
	OldCorrelationMatrix = ( float ** )calloc( 1, sizeof( float * ) );
	
	OldCorrelationMatrix[ 0 ] = ( float * )calloc( TotalPixel, sizeof( float ) );
				
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FileListBox1Click(TObject *Sender)
{
	TabControl1Change( Sender );
}
//---------------------------------------------------------------------------

void RGB2XYZ( unsigned char B, unsigned char G, unsigned char R, double *X, double *Y, double *Z )
{
	
	*X = ( double )( 0.412453 * R ) + ( double )( 0.357580 * G ) + ( double )( 0.180423 * B );
	*Y = ( double )( 0.212671 * R ) + ( double )( 0.715160 * G ) + ( double )( 0.072169 * B );
	*Z = ( double )( 0.019334 * R ) + ( double )( 0.119193 * G ) + ( double )( 0.950227 * B );
	
}


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
			
			char xyStatisticsGraphFileName[ 150 ];
			
			strcpy( xyStatisticsGraphFileName, FileListBox1 -> FileName.c_str() );
			
			sprintf( strrchr( xyStatisticsGraphFileName, '.' ), ".emf" );
			
			if( !FileExists( xyStatisticsGraphFileName ) )
			{
				
				char PhotoDataFileName[ 150 ];
				
				strcpy( PhotoDataFileName, FileListBox1 -> FileName.c_str() );
				
				sprintf( strrchr( PhotoDataFileName, '.' ), ".dat" );
				
				FILE * PhotoDataFile= fopen( PhotoDataFileName, "w" );
				
				for( int i = 0; i < xMax - xMin; i ++ )
					for( int j = 0; j < yMax - yMin; j ++ )
						xyDataBlock[ i ][ j ] = 0;		// first initialize
				
				char PhotoFileName[ 150 ];
				
				strcpy( PhotoFileName, FileListBox1 -> FileName.c_str() );
				
				ReadBMPData(  PhotoFileName );
				
				double X;
				double Y;
				double Z;
				
				for( int i = 0; i < imageHeight; i++ )
					for( int j = 0; j < imageWidth * 3; j += 3 )
					{
						
						RGB2XYZ
						(
							
							ImageDataBlock[ i ][ j + 0 ],	// B
							ImageDataBlock[ i ][ j + 1 ],	// G
							ImageDataBlock[ i ][ j + 2 ],	// R
							&X, &Y, &Z
							
						);
						
						if( ( X + Y + Z ) != 0 )
						{
							
							int m = ( int )( 100 * X / ( X + Y + Z ) ) - xMin;
							int n = ( int )( 100 * Y / ( X + Y + Z ) ) - yMin;
							
							if( xyDataBlock[ m ][ n ] == 0 )		// this line have to exist in order to avoid error happen
								xyDataBlock[ m ][ n ] = 1;
							
						}
						
					}
				
				for( int i = 0; i < xMax - xMin; i ++ )
					for( int j = 0; j < yMax - yMin; j ++ )
						if( xyDataBlock[ i ][ j ] != 0 )
							fprintf( PhotoDataFile, "%f\t%f\t\n", i / 100.0, j / 100.0 );
				
				fclose( PhotoDataFile );
				
				char PresentWorkingDirectory[ 150 ];
				
				sprintf( PresentWorkingDirectory, "%s", ExtractFileDir( FileListBox1 -> FileName ) );
				
				FILE *fp_domain_script = fopen( "histogram.dem", "w" );
				fprintf( fp_domain_script, "cd '%s'\n", PresentWorkingDirectory );
				
				fprintf( fp_domain_script, "set terminal emf\n" );
				fprintf( fp_domain_script, "set xrange [ 0.15 : 0.64 ]\n" );
				fprintf( fp_domain_script, "set yrange [ 0.06 : 0.60 ]\n" );
				
				fprintf( fp_domain_script, "set xlabel 'x'\n" );
				fprintf( fp_domain_script, "set ylabel 'y'\n" );
				
				fprintf( fp_domain_script, "set title 'xyY domain distribution'\n" );
				fprintf( fp_domain_script, "set grid\n" );
				
				fprintf( fp_domain_script, "set output '%s'\n", xyStatisticsGraphFileName );
				
				fprintf( fp_domain_script, "plot '%s' notitle with points 1 4\n", PhotoDataFileName );
				
				fclose( fp_domain_script );
				
				char command[ 200 ];
				sprintf( command, "%s\\gnuplot\\bin\\wgnuplot.exe histogram.dem", ExtractFileDrive( FileListBox1 -> FileName ) );
				
				system( command );
				
				remove( "histogram.dem" );
				//remove( PhotoDataFileName );
				
			}
			
			Image4 -> Picture -> LoadFromFile( xyStatisticsGraphFileName );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
			
		}
		
		case 2:
		{
			
			char xyStatisticsGraphFileName[ 150 ];
			
			sprintf( xyStatisticsGraphFileName, "%s.emf", ExtractFileName( ExtractFileDir( FileListBox1 -> FileName ) ) );
			
			if( !FileExists( xyStatisticsGraphFileName ) )
			{
				
				char PhotoDataFileName[ 150 ];
				
				sprintf( PhotoDataFileName, "%s.dat", ExtractFileName( ExtractFileDir( FileListBox1 -> FileName ) ) );
				
				FILE * PhotoDataFile= fopen( PhotoDataFileName, "w" );
				
				for( int i = 0; i < xMax - xMin; i ++ )
					for( int j = 0; j < yMax - yMin; j ++ )
					{
						
						TotalxyDataBlock[ i ][ j ] = 0;
						xyDataBlock[ i ][ j ] = 0;		// first initialize
						
					}
				
				for( int n = 0; n < FileListBox1 -> Items -> Count; n ++ )
				{
					
					char PhotoFileName[ 150 ];
					
					sprintf( PhotoFileName, "%s%s", ExtractFilePath( FileListBox1 -> FileName ), FileListBox1 -> Items -> Strings[ n ].c_str() );
					
					ReadBMPData(  PhotoFileName );
					
					double X;
					double Y;
					double Z;
					
					for( int i = 0; i < imageHeight; i++ )
						for( int j = 0; j < imageWidth * 3; j += 3 )
						{
							
							RGB2XYZ
							(
								
								ImageDataBlock[ i ][ j + 0 ],	// B
								ImageDataBlock[ i ][ j + 1 ],	// G
								ImageDataBlock[ i ][ j + 2 ],	// R
								&X, &Y, &Z
								
							);
							
							if( ( X + Y + Z ) != 0 )
							{
								
								int m = ( int )( 100 * X / ( X + Y + Z ) ) - xMin;
								int n = ( int )( 100 * Y / ( X + Y + Z ) ) - yMin;
								
								if( xyDataBlock[ m ][ n ] == 0 )		// this line have to exist in order to avoid error happen
									xyDataBlock[ m ][ n ] = 1;
								
							}
							
						}
					
					for( int i = 0; i < xMax - xMin; i ++ )
						for( int j = 0; j < yMax - yMin; j ++ )
						{
							
							TotalxyDataBlock[ i ][ j ] += xyDataBlock[ i ][ j ];
							xyDataBlock[ i ][ j ] = 0;
							
						}
					
				}
				
				float MaxValue = 0;
				
				for( int i = 0; i < xMax - xMin; i ++ )
					for( int j = 0; j < yMax - yMin; j ++ )
						if( TotalxyDataBlock[ i ][ j ] != 0 )
						{
							
							fprintf( PhotoDataFile, "%f\t%f\t%d\n", i / 100.0, j / 100.0, TotalxyDataBlock[ i ][ j ] );
							
							// find out the maximum value of many picture in the unique luminance
							if( TotalxyDataBlock[ i ][ j ] > MaxValue )
								MaxValue = TotalxyDataBlock[ i ][ j ];
							
						}
				
				fclose( PhotoDataFile );
				
				char PresentWorkingDirectory[ 150 ];
				
				sprintf( PresentWorkingDirectory, "%s", ExtractFileDir( FileListBox1 -> FileName ) );
				
				FILE *fp_domain_script = fopen( "histogram.dem", "w" );
				fprintf( fp_domain_script, "cd '%s'\n", PresentWorkingDirectory );
				
				fprintf( fp_domain_script, "set terminal emf\n" );
				fprintf( fp_domain_script, "set xrange [ 0.15 : 0.64 ]\n" );
				fprintf( fp_domain_script, "set yrange [ 0.06 : 0.60 ]\n" );
				
				fprintf( fp_domain_script, "set xlabel 'x'\n" );
				fprintf( fp_domain_script, "set ylabel 'y'\n" );
				fprintf( fp_domain_script, "set zlabel 'intensity'\n" );
				
				fprintf( fp_domain_script, "set dgrid3d 30, 30\n" );
				fprintf( fp_domain_script, "set hidden3d\n" );
				
				fprintf( fp_domain_script, "set title 'xyY domain distribution'\n" );
				fprintf( fp_domain_script, "set grid\n" );
				fprintf( fp_domain_script, "set ticslevel 0\n" );
				
				fprintf( fp_domain_script, "set output '%s'\n", xyStatisticsGraphFileName );
				
				fprintf( fp_domain_script, "splot '%s' notitle with lines\n\n", PhotoDataFileName );
				
				fclose( fp_domain_script );
				
				char command[ 200 ];
				sprintf( command, "%s\\gnuplot\\bin\\wgnuplot.exe histogram.dem", ExtractFileDrive( FileListBox1 -> FileName ) );
				
				system( command );
				
				remove( "histogram.dem" );
				//remove( PhotoDataFileName );
				
				// fill new luminance to memory
				// declare new matrix
				NewCorrelationMatrix = ( float ** )calloc( ++ IndexRow, sizeof( float * ) );
				
				for( int i = 0; i < IndexRow; i ++ )
					NewCorrelationMatrix[ i ] = ( float * )calloc( TotalPixel, sizeof( float ) );
				
				int length;
				// fill out new row to the start of new matrix 
				for( int i = 0; i < xMax - xMin; i ++ )
				{
					
					length = i * ( yMax - yMin );
					
					for( int j = 0; j < yMax - yMin; j ++ )
						NewCorrelationMatrix[ 0 ][ length + j ] = TotalxyDataBlock[ i ][ j ] / MaxValue;
					
				}
				
				// append original matrix to the end of the new matrix
				for( int i = 1, k = 0; i < IndexRow; i ++, k ++ )
					for( int j = 0; j < TotalPixel; j ++ )
						NewCorrelationMatrix[ i ][ j ] = OldCorrelationMatrix[ k ][ j ];
				
				free( OldCorrelationMatrix );
				
				// declare original matrix
				OldCorrelationMatrix = ( float ** )calloc( IndexRow, sizeof( float * ) );
				
				for( int i = 0; i < IndexRow; i ++ )
					OldCorrelationMatrix[ i ] = ( float * )calloc( TotalPixel, sizeof( float ) );
				
				// fill out new matrix to original matrix
				for( int i = 0; i < IndexRow; i ++ )
					for( int j = 0; j < TotalPixel; j ++ )
						OldCorrelationMatrix[ i ][ j ] = NewCorrelationMatrix[ i ][ j ];
				
				free( NewCorrelationMatrix );
				
			}
			
			Image4 -> Picture -> LoadFromFile( xyStatisticsGraphFileName );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			
			break;
			
		}
		
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BuildDataBaseClick(TObject *Sender)
{
	
	char CorrelationMatrixFileName[ 150 ];
	
	sprintf( CorrelationMatrixFileName, "%s",ExtractFileDir( ExtractFileDir( FileListBox1 -> FileName ) ) );
	
	strcat( CorrelationMatrixFileName, "\\CorrelationMatrix.h" );
	
	FILE * CorrelationMatrixFile = fopen( CorrelationMatrixFileName, "w" );
	
	fprintf( CorrelationMatrixFile, "static float Probability[ %d ][ %d ] =\n", IndexRow - 1, TotalPixel );
	
	fprintf( CorrelationMatrixFile, "{\n" );
	
	for( int i = 0; i < IndexRow - 1; i ++ )
	{
		
		fprintf( CorrelationMatrixFile, "\t{" );
		
		for( int j = 0; j < TotalPixel; j ++ )
		{
			if( j % 8 == 0 )
				fprintf( CorrelationMatrixFile, "\n\t" );
			fprintf( CorrelationMatrixFile, "%f, ", OldCorrelationMatrix[ i ][ j ] );
			
		}
		
		fprintf( CorrelationMatrixFile, "\n\t}\n" );
		
	}
	
	fprintf( CorrelationMatrixFile, "};\n" );
	
	fclose( CorrelationMatrixFile );
	
}
//---------------------------------------------------------------------------

