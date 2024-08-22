
#include "functionality.h"


int main() {
    //---Game Data Initialization---//
    srand(time(nullptr));
    GameData gameData;
    Clock clock;
    Texture texture_tiles, texture_background, texture_frame, texture_gameover, texture_shadow, texture_bomb;
    RenderWindow window(VideoMode(320, 480), title);
    //---End of Game Data Initialization---//

    //---Loading Textures and Font---//
    texture_tiles.loadFromFile("GameResources/tiles.png");
    texture_background.loadFromFile("GameResources/background.png");
    texture_frame.loadFromFile("GameResources/frame.png");
    texture_shadow.loadFromFile("GameResources/shadow.png");
    texture_bomb.loadFromFile("GameResources/bomb.png");

    gameData.font.loadFromFile("GameResources/skia.otf");
    gameData.tile.setTexture(texture_tiles);
    gameData.background.setTexture(texture_background);
    gameData.frame.setTexture(texture_frame);
    gameData.shadow.setTexture(texture_shadow);
    gameData.bomb.setTexture(texture_bomb);
    //---End of Loading Textures and Font---//

    display_main_menu(window, gameData);
    //---Game Loop---//
    while (window.isOpen()) {
        //---Time Calculation Part---//
        gameData.time = clock.getElapsedTime().asSeconds();
        clock.restart();
        gameData.timer += gameData.time;
        gameData.bomb_timer += gameData.time;
        gameData.bomb_countdown -= gameData.time;
        //---End of Time Calculation Part---//

        gameData.n = rand() % 7;

        for(auto i: gameGrid[0]){
            if(i){
                gameData.fall = false;
            }
        }

        //---Event Listening---//
        event_handling(window, gameData);
        //---End of Event Listening---//

        //---Game Logic Section---//
        if (gameData.fall) {
            falling_piece(gameData.timer, gameData.current_delay, gameData.color_num, gameData.n);

            //Levels---------   Speed will increase after every 5 lines cleared with decreasing delay
            if (gameData.lines_cleared % 5 == 0 && gameData.lines_cleared != 0 && gameData.prime_delay > 0.05 && gameData.line_cleared) {
                gameData.prime_delay -= 0.05;
                gameData.line_cleared = false;
            }
            //Levels----------

            gameData.current_delay = gameData.prime_delay; //Setting the current delay to the prime delay

            if((gameData.bomb_fall = gameData.bomb_countdown <= 0)) //Bomb will fall after every 30 seconds
                falling_bomb(gameData.bomb_timer, gameData.current_delay, gameData.bomb_color, gameData.bomb_fall,
                             gameData.bomb_countdown);
        }
        gameData.line_cleared = clear_line(gameData.score, gameData.lines_cleared);
        if (gameData.rotate) {
            rotating();
            gameData.rotate = false;         //Resetting the rotate flag
        }
        //---End of Game Logic Section---//



        //---Drawing---//
        window.clear(Color::Black);
        window.draw(gameData.background);
        drawing_blocks(window, gameData.tile, gameData.shadow, gameData.bomb, gameData.color_num, gameData.bomb_color, gameData.bomb_fall);
        drawing_score(window, gameData.score, gameData.font);
        window.draw(gameData.frame);
        //---End of Drawing---//

        //---Game Over Section---//
        if (!gameData.fall) {
            string name = display_gameover(window, gameData);
            file_handling(name, gameData.score);
            start_new_game(window, gameData);
            display_main_menu(window, gameData);
        }
        //---End of Game Over Section---//
        window.display();
    }
    //---End of Game Loop---//

    return 0;
}
