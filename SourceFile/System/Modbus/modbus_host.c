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
*文件名     : modbus_host.c
*作用       : 扩展Modbus模块
*原理       : 通过标准Modbus协议扩展子模块
*           : 通过Modbus基于PLC定义的四类地址
*           : 通过四个数组，数组的序号
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2015/12/15       初始版本
********************************************************************************
*/

#include "system.h"

#define RxdTimeoutSum                  2

#define TxdBufferSum                256
#define RxdBufferSum                256

typedef struct SwitchSelf
{
    byte ID;
    bool Data;
    ushort Address;
    void * DataPointer;
    struct SwitchSelf * NextSwitchPointer;
}Switch;

typedef struct WordSelf
{
    byte ID;
    ushort Data;
    ushort Address;
    void * DataPointer;
    struct WordSelf * NextWordPointer;
}Word;

static struct
{
    Switch X0Switch;
    Switch X1Switch;
    Switch X2Switch;
    Switch X3Switch;
}DI;

static struct
{
    Switch Y0Switch;
    Switch Y1Switch;
    Switch Y2Switch;
    Switch Y3Switch;
    Switch Y4Switch;
    Switch Y5Switch;
}DO;

static struct
{
    Word A0Word;
    Word A1Word;
    Word A2Word;
    Word A3Word;
}Adc;

static struct
{
    Word R0Word;
    Word R1Word;
}Reg;

static struct
{
    ushort DI;
    ushort DO;
    ushort Adc;
    ushort Reg;
}Sum;

static struct
{
    ushort Type;
    ushort Number;
}TxdNode, RxdNode;


static void InitHostData(void)
{
    Sum.DI = sizeof(DI) / sizeof(Switch);
    Sum.DO = sizeof(DO) / sizeof(Switch);
    Sum.Adc = sizeof(Adc) / sizeof(Word);
    Sum.Reg = sizeof(Reg) / sizeof(Word);

    TxdNode.Type= 0;
    TxdNode.Number = 0;

    RxdNode.Type= TxdNode.Type;
    RxdNode.Number = TxdNode.Number;
    
    DI.X0Switch.ID = 1;
    DI.X0Switch.Address = 0;
    DI.X0Switch.Data = false;
    DI.X0Switch.DataPointer = &AppDataPointer->SubPlc.DI.X0;
    DI.X0Switch.NextSwitchPointer = &DI.X1Switch;

    DI.X1Switch.ID = 1;
    DI.X1Switch.Address = 1;
    DI.X1Switch.Data = false;
    DI.X1Switch.DataPointer = &AppDataPointer->SubPlc.DI.X1;
    DI.X1Switch.NextSwitchPointer = &DI.X2Switch;

    DI.X2Switch.ID = 1;
    DI.X2Switch.Address = 2;
    DI.X2Switch.Data = false;
    DI.X2Switch.DataPointer = &AppDataPointer->SubPlc.DI.X2;
    DI.X2Switch.NextSwitchPointer = &DI.X3Switch;

    DI.X3Switch.ID = 1;
    DI.X3Switch.Address = 3;
    DI.X3Switch.Data = false;
    DI.X3Switch.DataPointer = &AppDataPointer->SubPlc.DI.X3;
    DI.X3Switch.NextSwitchPointer = null;
    /***********************************************/
    DO.Y0Switch.ID = 1;
    DO.Y0Switch.Address = 0;
    DO.Y0Switch.Data = false;
    DO.Y0Switch.DataPointer = &AppDataPointer->SubPlc.DO.Y0;
    DO.Y0Switch.NextSwitchPointer = &DO.Y1Switch;

    DO.Y1Switch.ID = 1;
    DO.Y1Switch.Address = 1;
    DO.Y1Switch.Data = true;
    DO.Y1Switch.DataPointer = &AppDataPointer->SubPlc.DO.Y1;
    DO.Y1Switch.NextSwitchPointer = &DO.Y2Switch;

    DO.Y2Switch.ID = 1;
    DO.Y2Switch.Address = 2;
    DO.Y2Switch.Data = true;
    DO.Y2Switch.DataPointer = &AppDataPointer->SubPlc.DO.Y2;
    DO.Y2Switch.NextSwitchPointer = &DO.Y3Switch;

    DO.Y3Switch.ID = 1;
    DO.Y3Switch.Address = 3;
    DO.Y3Switch.Data = false;
    DO.Y3Switch.DataPointer = &AppDataPointer->SubPlc.DO.Y3;
    DO.Y3Switch.NextSwitchPointer = &DO.Y4Switch;

    DO.Y4Switch.ID = 1;
    DO.Y4Switch.Address = 4;
    DO.Y4Switch.Data = false;
    DO.Y4Switch.DataPointer = &AppDataPointer->SubPlc.DO.Y4;
    DO.Y4Switch.NextSwitchPointer = &DO.Y5Switch;
    
    DO.Y5Switch.ID = 1;
    DO.Y5Switch.Address = 5;
    DO.Y5Switch.Data = false;
    DO.Y5Switch.DataPointer = &AppDataPointer->SubPlc.DO.Y5;
    DO.Y5Switch.NextSwitchPointer = null;
    /***********************************************/
    Adc.A0Word.ID = 1;
    Adc.A0Word.Address = 0;
    Adc.A0Word.Data = 0;
    Adc.A0Word.DataPointer = &AppDataPointer->SubPlc.Adc.A0;
    Adc.A0Word.NextWordPointer = &Adc.A1Word;

    Adc.A1Word.ID = 1;
    Adc.A1Word.Address = 1;
    Adc.A1Word.Data = 0;
    Adc.A1Word.DataPointer = &AppDataPointer->SubPlc.Adc.A1;
    Adc.A1Word.NextWordPointer = &Adc.A2Word;

    Adc.A2Word.ID = 1;
    Adc.A2Word.Address = 2;
    Adc.A2Word.Data = 0;
    Adc.A2Word.DataPointer = &AppDataPointer->SubPlc.Adc.A2;
    Adc.A2Word.NextWordPointer = &Adc.A3Word;

    Adc.A3Word.ID = 1;
    Adc.A3Word.Address = 3;
    Adc.A3Word.Data = 0;
    Adc.A3Word.DataPointer = &AppDataPointer->SubPlc.Adc.A3;
    Adc.A3Word.NextWordPointer = null;
    /***********************************************/
    Reg.R0Word.ID = 1;
    Reg.R0Word.Address = 0;
    Reg.R0Word.Data = 1;
    Reg.R0Word.DataPointer = (ushort *)&AppDataPointer->SubPlc.Reg;
    Reg.R0Word.NextWordPointer = &Reg.R1Word;

    Reg.R1Word.ID = 1;
    Reg.R1Word.Address = 1;
    Reg.R1Word.Data = 1;
    Reg.R1Word.DataPointer = (ushort *)&AppDataPointer->SubPlc.Reg + 1;
    Reg.R1Word.NextWordPointer = null; 
}


