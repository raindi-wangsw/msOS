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
*文件名     : data.h
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2015/08/29       初始版本
********************************************************************************
*/


#ifndef __DATA_H
#define __DATA_H


typedef struct
{
    int Frequency;                 // Frq
    int PowerPercent;
    int Power;
    int Voltage;
    int Current;
    int Temperature;  
    string OnOff;    

    int MaxPower;
    int MaxTemperature;
    int MaxFrequency;
    int MaxFrequencyOffset;
    float MaxPress;
    int State;

// 以下为系统保留数据，系统层直接使用
    uint Systick1000;
    RtcStruct Rtc;

    uint SerialNumber;              // SN
    int Year;
    int Month;
    int Day;
    uint IdentifyNumber0;           // ID0
    uint IdentifyNumber1;           // ID1
    uint IdentifyNumber2;           // ID2
    
    AdcStruct Adc;
    DiStruct DI;
    DoStruct DO;
}DataStruct;


#endif /*__Data_H*/

