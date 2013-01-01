// Code wrote by Henrique Gemignani Passos Lima

#include "logic.h"

#include <set>
#include <algorithm>

#include "Piece.h"

/*void minmax(double *a, double *b) {
    double aux = *a;
    *a = std::min(*a, *b);
    *b = std::max(aux, *b);
}*/

Logic::Logic(const Layout& layout)
    :   layout_(layout), pieces_(layout) {}

void Logic::Start() {
    pieces_.Scramble();
}

void Logic::HandleEvent(const SDL_Event& event) {
    if( pieces_.get_matches() >= layout_.total()) {
        // do nothing
    }
    else if( event.type == SDL_MOUSEBUTTONDOWN ) {
        if(event.button.button == SDL_BUTTON_LEFT/* && !rightdrag*/) {
            leftDown(event.button);

        } else if(event.button.button == SDL_BUTTON_RIGHT) {
            //rightDown(event.button);
        }

    } else if( event.type == SDL_MOUSEBUTTONUP ) {
        if(event.button.button == SDL_BUTTON_LEFT/* && rect_up*/) {
            leftUp(event.button);

        } else if(event.button.button == SDL_BUTTON_RIGHT) {
            //rightUp(event.button);
        }
    }
    else if( event.type == SDL_MOUSEMOTION ) {
        // Keep track of the mouse position in a handy format.
        cursor_.position = layout_.ScreenToGame(event.motion.x, event.motion.y);
    }
}

void Logic::leftDown(const SDL_MouseButtonEvent& button) {
    cursor_.position = layout_.ScreenToGame(button.x, button.y);

    selection_.AddPiece(int(cursor_.position.x), int(cursor_.position.y));
    selection_.AddPiece(int(cursor_.position.x)+1, int(cursor_.position.y)+1);

    cursor_.offset = cursor_.position - Vector2D(floor(cursor_.position.x), floor(cursor_.position.y));

    /* if(rect_up) {
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
    }*/
}

void Logic::rightDown(const SDL_MouseButtonEvent& button) {/*
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
    drag_offset_y = (double)(layout_.height()) * event.motion.y / screenheight;*/
}

void Logic::leftUp(const SDL_MouseButtonEvent& button) {

    Piece* piece = NULL;
    for(auto it : selection_) {
        piece = pieces_.get_current(it);
        break;
    }
    selection_.Clear();

    /*
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
    drag_offset_x = drag_offset_y = drag_end_x = drag_end_y = -1;
    rect_up = false;
    leftdrag = false;
    selected_x = selected_y = -1;*/
    
    //moves++;
}

void Logic::rightUp(const SDL_MouseButtonEvent& button) {/*
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
    }*/
}