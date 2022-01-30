#include "filter.h"

short  GetMedianNum(short * bArray, int iFilterLen)  
{  
    int   i,j;
    short bTemp;  
      
    //冒泡法排序
    for (j = 0; j < iFilterLen - 1; j ++)  
    {  
        for (i = 0; i < iFilterLen - j - 1; i ++)  
        {  
            if (bArray[i] > bArray[i + 1])  
            {   
                bTemp = bArray[i];  
                bArray[i] = bArray[i + 1];  
                bArray[i + 1] = bTemp;  
            }  
        }  
    }  
    
		//判断是奇数还是偶数
    if ((iFilterLen & 1) > 0)  
    {  
        
        bTemp = bArray[(iFilterLen + 1) / 2];  
    }  
    else  
    {  
        bTemp = (bArray[iFilterLen / 2] + bArray[iFilterLen / 2 + 1]) / 2;  
    }  
  
    return bTemp;  
}  
