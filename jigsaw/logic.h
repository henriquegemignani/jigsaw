#ifndef LOGIC_H_
#define LOGIC_H_

#include "SDL/SDL_events.h"

#include "PieceSet.h"
#include "Cursor.h"
#include "Layout.h"
#include "Selection.h"

class Logic {
  public:
    Logic(const Layout& layout);

    const PieceSet& pieces() const { return pieces_; }
    const Cursor& cursor() const { return cursor_; }
    const Selection& selection() const { return selection_; }

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
    Selection selection_;
};


#endif // LOGIC_H_