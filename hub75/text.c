#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "display.h"
#include "display_config.h"
#include "smiling1.h"


void draw() {
    display_put_char(2, 2, 'J', 0xffffff, 1);
}

int main() {
    stdio_init_all();
    printf("Initializing with callback...\n");

    display_start(draw);
}
