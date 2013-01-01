// Code wrote by Henrique Gemignani Passos Lima

#include "logic.h"

#include <algorithm>
#include <iostream>

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
    std::cout << "Starting the game." << std::endl;
}

void Logic::HandleEvent(const SDL_Event& event) {
    if( pieces_.get_matches() >= layout_.total()) {
        // do nothing
    }
    else if( event.type == SDL_MOUSEBUTTONDOWN ) {
        cursor_.position = layout_.ScreenToGame(event.button.x, event.button.y);
        if(event.button.button == SDL_BUTTON_LEFT/* && !rightdrag*/) {
            leftDown();

        } else if(event.button.button == SDL_BUTTON_RIGHT) {
            //rightDown();
        }

    } else if( event.type == SDL_MOUSEBUTTONUP ) {
        cursor_.position = layout_.ScreenToGame(event.button.x, event.button.y);
        if(event.button.button == SDL_BUTTON_LEFT/* && rect_up*/) {
            leftUp();

        } else if(event.button.button == SDL_BUTTON_RIGHT) {
            //rightUp();
        }
    }
    else if( event.type == SDL_MOUSEMOTION ) {
        // Keep track of the mouse position in a handy format.
        cursor_.position = layout_.ScreenToGame(event.motion.x, event.motion.y);
    }
}

void Logic::leftDown() {

    // If simple left click, add pointed piece to pending selection.
    if(!selection_.active())
        selection_.AddPiece(int(cursor_.position.x), int(cursor_.position.y));

    // Make the 
    dragged_ = selection_;

    selection_.Clear();

    cursor_.offset = cursor_.position - Vector2D(dragged_.start_x(), dragged_.start_y());
}

void Logic::rightDown() {/*
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

void Logic::leftUp() {
    int final_x = int(cursor_.position.x) - int(cursor_.offset.x), final_y = int(cursor_.position.y) - int(cursor_.offset.y);

    if(final_x < 0 || final_x + dragged_.width() >= layout_.num_x() ||
       final_y < 0 || final_y + dragged_.height() >= layout_.num_y()) {
           dragged_.Clear();
           return;
    }

    for(int i : dragged_.x_range(dragged_.start_x() >= final_x)) {
        for(int j : dragged_.y_range(dragged_.start_y() >= final_y)) {
            pieces_.Swap(i, j, final_x + i - dragged_.start_x(), final_y + j - dragged_.start_y());
        }
    }

    dragged_.Clear();
    //moves++;
}

void Logic::rightUp() {/*
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