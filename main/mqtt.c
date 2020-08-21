

#include "mqtt.h"

static const char *TAG2 = "MQTT_EXAMPLE";
 static esp_mqtt_client_handle_t client;


static xQueueHandle cola_Comandos;



static struct  mqtt_buffer_in client_buffer_in;
static char *expected_data = NULL;
static char *actual_data = NULL;
static size_t expected_size = 0;
static size_t expected_published = 0;
static size_t actual_published = 0;
static int qos_test = 0;




static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
   
    static int msg_id = 0;
    static int actual_len = 0;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG2, "MQTT_EVENT_CONNECTED");
            printf("SISTEMAS MQTT CONECTADO---------\n");
           

            msg_id = esp_mqtt_client_subscribe(client, "ESP32/COMANDOS", 0);
            ESP_LOGI(TAG2, "Te subcribiste a comandos, msg_id=%d", msg_id);
            break;



              case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG2, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG2, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG2, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG2, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
       
            
         
          
         if(event->data_len ==3)
         {
            
            
            printf("comando de un caracter: %d\n",(event->data[0]));
             struct comando comando ;
             
             comando.func=(event->data)[0];
             comando.reg=(event->data)[1];
             comando.adddress=(event->data)[2];
             
             xQueueSendToBack(cola_Comandos, &comando,500/portTICK_RATE_MS);
         }
         else
         {
             printf("comando de un caracter: %s\n",event->data);
         }

        break;
       
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG2, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG2, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG2, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

static void mqtt_app_start(void)
{
     cola_Comandos = xQueueCreate(1,sizeof(struct comando));   // para enviar los comandos
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri ="ws://vecserver.herokuapp.com/",    //https://vecserver.herokuapp.com/    ,"mqtt://192.168.0.38:1883"
    };
    printf("INTENTANDO CONECTARSE\n");
     client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

