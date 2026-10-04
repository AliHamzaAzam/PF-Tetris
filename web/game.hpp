#pragma once
#include "../functionality.h"
#include <algorithm>

// A single update per animation frame; browser code never calls the native
// blocking menu, pause or game-over functions.
class BrowserGame {
public:
    enum class Screen {Menu,Playing,Paused,GameOver};
    Screen screen{Screen::Menu};
    GameData data{};
    RenderWindow& window;
    explicit BrowserGame(RenderWindow& value):window(value){}
    void start(){start_new_game(window,data);screen=Screen::Playing;}
    void key(Keyboard::Key key){
        if(key==Keyboard::Escape){screen=Screen::Menu;return;}
        if(key==Keyboard::P){
            if(screen==Screen::Playing)screen=Screen::Paused;
            else if(screen==Screen::Paused)screen=Screen::Playing;
            return;
        }
        if(key==Keyboard::Enter){
            if(screen==Screen::Paused)screen=Screen::Playing;
            else if(screen!=Screen::Playing)start();
            return;
        }
        if(screen!=Screen::Playing)return;
        if(key==Keyboard::Left)moving_left();
        if(key==Keyboard::Right)moving_right();
        if(key==Keyboard::Up)rotating();
        if(key==Keyboard::Down)step();
        if(key==Keyboard::Space){
            const int drop=min_drop_value();
            for(auto& cell:point_1)cell[1]+=drop;
            step();
        }
    }
    void blur(){if(screen==Screen::Playing)screen=Screen::Paused;}
    void step(){
        data.timer=data.current_delay+1;
        falling_piece(data.timer,data.current_delay,data.color_num,rand()%7);
        clear_line(data.score,data.lines_cleared);
        for(int cell:gameGrid[0])if(cell){screen=Screen::GameOver;return;}
        if(!anomaly()){screen=Screen::GameOver;return;}
        data.prime_delay=std::max(0.05f,0.3f-(data.lines_cleared/5)*0.05f);
        data.current_delay=data.prime_delay;
    }
    void update(float delta){
        if(screen!=Screen::Playing)return;
        // Discard background-tab time rather than jumping the board on resume.
        delta=std::clamp(delta,0.0f,0.1f);
        data.timer+=delta;data.bomb_timer+=delta;data.bomb_countdown-=delta;
        if(data.timer>data.current_delay)step();
        if(screen!=Screen::Playing)return;
        if(data.bomb_countdown<=0)data.bomb_fall=true;
        if(data.bomb_fall)falling_bomb(data.bomb_timer,data.current_delay,data.bomb_color,data.bomb_fall,data.bomb_countdown);
    }
};
