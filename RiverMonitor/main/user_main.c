/*
 * user_main.c
 *
 *  Created on: 2021年4月4日
 *      Author: 10094
 */

#include <stdio.h>
#include "freertos/include/freertos/FreeRTOS.h"
#include "freertos/include/freertos/task.h"
#include "freertos/include/freertos/event_groups.h"

#include "esp8266/include/esp_wifi.h"
#include "esp_event_loop.h"
#include "mqtt/esp-mqtt/include/mqtt_client.h"

#define	WIFI_SSID			"XDT-2.4G"
#define	WIFI_PASS			"1234567890asd"
#define	MQTT_PRODUCT_KEY	"a1LvrPQRzLl"
#define	MQTT_DEVICE_NAME	"RiverMonitor"
#define	MQTT_DEVICE_SECRET	"2603ca55584beee26e15a95113dbc32b"

static char mqtt_clientId[150];
static char mqtt_username[65];
static char mqtt_password[65];
static esp_mqtt_client_handle_t mqtt_client;
static EventGroupHandle_t wifi_event_group;
const static int CONNECTED_BIT = BIT0;

extern int aiotMqttSign(const char *productKey, const char *deviceName, const char *deviceSecret,
                     char clientId[150], char username[64], char password[65]);
static void user_init();
static esp_err_t wifi_event_handler(void* ctx, system_event_t* event);
static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);
static void mqtt_app_start(void);

void app_main()
{
	printf("Hello world\r\n");
	user_init();
}

static void user_init()
{
	//tcp-ip stack init
	tcpip_adapter_init();

	//register wifi events
	esp_event_loop_init(wifi_event_handler, NULL);

	//wifi init
	wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
	wifi_config_t wifi_cfg = {
			.sta = {
					.ssid = WIFI_SSID,
					.password = WIFI_PASS,
			},
	};
	wifi_event_group = xEventGroupCreate();
	esp_wifi_init(&wifi_init_cfg);
	esp_wifi_set_mode(WIFI_MODE_STA);
	esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_cfg);
	esp_wifi_start();
	printf("waiting for wifi\r\n");
	xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);

	//mqtt init
	aiotMqttSign(MQTT_PRODUCT_KEY, MQTT_DEVICE_NAME, MQTT_DEVICE_SECRET, mqtt_clientId, mqtt_username, mqtt_password);
	mqtt_app_start();

}

static esp_err_t wifi_event_handler(void* ctx, system_event_t* event)
{
	switch(event->event_id)
	{
		case SYSTEM_EVENT_STA_START:
		{
			esp_wifi_connect();
			printf("WIFI connected\r\n");
			break;
		}
		case SYSTEM_EVENT_STA_DISCONNECTED:
		{
			esp_wifi_connect();
			printf("WIFI auto-reconnect\r\n");
			xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
			break;
		}
		case SYSTEM_EVENT_STA_GOT_IP:
		{
			printf("Connected with IP Address:%s\r\n", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
			xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
			break;
		}
		default:
		{
			break;
		}
	}

	return ESP_OK;
}

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
	esp_mqtt_client_handle_t client = event->client;
	int msg_id = 0;
	// your_context_t *context = event->context;

	switch(event->event_id)
	{
		case MQTT_EVENT_CONNECTED:
		{
			printf("MQTT EVENT CONNECTED\r\n");
			msg_id = esp_mqtt_client_subscribe(client, "/sys/a1LvrPQRzLl/RiverMonitor/thing/event/property/post_reply", 0);
			printf("send subscription successfully, msg_id = %d\r\n", msg_id);
			break;
		}
		case MQTT_EVENT_DISCONNECTED:
		{
			printf("MQTT EVENT DISCONNECTED, msg_id = %d\r\n", msg_id);
			break;
		}
		case MQTT_EVENT_SUBSCRIBED:
		{
			printf("MQTT EVENT SUBSCRIBED, msg_id = %d\r\n", msg_id);
			msg_id = esp_mqtt_client_publish(client, "/sys/a1LvrPQRzLl/RiverMonitor/thing/event/property/post", "data", 0, 0, 0);
			break;
		}
		case MQTT_EVENT_UNSUBSCRIBED:
		{
			printf("MQTT EVENT UNSUBSCRIBED, msg_id = %d\r\n", msg_id);
			break;
		}
		case MQTT_EVENT_PUBLISHED:
		{
			printf("MQTT EVENT PUBLISHED, msg_id = %d\r\n", msg_id);
			break;
		}
		case MQTT_EVENT_DATA:
		{
			printf("MQTT EVENT DATA\r\n");
			printf("TOPIC = %.*s\r\n", event->topic_len, event->topic);
			printf("DATA = %.*s\r\n", event->data_len, event->data);
			break;
		}
		case MQTT_EVENT_ERROR:
		{
			printf("MQTT EVENT ERROR\r\n");
			break;
		}
		default:
		{
			break;
		}
	}

	return ESP_OK;
}

static void mqtt_app_start(void)
{
	const esp_mqtt_client_config_t mqtt_cfg = {
			.host = "a1LvrPQRzLl.iot-as-mqtt.cn-shanghai.aliyuncs.com",
			.client_id = mqtt_clientId,
			.username = mqtt_username,
			.password = mqtt_password,
			.event_handle = mqtt_event_handler,
	};
	printf("[Free Memory]: %d bytes\r\n", esp_get_free_heap_size());
	mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
	esp_mqtt_client_start(mqtt_client);
}
