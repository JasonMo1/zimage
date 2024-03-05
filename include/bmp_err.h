#ifndef BMP_ERR_H
#define BMP_ERR_H

typedef enum {
    BMP_SUCCESS = 0,
    BMP_ERR_BAD_MAGIC_NUMBER,
    BMP_ERR_FILE,
    BMP_ERR_TYPE_NOT_SUPPORT,
    BMP_ERR_NONE,
    BMP_ERR_INVALID_DATA,
    BMP_ERR_MEMORY
} bmp_err_t;

#endif