#ifndef BOARDTILES_H_INCLUDED
#define BOARDTILES_H_INCLUDED

///LetterTile class
class LetterTile {
    public:
        //variables
        char symbol = ' ';
        bool isNew = false;
        bool isWild = false;
        bool validHighlight = false;
        bool invalidHighlight = false;
        //constructors
        LetterTile(){}
        LetterTile(char symbol, bool isNew):
            symbol(symbol == '$' ? 'A' : symbol), isNew(isNew), isWild(symbol == '$') {}
};

///PointsTile class
class PointsTile {
    public:
        //enumerations
        enum bonus_t{
            B_LETTER,
            B_WORD,
            B_START
        };
        //constants
        static const unsigned int START_BONUS = 2;
        //variables
        bonus_t bonus;
        unsigned int points;
        //helper getters
        unsigned int getWordBonus() const {return bonus == B_WORD ? points : 1;}
        unsigned int getLetterBonus() const {return bonus == B_LETTER ? points : 1;}
        //constructor
        PointsTile(bonus_t bonus = B_LETTER, unsigned int points = 1):
            bonus(bonus), points(points){}
};

///BoardTile structure (combination of letter and points tile)
struct BoardTile {
    PointsTile pointsTile;
    LetterTile letterTile;
};


#endif // BOARDTILES_H_INCLUDED
