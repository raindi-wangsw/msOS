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

#define PinBeep PaOut->Bit4

#define PinX0   PcIn->Bit10
#define PinX1   PcIn->Bit11
#define PinX2   PcIn->Bit12
#define PinX3   PdIn->Bit2

#define PinY0   PbOut->Bit5
#define PinY1   PbOut->Bit4

#define ShortInterval       2		// 短按间隔
#define LongInterval        20		// 长按间隔
#define InvalidInterval     2       // 无效间隔
#define DoubleHitInterval   10		// 双击间隔
#define KeyBeepInterval     10      // 按键音响的时间

static byte Scan = invalid;
static byte ScanData;

static byte ValidCounter = 0;
static byte InvalidCounter = 0;
static byte DoubleHitCounter = 0;
static byte KeyBeepCounter = 0;
static bool Enable = false;


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

static void EnableKey(bool status)
{
    Enable = status;
}

void KeySystick100Routine(void) 
{
    byte key;
    static bool Switch = false;
    Switch = ~Switch;
    if (Switch)
    {
        key = invalid;
        
        if(PinX3 == 0)  key &= 0x7F;
        if(PinX2 == 0)  key &= 0xBF;
        if(PinX1 == 0)  key &= 0xDF;
        if(PinX0 == 0)  key &= 0xEF;
        
        PinY0 = 0;
        PinY1 = 1;
        
        Delay(1);
        if(PinX3 == 0)  key &= 0xF7;
        if(PinX2 == 0)  key &= 0xFB;
        if(PinX1 == 0)  key &= 0xFD;
        if(PinX0 == 0)  key &= 0xFE;
        
        PinY1 = 0;
        PinY0 = 1;

        if (Enable)
            Scan = key;
        else
            Scan = invalid;
    }
    else
    {
        if (KeyBeepCounter == 1) PinBeep = 0;
            
        if (KeyBeepCounter > 0) KeyBeepCounter--;
    	
        if (Scan != invalid) 
        {
            ScanData = Scan;
            InvalidCounter = 0;
    		
            if (++ValidCounter > LongInterval) 
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

                if (ValidCounter == LongInterval) 
                    key = RemapLongKey(ScanData);
                else if (ValidCounter >= ShortInterval) 
                    key = RemapKey(ScanData);
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
}

void InitKey(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST,ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

    PinY1 = 0;
    PinY0 = 1;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    PinBeep = 0;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //Input
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    System.Device.Key.Enable = EnableKey;
}

