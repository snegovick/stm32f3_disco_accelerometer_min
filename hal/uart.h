#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>

void uart_init(void);

void uart_send_byte(uint16_t b);

#endif/*__UART_H__*/
