#pragma once
// Only the SFML graphics API used by PF-Tetris. Native builds still use SFML.
#include <algorithm>
#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>
#ifndef PF_HEADLESS_TEST
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#endif
namespace sf {
struct Vector2u { unsigned x{}, y{}; };
struct Vector2f { float x{}, y{}; Vector2f(float a, float b): x(a), y(b) {} };
struct IntRect { int left{}, top{}, width{}, height{}; IntRect() = default; IntRect(int x,int y,int w,int h):left(x),top(y),width(w),height(h){} };
struct FloatRect { float left{},top{},width{},height{}; };
struct Color {
    unsigned char r{},g{},b{},a{255};
    Color()=default;
    Color(int red,int green,int blue,int alpha=255):r(red),g(green),b(blue),a(alpha){}
    static const Color Black,White;
};
inline const Color Color::Black{0,0,0}, Color::White{255,255,255};
struct VideoMode { unsigned width,height; VideoMode(unsigned w,unsigned h):width(w),height(h){} };
struct Time { float seconds; float asSeconds() const{return seconds;} };
class Clock {
    std::chrono::steady_clock::time_point last=std::chrono::steady_clock::now();
public:
    Time getElapsedTime() const {return {std::chrono::duration<float>(std::chrono::steady_clock::now()-last).count()};}
    Time restart(){auto elapsed=getElapsedTime();last=std::chrono::steady_clock::now();return elapsed;}
};
struct Keyboard { enum Key {Unknown,Left,Right,Up,Down,Space,Escape,P,Enter}; };
inline bool dispatchKey(Keyboard::Key key, bool repeat){
    return !repeat || key==Keyboard::Left || key==Keyboard::Right || key==Keyboard::Down;
}
struct Event {
    enum EventType {Closed,KeyPressed,TextEntered,LostFocus,GainedFocus};
    EventType type{};
    struct {Keyboard::Key code{};} key;
    struct {unsigned unicode{};} text;
};
#ifndef PF_HEADLESS_TEST
inline SDL_Renderer* renderer=nullptr;
#endif
class Texture {
public:
    int width{},height{};
#ifndef PF_HEADLESS_TEST
    std::shared_ptr<SDL_Texture> handle;
#endif
    bool loadFromFile(const std::string& path) {
#ifndef PF_HEADLESS_TEST
        handle.reset(IMG_LoadTexture(renderer,path.c_str()),SDL_DestroyTexture);
        if(!handle)return false;
        SDL_QueryTexture(handle.get(),nullptr,nullptr,&width,&height);
        SDL_SetTextureBlendMode(handle.get(),SDL_BLENDMODE_BLEND);
#else
        (void)path;width=320;height=480;
#endif
        return true;
    }
};
struct Sprite {
    const Texture* texture{}; IntRect rect{}; float x{},y{};
    void setTexture(const Texture& value){texture=&value;rect={0,0,value.width,value.height};}
    void setTextureRect(IntRect value){rect=value;}
    void setPosition(float a,float b){x=a;y=b;}
    void move(float a,float b){x+=a;y+=b;}
};
class Font {
    std::string path;
#ifndef PF_HEADLESS_TEST
    mutable std::unordered_map<unsigned,std::shared_ptr<TTF_Font>> sizes;
#endif
public:
    bool loadFromFile(const std::string& value){
        if(path!=value){path=value;
#ifndef PF_HEADLESS_TEST
            sizes.clear();
#endif
        }
#ifndef PF_HEADLESS_TEST
        return get(20)!=nullptr;
#else
        return true;
#endif
    }
#ifndef PF_HEADLESS_TEST
    TTF_Font* get(unsigned size) const {
        auto& font=sizes[size];
        if(!font)font.reset(TTF_OpenFont(path.c_str(),static_cast<int>(size)),TTF_CloseFont);
        return font.get();
    }
#endif
};
struct Text {
    enum Style {Bold=1};
    const Font* font{};std::string string;unsigned size{30};Color color=Color::White;
    float x{},y{},ox{},oy{};
    void setFont(const Font& v){font=&v;}
    void setStyle(Style){} // Existing font carries its own weight.
    void setFillColor(Color v){color=v;}
    void setString(const std::string& v){string=v;}
    void setCharacterSize(unsigned v){size=v;}
    void setOrigin(float a,float b){ox=a;oy=b;}
    void setPosition(float a,float b){x=a;y=b;}
    void move(float a,float b){x+=a;y+=b;}
    FloatRect getLocalBounds() const {
        int w=static_cast<int>(string.size()*size/2),h=static_cast<int>(size);
#ifndef PF_HEADLESS_TEST
        if(font && font->get(size))TTF_SizeUTF8(font->get(size),string.c_str(),&w,&h);
#endif
        return {0,0,static_cast<float>(w),static_cast<float>(h)};
    }
};
struct RectangleShape {
    Vector2f size;Color color;float x{},y{};
    explicit RectangleShape(Vector2f v):size(v){}
    void setFillColor(Color v){color=v;}
    void setPosition(float a,float b){x=a;y=b;}
};
class RenderWindow {
    bool open{true};Vector2u size;
#ifndef PF_HEADLESS_TEST
    SDL_Window* window{};
#endif
public:
    RenderWindow(VideoMode mode,const char* title):size{mode.width,mode.height}{
#ifndef PF_HEADLESS_TEST
        SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "#canvas");
        if(SDL_Init(SDL_INIT_VIDEO)!=0 || TTF_Init()!=0){open=false;return;}
        IMG_Init(IMG_INIT_PNG);
        window=SDL_CreateWindow(title,0,0,mode.width,mode.height,SDL_WINDOW_SHOWN);
        renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
        if(!renderer){open=false;return;}
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
#else
        (void)title;
#endif
    }
    bool isOpen() const{return open;}
    void close(){open=false;}
    Vector2u getSize() const{return size;}
    bool pollEvent(Event& out){
#ifndef PF_HEADLESS_TEST
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT){out.type=Event::Closed;return true;}
            if(event.type==SDL_WINDOWEVENT && event.window.event==SDL_WINDOWEVENT_FOCUS_LOST){out.type=Event::LostFocus;return true;}
            if(event.type!=SDL_KEYDOWN)continue;
            out.type=Event::KeyPressed;
            switch(event.key.keysym.sym){
                case SDLK_LEFT:out.key.code=Keyboard::Left;break;
                case SDLK_RIGHT:out.key.code=Keyboard::Right;break;
                case SDLK_UP:out.key.code=Keyboard::Up;break;
                case SDLK_DOWN:out.key.code=Keyboard::Down;break;
                case SDLK_SPACE:out.key.code=Keyboard::Space;break;
                case SDLK_ESCAPE:out.key.code=Keyboard::Escape;break;
                case SDLK_p:out.key.code=Keyboard::P;break;
                case SDLK_RETURN:out.key.code=Keyboard::Enter;break;
                default:continue;
            }
            if(!dispatchKey(out.key.code,event.key.repeat!=0))continue;
            return true;
        }
