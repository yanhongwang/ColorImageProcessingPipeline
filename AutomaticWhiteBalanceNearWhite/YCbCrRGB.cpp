#include "YCbCrRGB.h"

void RGB2YCbCr( unsigned char B, unsigned char G, unsigned char R, unsigned char * Y, unsigned char * Cb, unsigned char * Cr )
{
	
	double tempY;
	double tempCb;
	double tempCr;
	
	tempY = 0.299 * R + 0.587 * G + 0.114 * B;
	tempCb = -0.1687 * R - 0.3313 * G + 0.5 * B + 128;
	tempCr = 0.5 * R - 0.4187 * G - 0.0813 * B + 128; 
	
	* Y = ( unsigned char )( tempY < 0 ? 0 : tempY > 255 ? 255 : tempY );
	* Cb = ( unsigned char )( tempCb < 0 ? 0 : tempCb > 255 ? 255 : tempCb );
	* Cr = ( unsigned char )( tempCr < 0 ? 0 : tempCr > 255 ? 255 : tempCr );
	
}

void YCbCr2RGB( unsigned char Y, unsigned char Cb, unsigned char Cr, unsigned char * B, unsigned char * G, unsigned char * R )
{
	
	double tempR;
	double tempG;
	double tempB;
	
	tempR = Y + 1.4020 * ( Cr - 128 );
	tempG = Y - 0.3441 * ( Cb - 128 ) - 0.7141 * ( Cr - 128 );
	tempB = Y + 1.7720 * ( Cb - 128 );
	
	* R = ( unsigned char )( tempR < 0 ? 0 : tempR > 255 ? 255 : tempR );
	* G = ( unsigned char )( tempG < 0 ? 0 : tempG > 255 ? 255 : tempG );
	* B = ( unsigned char )( tempB < 0 ? 0 : tempB > 255 ? 255 : tempB );
	
}

