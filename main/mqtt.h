
/*
Archivos de cabecera para aplicaciones modbus
*/

#include "mqtt_client.h"
#include "esp_log.h"


static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) ;

static void mqtt_app_start(void);

