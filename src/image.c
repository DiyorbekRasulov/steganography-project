#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "image.h"

Image *load_bmp(const char *filename)
{
  // open the image in binary mode
  FILE *file = fopen(filename, "rb");

    if (file == NULL)
    {
        fprintf(stderr, "failed to open bmp file\n");
        return NULL;
	
    }
    
    
    // allocate memory for the image struct
    Image *image = malloc(sizeof(Image));

    if (image == NULL)
    {
        fprintf(stderr, "failed to allocate image struct\n");
        fclose(file);
        return NULL;
    }

    /* read the first 54 bytes of the bmp header */
    size_t bytes_read = fread(image->header, 1, 54, file);

    if (bytes_read != 54)
    {
        fprintf(stderr, "failed to read bmp header\n");

        fclose(file);
        free(image);

        return NULL;
    }

    /* verify this is actually a bmp file */
    if (image->header[0] != 'B' || image->header[1] != 'M')
    {
        fprintf(stderr, "file is not a valid bmp\n");

        fclose(file);
        free(image);

        return NULL;
    }

    /* extract width from the header */
    image->width = *(int *)&image->header[18];

    /* extract height from the header */
    image->height = *(int *)&image->header[22];

    /* extract bits per pixel */
    int bits_per_pixel = *(short *)&image->header[28];

    /* right now we only support 24-bit bmp files */
    if (bits_per_pixel != 24)
    {
        fprintf(stderr, "only 24-bit bmp files are supported\n");

        fclose(file);
        free(image);

        return NULL;
    }

    /* 24-bit rgb means 3 color channels */
    image->channels = 3;

    /* calculate total pixel data size */
    image->data_size = image->width * image->height * image->channels;

    /* allocate memory for the pixel data */
    image->data = malloc(image->data_size);

    if (image->data == NULL)
    {
        fprintf(stderr, "failed to allocate pixel data\n");

        fclose(file);
        free(image);

        return NULL;
    }

    /* move file pointer to pixel array location */
    int pixel_offset = *(int *)&image->header[10];

    fseek(file, pixel_offset, SEEK_SET);

    /* read all pixel bytes into memory */
    bytes_read = fread(image->data, 1, image->data_size, file);

    if ((int)bytes_read != image->data_size)
    {
        fprintf(stderr, "failed to read pixel data\n");

        fclose(file);

        free(image->data);
        free(image);

        return NULL;
    }

    /* close the file because everything is now in memory */
    fclose(file);

    return image;
}

int save_bmp(const char *filename, Image *image)
{
    /* open output file in binary write mode */
    FILE *file = fopen(filename, "wb");

    if (file == NULL)
    {
        fprintf(stderr, "failed to create output bmp\n");
        return 1;
    }

    /* write the original bmp header first */
    size_t bytes_written = fwrite(image->header, 1, 54, file);

    if (bytes_written != 54)
    {
        fprintf(stderr, "failed to write bmp header\n");

        fclose(file);

        return 1;
    }

    /* bmp pixel data starts after the header */
    int pixel_offset = *(int *)&image->header[10];

    /* move to where pixel bytes belong */
    fseek(file, pixel_offset, SEEK_SET);

    /* write modified pixel bytes */
    bytes_written = fwrite(image->data, 1, image->data_size, file);

    if ((int)bytes_written != image->data_size)
    {
        fprintf(stderr, "failed to write pixel data\n");

        fclose(file);

        return 1;
    }

    fclose(file);

    return 0;
}

void free_image(Image *image)
{
    /* avoid freeing a null pointer */
    if (image == NULL)
    {
        return;
    }

    /* free pixel memory first */
    free(image->data);

    /* then free the image struct itself */
    free(image);
}
