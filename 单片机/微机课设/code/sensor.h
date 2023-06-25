#ifndef __SENSOR_H__
#define __SENSOR_H__

// unsigned int light = 0;

// USB初始化
void USB_Minit();

/** 光传感器初始化 */
void OPS_Init();

/** 运行一次, 向USB发送一次数据 */
unsigned int OPS_Get();

#endif 