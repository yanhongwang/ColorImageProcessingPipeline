#include <string.h>
#include "BlackLevelSubtraction.h"

void BlackLevelThreshold( unsigned char ** ImageDataBlock, int imageHeight, int imageWidth, int * RBlackLevelThreshold, int * GBlackLevelThreshold, int * BBlackLevelThreshold )
{
	
	int RHistogram[ 1024 ];
	int GHistogram[ 1024 ];
	int BHistogram[ 1024 ];
	
	memset( RHistogram, 0, sizeof( RHistogram ) );
	memset( GHistogram, 0, sizeof( GHistogram ) );
	memset( BHistogram, 0, sizeof( BHistogram ) );
	
	for( int i = 0; i < imageHeight; i ++ )
		for( int j = 0; j < imageWidth * 3; j += 3  )
		{	
			
			RHistogram[ ImageDataBlock[ i ][ j + 2 ] ] ++;
			GHistogram[ ImageDataBlock[ i ][ j + 1 ] ] ++;
			BHistogram[ ImageDataBlock[ i ][ j + 0 ] ] ++;
			
		}
	
	int RAccumulation = 0;
	int GAccumulation = 0;
	int BAccumulation = 0;
	
	while( RAccumulation <= imageHeight * imageWidth * BlackLevelRatio )
		RAccumulation += RHistogram[ ( *RBlackLevelThreshold ) ++ ];
	
	while( GAccumulation <= imageHeight * imageWidth * BlackLevelRatio )
		GAccumulation += GHistogram[ ( *GBlackLevelThreshold ) ++ ];
	
	while( BAccumulation <= imageHeight * imageWidth * BlackLevelRatio )
		BAccumulation += BHistogram[ ( *BBlackLevelThreshold ) ++ ];
	
	if(  * RBlackLevelThreshold > BlackLevelLimitation )
		 * RBlackLevelThreshold = BlackLevelLimitation;
	
	if(  * GBlackLevelThreshold > BlackLevelLimitation )
		 * GBlackLevelThreshold = BlackLevelLimitation;
	
	if(  * BBlackLevelThreshold > BlackLevelLimitation )
		 * BBlackLevelThreshold = BlackLevelLimitation;
	
}
