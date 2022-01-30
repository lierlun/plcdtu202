#ifndef _QUEUE_H
#define _QUEUE_H
#include "modbus.h"
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
#define QUEUESIZE 50

//定义数据类型
typedef _CMD           ElemType ;
//定义程序返回状态类型
typedef unsigned char  State;

//循环队列存储结构
typedef struct _CircleQueue
{
    ElemType data[QUEUESIZE];//存储队列元素
    int      front;          //队列头指针
    int      rear;           //队列尾指针
    int      count;          //队列元素个数
}CircleQueue;

extern CircleQueue g_M485Cmd;

State InitQueue(CircleQueue *queue);
State IsQueueEmpty(CircleQueue *queue);
State IsQueueFull(CircleQueue *queue);
State EnQueue(CircleQueue *queue, ElemType e);
State DeQueue(CircleQueue *queue,ElemType *e);
State GetHead(CircleQueue *queue,ElemType *e);
State SetHead(CircleQueue *queue,ElemType e);
State ClearQueue(CircleQueue *queue );
int   GetLength(CircleQueue *queue);
#endif
