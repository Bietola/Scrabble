#include "InterTile.h"

///InterTile functions
//constructor
InterTile::InterTile(const TileSheet& tileSheet, char symbol):
    Button()
{
    //generate clip rectangle from symbol and tile sheet
    mSymbol = symbol;
    auto clipRect = tileSheet.makeClipRect(mSymbol);
    if(clipRect.top == -1){
        std::cout << "ERROR: could not get clip rectangle from tile sheet for tile (symbol: " << symbol << ")." << std::endl;
        return;
    }
    //aust tile size based on clip
    setBounds(sf::FloatRect(0, 0, clipRect.width, clipRect.height));
    //set up tile sprite according to generated clip rect
    addRelative<gr::PictureBox>("image", sf::FloatRect(0, 0, 1, 1), tileSheet.getTexture(), clipRect);
}
