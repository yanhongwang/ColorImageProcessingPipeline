#include "RGBCIELAB.h"
#include <math.h>

//	amin =  -86, amax = 98
//	bmin = -108, bmax = 94
//	86 + 98 + 1 = 185
//	108 + 94 + 1 = 203

void XYZ2RGB( double X, double Y, double Z, unsigned char *B, unsigned char *G, unsigned char *R )
{
	double RR, GG, BB;

	RR =  3.240479 * X - 1.537150 * Y - 0.498535 * Z;
	GG = -0.969256 * X + 1.875992 * Y + 0.041556 * Z;
	BB =  0.055648 * X - 0.204043 * Y + 1.057311 * Z;

	*R = ( unsigned char )( RR < 0 ? 0 : RR > 255 ? 255 : RR );
	*G = ( unsigned char )( GG < 0 ? 0 : GG > 255 ? 255 : GG );
	*B = ( unsigned char )( BB < 0 ? 0 : BB > 255 ? 255 : BB );

}

void Lab2XYZ( double L, double a, double b, double *X, double *Y, double *Z )
{
	double fX, fY, fZ;
	
	fY = ( L + 16.0 ) / 116.0;
	if( fY > 0.206893 )
		*Y = pow( fY, 3.0 );
	else
		*Y = ( fY - 16.0 / 116.0 ) / 7.787;
	
	fX = a / 500.0 + fY;
	if( fX > 0.206893 )
		*X = pow( fX, 3.0 );
	else
		*X = ( fX - 16.0 / 116.0 ) / 7.787;
	
	fZ = fY - b / 200.0;
	if( fZ > 0.206893 )
		*Z = pow( fZ, 3.0 );
	else
		*Z = ( fZ - 16.0 / 116.0 ) / 7.787;
	
	( *X ) *= ( 0.950456 *  255.0 );
	( *Y ) *=		255.0;
	( *Z ) *= ( 1.088754 *  255.0 );

}

void Lab2RGB( double L, double a, double b, unsigned char *B, unsigned char *G, unsigned char *R )
{
	double X, Y, Z;

	Lab2XYZ( L, a, b, &X, &Y, &Z );
	
	XYZ2RGB( X, Y, Z, B, G, R );

}

void RGB2XYZ( unsigned char B, unsigned char G, unsigned char R, double *X, double *Y, double *Z )
{
	*X = ( double )( 0.412453 * R ) + ( double )( 0.357580 * G ) + ( double )( 0.180423 * B );
	*Y = ( double )( 0.212671 * R ) + ( double )( 0.715160 * G ) + ( double )( 0.072169 * B );
	*Z = ( double )( 0.019334 * R ) + ( double )( 0.119193 * G ) + ( double )( 0.950227 * B );
}

void XYZ2Lab( double X, double Y, double Z, double *L, double *a, double *b )
{
	
	double fX, fY, fZ;
	
	// with the D65 reference white: Xn = 0.9505; Yn = 1.00; Zn = 1.0891;
	
	X /= ( 255 * 0.950456 );
	Y /=  255;
	Z /= ( 255 * 1.088754 );
	
	if( Y > 0.008856 )
		fY = pow( Y, 1.0 / 3.0 );
	else
		fY = 7.787 * Y + 16.0 / 116.0;
	
	*L = 116.0 * fY - 16.0;
	
	if( X > 0.008856 )
		fX = pow( X, 1.0 / 3.0 );
	else
		fX = 7.787 * X + 16.0 / 116.0;
	
	if( Z > 0.008856 )
		fZ = pow( Z, 1.0 / 3.0 );
	else
		fZ = 7.787 * Z + 16.0 / 116.0;
	
	*a = 500.0 * ( fX - fY );
	*b = 200.0 * ( fY - fZ );
	
}

void RGB2Lab( unsigned char B, unsigned char G, unsigned char R, double *L, double *a, double *b )
{
	double X, Y, Z;
	
	RGB2XYZ( B, G, R, &X, &Y, &Z );
	
	XYZ2Lab( X, Y, Z, L, a, b );

}