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
*文件名     : device_adc.c
*作用       : ADC设备
*原理       : Adc设备功能块，利用DMA实现实时数据采样
*           : 利用注册函数，回馈采样数据给上层应用变量                  
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/01       初始版本
********************************************************************************
*/

#include "drive.h"
#include "system.h"

#define Sum         5       // 4通道
#define Depth       8       // 每通道采8次


static ushort DmaBuffer[Depth][Sum];
static ushort Adc[Sum - 1];

static ushort NullValue;
static ushort * RegisterPointerBlock[Sum];


static void InitRegisterBlock(void)
{
    int i;
    for (i = 0; i < Sum; i++)
        RegisterPointerBlock[i] = &NullValue;
}

/*******************************************************************************
* 描述	    : 把应用层变量指针注册到对应的通道数组中，实现底层数据向顶层传递
* 输入参数  : adcChannel: ADC通道号
*           : dataPoint: 应用层变量指针
*******************************************************************************/
static void Register(AdcChannelEnum channel, ushort * dataPointer)
{
    RegisterPointerBlock[channel] = dataPointer;
}

static void PortRegister(void)
{
    AppDataPointer->Adc.pA0 = &Adc[0];
    AppDataPointer->Adc.pA1 = &Adc[1];
    AppDataPointer->Adc.pA2 = &Adc[2];
    AppDataPointer->Adc.pA3 = &Adc[3];
}


/*******************************************************************************
* 描述	    : 系统时钟10000/S，即0.1mS间隔调用一次运行，获取ADC值给App层变量
*******************************************************************************/
void AdcSystick10000Routine(void)
{
    int i, j;
    uint acc;
    float temp;
    for (i = 0; i < Sum; i++)
    {
        acc = 0;
        for (j = 0; j < Depth; j++)
            acc = acc + DmaBuffer[j][i];
        
        acc= acc / Depth;
        if (i < 4)
        {   
            Adc[i] = acc;
            *RegisterPointerBlock[i] = acc;
        }
        else
        {
            temp = acc;
            temp = (1.43 - (temp / 4096 * 3.3)) / 0.0043 + 25;              // 芯片温度公式
            *RegisterPointerBlock[i] = temp;                                // 误差可以达到45度
        }
    }
    AppDataPointer->Adc.A0 = Adc[0];
    AppDataPointer->Adc.A1 = Adc[1];
    AppDataPointer->Adc.A2 = Adc[2];
    AppDataPointer->Adc.A3 = Adc[3];
}

void InitAdc(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    // IO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // ADC
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);                                       //12MHz
    ADC_DeInit(ADC1);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;            
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = Sum;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1 , ADC_SampleTime_7Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 2 , ADC_SampleTime_7Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 3 , ADC_SampleTime_7Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 4 , ADC_SampleTime_7Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 5 , ADC_SampleTime_41Cycles5);   // 芯片温度
    ADC_TempSensorVrefintCmd(ENABLE);                                               // 温度使能

    ADC_DMACmd(ADC1, ENABLE);	                                            //ADC DMA Enable

    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);

    while(ADC_GetResetCalibrationStatus(ADC1));

    ADC_StartCalibration(ADC1);

    while(ADC_GetCalibrationStatus(ADC1));
    
    // DMA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);		 
	DMA_DeInit(DMA1_Channel1);		  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint)(&(ADC1->DR));
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint)DmaBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = Depth * Sum; 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);              

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    InitRegisterBlock();
    PortRegister();
    System.Device.Adc.Register = Register;
}


	

	
	 

