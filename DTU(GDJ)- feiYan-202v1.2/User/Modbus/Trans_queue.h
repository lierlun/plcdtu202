#ifndef _TRANS_QUEUE_H
#define _TRANS_QUEUE_H
#include "modbus.h"
#include "queue.h"
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
#define T_QUEUESIZE 15

#define T_BUF_LEN        100
typedef struct
{
	char len;
	char buf[T_BUF_LEN];
}TRANS_BUF;

//定义数据类型
typedef TRANS_BUF      T_ElemType ;

//定义程序返回状态类型
typedef unsigned char  State;

//循环队列存储结构
typedef struct _TCircleQueue
{
    //T_ElemType data[T_QUEUESIZE];//存储队列元素
	  ElemType   data[T_QUEUESIZE];//存储队列元素
    int        front;            //队列头指针
    int        rear;             //队列尾指针
    int        count;            //队列元素个数
}TCircleQueue;

extern TCircleQueue Trans_Queue;

State T_InitQueue(TCircleQueue *queue);
State T_IsQueueEmpty(TCircleQueue *queue);
State T_IsQueueFull(TCircleQueue *queue);
State T_EnQueue(TCircleQueue *queue, ElemType e);
State T_DeQueue(TCircleQueue *queue,ElemType *e);
State T_GetHead(TCircleQueue *queue,ElemType *e);
State T_SetHead(TCircleQueue *queue,ElemType e);
State T_ClearQueue(TCircleQueue *queue );
int   T_GetLength(TCircleQueue *queue);
#endif
