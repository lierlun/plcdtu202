/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*	ģ������ : uCOS-III��Ӧ������
*	�ļ����� : app_cfg.c
*	��    �� : V1.0
*	˵    �� : ucos-ii��Ӧ������
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2015-08-10 Eric2013  ��ʽ����
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#ifndef  APP_CFG_MODULE_PRESENT
#define  APP_CFG_MODULE_PRESENT


/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/
#define  APP_CFG_TASK_START_PRIO                              2u
#define  APP_CFG_TASK_SIM800C_PRIO                            3u
#define  APP_CFG_TASK_UI_PRIO                                 4u
#define  APP_CFG_TASK_MICS_PRIO                               5u
/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/
#define  APP_CFG_TASK_SIM800C_STK_SIZE                    		1024u
#define  APP_CFG_TASK_START_STK_SIZE                      		512u
#define  APP_CFG_TASK_UI_STK_SIZE                         		512u
#define  APP_CFG_TASK_MICS_STK_SIZE                         	512u
/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/

#ifndef  TRACE_LEVEL_OFF
#define  TRACE_LEVEL_OFF                        0u
#endif

#ifndef  TRACE_LEVEL_INFO
#define  TRACE_LEVEL_INFO                       1u
#endif

#ifndef  TRACE_LEVEL_DBG
#define  TRACE_LEVEL_DBG                        2u
#endif

#define  APP_TRACE_LEVEL                        TRACE_LEVEL_OFF
#define  APP_TRACE                              printf

#define  APP_TRACE_INFO(x)               ((APP_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_TRACE x) : (void)0)
#define  APP_TRACE_DBG(x)                ((APP_TRACE_LEVEL >= TRACE_LEVEL_DBG)   ? (void)(APP_TRACE x) : (void)0)

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
