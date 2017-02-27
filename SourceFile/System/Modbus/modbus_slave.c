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
*文件名     : modbus_slave.c
*作用       : 扩展HMI屏
*原理       : 通过标准Modbus协议扩展工业用HMI屏
*           : 通过Modbus基于PLC定义的四类地址
*           : 通过四个数组，数组的序号，也即HMI屏数据地址关联数据指针
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2014/11/8       初始版本
********************************************************************************
*/

#include "system.h"

#define DeviceID                    1
#define RxdTimeoutSum               2

#define TxdBufferSum                256
#define RxdBufferSum                256
#define DataPointer                 ((DataStruct *)0)

//************************************************************************************
//与HMI关联数据，填入数据相应的地址
static const bool * DoBlock[] =                // DataOutPort
{
    &DataPointer->DO.Y0, &DataPointer->DO.Y1, &DataPointer->DO.Y2, &DataPointer->DO.Y3, &DataPointer->DO.Y4, &DataPointer->DO.Y5
};

static const bool * DiBlock[] =                // DataInPort
{
    &DataPointer->DI.X0, &DataPointer->DI.X1, &DataPointer->DI.X2, &DataPointer->DI.X3
};
static const ushort * AdcBlock[] =              // AdcInPort
{
    &DataPointer->Adc.A0, &DataPointer->Adc.A1, &DataPointer->Adc.A2, &DataPointer->Adc.A3
};

static const ushort * RegBlock[] =              // Register
{
    (ushort *)(&DataPointer->Frequency), (ushort *)(&(DataPointer->Frequency)) + 1,
    (ushort *)(&(DataPointer->Voltage)), (ushort *)(&(DataPointer->Current))
};
//************************************************************************************

static byte TxdBuffer[TxdBufferSum];    // 发送帧Buffer
static byte RxdBuffer[RxdBufferSum];    // 接收帧Buffer

static bool RxdState;                   // 接收状态
static ushort RxdCounter;               // 接收计数
static ushort RxdTimeout;               // 接收超时成帧
#define GetBool(nodePointer) *(bool *)((uint)AppDataPointer + (uint)nodePointer)
#define GetUshort(nodePointer) *(ushort *)((uint)AppDataPointer + (uint)nodePointer)
#define UsartWrite(pointer, sum) System.Device.Usart2.Write(pointer, sum)
/*************************************************************************************
* MODBUS主机响应函数
* 传入：数据开始地址，长度，（1）是（0）否帧的开始？
* 被用户调用，要确保USART的发送完成中断是使能的！
*************************************************************************************/
static void Response(byte *bufferPointer, int sum)
{
    ushort crc;
    byte * pointer;

    crc = Crc16(bufferPointer, sum);

    pointer = bufferPointer + sum;
    
    *pointer++ = Byte1(crc);
    *pointer = Byte0(crc);
    sum = sum + 2;
    UsartWrite(bufferPointer, sum);
}

/*************************************************************************************
* 数字输出端口读取
*************************************************************************************/
static void ReadDo(ushort address)
{
    int i;
    byte byteSum, packetSum;
    ushort bitSum;
    byte * pointer;

    TxdBuffer[0] = RxdBuffer[0];      //帧地址
    TxdBuffer[1] = RxdBuffer[1];      //功能码
    
    Byte1(bitSum) = RxdBuffer[4];  
    Byte0(bitSum) = RxdBuffer[5];  //输出端口数

    
    if (bitSum & 0x07)                  //检测端口数是否是8的倍数
        byteSum = (bitSum >> 3) + 1;    //确定字节数
    else
        byteSum = bitSum >> 3;
    
    TxdBuffer[2] = byteSum;            //字节数

    pointer = &TxdBuffer[3];           //先清空发送内容
    for (i = 0; i < byteSum; i++)       
        pointer[i] = false;
    
    for (i = 0; i < bitSum; i++)        //写入端口信息
    {
        if (GetBool(DoBlock[address + i]) == true) //采用数据指针数组
            pointer[i >> 3] |= 1 << (i & 7);
    }

    packetSum = TxdBuffer[2] + 3;

    Response(TxdBuffer, packetSum);
}

