#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "media.h"

#include "Grappa/interface/Button.h"
#include "Grappa/interface/ButtonFunctions.h"
#include "Grappa/interface/PictureBox.h"
#include "Grappa/interface/TextBox.h"

#include "TileSheet.h"
#include "Board.h"
#include "Dictionary.h"
#include "handlers.h"

#define SCREEN_W 1250
#define SCREEN_H 800

int main()
{   
    ///initialization
    //seed random gen
    srand(time(NULL));

    //create window
    sf::RenderWindow window(sf::VideoMode(SCREEN_W, SCREEN_H), "Scrabble");

    //load media
    if(!load_media())
        std::cout << "ERROR: could not load media" << std::endl;

    //generate tile sheet
    TileSheet tileSheet;
    tileSheet.generate(6, 500);

    //load dictionary
    Dictionary dictionary("dictionaries/ita.txt");

    //generate board
    Board board("boards/scarabeo.txt", dictionary);

    //initialize turn handler
    Handler* gameHandler = new TurnHandler(tileSheet, window, std::move(board), "players.txt");

    ///main loop
    sf::Event e;
    sf::Clock frameClock;
    while(window.isOpen()) {
        //event handling
        while(window.pollEvent(e)) {
            if(e.type == sf::Event::Closed) window.close();
            gameHandler->handleEvent(e);
        }

        //logic
        gameHandler->handleLogic(frameClock.getElapsedTime());

        //graphics
        window.clear();
        gameHandler->handleDrawing();
        window.display();

        //attempt to change handler
        Handler::change(gameHandler);

        //reset frame clock
        frameClock.restart();
    }
}
