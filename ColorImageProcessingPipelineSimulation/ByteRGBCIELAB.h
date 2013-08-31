void ByteRGB2Lab( unsigned char ByteB, unsigned char ByteG, unsigned char ByteR, double *L, double *a, double *b );
void ByteRGB2NonLinearRGB( unsigned char ByteB, unsigned char ByteG, unsigned char ByteR, double *NonLinearB, double *NonLinearG, double *NonLinearR );
void NonLinearRGB2LinearRGB( double NonLinearB, double NonLinearG, double NonLinearR, double *LinearB, double *LinearG, double *LinearR );
void LinearRGB2XYZ( double LiearB, double LinearG, double LinearR, double *X, double *Y, double *Z );
void XYZ2Lab( double X, double Y, double Z, double *L, double *a, double *b );

void Lab2ByteRGB( double L, double a, double b, unsigned char *ByteB, unsigned char *ByteG, unsigned char *ByteR );
void Lab2XYZ( double L, double a, double b, double *X, double *Y, double *Z );
void XYZ2LinearRGB( double X, double Y, double Z, double *LinearB, double *LinearG, double *LinearR );
void LinearRGB2NonLinearRGB( double LinearB, double LinearG, double LinearR, double *NonLinearB, double *NonLinearG, double *NonLinearR );
void NonLinearRGB2ByteRGB( double NonLinearB, double NonLinearG, double NonLinearR, unsigned char *ByteB, unsigned char *ByteG, unsigned char *ByteR );