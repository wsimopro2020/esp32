/*
Libreria para manejar las conexiones wifi de manera sencilla
*/





#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"


#define EXAMPLE_ESP_WIFI_SSID      "Velardez"
#define EXAMPLE_ESP_WIFI_PASS      "156983664"
#define EXAMPLE_ESP_MAXIMUM_RETRY  5

#define WIFI_CONNECTED_BIT 1
#define WIFI_FAIL_BIT      0






static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);
void wifi_init_sta(  char SSID[],  char PASSWORD[]);