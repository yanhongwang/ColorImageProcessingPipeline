void ByteRGB2Lab( unsigned char ByteB, unsigned char ByteG, unsigned char ByteR, double *L, double *a, double *b )
{
	double NonLinearB, NonLinearG, NonLinearR;
	double LinearB, LinearG, LinearR;
	double X, Y, Z;
	
	ByteRGB2NonLinearRGB( ByteB, ByteG, ByteR, &NonLinearB, &NonLinearG, &NonLinearR );
	
	NonLinearRGB2LinearRGB( NonLinearB, NonLinearG, NonLinearR, &LinearB, &LinearG, &LinearR );
	
	LinearRGB2XYZ( LinearB, LinearG, LinearR, &X, &Y, &Z );
	
	XYZ2Lab( X, Y, Z, L, a, b );
}

void ByteRGB2NonLinearRGB( unsigned char ByteB, unsigned char ByteG, unsigned char ByteR,
				double *NonLinearB, double *NonLinearG, double *NonLinearR )
{
	*NonLinearB = ByteB / 255.0;
	*NonLinearG = ByteG / 255.0;
	*NonLinearR = ByteR / 255.0;
}

void NonLinearRGB2LinearRGB( double NonLinearB, double NonLinearG, double NonLinearR,
				double *LinearB, double *LinearG, double *LinearR )
{
	if( NonLinearB <= 0.04045 )
		*LinearB = NonLinearB / 12.92;
	else
		*LinearB = pow( ( NonLinearB + 0.055 ) / 1.055, 2.4 );
	
	if( NonLinearG <= 0.04045 )
		*LinearG = NonLinearG / 12.92;
	else
		*LinearG = pow( ( NonLinearG + 0.055 ) / 1.055, 2.4 );
	
	if( NonLinearR <= 0.04045 )
		*LinearR = NonLinearR / 12.92;
	else
		*LinearR = pow( ( NonLinearR + 0.055 ) / 1.055, 2.4 );
}

void LinearRGB2XYZ( double LinearB, double LinearG, double LinearR, double *X, double *Y, double *Z )
{
	*X = 0.412453 * LinearR + 0.357580 * LinearG + 0.180423 * LinearB;
	*Y = 0.212671 * LinearR + 0.715160 * LinearG + 0.072169 * LinearB;
	*Z = 0.019334 * LinearR + 0.119193 * LinearG + 0.950227 * LinearB;
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



///////////////////////////////////////////////////////////////////////////////////////////////////////

void Lab2ByteRGB( double L, double a, double b, unsigned char *ByteB, unsigned char *ByteG, unsigned char *ByteR )
{
	double X, Y, Z;
	double LinearB, LinearG, LinearR;
	double NonLinearB, NonLinearG, NonLinearR;
	
	Lab2XYZ( L, a, b, &X, &Y, &Z );
	
	XYZ2LinearRGB( X, Y, Z, &LinearB, &LinearG, &LinearR );
	
	LinearRGB2NonLinearRGB( LinearB, LinearG, LinearR, &NonLinearB, &NonLinearG, &NonLinearR );
	
	NonLinearRGB2ByteRGB( NonLinearB, NonLinearG, NonLinearR, ByteB, ByteG, ByteR );
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
	
	fZ = fY - b /200.0;
	if( fZ > 0.206893 )
		*Z = pow( fZ, 3.0 );
	else
		*Z = ( fZ - 16.0 / 116.0 ) / 7.787;
	
	( *X ) *= ( 0.950456 * 255.0 );
	( *Y ) *=             255.0;
	( *Z ) *= ( 1.088754 * 255.0 );
	
}

void XYZ2LinearRGB( double X, double Y, double Z, double *LinearB, double *LinearG, double *LinearR )
{
	*LinearR =  3.240479 * X - 1.537150 * Y - 0.498535 * Z;
	*LinearG = -0.969256 * X + 1.875992 * Y + 0.041556 * Z;
	*LinearB =  0.055648 * X - 0.204043 * Y + 1.057311 * Z;
}

void LinearRGB2NonLinearRGB( double LinearB, double LinearG, double LinearR, double *NonLinearB, double *NonLinearG, double *NonLinearR )
{
	if( ( *NonLinearB = 12.92 * LinearB ) > 0.04045 )
		*NonLinearB = 1.055 * pow( LinearB, 1 / 2.4 ) - 0.055;
	
	if( ( *NonLinearG = 12.92 * LinearG ) > 0.04045 )
		*NonLinearG = 1.055 * pow( LinearG, 1 / 2.4 ) - 0.055;
	
	if( ( *NonLinearR = 12.92 * LinearR ) > 0.04045 )
		*NonLinearR = 1.055 * pow( LinearR, 1 / 2.4 ) - 0.055;
}

void NonLinearRGB2ByteRGB( double NonLinearB, double NonLinearG, double NonLinearR,
				unsigned char *ByteB, unsigned char *ByteG, unsigned char *ByteR )
{
	double RR, GG, BB;
	
	BB = NonLinearB * 255;
	GG = NonLinearG * 255;
	RR = NonLinearR * 255;
	
	*ByteR = (unsigned char)(RR < 0 ? 0 : RR > 255 ? 255 : RR);
	*ByteG = (unsigned char)(GG < 0 ? 0 : GG > 255 ? 255 : GG);
	*ByteB = (unsigned char)(BB < 0 ? 0 : BB > 255 ? 255 : BB);
}

