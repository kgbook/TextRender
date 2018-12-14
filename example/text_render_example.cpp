#include <iostream>
#include <string>
#include "font.h"
#include "time.h"
#include "exception.h"

int main(int argc, char**  argv )
{
    try {
        Time* time = Time::getInstance();
        std::string time_str = time->getTimeString();
        Font* font = new Font(std::string("../../font/TimesNewRoman.ttf"));

        std::cout <<time_str <<std::endl;
        if (!font->setFontSize(60)) {
            std::cout <<"setFontSize failed!" <<std::endl;
            return -1;
        }

        if (!font->toBitmapFile("font.bitmap", time_str, PixelFormat::ARGB1555)) {
            std::cout <<"toBitmapFile failed!" <<std::endl;
        }
    } catch (Exception::RuntimeError &err) {
        std::cout <<err.what() <<std::endl;
    } catch (...) {
        std::cout <<"Undefined error happened!" <<std::endl;
    }

    return 0;
}