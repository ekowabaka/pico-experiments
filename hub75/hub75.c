#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define DATA_BASE_PIN 2
#define DATA_N_PINS 6
#define ROWSEL_BASE_PIN 8
#define ROWSEL_N_PINS 4
#define ROWSEL_MASK 0x0F << ROWSEL_BASE_PIN
#define CLK_PIN 14
#define STROBE_PIN 12
#define OEN_PIN 13

#define WIDTH 64
#define HEIGHT 32

int main() {
    stdio_init_all();
    for(int i = 0; i < DATA_N_PINS; i++) {
        gpio_init(DATA_BASE_PIN + i);
        gpio_set_dir(DATA_BASE_PIN + i, GPIO_OUT);
    }
    for(int i = 0; i < ROWSEL_N_PINS; i++) {
        gpio_init(ROWSEL_BASE_PIN + i);
        gpio_set_dir(ROWSEL_BASE_PIN + i, GPIO_OUT);
    }
    gpio_init(CLK_PIN);
    gpio_set_dir(CLK_PIN, GPIO_OUT);
    gpio_init(STROBE_PIN);
    gpio_set_dir(STROBE_PIN, GPIO_OUT);
    gpio_init(OEN_PIN);
    gpio_set_dir(OEN_PIN, GPIO_OUT);

    gpio_set_mask(0);
    uint8_t row = 0;

    while(1) {
        // Clock in data for first two rows
        for(int i = 0; i < DATA_N_PINS * WIDTH; i++) {
            for(int j = 0; j < DATA_N_PINS; j++) {
                gpio_put(DATA_BASE_PIN + j, 1); 
            }
            gpio_xor_mask(1 << CLK_PIN);
        }

        // set latch and OE high
        gpio_set_mask(1 << STROBE_PIN | 1 << OEN_PIN);

        // move to next row
        row++;
        if(row == 1 << ROWSEL_N_PINS) {
            row = 0;
        }
        gpio_put_masked(ROWSEL_MASK, row << ROWSEL_BASE_PIN);

        // set latch and OE low
        gpio_xor_mask(1 << STROBE_PIN | 1 << OEN_PIN);
    }
}
