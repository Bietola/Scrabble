#ifndef INTERTILE_H_INCLUDED
#define INTERTILE_H_INCLUDED

#include <iostream>
#include <string>
#include <map>

#include <SFML/Graphics.hpp>

#include "Grappa/interface/Button.h"
#include "Grappa/interface/PictureBox.h"
#include "TileSheet.h"

///InterTile class
class InterTile: public gr::Button{
    public:
        ///constructor
        InterTile(const TileSheet& tileSheet, char symbol);
        ///destructor
        ;
        ///getters
        char getSymbol() const {return mSymbol;}
    private:
        ///variables
        //symbol
        char mSymbol;
};


#endif //TILE_H_INCLUDED
