#include "Board.h"

///Board functions
//constructor
//!WANING: remember to copy member initializer list to copy/move c.tors
Board::Board(const std::string& filePath, const Dictionary& dictionary):
    mTiles(),
    mDictionary(&dictionary)
{
    //tmp
    std::string tmpStr;
    //read and interpret board file
    std::ifstream f(filePath);
    if(!f){
        std::cout << "FATAL ERROR: could not open board file at file path: \"" << filePath << "\"." << std::endl;
        std::terminate();
    }
    //go on until read symbol
    while(f.get() != LOADSYM_READ);
    //get board size
    f >> mBoardWidth;
    f >> tmpStr; //for x between board sizes
    f >> mBoardHeight;
    //allocate board
    mTiles.resize(mBoardWidth * mBoardHeight);
    //read and interpret board info
    for(int j = 0; j < mBoardWidth * mBoardHeight; j++){
        //put empty letter tile
        mTiles[j].letterTile = LetterTile();
        //get points tile symbol
        std::string ptSyms;
        f >> ptSyms;
        //validate it
        if(ptSyms.size() != 2){
            std::cout << "FATAL ERROR: found tile symbol of invalid length while loading board." << std::endl;
            std::terminate();
        }
        //extrapolate info
        unsigned points = (unsigned)(ptSyms[0] - '0');
        char type = ptSyms[1];
        //validate points
        if((type == LOADSYM_BONUS_LETTER || type == LOADSYM_BONUS_WORD) && points > MAX_POINT_MULT){
            std::cout << "ERROR: too many points attributed to board (" << points << " > " << MAX_POINT_MULT << ")." << std::endl;
            mTiles[j].pointsTile = PointsTile();
            continue;
        }
        //interpret symbol and create points tile from it
        switch(type){
            case LOADSYM_BONUS_EMPTY:
                mTiles[j].pointsTile = PointsTile();
                break;
            case LOADSYM_BONUS_LETTER:
                mTiles[j].pointsTile = PointsTile(PointsTile::B_LETTER, points);
                break;
            case LOADSYM_BONUS_WORD:
                mTiles[j].pointsTile = PointsTile(PointsTile::B_WORD, points);
                break;
            case LOADSYM_BONUS_START:
                mTiles[j].pointsTile = PointsTile(PointsTile::B_START, 2);
                break;
            default:
                std::cout << "ERROR: failed to interpret symbol while creating points tile in board loading process (symbol = " << type << ")." << std::endl;
        }
    }
}

//move constructor
Board::Board(Board&& board):
	mBoardWidth(board.mBoardWidth),
	mBoardHeight(board.mBoardHeight),
    mTiles(std::move(board.mTiles)),
	mNewTilesNum(board.mNewTilesNum),
    mDictionary(std::move(board.mDictionary)),
	mNeedToUpdateVertices(board.mNeedToUpdateVertices)
{}


//tile const and nonconst accessors
const LetterTile& Board::getLetterTile(const sf::Vector2i& pos) const {
    assert(!posOutOfBounds(pos));
    return mTiles[pos.y * mBoardHeight + pos.x].letterTile;
}
const PointsTile& Board::getPointsTile(const sf::Vector2i& pos) const {
    assert(!posOutOfBounds(pos));
    return mTiles[pos.y * mBoardHeight + pos.x].pointsTile;
}
LetterTile& Board::getLetterTile(const sf::Vector2i& pos){
    return const_cast<LetterTile&>(
        static_cast<const Board*>(this)->getLetterTile(pos)
    );
}
PointsTile& Board::getPointsTile(const sf::Vector2i& pos){
    return const_cast<PointsTile&>(
        static_cast<const Board*>(this)->getPointsTile(pos)
    );
}

//get points yielded by letter tile in specified position (ignores word bonuses)
unsigned int Board::getLetterTilePoints(const sf::Vector2i& pos) const {
    const auto& lTile = getLetterTile(pos);
    const auto& pTile = getPointsTile(pos);
    return (lTile.isWild ? 0 : LETTERS_POINTS.at(lTile.symbol)) * (pTile.bonus == PointsTile::B_LETTER ? pTile.points : 1);
}

