#include "handlers.h"

#include "Grappa/interface/ButtonFunctions.h"
#include "Grappa/interface/TextBox.h"

///-TurnHandler functions
///constructor
TurnHandler::TurnHandler(const TileSheet& tileSheet, sf::RenderWindow& window,
                         Board&& board, std::string playersFilePath):
    Handler(window),
    mInterTileDimension(),
    mInterBoard(std::move(board), tileSheet),
    mDeck(),
    mPlayers(),
    mTileSheet(tileSheet)
{
    //initialize player interface
    initializePlayerInterface();
    //load player info
    loadPlayerInfo(playersFilePath);
    //prepare first player's UI
    mCurrentPlayer = mPlayers.end() - 1;
    nextTurn();
}

///destructor
TurnHandler::~TurnHandler(){
    //delete player interface
    delete mPlayerInterface;
}

///initializers
//initialize player interface
void TurnHandler::initializePlayerInterface() {
    ///player interface setup
    //set initial parameters of interactive board
    auto bs = std::bind(&InterBoard::getSize, &mInterBoard);
    auto ws = std::bind(&sf::Window::getSize, &mWindow);
    mInterBoard.setOrigin(bs().x/2.f, bs().y/2.f);
    mInterBoard.setScale(ws().y / bs().x, ws().y / bs().y);
    mInterBoard.setPosition(ws().x/2.f, bs().y/2.f);

    //allocate player interface
    mPlayerInterface = new gr::Interface(sf::FloatRect(0, 0, mWindow.getSize().x, mWindow.getSize().y));

    //resources tmps
    const auto& bgTexture = gTextureProvider.getMedia("interface_bg");
    const auto& buttonTexture = gTextureProvider.getMedia("interface_button");
	const auto& buttonFont = gFontProvider.getMedia("tile_letters");
	auto buttonFontSize = 40;

    //create and get options interface
    auto& optionsInter = mPlayerInterface->addRelative<gr::PictureBox>("options", sf::FloatRect(0.f, 0.f, 1.f/4.f, 1.f), bgTexture);

    //add pass button
    optionsInter.addRelative<gr::Button>("pass_button", sf::FloatRect(0, 1.f/6.f, 1.f, 1.f/6.f))
		.addClickFunction([this](gr::Button& but) {
			//pass turn if button is pressed and released
			if(but.wasJustClicked()) nextTurn();
		})
		.addClickFunctionality<gr::Clip>(buttonTexture, gr::Orientation::Horizontal,
			ilist({gr::CLICKST_DEFAULT, gr::CLICKST_CLICKING}))
		.setMargins(sf::FloatRect(0.04, 0.02, 0.04, 0.02))
	.getParent()

    //add undo button
	.addRelative<gr::Button>("undo_button", sf::FloatRect(0, 2.f/6.f, 1.f/2.f, 1.f/6.f))
		.addClickFunction([this](gr::Button& but) {
			//undo board changes if button is pressed and released
			if(but.wasJustClicked()){
				mInterBoard.undoWordSelection();
				mTurnPoints = 0;
			}
		})
		.addClickFunctionality<gr::Clip>(buttonTexture, gr::Orientation::Horizontal,
			ilist({gr::CLICKST_DEFAULT, gr::CLICKST_CLICKING}))
		.setMargins(sf::FloatRect(0.04, 0.02, 0.01, 0.02))
	.getParent()

    //add confirm button
    .addRelative<gr::Button>("confirm_button", sf::FloatRect(1.f/2.f, 2.f/6.f, 1.f/2.f, 1.f/6.f))
		.addClickFunction([this](gr::Button& but) {
			//confirm board changes if button is pressed and released
			if(but.wasJustClicked()) {
				switch(mBoardValidationState) {
					case BVS_NONE:
					case BVS_INVALID:
						getMsgOutput()->setText("No valid word selection made...");
						break;
					case BVS_VALID:
						mInterBoard.enstablishNewLetters();
						getMsgOutput()->setText("Board validated!");
						mBoardValidationState = BVS_VALIDATED;
						break;
					case BVS_VALIDATED:
						getMsgOutput()->setText("Board was already validated this turn...");
						break;
				}
			}
		})
		.addClickFunctionality<gr::Clip>(buttonTexture, gr::Orientation::Horizontal,
			ilist({gr::CLICKST_DEFAULT, gr::CLICKST_CLICKING}))
		.setMargins(sf::FloatRect(0.01, 0.02, 0.04, 0.02))
	.getParent()

    //add hints button
	.addRelative<gr::Button>("hints_button", sf::FloatRect(0, 3.f/6.f, 1.f, 1.f/6.f))
		.addClickFunction([this](gr::Button& but) {
			//pass turn if button is pressed and released
			if(but.wasJustClicked()) {
				//!WIP: print them in a nicer way
				auto bestWordsInfo = mInterBoard.suggestBestWord(*mCurrentPlayer);
				std::stringstream ss;
				const auto& mainWordInfo = bestWordsInfo.getMainWordInfo();
				ss << mainWordInfo.word << " (" << mainWordInfo.position.x << ", " << mainWordInfo.position.y << ")";
				ss << " [" << (mainWordInfo.orientation == O_HORIZONTAL ? 'o' : 'v') << "]\n";
				if(!mainWordInfo.wildPositions.empty()){
					ss << "wild: [";
					for(auto pos : mainWordInfo.wildPositions)
						ss << pos << ", ";
					ss << "]\n";
				}
				getMsgOutput()->setText(ss.str());
			}
		})
		.addClickFunctionality<gr::Clip>(buttonTexture, gr::Orientation::Horizontal,
			ilist({gr::CLICKST_DEFAULT, gr::CLICKST_CLICKING}))
		.setMargins(sf::FloatRect(0.04, 0.02, 0.04, 0.02));

    //add text box
    mMsgOutput = &optionsInter.addRelative<gr::TextBox>("text_box", sf::FloatRect(0.f, 4.f/6.f, 1.f, 1.f/3.f));
    mMsgOutput->setMargins(sf::FloatRect(0.08, 0.02, 0.08, 0.02));
    mMsgOutput->setFont(gFontProvider.getMediaPtr("tile_letters"));
    mMsgOutput->setCharacterSize(30);
    mMsgOutput->setText("Welcome to Scrabble! (by Stefano Montesi).");

    //initialize hand interface
    mPlayerInterface->addRelative<gr::PictureBox>("hand", sf::FloatRect(3.f/4.f, 0.f, 1.f/4.f, 1.f), bgTexture);

    //initialize board interface
    mPlayerInterface->addRelative<gr::Button>("boardButton", sf::FloatRect(1.f/4.f, 0.f, 2.f/4.f, 1.f))
    	.addClickFunction([this] (auto& but) {boardButtonFunction(but);});
}

