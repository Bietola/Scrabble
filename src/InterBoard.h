#ifndef INTERBOARD_H_INCLUDED
#define INTERBOARD_H_INCLUDED

#include <SFML/Graphics.hpp>

#include "TileSheet.h"
#include "Board.h"

///InterBoard class
class InterBoard: public Board, public sf::Drawable, public sf::Transformable {
    public:
        ///friends
        friend class TurnHandler;
		
        ///constructors
        InterBoard(Board&& board, const TileSheet& tileSheet);

        ///getters
        //get tile size
        sf::Vector2i getTileSize() const {return {mBoardWidth, mBoardHeight};}
        //get size with scale
        sf::Vector2f getSize() const;
        //get size without scale (no transform)
        sf::Vector2f getSizeNT() const;
        //get tile sheet
        const TileSheet& getTileSheet() const {return mTileSheet;}

        ///utility
        sf::Vector2i getTileMousePosition(const sf::RenderWindow& window) const;

        ///interface functions
        void handleEvent(const sf::Event& e, const sf::RenderWindow& window);
        void handleLogic(const sf::RenderWindow& window);

	private:
        ///graphical variables
        //vertex array
        mutable std::vector<sf::Vertex> mVertices;
        //tile sheet
        const TileSheet& mTileSheet;

        ///graphical functions
        //update vertex array
        void updateVertices() const;
        //render board to screen
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

#endif // INTERBOARD_H_INCLUDED
