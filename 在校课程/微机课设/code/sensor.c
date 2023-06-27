#include "STC8h.h"
#include "stc32_stc8_usb.h"
#include <intrins.h>   
#include <string.h>
#include "stdio.h"
#include "delay.h"
#include "sensor.h"
#include "tm1637.h"


/**
ADCת������ ģ������������
S: P1.0
-: GND
û��: VCC
*/


char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#"; //��ͣ���Զ� ISP ��������
char SendBuff[50];	
unsigned int light = 0;			// ADCת������


/** USB��ʼ�� */
void USB_Minit() {
	P_SW2 |= 0x80;
	
	P3M0 &= ~0x03; //P3.0/P3.1 �� USB �� D-/D+���� PIN �ţ�
	P3M1 |= 0x03; //��Ҫ�� P3.0/P3.1 ����Ϊ��������ģʽ
	IRC48MCR = 0x80; //ʹ���ڲ� 48M �� USB ר�� IRC
	while (!(IRC48MCR & 0x01));
	USBCLK = 0x00; //���� USB ʱ��ԴΪ�ڲ� 48M �� USB ר�� IRC
	USBCON = 0x90; //ʹ�� USB ����
	usb_init(); //���� USB CDC ��ʼ���⺯��
	IE2 |= 0x80; //ʹ�� USB �ж�
	EA = 1;
	while (DeviceState != DEVSTATE_CONFIGURED); //�ȴ� USB �������
}


/** �⴫������ʼ�� */
void OPS_Init()
{
	//AD��ʼ��
	P1M0 = 0x00; P1M1 = 0x01; 
	P_SW2 |= 0x80;
	ADCTIM = 0x3f;                              //����ADC�ڲ�ʱ��
	P_SW2 &= 0x7f;
	ADCCFG = 0x2f;                              //����ADCʱ��Ϊϵͳʱ��/2/16
	ADC_CONTR = 0x80;                           //ʹ��ADCģ��
	
//	USB_Minit();								//USB��ʼ��
}


unsigned int OPS_Get()
{
	//AD�ɼ�
	ADC_CONTR |= 0x40;                      //����ADת��
	_nop_();
	_nop_();
	while (!(ADC_CONTR & 0x20));            //��ѯADC��ɱ�־
	ADC_CONTR &= ~0x20;                     //����ɱ�־
	light = ADC_RES;                        //��ȡADC���
	light = light * 256;
	light += ADC_RESL;
	
	// �������ʾ����
	// TM1637_DisplayInt(light);
	
//	//�������� ת��Ϊ��ѹ��ʽ=temp*3.3/4095
//	sprintf(SendBuff,"ADC=%d\r\n\0", light);
//	USB_SendData(SendBuff,strlen(SendBuff));

	return light;
}