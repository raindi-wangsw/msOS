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
*文件名     : menu.h
*作用       : 菜单设计
*原理       : 无
********************************************************************************
*版本     作者            日期            说明
*V0.1    Wangsw        2014/06/21       初始版本
********************************************************************************
*/


#ifndef __MENU_H
#define __MENU_H

typedef struct
{
    Form LogoForm;
    Form CheckForm;
    Form WorkForm;
    Form SetupForm;
    Form ServiceForm;
    Form PortForm;
    Form * FocusFormPointer;
}MenuStruct;

extern void InitMenu(void);

extern void MenuTask (void);

#endif /*__MMI_H*/


