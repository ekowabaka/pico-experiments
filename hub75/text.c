#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "display.h"
#include "display_config.h"
#include "smiling1.h"
#include "fonts/Org_01.h"
#include "fonts/Picopixel.h"
#include "fonts/Tiny3x3a2pt7b.h"
#include "fonts/TomThumb.h"
#include "fonts/Font_5x7_practical.h"
#include "fonts/spleen-5x8.h"


void draw() {
    //display_put_char(2, 16, 'A', 0x081d3d & 0x101010, &Org_01);
    display_draw_text(0, 10, "Ekow", 0x0020ff, &spleen5x8);
}

int main() {
    stdio_init_all();
    printf("Initializing with callback...\n");
    display_start(draw);
}
