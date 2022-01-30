#include "Trans_queue.h"
TCircleQueue Trans_Queue;

/*---------------------------InitQueue----------------------------------------
 *  功能：    初始化队列
 *  参数:     queue：需要初始化的队列                   
 *  返回:     成功返回OK
 *  编写时间：2014年4月22日
 *  编写人：  张永强
 *  修改时间：
 *  修改人：
 *---------------------------------------------------------------------------*/
State T_InitQueue(TCircleQueue *queue)
{
    queue->front = 0; 
	  queue->rear = 0;
    queue->count = 0;
    return OK;
}

//判断队列为空和满
//1、使用计数器count,队列为空和满时，front都等于rear
//2、少用一个元素的空间，约定队列满时：(rear+1)%QUEUESIZE=front,为空时front=rear
//rear指向队尾元素的下一个位置，始终为空；队列的长度为(rear-front+QUEUESIZE)%QUEUESIZE

/*---------------------------IsQueueEmpty------------------------------------
 *  功能：    判断队列是否为空
 *  参数:     queue：指向相应队列的指针                   
 *  返回:     成功返回OK
 *  编写时间：2014年4月22日
 *  编写人：  张永强
 *  修改时间：
 *  修改人：
 *---------------------------------------------------------------------------*/
State T_IsQueueEmpty(TCircleQueue *queue)
{
    if(queue->count == 0)
        return TRUE;
    else
        return FALSE;
}

/*---------------------------IsQueueFull------------------------------------
 *  功能：    判断队列是否满
 *  参数:     queue：指向相应队列的指针                   
 *  返回:     成功返回OK
 *  编写时间：2014年4月22日
 *  编写人：  张永强
 *  修改时间：
 *  修改人：
 *---------------------------------------------------------------------------*/
State T_IsQueueFull(TCircleQueue *queue)
{
    if(queue->count == T_QUEUESIZE)
        return TRUE;
    else
        return FALSE;
}

/*---------------------------EnQueue----------------------------------------
 *  功能：    判断队列是否满
 *  参数:     queue：指向相应队列的指针 
 *            e:需要入队的元素                  
 *  返回:     成功返回OK
 *  编写时间：2014年4月22日
 *  编写人：  张永强
 *  修改时间：
 *  修改人：
 *---------------------------------------------------------------------------*/
State T_EnQueue(TCircleQueue *queue, ElemType e)
{
    //验证队列是否已满
    if(queue->count == T_QUEUESIZE)
       return ERROR;
    //入队
    queue->data[queue->rear] = e;
    //对尾指针后移
    queue->rear = (queue->rear + 1) % T_QUEUESIZE;
    //更新队列长度
    queue->count++;
    return OK;

}

/*---------------------------DeQueue----------------------------------------
 *  功能：    出队
 *  参数:     queue：指向相应队列的指针 
 *            e:指向取出元素的指针                  
 *  返回:     成功返回OK
 *  编写时间：2014年4月22日
 *  编写人：  张永强
 *  修改时间：
 *  修改人：
 *---------------------------------------------------------------------------*/
State T_DeQueue(TCircleQueue *queue,ElemType *e)
{
    //判断队列是否为空
    if(queue->count == 0)
	  return ERROR;
    //保存返回值
    *e = queue->data[queue->front];
    //更新队头指针
    queue->front = (queue->front + 1) % T_QUEUESIZE;
    //更新队列长度
    queue->count--;
    return OK;

}


/*---------------------------GetHead----------------------------------------
 *  功能：    获取队列的队首元素
 *  参数:     queue：指向相应队列的指针 
 *            e:指向队首元素的指针                  
 *  返回:     成功返回OK
 *  编写时间：2014年4月22日
 *  编写人：  张永强
 *  修改时间：
 *  修改人：
 *---------------------------------------------------------------------------*/
State T_GetHead(TCircleQueue *queue,ElemType *e)
{
    //判断队列是否为空
    if(queue->count == 0)
	   return ERROR;

    *e = queue->data[queue->front];
	return OK;
}

/*---------------------------SetHead----------------------------------------
 *  功能：    设置队列的队首元素
 *  参数:     queue：指向相应队列的指针 
 *            e:指向队首元素的指针                  
 *  返回:     成功返回OK
 *  编写时间：2014年4月22日
 *  编写人：  张永强
 *  修改时间：
 *  修改人：
 *---------------------------------------------------------------------------*/
State T_SetHead(TCircleQueue *queue,ElemType e)
{
    //判断队列是否为空
    if(queue->count == 0)
	   return ERROR;

    queue->data[queue->front] = e;
	return OK;
}

/*---------------------------ClearQueue----------------------------------------
 *  功能：    清空队列
 *  参数:     queue：指向相应队列的指针                 
 *  返回:     成功返回OK
 *  编写时间：2014年4月22日
 *  编写人：  张永强
 *  修改时间：
 *  修改人：
 *---------------------------------------------------------------------------*/
State T_ClearQueue(TCircleQueue *queue )
{
    queue->front = 0;
	  queue->rear = 0;
    queue->count = 0;
    return OK;

}

/*---------------------------GetLength----------------------------------------
 *  功能：    获取队列的长度
 *  参数:     queue：指向相应队列的指针                 
 *  返回:     成功返回OK
 *  编写时间：2014年4月22日
 *  编写人：  张永强
 *  修改时间：
 *  修改人：
 *---------------------------------------------------------------------------*/
int T_GetLength(TCircleQueue *queue)
{
    return queue->count;
}
