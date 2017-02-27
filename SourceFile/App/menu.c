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
*文件名     : menu.c
*作用       : 界面设计
*原理       : 例子程序中包含五个窗体Form，每一个窗体可以加载BackText背景文字、
*           : Chart表、Label标签、TextBox文本四类控件，采用Add方法加载之后，
*           : 需要初始化值。
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/07/21       初始版本
********************************************************************************
*/

#include "system.h"
#include "app.h"



static void InitLogoForm(void)
{
    System.Gui.Form.Init(&App.Menu.LogoForm);
    App.Menu.LogoForm.BackTextPointer = "    雨滴科技    "
                                        "      msOS      "
                                        "     V1.3.2     "
                                        "   2016.12.05   ";
}
static void InitCheckForm(void)
{
    static Chart CheckChart;
    int i;
    System.Gui.Form.Init(&App.Menu.CheckForm);
    CheckChart.Character = '*';
    for(i = 0; i < 16; i++)
        CheckChart.Column[i] = 0;

    App.Menu.CheckForm.ChartPointer = &CheckChart;
}

void InitWorkForm(void)
{
    static TextBox FrequencyTextBox;
    static TextBox PowerPercentTextBox;
    static Label PowerLabel;
    static Label VoltageLabel;
    static Label CurrentLabel;
    static Label TemperatureLabel;
    static Label OnOffLabel;
    static const string OnOffString[] = {"关", "开"};
    
    System.Gui.Form.Init(&App.Menu.WorkForm);
    App.Menu.WorkForm.BackTextPointer = "频率          Hz"
                                        "功率   %       W"
                                        "电压    电流    "
                                        "温度   C        ";

    System.Gui.Form.AddTextBox(&App.Menu.WorkForm, &FrequencyTextBox);
    FrequencyTextBox.DataPointer = (void *)(&App.Data.Frequency);
    FrequencyTextBox.Type = GuiDataTypeIntDec;
    FrequencyTextBox.DataMax = 980000;
    FrequencyTextBox.DataMin = 600000;
    FrequencyTextBox.DataBigStep = 10000;
    FrequencyTextBox.DataStep = 1000;
    FrequencyTextBox.X = 13;
    FrequencyTextBox.Y = 0;
    
    System.Gui.Form.AddTextBox(&App.Menu.WorkForm, &PowerPercentTextBox);
    PowerPercentTextBox.DataPointer = (void *)(&App.Data.PowerPercent);
    PowerPercentTextBox.Type = GuiDataTypeIntDec;
    PowerPercentTextBox.DataMax = 100;
    PowerPercentTextBox.DataMin = 0;
    PowerPercentTextBox.DataBigStep = 10;
    PowerPercentTextBox.DataStep = 1;
    PowerPercentTextBox.X = 6;
    PowerPercentTextBox.Y = 1;
    
    System.Gui.Form.AddLabel(&App.Menu.WorkForm, &PowerLabel);
    PowerLabel.DataPointer = (void *)(&App.Data.Power);
    PowerLabel.Type = GuiDataTypeIntDec;
    PowerLabel.X = 14;
    PowerLabel.Y = 1;
    
    System.Gui.Form.AddLabel(&App.Menu.WorkForm, &VoltageLabel);
    VoltageLabel.DataPointer = (void *)(&App.Data.Voltage);
    VoltageLabel.Type = GuiDataTypeIntDec;
    VoltageLabel.X = 7;
    VoltageLabel.Y = 2;
    
    System.Gui.Form.AddLabel(&App.Menu.WorkForm, &CurrentLabel);
    CurrentLabel.DataPointer = (void *)(&App.Data.Current);
    CurrentLabel.Type = GuiDataTypeIntDec;
    CurrentLabel.X = 15;
    CurrentLabel.Y = 2;
    
    System.Gui.Form.AddLabel(&App.Menu.WorkForm, &TemperatureLabel);
    TemperatureLabel.DataPointer = (void *)(&App.Data.Temperature);
    TemperatureLabel.Type = GuiDataTypeIntDec;
    TemperatureLabel.X = 6;
    TemperatureLabel.Y = 3;

    System.Gui.Form.AddLabel(&App.Menu.WorkForm, &OnOffLabel);
    OnOffLabel.DataPointer = (void *)(&App.Data.OnOff);
    OnOffLabel.Type = GuiDataTypeSnString;
    OnOffLabel.Align = GuiDataAlignRight;
    OnOffLabel.StringBlockPointer = OnOffString;
    OnOffLabel.X = 15;
    OnOffLabel.Y = 3;
}

