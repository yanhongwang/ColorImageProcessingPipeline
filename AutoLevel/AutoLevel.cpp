#include <string.h>
#include "AutoLevel.h"

void LThreshold( double ** Lab_buf, int imageHeight, int imageWidth, int * LLowThreshold, int * LHighThreshold )
{
	
	int LHistogram[ 101 ];
	
	memset( LHistogram, 0, sizeof( LHistogram ) );
	
	for( int i = 0; i < imageHeight; i ++ )
		for( int j = 0; j < imageWidth * 3; j += 3 )
			LHistogram[ ( int )Lab_buf[ i ][ j ] ] ++;
	
	int LAccumulation = 0;
	
	while( LAccumulation <= imageHeight * imageWidth * 0.004 )
		LAccumulation += LHistogram[ ( *LLowThreshold ) ++ ];
	
	LAccumulation = 0;
	
	*LHighThreshold = 100;
	
	while( LAccumulation <= imageHeight * imageWidth * 0.004 )
		LAccumulation += LHistogram[ ( *LHighThreshold ) -- ];
	
}

void RGBThreshold( unsigned char ** ImageDataBlock, int imageHeight, int imageWidth, int * RLowThreshold, int * GLowThreshold, int * BLowThreshold, int * RHighThreshold, int * GHighThreshold, int * BHighThreshold )
{
	
	// RGB of Gamma Correction flatting
	
	int RHistogram[ 1024 ];
	int GHistogram[ 1024 ];
	int BHistogram[ 1024 ];
	
	memset( RHistogram, 0, sizeof( RHistogram ) );
	memset( GHistogram, 0, sizeof( GHistogram ) );
	memset( BHistogram, 0, sizeof( BHistogram ) );
	
	for( int i = 0; i < imageHeight; i ++ )
		for( int j = 0; j < imageWidth * 3; j += 3 )
		{
			
			BHistogram[ ImageDataBlock[ i ][ j + 0 ] ] ++;
			GHistogram[ ImageDataBlock[ i ][ j + 1 ] ] ++;
			RHistogram[ ImageDataBlock[ i ][ j + 2 ] ] ++;
			
		}
	
	int RAccumulation = 0;
	int GAccumulation = 0;
	int BAccumulation = 0;
	
	while( RAccumulation <= imageHeight * imageWidth * 0.01 )
		RAccumulation += RHistogram[ ( *RLowThreshold ) ++ ];
	
	while( GAccumulation <= imageHeight * imageWidth * 0.01 )
		GAccumulation += GHistogram[ ( *GLowThreshold ) ++ ];
	
	while( BAccumulation <= imageHeight * imageWidth * 0.01 )
		BAccumulation += BHistogram[ ( *BLowThreshold ) ++ ];
	
	RAccumulation = 0;
	GAccumulation = 0;
	BAccumulation = 0;
	
	*RHighThreshold = 1023;
	*GHighThreshold = 1023;
	*BHighThreshold = 1023;
	
	while( RAccumulation <= imageHeight * imageWidth * 0.01 )
		RAccumulation += RHistogram[ ( *RHighThreshold ) -- ];
	
	while( GAccumulation <= imageHeight * imageWidth * 0.01 )
		GAccumulation += GHistogram[ ( *GHighThreshold ) -- ];
	
	while( BAccumulation <= imageHeight * imageWidth * 0.01 )
		BAccumulation += BHistogram[ ( *BHighThreshold ) -- ];
	
}
