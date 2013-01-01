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
    ~Logic();

    const PieceSet& pieces() const { return pieces_; }
    const Cursor& cursor() const { return cursor_; }
    const Selection& dragged() const { return dragged_; }
    const Selection& selection() const { return selection_; }
    int moves() const { return moves_; }
    const Vector2D* selecting_origin() const { return selecting_origin_; }

    void Start();
    void Start(unsigned int seed);
    void HandleEvent(const SDL_Event& event);

  private:
    void leftDown();
    void rightDown();
    void leftUp();
    void rightUp();

    Layout layout_;
    PieceSet pieces_;
    Cursor cursor_;
    Selection dragged_;
    Selection selection_;
    int moves_;
    Vector2D* selecting_origin_;
};


#endif // LOGIC_H_