#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "connectivity.uart.h"
#include "helper.h"
#include "main.h"

Uart* uart_pool[PHYSICAL_UARTS_COUNT] = {NULL};

uint8_t uart_device_get_number(UART_HandleTypeDef* uart_device) {
  if (uart_device->Instance == USART1)
    return 1;
  if (uart_device->Instance == USART2)
    return 2;
  if (uart_device->Instance == USART3)
    return 3;
  if (uart_device->Instance == UART4)
    return 4;
  if (uart_device->Instance == UART5)
    return 5;
  return 0;
}

Uart* new_uart(UART_HandleTypeDef* uart_device,
               void (*on_receive)(char*, uint32_t)) {
  uint8_t uart_number = uart_device_get_number(uart_device);
  if (uart_pool[uart_number] != NULL)
    return NULL;
  Uart* uart = (Uart*)malloc(sizeof(Uart));
  uart->device = uart_device;
  uart->rx_buffer_index = 0;
  uart->tx_queue_index = 0;
  uart->on_receive = on_receive;
  if (uart_number != 0)
    uart_pool[uart_number - 1] = uart;
  uart_start_receive(uart);
  return uart;
}

void delete_uart(Uart* uart) {
  uint8_t uart_number = uart_device_get_number(uart->device);
  if (uart_number != 0)
    uart_pool[uart_number - 1] = NULL;
  free(uart);
};

void uart_start_receive(Uart* uart) {
  HAL_UART_Receive_IT(uart->device, (uint8_t*)&uart->received_byte,
                      sizeof(uint8_t));
}

void uart_receive_completed_callback(Uart* uart) {
  if (uart->received_byte == '\r') {
    uart->rx_buffer[uart->rx_buffer_index] = '\0';
    if (uart->on_receive) {
      uart->on_receive(uart, (char*)uart->rx_buffer, uart->rx_buffer_index);
      uart_send(uart, "\r");
    }
    uart->rx_buffer_index = 0;
    HAL_UART_Receive_IT(uart->device, (uint8_t*)&uart->received_byte, 1);
    return;
  }
  uart->rx_buffer[uart->rx_buffer_index] = uart->received_byte;
  uart->rx_buffer_index++;
  uart->rx_buffer_index %= UART_RX_BUFFER_SIZE;
  HAL_UART_Receive_IT(uart->device, (uint8_t*)&uart->received_byte, 1);
}

void uart_transmit_completed_callback(Uart* uart) {
  if (uart->tx_queue_index == 0)
    return;
  memcpy(uart->tx_buffer, uart->tx_queue, uart->tx_queue_index);
  HAL_UART_Transmit_IT(uart->device, uart->tx_buffer, uart->tx_queue_index);
  uart->tx_queue_index = 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
  UART_HandleTypeDef* uart_device = huart;
  uint8_t uart_number = uart_device_get_number(uart_device);
  if (uart_number == 0)
    return;
  Uart* uart = uart_pool[uart_number - 1];
  if (uart == NULL)
    return;
  uart_receive_completed_callback(uart);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart) {
  UART_HandleTypeDef* uart_device = huart;
  uint8_t uart_number = uart_device_get_number(uart_device);
  if (uart_number == 0)
    return;
  Uart* uart = uart_pool[uart_number - 1];
  if (uart == NULL)
    return;
  uart_transmit_completed_callback(uart);
}

void uart_enqueue_message(Uart* uart, char* message, uint32_t message_length) {
  uint32_t queue_free_size = UART_TX_BUFFER_SIZE - uart->tx_queue_index;
  if (queue_free_size == 0)
    return;
  uint32_t buffer_reserve_size = MIN(queue_free_size, message_length);
  memcpy(&uart->tx_queue[uart->tx_queue_index], message, buffer_reserve_size);
  uart->tx_queue_index += buffer_reserve_size;
}

void uart_send(Uart* uart, char* message) {
  __HAL_UART_DISABLE_IT(uart->device, UART_IT_TXE);
  uint32_t message_length = strlen(message);
  HAL_UART_StateTypeDef state = HAL_UART_GetState(uart->device);
  if (state != HAL_UART_STATE_BUSY_TX && state != HAL_UART_STATE_ERROR &&
      state != HAL_UART_STATE_BUSY && state != HAL_UART_STATE_BUSY_TX_RX) {
    uint32_t tx_size = MIN(message_length, UART_TX_BUFFER_SIZE);
    memcpy(uart->tx_buffer, message, tx_size);
    HAL_UART_Transmit_IT(uart->device, uart->tx_buffer, tx_size);
  } else {
    uart_enqueue_message(uart, message, message_length);
  }
  __HAL_UART_ENABLE_IT(uart->device, UART_IT_TXE);
}

void uart_sendln(Uart* uart, char* message) {
  uart_sendf(uart, "%s\n", message);
}

void uart_sendf(Uart* uart, const char* format, ...) {
  va_list args;
  va_start(args, format);
  uint32_t message_length = vsnprintf(NULL, 0, format, args);
  char message[message_length + 1];
  vsnprintf(message, message_length + 1, format, args);
  uart_send(uart, message);
  va_end(args);
}