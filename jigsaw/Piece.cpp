#include "Piece.h"
#include "SDL/SDL_opengl.h"
#include <cmath>
#include <cstdio>

Piece::Piece(int x, int y, int nx, int ny) : color(1,1,1), alpha(1.0) {
	size = Vector2D(1.0 / nx, 1.0 / ny);
	position = Vector2D(x, y);
	tex_origin = Vector2D((x)/(double)(nx), (y)/(double)(ny));
}

void Piece::Move(int x, int y) {
    position.x = x;
    position.y = y;
}

bool Piece::Matches(int x, int y) {
	Vector2D diff = position - Vector2D(x, y);
    return diff.NormOne() < 1.0E-6;
}

Piece::~Piece() {
    // TODO Auto-generated destructor stub
}

void Piece::CustomRender(const Cursor& cursor, const Vector2D& topleft) const {
    Vector2D total = (cursor.position + position - topleft - cursor.offset).Scale(size);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(total.x, total.y, 0.0);
    //Start quad
    internalRender();
    //Reset
    glPopMatrix();
}

void Piece::Render() {
    Vector2D total = position.Scale(size);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(total.x, total.y, 0);
    //Start quad
    internalRender();
    //Reset
    glPopMatrix();
}

void Piece::internalRender() const {
    //Start quad
    glBegin( GL_QUADS );
        //Set color to white
        glColor4d( color.r, color.g, color.b, alpha );

        //Draw square
        glTexCoord2d(tex_origin.x, tex_origin.y);
        glVertex2d(0, 0);

        //glTexCoord2d(tex_end_x, tex_origin_y);
		glTexCoord2d(tex_origin.x + size.x, tex_origin.y);
        glVertex2d(size.x, 0);

        //glTexCoord2d(tex_end_x, tex_end_y);
		glTexCoord2d(tex_origin.x + size.x, tex_origin.y + size.y);
        glVertex2d(size.x, size.y);

        //glTexCoord2d(tex_origin_x, tex_end_y);
		glTexCoord2d(tex_origin.x, tex_origin.y + size.y);
        glVertex2d(0, size.y);
    //End quad
    glEnd();
}
