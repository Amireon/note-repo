#include "stc8h.h"
#include "stc32_stc8_usb.h"
#include "sensor.h"
#include "delay.h"
#include "beep.h"
#include "tm1637.h"

sbit K1 = P3^4;     // T0
sbit K3 = P3^3;		// INI1 
sbit K4 = P3^5;     // T1
sbit Buzz = P1^1;   // 蜂鸣器

bit LED_Flag = 0;			   // 指示是否被修改过
unsigned char Floor = 3;	   // 最低值, 3*500=1500, 暂无法修改
unsigned char Ceiling = 5;	   // 光传感器报警触发阈值, 默认5*500=2500
//unsigned char Granularity = 500; // 划分粒度
unsigned char count = 50;      // 定时250ms
unsigned int ldegree = 0;      // 光照信息


/**
 * 初始化MCU端口
 */
void Sys_Init() {
    P_SW2 |= 0x80;
	
	// 准双向口模式
	P0M0 = 0x00;
    P0M1 = 0x00;
    P1M0 = 0x00;
    P1M1 = 0x00;
    P2M0 = 0x00;
    P2M1 = 0x00;
    P3M0 = 0x00;
    P3M1 = 0x00;
    P4M0 = 0x00;
    P4M1 = 0x00;
}


/**
 * INI1中断(下降沿触发)启动
*/
void Int1_Init()
{
	
	IT1 = 1;	// 使能INI1下降沿中断
	EX1 = 1;    // 使能INT1中断
	EA = 1;     //
}


/**
 * 定时器T2启动, 5毫秒@12.000MHz
*/
void Timer2_Init()		
{
	AUXR &= 0xFB;			//定时器时钟12T模式
	T2L = 0x78;				//设置定时初始值
	T2H = 0xEC;				//设置定时初始值
	AUXR |= 0x10;			//定时器2开始计时
	IE2 = 0x04;				//使能定时中断
	EA = 1;
}


/**
 * 检测亮度, 控制单片机行为
*/
void Check_Ldegree()
{
	if(ldegree <= Floor * 500)
	{
		Beep_Set(1);
	}
	else if(ldegree >= Ceiling * 500)
	{
		LED_Flag = 1;	
		Beep_Set(0);
		Buzz = 0;
		P2 = 0x00;
	} else if(LED_Flag == 1){ // 避免频繁赋值
		LED_Flag = 0;
		Beep_Set(0);
		Buzz = 1;
		P2 = 0xaa;
	}
}



void main()
{
	Sys_Init();		// STC端口初始化
	OPS_Init();     // 光传感器初始化
	Beep_Init();	// 蜂鸣器播放初始化
//	USB_Minit();	// USB初始化(不启用)
	
	Int1_Init();	// INT1中断
	Timer2_Init();  // Timer2定时器
	EA = 1;			// 总中断开
		
	Buzz = 1;   	// 蜂鸣器静音
	P2 = 0xaa;  	// LED初始闪烁
	
	Beep_Set(0);	// 设置不播放音乐
	
	while(1)
	{
		Beep_Play();
	}
	
}


/** 
 * 外部中断INT1, 设置报警阈值
*/ 
void INT1_Isr() interrupt 2 
{
	Ceiling = (++Ceiling) % 9;		// 分为8挡, [0, 8], 上限为8 * 500 = 4000
	if(Ceiling <= Floor)
	{
		Ceiling = Floor + 1;
	}
//	P2 = 0xaa;
	Buzz = 0;
	while(!K3);
	Buzz = 1;
}


/** 
 * 定时器2, 设置定时任务
*/
void Timer2_Isr() interrupt 12
{
	count--;
	if(count == 0)
	{
		count = 50;
		ldegree = OPS_Get();			// 光传感器采集
		TM1637_DisplayInt(ldegree);		// 显示数据
		Check_Ldegree();				// 控制处理
		P2 = ~P2;
    }
}
