//
// Created by kang on 2018/12/12.
//

#include <iostream>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "font.h"
#include "exception.h"

using std::cout;
using std::endl;
using namespace Exception;

Font::Font(std::string path) {
    runtimeAssert("such file not exist!", (path.length() > 1) && (0 == access(path.c_str(), F_OK)));
    runtimeAssert("FT_Init_FreeType failed!", 0 == FT_Init_FreeType(&library));
    runtimeAssert("FT_New_Face failed!", 0 == FT_New_Face(library, path.c_str(), 0, &face));
    std::memset(&image, 0, sizeof(image));
    argb1555_bitmap = nullptr;
}

Font::~Font() {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
    destroyBitmap();
}

bool Font::setFontSize(int32_t font_size) {
    return (0 == FT_Set_Pixel_Sizes(face, 0, font_size));
}

int32_t Font::toBitmapMem(std::string time_string) {
    if (!enroll(time_string)) {
        return false;
    }

    int32_t max_height = getMaxHeight(time_string);
    int32_t total_width = getTotalWidth(time_string);
    int32_t max_horizontal_bering_Y = getMaxBeringY(time_string);
    int32_t start_byte = 0;
    int32_t bytes_per_copy = 0;
    int32_t copied_horizontal_byte = 0;

    if (max_height < 0 || max_horizontal_bering_Y < 0 || 0 == total_width) {
        return -1;
    }
    createBitmap(total_width, max_height);

    int32_t stride = total_width * bytes_per_pixel_;
    for (auto character = time_string.begin(); character != time_string.end(); character++) {
        auto it = font_map_.find(*character);
        auto start_row = max_horizontal_bering_Y - it->second.horiBearingY_;
        bytes_per_copy = it->second.bitmap_.width_ * bytes_per_pixel_;
        start_byte = copied_horizontal_byte + it->second.horiBearingX_;

        if (it != font_map_.end()) {
            for (int32_t row = 0; row < max_height; row++) {
                if (0 == bytes_per_copy) {
                    break;
                }

                if (row >= start_row && row < (start_row + it->second.bitmap_.height_)) {
                    std::memcpy(image.addr_ + start_byte, it->second.bitmap_.addr_ + (row - start_row) * bytes_per_copy, bytes_per_copy);
                }
                start_byte += stride;
            }
            copied_horizontal_byte += it->second.horiAdvance_;
        } else {
            cout <<"character:" <<*character <<" not found!" <<endl;
            return -1;
        }
    }
    std::cout <<"total_width:" <<total_width <<", max_height:" <<max_height <<std::endl;

    return (max_height * stride);
}

bool Font::toBitmapFile(std::string fname, std::string time_string) {
    int fd = open(fname.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if (fd < 0) {
        cout <<"open failed! " <<fname <<", " <<strerror(errno) <<endl;
        return false;
    }

    int32_t len = toBitmapMem(time_string);
    if (len < 1) {
        return false;
    }
#if 0
    if (convert(PixelFormat::ARGB1555) < 0) {
        cout <<"convert failed!" <<endl;
        return false;
    }

    auto num = write(fd, argb1555_bitmap, image.width_ * image.height_ * sizeof(ARGB1555Box));
    if (num != (image.width_ * image.height_ * sizeof(ARGB1555Box))) {
        cout<<"write failed! " <<endl;
        return false;
    }
#else
    auto num = write(fd, image.addr_, image.len_);
    if (num != image.len_) {
        cout<<"write failed! " <<endl;
        return false;
    }
#endif
    close(fd);

    return true;
}

bool Font::enroll(const char *text) {
    if (nullptr == text) {
        return false;
    }
    return enroll(std::string(text));
}

bool Font::enroll(const std::string text) {
    bool success = true;
    if (text.length() < 1) {
        return false;
    }

    for (auto it = text.begin(); it != text.end(); it++) {
        if (!enroll(*it)) {
            success = false;
        }
    }

    return success;
}

bool Font::enroll(char character) {
    FontInfo fontInfo;

    auto it = font_map_.find(character);
    if (font_map_.end() != it) {
//        cout <<"character:" <<std::string(1, character) <<" already enrolled!" <<endl;
        return true;
    }

    if (0 != FT_Load_Char(face, character, FT_LOAD_DEFAULT)) {
        cout <<"FT_Load_Char failed!" <<endl;
        return false;
    }

    if (0 != FT_Get_Glyph(face->glyph, &glyph))  {
        cout <<"FT_Set_Pixel_Sizes failed!" <<endl;
        return false;
    }

    FT_GlyphSlot& slot = face->glyph;
    if (FT_GLYPH_FORMAT_BITMAP != glyph->format) {
        if (0 != FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL)) {
            cout << "FT_Glyph_To_Bitmap failed!" <<endl;
            return false;
        }
    }

    FT_Bitmap& bitmap = slot->bitmap;
    FT_Glyph_Metrics& metrics = slot->metrics;

    fontInfo.horiBearingX_ = metrics.horiBearingX >> 6;
    fontInfo.horiBearingY_ = metrics.horiBearingY >> 6;
    fontInfo.horiAdvance_ = metrics.horiAdvance >> 6;
    fontInfo.bitmap_.height_ = metrics.height >> 6;
    fontInfo.bitmap_.width_ = metrics.width >> 6;
    fontInfo.bitmap_.len_ = fontInfo.bitmap_.width_ * fontInfo.bitmap_.height_;
    fontInfo.bitmap_.addr_ = new RGB8BPPBox[fontInfo.bitmap_.len_];

    std::memcpy(fontInfo.data(), bitmap.buffer, fontInfo.length());
    font_map_.insert(std::make_pair(character, fontInfo));

    return true;
}

