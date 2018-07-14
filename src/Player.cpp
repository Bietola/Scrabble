#include "Player.h"

using std::size_t;

///Player functions
//constructor
Player::Player(const std::string& name, contr_t contr):
    mName(name),
    mContr(contr),
    mHand()
{}
//hand management
bool Player::replenishHand(Deck& deck){
    for(size_t j = mHand.size(); j < HAND_SIZE; j++){
        char tile = deck.fishNextTile();
        if(tile == ' ') return false;
        mHand.push_back(tile);
    }
    //success
    return true;
}
bool Player::addToHand(char tile){
    if(mHand.size() == HAND_SIZE) return false;
    mHand.push_back(tile);
}
bool Player::removeFromHand(char tile){
    for(auto itr = mHand.begin(); itr != mHand.end(); ++itr){
        if(*itr == tile){
            mHand.erase(itr);
            return true;
        }
    }
    return false;
}
