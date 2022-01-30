#include "my_stdlib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "bsp.h"
#include "bsp_os.h"
#include "malloc.h"	
const char hex_table[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
/*
*********************************************************************************************************
*	函 数 名: My_Printf
*	功能说明: 格式化输出,与硬件串口相关
*	形    参: 
*	返 回 值: 无
*********************************************************************************************************
*/
void My_Printf(char *fmt,...)
{
	  __va_list v_list;
	  char *p;
	
	  //等待发送为空
	  while(comGetWaitSend(PRINTF_UART))
		{
#if uCOS_EN == 1 
			BSP_OS_TimeDlyMs(1);
#else
			bsp_DelayMS(1);
#endif
		}
		
		//分配一块内存
		p = mymalloc(SRAMIN,256);
		memset((char*)p,0,256);
		
		//清空FIFO
		comClearFifo(PRINTF_UART);	
		//不允许格式化的字符串大于256,缓冲区长度决定
		if(strlen(fmt) > 256)
		{
			myfree(SRAMIN,p);	
			return;
		}
		va_start(v_list, fmt);     
		vsprintf((char*)p, fmt, v_list ); 
		va_end(v_list);	
		
		//发送消息
		comSendBuf(PRINTF_UART, (unsigned char*)p, strlen((char*)p));

		//释放
		myfree(SRAMIN,p);		
}

/*
*********************************************************************************************************
*	函 数 名: left
*	功能说明: 从字符串左端截取n个字符
*	形    参: dst:目的字符串
*           src:源字符串
*           n:字符个数
*	返 回 值: 无
*********************************************************************************************************
*/
char * left(char *dst,char *src, int n)
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(n>len) n = len;
    /*p += (len-n);*/   
    while(n--) *(q++) = *(p++);
    *(q++)='\0'; 
    return dst;
}

/*
*********************************************************************************************************
*	函 数 名: mid
*	功能说明: 从字符串中间截取n个字符
*	形    参: dst:目的字符串
*           src:源字符串
*           n:字符个数
*           m:起始位置
*	返 回 值: 无
*********************************************************************************************************
*/
char * mid(char *dst,char *src, int n,int m) //n为个数，m为位置
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(n>len) n = len-m;  //从第m个到最后
    if(m<0) m=0;  
    if(m>len) return NULL;
    p += m;
    while(n--) *(q++) = *(p++);
    *(q++)='\0'; 
    return dst;
}

/*
*********************************************************************************************************
*	函 数 名: right
*	功能说明: 从字符串右端截取n个字符
*	形    参: dst:目的字符串
*           src:源字符串
*           n:字符个数
*	返 回 值: 无
*********************************************************************************************************
*/
char * right(char *dst,char *src, int n)
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(n>len) n = len;
    p += (len-n);  //从右边第n个字符开始
    while(*(q++) == *(p++));
    return dst;
}

//************************************
// Method: strrpl
// FullName: strrpl
// Access: public
// Returns: void
// Qualifier: 字符串替换函数,能替换所有的要替换的字符串,被替换的字符串和替换的字符串不一定一样长.
// Parameter: char * pDstOut,输出字符串,要保证足够的空间可以存储替换后的字符串.
// Parameter: char * pSrcIn,输入字符串.
// Parameter: char * pSrcRpl,被替换的字符串.
// Parameter: char * pDstRpl,替换后的字符串.
// 注意:以上的字符串均要以 '\0 '结尾.
//************************************
void strrpl(char* pDstOut, char* pSrcIn, const char* pSrcRpl, const char* pDstRpl)
{
	char* pi = pSrcIn;
	char* po = pDstOut;

	int nSrcRplLen = strlen(pSrcRpl);
	int nDstRplLen = strlen(pDstRpl);

	char *p = NULL;
	int nLen = 0;

	do{
			// 找到下一个替换点
			p = strstr(pi, pSrcRpl);
			if(p != NULL)
			{
				// 拷贝上一个替换点和下一个替换点中间的字符串
				nLen = p - pi;
				memcpy(po, pi, nLen);
				// 拷贝需要替换的字符串
				memcpy(po + nLen, pDstRpl, nDstRplLen);
			}
			else
			{
				strcpy(po, pi);
				// 如果没有需要拷贝的字符串,说明循环应该结束
				break;
			}
			pi = p + nSrcRplLen;
			po = po + nLen + nDstRplLen;
	} while (p != NULL);
}

/*
*********************************************************************************************************
*	函 数 名: swap_HbyteLbyte
*	功能说明: 交换高低字节
*	形    参: value:输入值
*	返 回 值: 交换后的值
*********************************************************************************************************
*/
unsigned short swap_HbyteLbyte(unsigned short value)
{
	unsigned char Hbyte = (value>>8)&0xff;
	unsigned char Lbyte = (value>>0)&0xff;

	return ((((unsigned short)Lbyte)<<8)|Hbyte);
}

/*
*********************************************************************************************************
*	函 数 名: Ascii2UINT8
*	功能说明: 将ASCII码转换为UINT8数据
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
char Ascii2UINT8(char value)
{
	//数字
	if((value >= '0')&&(value <= '9'))
	{
		return value - 0x30;
  }
	else if((value == 'a')||(value == 'A'))
	{
		return 10;
  }
	else if((value == 'b')||(value == 'B'))
	{
		return 11;
  }
	else if((value == 'c')||(value == 'C'))
	{
		return 12;
  }
	else if((value == 'd')||(value == 'D'))
	{
		return 13;
  }
	else if((value == 'e')||(value == 'E'))
	{
		return 14;
  }
	else if((value == 'f')||(value == 'F'))
	{
		return 15;
  }
	else 
	{	
		return 16;
  }
}

/*
*********************************************************************************************************
*	函 数 名: CHAR转HEX
*	功能说明: 交换高低字节
*	形    参: value:输入值
*	返 回 值: 交换后的值
*********************************************************************************************************
*/
unsigned char CharToHex(char bHex)  
{  
    if((bHex>='0')&&(bHex<='9'))  
    {  
        bHex -= 0x30;  
    }  
    else if((bHex>='A')&&(bHex<='F'))//Capital  
    {  
        bHex -= 0x37;  
    }  
    else if((bHex>='a')&&(bHex<='f'))//Capital   
    {  
        bHex -= 0x57;  
    }  
    return bHex;  
}


/*
*********************************************************************************************************
*	函 数 名: str转HEX
*	功能说明: 需要定义一个全局变量数组const char hex_table[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
*	形    参: s:需要转换的字符串   l:字符串长度   d:转换后的hex头指针
*	返 回 值: 无
*********************************************************************************************************
*/

void To_Hex(char *s, int l, char *d)
{
    while(l--)
    {
        *(d++) = hex_table[*s >> 4];
        *(d++) = hex_table[*(s++) & 0x0f];
    }
}

/*
*********************************************************************************************************
*	函 数 名: HEX转str
*	功能说明: 
*	形    参: s:需要转换的HEX串   l:字符串长度   d:转换后的str头指针
*	返 回 值: 无
*********************************************************************************************************
*/
void From_Hex(char *s, int l, char *d)
{
    while(l--)
    {
        *(d++) = ( (*s>'9' ? *(s++)+9 : *(s++)) << 4 ) | ( (*s>'9' ? *(s++)+9 : *(s++)) & 0x0F );
    }
}