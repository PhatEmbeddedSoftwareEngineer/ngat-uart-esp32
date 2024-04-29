#include <Arduino.h>
#include "driver/uart.h"

/**
 * su dung uart 2
 * buf_size 1024*2
 * buf_size_nhan 1024
 * rxd 16
 * txd 17
*/

#define NUMERO_PORTA_SERIALE UART_NUM_2
#define BUF_SIZE (1024 * 2)
#define RD_BUF_SIZE (1024)
#define U2RXD 16
#define U2TXD 17

/**
 * su dung bien volatile cnt de chay mot tac vu song song
*/

extern volatile int cnt;


class driverUart
{
private:
    /**
     * rxbuf co 256 bytes
    */
    uint8_t rxbuf[256];     //Buffer di ricezione
    /**
     * rx_fifo_len 2 bytes 
    */
    uint16_t rx_fifo_len;        //Lunghezza dati
    
public:
    /**
     * chuong trinh khoi tao driver_uart co su dung ngat
    */
    void init_driver_uart();
    /**
     * chuong trinh nhan du lieu va hien thi ra man hinh 
    */
    void recdata(String message);
    /**
     * chuong trinh truyen data bang txd
    */
    void send_data(char *test_str);
    /**
     * funcion chay dong thoi voi function ngat 
     * 
    */
    void functionOne();
};

extern driverUart _uart;
