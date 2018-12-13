//
// Created by kang on 2018/12/14.
//

#include <cmath>
#include <cstring>
#include "font_bitmap.h"
#include "exception.h"

using namespace Exception;

FontBitmap::FontBitmap(PixelFormat pixel_format)
    : pixel_format_(pixel_format){
    //
}

FontBitmap::FontBitmap(int32_t width, int32_t height, PixelFormat pixel_format)
    : pixel_format_(pixel_format), width_(width), height_(height){
    runtimeAssert("Not support such pixel format!", pixel_format_ == PixelFormat::RGB8BPP || pixel_format_ == PixelFormat::ARGB1555);
    auto bpp = getBpp(pixel_format_);
    bytes_per_pixel_ = static_cast<int32_t >(ceil(bpp / 8.0));
    len_ = static_cast<int64_t >(stride() * this->height());
    allocate(len_);
}

FontBitmap::~FontBitmap() {
    this->free(buf_);
}

uint8_t* FontBitmap::allocate(size_t size) {
    buf_ = new uint8_t[size];
    std::memset(buf_, 0, size);
    return buf_;
}

void FontBitmap::free(uint8_t *ptr) {
    delete[](ptr);
}

uint8_t* FontBitmap::data() {
    return buf_;
}

int64_t FontBitmap::length() {
    return len_;
}

int64_t FontBitmap::size() {
    return length();
}

int32_t FontBitmap::getBpp(PixelFormat pixel_format) {
    int32_t bpp = -1;

    switch(pixel_format_) {
        case PixelFormat::RGB8BPP: {
            bpp = 8;
            break;
        }

        case PixelFormat::RGB444:
        case PixelFormat::BGR444:
        case PixelFormat::RGB12BPP: {
            bpp = 12;
            break;
        }

        case PixelFormat::RGB14BPP: {
            bpp = 14;
            break;
        }

        case PixelFormat::RGB565:
        case PixelFormat::BGR565:
        case PixelFormat::ARGB4444:
        case PixelFormat::ABGR4444:
        case PixelFormat::ARGB1555:
        case PixelFormat::ABGR1555:{
            bpp = 16;
            break;
        }

        case PixelFormat::RGB888:
        case PixelFormat::BGR888:
        case PixelFormat::ARGB8565:
        case PixelFormat::ABGR8565:{
            bpp = 24;
            break;
        }

        case PixelFormat::ARGB8888:
        case PixelFormat::ABGR8888: {
            bpp = 32;
            break;
        }

        default: {
            runtimeAssert("Not support such pixel format!", ErrorCode::invalidParameter);
        }
    }

    return bpp;
}

int32_t FontBitmap::bytesPerPixel() {
    return bytes_per_pixel_;
}

int32_t FontBitmap::stride() {
    return (width() * bytesPerPixel());
}

int32_t FontBitmap::width() {
    return width_;
}

int32_t FontBitmap::height() {
    return height_;
}