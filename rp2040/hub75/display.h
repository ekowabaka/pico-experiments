#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "gfxfont.h"

void display_start(void (*callback)(void));
void display_set_pixel(uint8_t x, uint8_t y, uint32_t color);
uint8_t display_put_char(uint8_t x, uint8_t y, unsigned char c, uint32_t color, const GFXfont *font);
void display_draw_text(uint8_t x, uint8_t y, char *text, uint32_t color, const GFXfont *font);
void display_draw_image(const uint8_t x, const uint8_t y, const uint8_t width, const uint8_t height, const uint32_t *data);

#endif
