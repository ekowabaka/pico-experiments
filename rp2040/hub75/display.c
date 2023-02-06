#include "display.h"
#include "display_config.h"

#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hub75.pio.h"

static uint32_t *buffer;

/**
 * @brief Initialize the display.
 * 
 * @param callback A callback to be called when the display is ready to be drawn.
 */
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

/**
 * @brief Draw a pixel to the current buffer.
 * 
 * @param x Coordinate of the pixel.
 * @param y Coordinate of the pixel.
 * @param color Color of the pixel.
 */
void display_set_pixel(uint8_t x, uint8_t y, uint32_t color)
{
    if(x < WIDTH && y < HEIGHT) {
        buffer[x + y * WIDTH] = color;
    }
}

/**
 * @brief Draw a rectangle to the buffer.
 * 
 * @param x X coordinate of the top left corner of the rectangle.
 * @param y Y coordinate of the top left corner of the rectangle.
 * @param width Width of the rectangle.
 * @param height Height of the rectangle.
 * @param color Color of the rectangle.
 */
void display_draw_rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint32_t color)
{
    for(int i = 0; i < width; ++i) {
        display_set_pixel(x + i, y, color);
        display_set_pixel(x + i, y + height - 1, color);
    }
    for(int i = 0; i < height; ++i) {
        display_set_pixel(x, y + i, color);
        display_set_pixel(x + width - 1, y + i, color);
    }
}

/**
 * @brief Draw a filled rectangle to the buffer.
 * 
 * @param x X coordinate of the top left corner of the rectangle.
 * @param y Y coordinate of the top left corner of the rectangle.
 * @param width Width of the rectangle.
 * @param height Height of the rectangle.
 * @param color Color of the rectangle.
 */
void display_fill_rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint32_t color)
{
    for(int i = 0; i < width; ++i) {
        for(int j = 0; j < height; ++j) {
            display_set_pixel(x + i, y + j, color);
        }
    }
}

void display_draw_image(const uint8_t x, const uint8_t y, const uint8_t width, const uint8_t height, const uint32_t *data)
{
    if(x > WIDTH || y > HEIGHT) {
        return;
    }
    int right = MIN(WIDTH, x + width);
    int bottom = MIN(HEIGHT, y + height);
    int new_width = right - x;
    for(int i = y; i < bottom; i++) {
        memcpy(buffer + i * WIDTH + x, data + (i - y) * width, new_width * sizeof(uint32_t));
    }
}

uint8_t display_put_char(uint8_t x, uint8_t y, unsigned char c, uint32_t color, const GFXfont *font)
{
    if ((x >= WIDTH) || (y >= HEIGHT)) {
        return 0;    
    }

    c -= font->first; // get actual index of character
    GFXglyph *glyph  = &font->glyph[c];
    uint8_t  *bitmap = font->bitmap;

    uint8_t  w     = glyph->width;
    uint8_t  h     = glyph->height;
    int8_t  xo    = glyph->xOffset;
    int8_t  yo    = glyph->yOffset;
    uint8_t  xx, yy, bits, bit = 0;
    int16_t bo = glyph->bitmapOffset;

    for(yy=0; yy<h; yy++) {
        for(xx=0; xx<w; xx++) {
            if(!(bit++ & 7)) {
                bits = bitmap[bo++];
            }
            if(bits & 0x80) {
                display_set_pixel(x + xo + xx, y + yo + yy, color);
            }
            bits <<= 1;
        }
    }

    return glyph->xAdvance;
}

void display_draw_text(uint8_t x, uint8_t y, char *text, uint32_t color, const GFXfont *font)
{
    int len = strlen(text);
    uint8_t x_cursor = x;
    for (int i = 0; i < len; i++) {
        x_cursor += display_put_char(x_cursor, y, text[i], color, font);
        if (x_cursor > WIDTH) {
            break;
        }
    }
}
