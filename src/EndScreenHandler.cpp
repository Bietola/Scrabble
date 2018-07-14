#include "handlers.h"

///EndScreenHandler functions
//constructor
EndScreenHandler::EndScreenHandler(sf::RenderWindow& window, std::vector<Player>&& players):
    Handler(window),
    mEndText("Game Over", gFontProvider.getMedia("tile_letters")),
    mPlayers(std::move(players))
{
    mEndText.setOrigin(mEndText.getGlobalBounds().left / 2.f, mEndText.getGlobalBounds().top / 2.f);
    mEndText.setPosition(mWindow.getSize().x / 2.f, mWindow.getSize().y / 2.f);
    for(const auto& player : mPlayers){
        std::cout << player.getName() << ": " << player.getPoints() << "." << std::endl;
    }
}
///ui functions
//handle user event
void EndScreenHandler::handleEvent(const sf::Event& e) {
    ;
}
//handle logical operations
void EndScreenHandler::handleLogic(const sf::Time& time) {
    ;
}
//draw interface
void EndScreenHandler::handleDrawing() const {
    mWindow.draw(mEndText);
}
