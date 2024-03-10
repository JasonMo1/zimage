#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// #include "SDL.h"
// #include "SDL_image.h"

#include "bmp_func.h"
#include "bmp_head.h"
#include "bmp_rgb.h"
#include "bmp_err.h"
#include "zim_scope.h"

#include "zos_errors.h"
#include "zos_sys.h"
#include "zos_vfs.h"

/*
int ____main() {
    // zos_dev_t avctx->bmp_dev = open("B:/lena.bmp", O_RDONLY);

    // if (avctx->bmp_dev < 0) {
    //     printf("Error occurred while loading BMP file, error code: %d\n", -avctx->bmp_dev);
    //     return 1;
    // }

    // zos_stat_t avctx->bmp_dev_inf;
    // zos_err_t staterr = dstat(avctx->bmp_dev, &avctx->bmp_dev_inf);
    // printf("size: %ld", avctx->bmp_dev_inf);
    SDL_Window* window = NULL;//窗口指针
    SDL_Surface* screenSurface = NULL;  //窗口渲染区域指针
    //SDL初始化，关于SDL_INIT_VIDEO我们后面的章节会介绍
    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        printf("SDL初始化失败%s", SDL_GetError());
    }
    else
    {
        //创建SDL窗口，100,200是窗口的位置
        //（你可以用SDL_WINDOWPOS_UNDEFINED，让窗口出现在屏幕正中）
        //1024, 768是窗口的大小
        window = SDL_CreateWindow("Hello World", 
                          100, 200, 1024, 768, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            printf("创建窗口失败%s", SDL_GetError());
        }
        else
        {
            screenSurface = SDL_GetWindowSurface(window);//获取窗口渲染区域
            //向渲染区域涂色
            SDL_FillRect(screenSurface, NULL, 
                    SDL_MapRGB(screenSurface.format, 0xFF, 0xFF, 0xFF));
            SDL_Surface* imgSurface = IMG_Load("/home/jasonmo/mediap/lena/lena.bmp");
            if (imgSurface == NULL)
            {
                printf("加载图片失败%s", SDL_GetError());
                return -1;
            }
            SDL_BlitSurface(imgSurface, NULL, screenSurface, NULL);  //把图片渲染到窗口的绘图对象上
            SDL_FreeSurface(imgSurface); //释放绘图对象
            //把内容渲染到窗口上
            SDL_UpdateWindowSurface(window);          
            SDL_Event e; //定义一个SDL消息
            bool quit = false; //消息循环的哨兵变量
            while (quit == false) //开始消息循环
            { 
                //等待获取下一条消息
                //这里不能用SDL_PollEvent方法，会造成高CPU消耗
                while (SDL_WaitEvent(&e) != 0)
                { 
                    //如果消息是SDL_QUIT则退出消息循环
                    if (e.type == SDL_QUIT) {
                        quit = true;
                        break;
                    }
                } 
            }
        }
    }  
    //消息循环退出之后，关闭窗口
    SDL_DestroyWindow(window);
    //释放SDL资源，退出应用
    SDL_Quit();
    return 0;
}
*/

bmp_err_t main() {
    const char ctx[] = SCO_BMP_ORG;
    bmp_file_t avctx;
    bmp_err_t openstatus = bmp_open("B:/lena.bmp", &avctx);
    
    if (openstatus != BMP_SUCCESS) {
        printf("%s Error occuceded: %u\n", ctx, openstatus);
        return openstatus;
    }
    else {
        return BMP_SUCCESS;
    }
}

