#include  <includes.h>
/*
*********************************************************************************************************
*                                       静态全局变量
*********************************************************************************************************
*/      
static  OS_TCB   AppTaskSIM800CTCB;
static  CPU_STK  AppTaskSIM800CStk[APP_CFG_TASK_SIM800C_STK_SIZE];

static  OS_TCB   AppTaskStartTCB;
static  CPU_STK  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB   AppTaskUITCB;
static  CPU_STK  AppTaskUIStk[APP_CFG_TASK_UI_STK_SIZE];

static  OS_TCB   AppTaskMICSTCB;
static  CPU_STK  AppTaskMICSStk[APP_CFG_TASK_MICS_STK_SIZE];
/*
*********************************************************************************************************
*                                      函数声明
*********************************************************************************************************
*/
static  void   AppTaskStart          (void     *p_arg);
static  void   AppTaskUI			       (void 	   *p_arg);
static  void   AppTaskSIM800C			   (void 	   *p_arg);
static  void   AppTaskCreate         (void);     
static  void   AppObjCreate          (void);
				void   DispTaskInfo          (void);

/*
*********************************************************************************************************
*                                      系统组件
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int main(void)
{
    OS_ERR  err;                                         
    
	/* 初始化uC/OS-III 内核 */
    OSInit(&err);  


	/* 创建一个启动任务（也就是主任务）。启动任务会创建所有的应用程序任务 */
	  OSTaskCreate((OS_TCB       *)&AppTaskStartTCB,                 /* 任务控制块地址 */           
                 (CPU_CHAR     *)"App Task Start",                 /* 任务名 */
                 (OS_TASK_PTR   )AppTaskStart,                     /* 启动任务函数地址 */
                 (void         *)0,                                /* 传递给任务的参数 */
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO,          /* 任务优先级 */
                 (CPU_STK      *)&AppTaskStartStk[0],              /* 堆栈基地址 */
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE / 10, /* 堆栈监测区，这里表示后10%作为监测区 */
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,      /* 堆栈空间大小 */
                 (OS_MSG_QTY    )0,                                /* 本任务支持接受的最大消息数 */
                 (OS_TICK       )0,                                /* 设置时间片 */
                 (void         *)0,                                /* 堆栈空间大小 */  
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

	/* 启动多任务系统，控制权交给uC/OS-III */
    OSStart(&err);                                             
    
    (void)&err;
    
    return (0);
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 2
*********************************************************************************************************
*/
static  void  AppTaskStart (void *p_arg)
{
#if uCOS_EN == 1 
	  CPU_SR_ALLOC();
     (void)p_arg;
    CPU_CRITICAL_ENTER();
#endif
	
	CPU_Init();
	BSP_Tick_Init();
	//回调函数初始化
	App_OS_SetAllHooks();  
	bsp_Init();	
	Init_SoftPara();
#if OS_CFG_STAT_TASK_EN > 0u
     OSStatTaskCPUUsageInit(&err);   
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif
    
	/* 创建任务 */
  AppTaskCreate(); 
	AppObjCreate();
#if uCOS_EN == 1 
	 CPU_CRITICAL_EXIT();
#endif
	//初始化看门狗
	bsp_InitIwdg(1000);
  while (1)
	{  
		Para_Save_Check();
		//喂狗
		IWDG_Feed();
		BSP_OS_TimeDlyMs(10);
  }
}


/*
*********************************************************************************************************
*	函 数 名: AppTaskUI
*	功能说明: 
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 4
*********************************************************************************************************
*/
static void AppTaskUI(void *p_arg)
{	
  (void)p_arg;
	while(1)
	{
	//	bsp_LedToggle(1);
		BSP_OS_TimeDlyMs(10);
  }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskSIM800c
*	功能说明: 
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 3
*********************************************************************************************************
*/
static void AppTaskSIM800C(void *p_arg)
{	
  (void)p_arg;
	while(1)
	{
		AIR208_Task();
		BSP_OS_TimeDlyMs(100);
  }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskMICS
*	功能说明: 
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 5
*********************************************************************************************************
*/
static void AppTaskMICS(void *p_arg)
{	
  (void)p_arg;
	while(1)
	{
		
		Uart485_Task();
	//	Simchange();
		Para_Change_Check();//参数变化发送心跳用
		//json数据发送
		JsonTask();
		Led_Task();
		BSP_OS_TimeDlyMs(1);	
  }
}

/*
*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通讯
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*********************************************************************************************************
*/
static  void  AppObjCreate (void)
{
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*********************************************************************************************************
*/
static  void  AppTaskCreate (void)
{
	OS_ERR      err;
	
	/**************创建COM任务*********************/
	OSTaskCreate((OS_TCB       *)&AppTaskUITCB,            
                 (CPU_CHAR     *)"App Task UI",
                 (OS_TASK_PTR   )AppTaskUI, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_UI_PRIO,
                 (CPU_STK      *)&AppTaskUIStk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_UI_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_UI_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
	/**************创建SIM800C任务*********************/
	OSTaskCreate((OS_TCB       *)&AppTaskSIM800CTCB,            
                 (CPU_CHAR     *)"App Task SIM800C",
                 (OS_TASK_PTR   )AppTaskSIM800C, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_SIM800C_PRIO,
                 (CPU_STK      *)&AppTaskSIM800CStk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_SIM800C_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_SIM800C_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
	/**************其他任务****************************/
	OSTaskCreate((OS_TCB       *)&AppTaskMICSTCB,            
                 (CPU_CHAR     *)"App Task MICS",
                 (OS_TASK_PTR   )AppTaskMICS, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_MICS_PRIO,
                 (CPU_STK      *)&AppTaskMICSStk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_MICS_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_MICS_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
}

/*
*********************************************************************************************************
*	函 数 名: DispTaskInfo
*	功能说明: 将uCOS-III任务信息通过串口打印出来
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void DispTaskInfo(void)
{
	OS_TCB*  p_tcb;	        /* 定义一个任务控制块指针, TCB = TASK CONTROL BLOCK */
	float    CPU;
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();
  p_tcb = OSTaskDbgListPtr;
  CPU_CRITICAL_EXIT();
	
	/* 打印标题 */
	My_Printf("===============================================================\r\n");
	My_Printf(" 优先级 使用栈 剩余栈 百分比 利用率   任务名\r\n");
	My_Printf("  Prio   Used  Free   Per    CPU     Taskname\r\n");

	/* 遍历任务控制块列表(TCB list)，打印所有的任务的优先级和名称 */
	while (p_tcb != (OS_TCB *)0) 
	{
		CPU = (float)p_tcb->CPUUsage / 100;
		My_Printf("   %2d  %5d  %5d   %02d%%   %5.2f%%   %s\r\n", 
		p_tcb->Prio, 
		p_tcb->StkUsed, 
		p_tcb->StkFree, 
		(p_tcb->StkUsed * 100) / (p_tcb->StkUsed + p_tcb->StkFree),
		CPU,
		p_tcb->NamePtr);		
	 	
		CPU_CRITICAL_ENTER();
    p_tcb = p_tcb->DbgNextPtr;
    CPU_CRITICAL_EXIT();
	}
}