int32_t Font::getMaxBeringY(std::string time_string) {
    int32_t max_bering_y = -1;
    for (auto ts_it = time_string.begin(); ts_it != time_string.end(); ts_it++) {
        auto font_it = font_map_.find(*ts_it);
        if (font_it != font_map_.end()) {
            max_bering_y = std::max(max_bering_y, font_it->second.horiBearingY_);
        }
    }

    return max_bering_y;
}

int32_t Font::getMaxHeight(std::string time_string) {
    int32_t max_height = -1;
    int32_t max_bering_y = getMaxBeringY(time_string);

    if (max_bering_y < 1) {
        return max_height;
    }

    for (auto ts_it = time_string.begin(); ts_it != time_string.end(); ts_it++) {
        auto font_it = font_map_.find(*ts_it);
        auto height = max_bering_y - font_it->second.horiBearingY_ + font_it->second.bitmap_.height_;
        if (font_it != font_map_.end()) {
            max_height = std::max(max_height, height);
        }
    }

    return max_height;
}

int32_t Font::getMaxWidth(std::string time_string) {
    int32_t max_width = -1;
    for (auto ts_it = time_string.begin(); ts_it != time_string.end(); ts_it++) {
        auto font_it = font_map_.find(*ts_it);
        if (font_it != font_map_.end()) {
            max_width = std::max(max_width, font_it->second.horiAdvance_);
        }
    }

    return max_width;
}

int32_t Font::getTotalWidth(std::string time_string) {
    int32_t total_width = 0;
    int32_t max_width = getMaxWidth(time_string);

    if (max_width < 1) {
        return total_width;
    }

    for (auto ts_it = time_string.begin(); ts_it != time_string.end(); ts_it++) {
        auto font_it = font_map_.find(*ts_it);
        if (font_it != font_map_.end()) {
            total_width += font_it->second.horiAdvance_;
        }
    }

    return total_width;
}

bool Font::createBitmap(int32_t total_width, int32_t max_height, PixelFormat pixel_format) {
    switch (pixel_format) {
        case PixelFormat::RGB8BPP : {
            bytes_per_pixel_ = sizeof(RGB8BPPBox);
            break;
        }

        default: {
            return false;
        }
    }

    if (nullptr != image.addr_) {
        destroyBitmap();
    }

    auto len_ = total_width * max_height * bytes_per_pixel_;
    image.len_ = len_;
    image.width_ = total_width;
    image.height_ = max_height;
    image.addr_ = new RGB8BPPBox[len_];
    std::memset(image.addr_, 0, len_);

    return true;
}

void Font::destroyBitmap() {
    for (auto it = font_map_.begin(); it != font_map_.end(); ) {
        delete [] (it->second.bitmap_.addr_);
        it = font_map_.erase(it);
    }

    if (nullptr != image.addr_) {
        delete [] (image.addr_);
        image.addr_ = nullptr;
    }

    if (nullptr != argb1555_bitmap) {
        delete[](argb1555_bitmap);
        argb1555_bitmap = nullptr;
    }
}

int64_t Font::convert_to_argb1555() {
    if (nullptr != argb1555_bitmap) {
        delete[](argb1555_bitmap);
    }
    argb1555_bitmap = new ARGB1555Box[image.width_ * image.height_];
    std::memset(argb1555_bitmap, 0, image.width_ * image.height_ * sizeof(ARGB1555Box));

    for (int32_t row = 0; row < image.height_; row++) {
        for (int32_t col = 0; col < image.width_; col++) {
//            ARGB1555Box &box = argb1555_bitmap[row * image.width_ + col];
//            box.red = image.addr_->red << 2;
//            box.green = image.addr_->green <<2;
//            box.blue = image.addr_->blue <<3;
//            box.alpha = true;
        }
    }

    return (image.width_ * image.height_ *sizeof(ARGB1555Box));
}

int64_t Font::convert(Font::PixelFormat pixel_format) {
    switch (pixel_format) {
        case PixelFormat::ARGB1555: {
            return convert_to_argb1555();
        }

        default: {
            return -1;
        }
    }
}
