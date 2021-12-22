#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "display.h"
#include "display_config.h"

#define NUM_STARS 50

struct star {
    float x;
    uint8_t y;
    uint32_t color;
    float speed;
};

struct star stars[NUM_STARS];

void draw() {
    for(int i = 0; i < NUM_STARS; ++i) {
        display_set_pixel(stars[i].x, stars[i].y, 0);
    }
    for(int i = 0; i < NUM_STARS; ++i) {
        stars[i].x = stars[i].speed + stars[i].x;
        if(stars[i].x > WIDTH) {
            uint8_t color = rand() % 0xff;
            stars[i].x = 0;
            stars[i].y = rand() % 32;
            stars[i].speed = color / 2048.0;
            stars[i].color = color | (color << 8) | (color << 16);
        }
    }
    for(int i = 0; i < NUM_STARS; ++i) {
        display_set_pixel(stars[i].x, stars[i].y, stars[i].color);
    }
}

int main() {
    stdio_init_all();
    printf("Initializing with callback...\n");

    for(int i = 0; i < NUM_STARS; ++i) {
        uint8_t color = rand() % 0xff;
        stars[i].x = rand() % 64;
        stars[i].y = rand() % 32;
        stars[i].speed = color / 2048.0;
        stars[i].color = color | (color << 8) | (color << 16);
    }

    display_start(draw);
}