//************************************************************************************

static byte TxdBuffer[TxdBufferSum];    // 发送帧环型缓冲区
static byte RxdBuffer[RxdBufferSum];    // 接收帧数据缓冲区
static ushort RxdNodeSum;
static bool RxdState;                   // 接收状态
static ushort RxdCounter;               // 接收计数
static ushort RxdTimeout;               // 接收超时成帧
#define UsartWrite(pointer, sum) System.Device.Usart3.Write(pointer, sum)

static void ParseDi(void)
{
    ushort sum = 1,crc;
    Switch * pointer;
    
    pointer = (Switch *)(&DI) + TxdNode.Number;
    
    TxdBuffer[0] = pointer->ID;
    TxdBuffer[1] = ReadDi1x;
    TxdBuffer[2] = Byte1(pointer->Address);
    TxdBuffer[3] = Byte0(pointer->Address);
    
    while (pointer->NextSwitchPointer != null) 
    {
        pointer = pointer->NextSwitchPointer;
        sum++;
    }

    TxdBuffer[4] = Byte1(sum);
    TxdBuffer[5] = Byte0(sum);
    
    crc = Crc16(TxdBuffer, 6);
    
    TxdBuffer[6] = Byte1(crc);
    TxdBuffer[7] = Byte0(crc);

    UsartWrite(TxdBuffer, 8);

    RxdNode.Type = TxdNode.Type;
    RxdNode.Number = TxdNode.Number;
    
    TxdNode.Number += sum;
    if (TxdNode.Number >= Sum.DI)
    {
        TxdNode.Type = 1;
        TxdNode.Number = 0;
    }
}

