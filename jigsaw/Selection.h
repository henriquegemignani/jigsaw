#ifndef SELECTION_H_
#define SELECTION_H_

#include "Vector2D.h"
#include "Color.h"
#include "PieceSet.h"

class Selection {
public:
	Selection(PieceSet *set) : pieces(set), is_active(false), is_dragging(false) {}
	~Selection(void);
	void Render(Vector2D mousepos);

	void StartDrag(Vector2D origin);
	void EndDrag(Vector2D target);
	void Clear();

private:
	PieceSet *pieces;
	Vector2D start, end, offset;
	bool is_active, is_dragging;
	int selected_x, selected_y;

	void CalculateDragOffset(Vector2D origin);
	void UpdateColors(Color &c);
};

#endif
