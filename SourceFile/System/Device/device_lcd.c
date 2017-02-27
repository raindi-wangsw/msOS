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
*文件名     : device_lcd.c
*作用       : 128*64带字库黑白屏设备
*原理       : 128*64字库黑白屏，分为四行，字库汉字部分支持GB2312编码，
*           : Keil编译器直接支持GB2312，汉字每个16*16点阵，一行可以显示8个汉字
*           : 字母支持ASCII标准，每个8*16，一行可以显示16个字母
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
********************************************************************************
*/



#include "drive.h"
#include "system.h"


static uint * pPinClk;
static uint * pPinCs;
static uint * pPinData;
static uint * pPinReset;

const char Array[4] = 
{
    0x80, 0x90, 0x88, 0x98
};
static void SendInstruct(byte instruct)
{
    short i;
    uint variable;

    variable = 0x00F80000 | ((instruct & 0xF0) << 8) |((instruct & 0x0F) << 4); 

    *pPinClk = 0;	
    *pPinCs = 1;
    
    for (i = 23; i > -1; i--) 
    {
        if(GetBit(variable, i)) 
            *pPinData = 1;
        else 
            *pPinData = 0;
        DelayUs(10);     // 7不能正常工作，考虑余量，取10
        *pPinClk = 1;
        DelayUs(10);
        *pPinClk = 0;
        DelayUs(10);
    }
    *pPinCs = 0;
}

static void SendData(byte data)
{
    short i;
    uint variable;
    variable = 0x00FA0000 | ((data & 0xF0) << 8) |((data & 0x0F) << 4); 

    *pPinClk = 0;
    *pPinCs = 1;

    for (i = 23; i > -1; i--) 
    {
        if (GetBit(variable, i)) 
            *pPinData = 1;
        else 
            *pPinData = 0;
        DelayUs(10);
        *pPinClk = 1;
        DelayUs(10);
        *pPinClk = 0;
        DelayUs(10);
    }
    *pPinCs = 0;
}


/*******************************************************************************
* 描述	    : Lcd屏显示函数，显示一整行16个字母或者8个汉字，汉字必须要在偶数位置开始
* 输入参数  : y: 行号0、1、2、3
*           : string 字符串，必须要16个char，一个汉字等于两个char
*******************************************************************************/
static void LcdDisplayString(byte y, string string)
{ 
    byte i;
    	
    SendInstruct(Array[y]);

    for (i = 0; i < 16; i++)
        SendData(*string++);
}

#if 0
static void LcdDisplayGraphy(void)
{
    int i, j, k;
    
    SendInstruct(0x34);
    SendInstruct(0x36);

    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 32; j++)
        {
            SendInstruct(0x80 + j);
            if (i == 0)
                SendInstruct(0x80);
            else
                SendInstruct(0x88);
        
            for (k = 0; k < 16; k++)
                SendData(0xFF);
        }
    }
    SendInstruct(0x34);
    SendInstruct(0x30);
}
#endif

void InitLcd(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    pPinClk = (uint *)BitBand(GPIOC_ODR_ADDR, 7);
    pPinCs = (uint *)BitBand(GPIOC_ODR_ADDR, 9);
    pPinData = (uint *)BitBand(GPIOC_ODR_ADDR, 8);
    pPinReset = (uint *)BitBand(GPIOC_ODR_ADDR, 6);
    
    *pPinClk = 0;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    *pPinCs = 0;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);	
    *pPinData = 0;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);	
    *pPinReset = 0;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);	
    	
    DelayMs(100);
    *pPinReset = 1;
    
    SendInstruct(0x30);
    SendInstruct(0x0C);
    SendInstruct(0x01);
    SendInstruct(0x02);
    SendInstruct(0x80);
	
    SendInstruct(0x01);

    System.Device.Lcd.DisplayString = LcdDisplayString;
}





