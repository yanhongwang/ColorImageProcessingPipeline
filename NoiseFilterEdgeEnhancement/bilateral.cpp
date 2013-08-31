#include <math.h>
#include <stdlib.h>

void bilateral(int imageHeight,int imageWidth,unsigned char **buf, int intBilateralSize,int fClossnessCoefficient,int fSimilarityCoefficient, unsigned char **Rembuf )
{
	int intNeighborRow,intNeighborColumn;
    int intCenterRow,intCenterColumn; //中心點
	intCenterRow=0;
	intCenterColumn=0;
	float fClosenessWeighted,fSimilarityWeighted,wupper,wdown,w;
	fClosenessWeighted=0;                                 //closeness function
	fSimilarityWeighted=0;                                //similarity function
	wupper=0;                                             //分子(weighted)                  
	wdown=0;                                              //分母(weighted)
	float n;
	int i,j;
	
	int color = 0;
	
	unsigned char Bil_buf[ 480 ][ 640 * 3 ];
	
	for(color = 0; color < 3 ; color++)
	{
		for( intCenterRow = 0; intCenterRow < imageHeight; intCenterRow++ )
		{
			for( intCenterColumn = color; intCenterColumn < imageWidth * 3; intCenterColumn += 3 )
			{
				for(  i = -intBilateralSize; i < intBilateralSize ; i++ )
				{
					for(  j = (-intBilateralSize * 3 ) ; j < (intBilateralSize * 3); j += 3 )
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

						if((intNeighborRow > (imageHeight-1)) || (intNeighborColumn > (imageWidth-1) * 3))
							continue;	
						if(intNeighborRow >= 0 && intNeighborColumn >= 0 && intNeighborRow <= imageHeight && intNeighborColumn <= imageWidth * 3)
						{
					
						n = pow(fabs(i),2) + pow(fabs(j),2);    //Euclidean Distance Square
					  
						
						if(Rembuf[intCenterRow][intCenterColumn/3] != 1)  //region
								fClosenessWeighted = exp(-1/2*fabs(n/(pow(fabs(fClossnessCoefficient),2))));
						else        //edge
							fClosenessWeighted = 1;

					    fSimilarityWeighted = exp(-1/2 * pow((fabs(buf[ intNeighborRow][ intNeighborColumn ] - buf[ intCenterRow ][ intCenterColumn ]) / fSimilarityCoefficient),2));
						w = fClosenessWeighted*fSimilarityWeighted;
						wupper += ( w * buf[ intNeighborRow ][ intNeighborColumn ] ) ;
						wdown += w;
						}
							
					}
				}
				Bil_buf[ intCenterRow ][ intCenterColumn ]=(wupper/wdown);
				wupper = 0;
				wdown=0;

			}
		}
	}
		for(i = 0 ; i < imageHeight ; i++)    //將Bil_buf中暫存的資料放回buf
		{                    
			for(j = 0 ; j < imageWidth*3 ; j++)
			{
				if(Rembuf[i][j/3]!=1)
					buf[i][j]=Bil_buf[i][j];
			}
		}

}
