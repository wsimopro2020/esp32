/*
Api para manejar el puerto serie del esp32
Autor German Velardez
*/

#include "modbus_uart.h"






/*

Funciones de configuracion rapida
*/



void configurarUart(uint8_t uart, uint32_t baud,uint8_t bitstop, uint8_t paridad, uint8_t rx , uint8_t tx )    //18 y 19
{

    uart_config_t uart_config =
  {
    .baud_rate =baud,
    .data_bits = UART_DATA_8_BITS,
    .parity= paridad,
    .stop_bits =bitstop,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,    //UART_HW_FLOWCTRL_DISABLE    UART_HW_FLOWCTRL_CTS_RTS
    
  };

ESP_ERROR_CHECK(uart_param_config(uart,&uart_config));
ESP_ERROR_CHECK(uart_set_pin(uart,rx,tx,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE));   //18 Y 19     , 17 Y 18   RX Y TX
ESP_ERROR_CHECK(uart_driver_install(uart,(const int)2048,(const int)2048,1000,NULL,0));
}



void getStringUart2( int size)
{
    uint8_t   buffer[size];
     int buf =0;
    buf= uart_read_bytes(UART_NUM_2,buffer,size,100);   //Se puede borrar buffer con uart_flush(UART_NUM_2);
    if(buf != -1)
    {
        printf("Se recibios %d bytes por puerto \n",buf);
         printf("MENSAJE: %s\n",buffer);
        
    }
    else
    {
        printf("No se recibio ningun byte");
        
    }
    uart_flush(UART_NUM_2);
    

}