static void InitSetupForm(void)
{
    static TextBox MaxPowerTextBox;
    static TextBox MaxTemperatureTextBox;
    static TextBox MaxFrequencyTextBox;
    static TextBox MaxFrequencyOffsetTextBox;
    static TextBox MaxPressTextBox;
    static Label StateLabel;
    static const string StateString[] = 
    {
        "开机扫频",
        "正常待机",
        "定频工作",
        "锁相工作",
        "异常报警"
    };
    
    System.Gui.Form.Init(&App.Menu.SetupForm);
    App.Menu.SetupForm.BackTextPointer = "功率    温度   C"
                                         "频率   K频偏   K"
                                         "水压            "
                                         "                ";
    System.Gui.Form.AddTextBox(&App.Menu.SetupForm, &MaxPowerTextBox);
    MaxPowerTextBox.DataPointer = (void *)(&App.Data.MaxPower);
    MaxPowerTextBox.Type = GuiDataTypeIntDec;
    MaxPowerTextBox.DataMax = 7500;
    MaxPowerTextBox.DataMin = 4000;
    MaxPowerTextBox.DataBigStep = 500;
    MaxPowerTextBox.DataStep = 500;
    MaxPowerTextBox.X = 7;
    MaxPowerTextBox.Y = 0;
    
    System.Gui.Form.AddTextBox(&App.Menu.SetupForm, &MaxTemperatureTextBox);
    MaxTemperatureTextBox.DataPointer = (void *)(&App.Data.MaxTemperature);
    MaxTemperatureTextBox.Type = GuiDataTypeIntDec;
    MaxTemperatureTextBox.DataMax = 60;
    MaxTemperatureTextBox.DataMin = 40;
    MaxTemperatureTextBox.DataBigStep = 1;
    MaxTemperatureTextBox.DataStep = 1;
    MaxTemperatureTextBox.X = 14;
    MaxTemperatureTextBox.Y = 0;
    
    System.Gui.Form.AddTextBox(&App.Menu.SetupForm, &MaxFrequencyTextBox);
    MaxFrequencyTextBox.DataPointer = (void *)(&App.Data.MaxFrequency);
    MaxFrequencyTextBox.Type = GuiDataTypeIntDec;
    MaxFrequencyTextBox.DataMax = 980000;
    MaxFrequencyTextBox.DataMin = 500000;
    MaxFrequencyTextBox.DataBigStep = 100000;
    MaxFrequencyTextBox.DataStep = 100000;
    MaxFrequencyTextBox.Coefficient = 0.001;
    MaxFrequencyTextBox.X = 6;
    MaxFrequencyTextBox.Y = 1;
    
    System.Gui.Form.AddTextBox(&App.Menu.SetupForm, &MaxFrequencyOffsetTextBox);
    MaxFrequencyOffsetTextBox.DataPointer = (void *)(&App.Data.MaxFrequencyOffset);
    MaxFrequencyOffsetTextBox.Type = GuiDataTypeIntDec;
    MaxFrequencyOffsetTextBox.DataMax = 100000;
    MaxFrequencyOffsetTextBox.DataMin = 50000;
    MaxFrequencyOffsetTextBox.DataBigStep = 10000;
    MaxFrequencyOffsetTextBox.DataStep = 10000;
    MaxFrequencyOffsetTextBox.Coefficient = 0.001;
    MaxFrequencyOffsetTextBox.X = 14;
    MaxFrequencyOffsetTextBox.Y = 1;

    System.Gui.Form.AddTextBox(&App.Menu.SetupForm, &MaxPressTextBox);
    MaxPressTextBox.DataPointer = (void *)(&App.Data.MaxPress);
    MaxPressTextBox.Type = GuiDataTypeFloatDec;
    MaxPressTextBox.Digits = 1;
    Float(MaxPressTextBox.DataMax) = 5.0;
    Float(MaxPressTextBox.DataMin) = 1.0;
    Float(MaxPressTextBox.DataBigStep) = 0.5;
    Float(MaxPressTextBox.DataStep) = 0.1;
    MaxPressTextBox.X = 7;
    MaxPressTextBox.Y = 2; 

    System.Gui.Form.AddLabel(&App.Menu.SetupForm, &StateLabel);
    StateLabel.DataPointer = (void *)(&App.Data.State);
    StateLabel.Type = GuiDataTypeSnString;
    StateLabel.Align = GuiDataAlignLeft;
    StateLabel.StringBlockPointer = StateString;
    StateLabel.X = 8;
    StateLabel.Y = 2;
}

