#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

void display_start(void (*callback)(void));
void display_set_pixel(uint8_t x, uint8_t y, uint32_t color);
void display_draw_text(uint8_t x, uint8_t y, char *text, uint32_t color);

#endif
