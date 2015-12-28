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
*文件名     : gui.h
*作用       : 
*原理       : 
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2014/06/21       初始版本
********************************************************************************
*/



#ifndef __GUI_H
#define __GUI_H




typedef enum 
{
    GuiDataTypeByteDec,
    GuiDataTypeShortDec,
    GuiDataTypeUshortDec,
    GuiDataTypeIntDec,
    GuiDataTypeUintDec,
    GuiDataTypeFloatDec,
    GuiDataTypeUshortHex,
    GuiDataTypeUintHex,
    GuiDataTypeString,              // 直接显示字符串
    GuiDataTypeSnString             // 序号显示字符串(与字符串数组配套)
}GuiDataType;

typedef enum
{
	GuiDataAlignLeft,
	GuiDataAlignRight
}GuiDataAlign;

typedef struct
{
    char Character;
    byte Column[16];
}Chart;

typedef struct LabelSelf
{
    byte X;
    byte Y;
    
    GuiDataType Type;
    GuiDataAlign Align;
    ushort Digits;
    
    float Offset;
    float Coefficient;
    
    void * DataPointer;
    const string * StringBlockPointer;
    struct LabelSelf * NextLabelPointer;
}Label;

typedef struct TextBoxSelf
{
    byte X;
    byte Y;
    
    GuiDataType Type;
    GuiDataAlign Align;
    ushort Digits;
    
    float Offset;
    float Coefficient;
    
    void * DataPointer;
    const string * StringBlockPointer;
    struct TextBoxSelf * NextTextBoxPointer;

    int DataMax;
    int DataMin;
    int DataStep;
    int DataBigStep;
}TextBox;

typedef struct
{
    const byte* BackTextPointer;
    Chart     * ChartPointer;
    Label     * LabelPointer;
    TextBox   * TextBoxPointer;
    TextBox   * FocusTextBoxPointer;
}Form;


extern void InitGui(void);
    
#endif 




