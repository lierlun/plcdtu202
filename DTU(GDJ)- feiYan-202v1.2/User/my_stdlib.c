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
*	�� �� ��: My_Printf
*	����˵��: ��ʽ�����,��Ӳ���������
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void My_Printf(char *fmt,...)
{
	  __va_list v_list;
	  char *p;
	
	  //�ȴ�����Ϊ��
	  while(comGetWaitSend(PRINTF_UART))
		{
#if uCOS_EN == 1 
			BSP_OS_TimeDlyMs(1);
#else
			bsp_DelayMS(1);
#endif
		}
		
		//����һ���ڴ�
		p = mymalloc(SRAMIN,256);
		memset((char*)p,0,256);
		
		//���FIFO
		comClearFifo(PRINTF_UART);	
		//�������ʽ�����ַ�������256,���������Ⱦ���
		if(strlen(fmt) > 256)
		{
			myfree(SRAMIN,p);	
			return;
		}
		va_start(v_list, fmt);     
		vsprintf((char*)p, fmt, v_list ); 
		va_end(v_list);	
		
		//������Ϣ
		comSendBuf(PRINTF_UART, (unsigned char*)p, strlen((char*)p));

		//�ͷ�
		myfree(SRAMIN,p);		
}

/*
*********************************************************************************************************
*	�� �� ��: left
*	����˵��: ���ַ�����˽�ȡn���ַ�
*	��    ��: dst:Ŀ���ַ���
*           src:Դ�ַ���
*           n:�ַ�����
*	�� �� ֵ: ��
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
*	�� �� ��: mid
*	����˵��: ���ַ����м��ȡn���ַ�
*	��    ��: dst:Ŀ���ַ���
*           src:Դ�ַ���
*           n:�ַ�����
*           m:��ʼλ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
char * mid(char *dst,char *src, int n,int m) //nΪ������mΪλ��
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(n>len) n = len-m;  //�ӵ�m�������
    if(m<0) m=0;  
    if(m>len) return NULL;
    p += m;
    while(n--) *(q++) = *(p++);
    *(q++)='\0'; 
    return dst;
}

/*
*********************************************************************************************************
*	�� �� ��: right
*	����˵��: ���ַ����Ҷ˽�ȡn���ַ�
*	��    ��: dst:Ŀ���ַ���
*           src:Դ�ַ���
*           n:�ַ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
char * right(char *dst,char *src, int n)
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(n>len) n = len;
    p += (len-n);  //���ұߵ�n���ַ���ʼ
    while(*(q++) == *(p++));
    return dst;
}

//************************************
// Method: strrpl
// FullName: strrpl
// Access: public
// Returns: void
// Qualifier: �ַ����滻����,���滻���е�Ҫ�滻���ַ���,���滻���ַ������滻���ַ�����һ��һ����.
// Parameter: char * pDstOut,����ַ���,Ҫ��֤�㹻�Ŀռ���Դ洢�滻����ַ���.
// Parameter: char * pSrcIn,�����ַ���.
// Parameter: char * pSrcRpl,���滻���ַ���.
// Parameter: char * pDstRpl,�滻����ַ���.
// ע��:���ϵ��ַ�����Ҫ�� '\0 '��β.
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
			// �ҵ���һ���滻��
			p = strstr(pi, pSrcRpl);
			if(p != NULL)
			{
				// ������һ���滻�����һ���滻���м���ַ���
				nLen = p - pi;
				memcpy(po, pi, nLen);
				// ������Ҫ�滻���ַ���
				memcpy(po + nLen, pDstRpl, nDstRplLen);
			}
			else
			{
				strcpy(po, pi);
				// ���û����Ҫ�������ַ���,˵��ѭ��Ӧ�ý���
				break;
			}
			pi = p + nSrcRplLen;
			po = po + nLen + nDstRplLen;
	} while (p != NULL);
}

/*
*********************************************************************************************************
*	�� �� ��: swap_HbyteLbyte
*	����˵��: �����ߵ��ֽ�
*	��    ��: value:����ֵ
*	�� �� ֵ: �������ֵ
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
*	�� �� ��: Ascii2UINT8
*	����˵��: ��ASCII��ת��ΪUINT8����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
char Ascii2UINT8(char value)
{
	//����
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
*	�� �� ��: CHARתHEX
*	����˵��: �����ߵ��ֽ�
*	��    ��: value:����ֵ
*	�� �� ֵ: �������ֵ
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
*	�� �� ��: strתHEX
*	����˵��: ��Ҫ����һ��ȫ�ֱ�������const char hex_table[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
*	��    ��: s:��Ҫת�����ַ���   l:�ַ�������   d:ת�����hexͷָ��
*	�� �� ֵ: ��
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
*	�� �� ��: HEXתstr
*	����˵��: 
*	��    ��: s:��Ҫת����HEX��   l:�ַ�������   d:ת�����strͷָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void From_Hex(char *s, int l, char *d)
{
    while(l--)
    {
        *(d++) = ( (*s>'9' ? *(s++)+9 : *(s++)) << 4 ) | ( (*s>'9' ? *(s++)+9 : *(s++)) & 0x0F );
    }
}