static void ParseDo(void)
{
    byte code = no, byteSum = 0, data;
    ushort bitSum = 0, crc;
    Switch * pointer;

    pointer = (Switch *)(&DO) + TxdNode.Number;
    
    TxdBuffer[0] = pointer->ID;
    //TxdBuffer[1] = code;
    TxdBuffer[2] = Byte1(pointer->Address);
    TxdBuffer[3] = Byte0(pointer->Address);
    
    while (pointer != null) 
    {
        data = pByte(pointer->DataPointer);
        if (pointer->Data != data)               // 写入
        {
            if (code == ReadDo0x ) break;
            
            if (code == no)                                             // 单次写入
            {
                code = WriteOneDo0x;
                if (data)
                    {TxdBuffer[4] = 0xFF; TxdBuffer[6] = 0x01;}                     
                else
                    {TxdBuffer[4] = 0x00; TxdBuffer[6] = 0x00;}
                                                    
                TxdBuffer[5] = 0x00;
            }
            else                                                        // 多次写入
            {
                code = WriteDo0x;
                if (data)
                    SetBit(TxdBuffer[6 + (bitSum >> 3)], bitSum & 0x07);
                else
                    ResetBit(TxdBuffer[6 + (bitSum >> 3)], bitSum & 0x07);
            } 
        }
        else                                                            // 读取
        {
            if (code == 0 || code == ReadDo0x)
                code = ReadDo0x;
            else
                break;
        }
        pointer = pointer->NextSwitchPointer;
        bitSum++;
    }

    TxdBuffer[1] = code;
    if (code == ReadDo0x)
    {
        TxdBuffer[4] = Byte1(bitSum);
        TxdBuffer[5] = Byte0(bitSum);
        crc = Crc16(TxdBuffer, 6);
        TxdBuffer[6] = Byte1(crc);
        TxdBuffer[7] = Byte0(crc);
        UsartWrite(TxdBuffer, 8);
    }
    else if (code == WriteOneDo0x)
    {
        crc = Crc16(TxdBuffer, 6);
        TxdBuffer[6] = Byte1(crc);
        TxdBuffer[7] = Byte0(crc);
        UsartWrite(TxdBuffer, 8);
    }
    else if (code == WriteDo0x)
    {
        TxdBuffer[4] = Byte1(bitSum);
        TxdBuffer[5] = Byte0(bitSum);
        
        if (bitSum & 7)
            byteSum = (bitSum >> 3) + 1;
        else
            byteSum = (bitSum >> 3);

        crc = Crc16(TxdBuffer, 6 + byteSum);
        TxdBuffer[6 + byteSum] = Byte1(crc);
        TxdBuffer[7 + byteSum] = Byte0(crc);
        UsartWrite(TxdBuffer, 8 + byteSum);
    }

    RxdNode.Type = TxdNode.Type;
    RxdNode.Number = TxdNode.Number;
    RxdNodeSum = bitSum;
    
    TxdNode.Number += bitSum;
    if (TxdNode.Number >= Sum.DO)
    {
        TxdNode.Type = 2;
        TxdNode.Number = 0;
    }
}

static void ParseAdc(void)
{
    byte sum = 0;
    ushort crc;
    Word * pointer;

    pointer = (Word *)(&Adc) + TxdNode.Number;
    
    TxdBuffer[0] = pointer->ID;
    TxdBuffer[1] = ReadAdc3x;
    TxdBuffer[2] = Byte1(pointer->Address);
    TxdBuffer[3] = Byte0(pointer->Address);

    while (pointer != null) 
    {
        pointer = pointer->NextWordPointer;
        sum++;
    }
     
    TxdBuffer[4] = 0;
    TxdBuffer[5] = sum;
    
    crc = Crc16(TxdBuffer, 6);
    
    TxdBuffer[6] = Byte1(crc);
    TxdBuffer[7] = Byte0(crc);

    UsartWrite(TxdBuffer, 8);

    RxdNode.Type = TxdNode.Type;
    RxdNode.Number = TxdNode.Number;
    
    TxdNode.Number += sum;
    if (TxdNode.Number >= Sum.Adc)
    {
        TxdNode.Type = 3;
        TxdNode.Number = 0;
    }
}

