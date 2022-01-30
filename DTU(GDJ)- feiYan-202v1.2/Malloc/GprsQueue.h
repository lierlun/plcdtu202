#ifndef _GPRS_QUEUE_H
#define _GPRS_QUEUE_H
//定义函数结果状态码
#ifndef OK
#define OK         1
#endif

#ifndef ERROR
#define ERROR      0
#endif

#ifndef TRUE
#define TRUE       1
#endif

#ifndef FALSE
#define FALSE      0
#endif

//定义循环队列空间大小
#define GPRS_QUEUESIZE 20

#define BUF_LEN        512
typedef struct
{ 
	char cmd;          //命令
	char trycnt;       //已经重发次数
	char addr;
	int   len;
	char  buf[BUF_LEN];
}GPRS_BUF;

//定义数据类型
typedef GPRS_BUF       ElemType_G ;
//定义程序返回状态类型
typedef unsigned char  State;

//循环队列存储结构
typedef struct 
{
    ElemType_G data[GPRS_QUEUESIZE];//存储队列元素
    int        front;               //队列头指针
    int        rear;                //队列尾指针
    int        count;               //队列元素个数
}GPRS_CircleQueue;

extern GPRS_CircleQueue g_GPRSQueue;

State GPRS_InitQueue(GPRS_CircleQueue *queue);
State GPRS_IsQueueEmpty(GPRS_CircleQueue *queue);
State GPRS_IsQueueFull(GPRS_CircleQueue *queue);
State GPRS_EnQueue(GPRS_CircleQueue *queue, ElemType_G e);
State GPRS_DeQueue(GPRS_CircleQueue *queue,ElemType_G *e);
State GPRS_GetHead(GPRS_CircleQueue *queue,ElemType_G *e);
State GPRS_SetHead(GPRS_CircleQueue *queue,ElemType_G e);
State GPRS_ClearQueue(GPRS_CircleQueue *queue );
int   GPRS_GetLength(GPRS_CircleQueue *queue);
#endif
