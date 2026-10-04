#include "../../functionality.h"
#include <cassert>
#include <cstring>
#include <iostream>

void reset(){std::memset(gameGrid,0,sizeof gameGrid);}
int main(int argc,char** argv){
    assert(argc==2);
    const std::string test=argv[1];
    reset();
    if(test=="ghost"){
        int shape[4][2]={{3,0},{3,1},{3,2},{3,3}};
        std::memcpy(point_1,shape,sizeof shape);
        gameGrid[4][3]=2;
        gameGrid[19][3]=2;
        assert(min_drop_value()==0);
    } else if(test=="rotation"){
        int shape[4][2]={{3,4},{3,5},{3,6},{3,7}};
        std::memcpy(point_1,shape,sizeof shape);
        gameGrid[5][2]=2;
        rotating();
        assert(std::memcmp(point_1,shape,sizeof shape)==0);
    } else if(test=="bomb"){
        bomb_point[0]=M-1;bomb_point[1]=N-1;
        float timer=1,delay=.3f,countdown=0;int color=1;bool fall=true;
        falling_bomb(timer,delay,color,fall,countdown);
        assert(bomb_point[0]==0 && !fall && countdown>=7);
    } else if(test=="rows"){
        gameGrid[0][1]=4;
        for(int x=0;x<N;x++)gameGrid[M-1][x]=1;
        int score=0,lines=0;
        assert(clear_line(score,lines));
        assert(score==10 && lines==1);
        assert(gameGrid[0][1]==0 && gameGrid[1][1]==4);
    } else if(test=="negative"){
        int shape[4][2]={{3,-1},{3,0},{3,1},{3,2}};
        std::memcpy(point_1,shape,sizeof shape);
        assert(!anomaly());
    } else {return 2;}
    std::cout << test << " passed\n";
}
