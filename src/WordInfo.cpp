#include "WordInfo.h"

///WordInfo functions
//turn error enumeration into error message string
std::string WordInfo::getErrorMessage() const {
    switch(error) {
        case ERR_NONE:
            return "No errors found...";
            break;
        case ERR_NO_NEW_TILES_ON_BOARD:
            return "No new tiles placed on board.";
            break;
        case ERR_NO_NEW_TILES_ON_WORD:
            return "No new tiles used to form word.";
            break;
        case ERR_INVALID_POSITION:
            return "Invalid starting position.";
            break;
        case ERR_NOT_IN_DICTIONARY:
            return "Word not in dictionary.";
            break;
        case ERR_EMPTY:
            return "Word empty (THERE IS PROBABLY A BUG SOMEWHERE...)";
            break;
        default:
            assert(false);
    }
}

///WordsInfo functions
//get total number of old tiles
unsigned int WordsInfo::getTotalNumberOfOldTiles() const {
    return mainWordInfo.numOfOldTiles + std::accumulate(collateralWordsInfo.begin(), collateralWordsInfo.end(), 0,
                                                        [](int tot, const auto& val) {return tot + val.numOfOldTiles;});
}
//check to see if there is an error (back up error string)
bool WordsInfo::hasError() const {
    //words form no interceptions
    if(getTotalNumberOfOldTiles() == 0) {
        return true;
    }
    //there is more than one word containing new letters
    else if((mainWordInfo.numOfNewTiles > 1 ? 1 : 0) +
            std::count_if(collateralWordsInfo.begin(), collateralWordsInfo.end(),
                          [](const auto& val) {return val.numOfNewTiles > 1;}) > 1) {
        return true;
    }
    for(const auto& ele : collateralWordsInfo) {
        if(ele.error)
            return true;
    }
    //main word has error
    if(mainWordInfo.error)
        return true;
    //
    return false;
}
//make an error string
std::string WordsInfo::makeErrorString() const {
    //main word has an error indicating that no new tiles where placed on the board
    if(mainWordInfo.error == WordInfo::ERR_NO_NEW_TILES_ON_BOARD) {
        return mainWordInfo.getErrorMessage();
    }
    //main does not have a valid starting position
    else if(mainWordInfo.error == WordInfo::ERR_INVALID_POSITION) {
        return mainWordInfo.getErrorMessage();
    }
    //words form no interceptions
    else if(getTotalNumberOfOldTiles() == 0) {
        return "Formed word(s) does not include any interception with already existing words.";
    }
    //there is more than one word containing new letters
    else if((mainWordInfo.numOfNewTiles > 1 ? 1 : 0) +
            std::count_if(collateralWordsInfo.begin(), collateralWordsInfo.end(),
                          [](const auto& val) {return val.numOfNewTiles > 1;}) > 1) {
        return "Too much new tiles used to form words.";
    }
    std::stringstream errSS;
    //print main word error
    if(mainWordInfo.error)
        errSS << "[" << mainWordInfo.word << "]: " << mainWordInfo.getErrorMessage() << std::endl;
    //print all collateral words errors
    for(const auto& ele : collateralWordsInfo) {
        if(ele.error)
            errSS << "[" << ele.word << "]: " << ele.getErrorMessage() << std::endl;
    }
    return errSS.str();
}
//calculate and get total points
unsigned int WordsInfo::getTotalPoints() const {
    unsigned int totalPoints = mainWordInfo.points * mainWordInfo.bonus;
    for(const auto& ele : collateralWordsInfo) {
        totalPoints += ele.points * ele.bonus;
    }
    return totalPoints;
}
