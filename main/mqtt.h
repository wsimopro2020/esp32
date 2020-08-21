
/*
Archivos de cabecera para aplicaciones modbus
*/

#include "mqtt_client.h"
#include "esp_log.h"


struct comando
{
   uint8_t func;
   uint8_t reg;
   uint8_t adddress;
   
};



 struct mqtt_buffer_in
 {
    char*    buffer;
    uint8_t   buffer_len;
    char*     topic;
    uint8_t topic_len;

 };




static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) ;

static void mqtt_app_start(void);

