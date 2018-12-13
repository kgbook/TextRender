//
// Created by kang on 2018/12/14.
//

#ifndef TEXTRENDER_FONT_BITMAP_H
#define TEXTRENDER_FONT_BITMAP_H

#include <cstdint>
#include <cstddef>
#include "rgb_pixel.h"

class FontBitmap {
public:
    FontBitmap(PixelFormat pixel_format = PixelFormat::RGB8BPP);

    FontBitmap(int32_t width, int32_t height, PixelFormat pixel_format = PixelFormat::RGB8BPP);

    ~FontBitmap();

    uint8_t *data();

    int64_t length();

    int64_t size();

    int32_t bytesPerPixel();

    int32_t stride();

    int32_t width();

    int32_t height();

private:
    uint8_t *allocate(size_t size);

    void free(uint8_t *ptr);

    int32_t getBpp(PixelFormat pixel_format);

private:
    uint8_t *buf_;

    int32_t width_;

    int32_t height_;

    int64_t len_;

    int32_t bytes_per_pixel_;

    PixelFormat pixel_format_;
};

#endif //TEXTRENDER_FONT_BITMAP_H
