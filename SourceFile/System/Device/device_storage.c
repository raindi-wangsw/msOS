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
*文件名     : device_storage.c
*作用       : 存储设备(参数存储和日志存储)
*原理       : STM32F103R8系列flash为64*1KByte,最少擦除单元为1KByte
*           : STM32F103RB系列flash为128*1KByte,最少擦除单元为1KByte
*           : storage参数存储采用追加式存储方式，存储形式为[Address][Data]，
*           : 即把数据的地址与数据作为一个整体存储进去，当数据有变化的时候，
*           : 在后面追加存储即可。重启时检测清理无效的数据。
*           : 日志存储直接存储写入数据即可，连续追加方式，注意，参数存储和
*           : 日志存储空间位置不同。
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/08/23       初始版本      
********************************************************************************
*/

#include "drive.h"
#include "system.h"

#define PageSize                1024 //Stm32F103R8T6 1Page=1024Byte

#define ParameterAddress        (FLASH_BASE + (63 * 1024)) 
#define ParameterSpace          PageSize / 4 / 2

#define LogAddress              (FLASH_BASE + (60 * 1024)) 
#define LogSpace                PageSize * 3

typedef struct
{
    uint Address;               //参数存储单元地址
    uint Data;                  //参数存储单元数据
}CellStruct;

typedef struct
{
    ushort Entries;             //存入参数个数
    ushort Size;                //已用参数空间
    ushort Space;               //总参数空间
    ushort WritePonit;          //写入点
}ParameterStruct;

typedef struct
{
    char * ReadPointer;         //读取点
    char * WritePointer;        //写入点
    ushort Size;                //已用日志空间
    ushort Space;               //总日志空间
}LogStruct;

static CellStruct * CellBlock = (CellStruct *)ParameterAddress;

static ParameterStruct Parameter; 

static LogStruct Log;

/*******************************************************************************
* 描述	    : Flash锁打开，允许写入，擦除数据
*******************************************************************************/
static void Open(void)
{
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
}

/*******************************************************************************
* 描述	    : Flash锁关闭，不允许写入，擦除数据
*******************************************************************************/
static void Close(void)
{
    FLASH_Lock();
}

/*******************************************************************************
* 描述	    : 开机读取参数存储信息到数据库中，检测数据库，清理垃圾数据，节省存储空间
*******************************************************************************/
static void ReadAllParameter(void)
{
    bool CleanFlag;
    int i, j;
    CellStruct cell;
    uint AddressBlock[ParameterSpace];
    
    for (i = 0; i < ParameterSpace; i++)
    {
        if (CellBlock[i].Address != 0xFFFFFFFF)
        {
            AddressBlock[i] = CellBlock[i].Address;             // 备份页面到临时数组中
            pUint(AddressBlock[i]) = CellBlock[i].Data;   // 初始化数据库数据
        }
        else
            break;
    }
    
    Parameter.Size = i;
    Parameter.Space = ParameterSpace;                           
    if (Parameter.Size < 2)                                     // 检测是否有无效单元，是否需要清理
        Parameter.Entries = Parameter.Size;
    else
    {
        CleanFlag = false;
    
        for (i = 0; i < Parameter.Size - 1; i++)
        {
            for (j = i + 1; j < Parameter.Size; j++)            // 把无效单元地址清空，单元失效
            {
                if (AddressBlock[i] == AddressBlock[j])
                {
                    AddressBlock[i] = 0;            
                    CleanFlag = true;
                    break;
                }   
            }     
        }
        
        if (CleanFlag == false)                                 // 清理无效单元
            Parameter.Entries = Parameter.Size;
        else
        {
            Open();
            FLASH_ErasePage(ParameterAddress);                  // 擦除页面

            j = 0;
            for (i = 0; i < Parameter.Size; i++)                // 烧写单元
            {
                if (AddressBlock[i] == 0) continue;
                
                cell.Address = AddressBlock[i];
                cell.Data = pUint(AddressBlock[i]);
                
                FLASH_ProgramWord((uint)(&CellBlock[j].Address), cell.Address);
                FLASH_ProgramWord((uint)(&CellBlock[j].Data), cell.Data);
                j++;
            }
            Close();
            Parameter.Entries = j;
        }
    }

    Parameter.WritePonit = Parameter.Entries;
    
    printf("\n\n\n");
    printf("\n参数总存储空间 = %d", Parameter.Space);
    printf("\n参数已存储空间  = %d", Parameter.Size);
    printf("\n参数存储个数 = %d", Parameter.Entries);
    printf("\n参数写入点 = %d", Parameter.WritePonit);
}


/*******************************************************************************
* 描述	    : 存储数据，基于数据的指针来存储数据。
* 输入参数  : 数据的指针
* 返回参数  : bool变量
*******************************************************************************/
static bool WriteParameter(void * dataPointer)
{
    uint * pointer;
    pointer = (uint *)dataPointer;
    
    if (dataPointer == (uint *)0) return (false);

    if (Parameter.WritePonit == Parameter.Space) return(false);

    Open();
    FLASH_ProgramWord((uint)(&CellBlock[Parameter.WritePonit].Address), (uint)pointer);
    FLASH_ProgramWord((uint)(&CellBlock[Parameter.WritePonit].Data), *pointer);
    Close();
    Parameter.WritePonit++;
    return (true);
}