static void ParseReg(void)
{
    byte code = no, sum = 0, byteSum = 0;
    ushort crc, data;
    Word * pointer;

    pointer = (Word *)(&Reg) + TxdNode.Number;

    TxdBuffer[0] = pointer->ID;
    //TxdBuffer[1] = ReadReg4x;
    TxdBuffer[2] = Byte1(pointer->Address);
    TxdBuffer[3] = Byte0(pointer->Address);

    while (pointer != null) 
    {
        data = pUshort(pointer->DataPointer);
        if (pointer->Data != data)               // 写入
        {
            if (code == ReadReg4x ) break;
            
            if (code == no)                                             // 单次写入
            {
                code = WriteOneReg4x;
                TxdBuffer[4] = Byte1(data);
                TxdBuffer[5] = Byte0(data);
                TxdBuffer[7] = Byte1(data);
                TxdBuffer[8] = Byte0(data);
            }
            else                                                        // 多次写入
            {
                code = WriteReg4x;
                TxdBuffer[7 + byteSum] = Byte1(data);
                TxdBuffer[8 + byteSum] = Byte0(data);
            } 
        }
        else                                                            // 读取
        {
            if (code == 0 || code == ReadReg4x)
                code = ReadReg4x;
            else
                break;
        }
        pointer = pointer->NextWordPointer;
        sum++; byteSum = sum << 1;
    }

    TxdBuffer[1] = code;
    if (code == ReadReg4x)
    {
        TxdBuffer[4] = 0;
        TxdBuffer[5] = sum;
        crc = Crc16(TxdBuffer, 6);
        TxdBuffer[6] = Byte1(crc);
        TxdBuffer[7] = Byte0(crc);
        UsartWrite(TxdBuffer, 8);
    }
    else if (code == WriteOneReg4x)
    {
        crc = Crc16(TxdBuffer, 6);
        TxdBuffer[6] = Byte1(crc);
        TxdBuffer[7] = Byte0(crc);
        UsartWrite(TxdBuffer, 8);
    }

    else if (code == WriteReg4x)
    {
        TxdBuffer[4] = 0;
        TxdBuffer[5] = sum;
        TxdBuffer[6] = byteSum;
        crc = Crc16(TxdBuffer, 7 + byteSum);
        TxdBuffer[7 + byteSum] = Byte1(crc);
        TxdBuffer[8 + byteSum] = Byte0(crc);
        UsartWrite(TxdBuffer, 9 + byteSum);
    }

    RxdNode.Type = TxdNode.Type;
    RxdNode.Number = TxdNode.Number;
    RxdNodeSum = sum;

    TxdNode.Number += sum;
    if (TxdNode.Number >= Sum.Reg)
    {
        TxdNode.Type = 0;
        TxdNode.Number = 0;
    }
}

static void ParseObject(void)
{
    int i;
    for (i = 0; i < 4; i++)                     // 寻找当前处理节点
    {
        if (((ushort *)&Sum)[TxdNode.Type] == 0) 
        {
            
            TxdNode.Type++;
            if (TxdNode.Type == 4) TxdNode.Type = 0;
        }
    }
    switch(TxdNode.Type)
    {
        case 0:
            if (Sum.DI) ParseDi();
            break;
        case 1:
            if (Sum.DO) ParseDo();
            break;
        case 2:
            if (Sum.Adc) ParseAdc();
            break;
        case 3:
            if (Sum.Reg) ParseReg();
            break;
    }
}

static void ReadDi(void)
{
    int i = 3, j = 0;
    bool data;
    Switch * pointer;
    if (RxdNode.Type != 0) return;
    
    pointer = (Switch *)&DI + RxdNode.Number;

    while (pointer != null)
    {
        data = GetBit(RxdBuffer[i], j);
        if (++j == 8) {j = 0; i++;}
        pointer->Data = data;
        *(bool *)(pointer->DataPointer) = data;
        pointer = pointer->NextSwitchPointer;
    }
}

static void ReadDo(void)
{
    int i = 3, j = 0;
    bool data;
    Switch * pointer;
    if (RxdNode.Type != 1) return;
    
    pointer = (Switch *)&DO + RxdNode.Number;

    while(RxdNodeSum--)
    {
        if (pointer->Data == pByte(pointer->DataPointer))
        {
            data = GetBit(RxdBuffer[i], j);
            pointer->Data = data;
            pByte(pointer->DataPointer) = data;
        }
        if (++j == 8) {j = 0; i++;}
        pointer = pointer->NextSwitchPointer;
    }
}

static void ReadAdc(void)
{
    int i = 3;
    ushort data;
    Word * pointer;
    if (RxdNode.Type != 2) return;
    
    pointer = (Word *)&Adc + RxdNode.Number;

    while (pointer != null)
    {
        Byte1(data) = RxdBuffer[i++];
        Byte0(data) = RxdBuffer[i++];
        pointer->Data = data;
        *(ushort *)(pointer->DataPointer) = data;
        pointer = pointer->NextWordPointer;
    }
}

