#include "delay.h"
#include "intrins.h" //使用其nop()进行延时

/** 自定义延时 */
void Delay(unsigned int xms)
{
	unsigned char i, j;
	while(xms--)
	{
		i = 2;
		j = 239;
		do
		{
			while (--j);
		} while (--i);
	}
}


void Delay250ms()		//@12.000MHz
{
	unsigned char data i, j, k;

	_nop_();
	_nop_();
	i = 16;
	j = 57;
	k = 12;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}



void Delay500ms()		//@12.000MHz
{
	unsigned char data i, j, k;

	_nop_();
	_nop_();
	i = 23;
	j = 205;
	k = 120;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


void Delay1000ms()		//@12.000MHz
{
	unsigned char data i, j, k;

	_nop_();
	_nop_();
	i = 61;
	j = 225;
	k = 62;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


void Delay2000ms()		//@12.000MHz
{
	unsigned char data i, j, k;

	_nop_();
	_nop_();
	i = 122;
	j = 193;
	k = 128;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
