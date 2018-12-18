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

Font::Font(std::string path){
    runtimeAssert("such file not exist!", (path.length() > 1) && (0 == access(path.c_str(), F_OK)));
    runtimeAssert("FT_Init_FreeType failed!", 0 == FT_Init_FreeType(&library_));
    runtimeAssert("FT_New_Face failed!", 0 == FT_New_Face(library_, path.c_str(), 0, &face_));
    setFontSize(30);
}

Font::~Font() {
    FT_Done_Face(face_);
    FT_Done_FreeType(library_);
}

bool Font::setFontSize(int32_t font_size) {
    return (0 == FT_Set_Pixel_Sizes(face_, 0, font_size));
}

std::shared_ptr<FontBitmap> Font::toBitmapMem(std::string time_string) {
    if (!enroll(time_string)) {
        return nullptr;
    }

    max_height_ = getMaxHeight(time_string);
    total_width_ = getTotalWidth(time_string);

    max_height_ = align(max_height_, 2);
    total_width_ = align(total_width_, 2);
    int32_t max_horizontal_bering_Y = getMaxBeringY(time_string);
    int32_t start_byte = 0;
    int32_t bytes_per_copy = 0;
    int32_t copied_horizontal_byte = 0;

    if (max_height_ < 0 || max_horizontal_bering_Y < 0 || 0 == total_width_) {
        return nullptr;
    }
    src_img_ = std::make_shared<FontBitmap>(total_width_, max_height_, PixelFormat::RGB8BPP);

    int32_t stride = total_width_ * src_img_->bytesPerPixel();
    for (auto character = time_string.begin(); character != time_string.end(); character++) {
        auto it = font_map_.find(*character);
        if (it != font_map_.end()) {
            auto start_row = max_horizontal_bering_Y - it->second.horiBearingY_;
            auto char_font_buf = it->second.bitmap_->data();
            auto char_font_height = it->second.bitmap_->height();
            bytes_per_copy = it->second.bitmap_->stride();
            start_byte = copied_horizontal_byte + it->second.horiBearingX_;

            for (int32_t row = 0; row < max_height_; row++) {
                if (0 == bytes_per_copy) {
                    break;
                }

                if (row >= start_row && row < (start_row + char_font_height)) {
                    std::memcpy(src_img_->data() + start_byte, char_font_buf + (row - start_row) * bytes_per_copy, bytes_per_copy);
                }
                start_byte += stride;
            }
            copied_horizontal_byte += it->second.horiAdvance_;
        } else {
            cout <<"character:" <<*character <<" not found!" <<endl;
            return nullptr;
        }
    }
    std::cout <<"total_width_:" <<total_width_ <<", max_height_:" <<max_height_ <<std::endl;

    return src_img_;
}

bool Font::toBitmapFile(std::string fname, std::string time_string, PixelFormat pixel_format) {
    int fd = open(fname.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if (fd < 0) {
        cout <<"open failed! " <<fname <<", " <<strerror(errno) <<endl;
        return false;
    }

    auto src_img = toBitmapMem(time_string);
    if (nullptr == src_img) {
        return false;
    }

    auto dst_img = convert(pixel_format);
    if (nullptr == dst_img) {
        return false;
    }

    auto num = write(fd, dst_img->data(), dst_img->size());
    if (num != dst_img->size()) {
        cout<<"write failed! " <<endl;
        return false;
    }

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

    if (0 != FT_Load_Char(face_, character, FT_LOAD_DEFAULT)) {
        cout <<"FT_Load_Char failed!" <<endl;
        return false;
    }

    if (0 != FT_Get_Glyph(face_->glyph, &glyph_))  {
        cout <<"FT_Set_Pixel_Sizes failed!" <<endl;
        return false;
    }

    FT_GlyphSlot& slot = face_->glyph;
    if (FT_GLYPH_FORMAT_BITMAP != glyph_->format) {
        if (0 != FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL)) {
            cout << "FT_Glyph_To_Bitmap failed!" <<endl;
            return false;
        }
    }

    FT_Bitmap& bitmap = slot->bitmap;
    FT_Glyph_Metrics& metrics = slot->metrics;
    int32_t height = metrics.height >> 6;
    int32_t width = metrics.width >> 6;
    fontInfo.horiBearingX_ = metrics.horiBearingX >> 6;
    fontInfo.horiBearingY_ = metrics.horiBearingY >> 6;
    fontInfo.horiAdvance_ = metrics.horiAdvance >> 6;

    fontInfo.bitmap_ = std::make_shared<FontBitmap>(width, height, PixelFormat::RGB8BPP);
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
        auto height = max_bering_y - font_it->second.horiBearingY_ + font_it->second.bitmap_->height();
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

std::shared_ptr<FontBitmap> Font::rgb8pp_to_argb1555() {
    dst_img_ = std::make_shared<FontBitmap>(total_width_, max_height_, PixelFormat::ARGB1555);
    ARGB1555Pixel* argb1555_bitmap = reinterpret_cast<ARGB1555Pixel*>(dst_img_->data());
    RGB8BPPPixel* rgb8pp_bitmap = reinterpret_cast<RGB8BPPPixel*>(src_img_->data());

    for (int32_t row = 0; row < max_height_; row++) {
        for (int32_t col = 0; col < total_width_; col++) {
            ARGB1555Pixel &argb1555_pixel = argb1555_bitmap[row * total_width_ + col];
            RGB8BPPPixel &rgb8pp_box_pixel = rgb8pp_bitmap[row * total_width_ + col];
            argb1555_pixel.red = rgb8pp_box_pixel.red << 2 | (rgb8pp_box_pixel.red >> 1);
            argb1555_pixel.green = rgb8pp_box_pixel.green <<2 | (rgb8pp_box_pixel.green >> 1);
            argb1555_pixel.blue = rgb8pp_box_pixel.blue <<3 | (rgb8pp_box_pixel.blue << 1);
            argb1555_pixel.alpha = true;
        }
    }

    return dst_img_;
}

std::shared_ptr<FontBitmap> Font::convert(PixelFormat pixel_format) {
    switch (pixel_format) {
        case PixelFormat::ARGB1555: {
            return rgb8pp_to_argb1555();
        }

        case PixelFormat::RGB8BPP: {
            dst_img_ = src_img_;
            return dst_img_;
        }

        default: {
            runtimeAssert("Not support to convert to such pixel format!", ErrorCode::notSupport);
        }
    }
    return nullptr;
}
