#pragma once

#include "main.h"

#define UART_RX_BUFFER_SIZE 256
#define UART_TX_BUFFER_SIZE 256
#define PHYSICAL_UARTS_COUNT 5

typedef struct _Uart {
  UART_HandleTypeDef* device;
  volatile uint32_t rx_buffer_index;
  volatile uint32_t tx_queue_index;
  uint8_t rx_buffer[UART_RX_BUFFER_SIZE];
  uint8_t tx_buffer[UART_TX_BUFFER_SIZE];
  uint8_t tx_queue[UART_TX_BUFFER_SIZE];
  osThreadId rx_thread;
  osThreadId tx_thread;
  volatile uint8_t received_byte;
  void (*on_receive)(struct _Uart* uart, char*, uint32_t);
} Uart;

Uart* new_uart(UART_HandleTypeDef* uart_device,
               void (*on_receive)(char*, uint32_t));
void delete_uart(Uart* uart);
void uart_start_receive(Uart* uart);
void uart_send(Uart* uart, char* message);
void uart_sendf(Uart* uart, const char* format, ...);
void uart_sendln(Uart* uart, char* message);