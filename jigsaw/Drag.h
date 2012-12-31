#ifndef DRAG_H_
#define PIECESET_H_

class Piece;
class Drag {
public:
	Drag(void);
	~Drag(void);

private:
	int selected_x, selected_y;
    double offset_x, offset_y;
    double drag_offset_x, drag_offset_y;
    double drag_end_x, drag_end_y;
    int rectsizex, rectsizey;
    bool leftdrag, rightdrag, rect_up;
};

#endif

