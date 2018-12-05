#ifndef __IMAGE_H__
#define __IMAGE_H__

typedef enum { 
  I_SAMPLE1,
  I_SAMPLE2
} images;

void render_image(images);

#endif