//loads player info from a given file
void TurnHandler::loadPlayerInfo(std::string filePath) {
    assert(mPlayers.empty());

    //open file to create player array
    std::ifstream playersFile(filePath);
    assert(playersFile);

    //go on until the : symbol
    while(playersFile.get() != ':');

    //read player names and create them
    std::string playerName, playerContrStr;
    while(playersFile >> playerName >> playerContrStr) {
        Player::contr_t playerContr;
        if(playerContrStr == "CPU")
            playerContr = Player::CONTR_CPU;
        else if(playerContrStr == "PLAYER")
            playerContr = Player::CONTR_PLAYER;
        else
            assert(false);
        mPlayers.emplace_back(playerName, playerContr);
    }
}

///gameplay
//function assigned to board button
void TurnHandler::boardButtonFunction(gr::Button& but) {
    ///button functions
    //if board has been clicked, convert coordinates to tile position
    if(but.wasJustClicked()) {
        //get mouse position
        auto posAsTile = mInterBoard.getTileMousePosition(mWindow);

        //don't do anything if pos is out of bounds
        if(mInterBoard.posOutOfBounds(posAsTile)) return;

        //check if tile is empty
        bool tileIsEmpty = (mInterBoard.getLetterTile(posAsTile).symbol == ' ');

        //if an inter-tile is selected...
        if(mMovingTile != nullptr) {
            //if board tile is empty...
            if(tileIsEmpty){
                //set letter at calculated indexes to be equal to the letter of the selected tile
                mInterBoard.setLetterTile(posAsTile, mMovingTile->getSymbol());
                //deactivate selected tile
                mMovingTile->setActive(false);
                //remove tile's symbol from player's hand
                assert(mCurrentPlayer->removeFromHand(mMovingTile->getSymbol()));
                //de-selected selected tile
                mMovingTile = nullptr;
            }
            //if a tile is already there, send moving tile back in random position
            else {
                mMovingTile->setPosition(pickInterTileRandomPosition());
                mMovingTile = nullptr;
            }
        }
        //if an inter tile is not selected...
        else {
            //if the tile on the board is not empty...
            if(!tileIsEmpty) {
                //if points were not already made this turn, attempt word validation
                if(mBoardValidationState != BVS_VALIDATED) {
                    //clear previous selection
                    mInterBoard.undoWordSelection();
                    mTurnPoints = 0;

                    //validate word, highlight board and print errors
                    auto validatedWordsInfo = mInterBoard.findValidWords(posAsTile);
                    mInterBoard.highlightWords(validatedWordsInfo);
                    if(validatedWordsInfo.hasError()){
                        getMsgOutput()->setText(validatedWordsInfo.makeErrorString());
                        mBoardValidationState = BVS_INVALID;
                    }

                    //no errors found, print word points
                    else {
                        mTurnPoints = validatedWordsInfo.getTotalPoints();
                        std::stringstream ss;
                        ss << "No errors found.\nWord points: " << mTurnPoints << ".\nTotal player points: " << mCurrentPlayer->getPoints() + mTurnPoints << "." << std::endl;
                        getMsgOutput()->setText(ss.str());
                        mBoardValidationState = BVS_VALID;
                    }
                }
                //points were already made
                else {
                    std::stringstream ss;
                    ss << "Points were already made this turn." << std::endl;
                    getMsgOutput()->setText(ss.str());
                }
            }
        }
    }
}

