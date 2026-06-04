
// On Windows, <windows.h> defines a global `byte` that clashes with std::byte
// once utils.h does `using namespace std;`. Disable std::byte to avoid C2872.
#if defined(_WIN32)
#  define _HAS_STD_BYTE 0
#endif

#include "functionality.h"
#include <filesystem>
#include <system_error>
#if defined(_WIN32)
  #include <windows.h>
#elif defined(__APPLE__)
  #include <mach-o/dyld.h>
  #include <cstdint>
#endif

// Directory of the running executable, so GameResources/ and scores.txt resolve
// no matter where the game is launched from (Terminal or double-click).
static std::filesystem::path executableDir() {
#if defined(_WIN32)
    wchar_t buf[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, buf, MAX_PATH);
    if (len > 0 && len < MAX_PATH)
        return std::filesystem::path(std::wstring(buf, len)).parent_path();
#elif defined(__APPLE__)
    std::uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);
    std::string buf(size, '\0');
    if (size > 0 && _NSGetExecutablePath(buf.data(), &size) == 0) {
        std::error_code ec;
        auto p = std::filesystem::canonical(buf.c_str(), ec);
        if (!ec) return p.parent_path();
    }
#elif defined(__linux__)
    std::error_code ec;
    auto p = std::filesystem::canonical("/proc/self/exe", ec);
    if (!ec) return p.parent_path();
#endif
    return std::filesystem::current_path();
}


int main() {
    // Work from the executable's directory so GameResources/ and scores.txt
    // resolve no matter where the game is launched from.
    std::error_code chdirEc;
    std::filesystem::current_path(executableDir(), chdirEc);

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

    gameData.font.loadFromFile("GameResources/Skia.otf");
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
