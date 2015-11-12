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
*文件名   : os.c
*创建时间 : 2014.9.9
********************************************************************************
*/





#include "system.h"
#include "os.h"


typedef struct                              // 定义队列类型
{
    uint    *   Start;                      // 指向队列开始
    uint    *   End;                        // 指向队列结束
    uint    *   In;                         // 插入一个消息
    uint    *   Out;                        // 取出一个消息
    ushort      Entries;                    // 消息长度
} QueueStruct;

typedef struct 
{
    uint *          StackPointer;           // 栈顶指针
    uint            Message;                // 消息
    uint            Delay;                  // 任务延时节拍数
} TaskStruct;

#define LogicTaskStackSum   400             // 业务逻辑任务栈深度
#define MenuTaskStackSum    400             // 菜单界面任务栈深度
#define QueueBufferSum      40              // 消息队列深度

#define PendSVRegister *((volatile  uint *)0xE000ED04)
#define PendSVSet 0x10000000

#define SwitchContext() PendSVRegister = PendSVSet

static QueueStruct MessageQueue;
static uint QueueBuffer[QueueBufferSum];                    // 业务逻辑消息队列
__align(8) static uint LogicTaskStack[LogicTaskStackSum];   //业务逻辑任务栈数组
__align(8) static uint MenuTaskStack[MenuTaskStackSum];     //菜单界面任务栈数组

static TaskStruct LogicTask;
static TaskStruct MenuTask;

TaskStruct * CurrentTaskPointer;
TaskStruct * NextTaskPointer;

/*******************************************************************************
* 描述	    : 初始化任务栈，xPSR、PC、R14、R12、R3、R2、R1、R0入栈顺序是由Cortex
*           : 芯片决定的。中断入栈是芯片硬件自动完成，R11~R4顺序是由OS自己决定的。
* 输入参数  : taskPointer: 任务入口地址, stackRamTopPointer: 栈内存最高地址
* 返回参数  : 栈顶
*******************************************************************************/
static uint * InitStack(void (*taskPointer)(void), uint * stackPointer)
{
    *(stackPointer)   = (uint)0x01000000L;       // xPSR
    *(--stackPointer) = (uint)taskPointer;       // 任务入口地址
    *(--stackPointer) = (uint)0xFFFFFFFEL;       // R14 (LR)
                                                                           
    *(--stackPointer) = (uint)0x12121212L;       // R12
    *(--stackPointer) = (uint)0x03030303L;       // R3
    *(--stackPointer) = (uint)0x02020202L;       // R2
    *(--stackPointer) = (uint)0x01010101L;       // R1
    *(--stackPointer) = (uint)0x00000000L;       // R0 

    *(--stackPointer) = (uint)0x11111111L;       // R11
    *(--stackPointer) = (uint)0x10101010L;       // R10
    *(--stackPointer) = (uint)0x09090909L;       // R9
    *(--stackPointer) = (uint)0x08080808L;       // R8
    *(--stackPointer) = (uint)0x07070707L;       // R7
    *(--stackPointer) = (uint)0x06060606L;       // R6
    *(--stackPointer) = (uint)0x05050505L;       // R5
    *(--stackPointer) = (uint)0x04040404L;       // R4

    return(stackPointer);
}


/*******************************************************************************
* 描述	    : 初始化任务表
* 输入参数  : task: 任务表, sp:栈顶
*******************************************************************************/
static void InitTask(TaskStruct * task, uint * sp)
{
    task->StackPointer = sp;
    task->Message = 0;
    task->Delay = 0;
}


/*******************************************************************************
* 描述	    : 创建业务逻辑任务
* 输入参数  : 业务逻辑任务入口地址
*******************************************************************************/
static void CreateLogicTask(function taskPointer)
{
    uint * sp;	
    sp = InitStack(taskPointer, &LogicTaskStack[LogicTaskStackSum - 1]);
    InitTask(&LogicTask, sp);
}

/*******************************************************************************
* 描述	    : 创建菜单界面任务
* 输入参数  : 菜单界面任务入口地址
*******************************************************************************/
static void CreateMenuTask(function taskPointer)
{
    uint * sp;	
    sp = InitStack(taskPointer, &MenuTaskStack[MenuTaskStackSum - 1]);
    InitTask(&MenuTask, sp);
}

/*******************************************************************************
* 描述	    : 任务切换
* 输入参数  : 切换到指定任务,激活PendSV中断实现切换
*******************************************************************************/
static void Schedule(TaskStruct *taskPointer)
{
    if (taskPointer != CurrentTaskPointer)
    {
        NextTaskPointer = taskPointer;
        SwitchContext();    //激活PendSV中断，在中断中实现任务的真正切换
    }
}

/*******************************************************************************
* 描述	    : 节拍例行，主要用于任务挂起延时作用
*******************************************************************************/
static void SystemTickRoutine(void)
{       
    if (LogicTask.Delay > 0) 
    {
        LogicTask.Delay--;
        if (LogicTask.Delay == 0)            
            Schedule(&LogicTask);
    }
}

