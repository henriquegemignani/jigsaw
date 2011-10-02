#ifndef PIECE_H_
#define PIECE_H_

#include "Vector2D.h"
#include "Color.h"

class Piece {
public:
    Piece(int x, int y, int nx, int ny);
    virtual ~Piece();
    void Render();
    void CustomRender(float x, float y);
    void Move(int x, int y);
    bool Matches(int x, int y);

	void set_color(Color c) { color = c; }
	void set_alpha(float a) { alpha = a; }
	void QueueRenderPosition(Vector2D pos) {
		queue_pos = pos;
		position_queued = true;
	}
private:
	Vector2D position, size, tex_origin, queue_pos;
	bool position_queued;
	Color color;
	float alpha;
    void InternalRender();
};

#endif /* PIECE_H_ */
