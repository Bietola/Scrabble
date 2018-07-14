#ifndef TILESHEET_H_INCLUDED
#define TILESHEET_H_INCLUDED

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <SFML/Graphics.hpp>

#include "constants.h"
#include "media.h"

#include "Grappa/interface/Button.h"

#include "Grappa/MediaProvider.h"
#include "BoardTiles.h"

///TileSheet class
class TileSheet{
    public:
        ///constants and enumerations
        //number of tiles with letters on them
        static const unsigned int LETTER_TILES_NUMBER = 26;
        //number of letter bonus point tiles
        static const unsigned int LETTER_BONUS_POINTS_TILES_NUMBER = 5;
        //number of word bonus points tiles
        static const unsigned int WORD_BONUS_POINTS_TILES_NUMBER = 5;
        //starting index of letter tiles
        static const unsigned int LETTER_TILES_SI = 0;
        //starting index of letter bonus points tiles
        static const unsigned int LETTER_BONUS_POINTS_TILES_SI = LETTER_TILES_NUMBER;
        //starting index of word bonus points tiles
        static const unsigned int WORD_BONUS_POINTS_TILES_SI = LETTER_BONUS_POINTS_TILES_SI + LETTER_BONUS_POINTS_TILES_NUMBER;
        //starting index of special tiles
        static const unsigned int SPECIAL_TILES_SI = WORD_BONUS_POINTS_TILES_SI + WORD_BONUS_POINTS_TILES_NUMBER;
        //special tiles indexes
        enum special_tile_index_t{
            SPECIALT_EMPTY = SPECIAL_TILES_SI,
            SPECIALT_START,
            SPECIALT_WILD,
            SPECIALT_NOGRAPHIC,
            SPECIALT_MAX
        };
        //total number of tiles
        static const unsigned int TOTAL_TILES_NUMBER = SPECIAL_TILES_SI + SPECIALT_MAX;
        ///constructor
        TileSheet(){};
        ///texture generation functions
        //deduce tile position within tile sheet given the tile index
        sf::Vector2i makeTilePositionFromIndex(unsigned int index) const;
        //generate tile sheet texture
        bool generate(unsigned int elementsInRow, unsigned int tileDimension);
        ///getters
        const sf::Texture& getTexture() const {return mTexture;}
        unsigned int getTileDimension() const {return mTileDimension;}
        ///clip rectangle converters (they all return a clip rectangle to the NO_GRAPHIC special tile on failure)
        //return clip rectangle corresponding to given symbol
        sf::IntRect makeClipRect(char symbol) const;
        //make clip rectangle corresponding to given points tile
        sf::IntRect makeClipRect(const PointsTile& pointsTile) const;
    private:
        ///static variables
        //flag for keeping track of tile sheet generation
        bool sIsGenerated = false;
        ///variables
        //generated texture
        sf::Texture mTexture;
        //maximum elements put in a consecutive row while generating the tile sheet (used for optimizing texture space)
        unsigned int mElementsInRow = 6;
        //tile dimensions (the bg texture will be stretched to this)
        unsigned int mTileDimension = 100;
        ///texture generation functions
        //return a sprite representing the background of a "tile located in the tile sheet at a given index"
        sf::Sprite makeTileBgSprite(const sf::Texture& texture, unsigned int index) const;
        //return a sfml text object representing the letter of a ""
        sf::Text makeTileLetterText(const sf::Font& font, char letter, unsigned int index) const;
        //return "" representing the points given by a ""
        sf::Text makeTilePointsText(const sf::Font& font, unsigned int points, unsigned int index) const;
        //return "" representing the points and the type of bonus
        sf::Text makeTileBonusText(const sf::Font& font, PointsTile::bonus_t bonus, unsigned int points, unsigned int index) const;
};
#endif // TILESHEET_H_INCLUDED