//set letter tile
void Board::setLetterTile(const sf::Vector2i& pos, char symbol){
    //validate params
    assert(!posOutOfBounds(pos));
    //get letter tile in specified position
    auto& tile = mTiles[pos.y * mBoardHeight + pos.x].letterTile;
    //if a symbol was not previously set, set it
    if(tile.symbol != symbol){
        //set letter tile
        tile = LetterTile(symbol, true);
        //set board graphics to be updated
        mNeedToUpdateVertices = true;
        //increment number of new tiles on board
        mNewTilesNum++;
    }
}

//attempts to find valid word - more in header file
WordInfo Board::findValidWord(sf::Vector2i pos, orientation_t orientation,
                              char virtualLetter, const sf::Vector2i& virtualLetterPos) const {
    //validate params
    assert(!posOutOfBounds(pos));

    //return value
    WordInfo retVal;
    retVal.position = pos;
    retVal.orientation = orientation;

    //call function only if there are new words to validate
    if(mNewTilesNum == 0 && virtualLetter == '/0'){
        retVal.error = WordInfo::ERR_NO_NEW_TILES_ON_BOARD;
        return retVal;
    }

    //-deduce orientation of word to validate it
    //word to validate
    std::string word;
    //determinate if word orientation is valid
    if((orientation == O_HORIZONTAL && !posOutOfBounds({pos.x-1, pos.y}) && getLetterTile({pos.x-1, pos.y}).symbol != ' ') ||
       (orientation == O_VERTICAL && !posOutOfBounds({pos.x, pos.y-1}) && getLetterTile({pos.x, pos.y-1}).symbol != ' ')) {
        retVal.error = WordInfo::ERR_INVALID_POSITION;
        return retVal;
    }
    auto posIncrement = (orientation == O_HORIZONTAL ? sf::Vector2i(1, 0) : sf::Vector2i(0, 1));

    //get word by reading every letter from the initial position on out
    //also calculate points - and letter and word bonuses
    while(!posOutOfBounds(pos) && (getLetterTile(pos).symbol != ' ' || pos == virtualLetterPos)){
        //pick tile (make virtual tile if necessary)
        LetterTile lTile;
        if(pos == virtualLetterPos)
            lTile = LetterTile(virtualLetter, true);
        else
            lTile = getLetterTile(pos);

        //temps
        const auto& pTile = getPointsTile(pos);
        unsigned int letterPoints = (lTile.isWild ? 0 : LETTERS_POINTS.at(lTile.symbol));

        //keep track of new and old tiles
        if(lTile.isNew)
            retVal.numOfNewTiles++;
        else
            retVal.numOfOldTiles++;

        //add an old tile to represent the passage on the start tile
        if(pTile.bonus == PointsTile::B_START)
            retVal.numOfOldTiles++;

        //add bonus letter points for letter bonuses
            //!WIP: do it (maybe...) for all tiles (rules are uncertain)
        if(lTile.isNew) {
            if(pTile.bonus == PointsTile::B_LETTER){
                letterPoints *= pTile.points;
            }
            //add bonus word points for word bonuses
            else if(pTile.bonus == PointsTile::B_WORD || pTile.bonus == PointsTile::B_START){
                retVal.bonus *= pTile.points;
            }
        }

        //add letter to word
        word.push_back(lTile.symbol);
        //increment total word points by letter points
        retVal.points += letterPoints;
        //increment letter position horizontally or vertically
        pos += posIncrement;
    }

    //error: word is empty
    if(word.empty()) {
        retVal.error = WordInfo::ERR_EMPTY;
        return retVal;
    }

    //error: word not in dictionary
    if(!mDictionary->searchWord(word)) {
        retVal.error = WordInfo::ERR_NOT_IN_DICTIONARY;
        retVal.word = word;
        return retVal;
    }
    //error: word contains no new tile
    if(retVal.numOfNewTiles == 0) {
        retVal.error = WordInfo::ERR_NO_NEW_TILES_ON_WORD;
        retVal.word = word;
        return retVal;
    }

    //update vertex array to show highlighting of word
    mNeedToUpdateVertices = true;

    //success
    return retVal;
}