/*************************************************************************************
* 数字输入端口读取
*************************************************************************************/
static void ReadDi(ushort address)
{
    int i, packetSum;
    ushort bitSum, byteSum;
    byte * pointer;

    TxdBuffer[0] = RxdBuffer[0];      //帧地址
    TxdBuffer[1] = RxdBuffer[1];      //功能码
    
    Byte1(bitSum) = RxdBuffer[4];
    Byte0(bitSum) = RxdBuffer[5];

    
    if (bitSum & 0x07)
        byteSum = (bitSum >> 3) + 1;
    else
        byteSum = bitSum >> 3;
    
    TxdBuffer[2] = byteSum;            //字节数

    pointer = &TxdBuffer[3];           //发送内容清零
    for (i = 0; i < byteSum; i++)       
        pointer[i] = false;

    for (i = 0; i < bitSum; i++)        //写入bit信息,msOS中存储bit信息是用Byte数组来存储
    {
        if (GetBool(DiBlock[address + i]) == true)
            pointer[i >> 3] |= 1 << (i & 7);
    }

    packetSum = TxdBuffer[2] + 3;

    Response(TxdBuffer, packetSum);
}

/*************************************************************************************
* Adc输入处理
*************************************************************************************/
static void ReadAdc(ushort address)
{
    int i, packetSum;
    ushort registerSum, data;
    byte * pointer;
    
    TxdBuffer[0] = RxdBuffer[0];      //帧地址
    TxdBuffer[1] = RxdBuffer[1];      //功能码
    
    Byte1(registerSum) = RxdBuffer[4];
    Byte0(registerSum) = RxdBuffer[5];
    
    TxdBuffer[2] = registerSum << 1;   //字节数 registerSum <= 0x7D，register是ushort类型

    pointer = &TxdBuffer[3];
    for (i = 0; i < registerSum; i++)
    {
        data = GetUshort(AdcBlock[address++]);
        *pointer++ = Byte1(data);
        *pointer++ = Byte0(data);
    }

    packetSum = TxdBuffer[2] + 3;
    
    Response(TxdBuffer, packetSum);
}

/*************************************************************************************
* 寄存器读操作响应
*************************************************************************************/
static void ReadReg(ushort address)
{
    int i, packetSum;
    ushort registerSum, data;
    byte * pointer;
    
    TxdBuffer[0] = RxdBuffer[0];      //帧地址
    TxdBuffer[1] = RxdBuffer[1];      //功能码
    
    Byte1(registerSum) = RxdBuffer[4];
    Byte0(registerSum) = RxdBuffer[5];
    
    TxdBuffer[2] = registerSum << 1;   //字节数 registerSum <= 0x7D，register是ushort类型

    pointer = &TxdBuffer[3];
    for (i = 0; i < registerSum; i++)
    {
        data = GetUshort(RegBlock[address++]);
        *pointer++ = Byte1(data);
        *pointer++ = Byte0(data);
    }

    packetSum = TxdBuffer[2] + 3;
    
    Response(TxdBuffer, packetSum);
}

/*************************************************************************************
* 线圈单次写操作
*************************************************************************************/
static void WriteOneDo(ushort address)
{
    ushort bitFlag;

    Byte1(bitFlag) = RxdBuffer[4];
    Byte0(bitFlag) = RxdBuffer[5];
        
    if (bitFlag == 0xFF00)                                  //true:0xFF00
        GetBool(DoBlock[address]) = true;
    else                                                    //false:0x0000
        GetBool(DoBlock[address]) = false;
    
    UsartWrite(RxdBuffer, RxdCounter);    //应答帧，跟接收帧相同
}

/*************************************************************************************
* 线圈多次写操作命令响应
* 协议解析不完整，存在只能写8bit数据的缺陷，刚修改了协议解析错误问题。
*************************************************************************************/
static void WriteDo(ushort address)
{
    int i;
    ushort bitSum;
    byte * pointer;
    
    Byte1(bitSum) = RxdBuffer[4];
    Byte0(bitSum) = RxdBuffer[5];

    pointer = &RxdBuffer[7];
    for (i = 0; i < bitSum; i++)
        GetBool(DoBlock[address + i]) = GetBit(pointer[i >> 3], i & 0x07);

    memcpy(TxdBuffer, RxdBuffer, 7);

    Response(TxdBuffer, 6);
}

