//
// Created by kang on 2018/12/14.
//

#ifndef TEXTRENDER_RGB_PIXEL_H
#define TEXTRENDER_RGB_PIXEL_H

enum class PixelFormat {
    RGB444,
    RGB565,
    RGB888,

    BGR444,
    BGR565,
    BGR888,

    ARGB1555,
    ARGB4444,
    ARGB8565,
    ARGB8888,

    ABGR1555,
    ABGR4444,
    ABGR8565,
    ABGR8888,

    RGB8BPP,
    RGB10BPP,
    RGB12BPP,
    RGB14BPP,

    kSize /* kSize is the max value of the enum,  sometimes it indicates an error if return value is kSize */
};

struct RGB8BPPPixel {
    uint8_t red :3;
    uint8_t green :3;
    uint8_t blue :2;
};

struct ARGB1555Pixel {
    uint16_t alpha:1;
    uint16_t blue:5;
    uint16_t green:5;
    uint16_t red:5;
};

#endif //TEXTRENDER_RGB_PIXEL_H
