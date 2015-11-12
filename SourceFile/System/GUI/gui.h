/*
********************************************************************************
*                                Ƕ��ʽ΢ϵͳ
*                                   msOS
*
*                            Ӳ��ƽ̨:msPLC DEMO
*                          ��оƬ:STM32F103R8T6/RBT6
*                           ��������οƼ����޹�˾
*
*                                ����:����ΰ
*                                ����:������
*                                ��ʶ:Wangsw
*
*                                QQ:26033613
*                               QQȺ:291235815
*                        �Ա���:http://52edk.taobao.com
*                      ��̳:http://gongkong.eefocus.com/bbs/
*                ����:http://forum.eet-cn.com/BLOG_wangsw317_1268.HTM
********************************************************************************
*�ļ���     : gui.h
*����       : 
*ԭ��       : 
********************************************************************************
*�汾     ����            ����            ˵��
*V0.1    Wangsw        2014/06/21       ��ʼ�汾
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
    GuiDataTypeUintHex,
    GuiDataTypeString,              // ֱ����ʾ�ַ���
    GuiDataTypeSnString             // �����ʾ�ַ���(���ַ�����������)
}DataType;

typedef enum
{
	GuiDataAlignLeft,
	GuiDataAlignRight
}DataAlign;

typedef struct
{
    char Character;
    byte Column[16];
}Chart;

typedef struct LabelSelf
{
    byte X;
    byte Y;
    
    DataType Type;
    DataAlign Align;
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
    
    DataType Type;
    DataAlign Align;
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



