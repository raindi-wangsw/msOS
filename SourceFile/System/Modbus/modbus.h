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
*文件名     : modbus.h
*作用       : 扩展HMI屏
*原理       : 通过标准Modbus协议扩展工业用HMI屏
*           : 通过Modbus基于PLC定义的四类地址
*           : 通过四个数组，数组的序号，也即HMI屏数据地址关联数据指针
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2014/11/8       初始版本
********************************************************************************
*/
#ifndef __MODBUS_H
#define __MODBUS_H

#define ReadDi1x        0x02    // 读1X地址:10001~19999,PLC数字输入端口(触点)，比特访问,最多2000bit
#define ReadDo0x        0x01    // 读0X地址:00001~09999,PLC数字输出端口(线圈)，比特访问,最多2000bit
#define ReadAdc3x       0x04    // 读3X地址:30001~39999,PLC模拟输入端口(寄存器)，16bit访问,最多125
#define ReadReg4x       0x03    // 读4X地址:40001~49999,PLC内部寄存器(寄存器)，16bit访问,最多125

#define WriteOneDo0x    0x05    // 写单个0X地址:00001~09999,PLC数字输出端口(线圈)，比特访问
#define WriteOneReg4x   0x06    // 写单个4X地址:40001~49999,PLC内部寄存器(寄存器)，16bit访问
#define WriteDo0x       0x0F    // 写多个0X地址:00000~09999,PLC数字输出端口(线圈)，比特访问,最多1968bit
#define WriteReg4x      0x10    // 写多个4X地址:40001~49999,PLC内部寄存器(寄存器)，16bit访问,最多120

#define ReadWriteReg4x  0x17    // 读写多个4X地址:40001~49999,PLC内部寄存器(寄存器)，16bit访问

#define MaskReg         0x16    // 屏蔽写寄存器，用于设置或者清楚寄存器中的单个比特，类似SetBit or ReSetBit
#define ReadDeviceID    0x2B    // 读取设备识别码

typedef struct
{
    DiStruct DI;
    DoStruct DO;
    AdcStruct Adc;
    int Reg;
}SubPlcStruct;

extern const byte CrcHighBlock[256];
extern const byte CrcLowBlock[256];
extern ushort Crc16(byte *bufferPointer, int sum);

extern void InitModbus(void);
#endif


