#include "stc8h.h"
#include "stc32_stc8_usb.h"
#include "sensor.h"
#include "delay.h"
#include "beep.h"
#include "tm1637.h"

sbit K1 = P3^4;     // T0
sbit K3 = P3^3;		// INI1 
sbit K4 = P3^5;     // T1
sbit Buzz = P1^1;   // ������

bit LED_Flag = 0;			   // ָʾ�Ƿ��޸Ĺ�
unsigned char Floor = 3;	   // ���ֵ, 3*500=1500, ���޷��޸�
unsigned char Ceiling = 5;	   // �⴫��������������ֵ, Ĭ��5*500=2500
//unsigned char Granularity = 500; // ��������
unsigned char count = 50;      // ��ʱ250ms
unsigned int ldegree = 0;      // ������Ϣ


/**
 * ��ʼ��MCU�˿�
 */
void Sys_Init() {
    P_SW2 |= 0x80;
	
	// ׼˫���ģʽ
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
 * INI1�ж�(�½��ش���)����
*/
void Int1_Init()
{
	
	IT1 = 1;	// ʹ��INI1�½����ж�
	EX1 = 1;    // ʹ��INT1�ж�
	EA = 1;     //
}


/**
 * ��ʱ��T2����, 5����@12.000MHz
*/
void Timer2_Init()		
{
	AUXR &= 0xFB;			//��ʱ��ʱ��12Tģʽ
	T2L = 0x78;				//���ö�ʱ��ʼֵ
	T2H = 0xEC;				//���ö�ʱ��ʼֵ
	AUXR |= 0x10;			//��ʱ��2��ʼ��ʱ
	IE2 = 0x04;				//ʹ�ܶ�ʱ�ж�
	EA = 1;
}


/**
 * �������, ���Ƶ�Ƭ����Ϊ
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
	} else if(LED_Flag == 1){ // ����Ƶ����ֵ
		LED_Flag = 0;
		Beep_Set(0);
		Buzz = 1;
		P2 = 0xaa;
	}
}



void main()
{
	Sys_Init();		// STC�˿ڳ�ʼ��
	OPS_Init();     // �⴫������ʼ��
	Beep_Init();	// ���������ų�ʼ��
//	USB_Minit();	// USB��ʼ��(������)
	
	Int1_Init();	// INT1�ж�
	Timer2_Init();  // Timer2��ʱ��
	EA = 1;			// ���жϿ�
		
	Buzz = 1;   	// ����������
	P2 = 0xaa;  	// LED��ʼ��˸
	
	Beep_Set(0);	// ���ò���������
	
	while(1)
	{
		Beep_Play();
	}
	
}


/** 
 * �ⲿ�ж�INT1, ���ñ�����ֵ
*/ 
void INT1_Isr() interrupt 2 
{
	Ceiling = (++Ceiling) % 9;		// ��Ϊ8��, [0, 8], ����Ϊ8 * 500 = 4000
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
 * ��ʱ��2, ���ö�ʱ����
*/
void Timer2_Isr() interrupt 12
{
	count--;
	if(count == 0)
	{
		count = 50;
		ldegree = OPS_Get();			// �⴫�����ɼ�
		TM1637_DisplayInt(ldegree);		// ��ʾ����
		Check_Ldegree();				// ���ƴ���
		P2 = ~P2;
    }
}
