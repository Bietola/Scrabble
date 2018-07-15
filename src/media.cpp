#include "media.h"

///global objects and variables
//media providers
MediaProvider<sf::Texture> gTextureProvider;
MediaProvider<sf::Font> gFontProvider;

///global functions
//load all media, return false on failure
bool load_media(){
    //success/failure flag
    bool success = true;
    //load textures
    success &= gTextureProvider.loadMedia({
        {"letter_tile_bg", "assets/art/letter_tile_bg.png"},
        {"letter_bonus_bg", "assets/art/letter_bonus_bg.png"},
        {"word_bonus_bg", "assets/art/word_bonus_bg.png"},
        {"empty_tile", "assets/art/empty_tile.png"},
        {"start_bonus_tile", "assets/art/start_bonus_tile.png"},
        {"no_graphics", "assets/art/no_graphics.png"},
        {"beetle", "assets/art/beetle.png"},

        {"interface_bg", "assets/art/interface/bg.jpg"},
        {"interface_button", "assets/art/interface/kenney_button.png"},
        {"interface_frame", "assets/art/interface/steam_frame.png"}
    });
    //load fonts
    success &= gFontProvider.loadMedia({
        {"tile_letters", "assets/fonts/LinLibertine_DRah.ttf"},
        {"bonus_text", "assets/fonts/GoodDog.otf"}
    });
    //return
    return success;
}
