#ifndef EDGE_H
#define EDGE_H

unsigned char SearchMin(unsigned char *min_buf, const int iLength);
void edge_prefiltering(int imageHeight,int imageWidth,unsigned char **buf, unsigned char **edg_Bbuf , int a );
void edge_detection(int imageHeight , int imageWidth , unsigned char **edge_Bbuf, int bc1 , unsigned char **Rembuf );
void edge_enhancement(int imageHeight,int imageWidth,double **Lab_buf, unsigned char **Rembuf );
void bilateral(int imageHeight,int imageWidth,unsigned char **buf, int intBilateralSize,int fClossnessCoefficient,int fSimilarityCoefficient, unsigned char **Rembuf );

#endif