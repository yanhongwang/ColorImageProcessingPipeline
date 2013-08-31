#ifndef EDGE_H
#define EDGE_H

unsigned char SearchMin(unsigned char *min_buf, const int iLength);
void edge_prefiltering_Lab(int imageHeight,int imageWidth,double **Lab_buf, unsigned char **edg_Bbuf , int a );
void edge_prefiltering_rgb(int imageHeight,int imageWidth,unsigned char **Lab_buf, unsigned char **edg_Bbuf , int a );
void edge_detection(int imageHeight , int imageWidth , unsigned char **edge_Bbuf,unsigned char **edg_Rembuf, int bc1 );

#endif