#include  <includes.h>
/*
*********************************************************************************************************
*                                       ��̬ȫ�ֱ���
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
*                                      ��������
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
*                                      ϵͳ���
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int main(void)
{
    OS_ERR  err;                                         
    
	/* ��ʼ��uC/OS-III �ں� */
    OSInit(&err);  


	/* ����һ����������Ҳ���������񣩡���������ᴴ�����е�Ӧ�ó������� */
	  OSTaskCreate((OS_TCB       *)&AppTaskStartTCB,                 /* ������ƿ��ַ */           
                 (CPU_CHAR     *)"App Task Start",                 /* ������ */
                 (OS_TASK_PTR   )AppTaskStart,                     /* ������������ַ */
                 (void         *)0,                                /* ���ݸ�����Ĳ��� */
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO,          /* �������ȼ� */
                 (CPU_STK      *)&AppTaskStartStk[0],              /* ��ջ����ַ */
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE / 10, /* ��ջ������������ʾ��10%��Ϊ����� */
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,      /* ��ջ�ռ��С */
                 (OS_MSG_QTY    )0,                                /* ������֧�ֽ��ܵ������Ϣ�� */
                 (OS_TICK       )0,                                /* ����ʱ��Ƭ */
                 (void         *)0,                                /* ��ջ�ռ��С */  
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

	/* ����������ϵͳ������Ȩ����uC/OS-III */
    OSStart(&err);                                             
    
    (void)&err;
    
    return (0);
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskStart
*	����˵��: 
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 2
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
	//�ص�������ʼ��
	App_OS_SetAllHooks();  
	bsp_Init();	
	Init_SoftPara();
#if OS_CFG_STAT_TASK_EN > 0u
     OSStatTaskCPUUsageInit(&err);   
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif
    
	/* �������� */
  AppTaskCreate(); 
	AppObjCreate();
#if uCOS_EN == 1 
	 CPU_CRITICAL_EXIT();
#endif
	//��ʼ�����Ź�
	bsp_InitIwdg(1000);
  while (1)
	{  
		Para_Save_Check();
		//ι��
		IWDG_Feed();
		BSP_OS_TimeDlyMs(10);
  }
}


/*
*********************************************************************************************************
*	�� �� ��: AppTaskUI
*	����˵��: 
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 4
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
*	�� �� ��: AppTaskSIM800c
*	����˵��: 
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 3
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
*	�� �� ��: AppTaskMICS
*	����˵��: 
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 5
*********************************************************************************************************
*/
static void AppTaskMICS(void *p_arg)
{	
  (void)p_arg;
	while(1)
	{
		
		Uart485_Task();
	//	Simchange();
		Para_Change_Check();//�����仯����������
		//json���ݷ���
		JsonTask();
		Led_Task();
		BSP_OS_TimeDlyMs(1);	
  }
}

/*
*********************************************************************************************************
*	�� �� ��: AppObjCreate
*	����˵��: ��������ͨѶ
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void  AppObjCreate (void)
{
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void  AppTaskCreate (void)
{
	OS_ERR      err;
	
	/**************����COM����*********************/
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
	/**************����SIM800C����*********************/
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
	/**************��������****************************/
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
*	�� �� ��: DispTaskInfo
*	����˵��: ��uCOS-III������Ϣͨ�����ڴ�ӡ����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DispTaskInfo(void)
{
	OS_TCB*  p_tcb;	        /* ����һ��������ƿ�ָ��, TCB = TASK CONTROL BLOCK */
	float    CPU;
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();
  p_tcb = OSTaskDbgListPtr;
  CPU_CRITICAL_EXIT();
	
	/* ��ӡ���� */
	My_Printf("===============================================================\r\n");
	My_Printf(" ���ȼ� ʹ��ջ ʣ��ջ �ٷֱ� ������   ������\r\n");
	My_Printf("  Prio   Used  Free   Per    CPU     Taskname\r\n");

	/* ����������ƿ��б�(TCB list)����ӡ���е���������ȼ������� */
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
