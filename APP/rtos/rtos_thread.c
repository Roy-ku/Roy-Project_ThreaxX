/* Includes ------------------------------------------------------------------*/
#include "rtos_thread.h"
#include <stdarg.h>
/* typedef -----------------------------------------------------------*/

/* define ------------------------------------------------------------*/
#define APP_BASIC_TIME_PRIO 28u
#define APP_BASIC_TIME_STACK_SIZE 1024u

#define APP_IDLE_PRIO 31u
#define APP_IDLE_STACK_SIZE 256u

#define APP_STAT_PRIO 30u
#define APP_STATSTACK_SIZE 256u

#define APP_THREAD_01_PRIO 29u
#define APP_THREAD_01_STACK_SIZE 1024u

#define APP_THREAD_02_PRIO 5u
#define APP_THREAD_02_STACK_SIZE 1024u

/* macro -------------------------------------------------------------*/

/* variables ---------------------------------------------------------*/

/* 統計任務使用 */
__IO uint8_t OSStatRdy;     /* 統計任務就緒標誌 */
__IO uint32_t OSIdleCtr;    /* 空閒任務計數 */
__IO uint8_t OSCPUUsage;    /* CPU百分比 */
__IO uint32_t OSIdleCtrRun; /* 1秒內空閒任務當前計數 */

/* 任務句柄 */
static TX_THREAD AppBasicTimeTCB;
static TX_THREAD AppTaskIdleTCB;
static TX_THREAD AppTaskStatTCB;

static TX_THREAD AppThread01TCB;
static TX_THREAD AppThread02TCB;
/* 定時器 */
TX_TIMER AppTimer;
/* 互斥信號 */
static TX_MUTEX AppPrintfSemp; /* 用於printf互斥 */
TX_MUTEX AppI2CSemp;           /* 用於I2C互斥 */

/* function prototypes -----------------------------------------------*/
static void OSStatInit(void);
static void DispTaskInfo(void);
static void AppTimerCallback(ULONG thread_input);
static void AppBasicTime(ULONG thread_input);
static void AppThread01(ULONG thread_input);
static void AppThread02(ULONG thread_input);
static void AppTaskIDLE(ULONG thread_input);
static void AppTaskStat(ULONG thread_input);

UINT AppTaskCreate(VOID *memory_ptr)
{
    TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL *)memory_ptr;
    UCHAR *pointer;
    UINT ret = TX_SUCCESS;
    OSStatInit();
    /*******************************************************************/
    ret = tx_byte_allocate(byte_pool, (VOID **)&pointer, APP_BASIC_TIME_STACK_SIZE, TX_NO_WAIT);
    ret = tx_thread_create(&AppBasicTimeTCB,          /* 任務控制塊地址 */
                           "App Basic Time",          /* 任務名 */
                           AppBasicTime,              /* 啟動任務函數 */
                           0,                         /* 傳遞給任務的參數 */
                           pointer,                   /* 堆棧基地址 */
                           APP_BASIC_TIME_STACK_SIZE, /* 堆棧空間大小 */
                           APP_BASIC_TIME_PRIO,       /* 任務優先級 */
                           APP_BASIC_TIME_PRIO,       /* 任務搶占閥值 */
                           TX_NO_TIME_SLICE,          /* 不開啟時間片 */
                           TX_AUTO_START);            /* 創建後立即啟 */

    ret = tx_byte_allocate(byte_pool, (VOID **)&pointer, APP_THREAD_01_STACK_SIZE, TX_NO_WAIT);
    ret = tx_thread_create(&AppThread01TCB,          /* 任務控制塊地址 */
                           "App Thread 01",          /* 任務名 */
                           AppThread01,              /* 啟動任務函數 */
                           0,                        /* 傳遞給任務的參數 */
                           pointer,                  /* 堆棧基地址 */
                           APP_THREAD_01_STACK_SIZE, /* 堆棧空間大小 */
                           APP_THREAD_01_PRIO,       /* 任務優先級 */
                           APP_THREAD_01_PRIO,       /* 任務搶占閥值 */
                           TX_NO_TIME_SLICE,         /* 不開啟時間片 */
                           TX_AUTO_START);           /* 創建後立即啟 */

    ret = tx_byte_allocate(byte_pool, (VOID **)&pointer, APP_THREAD_02_STACK_SIZE, TX_NO_WAIT);
    ret = tx_thread_create(&AppThread02TCB,          /* 任務控制塊地址 */
                           "App Thread 02",          /* 任務名 */
                           AppThread02,              /* 啟動任務函數 */
                           0,                        /* 傳遞給任務的參數 */
                           pointer,                  /* 堆棧基地址 */
                           APP_THREAD_02_STACK_SIZE, /* 堆棧空間大小 */
                           APP_THREAD_02_PRIO,       /* 任務優先級 */
                           APP_THREAD_02_PRIO,       /* 任務搶占閥值 */
                           TX_NO_TIME_SLICE,         /* 不開啟時間片 */
                           TX_AUTO_START);           /* 創建後立即啟 */

    ret = tx_byte_allocate(byte_pool, (VOID **)&pointer, APP_STATSTACK_SIZE, TX_NO_WAIT);
    ret = tx_thread_create(&AppTaskStatTCB,    /* 任務控制塊地址 */
                           "App Task Stat",    /* 任務名 */
                           AppTaskStat,        /* 啟動任務函數 */
                           0,                  /* 傳遞給任務的參數 */
                           pointer,            /* 堆棧基地址 */
                           APP_STATSTACK_SIZE, /* 堆棧空間大小 */
                           APP_STAT_PRIO,      /* 任務優先級 */
                           APP_STAT_PRIO,      /* 任務搶占閥值 */
                           TX_NO_TIME_SLICE,   /* 不開啟時間片 */
                           TX_AUTO_START);     /* 創建後立即啟 */

    ret = tx_byte_allocate(byte_pool, (VOID **)&pointer, APP_IDLE_STACK_SIZE, TX_NO_WAIT);
    ret = tx_thread_create(&AppTaskIdleTCB,     /* 任務控制塊地址 */
                           "App Task Idle",     /* 任務名 */
                           AppTaskIDLE,         /* 啟動任務函數 */
                           0,                   /* 傳遞給任務的參數 */
                           pointer,             /* 堆棧基地址 */
                           APP_IDLE_STACK_SIZE, /* 堆棧空間大小 */
                           APP_IDLE_PRIO,       /* 任務優先級 */
                           APP_IDLE_PRIO,       /* 任務搶占閥值 */
                           TX_NO_TIME_SLICE,    /* 不開啟時間片 */
                           TX_AUTO_START);      /* 創建後立即啟 */
    /*******************************************************************/
    ret = tx_mutex_create(&AppPrintfSemp, "AppPrintfSemp", TX_NO_INHERIT);
    ret = tx_mutex_create(&AppI2CSemp, "AppI2CSemp", TX_NO_INHERIT);
    ret = tx_timer_create(&AppTimer,
                          "App Timer",
                          AppTimerCallback,
                          0,                 /* 傳遞的參數 */
                          1,                 /* 設置定時器時間溢出的初始延遲，單位ThreadX系統時間節拍數 */
                          1,                 /* 設置初始延遲後的定時器運行週期，如果設置為0，表示單次定時器 */
                          TX_AUTO_ACTIVATE); /* 激活定時器 */
    return ret;
}