static void ReadReg(void)
{
    int i = 3;
    ushort data;
    Word * pointer;
    if (RxdNode.Type != 3) return;
    
    pointer = (Word *)&Reg + RxdNode.Number;

    while(RxdNodeSum--)
    {
        if (pointer->Data == pByte(pointer->DataPointer))
        {
            Byte1(data) = RxdBuffer[i];
            Byte0(data) = RxdBuffer[i + 1];
            pointer->Data = data;
            *(ushort *)(pointer->DataPointer) = data;
        }
        i += 2;
        pointer = pointer->NextWordPointer;
    }
}

static void WriteOneDo(void)
{
    Switch * pointer;
    if (RxdNode.Type != 1) return;
    
    pointer = (Switch *)&DO + RxdNode.Number;
    pointer->Data = pByte(pointer->DataPointer);
}

static void WriteDo(void)
{
    Switch * pointer;
    if (RxdNode.Type != 1) return;
    
    pointer = (Switch *)&DO + RxdNode.Number;
    while(RxdNodeSum--)
    {
        pointer->Data = pByte(pointer->DataPointer);
        pointer = pointer->NextSwitchPointer;
    }
}

static void WriteOneReg(void)
{
    Word  * pointer;
    if (RxdNode.Type != 3) return;
    
    pointer = (Word  *)&Reg + RxdNode.Number;
    pointer->Data = pUshort(pointer->DataPointer);
}

static void WriteReg(void)
{
    Word * pointer;
    if (RxdNode.Type != 3) return;
    
    pointer = (Word  *)&Reg + RxdNode.Number;
    while(RxdNodeSum--)
    {
        pointer->Data = pUshort(pointer->DataPointer);
        pointer = pointer->NextWordPointer;
    }
}

static void Reset()
{
    RxdCounter = 0;
    RxdTimeout = 0;
    RxdState = no;
}

static void Receive(byte data)
{
    if(RxdCounter >= RxdBufferSum)
        RxdCounter = 0;
		
    RxdBuffer[RxdCounter++] = data;

    RxdTimeout = 0;//1 
    RxdState = yes;
}

static void ParseRxdFrame(void)
{
    ushort crcCalculate , crcReceive;
    if (RxdCounter <= 4)
    {
        Reset(); return;
    }

    if (TxdBuffer[0] != RxdBuffer[0] || TxdBuffer[1] != RxdBuffer[1]) {Reset(); return;} // ID、FunctionID校验
    
    crcCalculate = Crc16(RxdBuffer, RxdCounter - 2);
    Byte1(crcReceive) = RxdBuffer[RxdCounter - 2];
    Byte0(crcReceive) = RxdBuffer[RxdCounter - 1];
    
    if (crcCalculate != crcReceive) {Reset(); return;}               // CRC校验
    
    
    switch(RxdBuffer[1])
    {
        case ReadDi1x:
            ReadDi();
            break;
            
        case ReadDo0x:
            ReadDo();
            break;
            
        case ReadAdc3x:
            ReadAdc();
            break;
            
        case ReadReg4x:
            ReadReg();
            break;
        
        case WriteOneDo0x:
            WriteOneDo();
            break;
            
        case WriteDo0x:
            WriteDo();
            break;
            
        case WriteOneReg4x:
            WriteOneReg();
            break;

        case WriteReg4x:
            WriteReg();
            break;

        case ReadWriteReg4x:
            break;
        case MaskReg:
            break;
        case ReadDeviceID:
            break;  
        default:
            break;
    }

    Reset();
}


static void SystickRoutine(void)
{
    static ushort Counter = 0;
    if(++Counter == 100)
    {
        Counter = 0;
        
        ParseObject();
    }

    if (RxdState == yes)
    {
        RxdTimeout++;
        if (RxdTimeout > RxdTimeoutSum)
        {
            RxdState = no;
            System.Device.Usart1.Write(RxdBuffer, RxdCounter);
            ParseRxdFrame();
            Reset();
        }
    }
}
/************************************************************************************
* MODBUS初始化
*************************************************************************************/
void InitHostModbus(void)
{
    Reset();

    InitHostData();
    
    System.Device.Systick.Register(Systick10000, SystickRoutine);    

    System.Device.Usart3.Register((uint)Receive);
}










