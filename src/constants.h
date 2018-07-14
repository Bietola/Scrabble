#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

#include <map>

///global constants
//maximum cards kept in hand at any given time
const unsigned int HAND_SIZE = 8;
//number of letters in the alphabet
const unsigned int NUMBER_OF_LETTERS = 26;
//tiles put in deck when game starts
const std::map<char,unsigned int> DECK_TILE_NUMBERS = {
    /*{'A', 50},
    {'B', 50},
    {'C', 50},
    {'D', 0},
    {'E', 0},
    {'F', 0},
    {'G', 0},
    {'H', 0},
    {'I', 0},
    {'J', 0},
    {'K', 0},
    {'L', 0},
    {'M', 0},
    {'N', 0},
    {'O', 0},
    {'P', 0},
    {'Q', 0},
    {'R', 0},
    {'S', 0},
    {'T', 0},
    {'U', 0},
    {'V', 0},
    {'W', 0},
    {'X', 0},
    {'Y', 0},
    {'Z', 0}*/
    {'A', 12},
    {'B', 4},
    {'C', 7},
    {'D', 4},
    {'E', 12},
    {'F', 4},
    {'G', 4},
    {'H', 2},
    {'I', 12},
    {'J', 0},
    {'K', 0},
    {'L', 6},
    {'M', 6},
    {'N', 6},
    {'O', 12},
    {'P', 4},
    {'Q', 2},
    {'R', 7},
    {'S', 7},
    {'T', 7},
    {'U', 1},
    {'V', 4},
    {'W', 0},
    {'X', 0},
    {'Y', 0},
    {'Z', 2},
    {'$', 2}
};
//points yielded by each individual letter
const std::map<char,unsigned int> LETTERS_POINTS = {
    {'A', 1},
    {'B', 4},
    {'C', 1},
    {'D', 4},
    {'E', 1},
    {'F', 4},
    {'G', 4},
    {'H', 8},
    {'I', 1},
    {'J', 0},
    {'K', 0},
    {'L', 2},
    {'M', 2},
    {'N', 2},
    {'O', 1},
    {'P', 2},
    {'Q', 10},
    {'R', 1},
    {'S', 1},
    {'T', 1},
    {'U', 4},
    {'V', 4},
    {'W', 0},
    {'X', 0},
    {'Y', 0},
    {'Z', 8},
    {'$', 0}
};

#endif // CONSTANTS_H_INCLUDED
