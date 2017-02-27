/*
********************************************************************************
*                                嵌入式微系统
*                                    msOS
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
**                        论坛:http://bbs.huayusoft.com
*                        淘宝店:http://52edk.taobao.com
*                博客:http://forum.eet-cn.com/BLOG_wangsw317_1268.HTM
********************************************************************************
*文件名     : device_do.c
*作用       : PLC开关量输出接口，PLC标识为X端
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2015/07/17       初始版本
********************************************************************************
*/

#include "drive.h"
#include "system.h"


static void Config(DoModeEnum mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    
    
    if (mode == DoY)
    {
        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
    else
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_AFIO, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
        
        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure); 
        
        GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure); 
    }
}

void DoSystick10000Routine(void)
{
    *AppDataPointer->DO.pY0 = AppDataPointer->DO.Y0;   
    *AppDataPointer->DO.pY1 = AppDataPointer->DO.Y1;
    *AppDataPointer->DO.pY2 = AppDataPointer->DO.Y2;
    *AppDataPointer->DO.pY3 = AppDataPointer->DO.Y3;
    *AppDataPointer->DO.pY4 = AppDataPointer->DO.Y4;
    *AppDataPointer->DO.pY5 = AppDataPointer->DO.Y5;
}

/*******************************************************************************
* 描述	    : 打开对应通道的PWM，这里默认系统时钟频率是 72MHz
* 输入参数  : channel: 开启哪路Pwm
*******************************************************************************/
static void Open(PwmEnum channel)
{
    switch(channel)
    {
        case PwmChannel0:
            TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Enable);
            break;
        case PwmChannel1:
            TIM_CCxCmd(TIM3, TIM_Channel_3, TIM_CCx_Enable);
            break;   
        case PwmChannel2:
            TIM_CCxCmd(TIM3, TIM_Channel_4, TIM_CCx_Enable);
            break;
        case PwmChannel3:
            TIM_CCxNCmd(TIM1,TIM_Channel_1, TIM_CCxN_Enable);
            break; 
        case PwmChannel4:
            TIM_CCxNCmd(TIM1,TIM_Channel_2, TIM_CCxN_Enable);
            break;
        case PwmChannel5:
            TIM_CCxNCmd(TIM1,TIM_Channel_3, TIM_CCxN_Enable);
            break; 
        default:
            break;
    }
}

/*******************************************************************************
* 描述	    : 关闭通道
* 输入参数  : channel:哪一路Pwm
*******************************************************************************/
static void Close(PwmEnum channel)
{   
    switch(channel)
    {
        case PwmChannel0:
            TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Disable);
            break;
        case PwmChannel1:
            TIM_CCxCmd(TIM3, TIM_Channel_3, TIM_CCx_Disable);
            break;   
        case PwmChannel2:
            TIM_CCxCmd(TIM3, TIM_Channel_4, TIM_CCx_Disable);
            break;
        case PwmChannel3:
            TIM_CCxNCmd(TIM1,TIM_Channel_1, TIM_CCxN_Disable);
            break; 
        case PwmChannel4:
            TIM_CCxNCmd(TIM1,TIM_Channel_2, TIM_CCxN_Disable);
            break;
        case PwmChannel5:
            TIM_CCxNCmd(TIM1,TIM_Channel_3, TIM_CCxN_Disable);
            break; 
        default:
            break;
    }
}

/*******************************************************************************
* 描述	    : 设置PWM占空比
* 输入参数  : channel:哪一路Pwm
*******************************************************************************/
static void SetDutyRatio(PwmEnum channel, int dutyRatio)
{
    switch(channel)
    {
        case PwmChannel0:
            TIM_SetCompare2(TIM3, dutyRatio);
            break;
        case PwmChannel1:
            TIM_SetCompare3(TIM3, dutyRatio);
            break;
        case PwmChannel2:
            TIM_SetCompare4(TIM3, dutyRatio);
            break;
        case PwmChannel3:
            TIM_SetCompare1(TIM1, dutyRatio);
            break;
        case PwmChannel4:
            TIM_SetCompare2(TIM1, dutyRatio);
            break;
        case PwmChannel5:
            TIM_SetCompare3(TIM1, dutyRatio);
            break;
        default:
            break;
    }
}

