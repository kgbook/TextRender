#include <cstdio>
#include <cstring>
#include <cmath>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>
#include <algorithm>
#include <string>
#include <iostream>
#include <cerrno>
#include <list>
#include <algorithm>
#include <memory>

#include <ft2build.h>
#include FT_FREETYPE_H
 
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>

using std::cout;
using std::endl;

unsigned char *image;

struct FontInfo {
    uint8_t *addr;

    int32_t width;

    int32_t height;

    int32_t len;

    int32_t horiBearingX;

    int32_t horiBearingY;

    int32_t horiAdvance;

    char character;
};

std::list<FontInfo> fontList;

std::list<FontInfo>::iterator find(char character) {
    for (auto it = fontList.begin(); it != fontList.end(); it++) {
        if (it->character == character) {
            return it;
        }
    }
    return fontList.end();
}

void insert(FT_GlyphSlot slot, char character) {
    FontInfo fontInfo;

    if (fontList.end() != find(character)) {
        cout <<"character:" <<std::string(1, character) <<" already register!" <<endl;
        return ;
    }

    FT_Bitmap& bitmap = slot->bitmap;
    FT_Glyph_Metrics& metrics = slot->metrics;

    fontInfo.character = character;
    fontInfo.horiBearingX = metrics.horiBearingX >> 6;
    fontInfo.horiBearingY = metrics.horiBearingY >> 6;
    fontInfo.height = metrics.height >> 6;
    fontInfo.width = metrics.width >> 6;
    fontInfo.horiAdvance = metrics.horiAdvance >> 6;
    fontInfo.len = fontInfo.height * fontInfo.width;
    fontInfo.addr = new uint8_t[fontInfo.len];

    std::memcpy(fontInfo.addr, bitmap.buffer, fontInfo.len);
    fontList.push_back(fontInfo);
}

void destory() {
    for (auto it = fontList.begin(); it != fontList.end(); ) {
        delete [] it->addr;
        it = fontList.erase(it);
    }
    delete [] image;
}

int32_t getMaxBeringY(std::string time_string) {
    int32_t maxBeringY = -1;
    for (auto ts_it = time_string.begin(); ts_it != time_string.end(); ts_it++) {
        auto font_it = find(*ts_it);
        if (font_it != fontList.end()) {
            maxBeringY = std::max(maxBeringY, font_it->horiBearingY);
        }
    }

    return maxBeringY;
}

int32_t getMaxHeight(std::string time_string) {
    int32_t maxHeight = -1;
    int32_t maxBeringY = getMaxBeringY(time_string);

    if (maxBeringY < 1) {
        return maxHeight;
    }

    for (auto ts_it = time_string.begin(); ts_it != time_string.end(); ts_it++) {
        auto font_it = find(*ts_it);
        auto height = maxBeringY - font_it->horiBearingY + font_it->height;
        if (font_it != fontList.end()) {
            maxHeight = std::max(maxHeight, height);
        }
    }

    return maxHeight;
}

int32_t getMaxWidth(std::string time_string) {
    int32_t maxWidth = -1;
    for (auto ts_it = time_string.begin(); ts_it != time_string.end(); ts_it++) {
        auto font_it = find(*ts_it);
        if (font_it != fontList.end()) {
            maxWidth = std::max(maxWidth, font_it->horiAdvance);
        }
    }

    return maxWidth;
}

int32_t getTotalWidth(std::string time_string) {
    int32_t totalWidth = 0;
    int32_t maxWidth = getMaxWidth(time_string);

    if (maxWidth < 1) {
        return totalWidth;
    }

    for (auto ts_it = time_string.begin(); ts_it != time_string.end(); ts_it++) {
        auto font_it = find(*ts_it);
        if (font_it != fontList.end()) {
            totalWidth += font_it->horiAdvance;
        }
    }

    return totalWidth;
}