//find valid collateral word - more in header file
boost::optional<WordInfo> Board::findValidCollateralWord(sf::Vector2i pos, orientation_t orientation,
                                                         char virtualLetter) const {
    WordInfo retVal;
    auto virtualLetterPos = pos;
    auto posIncrement = (orientation == O_HORIZONTAL ? sf::Vector2i(1, 0) : sf::Vector2i(0, 1));

    //check if collateral word exists
    if((posOutOfBounds(pos - posIncrement) || getLetterTile(pos - posIncrement).symbol == ' ') &&
       (posOutOfBounds(pos + posIncrement) || getLetterTile(pos + posIncrement).symbol == ' '))
        return boost::optional<WordInfo>();

    //find starting position of collateral word
    while(!posOutOfBounds(pos - posIncrement) &&
          getLetterTile(pos - posIncrement).symbol != ' ')
        pos -= posIncrement;
    if(virtualLetter != '\0')
        retVal = findValidWord(pos, orientation, virtualLetter, virtualLetterPos);
    else
        retVal = findValidWord(pos, orientation);

    //return
    return retVal;
}

//find valid words - more in header file
WordsInfo Board::findValidWords(sf::Vector2i pos) const {
    //parameter checks
    assert(!posOutOfBounds(pos));

    //try validating main word in both directions
    auto horizontalWordInfo = findValidWord(pos, O_HORIZONTAL);
    auto verticalWordInfo = findValidWord(pos, O_VERTICAL);

    //pick the direction with more new tiles
    WordInfo mainWordInfo;
    if(horizontalWordInfo.numOfNewTiles == verticalWordInfo.numOfNewTiles &&
       horizontalWordInfo.numOfNewTiles == 0) {
        return WordsInfo(horizontalWordInfo);
    }
    else if(horizontalWordInfo.numOfNewTiles >= verticalWordInfo.numOfNewTiles) {
        mainWordInfo = horizontalWordInfo;
    }
    else if(horizontalWordInfo.numOfNewTiles < verticalWordInfo.numOfNewTiles) {
        mainWordInfo = verticalWordInfo;
    }

    //create return value
    WordsInfo totalWordsInfo;
    totalWordsInfo.setMainWordInfo(mainWordInfo);

    //determine collateral word check parameters
    sf::Vector2i posIncrement;
    sf::Vector2i collateralPosIncrement;
    orientation_t collateralWordOrientation;
    if(mainWordInfo.orientation == O_HORIZONTAL) {
        posIncrement = {1, 0};
        collateralPosIncrement = {0, 1};
        collateralWordOrientation = O_VERTICAL;
    }
    else {
        posIncrement = {0, 1};
        collateralPosIncrement = {1, 0};
        collateralWordOrientation = O_HORIZONTAL;
    }

    //validate collateral words
    for(;!posOutOfBounds(pos) && getLetterTile(pos).symbol != ' '; pos += posIncrement) {
        //don't work on new tiles
        if(getLetterTile(pos).isNew) {
            //find collateral word, and add to the retval if it exists
            auto collateralWordInfo = findValidCollateralWord(pos, collateralWordOrientation);
            if(collateralWordInfo) {
                totalWordsInfo.addCollateral(*collateralWordInfo);
            }
        }
    }

    //return success
    return totalWordsInfo;
}

