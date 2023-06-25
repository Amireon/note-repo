#include "STC8h.h"
#include "stc32_stc8_usb.h"
#include <intrins.h>   
#include <string.h>
#include "stdio.h"
#include "delay.h"
#include "sensor.h"
#include "tm1637.h"


/**
ADC转换器： 模拟量，数字量
S: P1.0
-: GND
没标: VCC
*/


char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#"; //不停电自动 ISP 下载命令
char SendBuff[50];	
unsigned int light = 0;			// ADC转换数据


/** USB初始化 */
void USB_Minit() {
	P_SW2 |= 0x80;
	
	P3M0 &= ~0x03; //P3.0/P3.1 和 USB 的 D-/D+共用 PIN 脚，
	P3M1 |= 0x03; //需要将 P3.0/P3.1 设置为高阻输入模式
	IRC48MCR = 0x80; //使能内部 48M 的 USB 专用 IRC
	while (!(IRC48MCR & 0x01));
	USBCLK = 0x00; //设置 USB 时钟源为内部 48M 的 USB 专用 IRC
	USBCON = 0x90; //使能 USB 功能
	usb_init(); //调用 USB CDC 初始化库函数
	IE2 |= 0x80; //使能 USB 中断
	EA = 1;
	while (DeviceState != DEVSTATE_CONFIGURED); //等待 USB 完成配置
}


/** 光传感器初始化 */
void OPS_Init()
{
	//AD初始化
	P1M0 = 0x00; P1M1 = 0x01; 
	P_SW2 |= 0x80;
	ADCTIM = 0x3f;                              //设置ADC内部时序
	P_SW2 &= 0x7f;
	ADCCFG = 0x2f;                              //设置ADC时钟为系统时钟/2/16
	ADC_CONTR = 0x80;                           //使能ADC模块
	
//	USB_Minit();								//USB初始化
}


unsigned int OPS_Get()
{
	//AD采集
	ADC_CONTR |= 0x40;                      //启动AD转换
	_nop_();
	_nop_();
	while (!(ADC_CONTR & 0x20));            //查询ADC完成标志
	ADC_CONTR &= ~0x20;                     //清完成标志
	light = ADC_RES;                        //读取ADC结果
	light = light * 256;
	light += ADC_RESL;
	
	// 数码管显示数据
	// TM1637_DisplayInt(light);
	
//	//发送数据 转换为电压公式=temp*3.3/4095
//	sprintf(SendBuff,"ADC=%d\r\n\0", light);
//	USB_SendData(SendBuff,strlen(SendBuff));

	return light;
}