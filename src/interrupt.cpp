#include "interrupt.h"
driverUart _uart;

/**
 * su dung hang doi queue cho trinh xu ly ngat 
*/
static QueueHandle_t uart2_queue;

/**
 * funcion nay dung de hien thi tin nhan nhan duoc thong qua ngat uart2 
*/

void driverUart::recdata(String message)
{
    Serial.print("UART2 Receive from stm32:");
    Serial.println(message);
}
/**
 * function nay chay song song voi function chinh de dam bao moi thu van duoc hoat dong khi ngat 
 * duoc thuc thi
*/
volatile int cnt=0;
void driverUart::functionOne()
{
    cnt+=1;
    Serial.printf("cnt:= %d\n",cnt);
}

/**
 * function ngat 
 * 
*/
static void UART_ISR_ROUTINE(void *pvParameters)
{
    /**
     * khoi tao event de su dung struct uart_event_t
     * bien buffered_size dung de luu kich thuoc cua goi tin 
     * 
     * 
    */

    // typedef struct {
    //     uart_event_type_t type; /*!< UART event type */
    //     size_t size;            /*!< UART data size for UART_DATA event*/
    //     bool timeout_flag;      /*!< UART data read timeout flag for UART_DATA event (no new data received during configured RX TOUT)*/
    //                             /*!< If the event is caused by FIFO-full interrupt, then there will be no event with the timeout flag before the next byte coming.*/
    // } uart_event_t;



    uart_event_t event;
    size_t buffered_size;

    /**
     * dieu kien de thoat ra gap khi thuc hien ngat 
    */

    bool exit_condition = false;

    while(1)
    {

        if(xQueueReceive(uart2_queue, (void * )&event, (portTickType)portMAX_DELAY))
        {
            //Handle received event

            if (event.type == UART_DATA)
            {
                /**
                 * tao mot bien UART2_data 128 bytes de luu tru du lieu nhan duoc
                 * 
                */
                uint8_t UART2_data[128];
                int UART2_data_length = 0;
                /**
                 * esp_err_t uart_get_buffered_data_len(uart_port_t uart_num, size_t *size)
                */
                ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_NUM_2, (size_t*)&UART2_data_length));
                /**
                 * int uart_read_bytes(uart_port_t uart_num, void *buf, uint32_t length, TickType_t ticks_to_wait)
                */
                UART2_data_length = uart_read_bytes(UART_NUM_2, UART2_data, UART2_data_length, 100);

                /**
                 * tao mot bien String de luu giu du lieu nhan duoc 
                */
                String daaa="";
                /**
                 * vong lap for lap tu 0->chieu dai kich thuoc goi tin duoc dc
                 * dung ep kieu char cho tung du lieu nam trong UART2_data va cat no vao bien daaa
                 * sau khi xong thi in du lieu ra man hinh 
                */
                for(byte i=0; i<UART2_data_length;i++)
                {
                    daaa+=char(UART2_data[i]);
                }
                _uart.recdata(daaa);

            }
            /**
             * truong hop nay la danh cho viec cac goi tin khong duoc nhan duoc 
             * hoac qua trinh truyen tai thong tin bi loi
             * 
            */
            //Handle frame error event
            else if (event.type == UART_FRAME_ERR) {
                //TODO...
            }
            //Final else statement to act as a default case
            /**
             * 
            */
            else {
                //TODO...
            }    


        }

        /**
         * can co 1 dieu kien de thoat truong trinh gap thi xu dung dieu kien exit_condition
         * 
        */
        
        if (exit_condition) {
            break;
        }

        
    }
    //Out side of loop now. Task needs to clean up and self terminate before returning
    /**
     * truoc khi dong tac vu ta can delete tat ca 
    */
    vTaskDelete(NULL);
}

/**
 * function dung de truyen du lieu thong qua tx cua uart 
 * 
*/

void driverUart::send_data(char *test_str)
{
    /**
     * int uart_write_bytes(uart_port_t uart_num, const void *src, size_t size)
    */

    uart_write_bytes(UART_NUM_2, (const char*)test_str, strlen(test_str));
    /**
     * esp_err_t uart_wait_tx_done(uart_port_t uart_num, TickType_t ticks_to_wait)
    */
    ESP_ERROR_CHECK(uart_wait_tx_done(UART_NUM_2, 100)); 
    Serial.println("send done");

}

/**
 * chuong trinh khoi tao driver uart 
*/
void driverUart::init_driver_uart()
{
    /**
     * thiet lap monitor 
    */
    Serial.begin(115200);

    /**
     * cau hinh uart2
     * bao gom toc do baud 
     * so bit truyen
     * bit chan le
     * bit dung
     * su dung can thiep phan cung trong uart
     * 
    */
    uart_config_t Configurazione_UART2 = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };


    /***
     * esp_err_t uart_param_config(uart_port_t uart_num, const uart_config_t *uart_config)
    */
    uart_param_config(NUMERO_PORTA_SERIALE, &Configurazione_UART2);
    /**
     * esp_err_t uart_set_pin(uart_port_t uart_num, int tx_io_num, int rx_io_num, int rts_io_num, int cts_io_num)
    */
    uart_set_pin(NUMERO_PORTA_SERIALE, U2TXD, U2RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    /**
     * esp_err_t uart_driver_install(uart_port_t uart_num, int rx_buffer_size, int tx_buffer_size,
     *  int queue_size, QueueHandle_t *uart_queue, int intr_alloc_flags)
     * 
    */
    uart_driver_install(NUMERO_PORTA_SERIALE, BUF_SIZE, BUF_SIZE, 20, &uart2_queue, 0);
    //Create a task to handler UART event from ISR
    /**
     * extern "C" static inline BaseType_t xTaskCreate(TaskFunction_t pvTaskCode, 
     * const char *pcName, uint32_t usStackDepth, void *pvParameters, UBaseType_t uxPriority, TaskHandle_t *pxCreatedTask)
    */
    xTaskCreate(UART_ISR_ROUTINE, "UART_ISR_ROUTINE", 2048, NULL, 12, NULL);
    //xTaskCreate(functionOne,"function one",512*2,NULL,11,NULL);
}