bmp_err_t bmp_open(const char *filename, bmp_file_t *avctx) {
    const char ctx[] = SCO_IO_FILE;
    printf("%s Memory address of avctx: %p\n", SCO_DBG_LOG, avctx);
    printf("%s Trying to open file: %s\n", ctx, filename);
    avctx->bmp_dev = open(filename, O_RDONLY);
    uint16_t headersize = sizeof(bmp_header_t);
    zos_err_t filestatus = read(avctx->bmp_dev, &avctx->bmp_head, &headersize);

    // Check file status
    if (avctx->bmp_dev < 0) {
        printf("%s Error while opening file\n", ctx);
        return BMP_ERR_FILE;
    } 
    else if (filestatus != ERR_SUCCESS) {
        close(avctx->bmp_dev);
        printf("%s Error while reading file\n", ctx);
        return BMP_ERR_FILE;
    }
    else {
        printf("%s File loaded\n", ctx);
    }

    // Check bmp header
    // Check magic number (BM), big endian

    uint32_t _bfSize = avctx->bmp_head.bmp_filehead.bfSize;
    uint32_t _biSize = avctx->bmp_head.bmp_infohead.biSize;
    uint16_t _biComp = avctx->bmp_head.bmp_infohead.biCompression;
    uint16_t _biBitCount = avctx->bmp_head.bmp_infohead.biBitCount;

    if (avctx->bmp_head.bmp_filehead.bfType != 0x4d42) {
        close(avctx->bmp_dev);
        printf("%s Bad magic value or unsupported bitmap format: %x\n", ctx, avctx->bmp_head.bmp_filehead.bfType);
        return BMP_ERR_BAD_MAGIC_NUMBER;
    }
    else if (_biSize + 14LL > _bfSize) {
        close(avctx->bmp_dev);
        printf("%s Invalid header size: bf: %lu, bi: %lu\n", ctx, _bfSize, _biSize);
        return BMP_ERR_INVALID_DATA;
    }
    else if ((avctx->bmp_head.bmp_infohead.biHeight == 0) || (avctx->bmp_head.bmp_infohead.biWidth == 0)) {
        close(avctx->bmp_dev);
        printf("%s No bitmap data found\n", ctx);
        return BMP_ERR_NONE;
    }
    else if (avctx->bmp_head.bmp_infohead.biPlanes != 1) {
        close(avctx->bmp_dev);
        printf("%s Invalid bmp header: %u", ctx, avctx->bmp_head.bmp_infohead.biPlanes);
        return BMP_ERR_INVALID_DATA;
    }
    else if (_biComp != BMP_RGB || _biBitCount != 24) {
        close(avctx->bmp_dev);
        printf("%s Unsupported bmp coding: %u", ctx, _biComp);
        return BMP_ERR_TYPE_NOT_SUPPORT;
    }
    else {
        printf("%s Correct bmp header\n", ctx);
    }

    bool palette_exist = true;
    if (avctx->bmp_head.bmp_filehead.bfOffBits == sizeof(bmp_filehead_t) + sizeof(bmp_infohead_t)) {
        palette_exist = false;
    }

    printf("%s DEBUG: Header info\n", SCO_DBG_LOG);
    printf("bfSize: %lu\n", _bfSize);
    printf("biSize: %lu\n", _biSize);
    printf("biHeight: %lu\n", avctx->bmp_head.bmp_infohead.biHeight);
    printf("biWidth: %lu\n", avctx->bmp_head.bmp_infohead.biWidth);
    printf("biBitCount: %u\n", avctx->bmp_head.bmp_infohead.biBitCount);
    printf("palette_exist: %d\n", palette_exist);

    return BMP_SUCCESS;
}

// bmp_err_t bmp_decode(bmp_file_t *avctx, void *pixel_array) {
//     const char ctx[] = SCO_BMP_DEC;

//     // Create an array to store the RGB565 pixels
//     uint32_t width = avctx->bmp_head.bmp_infohead.biWidth;
//     uint32_t height = avctx->bmp_head.bmp_infohead.biHeight;
//     uint16_t *pixels = malloc(width * height * sizeof(uint16_t));
//     if (pixels == NULL) {
//         close(avctx->bmp_dev);
//         printf("%s Error while allocating memory for pixels\n", ctx);
//         return BMP_ERR_MEMORY;
//     }

//     // Read the bmp data and convert to RGB565 format
//     uint32_t row_size = ((24 * width + 31) / 32) * 4; // The number of bytes in each row of the bmp data, padded to a multiple of 4
//     uint8_t *row_buffer = malloc(row_size); // A buffer to store one row of bmp data
//     if (row_buffer == NULL) {
//         close(avctx->bmp_dev);
//         free(pixels);
//         printf("%s Error while allocating memory for row buffer\n", ctx);
//         return BMP_ERR_MEMORY;
//     }

//     // Loop through each row of the bmp data, from bottom to top
//     for (int i = height - 1; i >= 0; i--) {
//         // Read one row of bmp data into the buffer
//         uint16_t bytes_read = row_size;
//         zos_err_t read_status = read(avctx->bmp_dev, row_buffer, &bytes_read);
//         if (read_status != ERR_SUCCESS || bytes_read != row_size) {
//             close(avctx->bmp_dev);
//             free(pixels);
//             free(row_buffer);
//             printf("%s Error while reading bmp data\n", ctx);
//             return BMP_ERR_FILE;
//         }

//         // Loop through each pixel in the row, from left to right
//         for (int j = 0; j < width; j++) {
//             // Get the RGB values from the buffer, in BGR order
//             uint8_t b = row_buffer[j * 3];
//             uint8_t g = row_buffer[j * 3 + 1];
//             uint8_t r = row_buffer[j * 3 + 2];

//             // Convert the RGB values to RGB565 format, using bit shifting and masking
//             uint16_t rgb565 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);

//             // Store the RGB565 value in the pixels array, using the row and column index
//             pixels[i * width + j] = rgb565;
//         }
//     }

//     // Close the bmp file and free the row buffer
//     close(avctx->bmp_dev);
//     free(row_buffer);

//     // Return the pixels array
//     return pixels;
// }

