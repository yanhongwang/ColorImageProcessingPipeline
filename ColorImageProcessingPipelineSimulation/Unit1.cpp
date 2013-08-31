
#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "RGBCIELAB.h"
#include "bmp.h"
#include "edge.h"
#include "Interpolation.h"
#include "AutoLevel.h"
#include "AutoWhiteBalance.h"
#include "Utility.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------

#define	imageWidth	2048	// target bmp width size
#define	imageHeight	1536	// target bmp height size

#define	oriWidth		2064	// original raw data width size
#define	oriHeight		1545	// original raw data height size

#define	C		1
#define	LightGamma	0.45	// gamma coefficient
#define	HeavyGamma	2.5

#define	SaturationEnhancementCoefficient_a	1.1	// saturation coefficient
#define	SaturationEnhancementCoefficient_b	1.1

// the data of hp 435 is 10 bits size
short **ImageDataBlock;		// RGB pixel storage

short **GPlane;			// divide pattern into three parts R, G, B, interpolation work
short **RPlane;
short **BPlane;
short **SensorData;			// camera three sensors input data
short **BayerPattern;		// bayer pattern storage
//unsigned char **ucImageOut;		// image data block output, every pixel has RGB colors

double **Lab_buf;			// CIELab domain pixel storage

short **edg_Bbuf;			// allocate memory to B prefiltering buffer
short ** Rembuf;			// allocate memory to Rem prefiltering buffer
//"Rem prefiltering buffer" is used to remember which pixels are edge and the value of edge is not 0
short **edg_Abuf;
short **edg_Rembuf;
short **Bil_buf;

BitMapFileHeader header1;		// bmp format header1
BitMapInfoHeader header2;		// bmp format header2

