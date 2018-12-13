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

#include "rgb_pixel.h"
#include "font_bitmap.h"

class Font {
private:
    struct FontInfo {
        std::shared_ptr<FontBitmap> bitmap_;

        int32_t horiBearingX_;

        int32_t horiBearingY_;

        int32_t horiAdvance_;

        void *data() {
            return bitmap_->data();
        }

        int32_t length() {
            return ((nullptr == data()) ? 0 : bitmap_->length());
        }

        int32_t size() {
            return length();
        }
    };

public:
    Font(std::string path);

    ~Font();

    bool setFontSize(int32_t font_size);

    bool toBitmapFile(std::string fname, std::string time_string, PixelFormat pixel_format = PixelFormat::RGB8BPP);

    std::shared_ptr<FontBitmap> toBitmapMem(std::string time_string);

    std::shared_ptr<FontBitmap> convert(PixelFormat pixel_format);

private:
    int32_t getTotalWidth(std::string time_string);

    int32_t getMaxWidth(std::string time_string);

    int32_t getMaxHeight(std::string time_string);

    int32_t getMaxBeringY(std::string time_string);

    bool enroll(char character);

    bool enroll(const char* text);

    bool enroll(const std::string text);

    std::shared_ptr<FontBitmap> rgb8pp_to_argb1555();

private:
    std::shared_ptr<FontBitmap> src_img_;

    std::shared_ptr<FontBitmap> dst_img_;

    PixelFormat  pixel_format_;

    int32_t total_width_;

    int32_t max_height_;

    FT_Library  library_;

    FT_Face     face_;

    FT_Glyph    glyph_;

    int32_t font_size_;

    std::map<char, FontInfo>  font_map_;
};

#endif //TEXTRENDER_FONT_H
