#include <SFML/Graphics.hpp>
#include <cassert>
#include <iostream>
int main(){
    using K=sf::Keyboard;
    for(auto key:{K::Left,K::Right,K::Down,K::P,K::Space,K::Enter,K::Up,K::Escape})assert(sf::dispatchKey(key,false));
    for(auto key:{K::Left,K::Right,K::Down})assert(sf::dispatchKey(key,true));
    for(auto key:{K::P,K::Space,K::Enter,K::Up,K::Escape})assert(!sf::dispatchKey(key,true));
    std::cout<<"directional repeat and discrete action input passed\n";
}
