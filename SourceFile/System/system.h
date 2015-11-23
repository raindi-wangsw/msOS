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
*                                标识:wangsw
*
*                                QQ:26033613
*                               QQ群:291235815
*                        淘宝店:http://52edk.taobao.com
*                      论坛:http://gongkong.eefocus.com/bbs/
*                博客:http://forum.eet-cn.com/BLOG_wangsw317_1268.HTM
********************************************************************************
*文件名   : system.h
*作用     : 系统库全局头文件
*创建时间 : 2013.7.21
********************************************************************************
*/


#ifndef __SYSTEM_H
#define __SYSTEM_H


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>



/*
1、类型定义兼容C#，尽可能用C#标准，常用的为int
2、char在C#中为16bit的unicode，keil C下为8bit
*/
//8bit
//typedef char                sbyte;	    /*char Keil4默认定义为byte，若要带符号，需要更改keil设置*/
typedef unsigned char       byte;       
//16bit
//typedef short             short
typedef unsigned short      ushort;
//32bit
//typedef int               int;
typedef unsigned int        uint;

//字符串
typedef char *              string;

typedef unsigned char       bool;

typedef void (*function)(void);
#define Function(address)   ((function)(address))()

typedef enum
{
    MessageKey      = 0x01,     // 消息类型不能为0x00
    MessageTimer    = 0x02
    /*请填充类型*/
}MessageEnum;

#include "os.h"
#include "gui.h"
#include "device.h"  
#include "data.h"





#define LCD12864
//#define msMenu

#define true                        1
#define false                       0

#define on                          1
#define off                         0

#define ok                          1
#define yes                         1
#define no                          0

#define null                        ((void *) 0)                    // 空指针
#define invalid                     0xFF                            // 无效值
#define MainClock                   72000000                        // 系统时钟频率
#define IdentifyNumberAddress       0x1FFFF7E8                      // 芯片ID号

#define SetBit(data, offset)        ((data) |= 1U << (offset))      // 置位
#define ResetBit(data, offset)      ((data) &= ~(1U << (offset)))   // 复位
#define GetBit(data, offset)        (((data) >> (offset)) & 0x01)   // 获取位

#define Byte0(data)                 ((byte *)(&(data)))[0]
#define Byte1(data)                 ((byte *)(&(data)))[1]
#define Byte2(data)                 ((byte *)(&(data)))[2]
#define Byte3(data)                 ((byte *)(&(data)))[3]

#define Ushort0(data)               ((ushort *)(&(data)))[0]
#define Ushort1(data)               ((ushort *)(&(data)))[1]

#define Byte(data)                  *((byte *)(&data)) 
#define Ushort(data)                *((ushort *)(&data)) 
#define Uint(data)                  *((uint *)(&data)) 
#define Float(data)                 *(float *)(&data)

#define pByte(address)              *((byte *)(address)) 
#define pUshort(address)            *((ushort *)(address)) 
#define pUint(address)              *((uint *)(address)) 
#define pFloat(address)             *(float *)(address)

#define Assert(express)  if (!(express)){printf("\nASSERT: " #express "\n");}   //(void)0
 				  
extern uint Empty;
extern void Dummy(void);
extern ushort CriticalNesting;
#define	EnterCritical()		__disable_irq(); CriticalNesting++;
#define ExitCritical()		if(--CriticalNesting == 0){__enable_irq();}

#define EnableIrq()         __enable_irq();
#define DisableIrq()        __disable_irq();

extern uint RomBase;
extern uint RamBase;
extern DataStruct * AppDataPointer;

extern void Delay(int times);

extern void DelayUs(int times);

extern void DelayMs(int times);

extern byte HexToAscii(byte hex);

extern void PostMessage(MessageEnum message, uint data);

/*******************************************************************************
* 结构体名	: SystemStruct
* 描述	    : msOS中最重要的一个结构体，封装了System层的所有接口
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/09/11       初始版本
*******************************************************************************/
typedef struct
{
    void (*Init)(void);
    
    struct Device
    {     
        struct Adc
        {
            void (*Register)(AdcChannelEnum channel, ushort * dataPointer);
        }Adc;

        struct DI
        {
            void (*Config)(DiModeEnum mode);
            void (*Register)(int * frequencyPointer, short *dutyRatioPointer);
           
            struct Capture
            {
                void (*SetParameter)(PwmEnum channel);
            }Capture;
        }DI;
        
        struct DO
        {
            void (*Config)(DoModeEnum mode);
            struct Pwm
            {
                void (*Open)(PwmEnum channel);
                void (*Close)(PwmEnum channel); 
                void (*SetParameter)(PwmEnum channel, int prescaler, int period, int dutyRatio);
                void (*SetDutyRatio)(PwmEnum channel, int dutyRatio);
            }Pwm;
        }DO;

        struct Misc
        {
            void (*SetBeep)(bool status);
        }Misc;
        
        struct Lcd
        {
            void (* DisplayString)(byte y, string string);
        }Lcd;
        
        struct Usart1
        {
            void (*Open)(void);
            void (*Close)(void);
            bool (*WriteByte)(byte data);
            void (*Write)(byte * dataPointer, int sum);
        }Usart1;

        
        struct Timer
        {
            void (*Start)(int id, TimerModeEnum mode, int times, function functionPointer);
            void (*Stop)(int id); 
        }Timer;

        struct Storage
        {
            struct Parameter
            {
                void (*ReadAll)(void);
                bool (*Write)(void * dataPointer);  
            }Parameter;
            struct Log
            {
                void (*Erase)(void);
                char * (*Read)(int sequence);
                bool (*Write)(char *fmt, ...);
            }Log;
        }Storage;

        struct Systick
        {
            bool (*Register)(SystickEnum type, function functionPointer);
        }Systick;
        
    }Device;

    struct OS
    {
        void (* CreateLogicTask)(function taskPointer);
        void (* CreateMenuTask)(function taskPointer);
        void (* Start)(void);
        bool (* PostMessageQueue)(uint message);
        uint (* PendMessageQueue)(void);
        void (* DelayMs)(int times); 
    }OS;

    struct Gui
    {
        void (*Parse)(Form * formPointer);

        struct
        {
            void (* AddLabel)(Form * formPointer, Label *labelPointer);
            void (* AddTextBox)(Form * formPointer, TextBox *textBoxPointer);
            void (* Init)(Form *formPointer);
            void (*SwitchTextBoxFocus)(void);
            void (*ModifyTextBoxData)(KeyEnum key);
        }Form;
    }Gui;
}SystemStruct;

extern SystemStruct System;

#endif 
