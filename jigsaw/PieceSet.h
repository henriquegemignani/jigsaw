#ifndef PIECESET_H_
#define PIECESET_H_

class Layout;
class Piece;
class PieceSet {
public:
    PieceSet(const Layout&);
	~PieceSet(void);

	void Scramble();
	void Swap(int sx, int sy, int tx, int ty);

	int width() const { return width_; }
	int height() const { return height_; }
	int get_matches() const { return matches; }
	Piece* get_piece(int i, int j) const { return pieces[i][j]; }
	Piece* get_current(int i, int j) const { return current[i][j]; }

private:
	int width_, height_;
	int matches;
	Piece ***pieces, ***current;
};

#endif

