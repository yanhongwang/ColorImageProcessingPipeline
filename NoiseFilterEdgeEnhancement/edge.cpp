#include <math.h>
#include <stdlib.h>

unsigned char SearchMin(unsigned char *min_buf, const int iLength)
{	
	
	unsigned char temp = min_buf[0];

	for( int i = 1 ; i < iLength ; i++)
		if(temp > min_buf[i])
			temp = min_buf[i];
	
	return temp;
}

void edge_prefiltering(int imageHeight,int imageWidth,unsigned char **buf, unsigned char **edg_Bbuf, int a )
{

	unsigned char edg_Abuf[ 480 ][ 640 * 3 ];

	int i,j;
	
	
//prefiltering A	
	//int a = 5 ;
	unsigned char *b_buf;
	unsigned char *c_buf;
    unsigned char unchar_delta;
	int m,n;
	m=n=0;
	
	char charMiddleFunction=0;
	char charNonlinearFunction=0;
	int intCenterRow,intCenterColumn;
	int	intNeighborRow,intNeighborColumn;
	intCenterRow=intCenterColumn=intNeighborRow=intNeighborColumn=0;
	
	int intRGBco;
	int intCount;
	int iSgm;
	for(intCount = 1 ; intCount < 3 ; intCount++)
	{
		for(intRGBco=0 ; intRGBco<3 ; intRGBco++)
		{
			for( intCenterRow = 0; intCenterRow < imageHeight; intCenterRow++ )    
			{
				for( intCenterColumn = intRGBco; intCenterColumn < (imageWidth * 3); intCenterColumn += 3 ){
					for(  i = -1; i < 2 ; i++ )
					{
					   for(  j = -3  ; j < 6 ; j += 3 )
					   {
						
						intNeighborRow=intCenterRow+i;
		                intNeighborColumn=intCenterColumn+j;
						
						
						if(intNeighborRow < 0)
						{
							intNeighborRow = 0;
							i = intNeighborRow - intCenterRow;
						}
						
						if(intNeighborColumn < 0)
						{
							intNeighborColumn = 0;
							j = intNeighborColumn - intCenterColumn;
						}
						
						if(( intNeighborRow == intCenterRow ) && ( intNeighborColumn == intCenterColumn))
							continue;

						if((intNeighborRow > (imageHeight-1)) || (intNeighborColumn > (imageWidth-1) * 3))
							continue;	
						if(intNeighborRow >= 0 && intNeighborColumn >= 0 && intNeighborRow <= imageHeight && intNeighborColumn <= imageWidth * 3){
							if(fabs(buf[intNeighborRow][intNeighborColumn]-buf[intCenterRow][intCenterColumn])<=a)
								charMiddleFunction=(buf[intNeighborRow][intNeighborColumn]-buf[intCenterRow][intCenterColumn]);
							else if(fabs(buf[intNeighborRow][intNeighborColumn]-buf[intCenterRow][intCenterColumn])>(3*a))
								charMiddleFunction = 0 ;
							else{
								if((buf[intNeighborRow][intNeighborColumn]-buf[intCenterRow][intCenterColumn])<0)
									iSgm=(-1);
								else
									iSgm=1;
								charMiddleFunction = (((3*a-buf[intNeighborRow][intNeighborColumn]+buf[intCenterRow][intCenterColumn] )/2)*iSgm);
							}
						}
						charNonlinearFunction+=charMiddleFunction;


							
					}
				}
				edg_Abuf[intCenterRow][intCenterColumn]=buf[intCenterRow][intCenterColumn]+ charNonlinearFunction/8 ;
				charNonlinearFunction=0;

			}
		}
	
		}
	}

	
	


//prefiltering B	



	c_buf = (unsigned char *) calloc(imageHeight, sizeof(unsigned char )); 
	b_buf = (unsigned char *) calloc(imageHeight, sizeof(unsigned char )); 
	
		
	for(intRGBco=0 ; intRGBco<3 ; intRGBco++)
	{
		for( intCenterRow = 0; intCenterRow < imageHeight; intCenterRow++ )    
		{
			for( intCenterColumn = intRGBco; intCenterColumn < (imageWidth * 3); intCenterColumn += 3 )
			{
				for(  i = -1; i < 2 ; i++ )
				{
					for(  j = -3  ; j < 6 ; j += 3 )
					{
						
						intNeighborRow=intCenterRow+i;
		                intNeighborColumn=intCenterColumn+j;
						
						
						if(intNeighborRow < 0)
						{
							intNeighborRow = 0;
							i = intNeighborRow - intCenterRow;
						}
						
						if(intNeighborColumn < 0)
						{
							intNeighborColumn = 0;
							j = intNeighborColumn - intCenterColumn;
						}
						
						if(( intNeighborRow == intCenterRow ) && ( intNeighborColumn == intCenterColumn))
							continue;

						if((intNeighborRow > (imageHeight-1)) || (intNeighborColumn > (imageWidth-1) * 3))
							continue;	
						
						
						if(intNeighborRow >= 0 && intNeighborColumn >= 0 && intNeighborRow <= imageHeight && intNeighborColumn <= imageWidth * 3)
						{
							if((edg_Abuf[intNeighborRow][intNeighborColumn]-edg_Abuf[intCenterRow][intCenterColumn]) <= 0 )
							{
								b_buf[m]=0;
								m+=1;
							}

							else{
								b_buf[m]=(edg_Abuf[intNeighborRow][intNeighborColumn]-edg_Abuf[intCenterRow][intCenterColumn])/255;
								m+=1;
							}

							if((edg_Abuf[intCenterRow][intCenterColumn]-edg_Abuf[intNeighborRow][intNeighborColumn]) <= 0 )
							{
								c_buf[n]=0;
								n+=1;
							}
							else
							{
								c_buf[n]=(edg_Abuf[intCenterRow][intCenterColumn]-edg_Abuf[intNeighborRow][intNeighborColumn])/255;
								n+=1;
							}	         
						}							
					}
				}

	            m=n=0;
				unchar_delta = SearchMin(c_buf, 8) - SearchMin(b_buf, 8);
				edg_Bbuf[intCenterRow][intCenterColumn] = edg_Abuf[intCenterRow][intCenterColumn] - 255 * unchar_delta;
				
			}
		}
	}

}