static void InitServiceForm(void)
{
    static TextBox SerialNumberTextBox;
    static TextBox YearTextBox;
    static TextBox MonthTextBox;
    static TextBox DayTextBox;
    static Label IdentifyNumber0Label;
    static Label IdentifyNumber1Label;
    static Label IdentifyNumber2Label;

    System.Gui.Form.Init(&App.Menu.ServiceForm);
    App.Menu.ServiceForm.BackTextPointer = "出厂编号        "
                                           "出厂日期  :  :  "
                                           "                "
                                           "                ";
    System.Gui.Form.AddTextBox(&App.Menu.ServiceForm, &SerialNumberTextBox);
    SerialNumberTextBox.DataPointer = (void *)(&App.Data.SerialNumber);
    SerialNumberTextBox.Type = GuiDataTypeUintDec;
    SerialNumberTextBox.DataMax = 10000;
    SerialNumberTextBox.DataMin = 1;
    SerialNumberTextBox.DataBigStep = 100;
    SerialNumberTextBox.DataStep = 1;
    SerialNumberTextBox.X = 15;
    SerialNumberTextBox.Y = 0;

    System.Gui.Form.AddTextBox(&App.Menu.ServiceForm, &YearTextBox);
    YearTextBox.DataPointer = (void *)(&App.Data.Year);
    YearTextBox.Type = GuiDataTypeIntDec;
    YearTextBox.DataMax = 99;
    YearTextBox.DataMin = 14;
    YearTextBox.DataBigStep = 10;
    YearTextBox.DataStep = 1;
    YearTextBox.X = 9;
    YearTextBox.Y = 1;

    System.Gui.Form.AddTextBox(&App.Menu.ServiceForm, &MonthTextBox);
    MonthTextBox.DataPointer = (void *)(&App.Data.Month);
    MonthTextBox.Type = GuiDataTypeIntDec;
    MonthTextBox.DataMax = 12;
    MonthTextBox.DataMin = 1;
    MonthTextBox.DataBigStep = 10;
    MonthTextBox.DataStep = 1;
    MonthTextBox.X = 12;
    MonthTextBox.Y = 1;

    System.Gui.Form.AddTextBox(&App.Menu.ServiceForm, &DayTextBox);
    DayTextBox.DataPointer = (void *)(&App.Data.Day);
    DayTextBox.Type = GuiDataTypeIntDec;
    DayTextBox.DataMax = 31;
    DayTextBox.DataMin = 1;
    DayTextBox.DataBigStep = 10;
    DayTextBox.DataStep = 1;
    DayTextBox.X = 15;
    DayTextBox.Y = 1;
   
    System.Gui.Form.AddLabel(&App.Menu.ServiceForm, &IdentifyNumber0Label);
    IdentifyNumber0Label.DataPointer = (void *)(&App.Data.IdentifyNumber0);
    IdentifyNumber0Label.Type = GuiDataTypeUintHex;
    IdentifyNumber0Label.X = 7;
    IdentifyNumber0Label.Y = 2;
    IdentifyNumber0Label.Digits= 8;
    
    System.Gui.Form.AddLabel(&App.Menu.ServiceForm, &IdentifyNumber1Label);
    IdentifyNumber1Label.DataPointer = (void *)(&App.Data.IdentifyNumber1);
    IdentifyNumber1Label.Type = GuiDataTypeUintHex;
    IdentifyNumber1Label.X = 15;
    IdentifyNumber1Label.Y = 2;
    IdentifyNumber1Label.Digits = 8;
    
    System.Gui.Form.AddLabel(&App.Menu.ServiceForm, &IdentifyNumber2Label);
    IdentifyNumber2Label.DataPointer = (void *)(&App.Data.IdentifyNumber2);
    IdentifyNumber2Label.Type = GuiDataTypeUintHex;
    IdentifyNumber2Label.X = 7;
    IdentifyNumber2Label.Y = 3;
    IdentifyNumber2Label.Digits = 8;
}

