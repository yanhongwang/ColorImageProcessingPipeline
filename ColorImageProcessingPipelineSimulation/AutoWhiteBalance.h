void GrayWorld( short ** BayerPattern, int imageHeight, int imageWidth, double * scale_value_R, double * scale_value_B );
void Retinex( short ** BayerPattern, int imageHeight, int imageWidth, double * GainGB, double * GainGR );
void QuadraticMapping( short ** BayerPattern, int imageHeight, int imageWidth, double * FactorRU, double * FactorRV, double * FactorBU, double * FactorBV );
void ColorTemperatureCurve( short ** BayerPattern, int imageHeight, int imageWidth, double * RCoefficient, double * BCoefficient, char LargeBlockEnable );
