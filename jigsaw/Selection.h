#ifndef SELECTION_H_
#define SELECTION_H_

#include <set>
#include "Piece.h"
#include "Vector2D.h"

struct Selection {
    Vector2D origin, topleft;
	std::set<Piece*> pieces;
};

#endif // SELECTION_H_
