//void Pattern2RGBPlanes( unsigned char **BayerPattern, unsigned char **GPlane, unsigned char **RPlane, unsigned char **BPlane );
void Pattern2RGBPlanes( short **BayerPattern, short **GPlane, short **RPlane, short **BPlane );
void Interpolation_2003_Sobel( unsigned char **p, unsigned char **G, unsigned char **R, unsigned char **B );
void Interpolation_2002_fuzzy( short **G, short **R, short **B );