void AppPrintf(const char *fmt, ...)
{

    char buf_str[256]; /* 特別注意，如果printf的變量較多，注意此局部變量的大小是否夠用 */
    va_list v_args;

    va_start(v_args, fmt);
    (void)vsnprintf((char *)&buf_str[0],
                    (size_t)sizeof(buf_str),
                    (char const *)fmt,
                    v_args);
    va_end(v_args);

    /* 互斥操作 */
    tx_mutex_get(&AppPrintfSemp, TX_WAIT_FOREVER);

    // bsp_INFO("%s", buf_str);
    printf("%s", buf_str);
    tx_mutex_put(&AppPrintfSemp);
}

void AppTimerCallback(ULONG thread_input)
{
    if (g_ucEnableSystickISR == 1)
    {
        bsp_SysTick_ISR();
    }
}

void OSStatInit(void)
{
    OSStatRdy = FALSE;

    tx_thread_sleep(2u); /* 时钟同步 */

    __disable_irq();
    OSIdleCtr = 0uL; /* 清空闲计数 */
    __enable_irq();

    tx_thread_sleep(100); /* 统计100ms内，最大空闲计数 */

    __disable_irq();
    OSStatRdy = TRUE;
    __enable_irq();
}

/*
*********************************************************************************************************
*	函 数 名: DispTaskInfo
*	功能说明: 将uCOS-III任务信息通过串口打印出来
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispTaskInfo(void)
{
    TX_THREAD *p_tcb; /* 定义一个任务控制块指针 */
    UINT my_old_threshold;
    p_tcb = &AppBasicTimeTCB;

    tx_thread_preemption_change(&AppBasicTimeTCB, 3, &my_old_threshold);

    AppPrintf("===============================================================\r\n");
    AppPrintf("OS CPU Usage = %d%%\r\n", OSCPUUsage);
    AppPrintf("===============================================================\r\n");
    AppPrintf("Prio  StackSize  MaxStack  CurStack  Percentage  Taskname\r\n");

    while (p_tcb != (TX_THREAD *)0)
    {
        uint16_t stack_size = p_tcb->tx_thread_stack_size;
        uint16_t stack_end = (int)p_tcb->tx_thread_stack_end;
        uint16_t stack_highest = (int)p_tcb->tx_thread_stack_highest_ptr;
        uint16_t stack_Cur = (int)p_tcb->tx_thread_stack_ptr;

        AppPrintf("%4d  %9d  %8d  %8d  %9.2f%%  %8s\r\n",
                  p_tcb->tx_thread_priority,
                  stack_size,
                  stack_end - stack_highest,
                  stack_end - stack_Cur,
                  ((((float)stack_end - (float)stack_Cur) / stack_size) * 100),
                  p_tcb->tx_thread_name);

        p_tcb = p_tcb->tx_thread_created_next;

        if (p_tcb == &AppBasicTimeTCB)
            break;
    }
    AppPrintf("===============================================================\r\n");
    tx_thread_preemption_change(&AppBasicTimeTCB, APP_BASIC_TIME_PRIO, &my_old_threshold);
}
/**************************************************************************/
static void AppBasicTime(ULONG thread_input)
{
    (void)thread_input;
    static uint8_t ucKeyCode = 0;
    while (1)
    {
        ucKeyCode = bsp_keyEvent();
        if (ucKeyCode != KEY_NONE)
        {
            switch (ucKeyCode)
            {
            case KEY_DOWN_K1:
                DispTaskInfo();
                break;
            case KEY_UP_K1:

                break;
            case KEY_LONG_K1:

                break;

            case KEY_DOWN_K2:

                break;
            case KEY_UP_K2:

                break;
            case KEY_LONG_K2:

                break;

            default:

                break;
            }
        }

        tx_thread_sleep(20);
    }
}

