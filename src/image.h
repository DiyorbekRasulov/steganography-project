#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

// bmp files store a header before the actual pixel data
typedef struct
{
  uint8_t header[54];

  // raw pixel bytes get stored here
  uint8_t *data;

  // image dimensions
  int width;
  int height;
  
  // usually 3 for rgb
  int channels;
  
  // total size of pixel data
  int data_size;
  
} Image;

// loads a bmp image into memory
Image *load_bmp(const char *filename);

// saves a modified bmp back to disk
int save_bmp(const char *filename, Image *image);

// frees all allocated image memory
void free_image(Image *image);

#endif
