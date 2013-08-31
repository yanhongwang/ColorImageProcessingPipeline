#include "Unit1.h"

void PSNR
(
	
	unsigned char **o,	// object
	unsigned char **r,	// resource
	double *PSNRG,
	double *PSNRR,
	double *PSNRB
	
)
{
	
	double diffB = 0, diffG = 0, diffR = 0;
	
	for( int i = 0; i < imageHeight; i ++ )
		for( int j = 0; j < 3 * imageWidth; j += 3 )
		{
			
			diffB += pow( o[ i ][ j + 0 ] - r[ i ][ j + 0 ], 2 );
			diffG += pow( o[ i ][ j + 1 ] - r[ i ][ j + 1 ], 2 );
			diffR += pow( o[ i ][ j + 2 ] - r[ i ][ j + 2 ], 2 );
			
		}
	
	*PSNRB = 10 * log10( ( 255. * 255. ) * ( imageHeight * imageWidth ) / diffB );
	*PSNRG = 10 * log10( ( 255. * 255. ) * ( imageHeight * imageWidth ) / diffG );
	*PSNRR = 10 * log10( ( 255. * 255. ) * ( imageHeight * imageWidth ) / diffR );
	
}