void edge_detection(int imageHeight , int imageWidth , unsigned char **edge_Bbuf, int bc1 , unsigned char **Rembuf )
{
	
	unsigned char edg3_Rembuf[3][3]; 
	unsigned char edg_Rembuf[ 480 ][ 640 ]; 

	double A0,A1,A2;

	double B1,B2;
	float bc2=45.0;
	float f_u;
	int i,j;
	i=j=0;
	int intCenterRow,intCenterColumn;
	int a,b,c,d,m,n;
	int color;

	a=b=c=d=0;

	for( color = 0 ; color < 3 ; color++ )

	{	
	
		for( intCenterRow = 1; intCenterRow < imageHeight - 1; intCenterRow++ )
	
		{
	

			for( intCenterColumn = ( 3 + color ); intCenterColumn < imageWidth * 3 - 3; intCenterColumn += 3 )
		
			{
			
				A1=(edge_Bbuf[intCenterRow-1][intCenterColumn-3]+edge_Bbuf[intCenterRow-1][intCenterColumn]+edge_Bbuf[intCenterRow-1][intCenterColumn+3])/3;
				A2=(edge_Bbuf[intCenterRow-1][intCenterColumn-3]+edge_Bbuf[intCenterRow][intCenterColumn-3]+edge_Bbuf[intCenterRow+1][intCenterColumn-3])/3;
				A0=(edge_Bbuf[intCenterRow][intCenterColumn]+edge_Bbuf[intCenterRow][intCenterColumn+3]+edge_Bbuf[intCenterRow+1][intCenterColumn]+A1+A2)/5;	
			    B1 = fabs(A0-A1);
			    B2 = fabs(A0-A2);
			
			    if((0 <= B1)&&(B1<bc1))
					B1=1;
				else
					B1=exp(-pow(fabs(B1-bc1)/bc2,2));
			
				if((0 <= B2 )&&( B2 < bc1))
					B2 = 1;
				else
					B2 = exp(-pow(fabs(B2-bc1)/bc2,2));
				if(B1<B2)
					f_u=B1;
				else
					f_u=B2;
				
				if( (1-f_u) != 0 )      //edge
					edg_Rembuf[intCenterRow][intCenterColumn/3] = 1;   //if edge, then edge_Rembuf=1
				
			}

	}

}
		
	for( i = 0 ; i < imageHeight ; i++ )
	{
		for( j = 0 ; j < imageWidth ; j++ )
		{
			if( edg_Rembuf[i][j] == 1 )         //edge
			{			
				
				for(a = -1 ; a < 2 ; a++ )
				{
					for( b = -1 ; b < 2 ; b++ )
					{
						edg3_Rembuf[a+1][b+1] = 0 ;
						for( c = -1 ; c < 2 ; c++)
						{
							for( d = -1 ; d < 2 ; d++ )
							{
								m = i + 3 * a + c;
								n = j + 3 * b + d;
								if( (0 <= m ) && ( imageHeight > m ) && ( n >=0 ) && ( imageWidth > n ))
								{
									
									if( edg_Rembuf[m][n] == 1 )
										edg3_Rembuf[a+1][b+1]=1 ;
								}
							}
						}
					}
				}			

				Rembuf[i][j] = 0 ;
				for( a = 0 ; a < 3 ; a++ )
				{
					for( b = 0 ; b < 3 ; b++ )
					{									
						if(edg3_Rembuf[a][b] == 1 )
							{
								Rembuf[i][j] += 1 ;
							}						
					}					
				}
			}

		}	
	}


for( i = 0 ; i < imageHeight ; i++ )
	{
		for( j = 0 ; j < imageWidth ; j++ )
		{
			if( Rembuf[i][j] >= 5 )
			{
				Rembuf[i][j] = 1;
			}

			else
				Rembuf[i][j] = 0;

		}
	}

}

int int_enhance=10;
double mean=0;


void edge_enhancement(int imageHeight,int imageWidth,double **Lab_buf, unsigned char **Rembuf )
{
	
	for(int i = 0 ; i < imageHeight ; i++ )
	{
		for(int j = 0 ; j < imageWidth*3 ; j+=3)
		{
			if ( Rembuf[i][j/3] ==1 )
			{

				for( int m = -1 ; m < 2 ; m++)
				{
					for( int n = -1 ; n < 2 ; n++ )
					{
						if (!((i+m) < 0 || (j+n) < 0 || (i+m) > imageHeight || (j+n) >imageWidth  ))
							mean += Lab_buf[ i + m ][ j + n ];
					}
				} 
				mean = mean / 9;
				
				if(Lab_buf[i][j] > mean)
				{
					if((Lab_buf[i][j]+3) <= 100)
						Lab_buf[i][j]+=3;
					
				}
				else
				{
					if((Lab_buf[i][j]-3) >= 0)
						Lab_buf[i][j]-=3;
				}
				mean = 0;

			}

		}

	}
	
}
