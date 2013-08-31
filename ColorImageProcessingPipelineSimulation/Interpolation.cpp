#include	<math.h>
#define	imageWidth	2064
#define	imageHeight	1545

void Interpolation_2002_fuzzy( short **G, short **R, short **B )
{
	double hue_n, hue_s, hue_w, hue_e, hue_hor, hue_ver;
	double hue_nw, hue_sw, hue_ne, hue_se, hue_md, hue_sd, C_hor, C_ver, I_hor, I_ver;
	int i, j, m, n, s, t;
	double tmp;
  /*  interpoalate G on R(B) patterns  */                                     
	for(i = 3; i < imageHeight - 3; i++){
		for(j = 3 + ((i+1)%2); j < imageWidth - 3; j = j + 2){
			if(i % 2 == 0){
				I_hor = 0.5*(G[i][j-1] + G[i][j+1]) + 0.5*0.25*(2*R[i][j] - R[i][j-2] - R[i][j+2]);
				//I_ver = 0.5*(G[i][j-1] + G[i][j+1]) + 0.5*0.25*(2*R[i][j] - R[i][j-2] - R[i][j+2]);
				I_hor > 1023 ? I_hor = 1023 : I_hor = I_hor;
				I_hor < 0 ? I_hor = 0 : I_hor = I_hor;
				I_ver = 0.5*(G[i-1][j] + G[i+1][j]) + 0.5*0.25*(2*R[i][j] - R[i-2][j] - R[i+2][j]);
				//I_hor = 0.5*(G[i-1][j] + G[i+1][j]) + 0.5*0.25*(2*R[i][j] - R[i-2][j] - R[i+2][j]);
				I_ver > 1023 ? I_ver = 1023 : I_ver = I_ver;
				I_ver < 0 ? I_ver = 0 : I_ver = I_ver;
				C_hor = fabs( 0.5*(R[i][j+2] - R[i][j-2]) + G[i][j-1] - G[i][j+1] );
				C_ver = fabs( 0.5*(R[i+2][j] - R[i-2][j]) + G[i-1][j] - G[i+1][j] );
				if(C_hor < C_ver){
					tmp = 0.8333*I_hor + 0.1667*I_ver;
					if( tmp > 1023)	G[i][j] = 1023;
					else if(tmp < 0) 	G[i][j] = 0;
					else	G[i][j] = tmp;
				}
				else{
					if(C_ver < C_hor){
						tmp = 0.1667*I_hor + 0.8333*I_ver;
						if( tmp > 1023)	G[i][j] = 1023;
						else if(tmp < 0) 	G[i][j] = 0;
						else	G[i][j] = tmp;
					}
					else{
						tmp = 0.5*(I_hor + I_ver);
						if( tmp > 1023)	G[i][j] = 1023;
						else if(tmp < 0) 	G[i][j] = 0;
						else	G[i][j] = tmp;
					}
				}
			}
			else{
				I_hor = 0.5*(G[i][j-1] + G[i][j+1]) + 0.5*0.25*(2*B[i][j] - B[i][j-2] - B[i][j+2]);
				//I_ver = 0.5*(G[i][j-1] + G[i][j+1]) + 0.5*0.25*(2*B[i][j] - B[i][j-2] - B[i][j+2]);
				I_hor > 1023 ? I_hor = 1023 : I_hor = I_hor;
				I_hor < 0 ? I_hor = 0 : I_hor = I_hor;
				I_ver = 0.5*(G[i-1][j] + G[i+1][j]) + 0.5*0.25*(2*B[i][j] - B[i-2][j] - B[i+2][j]);
				//I_hor = 0.5*(G[i-1][j] + G[i+1][j]) + 0.5*0.25*(2*B[i][j] - B[i-2][j] - B[i+2][j]);
				I_ver > 1023 ? I_ver = 1023 : I_ver = I_ver;
				I_ver < 0 ? I_ver = 0 : I_ver = I_ver;
				C_hor = fabs( 0.5*(B[i][j+2] - B[i][j-2]) + G[i][j-1] - G[i][j+1] );
				C_ver = fabs( 0.5*(B[i+2][j] - B[i-2][j]) + G[i-1][j] - G[i+1][j] );
				if(C_hor < C_ver){
					tmp = 0.8333*I_hor + 0.1667*I_ver;
					if( tmp > 1023)	G[i][j] = 1023;
					else if(tmp < 0) 	G[i][j] = 0;
					else	G[i][j] = tmp;
				}
				else{
					if(C_ver < C_hor){
						tmp = 0.1667*I_hor + 0.8333*I_ver;
						if( tmp > 1023)	G[i][j] = 1023;
						else if(tmp < 0) 	G[i][j] = 0;
						else	G[i][j] = tmp;
					}
					else{
						tmp = 0.5*(I_hor + I_ver);
						if( tmp > 1023)	G[i][j] = 1023;
						else if(tmp < 0) 	G[i][j] = 0;
						else	G[i][j] = tmp;
					}
				}
			}
		}
	}
  /*  interpolate R (B) on B (R) patterns  */
	for(m = 5; m < imageHeight - 5; m++){
		for(n = 5 + ((m+1)%2); n < imageWidth - 5; n = n + 2){
			if(m%2 == 0){
				hue_nw = B[m-1][n-1] - G[m-1][n-1];
				hue_sw = B[m+1][n-1] - G[m+1][n-1];
				hue_ne = B[m-1][n+1] - G[m-1][n+1];
				hue_se = B[m+1][n+1] - G[m+1][n+1];
				hue_md = fabs( hue_nw - hue_se );
				hue_sd = fabs( hue_ne - hue_sw );
				if(hue_md < hue_sd){
					tmp = G[m][n] + 0.8333*0.5*(B[m-1][n-1] - G[m-1][n-1] + B[m+1][n+1] - G[m+1][n+1]) + 0.1667*0.5*(B[m-1][n+1] - G[m-1][n+1] + B[m+1][n-1] - G[m+1][n-1]);
					if( tmp > 1023)	B[m][n] = 1023;
					else if(tmp < 0) 	B[m][n] = 0;
					else	B[m][n] = tmp;
				}
				else{
					tmp = G[m][n] + 0.1667*0.5*(B[m-1][n-1] - G[m-1][n-1] + B[m+1][n+1] - G[m+1][n+1]) + 0.8333*0.5*(B[m-1][n+1] - G[m-1][n+1] + B[m+1][n-1] - G[m+1][n-1]);
					if( tmp > 1023)	B[m][n] = 1023;
					else if(tmp < 0) 	B[m][n] = 0;
					else	B[m][n] = tmp;
				}
			}
			else{
				hue_nw = R[m-1][n-1] - G[m-1][n-1];
				hue_sw = R[m+1][n-1] - G[m+1][n-1];
				hue_ne = R[m-1][n+1] - G[m-1][n+1];
				hue_se = R[m+1][n+1] - G[m+1][n+1];
				hue_md = fabs( hue_nw - hue_se );
				hue_sd = fabs( hue_ne - hue_sw );
				if(hue_md < hue_sd){
					tmp = G[m][n] + 0.8333*0.5*(R[m-1][n-1] - G[m-1][n-1] + R[m+1][n+1] - G[m+1][n+1]) + 0.1667*0.5*(R[m-1][n+1] - G[m-1][n+1] + R[m+1][n-1] - G[m+1][n-1]);
					if( tmp > 1023)	R[m][n] = 1023;
					else if(tmp < 0) 	R[m][n] = 0;
					else	R[m][n] = tmp;
				}
				else{
					tmp = G[m][n] + 0.1667*0.5*(R[m-1][n-1] - G[m-1][n-1] + R[m+1][n+1] - G[m+1][n+1]) + 0.8333*0.5*(R[m-1][n+1] - G[m-1][n+1] + R[m+1][n-1] - G[m+1][n-1]);
					if( tmp > 1023)	R[m][n] = 1023;
					else if(tmp < 0) 	R[m][n] = 0;
					else	R[m][n] = tmp;
				}
			}
		}
	}
  /*  interpolate R (B) on G patterns  */
	for(s = 7; s < imageHeight - 7; s++){
		for(t = 7 + (s%2); t < imageWidth - 7; t = t + 2){
			hue_n = B[s-1][t] - G[s-1][t];
			hue_e = B[s][t+1] - G[s][t+1];
			hue_w = B[s][t-1] - G[s][t-1];
			hue_s = B[s+1][t] - G[s+1][t];
			hue_hor = fabs( hue_e - hue_w );
			hue_ver = fabs( hue_n - hue_s );
			if(hue_hor < hue_ver){
				tmp = G[s][t] + 0.8333*0.5*(B[s][t-1] - G[s][t-1] + B[s][t+1] - G[s][t+1]) + 0.1667*0.5*(B[s-1][t] - G[s-1][t] + B[s+1][t] - G[s+1][t]);
				if( tmp > 1023)	B[s][t] = 1023;
				else if(tmp < 0) 	B[s][t] = 0;
				else	B[s][t] = tmp;
			}
			else{
				tmp = G[s][t] + 0.1667*0.5*(B[s][t-1] - G[s][t-1] + B[s][t+1] - G[s][t+1]) + 0.8333*0.5*(B[s-1][t] - G[s-1][t] + B[s+1][t] - G[s+1][t]);
				if( tmp > 1023)	B[s][t] = 1023;
				else if(tmp < 0) 	B[s][t] = 0;
				else	B[s][t] = tmp;
			}

			hue_n = R[s-1][t] - G[s-1][t];
			hue_e = R[s][t+1] - G[s][t+1];
			hue_w = R[s][t-1] - G[s][t-1];
			hue_s = R[s+1][t] - G[s+1][t];
			hue_hor = fabs( hue_e - hue_w );
			hue_ver = fabs( hue_n - hue_s );
			if(hue_hor < hue_ver){
				tmp = G[s][t] + 0.8333*0.5*(R[s][t-1] - G[s][t-1] + R[s][t+1] - G[s][t+1]) + 0.1667*0.5*(R[s-1][t] - G[s-1][t] + R[s+1][t] - G[s+1][t]);
				if( tmp > 1023)	R[s][t] = 1023;
				else if(tmp < 0) 	R[s][t] = 0;
				else	R[s][t] = tmp;
			}
			else{
				tmp = G[s][t] + 0.1667*0.5*(R[s][t-1] - G[s][t-1] + R[s][t+1] - G[s][t+1]) + 0.8333*0.5*(R[s-1][t] - G[s-1][t] + R[s+1][t] - G[s+1][t]);
				if( tmp > 1023)	R[s][t] = 1023;
				else if(tmp < 0) 	R[s][t] = 0;
				else	R[s][t] = tmp;
			}
		}
	}
}

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

void Pattern2RGBPlanes
(
	
	short  **BayerPattern,
	short  **GPlane,
	short  **RPlane,
	short  **BPlane
	
)
{
	
	float tempValue;
	
	for( int i = 0; i < imageHeight; i ++ )
	{
		for( int j = 0; j < imageWidth; j++ )
		{
			if( i % 2 == 0 )
			{
				if( j % 2 == 0 )
					RPlane[ i ][ j ] = BayerPattern[ i ][ j ];
				else
					GPlane[ i ][ j ] = BayerPattern[ i ][ j ];
			}
			else
			{
				if( j % 2 == 0 )
					GPlane[ i ][ j ] = BayerPattern[ i ][ j ];
				else
					BPlane[ i ][ j ] = BayerPattern[ i ][ j ];
			}
		}
	}
}
