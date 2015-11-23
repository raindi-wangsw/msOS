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
*文件名       : device.c
*作用         : 设备接口
*原理         : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
********************************************************************************
*/



#include "drive.h"
#include "system.h"

extern void InitUsart1(void);

extern void InitUsart2(void);

extern void InitAdc(void);

extern void InitDI(void);

extern void InitDO(void);

extern void InitKey(void);

extern void InitLcd(void);

extern void InitTimer(void);

extern void InitStorage(void);

extern void InitMisc(void);

extern void InitSystick(void);
/*******************************************************************************
* 描述	    : 初始化设备
*******************************************************************************/
void InitDevice(void)
{    
    InitUsart1();	
    
    InitUsart2();

    InitAdc();

    InitDI();

    InitDO();

    InitKey();	

    InitLcd();

    InitTimer();

    InitStorage();

    InitMisc();

    InitSystick();

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);     // 抢占优先级|非抢占优先级,0为高
                                                        // bit3 bit2 | bit1 bit0 
                                                        //  0 ~ 3    |    0 ~ 3 
	NVIC_SetPriority(SysTick_IRQn, 0x0E);               //    3      |      2
 	NVIC_SetPriority(PendSV_IRQn, 0x0F);                //    3      |      3
}


