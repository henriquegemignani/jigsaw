#ifndef LOGIC_H_
#define LOGIC_H_

#include "SDL/SDL.h"

#include "PieceSet.h"
#include "Cursor.h"

class Layout {
  public:
    Layout(int nx, int ny, int screen_width, int screen_height) 
        : num_x_(nx), num_y_(ny), screen_width_(screen_width), screen_height_(screen_height) {}

    int num_x() const { return num_x_; }
    int num_y() const { return num_y_; }

    int total() const { return num_x_ * num_y_; }

    Vector2D ScreenToGame(Uint16 screen_x, Uint16 screen_y) const {
        return Vector2D(num_x_ * static_cast<double>(screen_x) / screen_width_,
                        num_y_ * static_cast<double>(screen_y) / screen_height_);
    }

  private:
    int num_x_, num_y_;
    int screen_width_, screen_height_;
};

class Logic {
  public:
    Logic(const Layout& layout);

    const PieceSet& pieces() const { return pieces_; }
    const Cursor& cursor() const { return cursor_; }

    void Start();
    void HandleEvent(const SDL_Event& event);

  private:
    void leftDown(const SDL_MouseButtonEvent& button);
    void rightDown(const SDL_MouseButtonEvent& button);
    void leftUp(const SDL_MouseButtonEvent& button);
    void rightUp(const SDL_MouseButtonEvent& button);


    Layout layout_;
    PieceSet pieces_;
    Cursor cursor_;
};


#endif // LOGIC_H_