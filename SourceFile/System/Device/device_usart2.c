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
*文件名     : device_usart2.c
*作用       : 串口设备,用于HMI屏
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2015/11/20       初始版本
********************************************************************************
*/
#include "drive.h"
#include "system.h"

static void Usart2Dummy(byte data){;}
typedef void (*Rxd2Function)(byte data);
Rxd2Function Rxd2 = Usart2Dummy;

static void Register(uint rxdFucntion)
{
    Rxd2 = (Rxd2Function)rxdFucntion;
}
   
void USART2_IRQHandler(void)
{ 
    byte data;  
#if 0
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {    
        data = USART_ReceiveData(USART2);
        Rxd2(data);
    }
    
	if(USART_GetFlagStatus(USART2,USART_FLAG_ORE)==SET)
		USART_ReceiveData(USART2);
#else  
    uint sr;
    sr = USART2->SR;


    if (sr & USART_FLAG_RXNE)
    {
        data = USART_ReceiveData(USART2);
        Rxd2(data);
    }
    
    if (sr & USART_FLAG_ORE)
    {
        USART_ReceiveData(USART2);
    }
#endif 
}



static void WriteToUsart2(byte * dataPointer, int sum)
{
    DMA_Cmd(DMA1_Channel7, DISABLE);
    DMA1_Channel7->CMAR = (uint)dataPointer;
    DMA1_Channel7->CNDTR = sum;
    DMA_Cmd(DMA1_Channel7, ENABLE);  
}


void InitUsart2(void)
{

// GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;   
    DMA_InitTypeDef DMA_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  
    
// USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
// USART2_RX   PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

// Usart2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART2

// DMA 初始化
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
    DMA_DeInit(DMA1_Channel7);  

    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART2->DR));  
    DMA_InitStructure.DMA_MemoryBaseAddr = 0;  

    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
    DMA_InitStructure.DMA_BufferSize = 8;  
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;   
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
    DMA_Init(DMA1_Channel7,&DMA_InitStructure);  

// USART 初始化设置
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);   //开启中断
    USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);
    USART_Cmd(USART2, ENABLE);                       //使能串口
  
    //DMA_Cmd(DMA1_Channel7, ENABLE); 

    System.Device.Usart2.Register = Register;
    System.Device.Usart2.Write = WriteToUsart2; 
}

