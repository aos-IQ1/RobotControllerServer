#include "image.h"

#include <stdint.h>
#include <stddef.h>
#include <M5Stack.h>
#include <Arduino.h>

void render_image(images image) {
    switch (image) {
        case I_ICON :
            M5.Lcd.drawJpgFile(SD,"/icon.jpg");
            break;
        case I_WEB :
            M5.Lcd.drawJpgFile(SD,"/web.jpg");
            break;
        case I_CYBER :
            M5.Lcd.drawJpgFile(SD,"/cyber.jpg");
            break;
        case I_SUITS :
            M5.Lcd.drawJpgFile(SD,"/suits.jpg");
            break;
        default :
            return;
    }
}