__fastcall TForm1::TForm1(TComponent* Owner): TForm(Owner)
{
	
	// 2048 x 1536
	header1.bfType = 0x4D42;			// 2 bytes
	header1.bfSize = 0x00900036;			// 4 bytes
	header1.bfReserved1 = 0x0000;			// 2 bytes
	header1.bfReserved2 = 0x0000;			// 2 bytes
	header1.bfOffBits = 0x00000036;		// 4 bytes
	
	// 40 bytes
	header2.biSize = 0x00000028;			// 4 bytes
	header2.biWidth = 0x00000800;			// 4 bytes
	header2.biHeight = 0x00000600;		// 4 bytes
	header2.biPlanes = 0x0001;			// 2 bytes
	header2.biBitCount = 0x0018;			// 2 bytes
	header2.biCompression = 0x00000000;		// 4 bytes
	header2.biSizeImage = 0x00000000;		// 4 bytes
	header2.biXPelsPerMeter = 0x00000EC4;	// 4 bytes
	header2.biYpelsPerMeter = 0x00000EC4;		// 4 bytes
	header2.biClrUsed = 0x00000000;		// 4 bytes
	header2.biClrImportant = 0x00000000;		// 4 bytes
	
	ImageDataBlock = ( short ** )calloc( imageHeight, sizeof( short * ) );
	
	SensorData = ( short ** )calloc( oriHeight, sizeof( short * ) );			// interpolation work
	BayerPattern = ( short  ** )calloc( oriHeight, sizeof( short * ) );
	GPlane = ( short ** )calloc( oriHeight, sizeof( short * ) );
	RPlane = ( short ** )calloc( oriHeight, sizeof( short * ) );
	BPlane = ( short ** )calloc( oriHeight, sizeof( short * ) );
	
	Lab_buf = ( double ** )calloc( imageHeight, sizeof( double * ) );			// CIELAB domain
	
//	ucImageOut = ( unsigned char ** )calloc( imageHeight, sizeof( unsigned char * ) );
	
	edg_Bbuf = ( short ** )calloc( imageHeight, sizeof( short * ) );			// edge work
	Rembuf = ( short ** )calloc( imageHeight, sizeof( short * ) );
	edg_Abuf = ( short ** )calloc( imageHeight, sizeof( short * ) );
	edg_Rembuf = ( short ** )calloc( imageHeight, sizeof( short * ) );
	Bil_buf = ( short ** )calloc( imageHeight, sizeof( short * ) );
	
	for( int i = 0 ; i < oriHeight ; ++ i )
	{
		// raw data storage
		SensorData[ i ] = ( short * )calloc( oriWidth, sizeof( short ) );
		
		// bayer pattern image data buffer
		BayerPattern[ i ] = ( short * )calloc( oriWidth, sizeof( short ) );
		
		// G channel of image data buffer
		GPlane[ i ] = ( short * )calloc( oriWidth, sizeof( short ) );
		
		// R channel of image data buffer
		RPlane[ i ] = ( short * )calloc( oriWidth, sizeof( short ) );
		
		// B channel of image data buffer
		BPlane[ i ] = ( short * )calloc( oriWidth, sizeof( short ) );
		
	}
	
	for( int i = 0 ; i < imageHeight; ++ i )
	{
		
		// original image data buffer
		ImageDataBlock[ i ] = ( short * )calloc( imageWidth * 3, sizeof( short ) );
		
		// Lab domain image data buffer
		Lab_buf[ i ] = ( double * )calloc( imageWidth * 3, sizeof( double ) );
		
		// result image data buffer
	//	ucImageOut[ i ] = ( unsigned char * )calloc( imageWidth * 3, sizeof( unsigned char ) );
		
		// noise filter and edge enhancement
		edg_Bbuf[ i ] = ( short * )calloc( imageWidth * 3, sizeof( short ) );
		Rembuf[ i ] = ( short * )calloc( imageWidth, sizeof( short ) );
		edg_Abuf[ i ] = ( short * )calloc( imageWidth * 3, sizeof( short ) );
		edg_Rembuf[ i ] = ( short * )calloc( imageWidth, sizeof( short ) );
		Bil_buf[ i ] = ( short * )calloc( imageWidth * 3, sizeof( short ) );
		
	}
	
	AutoLevelSwitch -> Checked = true;
	WhiteBalanceSwitch -> Checked = true;
	EdgeDetectionSwitch -> Checked = false;
	NoiseFilterSwitch -> Checked = false;
	ColorCorrectionSwitch -> Checked = true;
	EdgeEnhancementSwitch -> Checked = false;
	SaturationEnhancementSwitch -> Checked = true;
	ToneReproductionSwitch -> Checked = true;
	GammaCorrectionSwitch -> Checked = true;
	
	// CTC + Large Object is default method
	WhiteBalanceGroup -> ItemIndex = 4;
	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FileListBox1Click(TObject *Sender)
{
	
	// TabControl1Change( Sender );
	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TabControl1Change(TObject *Sender)
{
	
	switch( TabControl1 -> TabIndex )
	{
		
		case 0:
		{
			
			char PSNRFileName[ 150 ];
			
			sprintf( PSNRFileName, "%s_RMS.emf", ExtractFileName( ExtractFileDir( FileListBox1 -> FileName ) ) );
			
			if( !FileExists( PSNRFileName ) )
			{
				
			//	double PSNRArrayR[ 100 ];
			//	double PSNRArrayG[ 100 ];
			//	double PSNRArrayB[ 100 ];
				
				/*
				double PSNRG = 0;
				double PSNRR = 0;
				double PSNRB = 0;
				
				FILE *PSNRDataR = fopen( "PSNRDataR.dat" , "w" );		// benchmark file in R domain
				FILE *PSNRDataG = fopen( "PSNRDataG.dat" , "w" );
				FILE *PSNRDataB = fopen( "PSNRDataB.dat" , "w" );
				*/
				
				for( int n = 0; n < FileListBox1 -> Items -> Count; n ++ )
				{
					
					char ProcessExtension[ 50 ];
					
					strcpy( ProcessExtension, "_RAW" );	// process abbreviation
					
					char RawDataFileName[ 150 ];		// raw data file name
					
					sprintf( RawDataFileName, "%s%s", ExtractFilePath( FileListBox1 -> FileName ), FileListBox1 -> Items -> Strings[ n ].c_str() );
					
					FILE *fp = fopen( RawDataFileName, "rb" );		// image raw data file name
					
					// read raw data from raw file into memory
					for( int i = 0; i < oriHeight; i ++ )
						fread( SensorData[ i ], oriWidth, sizeof( short ), fp );
					
					fclose( fp );
					
					//////////////////////
					
					for( int i = 0; i < oriHeight; i ++ )
						for( int j = 0; j < oriWidth; j ++ )
						{
							
							// if no shift, that would be 16 bit calculation
							// fill raw data into bayer pattern memory
							if( i % 3 == 0 )
								BayerPattern[ i ][ j ] = ( SensorData[ i / 3 ][ j ] >> 6 ) & 0x03FF;
							else if( i % 3 == 1 )
								BayerPattern[ i ][ j ] = ( SensorData[ 1030 + ( i / 3 ) ][ j ] >> 6 ) & 0x03FF;
							else	// i % 3 == 2
								BayerPattern[ i ][ j ] = ( SensorData[ 515 + ( i / 3 ) ][ j ] >> 6 ) & 0x03FF;
							
						}
					
					if( AutoLevelSwitch -> Checked == true )
					{
						
						strcat( ProcessExtension, "+AL" );
						
						int LowThreshold = 0;
						
						int HighThreshold = 1023;
						
						RGBThreshold( BayerPattern, oriHeight, oriWidth, &LowThreshold, &HighThreshold );
						
						for( int i = 0; i < oriHeight; i ++ )
							for( int j = 0; j < oriWidth; j ++ )
								if( ( BayerPattern[ i ][ j ] > LowThreshold ) && ( BayerPattern[ i ][ j ] < HighThreshold ) )	
									BayerPattern[ i ][ j ] = 1024.0 * ( BayerPattern[ i ][ j ] - LowThreshold ) / ( float( HighThreshold ) - float( LowThreshold ) );
								else if( BayerPattern[ i ][ j ] <= LowThreshold )
									BayerPattern[ i ][ j ] = 0;
								else	// BayerPattern[ i ][ j ] > HighThreshold
									BayerPattern[ i ][ j ] = 1023;
						
						/*
						if( BHighThreshold - BLowThreshold > 512 )
							for( int i = 0; i < imageHeight; i ++ )
								for( int j = 0; j < imageWidth * 3; j += 3 )
									if( ( ImageDataBlock[ i ][ j + 0 ] > BLowThreshold ) && ( ImageDataBlock[ i ][ j + 0 ] < BHighThreshold ) )	
										ImageDataBlock[ i ][ j + 0 ] = 1024 * ( ImageDataBlock[ i ][ j + 0 ] - BLowThreshold ) / ( BHighThreshold - BLowThreshold );
									else if( ImageDataBlock[ i ][ j + 0 ] <= BLowThreshold )
										ImageDataBlock[ i ][ j + 0 ] = 0;
									else	// ImageDataBlock[ i ][ j + 0 ] > BHighThreshold
										ImageDataBlock[ i ][ j + 0 ] = 1023;
						else
						{
							
							int OriginalLowThreshold = BLowThreshold;
							int OriginalHighThreshold = BHighThreshold;
							
							BLowThreshold = 512 - ( OriginalHighThreshold - OriginalLowThreshold );
							
							BHighThreshold = 512 + ( OriginalHighThreshold - OriginalLowThreshold );
							
							if( OriginalLowThreshold < BLowThreshold )
							{
								
								for( int i = 0; i < imageHeight; i ++ )
									for( int j = 0; j < imageWidth * 3; j += 3 )
										if( ( ImageDataBlock[ i ][ j + 0 ] > OriginalLowThreshold ) && ( ImageDataBlock[ i ][ j + 0 ] < OriginalHighThreshold ) )	
											ImageDataBlock[ i ][ j + 0 ] = 1024 * ( ImageDataBlock[ i ][ j + 0 ] - BLowThreshold ) / ( BHighThreshold - BLowThreshold );
										else if( ImageDataBlock[ i ][ j + 0 ] <= OriginalLowThreshold )
											ImageDataBlock[ i ][ j + 0 ] = 0;
										else	// ImageDataBlock[ i ][ j + 0 ] > BHighThreshold
											ImageDataBlock[ i ][ j + 0 ] = 1023;
								
							}
						}
						*/
						
					}
					
					// bayer pattern
					// R G R G R G R G
					// G B G B G B G B
					// R G R G R G R G
					// G B G B G B G B
					// R G R G R G R G
					// G B G B G B G B
					// R G R G R G R G
					// G B G B G B G B
					
					if( WhiteBalanceSwitch -> Checked == true )
					{
						
						switch( WhiteBalanceGroup -> ItemIndex )
						{
							
							// gray world method
							case 0:
							{
								
								// append process abbreviation behind original file name
								strcat( ProcessExtension, "+WB_GW" );
								
								double scale_value_R;	// scale factor in R domain
								double scale_value_B;
								
								GrayWorld( BayerPattern, oriHeight, oriWidth, &scale_value_R, &scale_value_B );
								
								double tempValue;
								
								for( int i = 0; i < oriHeight; i ++ )
									for( int j = 0; j < oriWidth; j++ )
										if( i % 2 == 0 )
											if( j % 2 == 0 )
											{
												tempValue = BayerPattern[ i ][ j ] * scale_value_R;
												RPlane[ i ][ j ] = ( short )( tempValue < 0 ? 0 : tempValue >  1023 ? 1023 : tempValue );
											}
											else
											{
												tempValue = BayerPattern[ i ][ j ];
												GPlane[ i ][ j ] = ( short )( tempValue < 0 ? 0 : tempValue >  1023 ? 1023 : tempValue );
											}
										else
											if( j % 2 == 0 )
											{
												tempValue = BayerPattern[ i ][ j ];
												GPlane[ i ][ j ] = ( short )( tempValue < 0 ? 0 : tempValue >  1023 ? 1023 : tempValue );
											}
											else
											{
												tempValue = BayerPattern[ i ][ j ] * scale_value_B;
												BPlane[ i ][ j ] = ( short )( tempValue < 0 ? 0 : tempValue >  1023 ? 1023 : tempValue );
											}
								
								break;
								
							}
							
							// Retinex, white point method
							case 1:
							{
								
								strcat( ProcessExtension, "+WB_WP");
								
								double GainGB;
								double GainGR;
								
								Retinex( BayerPattern, oriHeight, oriWidth, &GainGB, &GainGR );
								
								double tempValue;
								
								for( int i = 0; i < oriHeight; i ++ )
									for( int j = 0; j < oriWidth; j++ )
										if( i % 2 == 0 )
											if( j % 2 == 0 )
											{
												
												tempValue = BayerPattern[ i ][ j ] * GainGR;
												RPlane[ i ][ j ] = ( short )( tempValue < 0 ? 0 : tempValue >  1023 ? 1023 : tempValue );
												
											}
											else
												GPlane[ i ][ j ] = ( short )( BayerPattern[ i ][ j ] < 0 ? 0 : BayerPattern[ i ][ j ] >  1023 ? 1023 : BayerPattern[ i ][ j ] );
										else
											if( j % 2 == 0 )
												GPlane[ i ][ j ] = ( short )( BayerPattern[ i ][ j ] < 0 ? 0 : BayerPattern[ i ][ j ] >  1023 ? 1023 : BayerPattern[ i ][ j ] );
											else
											{
												
												tempValue = BayerPattern[ i ][ j ] * GainGB;
												BPlane[ i ][ j ] = ( short )( tempValue < 0 ? 0 : tempValue >  1023 ? 1023 : tempValue );
												
											}
								
								break;
								
							}
							
							// Quadratic Mapping method, apply in gray world + white point
							case 2:
							{
								
								strcat( ProcessExtension, "+WB_QM" );
								
								double FactorRU;
								double FactorRV;
								
								double FactorBU;
								double FactorBV;
								
								QuadraticMapping( BayerPattern, imageHeight, imageWidth, &FactorRU, &FactorRV, &FactorBU, &FactorBV );
								
								double tempValue;
								
								for( int i = 0; i < oriHeight; i ++ )
									for( int j = 0; j < oriWidth; j++ )
										if( i % 2 == 0 )
											if( j % 2 == 0 )	// R channel
											{
												
												tempValue = FactorRU * BayerPattern[ i ][ j ] * BayerPattern[ i ][ j ] + FactorRV * BayerPattern[ i ][ j ];
												RPlane[ i ][ j ] = ( short )( tempValue < 0 ? 0 : tempValue >  1023 ? 1023 : tempValue );
												
											}
											else		// G channel
												GPlane[ i ][ j ] = ( short )( BayerPattern[ i ][ j ] < 0 ? 0 : BayerPattern[ i ][ j ] >  1023 ? 1023 : BayerPattern[ i ][ j ] );
										else
											if( j % 2 == 0 )	// G channel
												GPlane[ i ][ j ] = ( short )( BayerPattern[ i ][ j ] < 0 ? 0 : BayerPattern[ i ][ j ] >  1023 ? 1023 : BayerPattern[ i ][ j ] );
											else		// B channel
											{
												
												tempValue = FactorBU * BayerPattern[ i ][ j ] * BayerPattern[ i ][ j ] + FactorBV * BayerPattern[ i ][ j ];
												BPlane[ i ][ j ] = ( short )( tempValue < 0 ? 0 : tempValue >  1023 ? 1023 : tempValue );
												
											}
								
								break;
								
							}
							
							// color temperature curve method
							case 3:
							case 4:
							{
								
								double RCoefficient;
								double BCoefficient;
								
								char LargeBlockEnable;
								
								if( WhiteBalanceGroup -> ItemIndex == 3 )
								{
									
									strcat( ProcessExtension, "+WB_CTC" );
									LargeBlockEnable = false;
									
									ColorTemperatureCurve( BayerPattern, oriHeight, oriWidth, &RCoefficient, &BCoefficient, LargeBlockEnable );
									
								}
								else	// WhiteBalanceGroup -> ItemIndex == 4
								{
									
									strcat( ProcessExtension, "+WB_CTC_LargeBlock" );
									LargeBlockEnable = true;
									
									ColorTemperatureCurve( BayerPattern, imageHeight, imageWidth, &RCoefficient, &BCoefficient, LargeBlockEnable );
									
								}
								
								double tempValue;
								
								for( int i = 0; i < oriHeight; i ++ )
									for( int j = 0; j < oriWidth; j++ )
										if( i % 2 == 0 )
											if( j % 2 == 0 )
											{
												
											//	tempValue = BayerPattern[ i ][ j ] * RCoefficient;
												tempValue = BayerPattern[ i ][ j ] * RCoefficient * 1.15;
												RPlane[ i ][ j ] = ( short )( tempValue < 0 ? 0 : tempValue >  1023 ? 1023 : tempValue );
												
											}
											else
												GPlane[ i ][ j ] = ( short )( BayerPattern[ i ][ j ] < 0 ? 0 : BayerPattern[ i ][ j ] >  1023 ? 1023 : BayerPattern[ i ][ j ] );
										else
											if( j % 2 == 0 )
												GPlane[ i ][ j ] = ( short )( BayerPattern[ i ][ j ] < 0 ? 0 : BayerPattern[ i ][ j ] >  1023 ? 1023 : BayerPattern[ i ][ j ] );
											else
											{
												
											//	tempValue = BayerPattern[ i ][ j ] * BCoefficient;
												tempValue = BayerPattern[ i ][ j ] * BCoefficient / 1.15;
												BPlane[ i ][ j ] = ( short )( tempValue < 0 ? 0 : tempValue >  1023 ? 1023 : tempValue );
												
											}
								
								break;
								
							}
							
						}
						
					}
					else
						Pattern2RGBPlanes( BayerPattern, GPlane, RPlane, BPlane );	// separate all raw data into three parts, no white balance adjustment
					
					// color interpolation
					//Interpolation_2003_Sobel( BayerPattern, GPlane, RPlane, BPlane );	// sobel algorithm
					
					// bayer pattern
					// G R G R G R G R
					// B G B G B G B G
					// G R G R G R G R
					// B G B G B G B G
					// G R G R G R G R
					// B G B G B G B G
					// G R G R G R G R
					// B G B G B G B G
					
					// color interpolation
					Interpolation_2002_fuzzy( GPlane, RPlane, BPlane );
					
					for( int i = 0; i < imageHeight; i ++ )
						for( int j = 0; j < imageWidth; j ++ )
						{
							
							// merge three memory block into one memory block
							// 1545 -> 1536, 2064 -> 2048
							ImageDataBlock[ i ][ 3 * j + 0 ] = BPlane[ i + 5 ][ j + 8 ] ;
							
							ImageDataBlock[ i ][ 3 * j + 1 ] = GPlane[ i + 5 ][ j + 8 ];
							
							ImageDataBlock[ i ][ 3 * j + 2 ] = RPlane[ i + 5 ][ j + 8 ];
							
						}
					
					//////////////////////////////////////////////////////////////////////////////////////////
					
					if( EdgeDetectionSwitch -> Checked == true )
					{
						
						strcat( ProcessExtension, "+ED" );
						
						int intApre = 6;
						int intEdgeDet = 30;
						
						edge_prefiltering( imageHeight, imageWidth, ImageDataBlock, edg_Bbuf, intApre );
						
						edge_detection( imageHeight , imageWidth , edg_Bbuf , intEdgeDet , Rembuf );	// edge detection
						
					}
					
					if( NoiseFilterSwitch -> Checked == true )
					{
						
						strcat( ProcessExtension, "+NF" );
						
						int intBilSize = 1;
						int intCloseness = 10;
						int intSimilarity = 10;
						
						bilateral( imageHeight, imageWidth, ImageDataBlock, intBilSize, intCloseness, intSimilarity,  Rembuf );	// noise fileter
						
					}
					
					if( ColorCorrectionSwitch -> Checked == true )
					{
						
						strcat( ProcessExtension, "+CC" );
						
						short Rtemp;
						short Gtemp;
						short Btemp;
						
						float tempRValue;
						float tempGValue;
						float tempBValue;
						
						for( int i = 0; i < imageHeight; i ++ )
							for( int j = 0; j < imageWidth * 3; j += 3 )
							{
								
								// make three swap variables
								Rtemp = ImageDataBlock[ i ][ j + 2 ];
								Gtemp = ImageDataBlock[ i ][ j + 1 ];
								Btemp = ImageDataBlock[ i ][ j + 0 ];
								
								// constant 512 is scaling factor
								// sensor RGB to sRGB, digital camera three sensors calibration parameter
								tempRValue = Rtemp * (  860.0 / 512 ) + Gtemp * ( -253.0 / 512 ) + Btemp * (  -95.0 / 512 );	// R element
								tempGValue = Rtemp * ( -109.0 / 512 ) + Gtemp * (  928.0 / 512 ) + Btemp * ( -307.0 / 512 );	// G element
								tempBValue = Rtemp * (   20.0 / 512 ) + Gtemp * ( -290.0 / 512 ) + Btemp * (  782.0 / 512 );	// B element
								
								ImageDataBlock[ i ][ j + 2 ] = ( short )( tempRValue < 0 ? 0 : tempRValue > 1023 ? 1023 : tempRValue );
								ImageDataBlock[ i ][ j + 1 ] = ( short )( tempGValue < 0 ? 0 : tempGValue > 1023 ? 1023 : tempGValue );
								ImageDataBlock[ i ][ j + 0 ] = ( short )( tempBValue < 0 ? 0 : tempBValue > 1023 ? 1023 : tempBValue );
								
							}
						
					}
					
					if( ( EdgeEnhancementSwitch -> Checked == true ) || ( SaturationEnhancementSwitch -> Checked == true ) )
					{
						
						// sRGB to CIELAB
						for( int i = 0; i < imageHeight; i ++ )
							for( int j = 0; j < imageWidth * 3; j += 3 )
								RGB2Lab( ImageDataBlock[ i ][ j + 0 ], ImageDataBlock[ i ][ j + 1 ], ImageDataBlock[ i ][ j + 2 ],
									&Lab_buf[ i ][ j + 0 ], &Lab_buf[ i ][ j + 1 ], &Lab_buf[ i ][ j + 2 ] );
						
					}
					
					if( EdgeEnhancementSwitch -> Checked == true )	// edge enhancement in L domain
					{
						
						strcat( ProcessExtension, "+EE" );
						
						edge_enhancement( imageHeight , imageWidth , Lab_buf , Rembuf );
						
					}
					
					if( SaturationEnhancementSwitch -> Checked == true )
					{
						
						strcat( ProcessExtension, "+SE" );
						
						for( int i = 0; i < imageHeight; i ++ )
							for( int j = 0; j < imageWidth * 3; j += 3 )
							{
								
								Lab_buf[ i ][ j + 1 ] *= SaturationEnhancementCoefficient_a;	// color saturation enhancement on a domain
								Lab_buf[ i ][ j + 2 ] *= SaturationEnhancementCoefficient_b;	// color saturation enhancement on b domain
								
							}
						
					}
					
					if( ( EdgeEnhancementSwitch -> Checked == true ) || ( SaturationEnhancementSwitch -> Checked == true ) )
					{
						
						// CIELAB to sRGB
						for( int i = 0; i < imageHeight; i ++ )
							for( int j = 0; j < imageWidth * 3; j += 3 )
								Lab2RGB( Lab_buf[ i ][ j + 0 ], Lab_buf[ i ][ j + 1 ], Lab_buf[ i ][ j + 2 ],
									&ImageDataBlock[ i ][ j + 0 ], &ImageDataBlock[ i ][ j + 1 ], &ImageDataBlock[ i ][ j + 2 ] );
						
					}
					
					if( ToneReproductionSwitch -> Checked == true )
					{
						
						strcat( ProcessExtension, "+TR" );
						
						float Lmin = 0, L1x = 5, L2x = 10, L3x = 50;
						float Lmax = 100, L1y = 10, L2y = 17.5, L3y = 55;
						float a1 = ( L1y - Lmin ) / ( L1x - Lmin ), a2 = ( L2y - L1y ) / ( L2x - L1x ), a3 = ( L3y - L2y ) / ( L3x - L2x ), a4 = ( Lmax - L3y ) / ( Lmax - L3x);
						float b1 = ( L1y - ( L1x * a1)), b2 = ( L1y - ( L1x * a2 )), b3 = ( L3y - ( L3x * a3 )), b4 = ( L3y - ( L3x * a4 ));
						
						for( int i = 0; i < imageHeight; i ++ )
							for(int j = 0; j < imageWidth * 3; j += 3)
							{
								
								if( Lab_buf[ i ][ j + 0 ] <= L1x )
								{
									ImageDataBlock[ i ][ j + 2 ] = ( ImageDataBlock[ i ][ j + 2 ] ) * a1 + b1;
									ImageDataBlock[ i ][ j + 1 ] = ( ImageDataBlock[ i ][ j + 1 ] ) * a1 + b1;
									ImageDataBlock[ i ][ j + 0 ] = ( ImageDataBlock[ i ][ j + 0 ] ) * a1 + b1;
								}
								else if( L1x < Lab_buf[ i ][ j + 0 ] && Lab_buf[ i ][ j + 0 ] <= L2x)
								{
									ImageDataBlock[ i ][ j + 2 ] = ( ImageDataBlock[ i ][ j + 2 ] ) * a2 + b2;
									ImageDataBlock[ i ][ j + 1 ] = ( ImageDataBlock[ i ][ j + 1 ] ) * a2 + b2;
									ImageDataBlock[ i ][ j + 0 ] = ( ImageDataBlock[ i ][ j + 0 ] ) * a2 + b2;
								}
								else if( L2x < Lab_buf[ i ][ j + 0 ] && Lab_buf[ i ][ j + 0 ] <= L3x)
								{
									ImageDataBlock[ i ][ j + 2 ] = ( ImageDataBlock[ i ][ j + 2 ] ) * a3 + b3;
									ImageDataBlock[ i ][ j + 1 ] = ( ImageDataBlock[ i ][ j + 1 ] ) * a3 + b3;
									ImageDataBlock[ i ][ j + 0 ] = ( ImageDataBlock[ i ][ j + 0 ] ) * a3 + b3;
								}
								else
								{
									ImageDataBlock[ i ][ j + 2 ] = ( ImageDataBlock[ i ][ j + 2 ] ) * a4 + b4;
									ImageDataBlock[ i ][ j + 1 ] = ( ImageDataBlock[ i ][ j + 1 ] ) * a4 + b4;
									ImageDataBlock[ i ][ j + 0 ] = ( ImageDataBlock[ i ][ j + 0 ] ) * a4 + b4;
								} 
								
							}
						
					}
					
					if( GammaCorrectionSwitch -> Checked == true )
					{
						
						strcat( ProcessExtension, "+GC" );
						
						float EndPointX = 20;
						//float EndPointX = 0;
						float EndPointY = C * pow( EndPointX / 1024.0, LightGamma ) * 1024.0;
						
						float slope = EndPointY / EndPointX;
						
						for( int i = 0; i < imageHeight; i ++ )
							for( int j = 0; j < imageWidth * 3; j += 3 )
							{
								
								if( ImageDataBlock[ i ][ j + 0 ] >= EndPointX )	// B element
									ImageDataBlock[ i ][ j + 0 ] = C * pow( ImageDataBlock[ i ][ j + 0 ] / 1024.0, LightGamma ) * 1024.0;
								else
									ImageDataBlock[ i ][ j + 0 ] *= slope;
								
								if( ImageDataBlock[ i ][ j + 1 ] >= EndPointX )	// G element
									ImageDataBlock[ i ][ j + 1 ] = C * pow( ImageDataBlock[ i ][ j + 1 ] / 1024.0, LightGamma ) * 1024.0;
								else
									ImageDataBlock[ i ][ j + 1 ] *= slope;
								
								if( ImageDataBlock[ i ][ j + 2 ] >= EndPointX )	// R element
									ImageDataBlock[ i ][ j + 2 ] = C * pow( ImageDataBlock[ i ][ j + 2 ] / 1024.0, LightGamma ) * 1024.0;
								else
									ImageDataBlock[ i ][ j + 2 ] *= slope;
								
							}
							
					}
					
					/////////////////////////////////////////////////////////////////////////////////
					/*
					char OriginalFileName[ 150 ];
					
					strcpy( OriginalFileName, FileListBox1 -> Items -> Strings[ n ].c_str() );
					
					sprintf( strrchr( OriginalFileName, '.' ), ".bmp" );
					
					fp = fopen( OriginalFileName, "rb" );
					
					fread( &header1, 14, 1, fp );	// dumb code
					
					fread( &header2, 40, 1, fp );	// dumb code
					
					for( int i = 0; i < imageHeight; i ++ )
						fread( ucImageOut[ i ], 3 * imageWidth, 1, fp );
					
					fclose( fp );
					*/
					/////////////////////////////////////////////////////////////////////////////////
					
					char ResultFileName[ 150 ];
					
					sprintf( ResultFileName, "%s%s", ExtractFilePath( FileListBox1 -> FileName ), FileListBox1 -> Items -> Strings[ n ].c_str() );
					
					strcat( ProcessExtension, ".BMP" );
					
					sprintf( strrchr( ResultFileName, '.' ), ProcessExtension );	// result image file name
					
					fp = fopen( ResultFileName, "wb" );
					
					fwrite( &header1.bfType, 2, 1, fp );
					fwrite( &header1.bfSize, 4, 1, fp );
					fwrite( &header1.bfReserved1, 2, 1, fp );
					fwrite( &header1.bfReserved2, 2, 1, fp );
					fwrite( &header1.bfOffBits, 4, 1, fp );
					
				//	fwrite( &header1, 14, 1, fp );
					
					fwrite( &header2, 40, 1, fp );
					
					unsigned char RGBValue;
					
					for( int i = imageHeight - 1; i >= 0; i -- )
						for( int j = 0; j < imageWidth * 3; j ++ )
						{
							
							// due to our bmp is 8 bit size
							ImageDataBlock[ i ][ j ] = ImageDataBlock[ i ][ j ] >> 2;
							RGBValue = ( unsigned char )( ImageDataBlock[ i ][ j ] < 0 ? 0 : ImageDataBlock[ i ][ j ] > 255 ? 255 : ImageDataBlock[ i ][ j ] );
							fwrite( &RGBValue, 1, 1, fp );
							
						}
					
					fclose( fp );
					
					/*
					// calculate processed image and ideal image difference
					PSNR( ( unsigned char ** )ImageDataBlock, ucImageOut, &PSNRG, &PSNRR, &PSNRB );
					
					fprintf( PSNRDataR, "%d %f\n", n, PSNRR );
					
					fprintf( PSNRDataG, "%d %f\n", n, PSNRG );
					
					fprintf( PSNRDataB, "%d %f\n", n, PSNRB );
					*/
					
				//	PSNRArrayR[ n ] = PSNRR;
					
				//	PSNRArrayG[ n ] = PSNRG;
					
				//	PSNRArrayB[ n ] = PSNRB;
					
				}
				
				/*
				fclose( PSNRDataR );
				fclose( PSNRDataG );
				fclose( PSNRDataB );
				*/
				
				//exchange_sort( PSNRArrayR, FileListBox1 -> Items -> Count );
				
				//exchange_sort( PSNRArrayG, FileListBox1 -> Items -> Count );
				
				//exchange_sort( PSNRArrayB, FileListBox1 -> Items -> Count );
				
				//FILE *PSNRFile = fopen( "PSNRFile.dat", "w" );
				
				//for( int i = 0; i < FileListBox1 -> Items -> Count; i ++ )
				//	fprintf( PSNRFile, "%d %f\n", i, PSNRArray[ i ] );
				
				//fclose( PSNRFile );
				
				/*
				FILE *PSNRScript = fopen( "PSNRScript.dem" , "w" );
				
				fprintf( PSNRScript, "set terminal emf color\n" );
				fprintf( PSNRScript, "set xrange [ 0 : %d ]\n", FileListBox1 -> Items -> Count );
				
				fprintf( PSNRScript, "set xlabel 'Picture'\n" );
				fprintf( PSNRScript, "set ylabel 'PSNR'\n" );
				
				fprintf( PSNRScript, "set grid\n" );
				
				fprintf( PSNRScript, "set title 'HP 435 raw data image manipulation result'\n" );
				fprintf( PSNRScript, "set ticslevel 0\n" );
				
				fprintf( PSNRScript, "set output '%s'\n", PSNRFileName );
				
				fprintf( PSNRScript, "plot 'PSNRDataR.dat' title 'R of PSNR' with linespoints 1 2,\
						'PSNRDataG.dat' title 'G of PSNR' with linespoints 2 3,\
						'PSNRDataB.dat' title 'B of PSNR' with linespoints 3 4\n"
					);
				
				fclose( PSNRScript );
				
				char command[ 200 ];
				sprintf( command, "%s\\gnuplot\\bin\\wgnuplot.exe PSNRScript.dem", ExtractFileDrive( FileListBox1 -> FileName ) );
				
				system( command );
				
				remove( "PSNRDataR.dat" );
				remove( "PSNRDataG.dat" );
				remove( "PSNRDataB.dat" );
				remove( "PSNRScript.dem" );
				*/
			}
			
			/*
			Image4 -> Picture -> LoadFromFile( PSNRFileName );
			TabControl1->Width=Image4->Picture->Width;
			TabControl1->Height=Image4->Picture->Height;
			*/
			
			break;
			
		}
		
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::WhiteBalanceSwitchClick(TObject *Sender)
{
	
	if( WhiteBalanceSwitch -> Checked == true )
		WhiteBalanceGroup -> Enabled = true;
	else
		WhiteBalanceGroup -> Enabled = false;
	
}
//---------------------------------------------------------------------------


void __fastcall TForm1::RunOneClick(TObject *Sender)
{
	
	TabControl1Change( Sender );
	
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RunAllClick(TObject *Sender)
{
	
	char DirectoryName[ 150 ];
	
	char CommandString[ 150 ];
	
	///////////////////////////////////////////////////////////////
	// RAW
	sprintf( DirectoryName, "RAW" );
	AutoLevelSwitch -> Checked = false;
	WhiteBalanceSwitch -> Checked = false;
	EdgeDetectionSwitch -> Checked = false;
	NoiseFilterSwitch -> Checked = false;
	ColorCorrectionSwitch -> Checked = false;
	EdgeEnhancementSwitch -> Checked = false;
	SaturationEnhancementSwitch -> Checked = false;
	ToneReproductionSwitch -> Checked = false;
	GammaCorrectionSwitch -> Checked = false;
	
	// CTC + Large Object is default method
	// WhiteBalanceGroup -> ItemIndex = 4;
	
	CreateDir( DirectoryName );
	
	TabControl1Change( Sender );
	
	sprintf( CommandString, "move *.bmp %s", DirectoryName );
	
	system( CommandString );
	
	//////////////////////////////////////////////////////////////
	// RAW+AL
	sprintf( DirectoryName, "RAW" );
	AutoLevelSwitch -> Checked = true; strcat( DirectoryName, "+AL" );
	WhiteBalanceSwitch -> Checked = false;
	EdgeDetectionSwitch -> Checked = false;
	NoiseFilterSwitch -> Checked = false;
	ColorCorrectionSwitch -> Checked = false;
	EdgeEnhancementSwitch -> Checked = false;
	SaturationEnhancementSwitch -> Checked = false;
	ToneReproductionSwitch -> Checked = false;
	GammaCorrectionSwitch -> Checked = false;
	
	// CTC + Large Object is default method
	// WhiteBalanceGroup -> ItemIndex = 4;
	
	CreateDir( DirectoryName );
	
	TabControl1Change( Sender );
	
	sprintf( CommandString, "move *.bmp %s", DirectoryName );
	
	system( CommandString );
	
	//////////////////////////////////////////////////////////////
	// RAW+AL+WB_CTC_LargeBlock
	sprintf( DirectoryName, "RAW" );
	AutoLevelSwitch -> Checked = true; strcat( DirectoryName, "+AL" );
	WhiteBalanceSwitch -> Checked = true; strcat( DirectoryName, "+WB_CTC_LargeBlock" );
	EdgeDetectionSwitch -> Checked = false;
	NoiseFilterSwitch -> Checked = false;
	ColorCorrectionSwitch -> Checked = false;
	EdgeEnhancementSwitch -> Checked = false;
	SaturationEnhancementSwitch -> Checked = false;
	ToneReproductionSwitch -> Checked = false;
	GammaCorrectionSwitch -> Checked = false;
	
	// CTC + Large Object is default method
	WhiteBalanceGroup -> ItemIndex = 4;
	
	CreateDir( DirectoryName );
	
	TabControl1Change( Sender );
	
	sprintf( CommandString, "move *.bmp %s", DirectoryName );
	
	system( CommandString );
	
	//////////////////////////////////////////////////////////////
	// RAW+AL+WB+CC
	sprintf( DirectoryName, "RAW" );
	AutoLevelSwitch -> Checked = true; strcat( DirectoryName, "+AL" );
	WhiteBalanceSwitch -> Checked = true; strcat( DirectoryName, "+WB_CTC_LargeBlock" );
	EdgeDetectionSwitch -> Checked = false;
	NoiseFilterSwitch -> Checked = false;
	ColorCorrectionSwitch -> Checked = true; strcat( DirectoryName, "+CC" );
	EdgeEnhancementSwitch -> Checked = false;
	SaturationEnhancementSwitch -> Checked = false;
	ToneReproductionSwitch -> Checked = false;
	GammaCorrectionSwitch -> Checked = false;
	
	// CTC + Large Object is default method
	WhiteBalanceGroup -> ItemIndex = 4;
	
	CreateDir( DirectoryName );
	
	TabControl1Change( Sender );
	
	sprintf( CommandString, "move *.bmp %s", DirectoryName );
	
	system( CommandString );
	
	//////////////////////////////////////////////////////////////
	// RAW+AL+WB+CC+SE
	sprintf( DirectoryName, "RAW" );
	AutoLevelSwitch -> Checked = true; strcat( DirectoryName, "+AL" );
	WhiteBalanceSwitch -> Checked = true; strcat( DirectoryName, "+WB_CTC_LargeBlock" );
	EdgeDetectionSwitch -> Checked = false;
	NoiseFilterSwitch -> Checked = false;
	ColorCorrectionSwitch -> Checked = true; strcat( DirectoryName, "+CC" );
	EdgeEnhancementSwitch -> Checked = false;
	SaturationEnhancementSwitch -> Checked = true; strcat( DirectoryName, "+SE" );
	ToneReproductionSwitch -> Checked = false;
	GammaCorrectionSwitch -> Checked = false;
	
	// CTC + Large Object is default method
	WhiteBalanceGroup -> ItemIndex = 4;
	
	CreateDir( DirectoryName );
	
	TabControl1Change( Sender );
	
	sprintf( CommandString, "move *.bmp %s", DirectoryName );
	
	system( CommandString );
	
	//////////////////////////////////////////////////////////////
	// RAW+AL+WB+CC+SE+TR
	sprintf( DirectoryName, "RAW" );
	AutoLevelSwitch -> Checked = true; strcat( DirectoryName, "+AL" );
	WhiteBalanceSwitch -> Checked = true; strcat( DirectoryName, "+WB_CTC_LargeBlock" );
	EdgeDetectionSwitch -> Checked = false;
	NoiseFilterSwitch -> Checked = false;
	ColorCorrectionSwitch -> Checked = true; strcat( DirectoryName, "+CC" );
	EdgeEnhancementSwitch -> Checked = false;
	SaturationEnhancementSwitch -> Checked = true; strcat( DirectoryName, "+SE" );
	ToneReproductionSwitch -> Checked = true; strcat( DirectoryName, "+TR" );
	GammaCorrectionSwitch -> Checked = false;
	
	// CTC + Large Object is default method
	WhiteBalanceGroup -> ItemIndex = 4;
	
	CreateDir( DirectoryName );
	
	TabControl1Change( Sender );
	
	sprintf( CommandString, "move *.bmp %s", DirectoryName );
	
	system( CommandString );
	
	//////////////////////////////////////////////////////////////
	// RAW+AL+WB+CC+SE+TR+GC
	sprintf( DirectoryName, "RAW" );
	AutoLevelSwitch -> Checked = true; strcat( DirectoryName, "+AL" );
	WhiteBalanceSwitch -> Checked = true; strcat( DirectoryName, "+WB_CTC_LargeBlock" );
	EdgeDetectionSwitch -> Checked = false;
	NoiseFilterSwitch -> Checked = false;
	ColorCorrectionSwitch -> Checked = true; strcat( DirectoryName, "+CC" );
	EdgeEnhancementSwitch -> Checked = false;
	SaturationEnhancementSwitch -> Checked = true; strcat( DirectoryName, "+SE" );
	ToneReproductionSwitch -> Checked = true; strcat( DirectoryName, "+TR" );
	GammaCorrectionSwitch -> Checked = true; strcat( DirectoryName, "+GC" );
	
	// CTC + Large Object is default method
	WhiteBalanceGroup -> ItemIndex = 4;
	
	CreateDir( DirectoryName );
	
	TabControl1Change( Sender );
	
	sprintf( CommandString, "move *.bmp %s", DirectoryName );
	
	system( CommandString );
	
}
//---------------------------------------------------------------------------

