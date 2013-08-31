#ifndef RGBCIELAB

#define RGBCIELAB

void Lab2RGB( double L, double a, double b, short *B, short *G, short *R );
void Lab2XYZ( double L, double a, double b, double *X, double *Y, double *Z );
void XYZ2RGB( double X, double Y, double Z, short *B, short *G, short *R );

void RGB2XYZ( short B, short G, short R, double *X, double *Y, double *Z );
void XYZ2Lab( double X, double Y, double Z, double *L, double *a, double *b );
void RGB2Lab( short B, short G, short R, double *L, double *a, double *b );

#endif