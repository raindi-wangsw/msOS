/*
********************************************************************************
*                                嵌入式微系统
*                                   msOS
*
*                            硬件平台:msPLC DEMO
*                          主芯片:STM32F103R8T6/RBT6
*                           深圳市雨滴科技有限公司
*
*                                作者:王绍伟
*                                网名:凤舞天
*                                标识:Wangsw
*
*                                QQ:26033613
*                               QQ群:291235815
*                        论坛:http://bbs.huayusoft.com
*                        淘宝店:http://52edk.taobao.com
*                博客:http://forum.eet-cn.com/BLOG_wangsw317_1268.HTM
********************************************************************************
*文件名     : device_rtc.c
*作用       : 32.768KHz时钟晶体产生万年历，时钟从2013年1月1日星期二开始,必须要从闰年后一年开始
*原理       : 本万年历，只能适合2013~2099年，2100年非闰年
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2015/11/25       初始版本
********************************************************************************
*/

#include "drive.h"
#include "system.h"

#define SecondsInDay            86400       // 1天总共86400秒
#define DayInFourYear           1461        // 4年总共1461天，365*3+366
                                                                       
static const byte DaysInNonLeapMonthTable[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};  // 闰年的月份日期表
static const byte DaysInLeapMonthTable[12] =    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};  // 平年的月份日期表


/*******************************************************************************
* 描述	    : 设置日期，这里应该设置的是当前时间
* 输入参数  : 日期
* 返回参数  : 设置状态
*******************************************************************************/
static void SetDate(void)
{
    int i;
    uint second;
    uint readSecond;
    uint year;
    uint month;
    uint day;


    year = AppDataPointer->Rtc.Year - 2013;

    day = (year / 4) * DayInFourYear + (year % 4) * 365;

    month = AppDataPointer->Rtc.Month - 1;
    if (AppDataPointer->Rtc.Year % 4 == 0)
    {
        for (i = 0; i < month; i++)
            day = day + DaysInLeapMonthTable[i];
    }
    else
    {
        for (i = 0; i < month; i++)
            day = day + DaysInNonLeapMonthTable[i];
    }

    day = day + AppDataPointer->Rtc.Day - 1;

    second = day * SecondsInDay + AppDataPointer->Rtc.Hour * 3600 + AppDataPointer->Rtc.Minute * 60 + AppDataPointer->Rtc.Second;
    readSecond = RTC_GetCounter();
    if (second > readSecond + 10 || second + 10 < readSecond)
    {
        PWR_BackupAccessCmd(ENABLE);                           //开始修改数据
        RTC_WaitForLastTask();
        RTC_SetCounter(second);
        RTC_WaitForLastTask(); 
    }                                                                                     
}


/*******************************************************************************
* 描述	    : 获取当前日期，获取到的日期直接存到数据库中
* 输入参数  : 无
* 返回参数  : 获取状态
*******************************************************************************/
static void GetDate(void)
{     
    uint second;  
    uint day;
    uint dayInFourYear;
    
    uint year = 2013;
    uint month = 0;



    second = RTC_GetCounter();
    day = second / SecondsInDay;
    AppDataPointer->Rtc.Week = (day + 2) % 7;
        
    dayInFourYear = day % DayInFourYear;

    AppDataPointer->Rtc.Year = year + (day / DayInFourYear) * 4 + dayInFourYear / 365;

    if (dayInFourYear > 1095)
    {
        day = dayInFourYear - 1095;
        while (day > DaysInLeapMonthTable[month])
        {
            day = day - DaysInLeapMonthTable[month];
            month++;
        }
    }
    else
    {
        day = dayInFourYear % 365;
        while (day > DaysInNonLeapMonthTable[month])
        {
            day = day - DaysInNonLeapMonthTable[month];
            month++;
        }
    }

    AppDataPointer->Rtc.Month = month + 1;               
    AppDataPointer->Rtc.Day   = day + 1;                 

    second = second % SecondsInDay;                  
    AppDataPointer->Rtc.Hour   = second / 3600;                        
    AppDataPointer->Rtc.Minute = (second % 3600) / 60;        
    AppDataPointer->Rtc.Second = (second % 3600) % 60;    
}    

static void RTC_Config(void)
{
    int counter = 0;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP,ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    BKP_DeInit();
    RCC_LSEConfig(RCC_LSE_ON);
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
        if(++counter == 2000000)
            break;
    }
    if(counter < 2000000)
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    else
    {
        RCC_LSICmd(ENABLE);
        while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    }
    RCC_RTCCLKCmd(ENABLE);
    RTC_WaitForSynchro();
    RTC_WaitForLastTask();

    if(counter < 2000000) 
        RTC_SetPrescaler(32767);
    else
        RTC_SetPrescaler(40000 - 1);
    RTC_WaitForLastTask();
    RTC_ITConfig(RTC_IT_SEC,ENABLE);
    RTC_WaitForLastTask();
}

void RtcSystick100Routine(void)
{
    static byte counter;
    if (++counter == 100)
    {
        counter = 0;

        SetDate();
        GetDate();
    }
}

/*******************************************************************************
* 描述	    : 初始化RTC设备
* 输入参数  : 无
* 返回参数  : 无
*******************************************************************************/
void InitRtc(void)
{
    if(BKP_ReadBackupRegister(BKP_DR1) != 0x1234)
    {
        RTC_Config();
        BKP_WriteBackupRegister(BKP_DR1,0x1234);
    }
    else
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP,ENABLE);
        PWR_BackupAccessCmd(ENABLE);
        if((RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) && (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET))
            RTC_Config();
        
        RTC_WaitForSynchro();
        RTC_WaitForLastTask();
        RTC_ITConfig(RTC_IT_SEC,ENABLE);
        RTC_WaitForLastTask();
    }

    GetDate();
}


