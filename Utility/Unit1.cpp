//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include	<math.h>

#include "Unit1.h"

unsigned char **ImageDataBlock;		//RGB buffer
unsigned char **BayerPattern;

#define	imageWidth	640
#define	imageHeight	480

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
	
	srand( time( NULL ) );
	
	ImageDataBlock = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	BayerPattern = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	
	for ( int i = 0 ; i < imageHeight ; ++ i )
	{
		
		ImageDataBlock[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );
		BayerPattern[ i ] = ( unsigned char * )calloc( imageWidth, sizeof( unsigned char ) );
		
	}
	
	// pow math operation is default option
	MathOperationGroup -> ItemIndex = 0;
	
}
//---------------------------------------------------------------------------

void AdjustRange( double X, double *Xmin, double *Xmax )
{
	if( X < *Xmin )
		*Xmin = X ;
	else if( X > *Xmax )
		*Xmax = X ;
}

void __fastcall TForm1::calculateClick(TObject *Sender)
{
	
	int r_range, g_range, b_range;
	double L, a, b;
	double Lmin = 10000.0, Lmax = -10000.0, amin = 10000.0, amax = -10000.0, bmin = 10000.0, bmax = -10000.0;
	
	char LRange[ 100 ];
	char aRange[ 100 ];
	char bRange[ 100 ];
	
	for( r_range = RMin -> Text.ToInt(); r_range <= RMax -> Text.ToInt(); r_range ++ )
		for( g_range = GMin -> Text.ToInt(); g_range <= GMax -> Text.ToInt(); g_range ++ )
			for( b_range = BMin -> Text.ToInt(); b_range <= BMax -> Text.ToInt(); b_range ++ )
			{
				
				RGB2Lab( b_range, g_range, r_range, &L, &a, &b );
				
				AdjustRange( L, &Lmin, &Lmax );
				AdjustRange( a, &amin, &amax );
				AdjustRange( b, &bmin, &bmax );
				
			}
	
	sprintf( LRange, "%f to %f", Lmin, Lmax );
	sprintf( aRange, "%f to %f", amin, amax );
	sprintf( bRange, "%f to %f", bmin, bmax );
	
	LValue -> Caption = LRange;
	aValue -> Caption = aRange;
	bValue -> Caption = bRange;
	
}
//---------------------------------------------------------------------------

void MakePattern(unsigned char **ImageDataBlock, long Height, long Width, unsigned char **BayerPattern )
{
	
	// Read Image Data Block according to imege height and width, then make the pattern.
	for( int i = 0; i < Height; i ++ )
		for( int j = 0; j < Width; j++ )
			if( i % 2 == 0 )
				if( j % 2 == 0 )
					BayerPattern[ i ][ j ] = ImageDataBlock[ i ][ 3 * j + 2 ];	//R
				else
					BayerPattern[ i ][ j ] = ImageDataBlock[ i ][ 3 * j + 1 ];	//G
			else
				if( j % 2 == 0 )
					BayerPattern[ i ][ j ] = ImageDataBlock[ i ][ 3 * j + 1 ];	//G
				else
					BayerPattern[ i ][ j ] = ImageDataBlock[ i ][ 3 * j ];		//B
	
}

void SaveToRaw(unsigned char **BayerPattern, long Height, long Width, char *outfile )
{
	
	FILE *fp;
	fp = fopen( outfile, "wb" );
	
	for( int i = 0; i < Height; i ++ )
		fwrite( BayerPattern[ i ], Width, 1, fp );
	fclose(fp);
	
}

