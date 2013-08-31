#include "Unit1.h"

void Interpolation_2003_Sobel
(
	
	unsigned char **p,
	unsigned char **G,
	unsigned char **R,
	unsigned char **B
	
)
{
	double T, D, L, Ri, TL, TR, DL, DR; // Value of the interpolation in each direction
    double wT, wD, wL, wR, wTL, wTR, wDL, wDR; // Weight in each direction
	int i,j;
	/* Green interpolation on R and B pixels in Bayer Pattern */
	for(i = 3; i < imageHeight - 3; i++){
		for(j = 3; j < imageWidth - 3; j++){                
			if((i%2 == 0 && j%2 == 0) || (i%2 == 1 && j%2 == 1)){     
																	   
                    T = p[i-1][j] + (p[i][j] - p[i-2][j])/2;
					L = p[i][j-1] + (p[i][j] - p[i][j-2])/2;
					Ri = p[i][j+1] + (p[i][j] - p[i][j+2])/2;
					D = p[i+1][j] + (p[i][j] - p[i+2][j])/2;
				T < 0 ? T = 0 : T = T;
				T > 255 ? T = 255 : T = T;					
				L < 0 ? L = 0 : L = L;
				L > 255 ? L = 255 : L = L;
				Ri < 0 ? Ri = 0 : Ri = Ri;
				Ri > 255 ? Ri = 255 : Ri = Ri;
				D < 0 ? D = 0 : D = D;
				D > 255 ? D = 255 : D = D;					
					wT = 1/(1 + fabs(p[i+1][j] - p[i-1][j]) + fabs(p[i-1][j] - p[i-3][j]) + fabs(p[i][j] - p[i-2][j]) + fabs((p[i][j-1] - p[i-2][j-1])/2) + fabs((p[i][j+1] - p[i-2][j+1])/2)) ; 
					wL = 1/(1 + fabs(p[i][j+1] - p[i][j-1]) + fabs(p[i][j-1] - p[i][j-3]) + fabs(p[i][j] - p[i][j-2]) + fabs((p[i-1][j] - p[i-1][j-2])/2) + fabs((p[i+1][j] - p[i+1][j-2])/2) );
					wR = 1/(1 + fabs(p[i][j-1] - p[i][j+1]) + fabs(p[i][j+1] - p[i][j+3]) + fabs(p[i][j] - p[i][j+2]) + fabs((p[i-1][j] - p[i-1][j+2])/2) + fabs((p[i+1][j] - p[i+1][j+2])/2) );
					wD = 1/(1 + fabs(p[i-1][j] - p[i+1][j]) + fabs(p[i+1][j] - p[i+3][j]) + fabs(p[i][j] - p[i+2][j]) + fabs((p[i][j-1] - p[i+2][j-1])/2) + fabs((p[i][j+1] - p[i+2][j+1])/2) );
                
					//cout << (wT*T + wL*L + wR*R + wD*D)/(wT + wL + wR + wD) << ' ';
					if (((wT*T + wL*L + wR*Ri + wD*D)/(wT + wL + wR + wD) + 0.5) > 255)
						G[i][j] = 255;
					else if (((wT*T + wL*L + wR*Ri + wD*D)/(wT + wL + wR + wD) + 0.5) < 0)
						G[i][j] = 0;
					else
						G[i][j] = (wT*T + wL*L + wR*Ri + wD*D)/(wT + wL + wR + wD) + 0.5;
			}			
			else{                                                       
                G[i][j] = p[i][j];    
			}
		}
	}

	/* Red interpolation on B pixels in Bayer Pattern and vice versa */ 
  for(i = 5; i < imageHeight - 5; i++){ 
		for(int j = 5; j < imageWidth - 5; j++){
			if(i%2 == 1 && j%2 == 1){                                           
				TL = p[i-1][j-1] + (G[i][j] - G[i-1][j-1]);
				TR = p[i-1][j+1] + (G[i][j] - G[i-1][j+1]);
				DL = p[i+1][j-1] + (G[i][j] - G[i+1][j-1]);
				DR = p[i+1][j+1] + (G[i][j] - G[i+1][j+1]);
				TL < 0 ? TL = 0 : TL = TL;
				TL > 255 ? TL = 255 : TL = TL;
				TR < 0 ? TR = 0 : TR = TR;
				TR > 255 ? TR = 255 : TR = TR;
				DL < 0 ? DL = 0 : DL = DL;
				DL > 255 ? DL = 255 : DL = DL;
				DR < 0 ? DR = 0 : DR = DR;
				DR > 255 ? DR = 255 : DR = DR;

				wTL = 1/(1 + fabs(G[i-2][j-2] - G[i-1][j-1]) + fabs(G[i-1][j-1] - G[i][j]) + fabs((G[i-1][j-2] - G[i][j-1])/2) + fabs((G[i-2][j-1] - G[i-1][j])/2));
				wTR = 1/(1 + fabs(G[i-2][j+2] - G[i-1][j+1]) + fabs(G[i-1][j+1] - G[i][j]) + fabs((G[i-2][j+1] - G[i-1][j])/2) + fabs((G[i-1][j+2] - G[i][j+1])/2));
				wDL = 1/(1 + fabs(G[i+2][j-2] - G[i+1][j-1]) + fabs(G[i+1][j-1] - G[i][j]) + fabs((G[i+1][j-2] - G[i][j-1])/2) + fabs((G[i+2][j-1] - G[i+1][j])/2));
				wDR = 1/(1 + fabs(G[i+2][j+2] - G[i+1][j+1]) + fabs(G[i+1][j+1] - G[i][j]) + fabs((G[i+2][j+1] - G[i+1][j])/2) + fabs((G[i+1][j+2] - G[i][j+1])/2));

				if (((wTL*TL + wTR*TR + wDL*DL + wDR*DR)/(wTL + wTR + wDL + wDR) +0.5) > 255)
					R[i][j] = 255;
				else if (((wTL*TL + wTR*TR + wDL*DL + wDR*DR)/(wTL + wTR + wDL + wDR) +0.5) < 0)
					R[i][j] = 0;
				else
					R[i][j] = (wTL*TL + wTR*TR + wDL*DL + wDR*DR)/(wTL + wTR + wDL + wDR) +0.5;

				B[i][j] = p[i][j];
 
			}
			
			else if(i%2 ==0 && j%2 == 0){                                               
				TL = p[i-1][j-1] + (G[i][j] - G[i-1][j-1]);
				TR = p[i-1][j+1] + (G[i][j] - G[i-1][j+1]);
				DL = p[i+1][j-1] + (G[i][j] - G[i+1][j-1]);
				DR = p[i+1][j+1] + (G[i][j] - G[i+1][j+1]);
				TL < 0 ? TL = 0 : TL = TL;
				TL > 255 ? TL = 255 : TL = TL;
				TR < 0 ? TR = 0 : TR = TR;
				TR > 255 ? TR = 255 : TR = TR;
				DL < 0 ? DL = 0 : DL = DL;
				DL > 255 ? DL = 255 : DL = DL;
				DR < 0 ? DR = 0 : DR = DR;
				DR > 255 ? DR = 255 : DR = DR;

				wTL = 1/(1 + fabs(G[i-2][j-2] - G[i-1][j-1]) + fabs(G[i-1][j-1] - G[i][j]) + fabs((G[i-1][j-2] - G[i][j-1])/2) + fabs((G[i-2][j-1] - G[i-1][j])/2));
				wTR = 1/(1 + fabs(G[i-2][j+2] - G[i-1][j+1]) + fabs(G[i-1][j+1] - G[i][j]) + fabs((G[i-2][j+1] - G[i-1][j])/2) + fabs((G[i-1][j+2] - G[i][j+1])/2));
				wDL = 1/(1 + fabs(G[i+2][j-2] - G[i+1][j-1]) + fabs(G[i+1][j-1] - G[i][j]) + fabs((G[i+1][j-2] - G[i][j-1])/2) + fabs((G[i+2][j-1] - G[i+1][j])/2));
				wDR = 1/(1 + fabs(G[i+2][j+2] - G[i+1][j+1]) + fabs(G[i+1][j+1] - G[i][j]) + fabs((G[i+2][j+1] - G[i+1][j])/2) + fabs((G[i+1][j+2] - G[i][j+1])/2));

				if (((wTL*TL + wTR*TR + wDL*DL + wDR*DR)/(wTL + wTR + wDL + wDR) +0.5) > 255)
					B[i][j] = 255;
				else if (((wTL*TL + wTR*TR + wDL*DL + wDR*DR)/(wTL + wTR + wDL + wDR) +0.5) < 0)
					B[i][j] = 0;
				else
					B[i][j] = (wTL*TL + wTR*TR + wDL*DL + wDR*DR)/(wTL + wTR + wDL + wDR) +0.5;

				R[i][j] = p[i][j];
			}
			
		}

	}

  /* Red and Blue interpolation on G pixels in Bayer Pattern */
	for(i = 7; i < imageHeight - 7; i++){
		for(int j = 7; j < imageWidth - 7; j++){                
			if((i%2 == 0 && j%2 == 1) || (i%2 == 1 && j%2 == 0)){     
																	   
                    T = R[i-1][j] + (G[i][j] - G[i-1][j]);
					L = R[i][j-1] + (G[i][j] - G[i][j-1]);
					Ri = R[i][j+1] + (G[i][j] - G[i][j+1]);
					D = R[i+1][j] + (G[i][j] - G[i+1][j]);
				T < 0 ? T = 0 : T = T;
				T > 255 ? T = 255 : T = T;					
				L < 0 ? L = 0 : L = L;
				L > 255 ? L = 255 : L = L;
				Ri < 0 ? Ri = 0 : Ri = Ri;
				Ri > 255 ? Ri = 255 : Ri = Ri;
				D < 0 ? D = 0 : D = D;
				D > 255 ? D = 255 : D = D;				
					wT = 1/(1 + fabs(G[i][j] - G[i-1][j]) + fabs(G[i-1][j] - G[i-2][j]) + fabs((G[i][j-1] - G[i-1][j-1])/2) + fabs((G[i-1][j-1] - G[i-2][j-1])/2) + fabs((G[i][j+1] - G[i-1][j+1])/2) + fabs((G[i-1][j+1] - G[i-2][j+1])/2) ); 
					wL = 1/(1 + fabs(G[i][j] - G[i][j-1]) + fabs(G[i][j-1] - G[i][j-2]) + fabs((G[i-1][j] - G[i-1][j-1])/2) + fabs((G[i-1][j-1] - G[i-1][j-2])/2) + fabs((G[i+1][j] - G[i+1][j-1])/2) + fabs((G[i+1][j-1] - G[i+1][j-2])/2) );
					wR = 1/(1 + fabs(G[i][j] - G[i][j+1]) + fabs(G[i][j+1] - G[i][j+2]) + fabs((G[i-1][j] - G[i-1][j+1])/2) + fabs((G[i-1][j+1] - G[i-1][j+2])/2) + fabs((G[i+1][j] - G[i+1][j+1])/2) + fabs((G[i+1][j+1] - G[i+1][j+2])/2) );
					wD = 1/(1 + fabs(G[i][j] - G[i+1][j]) + fabs(G[i+1][j] - G[i+2][j]) + fabs((G[i][j-1] - G[i+1][j-1])/2) + fabs((G[i+1][j-1] - G[i+2][j-1])/2) + fabs((G[i][j+1] - G[i+1][j+1])/2) + fabs((G[i+1][j+1] - G[i+2][j+1])/2));
					
					if (((wT*T + wL*L + wR*Ri + wD*D)/(wT + wL + wR + wD) + 0.5) > 255)
						R[i][j] = 255;
					else if (((wT*T + wL*L + wR*Ri + wD*D)/(wT + wL + wR + wD) + 0.5) < 0)
						R[i][j] = 0;
					else
						R[i][j] = (wT*T + wL*L + wR*Ri + wD*D)/(wT + wL + wR + wD) + 0.5;


                    T = B[i-1][j] + (G[i][j] - G[i-1][j]);
					L = B[i][j-1] + (G[i][j] - G[i][j-1]);
					Ri = B[i][j+1] + (G[i][j] - G[i][j+1]);
					D = B[i+1][j] + (G[i][j] - G[i+1][j]);
	            T < 0 ? T = 0 : T = T;
				T > 255 ? T = 255 : T = T;					
				L < 0 ? L = 0 : L = L;
				L > 255 ? L = 255 : L = L;
				Ri < 0 ? Ri = 0 : Ri = Ri;
				Ri > 255 ? Ri = 255 : Ri = Ri;
				D < 0 ? D = 0 : D = D;
				D > 255 ? D = 255 : D = D;				
					wT = 1/(1 + fabs(G[i][j] - G[i-1][j]) + fabs(G[i-1][j] - G[i-2][j]) + fabs((G[i][j-1] - G[i-1][j-1])/2) + fabs((G[i-1][j-1] - G[i-2][j-1])/2) + fabs((G[i][j+1] - G[i-1][j+1])/2) + fabs((G[i-1][j+1] - G[i-2][j+1])/2) ); 
					wL = 1/(1 + fabs(G[i][j] - G[i][j-1]) + fabs(G[i][j-1] - G[i][j-2]) + fabs((G[i-1][j] - G[i-1][j-1])/2) + fabs((G[i-1][j-1] - G[i-1][j-2])/2) + fabs((G[i+1][j] - G[i+1][j-1])/2) + fabs((G[i+1][j-1] - G[i+1][j-2])/2) );
					wR = 1/(1 + fabs(G[i][j] - G[i][j+1]) + fabs(G[i][j+1] - G[i][j+2]) + fabs((G[i-1][j] - G[i-1][j+1])/2) + fabs((G[i-1][j+1] - G[i-1][j+2])/2) + fabs((G[i+1][j] - G[i+1][j+1])/2) + fabs((G[i+1][j+1] - G[i+1][j+2])/2) );
					wD = 1/(1 + fabs(G[i][j] - G[i+1][j]) + fabs(G[i+1][j] - G[i+2][j]) + fabs((G[i][j-1] - G[i+1][j-1])/2) + fabs((G[i+1][j-1] - G[i+2][j-1])/2) + fabs((G[i][j+1] - G[i+1][j+1])/2) + fabs((G[i+1][j+1] - G[i+2][j+1])/2));

					if (((wT*T + wL*L + wR*Ri + wD*D)/(wT + wL + wR + wD) + 0.5) > 255)
						B[i][j] = 255;
					else if (((wT*T + wL*L + wR*Ri + wD*D)/(wT + wL + wR + wD) + 0.5) < 0)
						B[i][j] = 0;
					else
						B[i][j] = (wT*T + wL*L + wR*Ri + wD*D)/(wT + wL + wR + wD) + 0.5;
			}						
		}
	}
	
	for( int x = 0; x < imageHeight; x ++ )
		for( int y = 0; y < imageWidth; y ++ )
			if( x < 10 || x >= ( imageHeight - 10 ) || y < 10 || y >= ( imageWidth - 10 ) )
			{
				G[ x ][ y ] = 255;
				R[ x ][ y ] = 255;
				B[ x ][ y ] = 255;
			}
	
}

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
	
	for( int i = 10; i < imageHeight - 10; i ++ )
		for( int j = 3 * 10; j < 3 * ( imageWidth - 10 ); j += 3 )
		{
			
			diffB += pow( o[ i ][ j + 0 ] - r[ i ][ j + 0 ], 2 );
			diffG += pow( o[ i ][ j + 1 ] - r[ i ][ j + 1 ], 2 );
			diffR += pow( o[ i ][ j + 2 ] - r[ i ][ j + 2 ], 2 );
			
		}
	
	*PSNRB = 10 * log10( ( 255 * 255 ) / ( diffB / ( ( imageHeight - 10 ) * ( imageWidth - 10 ) ) ) );
	*PSNRG = 10 * log10( ( 255 * 255 ) / ( diffG / ( ( imageHeight - 10 ) * ( imageWidth - 10 ) ) ) );
	*PSNRR = 10 * log10( ( 255 * 255 ) / ( diffR / ( ( imageHeight - 10 ) * ( imageWidth - 10 ) ) ) );
	
}

void Pattern2RGBPlanes
(
	
	unsigned char **BayerPattern,
	unsigned char **GPlane,
	unsigned char **RPlane,
	unsigned char **BPlane
	
)
{
	for(int i = 0; i < imageHeight; i ++){
		for(int j = 0; j < imageWidth; j++ ){
			if(i%2 == 0){
				if(j%2 == 0)	RPlane[i][j] = BayerPattern[i][j];
				else	GPlane[i][j] = BayerPattern[i][j];
			}
			else{
				if(j%2 == 0)	GPlane[i][j] = BayerPattern[i][j];
				else	BPlane[i][j] = BayerPattern[i][j];
			}				
		}
	}
}
