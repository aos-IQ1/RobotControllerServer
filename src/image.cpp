#include "image.h"
#include "image_data.h"

#include <stdint.h>
#include <stddef.h>
#include <M5Stack.h>
#include <Arduino.h>

void render_image(images image) {
    uint8_t* data = icon_jpg;
    size_t len = icon_jpg_len;
    switch (image) {
        case I_ICON :
            data = icon_jpg;
            len = icon_jpg_len;
            break;
        case I_WEB :
            data = web_jpg;
            len = web_jpg_len;
            break;
        case I_CYBER :
            data = cyber_jpg;
            len = cyber_jpg_len;
            break;
        case I_SUITS :
            data = suits_jpg;
            len = suits_jpg_len;
            break;
        default :
            return;
    }
    M5.Lcd.drawJpg(data, len);
}