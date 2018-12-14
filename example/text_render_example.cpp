#include <iostream>
#include "font.h"
#include "exception.h"

int main(int argc, char**  argv )
{
    try {
        Font* font = new Font(std::string("../../font/TimesNewRoman.ttf"));

        if (!font->setFontSize(60)) {
            std::cout <<"setFontSize failed!" <<std::endl;
            return -1;
        }

        if (!font->toBitmapFile("font.bitmap", "2018-12-12 16:11:30", PixelFormat::ARGB1555)) {
            std::cout <<"toBitmapFile failed!" <<std::endl;
        }
    } catch (Exception::RuntimeError &err) {
        std::cout <<err.what() <<std::endl;
    } catch (...) {
        std::cout <<"Undefined error happened!" <<std::endl;
    }

    return 0;
}