/******************************************************************************
* 描述	    : 设置PWM参数，频率 = 72000000 / Prescaler / Period
* 输入参数  : channel: 哪一路Pwm; Prescaler:72MHz时钟预分频; Period:脉冲周期时钟数
*           : PwmChannel0~PwmChannel1:Prescaler < 65536, Period < 65536。  16bit计数器
*           : PwmChannel3~PwmChannel5:Prescaler < 65536, Period < 65536。  16bit计数器
*******************************************************************************/
static void SetParameter(PwmEnum channel, int prescaler, int period, int dutyRatio)
{
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;  
    
    // 频率=72000000 / Prescaler / Period
    TIM_TimeBaseStructure.TIM_Prescaler     = prescaler - 1;
    TIM_TimeBaseStructure.TIM_Period        = period - 1; 
    
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_Pulse = dutyRatio;

    switch(channel)
    {
        case PwmChannel0:
            TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
            
            TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;
            TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High;
            TIM_OC2Init(TIM3, &TIM_OCInitStructure);                      
            TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); 
            TIM_ARRPreloadConfig(TIM3, ENABLE);
            TIM_Cmd(TIM3, ENABLE);
            break;
        case PwmChannel1:
            TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
            
            TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;
            TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High;
            TIM_OC3Init(TIM3, &TIM_OCInitStructure);                      
            TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable); 
            TIM_ARRPreloadConfig(TIM3, ENABLE);
            TIM_Cmd(TIM3, ENABLE);       
            break;  
        case PwmChannel2:
            TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
            
            TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;
            TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High;
            TIM_OC4Init(TIM3, &TIM_OCInitStructure);                      
            TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable); 
            TIM_ARRPreloadConfig(TIM3, ENABLE);
            TIM_Cmd(TIM3, ENABLE);  
            break;
        case PwmChannel3:
            TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);  
            
            TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Disable;
            TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;     
            TIM_OCInitStructure.TIM_OCNPolarity  = TIM_OCNPolarity_High;  
            TIM_OC1Init(TIM1, &TIM_OCInitStructure);
            TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  
            TIM_ARRPreloadConfig(TIM1, ENABLE);
            TIM_Cmd(TIM1, ENABLE);     
            TIM_CtrlPWMOutputs(TIM1, ENABLE); 
            break;            
        case PwmChannel4: 
            TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);  
            
            TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Disable;
            TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;     
            TIM_OCInitStructure.TIM_OCNPolarity  = TIM_OCNPolarity_High;  
            TIM_OC2Init(TIM1, &TIM_OCInitStructure); 
            TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable); 
            TIM_ARRPreloadConfig(TIM1, ENABLE);
            TIM_Cmd(TIM1, ENABLE);
            TIM_CtrlPWMOutputs(TIM1, ENABLE); 
            break;    
        case PwmChannel5:   
            TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);  
            
            TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Disable;
            TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;     
            TIM_OCInitStructure.TIM_OCNPolarity  = TIM_OCNPolarity_High;  
            TIM_OC3Init(TIM1, &TIM_OCInitStructure); 
            TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
            TIM_ARRPreloadConfig(TIM1, ENABLE);
            TIM_Cmd(TIM1, ENABLE);
            TIM_CtrlPWMOutputs(TIM1, ENABLE); 
            break;
        default:
            break;
    }  
}


/*******************************************************************************
* 描述	    : 初始化PWM设备
*******************************************************************************/
void InitDO(void)
{    
    AppDataPointer->DO.pY0 = (bool *)BitBand(GPIOA_ODR_ADDR, 7);   
    AppDataPointer->DO.pY1 = (bool *)BitBand(GPIOB_ODR_ADDR, 0);
    AppDataPointer->DO.pY2 = (bool *)BitBand(GPIOB_ODR_ADDR, 1);
    AppDataPointer->DO.pY3 = (bool *)BitBand(GPIOB_ODR_ADDR, 13);
    AppDataPointer->DO.pY4 = (bool *)BitBand(GPIOB_ODR_ADDR, 14);
    AppDataPointer->DO.pY5 = (bool *)BitBand(GPIOB_ODR_ADDR, 15);
    
    System.Device.DO.Config = Config;
    System.Device.DO.Pwm.Open = Open;
    System.Device.DO.Pwm.Close  = Close;
    System.Device.DO.Pwm.SetDutyRatio = SetDutyRatio;
    System.Device.DO.Pwm.SetParameter = SetParameter;   

    Config(DoY);
}

