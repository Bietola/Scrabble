#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <list>

#include "constants.h"
#include "Deck.h"

///Player class
class Player{
    public:
        ///friends
        friend class TurnHandler;
        ///aliases
        using hand_t = std::list<char>;
        ///enumerations
        enum contr_t {
            CONTR_PLAYER,
            CONTR_CPU
        };
        ///constructor
        Player(const std::string& name, contr_t contr);
        ///getters
        //get name
        const std::string& getName() const {return mName;}
        //get contr
        contr_t getContr() const {return mContr;}
        //get hand
        const hand_t& getHand() const {return mHand;}
        hand_t& getHand() {return mHand;}
        ///functions
        //getters
        unsigned int getPoints() const {return mPoints;}
        //setters
        void setPoints(unsigned int points) {mPoints = points;}
        //hand management
        bool replenishHand(Deck& deck);
        bool addToHand(char tile);
        bool removeFromHand(char tile);
        bool isHandEmpty()const {return mHand.empty();}
    private:
        ///variables
        //hand
        std::list<char> mHand;
        //points
        unsigned int mPoints = 0;
        //name
        std::string mName;
        //control type
        contr_t mContr;
};

#endif // PLAYER_H_INCLUDED
