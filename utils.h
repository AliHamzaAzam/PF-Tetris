//---Title: PF-Project, Fall-2022 for BS(CS)---//

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <sstream>
#include <string>
#include <ctime>
#include<iostream>
#include<fstream>
using namespace sf;
using namespace std;

const char title[] = "PF-Project, Fall-2022";

//---Height and Width of the Actual Interactive Game---//
const int M = 20;   //Number of rows for a piece to cover on the screen (not the entire screen) = 20
const int N = 10;   //Number of columns for a piece to cover on the screen (not the entire screen) = 10

//---The Actual Interactive Game Grid - Built Over (MxN)---//
int gameGrid[M][N] = {0};

/*
* Shape of each piece is represented by rows in the array.
* TIP: Name the array what is already been coded to avoid any unwanted errors.
*/
int BLOCKS[7][4]={{0,2,4,6},{0,1,2,3},{1,2,3,4},{0,2,3,5},{1,3,4,5},{0,2,4,5},{1,2,3,5}};

//---To Hold the Coordinates of the Piece---//
int point_1[4][2], point_2[4][2];

//---To Hold the Coordinates of the bomb---//
int bomb_point[2] = {0, 0};

//---Check Uncertain Conditions---//
bool anomaly() {
    for (auto & i : point_1)
        if (i[0] < 0 || i[0] >= N || i[1] >= M || (gameGrid[i[1]][i[0]]) != 0)
            return false;
    return true;
}

//---Struct to Hold the Game Data---//
struct GameData {
    int delta_x, color_num, score, lines_cleared, bomb_color, n;
    float timer, prime_delay, current_delay, time, bomb_timer, bomb_countdown;
    bool rotate, fall, is_paused, line_cleared, bomb_fall;
    Font font;
    Sprite tile, background, frame, gameover, shadow, bomb;
};