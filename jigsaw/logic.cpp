// Code wrote by Henrique Gemignani Passos Lima

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_opengl.h"
#include "Timer.h"
#include "Piece.h"
#include "PieceSet.h"
#include "Drag.h"
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cmath>
#ifdef _WIN32
#include <Windows.h>
#endif

#include "logic.h"

//Screen attributes
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000
#define SCREEN_BPP 32
#define MAX_SCREEN_SCALE 0.9

int RESOLUTION_WIDTH = -1, RESOLUTION_HEIGHT = -1;

// The frame rate
#define FRAMES_PER_SECOND 60

void minmax(double *a, double *b) {
    double aux = *a;
    *a = std::min(*a, *b);
    *b = std::max(aux, *b);
}

void printTime(FILE *out, int time) {
    time /= 1000; // Time now in seconds.
    int seconds = time % 60;
    time /= 60;
    int minutes = time % 60;
    time /= 60;
    if(time > 0)
        fprintf(out, "%dh ", time);
    if(minutes > 0)
        fprintf(out, "%dmin ", minutes);
    fprintf(out, "%ds", seconds);
}

Logic::Logic(const Layout& layout)
    :   layout_(layout), pieces_(layout.width(), layout.height()) {}

void Logic::Start() {
    pieces_.Scramble();
}

void Logic::HandleEvent(const SDL_Event& event) {
    if( pieces_.get_matches() >= layout_.total()) {
        // do nothing
    }
    else if( event.type == SDL_MOUSEBUTTONDOWN ) {
        if(event.button.button == SDL_BUTTON_LEFT && !rightdrag) {
            leftDown(event.button);

        } else if(event.button.button == SDL_BUTTON_RIGHT) {
            rightDown(event.button);
        }

    } else if( event.type == SDL_MOUSEBUTTONUP ) {
        if(event.button.button == SDL_BUTTON_LEFT && rect_up) {
            leftUp(event.button);

        } else if(event.button.button == SDL_BUTTON_RIGHT) {
            rightUp(event.button);
        }
    }
    else if( event.type == SDL_MOUSEMOTION ) {
        // Keep track of the mouse position in a handy format.
        cursor_.position = layout_.ScreenToGame(event.motion.x, event.motion.y);
    }
}

void Logic::leftDown(const SDL_MouseButtonEvent& button) {
    cursor_.offset = layout_.ScreenToGame(button.x, button.y);

    if(rect_up) {
        int startx = (int)(drag_offset_x);
        int starty = (int)(drag_offset_y);
        for(int i = 0; i < rectsizex; i++)
            for(int j = 0; j < rectsizey; j++)
                pieces_.get_current(startx + i, starty + j)->set_color(Color(1.0, 1.0, 1.0));
    }


    rect_up = !(cursor_.offset.x < drag_offset_x || drag_end_x + 1 < cursor_.offset.x ||
        cursor_.offset.y < drag_offset_y || drag_end_y + 1 < cursor_.offset.y);
    if(!rect_up) {
        Vector2D temp = layout_.ScreenToGame(button.x, button.y);
        drag_end_x = drag_offset_x = temp.x;
        drag_end_y = drag_offset_y = temp.y;
        rectsizex = rectsizey = 1;
        rect_up = true;
    }
    leftdrag = true;
    // Store the piece we're dragging.
    selected_x = (int)(cursor_.offset.x);
    selected_y = (int)(cursor_.offset.y);

    // Store the position of the piece relative to the mouse
    cursor_.offset.x = (selected_x - cursor_.offset.x) / layout_.width();
    cursor_.offset.y = (selected_y - cursor_.offset.y) / layout_.height();
}

void Logic::rightDown(const SDL_MouseButtonEvent& button) {
    rightdrag = true;

    if(rect_up) {
        int startx = (int)(drag_offset_x);
        int starty = (int)(drag_offset_y);
        for(int i = 0; i < rectsizex; i++)
            for(int j = 0; j < rectsizey; j++)
                pieces_.get_current(startx + i, starty + j)->set_color(Color(1.0, 1.0, 1.0));
    }

    rect_up = false;

    // Store the rect origin.
    drag_offset_x = (double)(layout_.width()) * event.motion.x / screenwidth;
    drag_offset_y = (double)(layout_.height()) * event.motion.y / screenheight;
}

void Logic::leftUp(const SDL_MouseButtonEvent& button) {
    if(!rect_up) return;

    int finalx = layout_.width() * event.motion.x / screenwidth;
    int finaly = layout_.height() * event.motion.y / screenheight;
    int startx = (int)(drag_offset_x);
    int starty = (int)(drag_offset_y);
    finalx += (startx - selected_x);
    finaly += (starty - selected_y);
    if(finalx >= 0 && finaly >= 0 && finalx + rectsizex - 1 < layout_.width() && finaly + rectsizey - 1 < layout_.height()) {
        {
            for(int i = 0; i < rectsizex; i++)
                for(int j = 0; j < rectsizey; j++)
                    pieces_.get_current(startx + i, starty + j)->set_alpha(1.0);
        }
        if(startx >= finalx) {
            for(int i = 0; i < rectsizex; i++) {
                if(starty >= finaly)
                    for(int j = 0; j < rectsizey; j++)
                        pieces_.Swap(startx + i, starty + j, finalx + i, finaly + j);
                else
                    for(int j = rectsizey - 1; j >= 0; j--)
                        pieces_.Swap(startx + i, starty + j, finalx + i, finaly + j);
            }
        } else {
            for(int i = rectsizex - 1; i >= 0; i--) {
                if(starty >= finaly)
                    for(int j = 0; j < rectsizey; j++)
                        pieces_.Swap(startx + i, starty + j, finalx + i, finaly + j);
                else
                    for(int j = rectsizey - 1; j >= 0; j--)
                        pieces_.Swap(startx + i, starty + j, finalx + i, finaly + j);
            }
        }
    }
    selected_x = selected_y = -1;
    moves++;
    rect_up = false;
    leftdrag = false;
    drag_offset_x = drag_offset_y = drag_end_x = drag_end_y = -1;
}

void Logic::rightUp(const SDL_MouseButtonEvent& button) {
    // Store the rect end
    drag_end_x = (double)(layout_.width()) * event.motion.x / screenwidth;
    drag_end_y = (double)(layout_.height()) * event.motion.y / screenheight;

    // It's handy to have the end be to the bottom-right of the origin.
    minmax(&drag_offset_x, &drag_end_x);
    minmax(&drag_offset_y, &drag_end_y);

    // Store the rect size
    rectsizex = (int)(drag_end_x) - (int)(drag_offset_x) + 1;
    rectsizey = (int)(drag_end_y) - (int)(drag_offset_y) + 1;

    drag_offset_x = floor(drag_offset_x);
    drag_offset_y = floor(drag_offset_y);
    drag_end_x = floor(drag_end_x);
    drag_end_y = floor(drag_end_y);

    rect_up = true;
    rightdrag = false;

    {
        int startx = (int)(drag_offset_x);
        int starty = (int)(drag_offset_y);
        // Mouse pointer does not necessarily hold the origin, so fix it
        for(int i = 0; i < rectsizex; i++)
            for(int j = 0; j < rectsizey; j++)
                pieces_.get_current(startx + i, starty + j)->set_color(Color(0.5, 0.5, 1.0));
    }
}