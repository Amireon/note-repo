#ifndef __SENSOR_H__
#define __SENSOR_H__

// unsigned int light = 0;

// USB��ʼ��
void USB_Minit();

/** �⴫������ʼ�� */
void OPS_Init();

/** ����һ��, ��USB����һ������ */
unsigned int OPS_Get();

#endif 