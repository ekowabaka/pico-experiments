#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "display.h"
#include "display_config.h"
#include "smiling1.h"


void draw() {
    display_draw_image(2, 2, IMG_WIDTH, IMG_HEIGHT, smiley_1);
}

int main() {
    stdio_init_all();
    printf("Initializing with callback...\n");

    display_start(draw);
}