/*******************************************************************************
* 描述	    : OS启动
*******************************************************************************/
static void Start(void)
{
    System.Device.Systick.Register(Systick1000, SystemTickRoutine); //注册OS节拍函数

    CurrentTaskPointer = &LogicTask;
    NextTaskPointer = CurrentTaskPointer;
    
    SwitchContext();
    EnableIrq();
    while(1);
}

/*******************************************************************************
* 描述	    : 发送一个消息到消息队列中，处于等待的任务会自动运行
*******************************************************************************/
static bool PostMessageQueue(uint message)
{	
    EnterCritical();
    // 菜单界面任务下，且无消息，用任务成员Message传递，不用消息队列
    if (CurrentTaskPointer == &MenuTask && LogicTask.Message == 0)
    {
        LogicTask.Delay = 0;
        LogicTask.Message = message;    	
        ExitCritical();
        Schedule(&LogicTask);
        return (true);
    }

    if (MessageQueue.Entries >= QueueBufferSum)
    {                                                              
        ExitCritical();
        return (false);
    }

    MessageQueue.Entries++;
    
    *MessageQueue.In++ = message;
    if (MessageQueue.In > MessageQueue.End)
       	MessageQueue.In = MessageQueue.Start;

    ExitCritical();

    return (true);
}

/*******************************************************************************
* 描述	    : 等待消息队列，当消息队列为空时，所在任务挂起
*******************************************************************************/
static uint PendMessageQueue(void)
{
    uint message;
    EnterCritical();

    if (LogicTask.Message != 0) goto ReadMessage;
		
    if (MessageQueue.Entries > 0)
    {                    
       	message = *MessageQueue.Out++;
       	MessageQueue.Entries--;
       	if (MessageQueue.Out > MessageQueue.End) 
            MessageQueue.Out = MessageQueue.Start;
        
       	ExitCritical();
       	return (message);
    }
    LogicTask.Delay = 0;
    ExitCritical();
    
    Schedule(&MenuTask); 
    
    EnterCritical();
ReadMessage:
    message = LogicTask.Message; 	
    LogicTask.Message = 0; 	
    ExitCritical();
    return (message);
}

/*******************************************************************************
* 描述	    : 任务挂起等待时间，单位为1个OS节拍，默认是1mS.
* 输入参数  : times，挂起节拍数，默认一个节拍为1mS，比如1000,就是1秒钟
*******************************************************************************/
static void DelayMs(int times)
{
    if (times > 0) 
    {          
       	LogicTask.Delay = times;
        
       	Schedule(&MenuTask); 
    }
}

/*******************************************************************************
* 描述	    : 任务切换，在PendSV中断中实现
*******************************************************************************/                                                       
__asm void PendSV_Handler(void)
{
  IMPORT  CurrentTaskPointer
  IMPORT  NextTaskPointer

    CPSID   I                           // 关闭中断
    MRS     R0, PSP                     // 读取PSP到R0
    LDR     R2, =CurrentTaskPointer     // 获取指向当前任务指针的地址
    LDR     R3, =NextTaskPointer        // 获取指向新任务指针的地址
    CBZ     R0, NextTask                // 复位后，任务启动时，从中断模式跳转到任务中
    // 入栈                                    
    SUBS    R0, R0, #32                 // 栈顶减32，即8Word
    STM     R0, {R4-R11}                // 压栈R4~R11,8个寄存器
    LDR     R1, [R2]                    // 获取指向当前任务的指针，也指向第一成员StackPointer
    STR     R0, [R1]                    // 把当前栈顶存入表中第一个成员
    // 获取新栈顶
NextTask       
    LDR     R0, [R3]                    // 获取新任务指针变量值
    STR     R0, [R2]                    // 赋值给当前任务指针变量
    // 出栈
    LDR     R0, [R0]                    // 加载新任务栈顶到R0
    LDM     R0, {R4-R11}                // 加载栈内容到R4~R11
    ADDS    R0, R0, #32                 // 新栈顶升32，即8Word
    MSR     PSP, R0                     // 新栈顶写入PSP
    ORR     LR, LR, #0x04               // LR的bit2写入1，中断退出后进入任务模式，使用PSP栈
    CPSIE   I                           // 开中断
    BX      LR                          // 返回
    NOP
}

/*******************************************************************************
* 描述	    : 初始化用户栈为0，用于OS第一次任务切换判断用，具体看切换部分汇编函数
*******************************************************************************/
__asm void InitPsp(uint psp)
{
    MSR     PSP, R0
    BX      LR
}

void InitOs(void)
{    
    MessageQueue.Start        = QueueBuffer;
    MessageQueue.End          = QueueBuffer + QueueBufferSum - 1;
    MessageQueue.In           = MessageQueue.Start;
    MessageQueue.Out          = MessageQueue.Start;
    MessageQueue.Entries      = 0;	
    
    InitPsp(0);     // PSP清零，PSP用于中断处理模式判断
    
    System.OS.CreateLogicTask = CreateLogicTask;
    System.OS.CreateMenuTask = CreateMenuTask;
    System.OS.Start = Start;
    System.OS.PendMessageQueue = PendMessageQueue;
    System.OS.PostMessageQueue = PostMessageQueue;
    System.OS.DelayMs = DelayMs;
}

