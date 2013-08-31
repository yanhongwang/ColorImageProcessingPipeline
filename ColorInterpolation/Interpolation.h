
void Pattern2RGBPlanes( unsigned char **BayerPattern, unsigned char **GPlane, unsigned char **RPlane, unsigned char **BPlane );
void PSNR( unsigned char **o, unsigned char **r, double *PSNRG, double *PSNRR, double *PSNRB );
void Interpolation_2003_Sobel( unsigned char **p, unsigned char **G, unsigned char **R, unsigned char **B );