//place word on board based on the specified word info (only places valid words) - return success
bool Board::placeWord(const WordsInfo& wordsInfo, Player& player) {
    //check if word info is valid
    if(wordsInfo.hasError())
        return false;

    //letters to place (position and letter)
    std::vector<std::pair<sf::Vector2i, char>> lettersToPlace;

    //check if player has enough letter tiles
    auto& hand = player.getHand();
    auto pos = wordsInfo.mainWordInfo.position;
    auto posIncrement = (wordsInfo.mainWordInfo.orientation == O_HORIZONTAL ? sf::Vector2i(1, 0) : sf::Vector2i(0, 1));
    for(const auto& ele : wordsInfo.mainWordInfo.word) {
        if(getLetterTile(pos).symbol == ' ') {
            auto found = std::find(hand.begin(), hand.end(), std::toupper(ele));
            if(found != hand.end())
                lettersToPlace.push_back({pos, ele});
            else
                return false;
        }
        pos += posIncrement;
    }

    //place word
    for(const auto& ele : lettersToPlace) {
        auto found = std::find(hand.begin(), hand.end(), std::toupper(ele.second));
        assert(found != hand.end());
        hand.erase(found);
        setLetterTile(ele.first, std::toupper(ele.second));
    }

    //success
    return true;
}

//highlight word at given position as valid
void Board::highlightValidWord(sf::Vector2i pos, orientation_t orientation) {
    auto posIncrement = (orientation == O_HORIZONTAL ? sf::Vector2i(1, 0) : sf::Vector2i(0, 1));
    for(; !posOutOfBounds(pos) && getLetterTile(pos).symbol != ' '; pos += posIncrement) {
        getLetterTile(pos).validHighlight = true;
        getLetterTile(pos).invalidHighlight = false;
    }
    mNeedToUpdateVertices = true;
}

//highlight word at given position as invalid
void Board::highlightInvalidWord(sf::Vector2i pos, orientation_t orientation) {
    auto posIncrement = (orientation == O_HORIZONTAL ? sf::Vector2i(1, 0) : sf::Vector2i(0, 1));
    for(; !posOutOfBounds(pos) && getLetterTile(pos).symbol != ' '; pos += posIncrement) {
        getLetterTile(pos).validHighlight = false;
        getLetterTile(pos).invalidHighlight = true;
    }
    mNeedToUpdateVertices = true;
}

//highlight words as instructed by words info
void Board::highlightWords(const WordsInfo& wordsInfo) {
    //highlight main word
    if(wordsInfo.mainWordInfo.error)
        highlightInvalidWord(wordsInfo.mainWordInfo.position, wordsInfo.mainWordInfo.orientation);
    else
        highlightValidWord(wordsInfo.mainWordInfo.position, wordsInfo.mainWordInfo.orientation);

    //highlight collaterals
    for(const auto& ele : wordsInfo.collateralWordsInfo) {
        if(ele.error)
            highlightInvalidWord(ele.position, ele.orientation);
        else
            highlightValidWord(ele.position, ele.orientation);
    }
    mNeedToUpdateVertices = true;
}

//make all new letter tiles old
void Board::enstablishNewLetters() {
    mNewTilesNum = 0;
    for(auto& ele : mTiles){
        if(ele.letterTile.isNew && ele.letterTile.validHighlight)
            ele.letterTile.isNew = false;
    }
}

//remove highlighting
void Board::undoWordSelection(sf::Vector2i wordPosition, orientation_t wordOrientation) {
    auto posIncrement = (wordOrientation == O_HORIZONTAL ? sf::Vector2i(1, 0) : sf::Vector2i(0, 1));
    for(; !posOutOfBounds(wordPosition) && getLetterTile(wordPosition).symbol != ' '; wordPosition += posIncrement) {
        getLetterTile(wordPosition).validHighlight = false;
        getLetterTile(wordPosition).invalidHighlight = false;
    }
    mNeedToUpdateVertices = true;
}
void Board::undoWordSelection(){
    for(auto& ele : mTiles){
        ele.letterTile.validHighlight = false;
        ele.letterTile.invalidHighlight = false;
    }
    mNeedToUpdateVertices = true;
}

