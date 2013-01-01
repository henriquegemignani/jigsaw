#ifndef PIECE_H_
#define PIECE_H_

#include "Vector2D.h"
#include "Cursor.h"
#include "Color.h"

class Piece {
  public:
    Piece(int x, int y, int nx, int ny);
    virtual ~Piece();

    void Render();
    void CustomRender(const Cursor&, const Vector2D&) const;

    void Move(int x, int y);
    bool Matches(int x, int y);

	void set_color(Color c) { color = c; }
	void set_alpha(double a) { alpha = a; }

  private:
    void internalRender() const;

	Vector2D position, size, tex_origin;
	Color color;
	double alpha;
};

#endif /* PIECE_H_ */
