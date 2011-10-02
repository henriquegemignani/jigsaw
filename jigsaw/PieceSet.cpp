#include <cstdlib>
#include "PieceSet.h"
#include "Piece.h"

PieceSet::PieceSet(int nx, int ny) : width_(nx), height_(ny) {
    pieces = new Piece**[nx];
    current = new Piece**[nx];
    for(int i = 0; i < width_; i++) {
        pieces[i] = new Piece*[ny];
        current[i] = new Piece*[ny];
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

void PieceSet::Scramble() {
	for(int i = 0; i < width_; i++)
        for(int j = 0; j < height_; j++) {
            int x = rand() % width_, y = rand() % height_;
            if(i != x || j != y)
                Swap(i, j, x, y);
		}
	/*matches = 0;
	for(int i = 0; i < width_; i++)
        for(int j = 0; j < height_; j++)
            matches += (int)(pieces[i][j]->Matches(i, j)); */
}
