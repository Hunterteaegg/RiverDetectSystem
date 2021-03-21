/*
 * ds18b20.c
 *
 *  Created on: Mar 21, 2021
 *      Author: hunterteaegg
 */

#include "ds18b20.h"
#include <stdio.h>

void DS18B20_init(DS18B20_t* handle)
{
	HAL_TIM_Base_Start(handle->tim);
}

int DS18B20_convert(DS18B20_t* handle)
{
	int temperature = 0;
	int res;
	uint8_t TL, TH;

	res = DS18B20_reset(handle);
	printf("first res is %d\r\n", res);
	DS18B20_writeByte(handle, 0xCC);
	DS18B20_writeByte(handle, 0x44);
	res = DS18B20_reset(handle);
	printf("second res is %d\r\n", res);
	DS18B20_writeByte(handle, 0xCC);
	DS18B20_writeByte(handle, 0xBE);
	TL = DS18B20_readByte(handle);
	printf("TL = %d\r\n", TL);
	TH = DS18B20_readByte(handle);
	printf("TH = %d\r\n", TH);

	if(TH > 0x70)
	{
		  TH = ~TH;
		  TL = ~TL;
	}

	temperature = TH;
    temperature <<= 8;
	temperature += TL;
	temperature = temperature *10000 * 0.0625;

	return temperature;
}

inline void DS18B20_delay(DS18B20_t* handle, uint16_t timeout_us)
{
	handle->tim->Instance->CNT = 0;
	while(handle->tim->Instance->CNT <= timeout_us);
}

inline void DS18B20_setHigh(DS18B20_t* handle)
{
	handle->GPIOx->BSRR = handle->GPIO_Pin;
}

inline void DS18B20_setLow(DS18B20_t* handle)
{
	handle->GPIOx->BSRR = handle->GPIO_Pin << 16;
}

inline void DS18B20_input(DS18B20_t* handle)
{
	GPIO_InitTypeDef gpio_init;
	gpio_init.Mode = GPIO_MODE_INPUT;
	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_init.Pin = handle->GPIO_Pin;
	HAL_GPIO_Init(handle->GPIOx, &gpio_init);
}

inline void DS18B20_output(DS18B20_t* handle)
{
	GPIO_InitTypeDef gpio_init;
	gpio_init.Mode = GPIO_MODE_OUTPUT_OD;
	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_init.Pin = handle->GPIO_Pin;
	HAL_GPIO_Init(handle->GPIOx, &gpio_init);
}

inline uint8_t DS18B20_reset(DS18B20_t* handle)
{
	uint8_t res = 0;

	printf("entry reset\r\n");
	DS18B20_output(handle);
	DS18B20_setLow(handle);
	printf("reset: set output\r\n");
	DS18B20_delay(handle, 480);
	DS18B20_delay(handle, 20);
	DS18B20_input(handle);
	DS18B20_setHigh(handle);
	DS18B20_delay(handle, 70);
	while(HAL_GPIO_ReadPin(handle->GPIOx, handle->GPIO_Pin));

	return res;
}

inline void DS18B20_writeBit(DS18B20_t* handle, uint8_t bit)
{
	if(bit)
	{
		DS18B20_setLow(handle);
		DS18B20_output(handle);
		DS18B20_delay(handle, 10);

		DS18B20_setHigh(handle);
		DS18B20_input(handle);

		DS18B20_delay(handle, 55);
		DS18B20_input(handle);
	}
	else
	{
		DS18B20_setLow(handle);
		DS18B20_output(handle);
		DS18B20_delay(handle, 65);

		DS18B20_setHigh(handle);
		DS18B20_input(handle);

		DS18B20_delay(handle, 5);
		DS18B20_input(handle);
	}
}

inline uint8_t DS18B20_readBit(DS18B20_t* handle)
{
	uint8_t bit = 0;

	DS18B20_setLow(handle);
	DS18B20_output(handle);
	DS18B20_delay(handle, 2);

	DS18B20_setHigh(handle);
	DS18B20_input(handle);
	DS18B20_delay(handle, 10);

	if(HAL_GPIO_ReadPin(handle->GPIOx, handle->GPIO_Pin))
	{
		bit = 1;
	}

	DS18B20_delay(handle, 50);

	return bit;
}

void DS18B20_writeByte(DS18B20_t* handle, uint8_t byte)
{
	uint8_t i = 8;

	while(i--)
	{
		DS18B20_writeBit(handle, byte & 0x01);
		byte >>= 1;
	}
}

uint8_t DS18B20_readByte(DS18B20_t* handle)
{
	uint8_t i = 8, byte = 0;
	while(i--)
	{
		byte >>= 1;
		byte |= (DS18B20_readBit(handle) << 7);
	}

	return  byte;
}

