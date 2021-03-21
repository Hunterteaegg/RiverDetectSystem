/*
 * ds18b20.h
 *
 *  Created on: Mar 21, 2021
 *      Author: hunterteaegg
 */

#ifndef DS18B20_DS18B20_H_
#define DS18B20_DS18B20_H_

#include <stdint.h>
#include <stm32f1xx.h>

typedef struct
{
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
	TIM_HandleTypeDef* tim;
	uint8_t ROM[8];
} DS18B20_t;

void DS18B20_init(DS18B20_t* handle);
int DS18B20_convert(DS18B20_t* handle);
void DS18B20_writeByte(DS18B20_t* handle, uint8_t byte);
uint8_t DS18B20_readByte(DS18B20_t* handle);
void DS18B20_delay(DS18B20_t* handle, uint16_t timeout_us);
void DS18B20_setHigh(DS18B20_t* handle);
void DS18B20_setLow(DS18B20_t* handle);
void DS18B20_input(DS18B20_t* handle);
void DS18B20_output(DS18B20_t* handle);
uint8_t DS18B20_reset(DS18B20_t* handle);
void DS18B20_writeBit(DS18B20_t* handle, uint8_t bit);
uint8_t DS18B20_readBit(DS18B20_t* handle);

#endif /* DS18B20_DS18B20_H_ */