//removes all new tiles from board and returns their symbols
std::vector<char> Board::removeNewTiles(){
    //keep vector w/ removed tiles
    std::vector<char> removedTiles;

    //search for undefined and partly defined tiles
    for(auto& ele : mTiles){
        if(ele.letterTile.isNew){
            //remove them and add them to the vector
            removedTiles.push_back(ele.letterTile.isWild ? '$' : ele.letterTile.symbol);
            ele.letterTile = LetterTile();
        }
    }

    //update vertex array to cancel removed letter tiles
    mNeedToUpdateVertices = true;

    //return vector
    return removedTiles;
}

//suggest best possible word (more in header file)
WordsInfo Board::suggestBestWord(const Player& player) const {
	//
    WordsInfo bestWordsInfo;

    //check every row
	std::cout << "SEARCHING FOR BEST WORD POSSIBLE" << std::endl;
	std::cout << "checking rows: ";
    for(size_t y = 0; y < mBoardHeight; ++y) {
        for(size_t x = 0; x < mBoardWidth; ++x) {
            //check if starting position is valid
            if(x == 0 || getLetterTile({x - 1, y}).symbol == ' ')
                recSuggestBestWord(bestWordsInfo, player.getHand(),
                                   mDictionary->getRoot(), O_HORIZONTAL,
                                   y, x);
        }
		std::cout << '.';
    }
	std::cout << std::endl << "checking columns: ";

    //check ever column
    for(size_t x = 0; x < mBoardWidth; ++x) {
        for(size_t y = 0; y < mBoardHeight; ++y) {
            if(y == 0 || getLetterTile({x, y - 1}).symbol == ' ')
                recSuggestBestWord(bestWordsInfo, player.getHand(),
                                   mDictionary->getRoot(), O_VERTICAL,
                                   x, y);
        }
		std::cout << '.'; 
    }
	std::cout << std::endl;

	//
    return bestWordsInfo;
}
void Board::recSuggestBestWord(WordsInfo& bestWordsInfo, const Player::hand_t& hand,
                               const Dictionary::Node* node, orientation_t orientation,
                               size_t fixedIndex, size_t initIndex, size_t index,
                               WordsInfo currentWordsInfo) const {
    //parameter checks
    assert(initIndex >= 0 && index >= 0 && fixedIndex >= 0);

    //base case - letter does not exist in dictionary
    if(!node) {
        //!gLog << "found dead end: " << parolaCorrente << endl;
        return;
    }

    //base case - out of bounds
        //!redesign funzione per permettere tavole con lati assimetrici
        assert(mBoardWidth == mBoardHeight);
    if(index >= mBoardWidth || fixedIndex >= mBoardWidth || initIndex >= mBoardWidth){
        //!gLog << "algoritmo uscito dai bordi" << endl;
        return;
    }

    //check if a word has been found
    auto pos = (orientation == O_HORIZONTAL ? sf::Vector2i(index, fixedIndex) : sf::Vector2i(fixedIndex, index));
    if(node->endOfWord) {
        //!gLog << "trovata possibile parola massima: " << parolaCorrente << " [" << currentWordPoints << "]" << endl;
        //check that:
        //  -) word is better than currently best
        //  -) word is composed of at least 1 new letter
        //  -) word is composed of at least 1 old letter (or touches start tile)
        //  -) word is actually a stand alone word and does not go on to form another word
        if(currentWordsInfo.getTotalPoints() > bestWordsInfo.getTotalPoints() &&
           currentWordsInfo.getTotalNumberOfNewTiles() > 0 &&
           currentWordsInfo.getTotalNumberOfOldTiles() > 0 &&
           (posOutOfBounds(pos) || getLetterTile(pos).symbol == ' ')) {
            //!gLog << "parola vince!" << endl;
            bestWordsInfo = currentWordsInfo;
            bestWordsInfo.mainWordInfo.orientation = orientation;
        }
    }

    //get letter on board at current position
    char letter = std::tolower(getLetterTile(pos).symbol);

    //add bonus word points to word info
    currentWordsInfo.mainWordInfo.bonus *= getPointsTile(pos).getWordBonus();

    //recursive step - there is no letter: check for all possible letters in the player's hand
    if(letter == ' ') {
        //add a new tile to the word information
        currentWordsInfo.mainWordInfo.numOfNewTiles++;

        //add an old tile to the w.info if the word touches the start tile
        if(getPointsTile(pos).bonus == PointsTile::B_START)
            currentWordsInfo.mainWordInfo.numOfOldTiles++;
        //!gLog << "esplorando tutte le lettere" << endl;
        for(char handLetter : std::set<char>(hand.begin(), hand.end())) {
            //form new hand removing the used tile
            auto newHand = hand;
            newHand.erase(std::find(newHand.begin(), newHand.end(), handLetter));

            //add letter points to word info
            currentWordsInfo.mainWordInfo.points += LETTERS_POINTS.at(handLetter) * getPointsTile(pos).getLetterBonus();
            currentWordsInfo.mainWordInfo.bonus *= (getPointsTile(pos).bonus == PointsTile::B_WORD ?
                                                    getPointsTile(pos).bonus : 1);

            //if letter is wild, recurse for all possible letters
            if(handLetter == '$') {
                for(char wildLetter = 'a'; wildLetter <= 'z'; wildLetter++) {

                    //check collateral words
                    auto collateralWord = findValidCollateralWord(pos, orientation == O_HORIZONTAL ? O_VERTICAL : O_HORIZONTAL,
                                                                  std::toupper(wildLetter));
                    if(collateralWord) {
                        currentWordsInfo.addCollateral(*collateralWord);
                        if(collateralWord->error)
                            return;
                    }
                    auto newCurrentWordsInfo = currentWordsInfo;
                    newCurrentWordsInfo.addMainWildPosition(currentWordsInfo.getMainSize());
                    newCurrentWordsInfo += wildLetter;
                    recSuggestBestWord(bestWordsInfo, newHand,
                                       node->links[wildLetter - 'a'], orientation,
                                       fixedIndex, initIndex, index + 1,
                                       newCurrentWordsInfo);
                }
            }
            //if letter is not wild, recurse once
            else {
                //check collateral words
                auto collateralWord = findValidCollateralWord(pos, orientation == O_HORIZONTAL ? O_VERTICAL :
					   																		   	 O_HORIZONTAL,
                                                              std::toupper(handLetter));
                if(collateralWord) {
                    currentWordsInfo.addCollateral(*collateralWord);
                    if(collateralWord->error)
                        return;
                }
                handLetter = std::tolower(handLetter);
                recSuggestBestWord(bestWordsInfo, newHand,
                                   node->links[handLetter - 'a'], orientation,
                                   fixedIndex, initIndex, index + 1,
                                   currentWordsInfo + handLetter);
            }
        }
    }

    //recursive step - found a letter on the board: advance in the dictionary searching for words using that letter
    //in its particular position
    else {
        const auto& lTile = getLetterTile(pos);
        //find out if tile is new or old and add add the respective type to the word info
        if(lTile.isNew) currentWordsInfo.mainWordInfo.numOfNewTiles++;
        else currentWordsInfo.mainWordInfo.numOfOldTiles++;
        //!gLog << "trovata letter su tavola: " << letter << endl;

        //add letter points to word info
        currentWordsInfo.mainWordInfo.points += getLetterTilePoints(pos);

        //recurse
        recSuggestBestWord(bestWordsInfo, hand,
                           node->links[letter - 'a'], orientation,
                           fixedIndex, initIndex, index + 1,
                           currentWordsInfo + letter);
    }
}
void Board::recSuggestBestWord(WordsInfo& bestWordsInfo, const Player::hand_t& hand,
                               const Dictionary::Node* node, orientation_t orientation,
                               size_t fixedIndex, size_t initIndex) const {
    WordsInfo wordsInfo;
    wordsInfo.mainWordInfo.position = (orientation == O_HORIZONTAL ? sf::Vector2i{initIndex, fixedIndex} :
		   														   	 sf::Vector2i{fixedIndex, initIndex});
    recSuggestBestWord(bestWordsInfo, hand, node, orientation, fixedIndex, initIndex, initIndex, wordsInfo);
}

