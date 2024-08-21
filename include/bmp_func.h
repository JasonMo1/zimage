#ifndef BMP_FUNC_H
#define BMP_FUNC_H

#include "bmp_err.h"
#include "bmp_head.h"

bmp_err_t bmp_open(const char *filename, bmp_file_t *avctx);

bmp_err_t bmp_decode(bmp_file_t *avctx);

uint16_t bmp_get_pixel(bmp_file_t *avctx);

#endif