#else
        (void)out;
#endif
        return false;
    }
    void clear(Color color=Color::Black){
#ifndef PF_HEADLESS_TEST
        SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);SDL_RenderClear(renderer);
#else
        (void)color;
#endif
    }
    void draw(const Sprite& sprite){
#ifndef PF_HEADLESS_TEST
        if(!sprite.texture || !sprite.texture->handle)return;
        SDL_Rect from{sprite.rect.left,sprite.rect.top,sprite.rect.width,sprite.rect.height};
        SDL_Rect to{static_cast<int>(sprite.x),static_cast<int>(sprite.y),from.w,from.h};
        SDL_RenderCopy(renderer,sprite.texture->handle.get(),&from,&to);
#else
        (void)sprite;
#endif
    }
    void draw(const Text& text){
#ifndef PF_HEADLESS_TEST
        if(!text.font || text.string.empty())return;
        TTF_Font* font=text.font->get(text.size);if(!font)return;
        SDL_Color color{text.color.r,text.color.g,text.color.b,text.color.a};
        SDL_Surface* surface=TTF_RenderUTF8_Blended_Wrapped(font,text.string.c_str(),color,320);
        if(!surface)return;
        SDL_Texture* texture=SDL_CreateTextureFromSurface(renderer,surface);
        if(!texture){SDL_FreeSurface(surface);return;}
        SDL_Rect destination{static_cast<int>(text.x-text.ox),static_cast<int>(text.y-text.oy),surface->w,surface->h};
        SDL_RenderCopy(renderer,texture,nullptr,&destination);
        SDL_DestroyTexture(texture);SDL_FreeSurface(surface);
#else
        (void)text;
#endif
    }
    void draw(const RectangleShape& rect){
#ifndef PF_HEADLESS_TEST
        SDL_SetRenderDrawColor(renderer,rect.color.r,rect.color.g,rect.color.b,rect.color.a);
        SDL_Rect to{static_cast<int>(rect.x),static_cast<int>(rect.y),static_cast<int>(rect.size.x),static_cast<int>(rect.size.y)};
        SDL_RenderFillRect(renderer,&to);
#else
        (void)rect;
#endif
    }
    void display(){
#ifndef PF_HEADLESS_TEST
        SDL_RenderPresent(renderer);
#endif
    }
};
}
