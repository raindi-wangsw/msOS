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
*文件名     : device_di.c
*作用       : PLC开关量输入接口，PLC标识为X端
*原理       : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2015/07/17       初始版本
********************************************************************************
*/

#include "drive.h"
#include "system.h"

static short * CaptureDutyRatioPointer = (short *)&Empty;
static int * CaptureFrequencyPointer = (int*)&Empty;

static void Config(DiModeEnum mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_ICInitTypeDef  TIM_ICInitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    if (mode == DiCapture)                  // 捕获代码使用1100Hz以上信号，目前只用了X3一路
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

        NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
        TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
        TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
        TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
        TIM_ICInitStructure.TIM_ICFilter = 0x0;

        TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);

        TIM_SelectInputTrigger(TIM3, TIM_TS_TI2FP2);

        TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);

        TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable);

        TIM_Cmd(TIM3, ENABLE);

        TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
    }
}

static void PortRegister(void)
{
    AppDataPointer->DI.pX0 = (uint *)BitBand(GPIOA_IDR_ADDR, 4); 
    AppDataPointer->DI.pX1 = (uint *)BitBand(GPIOA_IDR_ADDR, 5);
    AppDataPointer->DI.pX2 = (uint *)BitBand(GPIOA_IDR_ADDR, 6);
    AppDataPointer->DI.pX3 = (uint *)BitBand(GPIOA_IDR_ADDR, 7);
}

void TIM3_IRQHandler(void)
{
    static ushort IC2Value = 0;
    static short DutyRatio = 0;
    static int Frequency = 0;
    
    /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);

    /* Get the Input Capture value */
    IC2Value = TIM_GetCapture2(TIM3);

    if (IC2Value != 0)
    {
        /* Duty cycle computation */
        DutyRatio = (TIM_GetCapture1(TIM3) * 100) / IC2Value;

        /* Frequency computation */
        Frequency = SystemCoreClock / IC2Value;
    }
    else
    {
        DutyRatio = 0;
        Frequency = 0;
    }
    *CaptureFrequencyPointer = Frequency;
    *CaptureDutyRatioPointer = DutyRatio;
}


static void Register(int * frequencyPointer, short *dutyRatioPointer)
{
    CaptureFrequencyPointer = frequencyPointer;
    CaptureDutyRatioPointer = dutyRatioPointer;
}


void InitDI(void)
{
    PortRegister();
    
    System.Device.DI.Config = Config;
    System.Device.DI.Register = Register;
    Config(DiX);
}

