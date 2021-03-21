#include "stm32f1xx_hal.h"

#define	WIFI_SSID	"DESKTOP-O806I6G 3151"
#define WIFI_PASSWD	"h1+2Q041"

extern UART_HandleTypeDef huart2;	//ESP_UART

void esp8266_sendBuf(uint8_t* buf, uint16_t len);
void esp8266_sendString(char *str);
void esp8266_exitUnvarnishedTrans(void);
uint8_t esp8266_findStr(char *dest, char* src, uint16_t timeout);
uint8_t esp8266_check(void);
uint8_t esp8266_init(void);
void esp8266_restore(void);
uint8_t esp8266_connectAP(char* ssid, char* passwd);
uint8_t esp8266_connectServer(char* mode, char* ip, uint16_t port);
uint8_t esp8266_disconnectServer(void);
