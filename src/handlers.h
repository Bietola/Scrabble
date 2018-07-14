#ifndef HANDLERS_H_INCLUDED
#define HANDLERS_H_INCLUDED

#include <cassert>
#include <string>
#include <vector>
#include <sstream>

#include <SFML/Graphics.hpp>

#include "media.h"
#include "Grappa/utils.h"
#include "constants.h"

#include "InterTile.h"
#include "InterBoard.h"
#include "Player.h"
#include "Grappa/MediaProvider.h"
#include "Grappa/interface/Interface.h"
#include "Grappa/interface/Button.h"
#include "Grappa/interface/PictureBox.h"
#include "Grappa/interface/TextBox.h"

#include "Grappa/Handler.h"

///TurnHandler class
class TurnHandler: public Handler{
    public:
        ///enumerations
        //validation state of board
        enum board_validation_state_t {
            BVS_NONE,
            BVS_INVALID,
            BVS_VALID,
            BVS_VALIDATED
        };
        ///constructor
        TurnHandler(const TileSheet& mTileSheet, sf::RenderWindow& window,
                    Board&& board, std::string playersFilePath);
        ///destructor
        ~TurnHandler();
        ///initialization functions
        //initialize player interface
        void initializePlayerInterface();
        //initialize players
        void loadPlayerInfo(std::string playersFilePath);
        ///gameplay functions
        //executes every time the board button is updated
        void boardButtonFunction(gr::Button& but);
        //clean up current turn and initialize next one (each turn is composed
        //of a dynamically allocated custom player UI based on the player's
        //current hand)
        void nextTurn();
        //end the game and switch game handler
        void endGame(const Player* endingPlayer);
        ///ui functions
        //handle user event
        void handleEvent(const sf::Event& e) override;
        //handle logical operations
        void handleLogic(const sf::Time& time) override;
        //draw interface
        void handleDrawing() const override;
    private:
        ///interface variables
        //dimension (width and height) of interface tiles
        float mInterTileDimension;
        //board interface
        InterBoard mInterBoard;
        //player UI interface
		gr::Interface* mPlayerInterface = nullptr;
        //text output used for printing game info
		gr::TextBox* mMsgOutput = nullptr;
        ///other variables
        //deck used by turn hander
        Deck mDeck;
        //tile that is currently being moved around by the cursor
        InterTile* mMovingTile = nullptr;
        //players playing the game
        std::vector<Player> mPlayers;
        //player currently handled
        std::vector<Player>::iterator mCurrentPlayer;
        //total points made in the current turn
        unsigned int mTurnPoints = 0;
        //turns elapsed since the begging of the game
        unsigned int mTurn = 0;
        //variable to keep track of the validation process of the board
        board_validation_state_t mBoardValidationState = BVS_NONE;
        //tile sheet used to render tiles
        const TileSheet& mTileSheet;
        ///interface generation functions
        //pick a random position for inter tiles to be in (within the space in which they are put when they're not moved)
        sf::Vector2f pickInterTileRandomPosition() const;
        //adapt an object of base type sf::Transformable to fit into the player user interface
        void adaptTransformableToInterface(sf::Transformable& transformable, sf::FloatRect bounds, const sf::FloatRect& windowFractions) const;
        ///logic handling functions
        //attempt to set button as currently active tile
        void setButtonAsMovingTile(gr::Button& button); //!DIRTY
        //move currently selected tile around
        void moveMovingTile();
        //returns the current text output used for messages, asserting if it is null
		gr::TextBox* getMsgOutput() const {assert(mMsgOutput);return mMsgOutput;}
};

///EndScreenHandler class
class EndScreenHandler: public Handler{
    public:
        ///constructor
        EndScreenHandler(sf::RenderWindow& window, std::vector<Player>&& players);
        ///ui functions
        //handle user event
        void handleEvent(const sf::Event& e) override;
        //handle logical operations
        void handleLogic(const sf::Time& time) override;
        //draw interface
        void handleDrawing() const override;
    private:
        ///variables
        sf::Text mEndText;
        std::vector<Player> mPlayers;
};

#endif // HANDLERS_H_INCLUDED
