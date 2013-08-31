#include <math.h>
#include "AutoWhiteBalance.h"

void ColorTemperatureCurve( short ** BayerPattern, int imageHeight, int imageWidth, double * RCoefficient, double * BCoefficient, char LargeBlockEnable )
{
	
	double GRRatio = 0;
	double GBRatio = 0;
	
	if( LargeBlockEnable == true )
	{
		double RightSumB = 0;
		double RightSumG = 0;
		double RightSumR = 0;
		
		double LeftSumB = 0;
		double LeftSumG = 0;
		double LeftSumR = 0;
		
		double deviation;
		
		double WeightEdgeTotalB = 0;
		double WeightEdgeTotalG = 0;
		double WeightEdgeTotalR = 0;
		
		// 0: not edge, 1: edge
		char EdgeFlag = 0;
		
		//FILE *WeightEdgeFile = fopen( "WeightEdge.dat", "w" );
		//FILE *test = fopen( "test.dat", "w" );
		for( int i = 0; i < imageHeight / 16; LeftSumB = 0, LeftSumG = 0, LeftSumR = 0, i ++ )
		{
			for( int j = 0; j < imageWidth / 16; RightSumB = 0, RightSumG = 0, RightSumR = 0, j ++ )
			{
				
				for( int k = i * 16; k <= i * 16 + 15; k ++ )
					for( int m = j * 16; m <= j * 16 + 15; m ++ )
					{
						
						// bayer pattern arrangement
						//     0  1  2  3  4  5  6  7 8  9 10 11
						// 0  R G R G R G R G R G R G R G 
						// 1  G B G B G B G  B G B G B G B 
						// 2  R G R G R G R G R G R G R G 
						// 3  G B G B G B G  B G B G B G B 
						// 4  R G R G R G R G R G R G R G 
						// 5  G B G B G B G  B G B G B G B 
						// 6  R G R G R G R G R G R G R G 
						// 7  G B G B G B G  B G B G B G B 
						// 8  R G R G R G R G R G R G R G 
						
						// 1545 -> 1536, 2064 -> 2048
						if( k % 2 == 0 )
							if( m % 2 == 0 )
								//RightSumR += BayerPattern[ k + 5 ][ m + 8 ];
								RightSumG += BayerPattern[ k + 5 ][ m + 8 ];
							else
								//RightSumG += BayerPattern[ k + 5 ][ m + 8 ];
								RightSumB += BayerPattern[ k + 5 ][ m + 8 ];
						else	// k % 2 == 1
							if( m % 2 == 0 )
								//RightSumG += BayerPattern[ k + 5 ][ m + 8 ];
								RightSumR += BayerPattern[ k + 5 ][ m + 8 ];
							else
								//RightSumB += BayerPattern[ k + 5 ][ m + 8 ];
								RightSumG += BayerPattern[ k + 5 ][ m + 8 ];
						
					//	fprintf( test, "%d %d\n", k + 5, m + 8 );
					}
					
				//fprintf( test, "...........................................................................................\n" );
				
				RightSumR /= ( ( 16 * 16 ) * 0.25 );
				RightSumG /= ( ( 16 * 16 ) * 0.5 );
				RightSumB /= ( ( 16 * 16 ) * 0.25 );
				
				deviation = ( pow( RightSumR - LeftSumR, 2 ) + pow( RightSumG - LeftSumG, 2 ) + pow( RightSumB - LeftSumB, 2 ) ) 
				/ ( pow( ( RightSumR + LeftSumR ) / 2, 2 ) + pow( ( RightSumG + LeftSumG ) / 2, 2 ) + pow( ( RightSumB + LeftSumB ) / 2, 2 ) );
				
				//fprintf( WeightEdgeFile, "%f ", deviation );
				
				if( j != 0 && deviation >= 0.1 )	// edge
				{
					
					WeightEdgeTotalB += RightSumB;
					WeightEdgeTotalG += RightSumG;
					WeightEdgeTotalR += RightSumR;
					
					if( 0 == EdgeFlag )	// the edge is not yet calculated last time
					{
						
						WeightEdgeTotalB += LeftSumB;
						WeightEdgeTotalG += LeftSumG;
						WeightEdgeTotalR += LeftSumR;
						
					}
					
					EdgeFlag = 1;
					
				}
				else	// not edge
					EdgeFlag = 0;
				
				LeftSumB = RightSumB;
				LeftSumG = RightSumG;
				LeftSumR = RightSumR;
				
			}
			
			//fprintf( WeightEdgeFile, "\n" );
			
		}
		
		//fclose( test );
		//fclose( WeightEdgeFile );
		
		GBRatio = WeightEdgeTotalG / WeightEdgeTotalB;
		GRRatio = WeightEdgeTotalG / WeightEdgeTotalR;
		
	}
	else	// LargeObjectEnable == false
	{
		
		// no edge extraction
		double RTotal = 0;
		double GTotal = 0;
		double BTotal = 0;
		
		for( int i = 0; i < imageHeight; i ++ )
			for( int j = 0; j < imageWidth; j += 3 )
			{
				
				if( i % 2 == 0 )
					if( j % 2 == 0 )
						RTotal += BayerPattern[ i ][ j ];
					else
						GTotal += BayerPattern[ i ][ j ];
				else	// i % 2 == 1
					if( j % 2 == 0 )
						GTotal += BayerPattern[ i ][ j ];
					else
						BTotal += BayerPattern[ i ][ j ];
				
			}
		
		GBRatio = ( GTotal * 0.5 ) / BTotal;
		GRRatio = ( GTotal * 0.5 ) / RTotal;
		
	}
	
	GBRatio = log10( GBRatio );
	GRRatio = log10( GRRatio );
	//GRRatio = -1;
	//GBRatio = -2;
	double x = GRRatio;
	double y = GBRatio;
	
	// New Lctc : y = -1.0163x + 0.4258
	double slope = 1 / 1.0163;
	double b = y - slope * x;
	
	GRRatio = ( -b + 0.4258 ) / ( slope + 1.0163 );
	GBRatio = -1.0163 * GRRatio + 0.4258;
	
	*RCoefficient = pow( 10, GRRatio );
	*BCoefficient = pow( 10, GBRatio );
	
//	*RCoefficient *= ( 37.47 / 39 );
//	*BCoefficient *= ( 37.47 / 46.35 );
	
}

