#ifndef __TIMTRACE__
#define __TIMTRACE__


#define EX_UART_NUM UART_NUM_1
#define PATTERN_CHR_NUM (3) /*!< Set the number of consecutive and identical characters received by receiver which defines a UART pattern*/

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)

#define KEY_EVENT 0
#define ENCODER_EVENT 1
#define BUTTON_EVENT 2

void timtrace_init();
void read_from_timtrace(const char *dtmp);
void uart_event_task(void *pvParameters);


#endif
