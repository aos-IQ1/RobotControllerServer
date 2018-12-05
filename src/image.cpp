#include "image.h"
#include "image_data.h"

#include <stdint.h>
#include <stddef.h>
#include <M5Stack.h>
#include <Arduino.h>

void render_image(images image) {
    uint8_t* data;
    size_t len;
    uint16_t x = 0;
    uint16_t y = 0;
    switch (image) {
        case I_SAMPLE1 :
            data = sample1_jpg;
            len = sample1_jpg_len;
            break;
        case I_SAMPLE2 :
            data = sample2_jpg;
            len = sample2_jpg_len;
            break;
    }
    M5.Lcd.drawJpg(data, len, x, y);
}