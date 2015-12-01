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
*文件名     : device_usart3.c
*作用       : 串口设备,用于HMI屏
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2015/11/20       初始版本
********************************************************************************
*/
#include "drive.h"
#include "system.h"

#define Direct PbOut(8)

typedef void (*Rxd3Function)(byte data);
Rxd3Function Rxd3;

static void Register(uint txdAddress, uint rxdFucntionAddress)
{
    DMA_InitTypeDef DMA_InitStucturer;
    DMA_InitStucturer.DMA_MemoryBaseAddr = txdAddress;  
    DMA_InitStucturer.DMA_BufferSize = 0;  
    DMA_InitStucturer.DMA_PeripheralBaseAddr = (uint)(&(USART3->DR));  
    DMA_InitStucturer.DMA_DIR = DMA_DIR_PeripheralDST;  
    DMA_InitStucturer.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
    DMA_InitStucturer.DMA_MemoryInc = DMA_MemoryInc_Enable;  
    DMA_InitStucturer.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    DMA_InitStucturer.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;   
    DMA_InitStucturer.DMA_Mode = DMA_Mode_Normal;  
    DMA_InitStucturer.DMA_Priority = DMA_Priority_High;  
    DMA_InitStucturer.DMA_M2M = DMA_M2M_Disable; 
    DMA_Init(DMA1_Channel2,&DMA_InitStucturer);  

    Rxd3 = (Rxd3Function)rxdFucntionAddress;
}
   
void USART3_IRQHandler(void)
{ 
    byte data;  
    
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {    
        data = USART_ReceiveData(USART3);
        
        if(USART_GetITStatus(USART3, USART_FLAG_PE) == RESET)
            Rxd3(data);
    }
    
	if(USART_GetFlagStatus(USART3,USART_FLAG_ORE)==SET)
		USART_ReceiveData(USART3);
}



static void WriteToUsart3(byte * dataPointer, int sum)
{
    Direct = 1;
    
    DMA_Cmd(DMA1_Channel2, DISABLE);
    // DMA发送完成中断早于发送数据2个Byte的时间，需要增加补偿，否则导致RS485切换丢失发送数据
    DMA1_Channel2->CNDTR = sum + 2; 
    DMA_Cmd(DMA1_Channel2, ENABLE);  

}

void DMA1_Channel2_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC2) == SET)
    {
        DMA_ClearITPendingBit(DMA1_IT_TC2);
        Direct = 0;
    }
}

void InitUsart3(void)
{
// GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;   
    DMA_InitTypeDef DMA_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);  

// RS485_DIR   PB.8
    Direct = 0;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
// USART3_TX   PB.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
// USART3_RX   PB.11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);  

// Usart3 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); 

// USART 初始化设置
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &USART_InitStructure);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);   //开启中断
    USART_Cmd(USART3, ENABLE);                       //使能串口
    
// DMA NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;      
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); 
// DMA 初始化
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  
    DMA_DeInit(DMA1_Channel2);  

    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART3->DR));  
    DMA_InitStructure.DMA_MemoryBaseAddr = 0;  

    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  
    DMA_InitStructure.DMA_BufferSize = 8;  
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;   
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  
    DMA_Init(DMA1_Channel2,&DMA_InitStructure);  
    
    DMA_ITConfig(DMA1_Channel2, DMA_IT_TC,ENABLE);
    
    USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);

  
    //DMA_Cmd(DMA1_Channel2, ENABLE); 

    System.Device.Usart3.Register = Register;
    System.Device.Usart3.Write = WriteToUsart3; 
}

