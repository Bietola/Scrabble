#include "Deck.h"

///Deck functions
//constructor
Deck::Deck(){
    replenishCards();
    shuffle();
}
//functions
void Deck::replenishCards(){
    for(const auto& ele : DECK_TILE_NUMBERS){
        for(unsigned int j = 0; j < ele.second; j++){
            mCards.push_back(ele.first);
        }
    }
}
void Deck::shuffle(){
    std::random_shuffle(mCards.begin(), mCards.end());
}
char Deck::fishNextTile(){
    if(mCards.empty()) return ' ';
    char fishedCard = *(mCards.end() - 1);
    mCards.pop_back();
    return fishedCard;
}
