#include "TileSheet.h"

///-TileSheet functions
///tile sheet texture generation functions
//make tile position from index
sf::Vector2i TileSheet::makeTilePositionFromIndex(unsigned int index) const {
    return sf::Vector2i(index % mElementsInRow, index / mElementsInRow);
}
//return generated objects representing tile within tile sheet
sf::Sprite TileSheet::makeTileBgSprite(const sf::Texture& texture, unsigned int index) const {
    //temps
    float tdim = (float)mTileDimension;
    auto pos = makeTilePositionFromIndex(index);
    //
    sf::Sprite sprite(texture);
    sprite.scale(tdim/texture.getSize().x,tdim/texture.getSize().y);
    sprite.setPosition(pos.x*tdim, pos.y*tdim);
    return sprite;
}
sf::Text TileSheet::makeTileLetterText(const sf::Font& font, char letter, unsigned int index) const {
    //temps
    float tdim = (float)mTileDimension;
    float tlc = tdim / 2.0;
    auto pos = makeTilePositionFromIndex(index);
    //
    sf::Text tileLetter;
    tileLetter.setFont(font);
    tileLetter.setCharacterSize(tlc);
    tileLetter.setString(letter);
    tileLetter.setPosition(pos.x*tdim, pos.y*tdim);
    tileLetter.move((tdim-tlc)/2.0, (tdim-tlc)/2.0);
    return tileLetter;
}
//return "" representing the points given by ""
sf::Text TileSheet::makeTilePointsText(const sf::Font& font, unsigned int points, unsigned int index) const{
    //temps
    float tdim = (float)mTileDimension;
    float tlc = tdim / 2.0;
    auto pos = makeTilePositionFromIndex(index);
    //
    sf::Text tilePoints;
    tilePoints.setFont(font);
    tilePoints.setCharacterSize(tlc/2);
    std::stringstream ss;
    if(points < 10) ss << " ";
    ss << points;
    tilePoints.setString(ss.str());
    tilePoints.setPosition(pos.x*tdim, pos.y*tdim);
    tilePoints.move((tdim-tlc)/3.0 + tlc, 0);
    return tilePoints;
}
//return "" representing the points and the type of bonus
sf::Text TileSheet::makeTileBonusText(const sf::Font& font, PointsTile::bonus_t bonus, unsigned int points, unsigned int index) const {
    //temps
    float tdim = (float)mTileDimension;
    float tlc = tdim / 2.f;
    auto pos = makeTilePositionFromIndex(index);
    //
    sf::Text bonusText;
    bonusText.setFont(font);
    bonusText.setCharacterSize(tlc);
    std::stringstream ss;
    ss << points;
    if(bonus == PointsTile::B_LETTER) ss << "L";
    else if(bonus == PointsTile::B_WORD) ss << "W";
    bonusText.setString(ss.str());
    bonusText.setPosition(pos.x*tdim, pos.y*tdim);
    bonusText.move((tdim-tlc)/3.0, (tdim-tlc)/3.0);
    return bonusText;
}
//generate tile sheet
bool TileSheet::generate(unsigned int elementsInRow, unsigned int tileDimension){
    //do not generate more than once
    if(sIsGenerated) return false;
    ///initialize tile sheet
    //set tile info
    mElementsInRow = elementsInRow;
    mTileDimension = tileDimension;
    //toggle generation flag
    sIsGenerated = true;
    ///load media
    //load empty tile texture
    const auto& emptyTexture = gTextureProvider.getMedia("empty_tile");
    //load letter tile background texture
    const auto& letterBgTexture = gTextureProvider.getMedia("letter_tile_bg");
    //load fonts
    const auto& tileFont = gFontProvider.getMedia("tile_letters");
    const auto& bonusFont = gFontProvider.getMedia("bonus_text");
    ///generate tile sprites for all letter tiles ("by rendering each tile on a render-texture")
    sf::RenderTexture rTexture;
    rTexture.create(mElementsInRow * mTileDimension, (TOTAL_TILES_NUMBER/mElementsInRow+1) * mTileDimension);
    for(unsigned int j = 0; j < LETTER_TILES_NUMBER; j++){
        //temp
        char symbol = 'A' + j;
        //render background onto tile texture using tile dimension (and so forcing the bg texture to become square)
        sf::Sprite bgSprite = makeTileBgSprite(letterBgTexture, j);
        rTexture.draw(bgSprite);
        //render tile letter
        sf::Text tileLetter = makeTileLetterText(tileFont, symbol, j);
        rTexture.draw(tileLetter);
        //render tile points
        sf::Text tilePoints = makeTilePointsText(tileFont, LETTERS_POINTS.at(symbol), j);
        rTexture.draw(tilePoints);
    }
    ///generate tile sprites for all points tiles ("")
    //letter bonus tiles
    for(unsigned int j = 0; j < LETTER_BONUS_POINTS_TILES_NUMBER; j++){
        //temp
        int index = LETTER_BONUS_POINTS_TILES_SI + j;
        //render background onto tile texture using tile dimension (and so forcing the bg texture to become square)
        sf::Sprite bgSprite = makeTileBgSprite(gTextureProvider.getMedia("letter_bonus_bg"), index);
        rTexture.draw(bgSprite);
        //render tile letter
        sf::Text tileLetter = makeTileBonusText(bonusFont, PointsTile::B_LETTER, j + 2, index);
        rTexture.draw(tileLetter);
    }
    //word bonus tiles
    for(unsigned int j = 0; j < WORD_BONUS_POINTS_TILES_NUMBER; j++){
        //temp
        int index = WORD_BONUS_POINTS_TILES_SI + j;
        //render background onto tile texture using tile dimension (and so forcing the bg texture to become square)
        sf::Sprite bgSprite = makeTileBgSprite(gTextureProvider.getMedia("word_bonus_bg"), index);
        rTexture.draw(bgSprite);
        //render tile letter
        sf::Text tileLetter = makeTileBonusText(bonusFont, PointsTile::B_WORD, j + 2, index);
        rTexture.draw(tileLetter);
    }
    ///generate tile sprites for all special tiles ("")
    //tmps
    unsigned int index;
    ///start bonus point tile
    index = SPECIALT_START;
    //front image
    rTexture.draw(makeTileBgSprite(gTextureProvider.getMedia("start_bonus_tile"), index));
    ///empty tile
    index = SPECIALT_EMPTY;
    //background and only image
    rTexture.draw(makeTileBgSprite(emptyTexture, index));
    ///wild tile
    index = SPECIALT_WILD;
    //back ground
    rTexture.draw(makeTileBgSprite(letterBgTexture, index));
    /*!beetle drawing
    rTexture.draw(makeTileBgSprite(gTextureProvider.getMedia("beetle"), index));*/
    ///no graphics tile
    index = SPECIALT_NOGRAPHIC;
    //background and only image
    rTexture.draw(makeTileBgSprite(gTextureProvider.getMedia("no_graphics"), index));
    ///convert render-texture into texture
    rTexture.display();
    mTexture = rTexture.getTexture();
    ///return success
    return true;
}
//make clip rectangle of given symbol
sf::IntRect TileSheet::makeClipRect(char symbol) const {
    //check validity
    if(!sIsGenerated) return {-1,-1,-1,-1};
    //tmp
    unsigned int tileIndex;
    //letters
    if(symbol >= 'A' && symbol <= 'Z'){
        tileIndex = (unsigned int)(symbol - 'A');
        if(tileIndex >= LETTER_TILES_NUMBER) tileIndex = SPECIALT_NOGRAPHIC;
        else tileIndex += LETTER_TILES_SI;
    }
    //empty tile
    else if(symbol == ' '){
        tileIndex = SPECIALT_EMPTY;
    }
    //wild tile
    else if(symbol == '$') {
        tileIndex = SPECIALT_WILD;
    }
    //undefined symbol (assert)
    else assert(false);
    //make rectangle from tile index
    sf::IntRect retRect;
    retRect.left   = (tileIndex % mElementsInRow) * mTileDimension;
    retRect.top    = (tileIndex / mElementsInRow) * mTileDimension;
    retRect.width  = mTileDimension;
    retRect.height = mTileDimension;
    return retRect;
}
//make clip rectangle corresponding to given points tile
sf::IntRect TileSheet::makeClipRect(const PointsTile& pointsTile) const {
    //check validity
    if(!sIsGenerated) return {-1,-1,-1,-1};
    //tmp
    unsigned int tileIndex;
    //
    switch(pointsTile.bonus){
        case PointsTile::B_LETTER:
            //if points tile exists, return its sprite's clip box
            if(pointsTile.points-2 <= LETTER_BONUS_POINTS_TILES_NUMBER)
                tileIndex = LETTER_BONUS_POINTS_TILES_SI + pointsTile.points - 2;
            //else return NOGRAPHIC's
            else tileIndex = SPECIALT_NOGRAPHIC;
            //
            break;
        case PointsTile::B_WORD:
            //if points tile exists, return its sprite's clip box
            if(pointsTile.points-2 <= WORD_BONUS_POINTS_TILES_NUMBER)
                tileIndex = WORD_BONUS_POINTS_TILES_SI + pointsTile.points - 2;
            //else return NOGRAPHIC's
            else tileIndex = SPECIALT_NOGRAPHIC;
            //
            break;
        case PointsTile::B_START:
            tileIndex = SPECIALT_START;
    }
    //make rectangle from tile index
    sf::IntRect retRect;
    retRect.left   = (tileIndex % mElementsInRow) * mTileDimension;
    retRect.top    = (tileIndex / mElementsInRow) * mTileDimension;
    retRect.width  = mTileDimension;
    retRect.height = mTileDimension;
    return retRect;
}
