#include "display.h"
#include "display_config.h"

#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"

#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hub75.pio.h"

static uint32_t *buffer;

void display_start(void (*callback)(void)) {
    PIO pio = pio0;
    uint sm_data = 0;
    uint sm_row = 1;

    uint data_prog_offs = pio_add_program(pio, &hub75_data_rgb888_program);
    uint row_prog_offs = pio_add_program(pio, &hub75_row_program);
    hub75_data_rgb888_program_init(pio, sm_data, data_prog_offs, DATA_BASE_PIN, CLK_PIN);
    hub75_row_program_init(pio, sm_row, row_prog_offs, ROWSEL_BASE_PIN, ROWSEL_N_PINS, STROBE_PIN);

    static uint32_t gc_row[2][WIDTH];
    
    buffer = (uint32_t*) calloc(WIDTH * HEIGHT, sizeof(uint32_t));

    while (1) {
        if(callback) {
            callback();
        }
        for (int rowsel = 0; rowsel < (1 << ROWSEL_N_PINS); ++rowsel) {
            memcpy(gc_row[0], buffer + (WIDTH * rowsel) , WIDTH * sizeof(uint32_t));
            memcpy(gc_row[1], buffer + (WIDTH * (rowsel + HEIGHT / 2)), WIDTH * sizeof(uint32_t));
            
            for (int bit = 0; bit < 8; ++bit) {
                hub75_data_rgb888_set_shift(pio, sm_data, data_prog_offs, bit);
                for (int x = 0; x < WIDTH; ++x) {
                    pio_sm_put_blocking(pio, sm_data, gc_row[0][x]);
                    pio_sm_put_blocking(pio, sm_data, gc_row[1][x]);
                }
                // Dummy pixel per lane
                pio_sm_put_blocking(pio, sm_data, 0);
                pio_sm_put_blocking(pio, sm_data, 0);
                // SM is finished when it stalls on empty TX FIFO
                hub75_wait_tx_stall(pio, sm_data);
                // Also check that previous OEn pulse is finished, else things can get out of sequence
                hub75_wait_tx_stall(pio, sm_row);

                // Latch row data, pulse output enable for new row.
                pio_sm_put_blocking(pio, sm_row, rowsel | (100u * (1u << bit) << 5));
            }
        }
    }    
}

void display_set_pixel(uint8_t x, uint8_t y, uint32_t color)
{
    buffer[x + y * WIDTH] = color;
}

