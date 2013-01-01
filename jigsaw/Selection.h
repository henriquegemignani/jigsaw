#ifndef SELECTION_H_
#define SELECTION_H_

#include <algorithm>
#include <utility>
#include <limits>
#include <list>
#include "Piece.h"
#include "Vector2D.h"

class Selection {
  public:
    class const_iterator {
      public:
        const_iterator() : owner_(NULL) {}

        std::pair<int, int> operator*() const { return std::pair<int, int>(x_, y_); }

        void operator++() {
            x_++;
            if(x_ > owner_->end_x_) {
                x_ = owner_->start_x_;
                y_++;
            }
        }

        void operator++(int) { this->operator++(); }

        bool operator==(const const_iterator& other) const { 
            return owner_ == other.owner_ && x_ == other.x_ && y_ == other.y_;
        }
        bool operator!=(const const_iterator& other) const { return !(operator==(other)); }

      private:
        const_iterator(const Selection* owner, int x, int y) : owner_(owner), x_(x), y_(y) {}

        const Selection* owner_;
        int x_, y_;

        friend class Selection;
    };

    Selection() { Clear(); }

    void AddPiece(int x, int y) {
        start_x_ = std::min(start_x_, x);
          end_x_ = std::max(  end_x_, x);
        start_y_ = std::min(start_y_, y);
          end_y_ = std::max(  end_y_, y);
    }
    void AddPiece(const std::pair<int, int>& p) { AddPiece(p.first, p.second); }

    void Clear() {
        start_x_ = start_y_ = std::numeric_limits<int>::max();
          end_x_ =   end_y_ = std::numeric_limits<int>::min();
    }

    bool IsSelected(int x, int y) const { 
        return start_x_ <= x && x <= end_x_ && start_y_ <= y && y <= end_y_;
    }
    bool IsSelected(const std::pair<int, int>& p) const { return IsSelected(p.first, p.second); }

    Vector2D top_left() const {
        return Vector2D(start_x_, start_y_);
    }

    bool active() const { return end_x_ >= start_x_ && end_y_ >= start_y_; }
    int width() const { return std::max(end_x_ - start_x_, 0); }
    int height() const { return std::max(end_y_ - start_y_, 0); }
    int start_x() const { return start_x_; }
    int start_y() const { return start_y_; }


    const_iterator begin() const { 
        return active() ? const_iterator(this, start_x_, start_y_) : end();
    }
    const_iterator end() const { 
        return const_iterator(this, start_x_, end_y_ + 1);
    }

    std::list<int> x_range(bool increasing) {
        std::list<int> result;
        if(increasing)
            for(int i = start_x_; i <= end_x_; ++i)
                result.push_back(i);
        else
            for(int i = end_x_; i >= start_x_; --i)
                result.push_back(i);
        return result;
    }

    std::list<int> y_range(bool increasing) {
        std::list<int> result;
        if(increasing)
            for(int i = start_y_; i <= end_y_; ++i)
                result.push_back(i);
        else
            for(int i = end_y_; i >= start_y_; --i)
                result.push_back(i);
        return result;
    }

  private:
    int start_x_, end_x_;
    int start_y_, end_y_;
};

#endif // SELECTION_H_
