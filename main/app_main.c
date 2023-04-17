#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

// Khai báo các GPIO
#define PIN_SWDIO 12
#define PIN_SWCLK 13


void sbw_init();
void sbw_deinit();
void sbw_reset();
void sbw_send_bit(uint8_t bit);
void sbw_send_data(uint8_t command, uint32_t data);

void app_main() {
    // Khởi tạo GPIO và cấu hình giao thức Spy-Bi-Wire
    sbw_init();

    // Gửi và nhận dữ liệu
    uint8_t tx_data = 0x01;
    sbw_send_data(0x20,  tx_data);
   
}

void sbw_init()
{
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 1,
        .pull_down_en = 0,
        .pin_bit_mask = (1ULL << PIN_SWCLK) | (1ULL << PIN_SWDIO)
    };
    gpio_config(&io_conf);

    // Đặt tín hiệu SWDIO và SWCLK ở trạng thái ban đầu
    gpio_set_level(PIN_SWCLK, 0);
    gpio_set_level(PIN_SWDIO, 1);
}

void sbw_deinit() {
  // Tắt tín hiệu SWDIO và SWCLK
  gpio_set_level(PIN_SWDIO, 0);
  gpio_set_level(PIN_SWCLK, 0);

  // Giải phóng GPIO SWDIO và SWCLK
  gpio_reset_pin(PIN_SWDIO);
  gpio_reset_pin(PIN_SWCLK);
}

void sbw_reset() {
  // Tắt tín hiệu SWDIO và SWCLK
  gpio_set_level(PIN_SWDIO, 0);
  gpio_set_level(PIN_SWCLK, 0);

  // Đặt tín hiệu SWDIO ở trạng thái 1 trong vòng 50 xung clock
  for (int i = 0; i < 50; i++) {
    gpio_set_level(PIN_SWCLK, 1);
    gpio_set_level(PIN_SWDIO, 1);
    gpio_set_level(PIN_SWCLK, 0);
  }
}

void sbw_send_bit(uint8_t bit)
{
    gpio_set_level(PIN_SWDIO, bit);
    gpio_set_level(PIN_SWCLK, 1);
    gpio_set_level(PIN_SWCLK, 0);
}

void sbw_send_data(uint8_t command, uint32_t data)
{

    // Gửi byte đầu tiên (command)
    for (int i = 0; i < 3; i++) {
        sbw_send_bit((command >> i) & 1);
    }

    // Gửi dữ liệu
    for (int i = 0; i < 32; i++) {
        sbw_send_bit((data >> i) & 1);
    }

    // Hoàn thành truyền dữ liệu
    gpio_set_level(PIN_SWDIO, 0);
    gpio_set_level(PIN_SWCLK, 0);
}




