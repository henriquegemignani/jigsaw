#include "Piece.h"
#include "SDL/SDL_opengl.h"
#include <cmath>
#include <cstdio>

Piece::Piece(int x, int y, int nx, int ny) : color(1,1,1), alpha(1.0f) {
	size = Vector2D(1.0f / nx, 1.0f / ny);
	position = Vector2D(x * size.x, y * size.y);
	tex_origin = Vector2D((x)/(float)(nx), (y)/(float)(ny));
	position_queued = false;
}

void Piece::Move(int x, int y) {
    position.x = x * size.x;
    position.y = y * size.y;
}

bool Piece::Matches(int x, int y) {
	Vector2D diff = position - Vector2D(x * size.x, y * size.y);
	return diff.norm_one() < 1.0E-6;
}

Piece::~Piece() {
    // TODO Auto-generated destructor stub
}

void Piece::CustomRender(float x, float y) {
    glPushMatrix();
    glLoadIdentity();
    glTranslatef( x, y, 0 );
    //Start quad
    InternalRender();
    //Reset
    glPopMatrix();
}

void Piece::Render() {
    glPushMatrix();
    glLoadIdentity();
	if(!position_queued) {
		glTranslatef( position.x, position.y, 0 );
	} else {
		glTranslatef( queue_pos.x, queue_pos.y, 0.1f );
		position_queued = false;
	}
    //Start quad
    InternalRender();
    //Reset
    glPopMatrix();
}

void Piece::InternalRender() {
    //Start quad
    glBegin( GL_QUADS );
        //Set color to white
        glColor4f( color.r, color.g, color.b, alpha );

        //Draw square
        glTexCoord2f(tex_origin.x, tex_origin.y);
        glVertex2f(0, 0);

        //glTexCoord2f(tex_end_x, tex_origin_y);
		glTexCoord2f(tex_origin.x + size.x, tex_origin.y);
        glVertex2f(size.x, 0);

        //glTexCoord2f(tex_end_x, tex_end_y);
		glTexCoord2f(tex_origin.x + size.x, tex_origin.y + size.y);
        glVertex2f(size.x, size.y);

        //glTexCoord2f(tex_origin_x, tex_end_y);
		glTexCoord2f(tex_origin.x, tex_origin.y + size.y);
        glVertex2f(0, size.y);
    //End quad
    glEnd();
}
