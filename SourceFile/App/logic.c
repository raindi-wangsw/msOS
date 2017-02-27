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
*文件名     : logic.c
*作用       : 业务逻辑处理文件
*原理       : 处理各类消息，如按键、传感器等，标准化一些按键的处理模式
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
********************************************************************************
*/


#include "system.h"
#include "app.h"



/*******************************************************************************
* 描述	    : 窗体页面焦点切换处理程序，切换窗体页面
*******************************************************************************/
static void FormFocusProcess(void)
{    
    if (App.Menu.FocusFormPointer == &App.Menu.SubPlcForm)
        App.Menu.FocusFormPointer = &App.Menu.WorkForm;
    else
        App.Menu.FocusFormPointer ++;
}

/*******************************************************************************
* 描述	    : 存储焦点文本控件的数据值
*******************************************************************************/
void SaveProcess(void)
{
    if (App.Menu.FocusFormPointer->FocusTextBoxPointer == null) return;
   
    System.Device.Storage.Parameter.Write((uint *)App.Menu.FocusFormPointer->FocusTextBoxPointer->DataPointer);
}


/*******************************************************************************
* 描述	    : 存储所有参数
*******************************************************************************/
void LongSaveProcess(void)
{
    TextBox * textBoxPointer;
    textBoxPointer = App.Menu.FocusFormPointer->TextBoxPointer;
    while (textBoxPointer != null)
    {
        System.Device.Storage.Parameter.Write(textBoxPointer->DataPointer);
        textBoxPointer = textBoxPointer->NextTextBoxPointer;
    }
}

/*******************************************************************************
* 描述	    : Stop缺省处理函数
*******************************************************************************/
static void StopDefaultProcess(void)
{
    App.Menu.FocusFormPointer->FocusTextBoxPointer = null;
    App.Menu.FocusFormPointer = &App.Menu.WorkForm;
}

static void TextBoxKeyProcess(KeyEnum key)
{
    void * dataPointer;
    // 以下仅为例子, 用于文本控件值修改后的直接操作，比如可以直接调用函数实现对外操作，非常重要
    dataPointer = App.Menu.FocusFormPointer->FocusTextBoxPointer->DataPointer;
    if (dataPointer == &App.Data.Frequency)
    {
        ;
    }
    else if(dataPointer == &App.Data.PowerPercent)
    {
        ;
    }
}

static byte Counter = 0;
/*******************************************************************************
* 描述	    : 按键消息处理函数
* 输入参数  : key，任意按键值
*******************************************************************************/
void KeyProcess(KeyEnum key)
{
    switch(key)
    {
        case KeyAdd:                // 数字按键操作
        case KeyLongAdd:
        case KeySub :
        case KeyLongSub:
            System.Gui.Form.ModifyTextBoxData(key);
            TextBoxKeyProcess(key);
            break;
            
        case KeyAuxUp:              // 写日志(测试)
            System.Device.Storage.Log.Write("Log Test = %d\n", Counter++);
            break;
        case KeyLongAuxUp:          // 打开PWMChannel3(测试)
            //System.Device.DO.Config(DOPWM);
            //System.Device.DO.Pwm.SetParameter(PwmChannel3, 18000, 1000, 500);

            //System.Device.DO.Pwm.SetDutyRatio(PwmChannel4, 500);
            //System.Device.DO.Pwm.Open(PwmChannel4);

            *App.Data.DO.pY0 = 0;
            *App.Data.DO.pY1 = 1;
            *App.Data.DO.pY2 = 0;
            *App.Data.DO.pY3 = 1;
            *App.Data.DO.pY4 = 0;
            *App.Data.DO.pY5 = 1;
            break;
        case KeyAuxDown:            // 读日志(测试)
            printf(System.Device.Storage.Log.Read(-1));
            break;
        case KeyLongAuxDown:        // 关闭PWMChannel0(测试)
            System.Device.DO.Pwm.Close(PwmChannel3);
            break;
            
        case KeySave:               // 焦点数据存储
            SaveProcess();     
            break;
        case KeyLongSave :          // 页数据存储
            LongSaveProcess();
            break;
            
        case KeyTextBoxFocus:       // 文本切换
            System.Gui.Form.SwitchTextBoxFocus();
            break; 
        case KeyLongFormFocus:      // 页面切换
            FormFocusProcess();
            break;   
 
        case KeyStart:              // 开始运行
            App.Data.OnOff = on;
            break;
        case KeyLongStart:          // 定时器(测试)
            System.Device.Timer.Start(0, TimerMessage, 1000, FormFocusProcess);
            System.Device.Timer.Start(1, TimerMessage, 2000, FormFocusProcess);
            System.Device.Timer.Start(2, TimerMessage, 3000, FormFocusProcess);
            System.Device.Timer.Start(3, TimerMessage, 4000, FormFocusProcess);
            System.Device.Timer.Start(4, TimerMessage, 5000, FormFocusProcess);
            System.Device.Timer.Start(5, TimerMessage, 6000, FormFocusProcess);
            System.Device.Timer.Start(6, TimerMessage, 7000, FormFocusProcess);
            System.Device.Timer.Start(7, TimerMessage, 7000, FormFocusProcess);
            break;
            
        case KeyStop:               //停止运行
            App.Data.OnOff = off;
        case KeyLongStop:
            //...
            StopDefaultProcess();
            break;         
    }
}

static void AppSystick100(void)
{
    //...
}

static void InitLogic(void)
{
    System.Device.Adc.Register(AdcChannel0, (ushort *)(&App.Data.Voltage));
    System.Device.Adc.Register(AdcChannel1, (ushort *)(&App.Data.Current));
    System.Device.Adc.Register(AdcTemperature, (ushort *)(&App.Data.Temperature));
    System.Device.Systick.Register(Systick100, AppSystick100);
}

void LogicTask(void)
{
    int i;
    uint message;
    uint data; 
    InitLogic();

    App.Menu.FocusFormPointer = &App.Menu.LogoForm;     //页面焦点

    System.OS.DelayMs(2000);
    
    //高频感应加热设备扫频自检，获取谐振点
    App.Menu.FocusFormPointer = &App.Menu.CheckForm;   
    for(i = 0; i < 16; i++)
    {
        System.OS.DelayMs(100);
        App.Menu.CheckForm.ChartPointer->Column[i] = i % 5;
    }
    App.Menu.FocusFormPointer = &App.Menu.WorkForm;   

    System.Device.Key.Enable(true);
    //逻辑业务任务获取消息，分配处理
    while(true)
    {     
        message = System.OS.PendMessageQueue();
        
        data = message & 0x00FFFFFF;
        switch(message >> 24)
        {
            case MessageKey:                    //按键消息
                KeyProcess((KeyEnum)data);
                break;
            case MessageTimer:
                Function(data + RomBase);
                break;
            default:
                break;
        }
    }
}
