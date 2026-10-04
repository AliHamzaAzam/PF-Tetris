#include "../game.hpp"
#include <cassert>
#include <cstring>
#include <iostream>
int main(){
    RenderWindow window(VideoMode(320,480),"test");BrowserGame game(window);
    assert(game.screen==BrowserGame::Screen::Menu);
    game.key(Keyboard::Enter);assert(game.screen==BrowserGame::Screen::Playing);
    assert(game.data.score==0 && game.data.lines_cleared==0);
    for(auto& cell:point_1)cell[0]+=4;
    int x=point_1[0][0];game.key(Keyboard::Left);assert(point_1[0][0]==x-1);
    game.key(Keyboard::Right);assert(point_1[0][0]==x);
    int y=point_1[0][1];game.key(Keyboard::Down);assert(point_1[0][1]==y+1);
    game.key(Keyboard::P);auto timer=game.data.timer;game.update(1);
    assert(game.screen==BrowserGame::Screen::Paused && game.data.timer==timer);
    game.key(Keyboard::Enter);assert(game.screen==BrowserGame::Screen::Playing);
    game.blur();assert(game.screen==BrowserGame::Screen::Paused);
    game.key(Keyboard::P);game.key(Keyboard::Space);
    int occupied=0;for(auto& row:gameGrid)for(int cell:row)occupied+=cell!=0;
    assert(occupied==4);assert(anomaly());
    game.key(Keyboard::Escape);assert(game.screen==BrowserGame::Screen::Menu);
    game.key(Keyboard::Enter);assert(game.data.score==0);
    for(auto& row:gameGrid)for(int cell:row)assert(cell==0);
    game.data.timer=0;game.update(100);assert(game.data.timer<=.1f);
    // Clearing the top row must happen before validating the spawned piece.
    game.start();
    for(int col=0;col<N;col++)gameGrid[0][col]=1;
    for(int i=0;i<4;i++){point_1[i][0]=i+5;point_1[i][1]=19;}
    game.step();assert(game.screen==BrowserGame::Screen::Playing);
    game.start();
    // Lock a piece above a blocked spawn. Game-over must not open a nested loop.
    for(auto& cell:point_1){cell[0]=9;cell[1]=19;}
    for(int row=0;row<4;row++)for(int col=0;col<2;col++)gameGrid[row][col]=1;
    game.step();assert(game.screen==BrowserGame::Screen::GameOver);
    game.key(Keyboard::Enter);assert(game.screen==BrowserGame::Screen::Playing);
    std::cout<<"lifecycle passed\n";
}
