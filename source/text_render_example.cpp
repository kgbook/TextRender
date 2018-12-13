#include <iostream>
#include "font.h"

int main(int argc, char**  argv )
{
    Font* font = new Font(std::string("../font/TimesNewRoman.ttf"));

    if (!font->setFontSize(60)) {
        std::cout <<"setFontSize failed!" <<std::endl;
        return -1;
    }

    if (!font->toBitmapFile("font.bitmap", "2018-12-12 16:11:30")) {
        std::cout <<"toBitmapFile failed!" <<std::endl;
    }

    return 0;
}