static void InitTimeForm(void)
{
    static TextBox RtcYearTextBox;
    static TextBox RtcMonthTextBox;
    static TextBox RtcWeekTextBox;
    static TextBox RtcDayTextBox;
    static TextBox RtcHourTextBox;
    static TextBox RtcMinuteTextBox;
    static TextBox RtcSecondTextBox;
    static const string WeekString[] = {"日", "一", "二", "三", "四", "五", "六"};
    
    System.Gui.Form.Init(&App.Menu.TimeForm);
    App.Menu.TimeForm.BackTextPointer = "    时    间    "
                                        "                "
                                        "      年  月  日"
                                        "星期      :  :  ";
    System.Gui.Form.AddTextBox(&App.Menu.TimeForm, &RtcYearTextBox);
    RtcYearTextBox.DataPointer = (void *)(&App.Data.Rtc.Year);
    RtcYearTextBox.Type = GuiDataTypeUshortDec;
    RtcYearTextBox.DataMax = 2100;
    RtcYearTextBox.DataMin = 2000;
    RtcYearTextBox.DataBigStep = 10;
    RtcYearTextBox.DataStep = 1;
    RtcYearTextBox.X = 5;
    RtcYearTextBox.Y = 2; 

    System.Gui.Form.AddTextBox(&App.Menu.TimeForm, &RtcMonthTextBox);
    RtcMonthTextBox.DataPointer = (void *)(&App.Data.Rtc.Month);
    RtcMonthTextBox.Type = GuiDataTypeByteDec;
    RtcMonthTextBox.DataMax = 12;
    RtcMonthTextBox.DataMin = 1;
    RtcMonthTextBox.DataBigStep = 2;
    RtcMonthTextBox.DataStep = 1;
    RtcMonthTextBox.X = 9;
    RtcMonthTextBox.Y = 2; 

    System.Gui.Form.AddTextBox(&App.Menu.TimeForm, &RtcDayTextBox);
    RtcDayTextBox.DataPointer = (void *)(&App.Data.Rtc.Day);
    RtcDayTextBox.Type = GuiDataTypeByteDec;
    RtcDayTextBox.DataMax = 31;
    RtcDayTextBox.DataMin = 0;
    RtcDayTextBox.DataBigStep = 1;
    RtcDayTextBox.DataStep = 1;
    RtcDayTextBox.X = 13;
    RtcDayTextBox.Y = 2;

    System.Gui.Form.AddTextBox(&App.Menu.TimeForm, &RtcWeekTextBox);
    RtcWeekTextBox.DataPointer = (void *)(&App.Data.Rtc.Week);
    RtcWeekTextBox.Type = GuiDataTypeSnString;
    RtcWeekTextBox.Align = GuiDataAlignLeft;
    RtcWeekTextBox.StringBlockPointer = WeekString;
    RtcWeekTextBox.DataMax = 6;
    RtcWeekTextBox.DataMin = 0;
    RtcWeekTextBox.DataBigStep = 1;
    RtcWeekTextBox.DataStep = 1;
    RtcWeekTextBox.X = 4;
    RtcWeekTextBox.Y = 3; 

    System.Gui.Form.AddTextBox(&App.Menu.TimeForm, &RtcHourTextBox);
    RtcHourTextBox.DataPointer = (void *)(&App.Data.Rtc.Hour);
    RtcHourTextBox.Type = GuiDataTypeByteDec;
    RtcHourTextBox.DataMax = 59;
    RtcHourTextBox.DataMin = 0;
    RtcHourTextBox.DataBigStep = 1;
    RtcHourTextBox.DataStep = 1;
    RtcHourTextBox.X = 9;
    RtcHourTextBox.Y = 3; 

    System.Gui.Form.AddTextBox(&App.Menu.TimeForm, &RtcMinuteTextBox);
    RtcMinuteTextBox.DataPointer = (void *)(&App.Data.Rtc.Minute);
    RtcMinuteTextBox.Type = GuiDataTypeByteDec;
    RtcMinuteTextBox.DataMax = 59;
    RtcMinuteTextBox.DataMin = 0;
    RtcMinuteTextBox.DataBigStep = 1;
    RtcMinuteTextBox.DataStep = 1;
    RtcMinuteTextBox.X = 12;
    RtcMinuteTextBox.Y = 3; 

    System.Gui.Form.AddTextBox(&App.Menu.TimeForm, &RtcSecondTextBox);
    RtcSecondTextBox.DataPointer = (void *)(&App.Data.Rtc.Second);
    RtcSecondTextBox.Type = GuiDataTypeByteDec;
    RtcSecondTextBox.DataMax = 59;
    RtcSecondTextBox.DataMin = 0;
    RtcSecondTextBox.DataBigStep = 1;
    RtcSecondTextBox.DataStep = 1;
    RtcSecondTextBox.X = 15;
    RtcSecondTextBox.Y = 3;
}

