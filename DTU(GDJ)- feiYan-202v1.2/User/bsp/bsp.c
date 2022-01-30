#include "includes.h"

/*
*********************************************************************************************************
*	�� �� ��: bsp_Init
*	����˵��: ��ʼ��Ӳ���豸��ֻ��Ҫ����һ�Ρ��ú�������CPU�Ĵ���������ļĴ�������ʼ��һЩȫ�ֱ�����
*			  ȫ�ֱ�����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_Init(void)
{
	/* ʹ��CRCУ��, ���ڿ���STemWin��ʹ�� */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	
	/* ���ȼ���������Ϊ4 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	bsp_InitUart(); 	
	bsp_InitLed(); 
//	bsp_InitDin();
	bsp_InitDout();
//	bsp_InitSPIBus();
//	bsp_InitSFlash();
	AIR208_Init();
}


/*
*********************************************************************************************************
*	�� �� ��: BSP_CPU_ClkFreq
*	����˵��: ��ȡϵͳʱ�ӣ�uCOS-II��Ҫʹ��
*	��    ��: ��
*	�� �� ֵ: ϵͳʱ��
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    RCC_ClocksTypeDef  rcc_clocks;

    RCC_GetClocksFreq(&rcc_clocks);
    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
}


/*
*********************************************************************************************************
*	�� �� ��: BSP_Tick_Init
*	����˵��: ��ʼ��ϵͳ�δ�ʱ����ΪuCOS-II��ϵͳʱ�ӽ��ģ�1msһ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void BSP_Tick_Init (void)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    
    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* ��ȡϵͳʱ��  */
    
#if (OS_VERSION >= 30000u)
    cnts  = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;     
#else
    cnts  = cpu_clk_freq / (CPU_INT32U)OS_TICKS_PER_SEC;        /* ��õδ�ʱ���Ĳ���  */
#endif
    
//	OS_CPU_SysTickInit(cnts);                                   
	SysTick_Config(cnts);
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_RunPer10ms
*	����˵��: �ú���ÿ��10ms��Systick�жϵ���1�Ρ���� bsp_timer.c�Ķ�ʱ�жϷ������һЩ��Ҫ�����Դ���
*			��������Է��ڴ˺��������磺����ɨ�衢���������п��Ƶȡ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_RunPer10ms(void)
{

}

/*
*********************************************************************************************************
*	�� �� ��: bsp_RunPer1ms
*	����˵��: �ú���ÿ��1ms��Systick�жϵ���1�Ρ���� bsp_timer.c�Ķ�ʱ�жϷ������һЩ��Ҫ�����Դ�����
*			������Է��ڴ˺��������磺��������ɨ�衣
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_RunPer1ms(void)
{

}
/***************************** ���������� www.armfly.com (END OF FILE) *********************************/