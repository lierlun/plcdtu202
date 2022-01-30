#ifndef _TRANS_QUEUE_H
#define _TRANS_QUEUE_H
#include "modbus.h"
#include "queue.h"
//���庯�����״̬��
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

//����ѭ�����пռ��С
#define T_QUEUESIZE 15

#define T_BUF_LEN        100
typedef struct
{
	char len;
	char buf[T_BUF_LEN];
}TRANS_BUF;

//������������
typedef TRANS_BUF      T_ElemType ;

//������򷵻�״̬����
typedef unsigned char  State;

//ѭ�����д洢�ṹ
typedef struct _TCircleQueue
{
    //T_ElemType data[T_QUEUESIZE];//�洢����Ԫ��
	  ElemType   data[T_QUEUESIZE];//�洢����Ԫ��
    int        front;            //����ͷָ��
    int        rear;             //����βָ��
    int        count;            //����Ԫ�ظ���
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
