/*
   LIBRERIA PARA MODBUS RTU EN ESP32_
   AUTOR: GERMAN GABRIEL VELARDEZ
   FECHA DE INICIO DEL PROYECTO: 9/8/2020
  
*/


#include "modbus_rtu.h"


/*
Configurar parametros de la peticion
*/



void set_req(struct modbus_master*modbus,uint8_t id,uint8_t func,uint16_t address,uint16_t size,u_int16_t* datos_enviar)
{
  if(func ==3)
  {
    set_request(modbus,id,func,address,size);
  }else
  {
   set_request_write(modbus,id,func,address,size,datos_enviar);
  }
}







void set_request(struct modbus_master*  master,uint8_t id,uint8_t func,uint16_t address,uint16_t size)
{
    master->req.state=0;
     master->req.id = id;                      //master tiene un miembro que es puntero a request   
     master-> req.func = func;
     master->port.requested_records=size;
     set_address(&(master->req),address);
     set_num_reg(&(master->req),size);




}

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
    req->state=0;
      req->id = id;
      req->func = func;
      set_address(req,address);
      set_num_reg(req,size);
      CRC16_2((uint8_t*)req,6,0);   //0 read  y 1 para write

}

//Write multiple register funcion: 16(0x10)
// /funcion/start address/ quantity register/byte count/ value.../
void set_request_write(struct modbus_master*  master,uint8_t id,uint8_t func,uint16_t address,uint16_t size,u_int16_t* values)
{
    master->state=0;
     master->req.id = id;                      //master tiene un miembro que es puntero a request   
     master-> req.func = func;
     master->port.requested_records=size;
     set_address(&(master->req),address);
     set_num_reg(&(master->req),size);
     master->req.byte_count = 2*((uint8_t)size);
     master->req.register_to_send =(uint16_t*)values;  
}


void get_responds_modbus(struct modbus_response* res,struct request* req)
{
  res->state_response=0;   //Se inicio espera de respuesta con o= espera 1 recibido
  res->id=req->id;
  res->func=req->func;
  res->size=req->l_size;
  uint8_t rsize =(req->l_size)*2+5; // por el momento funcionara, hasta q intente pedir mas de 255 registros a la vez
  
  uint8_t* buffer = malloc(rsize);
   if(buffer != NULL)
  {
      uart_read_bytes(UART_NUM_2,buffer,rsize,20);
      load_registers(buffer,res);
      free(buffer);
      uart_flush(UART_NUM_2);
  }
  else
  {
    printf("problemas en memoria al leer respuesta \n");  
  }

 
  
}


void load_registers(uint8_t* buffer,struct modbus_response*res)
{
        free(res->registers);
        if(buffer[1] == 3 && buffer[0]==1)            //Si en el bafer esta una respuesta modbus coherente
        {
              res->registers=malloc(buffer[2]);
  
           for(uint8_t i=3,j=0; j<((buffer[2]/2));i=i+2, j++)
          {
           res->registers[j]=((uint16_t)buffer[i]<<8 |(uint16_t) buffer[i+1]); 
          }
          res->state_response=1;
        }
      else
      {
        printf("(load registers)Sin conexion modbus\n");
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

  }
  else
  {
    printf("problemas en memoria \n");
  }
  free(buffer);
  uart_flush(UART_NUM_2);

}







void send_request(struct modbus_master* master, struct modbus_response* res)
{
  
 struct buffer_out o=serialize_request(&(master->req));
   uart_write_bytes(master->port.port,(const char*)o.out, o.size); //Envias la peticion, size aumenta en 2 por el CRC
   if(master->req.func == 3)
   {
      get_responds_modbus(res,&(master->req));   //Se imprime la repsuesta y se guarda en una estructura mosbus_response
   }
   else
   {
     printf("enviaste comandos de escritura \n");
   
  

      uart_flush(UART_NUM_2);
   }
   
   
}



struct buffer_out serialize_request(struct request* req)
{
  struct buffer_out o ={};
  o.size=6;
  
  uint16_t size=((uint16_t)req->h_size<<8 |(uint16_t) req->l_size);
  
  if (req->func ==0x10)
  {
     o.size=  6+ 2*size;          //Funcion 16 escribir multiples registros
  }     
   o.out = malloc( o.size);   // inicializamos el buffer para recibir la req serializada y enviarla
   if(o.out !=NULL)
   {
      //Obtuvimos memoria, prosigamos
      o.out[0]=req->id;
      o.out[1]=req->func;
      o.out[2]=req->h_address;
      o.out[3]=req->l_address;
      o.out[4]=req->h_size;
      o.out[5]=req->l_size;
      if(req->func == 0x10)
      {
        o.out[6]=req->byte_count;

        uint8_t* reg=(uint8_t*)req->register_to_send;

        for(uint8_t i=7, j=0; j<(2*size); i=i+2, j=j+2)
        {
         
           o.out[i] = *(req->register_to_send+(j+1));
           o.out[i+1]=  *(req->register_to_send+j);
          
        }
        CRC16_2(o.out,((2*size)+7),0);
        o.size=o.size+3;
      }
      else
      {
         CRC16_2(o.out,6,0);
         o.size=o.size+2;
      }
       
   }
   else
   {
     printf("problemas al asignar memoria al buffer\n");
     o.size=0;
   }
   
  return o;
}

