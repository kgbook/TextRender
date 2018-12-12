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
public:
    Font(std::string path);

    ~Font();

    bool setFontSize(int32_t font_size);

    bool toBitmapFile(std::string fname, std::string time_string);

    int32_t toBitmapMem(std::string time_string);

private:
    enum class BitmapDepth {
        kColorDepth_1,
        kColorDepth_2,
        kColorDepth_3,
        kColorDepth_4,
        kColorDepth_5,
        kColorDepth_6,
        kColorDepth_8,
        kColorDepth_12,

        /* BitmapDepth::kSize is the max value of the enum,  sometimes it indicates an error if return value is kSize */
            kSize
    };

    struct BitmapInfo {
        uint8_t *addr_;

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

private:
    int32_t getTotalWidth(std::string time_string);

    int32_t getMaxWidth(std::string time_string);

    int32_t getMaxHeight(std::string time_string);

    int32_t getMaxBeringY(std::string time_string);

    bool enroll(char character);

    bool enroll(const char* text);

    bool enroll(const std::string text);

    bool createBitmap(int32_t total_width, int32_t max_height, BitmapDepth depth = BitmapDepth::kColorDepth_8);

    void destroyBitmap();

private:
    BitmapInfo image;

    int32_t bytes_per_pixel_;

    FT_Library  library;

    FT_Face     face;

    FT_Glyph    glyph;

    int32_t font_size_;

    std::map<char, FontInfo>  font_map_;
};

#endif //TEXTRENDER_FONT_H
