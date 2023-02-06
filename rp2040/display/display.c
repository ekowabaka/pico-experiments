
#include <stdio.h>
#include <string.h>
#include "display.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT 	spi0
#define PIN_MISO 	16
#define PIN_CS   	17
#define PIN_SCK  	18
#define PIN_MOSI 	19
#define PIN_RESET	20
#define PIN_DC		21
#define PIN_TE    22

// Thanks Adafruit!
static const uint8_t initseq[] = {
  0xEF, 3, 0x03, 0x80, 0x02,
  0xCF, 3, 0x00, 0xC1, 0x30,
  0xED, 4, 0x64, 0x03, 0x12, 0x81,
  0xE8, 3, 0x85, 0x00, 0x78,
  0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
  0xF7, 1, 0x20,
  0xEA, 2, 0x00, 0x00,
  0xC0  , 1, 0x23,             // Power control VRH[5:0]
  0xC1, 1, 0x10,             // Power control SAP[2:0];BT[3:0]
  0xC5, 2, 0x3e, 0x28,       // VCM control
  0xC7, 1, 0x86,             // VCM control2
  0x36, 1, 0xe0,             // Memory Access Control
  0x37, 1, 0x00,             // Vertical scroll zero
  0x3A, 1, 0x55,
  0xB1, 2, 0x00, 0x18,
  0xB6, 3, 0x08, 0x82, 0x27, // Display Function Control
  0xF2, 1, 0x00,                         // 3Gamma Function Disable
  0x26, 1, 0x01,             // Gamma curve selected
  0xE0, 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
    0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
  0xE1, 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
    0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
  0x11, 0,                // Exit Sleep
  0x35, 0,
  0x29, 0,                // Display on
  0x00
};

void send_command(uint8_t command) {
	gpio_put(PIN_CS, 0);
	gpio_put(PIN_DC, 0);
	spi_write_blocking(SPI_PORT, &command, 1);
	//gpio_put(PIN_CS, 1);
	gpio_put(PIN_DC, 1);

  //sleep_ms(150);

  //A good amount of delay to get the command properly registered
  // for(int i = 0; i < 100; i++) {
  //   asm("nop");
  // }
}

void send_data(const void * data, int size) {
	//gpio_put(PIN_DC, 1);
	gpio_put(PIN_CS, 0);
	spi_write_blocking(SPI_PORT, data, size);
	gpio_put(PIN_CS, 1);
}

uint8_t reverseByte(uint8_t b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

// void send_uint16(uint16_t data) {
// 	gpio_put(PIN_DC, 1);
// 	gpio_put(PIN_CS, 0);
//   //data = (reverseByte(data & 0xff) << 8) | (reverseByte(data >> 8));
// 	spi_write16_blocking (SPI_PORT, &data, 1);
// 	gpio_put(PIN_CS, 1);
//   // send_data(&data, 2);
// }

void send_uint8(uint8_t data) {
  send_data(&data, 1);
}

uint16_t buffer[86400];
//uint16_t color = 0x0780;
uint8_t color = 0x07;

void alternate_display(uint gpio, uint32_t events) {
  if(events & GPIO_IRQ_EDGE_FALL) {
    return;
  }
  int i = 0;
  send_command(0x2c);
  // for(int x = 0; x < 320; x++) {
  //   for(int y = 0; y < 240; y++) {
  //     buffer[i] = color;
  //     i++;
  //   }
  // }
  memset(buffer, color, 153600);
  //color += 10;
  if(color == 0x07){ //80) {
    color = 0xff; //00;
  } else {
    color = 0x07;//80;
  }
  send_data(buffer, 153600);
  printf("Sent!");
}

void display_init() {
    // SPI initialisation. This example will use SPI at 1MHz.
    printf("Initialized spi to %d\n", spi_init(SPI_PORT, 62500000));

    gpio_set_function(PIN_MISO, 	GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK,  	GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, 	GPIO_FUNC_SPI);
    
    // gpio_set_function(PIN_CS,   	GPIO_FUNC_SIO);
    // gpio_set_function(PIN_DC,   	GPIO_FUNC_SIO);
    // gpio_set_function(PIN_RESET,   	GPIO_FUNC_SIO);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 0);

    gpio_init(PIN_DC);
    gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_put(PIN_DC, 0);


    gpio_init(PIN_RESET);
    gpio_set_dir(PIN_RESET, GPIO_OUT);
    gpio_put(PIN_RESET, 1);

    sleep_ms(150);
    gpio_put(PIN_RESET, 0);
    sleep_ms(150);
    gpio_put(PIN_RESET, 1);

 
    
    send_command(0x01);

    uint8_t command, numArgs;
    const uint8_t *sequence = initseq;

    while(true) {
    	command = sequence[0];
      printf("%X\n", command);
    	if(command == 0) {
    		break;
    	}
    	numArgs = sequence[1];
		  send_command(command);
		  sequence += 2;
    	if(numArgs > 0) {
    		send_data(sequence, numArgs);
    	} else {
			  sleep_ms(150);
    	}
		  sequence+=numArgs;
    }

    send_command(0x2a);
    send_uint8(0);
    send_uint8(0);
    send_uint8(1);
    send_uint8(0x3f);

    send_command(0x2b);

    send_uint8(0);
    send_uint8(0);
    send_uint8(0);
    send_uint8(200);

   gpio_set_irq_enabled_with_callback(PIN_TE, GPIO_IRQ_EDGE_RISE|GPIO_IRQ_EDGE_FALL, true, &alternate_display);
   while (true);

    // while(true) {
    //   alternate_display();
    // }
}