static void InitPortForm(void)
{
    static Label X0Label;
    static Label X1Label;
    static Label X2Label;
    static Label X3Label;

    static TextBox Y0TextBox;
    static TextBox Y1TextBox;
    static TextBox Y2TextBox;
    static TextBox Y3TextBox;
    static TextBox Y4TextBox;
    static TextBox Y5TextBox;

    static Label Adc0Label;
    static Label Adc1Label;
    static Label Adc2Label;
    static Label Adc3Label;

    System.Gui.Form.Init(&App.Menu.PortForm);
    App.Menu.PortForm.BackTextPointer = "X   :   Y       "
                                        "A0:     A1:     "
                                        "A2:     A3:     "
                                        "                ";
    System.Gui.Form.AddLabel(&App.Menu.PortForm, &X0Label);
    X0Label.DataPointer = (void *)(App.Data.DI.pX0);
    X0Label.Type = GuiDataTypeByteDec;
    X0Label.X = 2;
    X0Label.Y = 0; 
    
    System.Gui.Form.AddLabel(&App.Menu.PortForm, &X1Label);
    X1Label.DataPointer = (void *)(App.Data.DI.pX1);
    X1Label.Type = GuiDataTypeByteDec;
    X1Label.X = 3;
    X1Label.Y = 0; 

    System.Gui.Form.AddLabel(&App.Menu.PortForm, &X2Label);
    X2Label.DataPointer = (void *)(App.Data.DI.pX2);
    X2Label.Type = GuiDataTypeByteDec;
    X2Label.X = 4;
    X2Label.Y = 0; 
    
    System.Gui.Form.AddLabel(&App.Menu.PortForm, &X3Label);
    X3Label.DataPointer = (void *)(App.Data.DI.pX3);
    X3Label.Type = GuiDataTypeByteDec;
    X3Label.X = 5;
    X3Label.Y = 0; 
    /***********************************************/
    System.Gui.Form.AddTextBox(&App.Menu.PortForm, &Y0TextBox);
    Y0TextBox.DataPointer = (void *)(&App.Data.DO.Y0);
    Y0TextBox.Type = GuiDataTypeByteDec;
    Y0TextBox.X = 10;
    Y0TextBox.Y = 0;
    Y0TextBox.DataMax = 1;
    Y0TextBox.DataMin = 0;
    Y0TextBox.DataBigStep = 1;
    Y0TextBox.DataStep = 1;

    System.Gui.Form.AddTextBox(&App.Menu.PortForm, &Y1TextBox);
    Y1TextBox.DataPointer = (void *)(&App.Data.DO.Y1);
    Y1TextBox.Type = GuiDataTypeByteDec;
    Y1TextBox.X = 11;
    Y1TextBox.Y = 0;
    Y1TextBox.DataMax = 1;
    Y1TextBox.DataMin = 0;
    Y1TextBox.DataBigStep = 1;
    Y1TextBox.DataStep = 1;

    System.Gui.Form.AddTextBox(&App.Menu.PortForm, &Y2TextBox);
    Y2TextBox.DataPointer = (void *)(&App.Data.DO.Y2);
    Y2TextBox.Type = GuiDataTypeByteDec;
    Y2TextBox.X = 12;
    Y2TextBox.Y = 0;
    Y2TextBox.DataMax = 1;
    Y2TextBox.DataMin = 0;
    Y2TextBox.DataBigStep = 1;
    Y2TextBox.DataStep = 1;

    System.Gui.Form.AddTextBox(&App.Menu.PortForm, &Y3TextBox);
    Y3TextBox.DataPointer = (void *)(&App.Data.DO.Y3);
    Y3TextBox.Type = GuiDataTypeByteDec;
    Y3TextBox.X = 13;
    Y3TextBox.Y = 0;
    Y3TextBox.DataMax = 1;
    Y3TextBox.DataMin = 0;
    Y3TextBox.DataBigStep = 1;
    Y3TextBox.DataStep = 1;

    System.Gui.Form.AddTextBox(&App.Menu.PortForm, &Y4TextBox);
    Y4TextBox.DataPointer = (void *)(&App.Data.DO.Y4);
    Y4TextBox.Type = GuiDataTypeByteDec;
    Y4TextBox.X = 14;
    Y4TextBox.Y = 0;
    Y4TextBox.DataMax = 1;
    Y4TextBox.DataMin = 0;
    Y4TextBox.DataBigStep = 1;
    Y4TextBox.DataStep = 1;

    System.Gui.Form.AddTextBox(&App.Menu.PortForm, &Y5TextBox);
    Y5TextBox.DataPointer = (void *)(&App.Data.DO.Y5);
    Y5TextBox.Type = GuiDataTypeByteDec;
    Y5TextBox.X = 15;
    Y5TextBox.Y = 0;
    Y5TextBox.DataMax = 1;
    Y5TextBox.DataMin = 0;
    Y5TextBox.DataBigStep = 1;
    Y5TextBox.DataStep = 1;
    /***********************************************/
    System.Gui.Form.AddLabel(&App.Menu.PortForm, &Adc0Label);
    Adc0Label.DataPointer = (void *)(App.Data.Adc.pA0);
    Adc0Label.Type = GuiDataTypeUshortDec;
    Adc0Label.X = 6;
    Adc0Label.Y = 1;   

    System.Gui.Form.AddLabel(&App.Menu.PortForm, &Adc1Label);
    Adc1Label.DataPointer = (void *)(App.Data.Adc.pA1);
    Adc1Label.Type = GuiDataTypeUshortDec;
    Adc1Label.X = 15;
    Adc1Label.Y = 1;   

    System.Gui.Form.AddLabel(&App.Menu.PortForm, &Adc2Label);
    Adc2Label.DataPointer = (void *)(App.Data.Adc.pA2);
    Adc2Label.Type = GuiDataTypeUshortDec;
    Adc2Label.X = 6;
    Adc2Label.Y = 2;   

    System.Gui.Form.AddLabel(&App.Menu.PortForm, &Adc3Label);
    Adc3Label.DataPointer = (void *)(App.Data.Adc.pA3);
    Adc3Label.Type = GuiDataTypeUshortDec;
    Adc3Label.X = 15;
    Adc3Label.Y = 2;  
}