/*************************************************************************************
* 寄存器单个写操作命令响应
*************************************************************************************/
static void WriteOneReg(ushort address)
{
    ushort data;

    Byte1(data) = RxdBuffer[4];
    Byte0(data) = RxdBuffer[5];

    GetUshort(RegBlock[address]) = data;
    
    UsartWrite(RxdBuffer, RxdCounter);
}

/*************************************************************************************
*
* 多寄存器写操作命令响应
*
*************************************************************************************/
static void WriteReg(ushort address)
{
    int i;
    ushort registerSum, data;
    byte * pointer;
    
    Byte1(registerSum) = RxdBuffer[4];
    Byte0(registerSum) = RxdBuffer[5];

    pointer = &RxdBuffer[7];                   //前面还有一个字节数，在6位置
    for (i = 0; i < registerSum; i++)
    {
        Byte1(data) = *pointer++;
        Byte0(data) = *pointer++;

        GetUshort(RegBlock[address++]) = data;
    }
    
    memcpy(TxdBuffer, RxdBuffer, 6);

    Response(TxdBuffer, 6);
}



/************************************************************************************
* 数据接收复位
*************************************************************************************/
static void Reset()
{
    RxdCounter = 0;
    RxdTimeout = 0;
    RxdState = no;
}

/*************************************************************************************
* 接收缓冲
*************************************************************************************/
static void Receive(byte data)
{
    if(RxdCounter >= RxdBufferSum)
        RxdCounter = 0;
		
    RxdBuffer[RxdCounter++] = data;

    RxdTimeout = 0;//1 
    RxdState = yes;
}

/*************************************************************************************
* MODBUS接收帧处理:命令解析
* 传入：指示已经接收到一帧数据钩子函数指针，响应指示钩子函数指针
* 仅供主循环调用
*************************************************************************************/
static void ParseRxdFrame(void)
{
    ushort address, crcCalculate , crcReceive;

    if (RxdBuffer[0] != DeviceID)
    {
        Reset(); return;
    }
        
    if (RxdCounter <= 4)
    {
        Reset(); return;
    }
        
    crcCalculate = Crc16(RxdBuffer, RxdCounter - 2);
    Byte1(crcReceive) = RxdBuffer[RxdCounter - 2];
    Byte0(crcReceive) = RxdBuffer[RxdCounter - 1];
    
    if (crcCalculate != crcReceive) {Reset(); return;}//校验
    
    Byte1(address) = RxdBuffer[2];  //获取高位地址
    Byte0(address) = RxdBuffer[3];  //获取低位地址
    
    switch(RxdBuffer[1])            //识别支持的功能码
    {
        case ReadDo0x:
            ReadDo(address);
            break;
        case ReadDi1x:
            ReadDi(address);
            break;
        case ReadAdc3x:
            ReadAdc(address);
            break;
        case ReadReg4x:
            ReadReg(address);
            break;
        case WriteOneDo0x:
            WriteOneDo(address);
            break;
        case WriteOneReg4x:
            WriteOneReg(address);
            break;
        case WriteDo0x:
            WriteDo(address);
            break;
        case WriteReg4x:
            WriteReg(address);
            break;
        default:
            break;
    }

    Reset();
}



/*************************************************************************************
* MODBUS接收缓冲区超时控制
* 每0.1MS被调用一次
* 用于确认MODBUS接收完一帧数据
*************************************************************************************/
static void SystickRoutine(void)
{
    if (RxdState == yes)
    {
        RxdTimeout++;
        if (RxdTimeout > RxdTimeoutSum)
        {
            RxdState = no;
            ParseRxdFrame();
        }
    }
}


/************************************************************************************
* MODBUS初始化
*************************************************************************************/
void InitSlaveModbus(void)
{
    Reset();
    
    System.Device.Systick.Register(Systick10000, SystickRoutine);    

    System.Device.Usart2.Register((uint)Receive);
}
 



