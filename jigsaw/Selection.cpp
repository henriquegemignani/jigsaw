#include "Selection.h"
#include "Piece.h"

Selection::~Selection(void) {
}

void Selection::Render(Vector2D mousepos) {
}

void Selection::StartDrag(Vector2D origin) {
	is_dragging = true;
	if(is_active) {
		CalculateDragOffset(origin);
		UpdateColors(Color(1.0, 1.0, 1.0));
	} else {
		start = origin;
	}
}

void Selection::EndDrag(Vector2D target) {
	is_dragging = false;
	if(is_active) {
	} else {
		is_active = true;
		end = target;
		UpdateColors(Color(0.5, 0.5, 1.0));
	}
}

void Selection::Clear() {
	is_active = is_dragging = false;
}

void Selection::CalculateDragOffset(Vector2D origin) {
	offset.x = (double)(pieces->width()) * origin.x;
	offset.y = (double)(pieces->height()) * origin.y;

	// Store the piece we're dragging.
	selected_x = (int)(offset.x);
	selected_y = (int)(offset.y);

	// Store the position of the piece relative to the mouse
	offset.x = (selected_x - offset.x) / pieces->width();
	offset.y = (selected_y - offset.y) / pieces->height();
}

void Selection::UpdateColors(Color &c) {
	int startx = (int)(start.x);
	int starty = (int)(start.y);
	
	for(int i = (int)(start.x); i < (int)(end.x); i++)
		for(int j = (int)(start.y); j < (int)(end.y); j++) {
			pieces->get_current(i, j)->set_color(c);
		}
}