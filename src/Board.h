#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#include <vector>
#include <set>
#include <boost/optional.hpp>

#include "WordInfo.h"
#include "Dictionary.h"
#include "BoardTiles.h"
#include "Player.h"
#include "constants.h"

///Board class
class Board {
    public:
        ///constants
        //symbol interpreted by board loading process
        static const char LOADSYM_READ = '#';
        static const char LOADSYM_BONUS_EMPTY = '_';
        static const char LOADSYM_BONUS_LETTER = 'L';
        static const char LOADSYM_BONUS_WORD = 'W';
        static const char LOADSYM_BONUS_START = 'S';

        //maximum point multiplier possible
        static const unsigned MAX_POINT_MULT = 9;

        ///constructor
        Board(const std::string& filePath, const Dictionary& dictionary);
        ///move constructor
        Board(Board&& board);

        ///checkers
        //check if position is out of bounds
        bool posOutOfBounds(const sf::Vector2i& pos) const {return pos.x < 0 || pos.y < 0 ||
                                                                   pos.x >= mBoardWidth || pos.y >= mBoardHeight;}
        //positional tile accessors
        const LetterTile& getLetterTile(const sf::Vector2i& pos) const;
        const PointsTile& getPointsTile(const sf::Vector2i& pos) const;
        LetterTile& getLetterTile(const sf::Vector2i& pos);
        PointsTile& getPointsTile(const sf::Vector2i& pos);
        //get points yielded by letter tile in given position (ignores word bonuses)
        unsigned int getLetterTilePoints(const sf::Vector2i& pos) const;

        ///setters
        //set tile - only the symbol can be set, because
        //the other info is handled by the board validation process
        void setLetterTile(const sf::Vector2i& pos, char symbol);

        ///word checkers
        //returns information about the validity, position, orientation and points made about a word
        //that starts at the given position and has the given orientation
        WordInfo findValidWord(sf::Vector2i pos, orientation_t orientation,
                               char virtualLetter = '\0',
                               const sf::Vector2i& virtualLetterPos = sf::Vector2i(-1, -1)) const;
        //find and validate collateral word at given position.
        //a collateral word is formed when a newly placed letter from a main word (which is a word directly
        //selected by the player for validation) forms a new word which is different from the main word.
        //Its starting position is calculated from the position of the letter of the main word from which it
        //originates.
        boost::optional<WordInfo> findValidCollateralWord(sf::Vector2i mainPos, orientation_t orientation,
                                                          char virtualLetter = '\0') const;
        //-) Attempts to construct a valid word from a position and a preferred orientation.
        //-) The obtained word ought to be intersected correctly with the others around it.
        //-) Also takes into account other potential words formed by the examined word,
        //   redeeming it invalid if one of these collaterally formed words is also valid.
        //-) Aborts and writes error to error string on failure or if word is invalid.
        //-) Also calculates points yielded by word and highlights letters forming the words that yield said points.
        WordsInfo findValidWords(sf::Vector2i pos) const;

        ///word builders
        //position new word based on player's hand and passed word info - return success
        bool placeWord(const WordsInfo& wordsInfo, Player& player);
        ///board highlighting
        //highlight valid word at given position, with the given orientation
        void highlightValidWord(sf::Vector2i pos, orientation_t orientation);
        //highlight invalid word at given position, with the given orientation
        void highlightInvalidWord(sf::Vector2i pos, orientation_t orientation);
        //highlight words as instructed by words info
        void highlightWords(const WordsInfo& wordsInfo);

        ///board validation/reset
        //make all new letter tiles old - returns false of failure
        void enstablishNewLetters();
        //remove highlighting (from particular word or from all over the board)
        void undoWordSelection(sf::Vector2i wordPosition, orientation_t wordOrientation);
        void undoWordSelection();
        //removes all new tiles and returns them
        std::vector<char> removeNewTiles();

        ///hints generation functions
        //Suggest best possible playable word.
        //The function simply calls its recursive helper on all possible positions of
        //the board - returns best possible word in terms of points yielding.
        WordsInfo suggestBestWord(const Player& player) const;

		
    protected:
        ///tile info variables
        //board dimensions
        unsigned int mBoardWidth, mBoardHeight;
        //tiles
        std::vector<BoardTile> mTiles;
        //number of undefined tiles on board
        unsigned int mNewTilesNum = 0;
        //dictionary used to validate words
        const Dictionary* mDictionary;

		///graphical optimization variables
		//NB this variable is necessarily here (and not in InterBoard, the graphical expansion of
		//   this class) because the word selection logic is deeply coupled with the word highlighting
		//   system, which needs to have a way to tell the graphical code when the board's colors need to
		//   be updated. This system resembles the dirty flag pattern, and is so used as an optimization
		//   technique to avoid useless recalculations of the board's vertex array when drawing. This design
		//   problem could have been avoided by using the component pattern instead of inheritance.
		mutable bool mNeedToUpdateVertices = true;

        ///hints generation functions
        //recursive helpers for suggestBestWord.
        //They function by recursively checking the board's dictionary-tree while building all
        //possible valid words that start on the given position and have the given orientation.
        //Apart from dictionary validity, the functions also check for board positional validity
        //and collateral validity.
        //The dictionary is recursed in two ways:
        //  1) If a letter is found on the board at the current position, the corresponding letter is picked
        //     from the current dictionary level, and its subtree is checked for future letters.
        //  2) Otherwise (no letter found at current position), the node corresponding to each letter
        //     in the specified player's hand is checked at once (non-unique letters are ignored) but using
        //     only a subset of the initial hand, which has the checked letter taken away from it.
        //When the function builds a word, it checks to see if it has the best score yet by comparing it to
        //a variable passed to all its recursive calls (bestWordInfo), which, at the end of the process, contains
        //the best word possible in terms of points yielded.
        void recSuggestBestWord(WordsInfo& bestWordsInfo, const Player::hand_t& hand,
                                const Dictionary::Node* node, orientation_t orientation,
                                size_t fixedIndex, size_t startingIndex, size_t index,
                                WordsInfo currentWord) const;
        void recSuggestBestWord(WordsInfo& bestWordsInfo, const Player::hand_t& hand,
                                const Dictionary::Node* node, orientation_t orientation,
                                size_t fixedIndex, size_t startingIndex) const;
};


#endif // BOARD_H_INCLUDED
