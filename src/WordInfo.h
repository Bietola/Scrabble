#pragma once

#include <cassert>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <sstream>

#include <SFML/Graphics.hpp>

///orientation enumeration
enum orientation_t {
    O_HORIZONTAL,
    O_VERTICAL
};

///WordInfo class
struct WordInfo {
    public:
        ///error enumeration
        enum error_t {
            ERR_NONE = 0,
            ERR_NO_NEW_TILES_ON_BOARD = 1,
            ERR_NO_NEW_TILES_ON_WORD,
            ERR_INVALID_POSITION,
            ERR_NOT_IN_DICTIONARY,
            ERR_EMPTY
        };
        ///setters and getters
        bool isInvalid() const {return error;}
        auto getTotalPoints() const {if(this != nullptr);}
        orientation_t getOrientation() const {return orientation;}
        std::string getErrorMessage() const;
        ///operators
        WordInfo operator+(char c) {
            WordInfo retVal = *this;
            retVal.word += c;
            return retVal;
        }
        WordInfo& operator+=(char c) {
            *this = *this + c;
            return *this;
        }
        ///error info
        error_t error = ERR_NONE;
        ///points info
        unsigned int points = 0;
        unsigned int bonus = 1;
        ///word
        std::string word = "";
        ///word info
        unsigned int numOfOldTiles = 0;
        unsigned int numOfNewTiles = 0;
        std::vector<size_t> wildPositions;
        ///position info
        sf::Vector2i position = {0, 0};
        orientation_t orientation = O_HORIZONTAL;
};

///WordsInfo class
class WordsInfo {
    public:
        ///friends
        friend class Board;
        ///constructor
        WordsInfo() {}
        explicit WordsInfo(const WordInfo& mainWordInfo): mainWordInfo(mainWordInfo) {}
        ///setters and getters
        const WordInfo& getMainWordInfo() const {return mainWordInfo;}
        void setMainWordInfo(const WordInfo& mwi) {mainWordInfo = mwi;}
        void addCollateral(const WordInfo& collateralWordInfo) {collateralWordsInfo.push_back(collateralWordInfo);}
        void addMainWildPosition(size_t pos) {mainWordInfo.wildPositions.push_back(pos);}
        unsigned int getTotalPoints() const;
        unsigned int getTotalNumberOfOldTiles() const;
        unsigned int getTotalNumberOfNewTiles() const {return mainWordInfo.numOfNewTiles;}
        size_t getMainSize() const {return mainWordInfo.word.size();}
        ///error handling
        std::string makeErrorString() const;
        bool hasError() const;
        ///operators
        WordsInfo operator+(char letter) {
            auto retVal = *this;
            retVal.mainWordInfo.word += letter;
            return retVal;
        }
        WordsInfo& operator+=(char letter) {
            *this = *this + letter;
            return *this;
        }
    private:
        ///variables
        WordInfo mainWordInfo;
        std::vector<WordInfo> collateralWordsInfo;
};
