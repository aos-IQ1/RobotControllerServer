#ifndef __IMAGE_H__
#define __IMAGE_H__

typedef enum { 
  I_ICON,
  I_WEB,
  I_CYBER,
  I_SUITS
} images;

void render_image(images);

#endif