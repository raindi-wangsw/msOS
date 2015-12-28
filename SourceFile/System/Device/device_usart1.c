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
*文件名     : device_usart1.c
*作用       : 串口设备
*原理       : 发送采用Buffer发送，接收采用App注册函数方式接收
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
*V0.2    Wangsw        2013/09/13       第一次更改
********************************************************************************
*/

#include "drive.h"
#include "system.h"

typedef struct 
{
    byte    * Start;            //指向队列开始 
    byte    * End;              //指向队列结束 
    byte    * In;               //插入一个消息  
    byte    * Out;              //取出一个消息  
    ushort    Entries;     //消息长度      
}QueueStruct;

#define Txd1BufferSum 1000
static QueueStruct Txd1Queue;
static byte Txd1Buffer[Txd1BufferSum];

static void InitQueue(void)
{
    Txd1Queue.Start = Txd1Buffer;
    Txd1Queue.End = Txd1Buffer + Txd1BufferSum - 1;
    Txd1Queue.In = Txd1Queue.Start;
    Txd1Queue.Out = Txd1Queue.Start;
    Txd1Queue.Entries = 0;
}

/*******************************************************************************
* 描述	    : 串口1发送一个字节
* 输入参数  : data 字节类型，发送参数
* 返回参数  : bool 类型
*******************************************************************************/
static bool WriteByteToUsart1(byte data) 
{
    if (Txd1Queue.Entries >= Txd1BufferSum) return (false);
    
    *Txd1Queue.In++ = data;	
    
    if (Txd1Queue.In > Txd1Queue.End) 
        Txd1Queue.In = Txd1Queue.Start;

    EnterCritical();
    Txd1Queue.Entries++;
    ExitCritical();
    return (true);
}

/*******************************************************************************
* 描述	    : 串口1发送一串数据
* 输入参数  : dataPointer 数据指针，sum 数据总数
*******************************************************************************/
static void WriteToUsart1(byte * dataPointer, int sum)
{
    if (sum)
    {
        while (sum--) 
            WriteByteToUsart1(*dataPointer++);
    }
    else
    {
        while (*dataPointer) 
        {
            if (*dataPointer == '\n')       //换行符
                WriteByteToUsart1('\r');    //增加回车符
            WriteByteToUsart1(*dataPointer++);
        }
    }
}

/*******************************************************************************
* 描述	    : Keil编译器支持printf函数，需要调用此函数，注意这个需要MicorLib支持
* 输入参数  : ch 实为byte类型，实际发送的数据
*           : f 设备，不需要处理
*******************************************************************************/
int fputc(int ch, FILE *f)
{
    if (ch == '\n')                         //换行符
        WriteByteToUsart1('\r');            //增加回车符

    WriteByteToUsart1((byte)ch);

    return ch;
}

/*******************************************************************************
* 描述	    : 系统节拍1000/S,即1mS一次调用此服务程序发送缓冲区数据
*******************************************************************************/
void Usart1Systick1000Routine(void) 
{
    if (Txd1Queue.Entries == 0) return;

    if ((USART1->SR&0X40)==0) return;

    

    USART1->DR = *Txd1Queue.Out++;
    
    if (Txd1Queue.Out > Txd1Queue.End) 
        Txd1Queue.Out = Txd1Queue.Start;

    EnterCritical();
    Txd1Queue.Entries--;
    ExitCritical();
}

/*******************************************************************************
* 描述	    : 串口中断处理函数
*******************************************************************************/
void USART1_IRQHandler(void) 
{
    byte data;
    
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
    {
        data = (byte)USART_ReceiveData(USART1);
        
        PostMessage(MessageKey, data);
    } 
    
	if(USART_GetFlagStatus(USART1,USART_FLAG_ORE)==SET)
		USART_ReceiveData(USART1);
} 

static void OpenUsart1(void)
{
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

static void CloseUsart1(void)
{
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    USART_Cmd(USART1, DISABLE);
}
     
void InitUsart1(void) 
{
//GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
//USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
//USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

//Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;

    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	
  
//USART 初始化设置
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);          //开启中断
   
    USART_Cmd(USART1, ENABLE);                              //使能串口 

    InitQueue();

    System.Device.Usart1.Open = OpenUsart1;
    System.Device.Usart1.Close = CloseUsart1;
    System.Device.Usart1.WriteByte = WriteByteToUsart1;
    System.Device.Usart1.Write = WriteToUsart1;
}


