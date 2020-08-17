/*
   LIBRERIA PARA MODBUS RTU EN ESP32_
   AUTOR: GERMAN GABRIEL VELARDEZ
   FECHA DE INICIO DEL PROYECTO: 9/8/2020
  
*/


#include "modbus_rtu.h"


/*
Configurar parametros de la peticion
*/






/*
  Asignar la cantidad de registros a leer
*/
void set_num_reg( struct request *req,uint16_t num)
{
    req->h_size = (uint8_t)num >>8;
    req->l_size = (uint8_t)num;
}
/*
  Asignar la direccion de  start
*/
void set_address( struct request *req, uint16_t address)
{
      req->h_address = (uint8_t)address >>8;
      req->l_address = (uint8_t)address;
}
/*
 Calcular el CRC     (Podria usarse tabla ,verificar ques es mas eficiete)
*/
void CRC16_2(uint8_t *buf, uint8_t len,uint8_t write)
{  
  uint16_t crc = 0xFFFF;
  for (int pos = 0; pos < len; pos++)
  {
  crc ^= (uint16_t)buf[pos];    // XOR byte into least sig. byte of crc
  for (int i = 8; i != 0; i--) {    // Loop over each bit
    if ((crc & 0x0001) != 0) {      // If the LSB is set
      crc >>= 1;                    // Shift right and XOR 0xA001
      crc ^= 0xA001;
    }
    else                            // Else LSB is not set
      crc >>= 1;                    // Just shift right
    }
  }
 if(write)
 {

 }

  buf[len]=(uint8_t)crc;
  buf[len+1]=(uint8_t)(crc >> 8);
    
  
}

//Leer trama de respuesta




struct modbus_master modbus_master_start( uint8_t port,uint16_t baudios)
{
     struct modbus_master master;
     master.port.port =port;
     master.port.baudios=baudios;
     configurarUart(port,baudios,1, 0, 18 , 19 );
     return master;

}



void set_request_read(struct request *req,uint8_t id,uint8_t func,uint16_t address,uint16_t size)
{
      req->id = id;
      req->func = func;
      set_address(req,address);
      set_num_reg(req,size);
      CRC16_2((uint8_t*)req,6,0);   //0 read  y 1 para write

}


void send_request(struct modbus_master* master, struct modbus_response* res)
{
   uart_write_bytes(master->port.port,(const char*)&(master->req), 8);
   if(res != NULL)
   {
    get_responds_modbus(res,&(master->req));   //Se imprime la repsuesta y se guarda en una estructura mosbus_response
   }
   else
   {
     get_responds( master->port.requested_records);  //Se imprime las respuesta pero no se guarda
   }
   
}



void get_responds_modbus(struct modbus_response* res,struct request* req)
{
 
  res->id=req->id;
  res->func=req->func;
  res->size=req->l_size;
  uint8_t rsize =(req->l_size)*2+5; // por el momento funcionara, hasta q intente pedir mas de 255 registros a la vez
  
  uint8_t* buffer = malloc(rsize);
   if(buffer != NULL)
  {
      uart_read_bytes(UART_NUM_2,buffer,rsize,100);
     
  }
  else
  {
    printf("problemas en memoria \n");
  }

  load_registers(buffer,res);
  free(buffer);
  uart_flush(UART_NUM_2);
}


void load_registers(uint8_t* buffer,struct modbus_response*res)
{
        free(res->registers);
        res->registers=malloc(buffer[2]);
   //  printf("cantidad de registros a leer: %d\n",buffer[2]/2);
    for(uint8_t i=3,j=0; j<((buffer[2]/2));i=i+2, j++)
    {
        res->registers[j]=((uint16_t)buffer[i]<<8 |(uint16_t) buffer[i+1]);
   //    printf("el registro):%d\n",res->registers[j]);

    }
         
} 


void get_responds(uint8_t size)
{
   uint8_t rsize =(size*2)+5; 
   uint8_t* buffer=malloc(rsize);  //4 fijos son por id/func/  crc  /
  if(buffer != NULL)
  {   
     uart_read_bytes(UART_NUM_2,buffer,rsize,100);
//      printf("se recibio trama ok\nNumero de elementos: %d\n",rsize);
 //     printf("id slave %d\n",buffer[0]);
 //     printf("funcion:%d\n",buffer[1]);
 //     printf("cantidad de registros:%d\n",buffer[2]);
      for(int i=3; i<rsize-2; i++)
      {
  //        printf("%d) :%d  \n",i,buffer[i]);
      }
  }
  else
  {
    printf("problemas en memoria \n");
  }
  free(buffer);
  uart_flush(UART_NUM_2);

}





void set_request(struct modbus_master*  master,uint8_t id,uint8_t func,uint16_t address,uint16_t size)
{
  
    
     master->req.id = id;                      //master tiene un miembro que es puntero a request
    
     master-> req.func = func;
     
     master->port.requested_records=size;
     
     set_address(&(master->req),address);
     set_num_reg(&(master->req),size);
   
     CRC16_2((uint8_t*)&(master->req),6,0);   //0 read  y 1 para wrie

}
