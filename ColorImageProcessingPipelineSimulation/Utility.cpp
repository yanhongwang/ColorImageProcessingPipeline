#include "Unit1.h"
#include "Utility.h"

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
	
	*PSNRB = 10 * log10( ( 255. * 255. ) / ( diffB / ( imageHeight * imageWidth ) ) );
	*PSNRG = 10 * log10( ( 255. * 255. ) / ( diffG / ( imageHeight * imageWidth ) ) );
	*PSNRR = 10 * log10( ( 255. * 255. ) / ( diffR / ( imageHeight * imageWidth ) ) );
	
}

void swap_by_addr(double *v1, double *v2)
{
	double temp;
	temp = *v1;
	*v1 = *v2;
	*v2 = temp;
}

void exchange_sort(double A[], int len)
{
	
	for( int r = 0; r <= len - 2; r ++ )
		for( int s = r + 1; s <= len - 1; s ++ )
			if( A[ r ] > A[ s ] )
				swap_by_addr( &A[ r ], &A[ s ] );
	
}
