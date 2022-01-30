#ifndef _QUEUE_H
#define _QUEUE_H
#include "modbus.h"
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
#define QUEUESIZE 50

//������������
typedef _CMD           ElemType ;
//������򷵻�״̬����
typedef unsigned char  State;

//ѭ�����д洢�ṹ
typedef struct _CircleQueue
{
    ElemType data[QUEUESIZE];//�洢����Ԫ��
    int      front;          //����ͷָ��
    int      rear;           //����βָ��
    int      count;          //����Ԫ�ظ���
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
