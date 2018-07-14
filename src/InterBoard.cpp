#include "InterBoard.h"

///-InterBoard functions
///constructors
InterBoard::InterBoard(Board&& board, const TileSheet& tileSheet):
	Board(std::move(board)),
	mVertices(),
	mTileSheet(tileSheet)
{
    //set up vertex array
    mVertices.resize(mBoardWidth * mBoardHeight * 4);
}

//get size
sf::Vector2f InterBoard::getSizeNT() const {
    sf::Vector2f size;
    //calculate size without any transform
    size.x = mTileSheet.getTileDimension() * mBoardWidth;
    size.y = mTileSheet.getTileDimension() * mBoardHeight;
    return size;
}
sf::Vector2f InterBoard::getSize() const {
    sf::Vector2f size = getSizeNT();
    //scale it
    sf::Transform scaleT = sf::Transform::Identity;
    scaleT.scale(getScale().x, getScale().y);
    size = scaleT.transformPoint(size);
    //
    return size;
}

//transform mouse position into board tile position
sf::Vector2i InterBoard::getTileMousePosition(const sf::RenderWindow& window) const {
    //handle wild card decisions
    //get mouse position
    auto mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    //reverse transform it to match the board coordinate system
    auto boardPos = getInverseTransform().transformPoint(mousePos);
    //calculate tile indexes from transformed position
    sf::Vector2i posAsTile;
    posAsTile.x = (unsigned int)(boardPos.x) / getTileSheet().getTileDimension();
    posAsTile.y = (unsigned int)(boardPos.y) / getTileSheet().getTileDimension();
    return posAsTile;
}

//handle event
void InterBoard::handleEvent(const sf::Event& e, const sf::RenderWindow& window){
    //scrolling
	int symbolDelta = 0;
    if(e.type == sf::Event::MouseWheelScrolled) {
		symbolDelta = e.mouseWheelScroll.delta;
	}
	else if(e.type == sf::Event::KeyPressed) {
		if(e.key.code == sf::Keyboard::Add)
			symbolDelta = 1;
		else if(e.key.code == sf::Keyboard::Subtract)
			symbolDelta = -1;
	}
	if(symbolDelta != 0) {
        //manage wild card letter selection
        auto mouseTilePos = getTileMousePosition(window);
        bool wildTileFound = false;
        if(!posOutOfBounds(mouseTilePos)) {
            auto& tile = getLetterTile(mouseTilePos);
            if(tile.isWild && !tile.validHighlight && !tile.invalidHighlight && tile.isNew) {
                wildTileFound = (symbolDelta != 0);
				tile.symbol += symbolDelta;
				tile.symbol =
					tile.symbol < 'A' ? 'Z' :
						tile.symbol > 'Z' ? 'A' : tile.symbol;
            }
        }
        if(!wildTileFound) {
			float scaleFactor = 1 + (float)symbolDelta / 10.f;
			scale(scaleFactor, scaleFactor);
        }
        else
            mNeedToUpdateVertices = true;
    }
}

//handle logical operations
void InterBoard::handleLogic(const sf::RenderWindow& window){
    //handle board movement with wasd
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) move(0, 2);
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) move(0, -2);
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) move(2, 0);
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) move(-2, 0);
}

//update vertices
void InterBoard::updateVertices() const {
    for(unsigned int j = 0; j < mBoardWidth * mBoardHeight * 4; j+=4){
        //tmps
        const auto& letterTile = mTiles[j/4].letterTile;
        const auto& pointsTile = mTiles[j/4].pointsTile;
        auto td = mTileSheet.getTileDimension();
        //calculate coos
        unsigned int x = j/4 % mBoardHeight * td;
        unsigned int y = j/4 / mBoardHeight * td;
        //draw letter and special tiles with an higher priority in respect to point tiles
        sf::IntRect cr;
        if(letterTile.symbol != ' ') cr = mTileSheet.makeClipRect(letterTile.symbol);
        else if(pointsTile.points != 1) cr = mTileSheet.makeClipRect(pointsTile);
        else cr = mTileSheet.makeClipRect(' ');
        //!WIP: determine tile color modifier (highlighter)
        sf::Color color;
        if(letterTile.invalidHighlight) color = {0xFF, 0x00, 0x00, 0xFF};
        else if(letterTile.validHighlight) color = {0x00, 0xEE, 0xEE, 0xFF};
        else if(letterTile.isWild) color = {0xEE, 0xEE, 0x00, 0xFF};
        else color = {0xFF, 0xFF, 0xFF, 0xFF};
        //make vertices
        mVertices[j]   = sf::Vertex({x,    y},    color, {cr.left,    cr.top});
        mVertices[j+1] = sf::Vertex({x+td, y},    color, {cr.left+td, cr.top});
        mVertices[j+2] = sf::Vertex({x+td, y+td}, color, {cr.left+td, cr.top+td});
        mVertices[j+3] = sf::Vertex({x,    y+td}, color, {cr.left,    cr.top+td});
    }
    //!std::cout << "Vetex array upated" << std::endl;
}
//render board to screen
void InterBoard::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    //update vertices if needed
    if(mNeedToUpdateVertices){
        updateVertices();
        mNeedToUpdateVertices = false;
    }
    //set texture and transform
    states.texture = &mTileSheet.getTexture();
    states.transform = getTransform();
    //draw vertex array
    target.draw(&mVertices[0], mVertices.size(), sf::Quads, states);
}
