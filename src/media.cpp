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
        {"letter_tile_bg", "art/letter_tile_bg.png"},
        {"letter_bonus_bg", "art/letter_bonus_bg.png"},
        {"word_bonus_bg", "art/word_bonus_bg.png"},
        {"empty_tile", "art/empty_tile.png"},
        {"start_bonus_tile", "art/start_bonus_tile.png"},
        {"no_graphics", "art/no_graphics.png"},
        {"beetle", "art/beetle.png"},

        {"interface_bg", "art/interface/bg.jpg"},
        {"interface_button", "art/interface/kenney_button.png"},
        {"interface_panel", "art/interface/kenney_panel.png"},
        {"interface_frame", "art/interface/steam_frame.png"}
    });
    //load fonts
    success &= gFontProvider.loadMedia({
        {"tile_letters", "fonts/LinLibertine_DRah.ttf"},
        {"bonus_text", "fonts/GoodDog.otf"}
    });
    //return
    return success;
}
