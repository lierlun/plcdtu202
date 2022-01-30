#ifndef __MY_STDLIB_H
#define __MY_STDLIB_H

extern char           * left(char *dst,char *src, int n);
extern char           * mid(char *dst,char *src, int n,int m);
extern char           * right(char *dst,char *src, int n);
extern void           My_Printf(char *fmt,...);
extern unsigned short swap_HbyteLbyte(unsigned short value);
extern void           strrpl(char* pDstOut, char* pSrcIn, const char* pSrcRpl, const char* pDstRpl);
extern char           Ascii2UINT8(char value);
extern unsigned char  CharToHex(char bHex);
extern void           To_Hex(char *s, int l, char *d);
extern void           From_Hex(char *s, int l, char *d);

#define  PRINTF_UART     COM3

//版本信息
#define BOOT_VERSION                    0L              
#define BOOT_SUBVERSION                 1L             
#define BOOT_REVISION                   0L          
//打印版本信息和编译生成日期
#define BULID_Msg(fmt, arg...)                                        \
        My_Printf("\r\n%d.%d.%d build %s %s\r\n" fmt,                 \
        BOOT_VERSION, BOOT_SUBVERSION, BOOT_REVISION,                 \
				__DATE__, __TIME__, ## arg); 


//打印消息等级
#define ERR_LEVEL            1
#define WARN_LEVEL           2
#define INFO_LEVEL           3
#define DEBUG_LEVEL          4
//当前等级
#define CURRENT_LEVEL        4

//错误级
#define ERR_Msg(fmt, arg...)                                         \
    {                                                                \
			if (CURRENT_LEVEL >= ERR_LEVEL)                                \
			{                                                              \
				My_Printf(fmt, ## arg);                                      \
			}                                                              \
			else                                                           \
			{                                                              \
				My_Printf("");                                               \
			}	                                                             \
    }

//警告级
#define WARN_Msg(fmt, arg...)                                         \
    {                                                                 \
			if (CURRENT_LEVEL >= WARN_LEVEL)                                \
			{                                                               \
				My_Printf(fmt, ## arg);                                       \
			}                                                               \
			else                                                            \
			{                                                               \
				My_Printf("");                                                \
			}	                                                              \
    }	

//信息级		
#define INFO_Msg(fmt, arg...)                                         \
    {                                                                 \
			if (CURRENT_LEVEL >= INFO_LEVEL)                                \
			{                                                               \
				My_Printf(fmt, ## arg);                                       \
			}                                                               \
			else                                                            \
			{                                                               \
				My_Printf("");                                                \
			}	                                                              \
    }		

//调试级	
#define DBGU_Msg(fmt, arg...)                                         \
    {                                                                 \
			if (CURRENT_LEVEL >= DEBUG_LEVEL)                               \
			{                                                               \
				My_Printf(fmt,## arg);                                        \
			}                                                               \
			else                                                            \
			{                                                               \
				My_Printf("");                                                \
			}	                                                              \
    }		
#endif				
