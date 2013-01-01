#include "PieceSet.h"

#include <iostream>
#include <cstdlib>
#include <random>

#include "Piece.h"
#include "Layout.h"

PieceSet::PieceSet(const Layout& layout) : width_(layout.num_x()), height_(layout.num_y()) {
    pieces = new Piece**[width_];
    current = new Piece**[width_];
    for(int i = 0; i < width_; i++) {
        pieces[i] = new Piece*[height_];
        current[i] = new Piece*[height_];
        for(int j = 0; j < height_; j++)
            current[i][j] = pieces[i][j] = new Piece(i, j, width_, height_);
    }
	matches = width_ * height_;
}

PieceSet::~PieceSet(void) {
	for(int i = 0; i < width_; i++) {
		for(int j = 0; j < height_; j++)
            delete current[i][j];
        delete pieces[i];
        delete current[i];
    }
	delete pieces;
	delete current;
}

void PieceSet::Swap(int sx, int sy, int tx, int ty) {
	int change = (int)(pieces[sx][sy]->Matches(sx, sy))
            + (int)(pieces[tx][ty]->Matches(tx, ty));
    
	current[sx][sy]->Move(tx, ty);
    current[tx][ty]->Move(sx, sy);
    Piece *aux = current[tx][ty];
    current[tx][ty] = current[sx][sy];
    current[sx][sy] = aux;

	change -= (int)(pieces[sx][sy]->Matches(sx, sy))
            + (int)(pieces[tx][ty]->Matches(tx, ty));
	matches -= change;
}

void PieceSet::Scramble(int seed) {
    std::default_random_engine e(seed);
	for(int i = 0; i < width_; i++)
        for(int j = 0; j < height_; j++) {
            int x = e() % width_, y = e() % height_;
            if(i != x || j != y)
                Swap(i, j, x, y);
		}
}
