#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "nvs_flash.h"

/*

Librerias propias
*/
#include "modbus_rtu.c"
#include "wifi.c"
#include "mqtt.c"


//Vamos a enviar arrays de 16 elementos uint16_t   en total tamaño 32bytes

#define TAM_COLA_REGISTROS 1
 




u_int16_t datos_enviar[8]={3,3,300,260,500,400,256,850};



void uart_task(void *pvParameter);
void print_request(void *pvParameter);
void conexion ( void *pvParameter);

 struct request rq;

 xQueueHandle cola_Modbus;
 

void app_main(void)
{
     //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

     ESP_LOGI("INICIANDO WIFI", "ESP_WIFI_MODE_STA");
    wifi_init_sta("Velardez","156983664");
     mqtt_app_start();
    
    //xTaskCreate(uart_task, "uart_task", 4096, NULL, 1, NULL)
    cola_Modbus = xQueueCreate(TAM_COLA_REGISTROS,sizeof(struct modbus_response));
 
  

    printf("Inicio tarea de uart_task\n");
 

    //xTaskCreatePinnedToCore(uart_task, "uart_task", 30000, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(print_request, "print_task", 30000, NULL, 4, NULL, 1);
   
   
   

    while(1)
    {
        
       

        vTaskDelay(700 / portTICK_PERIOD_MS);     
      //    datos_enviar[0]=datos_enviar[0]+1;
    }
}







void uart_task(void *pvParameter)
{ 
    printf("Inicio tarea de uart_task\n");
    struct modbus_master esp32_modbus =modbus_master_start(2,19200);     
    struct modbus_response res;
    
    res.state_response=0;
    res.registers=NULL;
    res.size=0;
    set_request(&esp32_modbus,1,16,0,4);
  // set_request_write(&esp32_modbus,1,0x10,0x04,0x02,&datos_enviar);
    printf("CONFIGURACION MODBUS EXITOSA\n");
   vTaskDelay(20 / portTICK_PERIOD_MS);
   while(1)
   {
        send_request(&esp32_modbus,&res);
       
      
        if(res.state_response == 1)
        {
           
              if(xQueueSendToBack(cola_Modbus, &res,100/portTICK_RATE_MS)!=pdTRUE)
            {       
                printf( "Envio de registros modbus desde tarea 1\n");
            }
        }
        else
        {
             int msg_id = esp_mqtt_client_publish(client, "ESP32/TITULO", "SIN CONEXION:n\ ",strlen("SIN CONEXION:n\ "), 0, 0);
            printf("Sin conexion modbus \n");
        }
       
        res.state_response=0;
        res.registers=NULL;
        res.size=0;
      
        vTaskDelay(1000 / portTICK_PERIOD_MS);      
   }
    
}

void print_request(void *pvParameter)
{      
    
  printf("Comenzando tarea print request\n");
 struct modbus_response res;
 //int msg_id = esp_mqtt_client_subscribe(client, "ESP32/COMANDOS", 0);
 
   while(1)
   {


        /*  
             if(xQueueReceive(cola_Modbus,&res,100/portTICK_RATE_MS)==pdTRUE) 
         {
            int msg_id = esp_mqtt_client_publish(client, "ESP32/TITULO", "REGISTROS:n\ ",strlen("REGISTROS:n\ "), 0, 0);
          for(uint8_t i=0; i<res.size ; i++)
           {
             int msg_id = esp_mqtt_client_publish(client, "ESP32/SALIDAS", res.registers,(res.size)*2, 0, 0);
             ESP_LOGI(TAG, "[%d] PUBLICANDO...", i);
           }
        
        }
       */
     
     vTaskDelay(1000 / portTICK_PERIOD_MS);   
}
    
}