static void AppThread01(ULONG thread_input)
{
    (void)thread_input;
    uint8_t lcd_status = 0;
    bsp_LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
    bsp_LCD_SetFont(&LCD_DEFAULT_FONT);
    bsp_LCD_Clear(LCD_COLOR_BLUE);

    tx_thread_sleep(100);
    bsp_LCD_Backlight_ON();
    while (1)
    {
        if (LCD_Test == 1)
        {
            bsp_LCD_Test();
            lcd_status = 1;
        }
        else if (LCD_Test == 2)
        {
            bsp_LCD_Test_2();
            lcd_status = 1;
        }
        else if (LCD_Test == 3)
        {
            bsp_LCD_Test_3();
            lcd_status = 1;
        }
        else if (LCD_Test == 4)
        {
            if (lcd_status == 1)
            {
                bsp_LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLACK);
                bsp_LCD_SetFont(&LCD_DEFAULT_FONT);
                bsp_LCD_Clear(LCD_COLOR_BLUE);
                lcd_status = 0;
            }

            bsp_LCD_Test_4(0, Touch_X, Touch_Y, Touch_W);
        }
        tx_thread_sleep(20);
    }
}

static void AppThread02(ULONG thread_input)
{
    (void)thread_input;
    RTC_TimeTypeDef Get_Time = {0};
    RTC_DateTypeDef Get_Date = {0};
    while (1)
    {
        // AppPrintf("AppThread02\r\n");
        HAL_RTC_GetTime(&hrtc,&Get_Time,RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc,&Get_Date,RTC_FORMAT_BIN);
        AppPrintf("%02d/%02d/%02d ",2000+Get_Date.Year,Get_Date.Month,Get_Date.Date);
        AppPrintf("%02d:%02d:%02d\r\n",Get_Time.Hours,Get_Time.Minutes,Get_Time.Seconds);
        // __HAL_TIM_SET_COMPARE(g_tLEDG.htim, g_tLEDG.timchannel, 10000 - 1);
        // tx_thread_sleep(1000);
        // __HAL_TIM_SET_COMPARE(g_tLEDG.htim, g_tLEDG.timchannel, 0);
        tx_thread_sleep(1000);
    }
}

static void AppTaskStat(ULONG thread_input)
{
    (void)thread_input;

    while (OSStatRdy == FALSE)
    {
        tx_thread_sleep(10);
    }

    /* 設置初始CPU利用率 0% */
    __disable_irq();
    OSCPUUsage = 0u;
    OSIdleCtr = 0u;
    __enable_irq();

    while (1)
    {
        __disable_irq();
        OSIdleCtrRun = OSIdleCtr; /* 獲得100ms內空閒計數 */
        OSIdleCtr = 0uL;          /* 復位空閒計數 */
        __enable_irq();           /* 計算100ms內的CPU利用率 */
        OSCPUUsage = (100uL - OSIdleCtrRun);
        tx_thread_sleep(100); /* 每100ms統計一次 */
    }
}

static void AppTaskIDLE(ULONG thread_input)
{
    TX_INTERRUPT_SAVE_AREA

        (void)
    thread_input;

    while (1)
    {
        TX_DISABLE
        OSIdleCtr++;
        TX_RESTORE
        tx_thread_sleep(1);
    }
}

/***************************** (END OF FILE) *********************************/
