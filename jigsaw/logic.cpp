// Code wrote by Henrique Gemignani Passos Lima

#include "logic.h"

#include <algorithm>
#include <iostream>
#include <random>

#include "Piece.h"

Logic::Logic(const Layout& layout)
    :   layout_(layout), pieces_(layout), selecting_origin_(nullptr) {}

Logic::~Logic() {
    delete selecting_origin_;
}

void Logic::Start() {
    std::random_device rd;
    Start(rd());
}

void Logic::Start(unsigned int seed) {
    pieces_.Scramble(seed);
    moves_ = 0;
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
            rightDown();
        }

    } else if( event.type == SDL_MOUSEBUTTONUP ) {
        cursor_.position = layout_.ScreenToGame(event.button.x, event.button.y);
        if(event.button.button == SDL_BUTTON_LEFT/* && rect_up*/) {
            leftUp();

        } else if(event.button.button == SDL_BUTTON_RIGHT) {
            rightUp();
        }
    }
    else if( event.type == SDL_MOUSEMOTION ) {
        // Keep track of the mouse position in a handy format.
        cursor_.position = layout_.ScreenToGame(event.motion.x, event.motion.y);
    }
}

void Logic::leftDown() {
    // Can only drag pieces if you're not trying to select pieces.
    if(selecting_origin_) return;

    // If simple left click, add pointed piece to pending selection.
    if(!selection_.active())
        selection_.AddPiece(int(cursor_.position.x), int(cursor_.position.y));

    // Lets drag all the selected pieces.
    dragged_ = selection_;

    // And these pieces aren't selected anymore.
    selection_.Clear();

    // And store the mouse distance from the topleft of the dragged pieces.
    cursor_.offset = cursor_.position - Vector2D(dragged_.start_x(), dragged_.start_y());
}

void Logic::rightDown() {
    // Can only start a selection if not dragging anything.
    if(dragged_.active()) return;

    // Clear a previous selection.
    selection_.Clear();

    // This is both a flag for "choosing a selection" and "where the selection started".
    selecting_origin_ = new Vector2D(cursor_.position);
}

void Logic::leftUp() {
    // Can only move if there's pieces to move. Actually, this should work fine with an empty set, but let's just be safe.
    if(!dragged_.active()) return;

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

    moves_++;
}

void Logic::rightUp() {
    // We need to have started a selection.
    if(!selecting_origin_) return;

    // The selection started here.
    Vector2D start = *selecting_origin_;

    // And clear the flag.
    delete selecting_origin_;
    selecting_origin_ = nullptr;

    // And ended here.
    Vector2D end = cursor_.position;

    // AddPiece doesn't care which corner you give it first.
    selection_.AddPiece(int(start.x), int(start.y));
    selection_.AddPiece(int(end.x), int(end.y));
}