//pass previous turn and prepare a new one (by building a player interface for the next player)
void TurnHandler::nextTurn() {
    //increment turn
    ++mTurn;

    //do initial preparations on turn 0
    if(mTurn > 0) {
        //reset board validation state
        mBoardValidationState = BVS_NONE;
        //clean up unused letters on board and give them back to the player
        mInterBoard.undoWordSelection();
        auto unusedLetters = mInterBoard.removeNewTiles();
        for(const auto& letter : unusedLetters)
            assert(mCurrentPlayer->addToHand(letter));
        //give turn points to previous player
        mCurrentPlayer->setPoints(mCurrentPlayer->getPoints() + mTurnPoints);
        mTurnPoints = 0;
    }

    //get player hand interface
    auto& handInterface = mPlayerInterface->get("hand");

    //delete previous player's interface
    if(!handInterface.empty()) handInterface.clear();

    //change currently handled player
    if(mCurrentPlayer == mPlayers.end() - 1) mCurrentPlayer = mPlayers.begin();
    else ++mCurrentPlayer;
	//announce turn change
    std::stringstream ss;
    ss << "It's " << mCurrentPlayer->getName() << "'s turn!" << std::endl;
    getMsgOutput()->setText(ss.str());

    //replenish player's hand using deck
    mCurrentPlayer->replenishHand(mDeck);

    //if the player's hand is empty, end the game
    if(mCurrentPlayer->isHandEmpty()) {
        endGame(&*mCurrentPlayer);
        return;
    }

    //generate interactive hand tiles for real players
    if(mCurrentPlayer->getContr() == Player::CONTR_PLAYER) {
        unsigned int j = 0;
        for(const auto& ele : mCurrentPlayer->mHand) {
            //tmps
            auto bgr = handInterface.getBounds();
            auto td = (float)mTileSheet.getTileDimension();

            //generate interactive tile from symbol
            auto iTile = new InterTile(mTileSheet, ele);

            //adjust sizes
            mInterTileDimension =  std::max(bgr.width,bgr.height) / (float)(HAND_SIZE);
            iTile->setScale((float)mInterTileDimension / td, (float)mInterTileDimension / td);

            //set random positions
            iTile->setPosition(pickInterTileRandomPosition());

            //set click function
            iTile->addClickFunction([this](gr::Button& but) {
                //if button was just clicked...
                if(but.wasJustClicked()){
                    //if moving tile is not set, set it to the clicked button's
                    if(mMovingTile == nullptr){
                        setButtonAsMovingTile(but); //!DIRTY... there's probably a better way to do this....
                    }
                    //if the moving tile is set, unset it
                    else{
                        mMovingTile = nullptr;
                    }
                }
            });

            //add tile to interface
            std::stringstream ss;
            ss << "_" << j;
			handInterface.add(ss.str(), iTile);
            ++j;
        }
    }

    //place best word as a cpu
        //!WIP: make cpus of varying difficulties
    else if(mCurrentPlayer->getContr() == Player::CONTR_CPU) {
        auto bestWordsInfo = mInterBoard.suggestBestWord(*mCurrentPlayer);
        std::cout << mCurrentPlayer->getName() << " plays " << bestWordsInfo.getMainWordInfo().word << "!\n";
        assert(mInterBoard.placeWord(bestWordsInfo, *mCurrentPlayer));
        mInterBoard.highlightWords(bestWordsInfo);
        mInterBoard.enstablishNewLetters();
        mBoardValidationState = BVS_VALIDATED;
    }

    //logical error
    else assert(false);
}

