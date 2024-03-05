#ifndef BMP_HEAD_H
#define BMP_HEAD_H

#include <stdint.h>

#include "zos_vfs.h"

#include "bmp_err.h"

typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} bmp_filehead_t;

/*
    TODO: Support more headers 
    - BITMAPCOREHEADER    : Not supported
    - BITMAPCOREHEADER2   : Not supported
    - BITMAPINFOHEADER    : Supported
    - BITMAPV4HEADER      : Not supported
    - BITMAPV5HEADER      : Not supported
*/
typedef struct {
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} bmp_infohead_t; 

typedef struct {
    uint8_t rgbBlue; 
    uint8_t rgbGreen; 
    uint8_t rgbRed;
    uint8_t rgbReserved;
} bmp_rgbquad_t;

typedef struct {
    bmp_filehead_t bmp_filehead;
    bmp_infohead_t bmp_infohead;
    bmp_rgbquad_t bmp_rgbquad;
} bmp_header_t;

typedef struct {
    zos_dev_t bmp_dev;
    bmp_header_t bmp_head;
} bmp_file_t;

bmp_err_t bmp_open(const char *filename, bmp_file_t *context);

#endif