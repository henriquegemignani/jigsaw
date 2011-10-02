#ifndef PIECESET_H_
#define PIECESET_H_

class Piece;

class PieceSet {
public:
	PieceSet(int nx, int ny);
	~PieceSet(void);

	void Scramble();
	void Swap(int sx, int sy, int tx, int ty);

	int width() { return width_; }
	int height() { return height_; }
	int get_matches() { return matches; }
	Piece* get_piece(int i, int j) { return pieces[i][j]; }
	Piece* get_current(int i, int j) { return current[i][j]; }

private:
	int width_, height_;
	int matches;
	Piece ***pieces, ***current;
};

#endif