void GrayWorld( short ** BayerPattern, int imageHeight, int imageWidth, double * scale_value_R, double * scale_value_B )
{
	
	double RTotal = 0, GTotal = 0, BTotal = 0;
	
        	for( int i = 0; i < imageHeight; i ++ )
		for( int j = 0; j < imageWidth; j ++ )
			if( i % 2 == 0 )
				if( j % 2 == 0 )
					RTotal += BayerPattern[ i ][ j ];
				else
					GTotal += BayerPattern[ i ][ j ];
			else	// i % 2 == 1
				if( j % 2 == 0 )
					GTotal += BayerPattern[ i ][ j ];
				else
					BTotal += BayerPattern[ i ][ j ];
	
	double average_R = RTotal / ( imageHeight * imageWidth * 0.25 );
	double average_G = GTotal / ( imageHeight * imageWidth * 0.5 );
	double average_B = BTotal / ( imageHeight * imageWidth * 0.25 );
	
	*scale_value_R = average_G / average_R;
	*scale_value_B = average_G / average_B;
	
}

void Retinex( short ** BayerPattern, int imageHeight, int imageWidth, double * GainGB, double * GainGR )
{
	
	double MaxR = 0;
	double MaxG = 0;
	double MaxB = 0;
	
	for( int i = 0; i < imageHeight; i ++ )
		for( int j = 0; j < imageWidth; j ++ )
			if( i % 2 == 0 )
			{
				if( j % 2 == 0 )
				{
					if( MaxR < BayerPattern[ i ][ j ] )
						MaxR = BayerPattern[ i ][ j ];
				}
				else
				{
					if( MaxG < BayerPattern[ i ][ j ] )
						MaxG = BayerPattern[ i ][ j ];
				}
			}
			else	// i % 2 == 1
			{
				if( j % 2 == 0 )
				{
					if( MaxG < BayerPattern[ i ][ j ] )
						MaxG = BayerPattern[ i ][ j ];
				}
				else
				{
					if( MaxB < BayerPattern[ i ][ j ] )
						MaxB = BayerPattern[ i ][ j ];
				}
			}
	
	*GainGB = MaxG / MaxB;
	
	*GainGR = MaxG / MaxR;
	
}

