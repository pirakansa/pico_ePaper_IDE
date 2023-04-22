#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "ImageData.h"
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "EPD_2in13_V3.h"


const uint LED_PIN = PICO_DEFAULT_LED_PIN;


void initialize_board(){
    bi_decl(bi_program_description("https://github.com/pirakansa"));

    stdio_init_all();
    gpio_init(LED_PIN);
    if(DEV_Module_Init()!=0){
        return -1;
    }
    gpio_set_dir(LED_PIN, GPIO_OUT);
    EPD_2in13_V3_Init();
    EPD_2in13_V3_Clear();

    printf("boot pico program\n");
}

int main ()
{
    initialize_board();

    //Create a new image cache
    UBYTE *BlackImage;
    UWORD Imagesize = ((EPD_2in13_V3_WIDTH % 8 == 0)? (EPD_2in13_V3_WIDTH / 8 ): (EPD_2in13_V3_WIDTH / 8 + 1)) * EPD_2in13_V3_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    Paint_NewImage(BlackImage, EPD_2in13_V3_WIDTH, EPD_2in13_V3_HEIGHT, 0, WHITE);
	// Paint_Clear(WHITE);

    // Paint_SelectImage(BlackImage);
    Paint_DrawBitMap(lennaImage);
    EPD_2in13_V3_Display(BlackImage);

    sleep_ms(5000);


    free(BlackImage);
    BlackImage = NULL;
    DEV_Module_Exit();


    while(1)
    {
        gpio_put(LED_PIN, 1);
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
        sleep_ms(500);
    }
}
