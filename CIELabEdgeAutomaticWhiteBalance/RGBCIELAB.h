#ifndef RGBCIELAB

#define RGBCIELAB

void Lab2RGB( double L, double a, double b, unsigned char *B, unsigned char *G, unsigned char *R );
void Lab2XYZ( double L, double a, double b, double *X, double *Y, double *Z );
void XYZ2RGB( double X, double Y, double Z, unsigned char *B, unsigned char *G, unsigned char *R );

void RGB2XYZ( unsigned char B, unsigned char G, unsigned char R, double *X, double *Y, double *Z );
void XYZ2Lab( double X, double Y, double Z, double *L, double *a, double *b );
void RGB2Lab( unsigned char B, unsigned char G, unsigned char R, double *L, double *a, double *b );

#endif