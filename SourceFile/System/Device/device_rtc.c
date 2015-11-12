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
*                        淘宝店:http://52edk.taobao.com
*                      论坛:http://gongkong.eefocus.com/bbs/
*                博客:http://forum.eet-cn.com/BLOG_wangsw317_1268.HTM
********************************************************************************
*文件名     : device_rtc.c
*作用       : 虚拟时钟设备，替代32.768KHz晶体产生的时钟，一般用于精度不高场合
*原理       : 利用系统节拍产生的时钟分频后得到时、分、秒，此精度不高，用于一般场合
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
********************************************************************************
*/

#include "system.h"


/*******************************************************************************
* 描述	    : Rtc系统时钟100/S，即10mS一次调用
*******************************************************************************/
void RtcSystick100Routine(void) 
{
    static byte Counter = 0;
    
    if (++Counter == 100)
    {
        Counter = 0;

        if (AppDataPointer->Rtc.Second < 59)
            AppDataPointer->Rtc.Second++;
        else
        {
            AppDataPointer->Rtc.Second = 0;
            if(AppDataPointer->Rtc.Minute < 59)
                AppDataPointer->Rtc.Minute++;
            else
            {
                AppDataPointer->Rtc.Minute = 0;
                if(AppDataPointer->Rtc.Hour < 23)
                    AppDataPointer->Rtc.Hour++;
                else
                {
                    AppDataPointer->Rtc.Hour = 0;
                    AppDataPointer->Rtc.Day++;
                } 
            }
        }
    }	
}

