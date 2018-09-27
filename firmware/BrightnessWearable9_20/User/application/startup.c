/*
 * File      : startup.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://openlab.rt-thread.com/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-08-31     Bernard      first implementation
 * 2011-06-05     Bernard      modify for STM32F107 version
 */

#include <rthw.h>
#include <rtthread.h>

#include "stm32f4xx.h"
#include "board.h"
#include "ff.h"
#include "rtc.h"
#include "easy_rtthread.h"

/**
 * @addtogroup STM32
 */

/*@{*/

extern int  rt_application_init(void);
#ifdef RT_USING_FINSH
extern void finsh_system_init(void);
extern void finsh_set_device(const char* device);
#endif

#ifdef __CC_ARM
extern int Image$$RW_IRAM1$$ZI$$Limit;
#define STM32_SRAM_BEGIN    (&Image$$RW_IRAM1$$ZI$$Limit)
#elif __ICCARM__
#pragma section="HEAP"
#define STM32_SRAM_BEGIN    (__segment_end("HEAP"))
#else
extern int __bss_end;
#define STM32_SRAM_BEGIN    (&__bss_end)
#endif

/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{
    rt_kprintf("\n\r Wrong parameter value detected on\r\n");
    rt_kprintf("       file  %s\r\n", file);
    rt_kprintf("       line  %d\r\n", line);

    while (1);
}

/**
 * This function will startup RT-Thread RTOS.
 */
void rtthread_startup(void)
{
    /* init board */
	/* �弶��ʼ���������ж�������ϵͳʱ�ӡ����ڡ�SPI�ӿڡ�GPIO������̨�ȳ�ʼ�� */
    rt_hw_board_init();

    /* show version */
	/* ��ʾRT-Thread Logo �Լ��汾��Ϣ */
    rt_show_version();

    /* init timer system */
	/* ϵͳ��ʱ����ʼ�� */
    rt_system_timer_init();

	/* ϵͳ�ѿռ��ڴ��ʼ�� */
    rt_system_heap_init((void*)STM32_SRAM_BEGIN, (void*)STM32_SRAM_END);
	
	RTC_init(); //RTCʱ�ӳ�ʼ��
	
	mount(); //�����ļ�ϵͳ

//    f_mount(FS_SD, NULL);


	
    /* init scheduler system */
	/* ϵͳ��������ʼ�� */
    rt_system_scheduler_init();

    /* init application */
	/* Ӧ�ó����ʼ���������Լ���Ӧ���߳� */
    rt_application_init();

#ifdef RT_USING_FINSH
    /* init finsh */
	/* Finsh ϵͳ��ʼ�� */
    finsh_system_init();
	/* ��Finsh�ض���UART */
    finsh_set_device( FINSH_DEVICE_NAME );
#endif

    /* init timer thread */
	/* ������ʱ���߳� */
    rt_system_timer_thread_init();	/* ʹ��������ʱ��ʱ����Ҫ������ʱ���߳�ɨ��������ʱ������ */

    /* init idle thread */
	/* ����ϵͳ�����߳� */
    rt_thread_idle_init();
	
    /* start scheduler */
	/* ����ϵͳ������ */
    rt_system_scheduler_start();

    /* never reach here */
    return;
}

/* �����ļ��е�__main ���ջ����main()���� */
int main(void)
{
    /* disable interrupt first */
	/* �ر��ж� */
    rt_hw_interrupt_disable();

    /* startup RT-Thread RTOS */
    rtthread_startup();

    return 0;
}

/*@}*/