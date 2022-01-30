#ifndef _GPRS_QUEUE_H
#define _GPRS_QUEUE_H
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
#define GPRS_QUEUESIZE 20

#define BUF_LEN        512
typedef struct
{ 
	char cmd;          //����
	char trycnt;       //�Ѿ��ط�����
	char addr;
	int   len;
	char  buf[BUF_LEN];
}GPRS_BUF;

//������������
typedef GPRS_BUF       ElemType_G ;
//������򷵻�״̬����
typedef unsigned char  State;

//ѭ�����д洢�ṹ
typedef struct 
{
    ElemType_G data[GPRS_QUEUESIZE];//�洢����Ԫ��
    int        front;               //����ͷָ��
    int        rear;                //����βָ��
    int        count;               //����Ԫ�ظ���
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
