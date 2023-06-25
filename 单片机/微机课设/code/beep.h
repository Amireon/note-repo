#ifndef __BEEP_H__
#define __BEEP_H__

void Beep_Init();

/** 外部控制: 播放, 暂停 */
void Beep_Set(unsigned int flag);

void Beep_Play();

#endif