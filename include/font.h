//
// Created by kang on 2018/12/12.
//

#ifndef TEXTRENDER_FONT_H
#define TEXTRENDER_FONT_H

#include <map>
#include <string>
#include <cstdint>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <freetype/freetype.h>
#include <freetype/ftglyph.h>

class Font {
private:
    enum class PixelFormat {
        RGB444,
        RGB555,
        RGB565,
        RGB888,

        BGR444,
        BGR555,
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
        RGB16BPP,

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

    struct BitmapInfo {
        RGB8BPPPixel *addr_;

        int32_t width_;

        int32_t height_;

        int32_t len_;
    };

    struct FontInfo {
        BitmapInfo bitmap_;

        int32_t horiBearingX_;

        int32_t horiBearingY_;

        int32_t horiAdvance_;

        void *data() {
            return bitmap_.addr_;
        }

        int32_t length() {
            return ((nullptr == data()) ? 0 : bitmap_.len_);
        }

        int32_t size() {
            return length();
        }
    };

public:
    Font(std::string path);

    ~Font();

    bool setFontSize(int32_t font_size);

    bool toBitmapFile(std::string fname, std::string time_string);

    int32_t toBitmapMem(std::string time_string);

private:
    int32_t getTotalWidth(std::string time_string);

    int32_t getMaxWidth(std::string time_string);

    int32_t getMaxHeight(std::string time_string);

    int32_t getMaxBeringY(std::string time_string);

    bool enroll(char character);

    bool enroll(const char* text);

    bool enroll(const std::string text);

    bool createBitmap(int32_t total_width, int32_t max_height, PixelFormat pixel_format = PixelFormat::RGB8BPP);

    void destroyBitmap();

    int64_t convert(PixelFormat pixel_format);

    int64_t convert_to_argb1555();

private:
    BitmapInfo image_;

    ARGB1555Pixel* argb1555_bitmap_;

    int32_t bytes_per_pixel_;

    FT_Library  library_;

    FT_Face     face_;

    FT_Glyph    glyph_;

    int32_t font_size_;

    std::map<char, FontInfo>  font_map_;
};

#endif //TEXTRENDER_FONT_H
