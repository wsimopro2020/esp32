/*
DEFINICIONES PARA CONFIGURAR UART

*/
#include<stdio.h>
#include <driver/uart.h>





void configurarUart(uint8_t uart, uint32_t baud,uint8_t bitstop, uint8_t paridad, uint8_t rx , uint8_t tx ) ; 
void getStringUart2( int size);
/*
Funciones para enviar datos
*/