void __fastcall TForm1::RGB2RAWClick(TObject *Sender)
{
	
	FILE *fp;
	
	BitMapFileHeader header1;
	BitMapInfoHeader header2;
	
	char bmpfile[ 150 ];
	
	for( int m = 0; m < FileListBox1 -> Items -> Count; m ++ )
	{
		
		sprintf( bmpfile, "%s%s", ExtractFilePath( FileListBox1 -> FileName ), FileListBox1 -> Items -> Strings[ m ].c_str() );
		
		fp = fopen( bmpfile, "rb" );
		
		fread( &header1, 14, 1, fp );
		fread( &header2, 40, 1, fp );
		
		for( int j = 0; j < imageHeight; j ++ )
			fread( ImageDataBlock[ j ], 3 * imageWidth, 1, fp );
		
		fclose( fp );
		
		MakePattern( ImageDataBlock, imageHeight, imageWidth, BayerPattern );
		SaveToRaw( BayerPattern, imageHeight, imageWidth, bmpfile );
		
		rename( bmpfile, ChangeFileExt( bmpfile, ".raw" ).c_str() );
		
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm1::RGB2XYZTableClick(TObject *Sender)
{
	
	FILE *LookupTableRGB2X = fopen( "RGB2X.h", "w" );
	FILE *LookupTableRGB2Y = fopen( "RGB2Y.h", "w" );
	FILE *LookupTableRGB2Z = fopen( "RGB2Z.h", "w" );
	
	//////////////////////////////////////////////////////////////////////
	
	fprintf( LookupTableRGB2X, "static double RGB2X_R[] =\n{\n" );
	fprintf( LookupTableRGB2Y, "static double RGB2Y_R[] =\n{\n" );
	fprintf( LookupTableRGB2Z, "static double RGB2Z_R[] =\n{\n" );
	
	for( int i = 0; i <= 255; i ++ )
	{
		
		fprintf( LookupTableRGB2X, "\t%f,\n", i * 0.412453 );
		fprintf( LookupTableRGB2Y, "\t%f,\n", i * 0.212671 );
		fprintf( LookupTableRGB2Z, "\t%f,\n", i * 0.019334 );
		
	}
	
	fprintf( LookupTableRGB2X, "};\n\n" );
	fprintf( LookupTableRGB2Y, "};\n\n" );
	fprintf( LookupTableRGB2Z, "};\n\n" );
	
	//////////////////////////////////////////////////////////////////////
	
	fprintf( LookupTableRGB2X, "static double RGB2X_G[] =\n{\n" );
	fprintf( LookupTableRGB2Y, "static double RGB2Y_G[] =\n{\n" );
	fprintf( LookupTableRGB2Z, "static double RGB2Z_G[] =\n{\n" );
	
	for( int i = 0; i <= 255; i ++ )
	{
		
		fprintf( LookupTableRGB2X, "\t%f,\n", i * 0.357580 );
		fprintf( LookupTableRGB2Y, "\t%f,\n", i * 0.715160 );
		fprintf( LookupTableRGB2Z, "\t%f,\n", i * 0.119193 );
	
	}
	
	fprintf( LookupTableRGB2X, "};\n\n" );
	fprintf( LookupTableRGB2Y, "};\n\n" );
	fprintf( LookupTableRGB2Z, "};\n\n" );
	
	//////////////////////////////////////////////////////////////////////
	
	fprintf( LookupTableRGB2X, "static double RGB2X_B[] =\n{\n" );
	fprintf( LookupTableRGB2Y, "static double RGB2Y_B[] =\n{\n" );
	fprintf( LookupTableRGB2Z, "static double RGB2Z_B[] =\n{\n" );
	
	for( int i = 0; i <= 255; i ++ )
	{
		
		fprintf( LookupTableRGB2X, "\t%f,\n", i * 0.180423 );
		fprintf( LookupTableRGB2Y, "\t%f,\n", i * 0.072169 );
		fprintf( LookupTableRGB2Z, "\t%f,\n", i * 0.950227 );
	
	}
	
	fprintf( LookupTableRGB2X, "};\n\n" );
	fprintf( LookupTableRGB2Y, "};\n\n" );
	fprintf( LookupTableRGB2Z, "};\n\n" );
	
	//////////////////////////////////////////////////////////////////////
	
	fclose( LookupTableRGB2X );
	fclose( LookupTableRGB2Y );
	fclose( LookupTableRGB2Z );
	      
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RGB2LABTableClick(TObject *Sender)
{
	
	int stepBit = 4;
	
	int step = 1 << stepBit;
	
	int scale = 512;
	
	int RCount = ( RMax -> Text.ToInt() - RMin -> Text.ToInt() + 1 ) / step;
	
	int GCount = ( GMax -> Text.ToInt() - GMin -> Text.ToInt() + 1 ) / step;
	
	int BCount = ( BMax -> Text.ToInt() - BMin -> Text.ToInt() + 1 ) / step;
	
	FILE *LookUpTableRGB2LAB;
	
	LookUpTableRGB2LAB = fopen( "LookUpTableRGB2LAB.h", "w" );
	
	fprintf( LookUpTableRGB2LAB, "// static int RGB2LAB_16_512[ 4096 ][ 3 ]\n" );
	fprintf( LookUpTableRGB2LAB, "// 16 : increment step, 512 : integer scale, 4096 : row size\n\n" );
	
	fprintf( LookUpTableRGB2LAB, "#define	RGB_RCycle	%d\n\n", stepBit + stepBit );
	
	fprintf( LookUpTableRGB2LAB, "#define	RGB_GCycle	%d\n\n", stepBit );
	
	fprintf( LookUpTableRGB2LAB, "#define	RGB2LabStep	%d\n\n", step );
	
	fprintf( LookUpTableRGB2LAB, "#define	RGB2LabStepBit	%d\n\n", stepBit );
	
	fprintf( LookUpTableRGB2LAB, "static int RGB2LAB_%d_%d[ %d ][ 3 ] =\n{\n",
																step, scale, RCount * GCount * BCount );
	
	double L, a, b;
	
	for( int r_range = RMin -> Text.ToInt(); r_range <= RMax -> Text.ToInt(); r_range += step )
		for( int g_range = GMin -> Text.ToInt(); g_range <= GMax -> Text.ToInt(); g_range += step )
			for( int b_range = BMin -> Text.ToInt(); b_range <= BMax -> Text.ToInt(); b_range += step )
			{
				
				RGB2Lab( b_range, g_range, r_range, &L, &a, &b );
				
				fprintf( LookUpTableRGB2LAB, "\t{ %d, %d, %d },\t// R = %d, G = %d, B = %d\n",
																			( int )( L * scale ),
																			( int )( a * scale ),
																			( int )( b * scale ),
																			r_range,
																			g_range,
																			b_range
								);
				
			}
	
	fprintf( LookUpTableRGB2LAB, "};\n" );
	
	fclose( LookUpTableRGB2LAB );
	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::LAB2RGBTableClick(TObject *Sender)
{
	
	int stepBit = 4;
	
	int step = 1 << stepBit;
	
	int scale = 512;
	
	int L_range, a_range, b_range;
	
	unsigned char R, G, B;
	
	int LMax = 100;
	int LMin = 0;
	
	int aMax = 98;
	int aMin = -86;
	
	int bMax = 94;
	int bMin = -108;
	
	int LCount = ( LMax - LMin + 1 ) / step + 1;
	
	int aCount = ( aMax - aMin + 2 ) / step + 1;	// because its value cross postive and negtive
	
	int bCount = ( bMax - bMin + 2 ) / step + 1;	// because its value cross postive and negtive
	
	FILE *LookUpTableLAB2RGB;
	
	LookUpTableLAB2RGB = fopen( "LookUpTableLAB2RGB.h", "w" );
	
	fprintf( LookUpTableLAB2RGB, "// static int LAB2RGB_8_512[ 792 ][ 3 ]\n" );
	
	fprintf( LookUpTableLAB2RGB, "// 8 : increment step, 512 : integer scale, 792 : row size\n\n" );
	
	fprintf( LookUpTableLAB2RGB, "#define	Lab_LCycleNumber	%d\n\n", aCount * bCount );
	
	fprintf( LookUpTableLAB2RGB, "#define	Lab_aCycleNumber	%d\n\n", bCount );
	
	fprintf( LookUpTableLAB2RGB, "#define	Lab2RGBStep	%d\n\n", step );
	
	fprintf( LookUpTableLAB2RGB, "#define	Lab2RGBStepBit	%d\n\n", stepBit );
	
	fprintf( LookUpTableLAB2RGB, "static int LAB2RGB_%d_%d[ %d ][ 3 ] =\n{\n",
																step, scale, LCount * aCount * bCount );
	
	for( L_range = LMin; L_range <= LMax; L_range += step )
		for( a_range = aMin; a_range <= aMax; a_range += step )
			for( b_range = bMin; b_range <= bMax; b_range += step )
			{
				
				Lab2RGB( ( double )L_range, ( double )a_range, ( double )b_range, &B, &G, &R );
				fprintf( LookUpTableLAB2RGB, "\t{ %d, %d, %d },\t// L = %d, a = %d, b = %d\n", R * scale, G * scale, B * scale, L_range, a_range, b_range );
				//fprintf( LookUpTableLAB2RGB, "\t{ %d, %d, %d },\t// L = %d, a = %d, b = %d\n", R, G, B, L_range, a_range, b_range );
				
			}
	
	fprintf( LookUpTableLAB2RGB, "};\n" );
	
	fclose( LookUpTableLAB2RGB );
	
}
//---------------------------------------------------------------------------

void addnoise_RGB( unsigned char **ImageDataBlock )
{
	
	int intNoiseValue = 10;							// noise maximum value
	int intPerNValue = 10;
	int intImageSize = imageHeight * imageWidth * 3;			// input image pixel count
	int intNoisePointNumber = ( int )( intPerNValue * 0.01 * intImageSize );// noise point count
	int intRow, intColumn;      
	int temp;
	
	for( int i = 0 ; i <= intNoisePointNumber ; i ++ )
	{
		
		intRow = rand() % imageHeight;
		
		intColumn = rand() % ( imageWidth * 3 );
		temp = ImageDataBlock[ intRow ][ intColumn ] ;
		if( rand() % 2 ) 
			temp += rand() % intNoiseValue;
		else 
			temp -= rand() % intNoiseValue;

		if ( temp > 255 ) 
			ImageDataBlock[ intRow ][ intColumn ] = 255;
		else if ( temp < 0 ) 
			ImageDataBlock[ intRow ][ intColumn ] = 0;
		else 
			ImageDataBlock[ intRow ][ intColumn ] = (unsigned char) temp;
		
	}
	
}

void __fastcall TForm1::RandomRGBClick(TObject *Sender)
{
	
	BitMapFileHeader header1;
	BitMapInfoHeader header2;
	
	double RStartValue = RLow -> Text.ToDouble();
	double RStopValue = RHigh -> Text.ToDouble();
	
	double GStartValue = GLow -> Text.ToDouble();
	double GStopValue = GHigh -> Text.ToDouble();
	
	double BStartValue = BLow -> Text.ToDouble();
	double BStopValue = BHigh -> Text.ToDouble();
	
	float RRatio = 0;
	float GRatio = 0;
	float BRatio = 0;
	
	char OldFileName[ 150 ];
	char NewFileName[ 150 ];
	
	FILE *fp;
	
	unsigned char Rtemp;
	unsigned char Gtemp;
	unsigned char Btemp;
	
	for( int m = 0; m < FileListBox1 -> Items -> Count; m ++ )
	{
		
		RRatio = rand() / ( ( float )RAND_MAX ) * ( RStopValue - RStartValue ) + RStartValue;
		GRatio = rand() / ( ( float )RAND_MAX ) * ( GStopValue - GStartValue ) + GStartValue;
		BRatio = rand() / ( ( float )RAND_MAX ) * ( BStopValue - BStartValue ) + BStartValue;
		
		sprintf( OldFileName, "%s%s", ExtractFilePath( FileListBox1 -> FileName ), FileListBox1 -> Items -> Strings[ m ].c_str() );
		
		fp = fopen( OldFileName, "rb" );
		
		fread( &header1, 14, 1, fp );
		fread( &header2, 40, 1, fp );
		
		for( int j = 0; j < imageHeight; j ++ )
			fread( ImageDataBlock[ j ], 3 * imageWidth, 1, fp );
		
		fclose( fp );
		
		for( int i = 0; i < imageHeight; i ++ )
			for( int j = 0; j < imageWidth * 3; j += 3 )
			{
				
				ImageDataBlock[ i ][ j + 0 ] /= BRatio;	// B element random ratio
				ImageDataBlock[ i ][ j + 1 ] /= GRatio;	// G element random ratio
				ImageDataBlock[ i ][ j + 2 ] /= RRatio;	// R element random ratio
				
				Rtemp = ImageDataBlock[ i ][ j + 2 ];
				Gtemp = ImageDataBlock[ i ][ j + 1 ];
				Btemp = ImageDataBlock[ i ][ j + 0 ];
				
				ImageDataBlock[ i ][ j + 2 ] = Rtemp * 0.8 + Gtemp * 0.1 + Btemp * 0.1;	// R element camera factor
				ImageDataBlock[ i ][ j + 1 ] = Rtemp * 0.1 + Gtemp * 0.8 + Btemp * 0.1;	// G element camera factor
				ImageDataBlock[ i ][ j + 0 ] = Rtemp * 0.2 + Gtemp * 0.1 + Btemp * 0.7;	// B element camera factor
				
			}
		
		addnoise_RGB( ImageDataBlock );				// add random noise in image
		
		sprintf( NewFileName, "%s%s", ExtractFilePath( FileListBox1 -> FileName ), FileListBox1 -> Items -> Strings[ m ].c_str() );
		
		sprintf( strrchr( NewFileName, '.' ), "_%d_%d_%d.bmp", ( int )( RRatio * 1000 ), ( int )( GRatio * 1000 ), ( int )( BRatio * 1000 ) );
		
		rename( OldFileName, NewFileName );
		
		fp = fopen( NewFileName, "wb" );
		
		fwrite( &header1, sizeof( BitMapFileHeader ) - 2, 1, fp );
		fwrite( &header2, sizeof( BitMapInfoHeader ), 1, fp );
		
		for( int i = 0; i < imageHeight; i ++ )
			fwrite( ImageDataBlock[ i ], imageWidth * 3, 1, fp );
		
		fclose( fp );
		
	}
	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::calculate_xyClick(TObject *Sender)
{
	
	int r_range, g_range, b_range;
	double X, Y, Z;
	double xmin = 10000.0, xmax = -10000.0, ymin = 10000.0, ymax = -10000.0;
	
	char xRange[ 100 ];
	char yRange[ 100 ];
	
	for( r_range = RMin -> Text.ToInt(); r_range <= RMax -> Text.ToInt(); r_range ++ )
		for( g_range = GMin -> Text.ToInt(); g_range <= GMax -> Text.ToInt(); g_range ++ )
			for( b_range = BMin -> Text.ToInt(); b_range <= BMax -> Text.ToInt(); b_range ++ )
			{
				
				RGB2XYZ( ( unsigned char )b_range, ( unsigned char )g_range, ( unsigned char )r_range, &X, &Y, &Z );
				
				if( ( X + Y + Z ) != 0 )
				{
					AdjustRange( X / ( X + Y + Z ), &xmin, &xmax );
					AdjustRange( Y / ( X + Y + Z ), &ymin, &ymax );
				}
			}
	
	sprintf( xRange, "%f to %f", xmin, xmax );
	sprintf( yRange, "%f to %f", ymin, ymax );
	
	xValue -> Caption = xRange;
	yValue -> Caption = yRange;
	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::MathTableGenerateClick(TObject *Sender)
{
	
	char TableFileName[ 150 ];
	
	float range = MathMaxBoundary -> Text.ToInt() - MathMinBoundary -> Text.ToInt() + 1;
	
	FILE * TableFile;
	
	switch( MathOperationGroup -> ItemIndex )
	{
		
		// power operation
		case 0:
		{
			
			sprintf( TableFileName, "LookTable_pow_%03d.h", ( int )( Argument -> Text.ToDouble() * 100 ) );
			
			TableFile = fopen( TableFileName, "w" );
			
			fprintf( TableFile, "// power value = %f\n\n", Argument -> Text.ToDouble() );
			
			fprintf( TableFile, "static int pow_%d_%d_%d[ %d ] =\n",
													MathMinBoundary -> Text.ToInt(),
													MathMaxBoundary -> Text.ToInt(),
													ScaleNumber -> Text.ToInt(),
													( int )range
						);
			
			fprintf( TableFile,"{\t" );
			
			for( int i = MathMinBoundary -> Text.ToInt(); i <= MathMaxBoundary -> Text.ToInt(); i ++ )
			{
				
				if( i % 16 == 0 )
					fprintf( TableFile, "\n\t" );
				
				fprintf( TableFile, "%d, ", ( int )( pow( i / range, Argument -> Text.ToDouble() ) * range * ScaleNumber -> Text.ToInt() ) );
				
			}
			
			fprintf( TableFile, "\n};\n\n\n" );
			
			break;
			
		}
		
		// natural logarithm operation
		case 1:
		{
			
			sprintf( TableFileName, "LookTable_log_%03d.h", ( int )( Argument -> Text.ToDouble() * 100 ) );
			
			TableFile = fopen( TableFileName, "w" );
			
			
			fprintf( TableFile, "static int log_%d_%d_%d[ %d ] =\n",
													MathMinBoundary -> Text.ToInt(),
													MathMaxBoundary -> Text.ToInt(),
													ScaleNumber -> Text.ToInt(),
													MathMaxBoundary -> Text.ToInt() - MathMinBoundary -> Text.ToInt() + 1
						);
			
			fprintf( TableFile,"{\t" );
			
			for( int i = MathMinBoundary -> Text.ToInt(); i <= MathMaxBoundary -> Text.ToInt(); i ++ )
			{
				
				if( i % 16 == 0 )
					fprintf( TableFile, "\n\t" );
				
				fprintf( TableFile, "%d, ", ( int )( log( i ) * ScaleNumber -> Text.ToInt() ) );
				
			}
			
			fprintf( TableFile, "\n};\n\n\n" );
			
			break;
			
		}
		
		// radix base 10 logarithm operation
		case 2:
		{
			
			sprintf( TableFileName, "LookTable_log10.h" );
			
			TableFile = fopen( TableFileName, "w" );
			
			fprintf( TableFile, "static int log10_%d_%d_%d[ %d ] =\n",
													MathMinBoundary -> Text.ToInt(),
													MathMaxBoundary -> Text.ToInt(),
													ScaleNumber -> Text.ToInt(),
													MathMaxBoundary -> Text.ToInt() - MathMinBoundary -> Text.ToInt() + 1
						);
			
			fprintf( TableFile,"{\t" );
			
			for( int i = MathMinBoundary -> Text.ToInt(); i <= MathMaxBoundary -> Text.ToInt(); i ++ )
			{
				
				if( i % 16 == 0 )
					fprintf( TableFile, "\n\t" );
				
				fprintf( TableFile, "%d, ", ( int )( log10( i ) * ScaleNumber -> Text.ToInt() ) );
				
			}
			
			fprintf( TableFile, "\n};\n\n\n" );
			
			break;
			
		}
		
		// exponent operation
		case 3:
		{
			
			sprintf( TableFileName, "LookTable_exp_%03d.h", ( int )( Argument -> Text.ToDouble() * 100 ) );
			
			TableFile = fopen( TableFileName, "w" );
			
			
			fprintf( TableFile, "static int exp_%d_%d_%d[ %d ] =\n",
													MathMinBoundary -> Text.ToInt(),
													MathMaxBoundary -> Text.ToInt(),
													ScaleNumber -> Text.ToInt(),
													MathMaxBoundary -> Text.ToInt() - MathMinBoundary -> Text.ToInt() + 1
						);
			
			fprintf( TableFile,"{\t" );
			
			for( int i = MathMinBoundary -> Text.ToInt(); i <= MathMaxBoundary -> Text.ToInt(); i ++ )
			{
				
				if( i % 16 == 0 )
					fprintf( TableFile, "\n\t" );
				
				fprintf( TableFile, "%d, ", ( int )( exp( i ) * ScaleNumber -> Text.ToInt() ) );
				
			}
			
			fprintf( TableFile, "\n};\n\n\n" );
			
			break;
			
		}
		
	}
	
	fprintf( TableFile, "// exmaple : static int MathOperation_MinValue_MaxValue_ScaleNumber[ MatrixSize ]\n" );
	fprintf( TableFile, "//           static int pow_0_255_256[ 256 ]\n" );
	
	fclose( TableFile );
	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::calculateXYZClick(TObject *Sender)
{
	
	int r_range, g_range, b_range;
	double X, Y, Z;
	double Xmin = 10000.0, Xmax = -10000.0, Ymin = 10000.0, Ymax = -10000.0, Zmin = 10000.0, Zmax = -10000.0;
	
	char XRange[ 100 ];
	char YRange[ 100 ];
	char ZRange[ 100 ];
	
	for( r_range = RMin -> Text.ToInt(); r_range <= RMax -> Text.ToInt(); r_range ++ )
		for( g_range = GMin -> Text.ToInt(); g_range <= GMax -> Text.ToInt(); g_range ++ )
			for( b_range = BMin -> Text.ToInt(); b_range <= BMax -> Text.ToInt(); b_range ++ )
			{
				
				RGB2XYZ( b_range, g_range, r_range, &X, &Y, &Z );
				
				AdjustRange( X, &Xmin, &Xmax );
				AdjustRange( Y, &Ymin, &Ymax );
				AdjustRange( Z, &Zmin, &Zmax );
				
			}
	
	sprintf( XRange, "%f to %f", Xmin, Xmax );
	sprintf( YRange, "%f to %f", Ymin, Ymax );
	sprintf( ZRange, "%f to %f", Zmin, Zmax );
	
	CIEXValue -> Caption = XRange;
	CIEYValue -> Caption = YRange;
	CIEZValue -> Caption = ZRange;
	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::uglyRGB2XYZClick(TObject *Sender)
{
	
	int r_range, g_range, b_range;
	double X, Y, Z;
	double Xmin = 10000.0, Xmax = -10000.0, Ymin = 10000.0, Ymax = -10000.0, Zmin = 10000.0, Zmax = -10000.0;
	
	char Range[ 150 ];
	
	for( r_range = RMin -> Text.ToInt(); r_range <= RMax -> Text.ToInt(); r_range ++ )
		for( g_range = GMin -> Text.ToInt(); g_range <= GMax -> Text.ToInt(); g_range ++ )
			for( b_range = BMin -> Text.ToInt(); b_range <= BMax -> Text.ToInt(); b_range ++ )
			{
				
				// with the D65 reference white: Xn = 0.9505; Yn = 1.00; Zn = 1.0891;
				// X /= ( 255 * 0.950456 );
				// Y /=  255;
				// Z /= ( 255 * 1.088754 );
				
				X = 256 * ( 0.412453 * r_range + 0.357580 * g_range + 0.180423 * b_range ) / 242;
				Y = 256 * ( 0.212671 * r_range + 0.715160 * g_range + 0.072169 * b_range ) / 255;
				Z = 256 * ( 0.019334 * r_range + 0.119193 * g_range + 0.950227 * b_range ) / 278;
				
				AdjustRange( X, &Xmin, &Xmax );
				AdjustRange( Y, &Ymin, &Ymax );
				AdjustRange( Z, &Zmin, &Zmax );
				
			}
	
	sprintf( Range, "X : %f to %f\nY : %f to %f\nZ : %f to %f", Xmin, Xmax, Ymin, Ymax, Zmin, Zmax );
	
	ShowMessage( Range );
	
	//float Max = -10000;
	//float Min = 10000;
	
	int Max = -10000;
	int Min = 10000;
	
	if( Max < Xmax )	Max = Xmax;
	if( Max < Ymax )	Max = Ymax;
	if( Max < Zmax )	Max = Zmax;
	
	if( Min > Xmin )	Min = Xmin;
	if( Min > Ymin )	Min = Ymin;
	if( Min > Zmin )	Min = Zmin;
	
	// ShowMessage( Max );
	// ShowMessage( Min );
	
	Max = 256;
	Min = 0;
	
	char TableFileName[ 150 ];
	
	//int range = rangeMax - rangeMin + 1;
	int range = Max - Min + 1;
	
	FILE * TableFile;
	
	// power operation
	sprintf( TableFileName, "LookTable_pow_%03d.h", ( int )( 1.0 / 3.0 * 100 ) );
	
	TableFile = fopen( TableFileName, "w" );
	
	int CIC = 1;
	
	if( CIC == 1 )
	{
		
		char LUTPow033FileName[ 150 ];
		strcpy( LUTPow033FileName, "LUTPow033.emf" );
		
		FILE *LUTPow033Data = fopen( "LUTPow033Data.dat", "w" );
		
		int LUTPow033Value;
		
		for( int i = Min; i <= Max; i ++  )
		{
			
			LUTPow033Value = ( int )( pow( i, 1.0 / 3.0 ) * 256.0 );
			
			fprintf( TableFile, "%d : LUTPow033 = %d;\n", i, LUTPow033Value );
			
			fprintf( LUTPow033Data, "%d %d\n", i, LUTPow033Value );
			
		}
		
		fclose( LUTPow033Data );
		
		FILE *fp_domain_script = fopen( "histogram.dem", "w" );
		
		fprintf( fp_domain_script, "set terminal emf\n" );
		fprintf( fp_domain_script, "set xrange [ 0 : 257 ]\n" );
		
		fprintf( fp_domain_script, "set xlabel 'CIEXYZ Level'\n" );
		
		fprintf( fp_domain_script, "set ylabel 'CIELab Level'\n" );
		
		fprintf( fp_domain_script, "set grid\n" );
		fprintf( fp_domain_script, "set output '%s'\n", LUTPow033FileName );
		fprintf( fp_domain_script, "plot 'LUTPow033Data.dat' title '%s' with lines 1 4\n", "LUTPow033Data.dat" );
		
		fclose( fp_domain_script );
		
		char command[ 200 ];
		sprintf( command, "d:\\gnuplot\\bin\\wgnuplot.exe histogram.dem" );
		
		system( command );
		
		remove( "histogram.dem" );
		remove( "LUTPow033Data.dat" );
				
	}
	else
	{
		
		fprintf( TableFile, "// power value = %f\n\n", 1.0 / 3.0 );
		
		fprintf( TableFile, "static int pow_%d_%d_%d[ %d ] =\n", Min, Max, 256, range );
		
		fprintf( TableFile,"{\t" );
		
		for( int i = Min; i <= Max; i ++  )
		{
			
			if( i % 16 == 0 )
				fprintf( TableFile, "\n\t" );
			
			fprintf( TableFile, "%d, ", ( int )( pow( i, 1.0 / 3.0 ) * 256.0 ) );
			
		}
		
		fprintf( TableFile, "\n};\n" );
		
	}
	
	fclose( TableFile );
	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::uglyCTCClick(TObject *Sender)
{
	
	// assume ctc RCoefficient = 1.0 ~ 2.5 and BCoefficient = 1.0 ~ 2.5
	
	float GRRatioMin = 1.0;
	float GRRatioMax = 2.5;
	
	float GBRatioMin = 1.0;
	float GBRatioMax = 2.5;
	
	float Scale = 16;
	
	float GRRange = ( GRRatioMax - GRRatioMin ) * Scale;
	float GBRange = ( GBRatioMax - GBRatioMin ) * Scale;
	
	float GRstep = ( GRRatioMax - GRRatioMin ) / GRRange;
	float GBStep = ( GBRatioMax - GBRatioMin ) / GBRange;
	
	FILE * CTCRFile = fopen( "CTCR.h", "w" );
	FILE * CTCBFile = fopen( "CTCB.h", "w" );
	
	// 1: chip hardware design LUT. 0: ti software design LUT.
	int CIC = 1;
	
	int k = 0;
	
	double x;
	double y;
	
	double GRRatioNew;
	double GBRatioNew;
	
	float GRRatio;
	float GBRatio;
	
	int i;
	int j;
	
	char LUTCTCRFileName[ 150 ];
	char LUTCTCBFileName[ 150 ];
	
	strcpy( LUTCTCRFileName, "LUTCTCR.emf" );
	strcpy( LUTCTCBFileName, "LUTCTCB.emf" );
	
	FILE *LUTCTCRData = fopen( "LUTCTCRData.dat", "w" );
	FILE *LUTCTCBData = fopen( "LUTCTCBData.dat", "w" );
	
	if( CIC == 0 )
	{
		
		fprintf( CTCRFile, "static int CTCR[ ] =\n{\t" );
		
		fprintf( CTCBFile, "static int CTCB[ ] =\n{\t" );
		
	}
	
	for( GRRatio = GRRatioMin, i = 0; GRRatio < GBRatioMax; GRRatio += GRstep, i ++ )
		for( GBRatio = GBRatioMin, j = 0; GBRatio < GBRatioMax; GBRatio += GBStep, j ++ )
		{
			
			x = log10( GRRatio );
			y = log10( GBRatio );
			
			// New Lctc : y = -1.0163x + 0.4258
			double slope = 1 / 1.0163;
			double b = y - slope * x;
			
			GRRatioNew = ( -b + 0.4258 ) / ( slope + 1.0163 );
			GBRatioNew = -1.0163 * GRRatioNew + 0.4258;
			
			double RCoefficient = pow( 10, GRRatioNew );
			double BCoefficient = pow( 10, GBRatioNew );
			
			if( CIC == 1 )
			{
				
				fprintf( CTCRFile, "%d : begin RFactor = %d; BFactor = %d; end // GR = %1.2f, GB = %1.2f\n",
								i * ( int )GRRange + j,
								( int )( RCoefficient * 256.0 ),
								( int )( BCoefficient * 256.0 ),
								GRRatio,
								GBRatio
					);
				
				fprintf( LUTCTCRData, "%d %d\n", i * ( int )GRRange + j, ( int )( RCoefficient * 256.0 ) );
				fprintf( LUTCTCBData, "%d %d\n", i * ( int )GRRange + j, ( int )( BCoefficient * 256.0 ) );
				
			}
			else
			{
				
				if( ( k ++ ) % 8 == 0 )
				{
					
					fprintf( CTCRFile, "\n\t" );
					fprintf( CTCBFile, "\n\t" );
					
				}
				
				fprintf( CTCRFile, "%d, ", ( int )( RCoefficient * 256.0 ) );
				fprintf( CTCBFile, "%d, ", ( int )( BCoefficient * 256.0 ) );
				
			}
			
		}
	
	if( CIC == 0 )
	{
		
		fprintf( CTCRFile, "\n};\n" );
		
		fprintf( CTCBFile, "\n};\n" );
				
	}
	
	fclose( CTCRFile );
	fclose( CTCBFile );
	
	fclose( LUTCTCRData );
	fclose( LUTCTCBData );
	
	FILE *fp_domain_script;
	char command[ 200 ];
	
	// for RFactor
	fp_domain_script = fopen( "histogram.dem", "w" );
	
	fprintf( fp_domain_script, "set terminal emf\n" );
	fprintf( fp_domain_script, "set xrange [ 0 : 576 ]\n" );
	
	fprintf( fp_domain_script, "set xlabel 'Ratio Inputl'\n" );
	
	fprintf( fp_domain_script, "set ylabel 'RFactor Output'\n" );
	
	fprintf( fp_domain_script, "set grid\n" );
	fprintf( fp_domain_script, "set output '%s'\n", LUTCTCRFileName );
	fprintf( fp_domain_script, "plot 'LUTCTCRData.dat' title '%s' with lines 1 4\n", "LUTCTCRData.dat" );
	
	fclose( fp_domain_script );
	
	sprintf( command, "d:\\gnuplot\\bin\\wgnuplot.exe histogram.dem" );
	
	system( command );
	/////////////////////////////////////////
	// for BFactor
	fp_domain_script = fopen( "histogram.dem", "w" );
	
	fprintf( fp_domain_script, "set terminal emf\n" );
	fprintf( fp_domain_script, "set xrange [ 0 : 576 ]\n" );
	
	fprintf( fp_domain_script, "set xlabel 'Ratio Inputl'\n" );
	
	fprintf( fp_domain_script, "set ylabel 'BFactor Output'\n" );
	
	fprintf( fp_domain_script, "set grid\n" );
	fprintf( fp_domain_script, "set output '%s'\n", LUTCTCBFileName );
	fprintf( fp_domain_script, "plot 'LUTCTCBData.dat' title '%s' with lines 1 4\n", "LUTCTCBData.dat" );
	
	fclose( fp_domain_script );
	
	sprintf( command, "d:\\gnuplot\\bin\\wgnuplot.exe histogram.dem" );
	
	system( command );
	//////////////////////////////////////
	
	remove( "histogram.dem" );
	remove( "LUTCTCRData.dat" );
	remove( "LUTCTCBData.dat" );
	
}
//---------------------------------------------------------------------------
void __fastcall TForm1::uglyGammaCorrectionClick(TObject *Sender)
{
	
	char TableFileName[ 20 ];
	char LUTPow045FileName[ 150 ];
	
	FILE * TableFile;
	
	strcpy( LUTPow045FileName, "LUTPow045.emf" );
	
	FILE *LUTPow045Data = fopen( "LUTPow045Data.dat", "w" );
	
	// power operation
	sprintf( TableFileName, "GammaCorrection.h" );
	
	TableFile = fopen( TableFileName, "w" );
	
	int LUTPow045Value;
	
	for( int i = 0; i <= 255; i ++ )
	{
		
		LUTPow045Value = ( int )( pow( i / 256.0, 0.45 ) * 256.0 * 256.0 );
		
		fprintf( TableFile, "%d : target = %d;\n", i, LUTPow045Value );
		
		fprintf( LUTPow045Data, "%d %d\n", i, LUTPow045Value );
		
	}
	
	fclose( LUTPow045Data );
	fclose( TableFile );
	
	FILE *fp_domain_script = fopen( "histogram.dem", "w" );
	
	fprintf( fp_domain_script, "set terminal emf\n" );
	fprintf( fp_domain_script, "set xrange [ 0 : 255 ]\n" );
	
	fprintf( fp_domain_script, "set xlabel 'Input Level'\n" );
	
	fprintf( fp_domain_script, "set ylabel 'Output Level'\n" );
	
	fprintf( fp_domain_script, "set grid\n" );
	fprintf( fp_domain_script, "set output '%s'\n", LUTPow045FileName );
	fprintf( fp_domain_script, "plot 'LUTPow045Data.dat' title '%s' with lines 1 4\n", "LUTPow045Data.dat" );
	
	fclose( fp_domain_script );
	
	char command[ 200 ];
	sprintf( command, "d:\\gnuplot\\bin\\wgnuplot.exe histogram.dem" );
	
	system( command );
	
	remove( "histogram.dem" );
	remove( "LUTPow045Data.dat" );
	
}
//---------------------------------------------------------------------------
