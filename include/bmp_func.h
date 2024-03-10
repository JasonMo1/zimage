#ifndef BMP_FUNC_H
#define BMP_FUNC_H

#include "bmp_err.h"
#include "bmp_head.h"

bmp_err_t bmp_open(const char *filename, bmp_file_t *avctx);

#endif