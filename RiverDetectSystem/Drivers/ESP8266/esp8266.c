#include <esp8266.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

uint8_t usart2_txbuf[256];
uint8_t usart2_rxbuf[512];
uint8_t usart2_rxone[1];
uint8_t usart_rxcounter;

static void esp8266_sendByte(uint8_t val)
{
	((UART_HandleTypeDef *)&huart2)->Instance->DR = ((uint16_t)val & (uint16_t)0x01FF);

	while((((UART_HandleTypeDef *)&huart2)->Instance->SR & 0x40) == 0);
}

void esp8266_sendBuf(uint8_t* buf, uint16_t len)
{
	memset(usart2_rxbuf, 0, sizeof(usart2_rxbuf));
	usart_rxcounter = 0;
	HAL_UART_Transmit(&huart2, (uint8_t*)buf, len, 0xFFFF);
}

void esp8266_sendString(char *str)
{
	memset(usart2_rxbuf, 0, sizeof(usart2_rxbuf));
	usart_rxcounter = 0;
	while(*str)
	{
		esp8266_sendByte(*str++);
	}
}

void esp8266_exitUnvarnishedTrans(void)
{
	esp8266_sendString("+++");
	HAL_Delay(50);
	esp8266_sendString("+++");
	HAL_Delay(50);
	printf("esp8266 exit unvarnished mode successfully.\r\n");
}

uint8_t esp8266_findStr(char *dest, char* src, uint16_t timeout)
{
	timeout /= 10;

	while(strstr(dest, src) == 0 && timeout--)
	{
		HAL_Delay(10);
	}

	if(timeout)
	{
		return 1;
	}

	return 0;
}

uint8_t esp8266_check(void)
{
	uint8_t check_cnt = 5;

	while(check_cnt--)
	{
		memset(usart2_rxbuf, 0, sizeof(usart2_rxbuf));
		esp8266_sendString("AT\r\n");
		if(esp8266_findStr((char*)usart2_rxbuf, "OK", 200) != 0)
		{
			return 1;
		}
	}

	return 0;
}

uint8_t esp8266_init(void)
{
	memset(usart2_txbuf, 0, sizeof(usart2_txbuf));
	memset(usart2_rxbuf, 0, sizeof(usart2_rxbuf));

	esp8266_exitUnvarnishedTrans();
	HAL_Delay(500);
	esp8266_sendString("AT+RST\r\n");
	HAL_Delay(800);
	if(esp8266_check() == 0)
	{
		return 0;
	}

	memset(usart2_rxbuf, 0, sizeof(usart2_rxbuf));
	esp8266_sendString("ATE0\r\n");
	if(esp8266_findStr((char*)usart2_rxbuf, "OK", 500) == 0)
	{
		printf("esp8266 init successfully.\r\n");
		return 0;
	}

	printf("esp8266 init failed.\r\n");
	return 1;
}

void esp8266_restore(void)
{
	esp8266_exitUnvarnishedTrans();
	HAL_Delay(500);
	esp8266_sendString("AT+RESTORE\r\n");
}

uint8_t esp8266_connectAP(char* ssid, char* passwd)
{
	uint8_t cnt = 5;
	while(cnt--)
	{
		memset(usart2_rxbuf, 0, sizeof(usart2_rxbuf));
		esp8266_sendString("AT+CWMODE_CUR=1\r\n");
		if(esp8266_findStr((char*)usart2_rxbuf, "OK", 200) != 0)
		{
			break;
		}
	}
	if(cnt == 0)
	{
		return 0;
	}

	cnt = 2;
	while(cnt--)
	{
		memset(usart2_txbuf, 0, sizeof(usart2_txbuf));
		memset(usart2_rxbuf, 0, sizeof(usart2_rxbuf));
		sprintf((char*)usart2_txbuf, "AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", ssid, passwd);
		esp8266_sendString((char*)usart2_txbuf);
		if(esp8266_findStr((char*)usart2_rxbuf, "OK", 8000) != 0)
		{
			printf("esp8266 connectAP %s successfully.\r\n", WIFI_SSID);
			return 1;
		}
	}

	return 0;
}

static uint8_t esp8266_openTransmission(void)
{
	uint8_t cnt = 2;
	while(cnt--)
	{
		memset(usart2_rxbuf, 0 ,sizeof(usart2_rxbuf));
		esp8266_sendString("AT+CIPMODE=1\r\n");
		if(esp8266_findStr((char*)usart2_rxbuf, "OK", 200) != 0)
		{
			printf("esp8266 openTransmission successfully.\r\n");
			return 1;
		}
	}

	printf("esp8266 openTransmission failed.\r\n");
	return 0;
}

uint8_t esp8266_connectServer(char* mode, char* ip, uint16_t port)
{
	uint8_t cnt;

	esp8266_exitUnvarnishedTrans();
	HAL_Delay(500);

	cnt = 2;
	while(cnt--)
	{
		memset(usart2_txbuf, 0, sizeof(usart2_txbuf));
		memset(usart2_rxbuf, 0, sizeof(usart2_rxbuf));
		sprintf((char*)usart2_txbuf, "AT+CIPSTART=\"%s\",\"%s\",%d\r\n", mode, ip, port);
		esp8266_sendString((char*)usart2_txbuf);
		if(esp8266_findStr((char*)usart2_rxbuf, "CONNECT", 8000) != 0)
		{
			break;
		}
	}

	if(cnt == 0)
	{
		return 0;
	}

	if(esp8266_openTransmission() == 0)
	{
		return 0;
	}

	cnt = 2;

	while(cnt--)
	{
		memset(usart2_rxbuf, 0, sizeof(usart2_rxbuf));
		esp8266_sendString("AT+CIPSEND\r\n");
		if(esp8266_findStr((char*)usart2_rxbuf, ">", 200) != 0)
		{
			printf("esp8266 connect server successfully.\r\n");
			return 1;
		}
	}

	printf("esp8266 connect server failed.\r\n");
	return 0;
}

uint8_t esp8266_disconnectServer(void)
{
	uint8_t cnt;

	esp8266_exitUnvarnishedTrans();
	HAL_Delay(500);

	while(cnt--)
	{
		memset(usart2_rxbuf, 0, sizeof(usart2_rxbuf));
		esp8266_sendString("AT+CIPCLOSE\r\n");

		if(esp8266_findStr((char*)usart2_rxbuf, "CLOSED", 200) != 0)
		{
			break;
		}
	}

	if(cnt)
	{
		return 1;
	}

	return 0;
}
