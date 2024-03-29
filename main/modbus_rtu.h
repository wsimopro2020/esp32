#include "modbus_uart.c"

struct request
{
   uint8_t id;           //ID SLAVE
   uint8_t func;        //funcion modbus
   uint8_t h_address;
   uint8_t l_address;
   uint8_t h_size;
   uint8_t l_size;
   uint8_t byte_count;
   uint8_t* register_to_send ;
   uint16_t crc;         //CRC
   uint8_t state; 
} ;

struct buffer_out
{
   uint8_t* out;
   uint8_t size;
};

struct buffer_reg_to_write
{
   uint8_t* buffer_out;
   uint8_t size;
};

struct modbus_response
{
   uint8_t id;
   uint8_t func;
   uint8_t size;
   uint16_t* registers;
   uint8_t h_crc;
   uint8_t l_crc;
   uint8_t state_response ;

};
struct portconfig
{
   uint8_t port;
   uint16_t baudios;
   uint8_t rx;
   uint8_t requested_records;
};

struct modbus_master
{
  uint8_t state;
  struct request req;
  struct buffer_reg_to_write* reg;
  struct portconfig port;
};

//Funciones



struct modbus_master modbus_master_start( uint8_t port,uint16_t baudios);


//Funciones para implementarmodbus
void set_request_read(struct request *req,uint8_t id,uint8_t func,uint16_t address,uint16_t size);
void set_request(struct modbus_master*modbus,uint8_t id,uint8_t func,uint16_t address,uint16_t size);

struct buffer_out serialize_request(struct request* req);
//utilidades internas
void set_num_reg( struct request *req,uint16_t num);
void set_address( struct request *req, uint16_t address);
void set_request_write(struct modbus_master*  master,uint8_t id,uint8_t func,uint16_t address,uint16_t size,u_int16_t* values);

void CRC16_2(uint8_t *buf, uint8_t len,uint8_t write);


void get_responds(uint8_t size);


void get_responds_modbus(struct modbus_response* res,struct request* req);

void send_request(struct modbus_master* master, struct modbus_response* res);
void load_registers(uint8_t* buffer,struct modbus_response*res);



void set_request(struct modbus_master*  master,uint8_t id,uint8_t func,uint16_t address,uint16_t size);
void set_req(struct modbus_master*modbus,uint8_t id,uint8_t func,uint16_t address,uint16_t size,u_int16_t* datos_enviar);
