#include <string.h>
#include "AutoLevel.h"

#define	AutoLevelLowThreshold	0.001
#define	AutoLevelHighThreshold	0.001

#define	LowLevelLimitation		20
#define	HighLevelLimitation		950

void RGBThreshold( short ** BayerPattern, int imageHeight, int imageWidth, int * LowThreshold, int * HighThreshold )
{
	
	int Histogram[ 1024 ];
	
	memset( Histogram, 0, sizeof( Histogram ) );
	
	for( int i = 0; i < imageHeight; i ++ )
		for( int j = 0; j < imageWidth; j++ )
			Histogram[ BayerPattern[ i ][ j ] ] ++;
	
	// low level
	int Accumulation = 0;
	
	while( Accumulation <= imageHeight * imageWidth * AutoLevelLowThreshold )
		Accumulation += Histogram[ ( *LowThreshold ) ++ ];
	
	if(  * LowThreshold > LowLevelLimitation )
		* LowThreshold = LowLevelLimitation;
	
	// high level
	Accumulation = 0;
	
	while( Accumulation <= imageHeight * imageWidth * AutoLevelHighThreshold )
		Accumulation += Histogram[ ( *HighThreshold ) -- ];
	
	if(  * HighThreshold < HighLevelLimitation )
		* HighThreshold = HighLevelLimitation;
	
}
