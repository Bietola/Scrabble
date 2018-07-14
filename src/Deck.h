#ifndef DECK_H_INCLUDED
#define DECK_H_INCLUDED

#include <cassert>
#include <vector>
#include <algorithm>

#include "constants.h"

///Deck class
class Deck{
    public:
        ///constructor
        Deck();
        ///other functions
        //initialization based on constants
        void replenishCards();
        //shuffle deck
        void shuffle();
        //remove and return next tile in deck - return ' ' if deck is empty
        char fishNextTile();
    //!private:
        ///variables
        //cards
        std::vector<char> mCards;
};

#endif // DECK_H_INCLUDED