int32_t toBitmapMem(std::string time_string) {
    int32_t maxHeight = getMaxHeight(time_string);
    int32_t totalWidth = getTotalWidth(time_string);
    int32_t maxHoriBeringY = getMaxBeringY(time_string);
    int32_t bytesPerPixel = 1; //8bpp
    int32_t stride = totalWidth * bytesPerPixel;
    int32_t startByte = 0;
    int32_t bytesPerCopy = 0;
    int32_t copydRowBytes = 0;

    if (maxHeight < 0 || maxHoriBeringY < 0 || 0 == totalWidth) {
        return -1;
    }

    image = new unsigned char [maxHeight * totalWidth];
    std::memset(image, 0, maxHeight * totalWidth);
    for (auto var = time_string.begin(); var != time_string.end(); var++) {
        auto it = find(*var);
        auto startRow = maxHoriBeringY - it->horiBearingY;
        bytesPerCopy = it->width * bytesPerPixel;
        startByte = copydRowBytes + it->horiBearingX;

        if (it != fontList.end()) {
            for (int32_t row = 0; row < maxHeight; row++) {
                if (0 == bytesPerCopy) {
                    break;
                }

                if (row >= startRow && row < (startRow + it->height)) {
                    std::memcpy(image + startByte, it->addr + (row - startRow) * bytesPerCopy, bytesPerCopy);
                }
                startByte += stride;
            }
            copydRowBytes += it->horiAdvance;
        } else {
            cout <<"character:" <<*var <<" not found!" <<endl;
            return -1;
        }
    }
    return (maxHeight * stride);
}

bool toBitmapFile(std::string fname, std::string time_string) {
    int fd = open(fname.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if (fd < 0) {
        cout <<"open failed! " <<fname <<", " <<strerror(errno) <<endl;
        return false;
    }

    int32_t len = toBitmapMem(time_string);
    if (len < 0) {
        return false;
    }

    int32_t num = write(fd, image, len);
    if (num != len) {
        cout<<"write failed!" <<endl;
        return false;
    }
    close(fd);

    return true;
}

int main(int argc, char**  argv )
{
    FT_Library  library;
    FT_Face     face;
    FT_Glyph    glyph;
    std::string charTable = "1234567890:- ABCDEFGabcdefg";
    std::string time_str = "2018-12-05 20:11:00 ABCDEFGabcdefg";
    std::string fname = "font.bitmap";
    FT_UInt fontSize = 64;

    if (0 != FT_Init_FreeType(&library)){
        cout <<"FT_Init_FreeType failed" <<endl;
        return - 1 ;  
    }  

    if (0 != FT_New_Face(library,  "../font/TimesNewRoman.ttf" ,  0,  &face)) {
        cout <<"FT_New_Face failed" <<endl;
        return -1;
    }

    if (0 != FT_Set_Pixel_Sizes(face, 0, fontSize)) {
        cout <<"FT_Set_Pixel_Sizes failed" <<endl;
        return -1;
    }

    for (auto it = charTable.begin(); it != charTable.end(); it++) {
        if (0 != FT_Load_Char(face, *it, FT_LOAD_DEFAULT)) {
            cout <<"FT_Load_Char failed!" <<endl;
            return -1;
        }

        if (0 != FT_Get_Glyph(face->glyph, &glyph))  {
            cout <<"FT_Set_Pixel_Sizes failed!" <<endl;
            return -1;
        }

        if (FT_GLYPH_FORMAT_BITMAP != glyph->format) { //NOT FT_GLYPH_FORMAT_BITMAP!
            if (0 != FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL)) {
                cout << "FT_Glyph_To_Bitmap failed!" <<endl;
                return -1;
            }
        }

        FT_GlyphSlot slot = face->glyph;
        insert(slot, *it);
    }

    FT_Done_Face(face);
    FT_Done_FreeType(library);

    toBitmapFile(fname, time_str);
    destory();

    return 0;
}