#ifndef EDGE_H
#define EDGE_H

unsigned char SearchMin(unsigned char *min_buf, const int iLength);
void edge_prefiltering(int imageHeight,int imageWidth, short **buf, short **edg_Bbuf , int a );
void edge_detection(int imageHeight , int imageWidth , short **edge_Bbuf, int bc1 , short **Rembuf );
void edge_enhancement(int imageHeight,int imageWidth,double **Lab_buf, short ** Rembuf );
void bilateral(int imageHeight,int imageWidth, short **buf, int intBilateralSize,int fClossnessCoefficient,int fSimilarityCoefficient, short **Rembuf );

#endif