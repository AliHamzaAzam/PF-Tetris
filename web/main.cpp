#include "game.hpp"
#include <emscripten.h>
#include <stdexcept>

static RenderWindow* window;
static BrowserGame* game;
static Texture tiles,background,frame,shadow,bomb;
static Clock frameClock;

static void label(const std::string& value,int y,int size=24){
    Text text;text.setFont(game->data.font);text.setString(value);text.setCharacterSize(size);
    auto bounds=text.getLocalBounds();text.setOrigin(bounds.width/2,0);text.setPosition(160,y);window->draw(text);
}
static void tick(){
    Event event;
    while(window->pollEvent(event)){
        if(event.type==Event::LostFocus)game->blur();
        if(event.type==Event::KeyPressed)game->key(event.key.code);
    }
    game->update(frameClock.restart().asSeconds());
    window->clear();window->draw(game->data.background);
    if(game->screen!=BrowserGame::Screen::Menu){
        drawing_blocks(*window,game->data.tile,game->data.shadow,game->data.bomb,game->data.color_num,game->data.bomb_color,game->data.bomb_fall);
        drawing_score(*window,game->data.score,game->data.font);window->draw(game->data.frame);
    }
    if(game->screen!=BrowserGame::Screen::Playing){
        RectangleShape overlay(Vector2f(320,480));overlay.setFillColor(Color(9,15,31,215));window->draw(overlay);
        if(game->screen==BrowserGame::Screen::Menu){label("PF Tetris",140,38);label("Enter to start",220);}
        if(game->screen==BrowserGame::Screen::Paused){label("Paused",160,38);label("P or Enter to resume",230,21);}
        if(game->screen==BrowserGame::Screen::GameOver){label("Game over",130,36);label("Score: "+std::to_string(game->data.score),195);label("Enter to play again",255,22);}
    }
    window->display();
}
extern "C" EMSCRIPTEN_KEEPALIVE void start_game(){game->start();frameClock.restart();}
extern "C" EMSCRIPTEN_KEEPALIVE void pause_game(){game->blur();frameClock.restart();}
int main(){
    srand(static_cast<unsigned>(time(nullptr)));
    window=new RenderWindow(VideoMode(320,480),"PF Tetris");
    game=new BrowserGame(*window);
    if(!window->isOpen() || !tiles.loadFromFile("GameResources/tiles.png") ||
       !background.loadFromFile("GameResources/background.png") || !frame.loadFromFile("GameResources/frame.png") ||
       !shadow.loadFromFile("GameResources/shadow.png") || !bomb.loadFromFile("GameResources/bomb.png") ||
       !game->data.font.loadFromFile("GameResources/Skia.otf")){
        EM_ASM({ window.dispatchEvent(new Event('pf-load-error')); });return 1;
    }
    game->data.tile.setTexture(tiles);game->data.background.setTexture(background);game->data.frame.setTexture(frame);
    game->data.shadow.setTexture(shadow);game->data.bomb.setTexture(bomb);
    emscripten_set_main_loop(tick,0,1);
}
