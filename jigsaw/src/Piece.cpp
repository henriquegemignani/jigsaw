#include "Piece.h"
#include "SDL_opengl.h"
#include <cmath>
#include <cstdio>

Piece::Piece(int x, int y, int nx, int ny) {
	size_ = Vector2D(1.0 / nx, 1.0 / ny);
	position_ = Vector2D(x, y);
	tex_origin_ = Vector2D((x)/(double)(nx), (y)/(double)(ny));
}

void Piece::Move(int x, int y) {
    position_.x = x;
    position_.y = y;
}

bool Piece::Matches(int x, int y) {
	Vector2D diff = position_ - Vector2D(x, y);
    return diff.NormOne() < 1.0E-6;
}

Piece::~Piece() {}

void Piece::CustomRender(const Cursor& cursor, const Vector2D& topleft) const {
    Vector2D total = (cursor.position + position_ - topleft - cursor.offset).Scale(size_);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(total.x, total.y, 0.0);
    //Start quad
    internalRender();
    //Reset
    glPopMatrix();
}

void Piece::Render() {
    Vector2D total = position_.Scale(size_);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(total.x, total.y, 0);
    //Start quad
    internalRender();
    //Reset
    glPopMatrix();
}

void Piece::internalRender() const {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    GLdouble   vertices[] = { 0.0, 0.0, size_.x, 0.0, size_.x, size_.y, 0.0, size_.y };
    GLdouble tex_coords[] = { tex_origin_.x, tex_origin_.y, tex_origin_.x + size_.x, tex_origin_.y, tex_origin_.x + size_.x, tex_origin_.y + size_.y, tex_origin_.x, tex_origin_.y + size_.y };
    glVertexPointer(2, GL_DOUBLE, 0, vertices);
    glTexCoordPointer(2, GL_DOUBLE, 0, tex_coords);

    glDrawArrays(GL_QUADS, 0, 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
