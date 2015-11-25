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
*文件名     : device_key.c
*作用       : 按键设备
*原理       : 基于SystemTick 100/秒 10mS扫描获取按键值，按键值分为长按与短按两种
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
********************************************************************************
*/



#include "drive.h"
#include "system.h"

#define PinBeep PaOut(4)

#define PinX0	PbOut(5)
#define PinX1	PbOut(4)

#define PinY0	PcIn(10)
#define PinY1	PcIn(11)
#define PinY2	PcIn(12)
#define PinY3	PdIn(2)

#define ShortInterval       4		// 短按按键间隔，不得低于3
#define LongInterval        40		// 长按按键间隔
#define InvalidInterval     4       // 无效识别长度
#define DoubleHitInterval   20		// 防连续双击误动间隔
#define KeyBeepInterval     20      // 按键音长度

static byte ScanData[3];

static byte ValidCounter = 0;
static byte InvalidCounter = 0;
static byte DoubleHitCounter = 0;
static byte KeyBeepCounter = 0;



static byte RemapKey(byte scan) 
{
    switch(scan)
    {
        case 0xEF:  return(0);           
        case 0xDF:  return(1);         
        case 0xBF:  return(2);            
        case 0x7F:  return(3);       
        case 0xFE:  return(4);         
        case 0xFD:  return(5);       
        case 0xFB:  return(6);   
        case 0xF7:  return(7);
        default:    return(invalid);  
    }
}

static byte RemapLongKey(byte scan) 
{
    switch(scan)
    {
        case 0xEF:  return(0x30);
        case 0xDF:  return(0x31);
        case 0xBF:  return(0x32);
        case 0x7F:  return(0x33);
        case 0xFE:  return(0x34);
        case 0xFD:  return(0x35);
        case 0xFB:  return(0x36);
        case 0xF7:  return(0x37);
        default:    return(invalid);
    }	
}

static byte ScanPin(void)
{
    byte scan = invalid;
    
    if(PinY3 == 0)  scan &= 0x7F;
    if(PinY2 == 0)  scan &= 0xBF;
    if(PinY1 == 0)  scan &= 0xDF;
    if(PinY0 == 0)  scan &= 0xEF;
    
    PinX0 = 0;
    PinX1 = 1;
    
    DelayUs(1);
    if(PinY3 == 0)  scan &= 0xF7;
    if(PinY2 == 0)  scan &= 0xFB;
    if(PinY1 == 0)  scan &= 0xFD;
    if(PinY0 == 0)  scan &= 0xFE;
    
    PinX1 = 0;
    PinX0 = 1;

    return(scan);
}


/*******************************************************************************
* 描述	    : 系统节拍100/S，即10mS一次扫描获取按键值，在多次检测确认后，
*           : 发送按键的映射消息到LogicTask的消息队列中。
*******************************************************************************/
void KeySystick100Routine(void) 
{
    byte scan;
    byte key;

    if (KeyBeepCounter == 1) PinBeep = 0;
    if (KeyBeepCounter > 0) KeyBeepCounter--;

    scan = ScanPin();
	
    if (scan != invalid) 
    {
        ValidCounter++;
        InvalidCounter = 0;
		
        if (ValidCounter == 1) 
            ScanData[0] = scan;
        else if(ValidCounter == 2)
            ScanData[1] = scan;
        else if(ValidCounter == 3)
            ScanData[2] = scan;
        else if (ValidCounter > LongInterval) 
            ValidCounter = LongInterval;
    }
    else
	{
        InvalidCounter++;
        if (InvalidCounter >= InvalidInterval)
        {   
            InvalidCounter = InvalidInterval;
       
            if(DoubleHitCounter)
            {
                DoubleHitCounter--;
                ValidCounter = 0;
                return;
            }

            if (ValidCounter < ShortInterval) return;

            if (ScanData[0] == ScanData[1])
                key = ScanData[0];
            else if (ScanData[0] == ScanData[2])
                key = ScanData[0];
            else if (ScanData[1] == ScanData[2])
                key = ScanData[1];
            else
            {
                ValidCounter = 0;
                return;
            }

            if (ValidCounter == LongInterval) 
                key = RemapLongKey(key);
            else if (ValidCounter >= ShortInterval) 
                key = RemapKey(key);
            else
                key = invalid;
            
            if (key != invalid)
            {
                PostMessage(MessageKey, key);  
                PinBeep = 1;
                KeyBeepCounter = KeyBeepInterval;
                DoubleHitCounter = DoubleHitInterval;
            }
            ValidCounter = 0;
    	} 
    }
}

void InitKey(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);

 	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
	PinX1 = 0;
	PinX0 = 1;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
 	GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    PinBeep = 0; 
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
}