//attempt to set button as currently active tile
void TurnHandler::setButtonAsMovingTile(gr::Button& button) {
    //attempt conversion
    auto tilePtr = dynamic_cast<InterTile*>(&button);
    //check validity
    assert(tilePtr);
    //set converted button as tile
    mMovingTile = tilePtr;
}

//end game and change current game handler
void TurnHandler::endGame(const Player* endingPlayer) {
    //validate params
    assert(endingPlayer);
    assert(endingPlayer->isHandEmpty());
    //switch handler
    setNext(new EndScreenHandler(mWindow, std::move(mPlayers)));
}

///logic handling functions
//move currently selected tile around
void TurnHandler::moveMovingTile() {
    if(mMovingTile) {
        sf::Vector2f mousePosition = mWindow.mapPixelToCoords(sf::Mouse::getPosition(mWindow));
        mMovingTile->setPosition(mousePosition);
    }
}

///interface generation functions
//return inter valid tile random position
sf::Vector2f TurnHandler::pickInterTileRandomPosition() const {
    //get hand interface
    auto& handInterface = mPlayerInterface->get("hand");

    //tmps
    auto bgr = handInterface.getBounds();
    sf::FloatRect interTileRect(0.f, 0.f, mInterTileDimension, mInterTileDimension);

    //keep picking a random position until a valid one is found
    bool tilePlaced;
    do {
        tilePlaced = true;
        //pick random position
        interTileRect.left = bgr.left + rrange(0.f, bgr.width - interTileRect.width);
        interTileRect.top = bgr.top + rrange(0.f, bgr.height - interTileRect.height);

        //check if a tile in the picked position would overlap any already existing !!and active!! tiles
        for(const auto& pair : handInterface.getChildMap()){
            const auto ele = pair.second;
            if(ele->isActive() && ele->getBounds().intersects(interTileRect)){
                tilePlaced = false;
                break;
            }
        }
    } while(!tilePlaced);

    //return
    return sf::Vector2f(interTileRect.left, interTileRect.top);
}

///ui functions
//handle event
void TurnHandler::handleEvent(const sf::Event& e) {
    mPlayerInterface->handleEvent(e);
    mInterBoard.handleEvent(e, mWindow);
}

//handle logical operations
void TurnHandler::handleLogic(const sf::Time& time) {
    //update player interface
    mPlayerInterface->update(mWindow);
    //update interactive board logic
    mInterBoard.handleLogic(mWindow);
    //move selected tile
    moveMovingTile();
}

//draw interface
void TurnHandler::handleDrawing() const {
    //draw board
    mWindow.draw(mInterBoard);
    //draw user interface
    mWindow.draw(*mPlayerInterface);
}