static void InitSubPlcForm(void)
{
    static Label X0Label;
    static Label X1Label;
    static Label X2Label;
    static Label X3Label;

    static TextBox Y0TextBox;
    static TextBox Y1TextBox;
    static TextBox Y2TextBox;
    static TextBox Y3TextBox;
    static TextBox Y4TextBox;
    static TextBox Y5TextBox;

    static Label Adc0Label;
    static Label Adc1Label;
    static Label Adc2Label;
    static Label Adc3Label;
    
    static TextBox Reg1TextBox;
    static TextBox Reg0TextBox;
    static TextBox RegTextBox;

    System.Gui.Form.Init(&App.Menu.SubPlcForm);
    App.Menu.SubPlcForm.BackTextPointer = "X   :   Y       "
                                          "A0:     A1:     "
                                          "A2:     A3:     "
                                          "                ";
    System.Gui.Form.AddLabel(&App.Menu.SubPlcForm, &X0Label);
    X0Label.DataPointer = (void *)(&App.Data.SubPlc.DI.X0);
    X0Label.Type = GuiDataTypeByteDec;
    X0Label.X = 2;
    X0Label.Y = 0; 
    
    System.Gui.Form.AddLabel(&App.Menu.SubPlcForm, &X1Label);
    X1Label.DataPointer = (void *)(&App.Data.SubPlc.DI.X1);
    X1Label.Type = GuiDataTypeByteDec;
    X1Label.X = 3;
    X1Label.Y = 0; 

    System.Gui.Form.AddLabel(&App.Menu.SubPlcForm, &X2Label);
    X2Label.DataPointer = (void *)(&App.Data.SubPlc.DI.X2);
    X2Label.Type = GuiDataTypeByteDec;
    X2Label.X = 4;
    X2Label.Y = 0; 
    
    System.Gui.Form.AddLabel(&App.Menu.SubPlcForm, &X3Label);
    X3Label.DataPointer = (void *)(&App.Data.SubPlc.DI.X3);
    X3Label.Type = GuiDataTypeByteDec;
    X3Label.X = 5;
    X3Label.Y = 0; 
    /***********************************************/
    System.Gui.Form.AddTextBox(&App.Menu.SubPlcForm, &Y0TextBox);
    Y0TextBox.DataPointer = (void *)(&App.Data.SubPlc.DO.Y0);
    Y0TextBox.Type = GuiDataTypeByteDec;
    Y0TextBox.X = 10;
    Y0TextBox.Y = 0;
    Y0TextBox.DataMax = 1;
    Y0TextBox.DataMin = 0;
    Y0TextBox.DataBigStep = 1;
    Y0TextBox.DataStep = 1;

    System.Gui.Form.AddTextBox(&App.Menu.SubPlcForm, &Y1TextBox);
    Y1TextBox.DataPointer = (void *)(&App.Data.SubPlc.DO.Y1);
    Y1TextBox.Type = GuiDataTypeByteDec;
    Y1TextBox.X = 11;
    Y1TextBox.Y = 0;
    Y1TextBox.DataMax = 1;
    Y1TextBox.DataMin = 0;
    Y1TextBox.DataBigStep = 1;
    Y1TextBox.DataStep = 1;

    System.Gui.Form.AddTextBox(&App.Menu.SubPlcForm, &Y2TextBox);
    Y2TextBox.DataPointer = (void *)(&App.Data.SubPlc.DO.Y2);
    Y2TextBox.Type = GuiDataTypeByteDec;
    Y2TextBox.X = 12;
    Y2TextBox.Y = 0;
    Y2TextBox.DataMax = 1;
    Y2TextBox.DataMin = 0;
    Y2TextBox.DataBigStep = 1;
    Y2TextBox.DataStep = 1;

    System.Gui.Form.AddTextBox(&App.Menu.SubPlcForm, &Y3TextBox);
    Y3TextBox.DataPointer = (void *)(&App.Data.SubPlc.DO.Y3);
    Y3TextBox.Type = GuiDataTypeByteDec;
    Y3TextBox.X = 13;
    Y3TextBox.Y = 0;
    Y3TextBox.DataMax = 1;
    Y3TextBox.DataMin = 0;
    Y3TextBox.DataBigStep = 1;
    Y3TextBox.DataStep = 1;

    System.Gui.Form.AddTextBox(&App.Menu.SubPlcForm, &Y4TextBox);
    Y4TextBox.DataPointer = (void *)(&App.Data.SubPlc.DO.Y4);
    Y4TextBox.Type = GuiDataTypeByteDec;
    Y4TextBox.X = 14;
    Y4TextBox.Y = 0;
    Y4TextBox.DataMax = 1;
    Y4TextBox.DataMin = 0;
    Y4TextBox.DataBigStep = 1;
    Y4TextBox.DataStep = 1;

    System.Gui.Form.AddTextBox(&App.Menu.SubPlcForm, &Y5TextBox);
    Y5TextBox.DataPointer = (void *)(&App.Data.SubPlc.DO.Y5);
    Y5TextBox.Type = GuiDataTypeByteDec;
    Y5TextBox.X = 15;
    Y5TextBox.Y = 0;
    Y5TextBox.DataMax = 1;
    Y5TextBox.DataMin = 0;
    Y5TextBox.DataBigStep = 1;
    Y5TextBox.DataStep = 1;
    /***********************************************/
    System.Gui.Form.AddLabel(&App.Menu.SubPlcForm, &Adc0Label);
    Adc0Label.DataPointer = (void *)(&App.Data.SubPlc.Adc.A0);
    Adc0Label.Type = GuiDataTypeUshortDec;
    Adc0Label.X = 6;
    Adc0Label.Y = 1;   

    System.Gui.Form.AddLabel(&App.Menu.SubPlcForm, &Adc1Label);
    Adc1Label.DataPointer = (void *)(&App.Data.SubPlc.Adc.A1);
    Adc1Label.Type = GuiDataTypeUshortDec;
    Adc1Label.X = 15;
    Adc1Label.Y = 1;   

    System.Gui.Form.AddLabel(&App.Menu.SubPlcForm, &Adc2Label);
    Adc2Label.DataPointer = (void *)(&App.Data.SubPlc.Adc.A2);
    Adc2Label.Type = GuiDataTypeUshortDec;
    Adc2Label.X = 6;
    Adc2Label.Y = 2;   

    System.Gui.Form.AddLabel(&App.Menu.SubPlcForm, &Adc3Label);
    Adc3Label.DataPointer = (void *)(&App.Data.SubPlc.Adc.A3);
    Adc3Label.Type = GuiDataTypeUshortDec;
    Adc3Label.X = 15;
    Adc3Label.Y = 2; 
    /***********************************************/
    System.Gui.Form.AddTextBox(&App.Menu.SubPlcForm, &Reg1TextBox);
    Reg1TextBox.DataPointer = (void *)((ushort*)&App.Data.SubPlc.Reg + 1);
    Reg1TextBox.Type = GuiDataTypeUshortHex;
    Reg1TextBox.X = 3;
    Reg1TextBox.Y = 3;
    Reg1TextBox.DataMax = 0xFFFF;
    Reg1TextBox.DataMin = 0;
    Reg1TextBox.DataBigStep = 0x10;
    Reg1TextBox.DataStep = 1;
    Reg1TextBox.Digits = 4;

    System.Gui.Form.AddTextBox(&App.Menu.SubPlcForm, &Reg0TextBox);
    Reg0TextBox.DataPointer = (void *)((ushort*)&App.Data.SubPlc.Reg);
    Reg0TextBox.Type = GuiDataTypeUshortHex;
    Reg0TextBox.X = 7;
    Reg0TextBox.Y = 3;
    Reg0TextBox.DataMax = 0xFFFF;
    Reg0TextBox.DataMin = 0;
    Reg0TextBox.DataBigStep = 0x10;
    Reg0TextBox.DataStep = 1;
    Reg0TextBox.Digits = 4;
    
    System.Gui.Form.AddTextBox(&App.Menu.SubPlcForm, &RegTextBox);
    RegTextBox.DataPointer = (void *)(&App.Data.SubPlc.Reg);
    RegTextBox.Type = GuiDataTypeIntDec;
    RegTextBox.X = 15;
    RegTextBox.Y = 3;
    RegTextBox.DataMax = 999999;
    RegTextBox.DataMin = 0;
    RegTextBox.DataBigStep = 10;
    RegTextBox.DataStep = 1;
}

void InitMenu(void)
{
    InitLogoForm();
    InitCheckForm();
    InitWorkForm();
    InitSetupForm();
    InitServiceForm();
    InitTimeForm();
    InitPortForm();
    InitSubPlcForm();
}
/*******************************************************************************
* 函数名	: MenuTask
* 描述	    : MenuTask任务，必须为最低优先级任务，解析窗体Form内的各个控件用于显示
* 输入参数  : 无
* 返回参数  : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2013/12/16       修改版本
*******************************************************************************/
void MenuTask (void)
{
    static uint MenuSystick; 
    
    InitMenu();
    MenuSystick = App.Data.Systick1000 + 100;
    while(true)             
    {
        if (App.Data.Systick1000 >= MenuSystick)
        {
            MenuSystick = App.Data.Systick1000 + 100;
            System.Gui.Parse(App.Menu.FocusFormPointer);
        }
    }
}

