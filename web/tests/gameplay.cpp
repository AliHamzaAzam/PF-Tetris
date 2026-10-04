#include "../game.hpp"
#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>

static int occupied(){int count=0;for(auto& row:gameGrid)for(int cell:row)count+=cell!=0;return count;}
int main(){
    RenderWindow window(VideoMode(320,480),"test");BrowserGame game(window);
    std::srand(42);
    // Every original shape must lock exactly four cells and produce a valid spawn.
    for(int shape=0;shape<7;++shape){
        game.start();
        for(int i=0;i<4;++i){point_1[i][0]=BLOCKS[shape][i]%2+4;point_1[i][1]=BLOCKS[shape][i]/2;}
        game.key(Keyboard::Space);assert(occupied()==4 && anomaly());
        int board[M][N];std::memcpy(board,gameGrid,sizeof board);
        game.key(Keyboard::Right);assert(std::memcmp(board,gameGrid,sizeof board)==0);
    }
    const int scores[]={0,10,30,60,100};
    for(int lines=1;lines<=4;++lines){
        game.start();for(int y=M-lines;y<M;++y)for(int& cell:gameGrid[y])cell=1;
        int score=0,total=3;assert(clear_line(score,total));
        assert(score==scores[lines] && total==3+lines && occupied()==0);
    }
    // Crossing five cleared lines changes gravity without needing an exact multiple.
    game.start();game.data.lines_cleared=4;
    for(int y=M-2;y<M;++y)for(int& cell:gameGrid[y])cell=1;
    game.step();assert(game.data.lines_cleared==6);
    assert(std::abs(game.data.current_delay-.25f)<.00001f);
    game.start();game.data.lines_cleared=9;
    for(int y=M-2;y<M;++y)for(int& cell:gameGrid[y])cell=1;
    game.step();assert(game.data.lines_cleared==11);
    assert(std::abs(game.data.current_delay-.20f)<.00001f);
    game.data.lines_cleared=100;game.step();assert(std::abs(game.data.current_delay-.05f)<.00001f);
    // Matching bomb colors clear the board; different colors preserve distant cells.
    for(bool match:{false,true}){
        game.start();gameGrid[5][2]=match?3:4;gameGrid[19][9]=7;
        bomb_point[0]=4;bomb_point[1]=2;
        float timer=1,delay=.3f,countdown=0;int color=3;bool falling=true;
        falling_bomb(timer,delay,color,falling,countdown);
        assert(!falling && countdown>=7 && countdown<=13 && bomb_point[0]==0);
        assert(gameGrid[5][2]==0 && gameGrid[19][9]==(match?0:7));
    }
    for(int bombColor:{0,3}){
        game.start();gameGrid[10][5]=7;bomb_point[0]=19;bomb_point[1]=9;
        float timer=1,delay=.3f,countdown=0;bool falling=true;
        falling_bomb(timer,delay,bombColor,falling,countdown);
        assert(gameGrid[10][5]==7 && !falling);
    }
    game.start();game.key(Keyboard::P);
    const auto timer=game.data.timer,bombTimer=game.data.bomb_timer,countdown=game.data.bomb_countdown;
    game.update(20);assert(game.data.timer==timer && game.data.bomb_timer==bombTimer && game.data.bomb_countdown==countdown);
    // Native top-out applies to any occupied top-row column, not just spawn cells.
    game.start();
    for(int i=0;i<4;++i){point_1[i][0]=i+5;point_1[i][1]=0;gameGrid[1][i+5]=2;}
    game.step();assert(game.screen==BrowserGame::Screen::GameOver);
    // Mixed controls exercise complete games under ASan/UBSan, including bombs.
    for(int round=0;round<100;++round){
        game.start();
        for(int frame=0;frame<2000 && game.screen==BrowserGame::Screen::Playing;++frame){
            const Keyboard::Key keys[]={Keyboard::Left,Keyboard::Right,Keyboard::Up,Keyboard::Down,Keyboard::Space};
            if(frame%3==0)game.key(keys[std::rand()%5]);
            game.update(.1f);
            if(game.screen==BrowserGame::Screen::Playing)assert(anomaly());
            for(auto& row:gameGrid)for(int cell:row)assert(cell>=0 && cell<=7);
        }
    }
    std::cout<<"gameplay parity and 100 randomized games passed\n";
}