void QuadraticMapping( short ** BayerPattern, int imageHeight, int imageWidth, double * FactorRU, double * FactorRV, double * FactorBU, double * FactorBV )
{
	
	// for gray world
	double total_R = 0;
	double total_G = 0;
	double total_B = 0;
	
	double total_R_square = 0;
	double total_B_square = 0;
	
	// for retinex
	double max_r = 0;
	double max_g = 0;
	double max_b = 0;
	
	for( int i = 0; i < imageHeight; i ++ )
		for( int j = 0; j < imageWidth; j ++ )
		{
			
			if( i % 2 == 0 )
			{
				if( j % 2 == 0 )	// R channel
				{
					// for gray world
					total_R += BayerPattern[ i ][ j ];
					total_R_square += BayerPattern[ i ][ j ] * BayerPattern[ i ][ j ];
					
					// for retinex
					if( max_r < BayerPattern[ i ][ j ] )
						max_r = BayerPattern[ i ][ j ];
			
				}
				else		// G channel
				{
					
					// for gray world
					total_G += BayerPattern[ i ][ j ];
					
					// for retinex
					if( max_g < BayerPattern[ i ][ j ] )
						max_g = BayerPattern[ i ][ j ];
					
				}
			}
			else	// i % 2 == 1
			{
				if( j % 2 == 0 )	// G channel
				{
					
					// for gray world
					total_G += BayerPattern[ i ][ j ];
					
					// for retinex
					if( max_g < BayerPattern[ i ][ j ] )
						max_g = BayerPattern[ i ][ j ];
					
				}
				else		// B channel
				{
					// for gray world
					total_B += BayerPattern[ i ][ j ];
					total_B_square += BayerPattern[ i ][ j ] * BayerPattern[ i ][ j ];
					
					// for retinex
					if( max_b < BayerPattern[ i ][ j ] )
						max_b = BayerPattern[ i ][ j ];
					
				}
			}
		}
	
	double max_r_square = max_r * max_r;
	
	double max_b_square = max_b * max_b;
	
	// R and B element only occupied 25% individually in bayer pattern, G element occupied 50% in bayer pattern
	// make amount of three element the same, we adjust G element
	total_G /= 2;
	
	// au + bv = x
	// cu + dv = y
	
	// adu + bdv = dx
	// cbu + dbv = by
	// ( ad - cb )u = dx - by
	// u = ( dx - by ) / ( ad - cb )
	
	// acu + bcv = cx
	// cau + dav = ay
	// ( bc - da )v = cx - ay
	// v = ( cx - ay ) / ( bc - da )
	
	// R channel
	// u = ( dx - by ) / ( ad - cb )
	* FactorRU = ( max_r * total_G - total_R * max_g ) / ( total_R_square * max_r - max_r_square * total_R );
	// R channel
	// v = ( cx - ay ) / ( bc - da )
	* FactorRV = ( max_r_square * total_G - total_R_square * max_g ) / ( total_R * max_r_square - max_r * total_R_square );
	
	// B channel
	// u = ( dx - by ) / ( ad - cb )
	* FactorBU = ( max_b * total_G - total_B * max_g ) / ( total_B_square * max_b - max_b_square * total_B );
	// B channel
	// v = ( cx - ay ) / ( bc - da )
	* FactorBV = ( max_b_square * total_G - total_B_square * max_g ) / ( total_B * max_b_square - max_b * total_B_square );
	
}