/*******************************************************************************
* 描述	    : 读取日志存储信息
*******************************************************************************/
static void ReadLogProfile(void)
{
    int i;
    char * readPointer;
    Log.Space = LogSpace;
    Log.ReadPointer = (char *)LogAddress;
    Log.WritePointer = Log.ReadPointer;
    Log.Size = 0;
    readPointer = (char *)LogAddress;
    
    for (i = 0; i < LogSpace; i++)
    {
        if (*readPointer == 0xFF) 
        {
            Log.WritePointer = readPointer;
            Log.Size = i;        
            break;
        }
        readPointer++;
    }
    printf("\n\n");
    printf("\n日志总存储空间 = %d", Log.Space);
    printf("\n日志已存储空间  = %d", Log.Size);
    printf("\n日志读取点 = %x", (uint)(Log.ReadPointer));
    printf("\n日志写入点 = %x", (uint)(Log.WritePointer));
    printf("\n\n");
}


/*******************************************************************************
* 描述	    : 清除日志信息
*******************************************************************************/
static void EraseLog(void)
{
    int i;
    
    Open();
    
    for (i = 0; i < (LogSpace / PageSize); i++)
        FLASH_ErasePage(LogAddress + (i * PageSize));

    Close();
    Log.ReadPointer = (char *)LogAddress;
    Log.WritePointer = (char *)LogAddress;
    Log.Size = 0;
}
    

/*******************************************************************************
* 描述	    : 读取日志存储区，按记录读取
* 输入参数  : sequence: -1: 按字符串一条一条读取，循环读
*           :            0: 初始化读取地址为存储区开头，返回开头地址，读取第一条
*           :           >0: 读取第N条记录
* 返回参数  : 字符串指针
*******************************************************************************/
static char * ReadLog(int sequence)
{
    char * pointer;
    char * returnPointer;
    
    pointer = (char *)LogAddress;
    if (*pointer == 0xFF) return ("Null\n");        // 检测是否为读取终点
    
    if (sequence == 0)                              // 读取第0条记录
    {
        Log.ReadPointer = (char *)LogAddress;
        return((char *)LogAddress);                 // 返回字符串首地址
    }
    else if (sequence == -1)                        // 从序号0开始循环读取
    {
        returnPointer = Log.ReadPointer;
        pointer = Log.ReadPointer;

        while(*pointer++ > 0x00); 
            
        if (*pointer == 0x00) pointer++;            // 半字存储，存在两个结束符0
        
        if ((*pointer == 0xFF) || ((uint)pointer > (LogAddress + LogSpace)))
            Log.ReadPointer = (char *)LogAddress;
        else
            Log.ReadPointer = pointer;
        
        return (returnPointer);
    }
    else if (sequence > 0)                          // 寻找序号位置记录
    {
        while(sequence--)
        {
            if(*pointer == 0xFF) return ("Null\n");
            
            while(*pointer > 0x00); pointer++;
            while(*pointer == 0x00); pointer++;
        }
        if ((*pointer == 0xFF) || ((uint)pointer > (LogAddress + LogSpace)))
            return ("Null\n");
        else
            return (pointer);
    }
    return ("ArgumentError\n");
}

/*******************************************************************************
* 描述	    : 写入日志信息，以字符串方式写入。日志函数用到了较大的栈Ram，需要在业务逻辑中执行。
* 输入参数  : 字符串
* 返回参数  : bool变量
*******************************************************************************/
static bool WriteLog(char *fmt, ...)
{
    int i, j, len;
    ushort * pointer;
    va_list list;
    char buffer[100];
    
    pointer = (ushort *)buffer;
    for (i = 0; i < 50; i++) *pointer++ = 0x0000;   // 初始化临时数组，值为0
    
    va_start(list, fmt);                            // 用vsprintf字符串转换函数把字符串打印到数组中
    len = vsprintf(buffer, fmt, list);
    va_end(list);

    if (len == 0) return (false);                   // 判断长度不为空
    
    len++;                                          // 加上字符串结束符长度
    if (len & 0x01) len++;                          // 因半字存储，要按偶数数据长度存储
    
    if (Log.Size + len >= LogSpace) return (false); // 判断空间是否为满
    j = len >> 1;                                   
    pointer = (ushort *)buffer;
    Open();
    for (i = 0; i < j; i++)                         // 半字烧写
    {
        FLASH_ProgramHalfWord((uint)(Log.WritePointer), *pointer);
        Log.WritePointer += 2;
        pointer++;
    }
    Close();
    
    Log.Size += len; 
    return (true);
}

void InitStorage(void)
{
    ReadAllParameter();
    System.Device.Storage.Parameter.Write = WriteParameter;

    ReadLogProfile();
    System.Device.Storage.Log.Erase = EraseLog;
    System.Device.Storage.Log.Read = ReadLog;
    System.Device.Storage.Log.Write = WriteLog;
}


