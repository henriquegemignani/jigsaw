#ifndef COLOR_H_
#define COLOR_H_

class Color {
  public:
    // Construtores e destrutores
	Color(float r, float g, float b) : r(r), g(g), b(b) { }
    ~Color() { }

	float r, g, b;
};

inline Color operator+(const Color& l, const Color& r) {
	return Color(l.r + r.r, l.r + r.g, l.b + r.b);
}
inline Color operator*(const Color& l, const Color& r) {
	return Color(l.r * r.r, l.r * r.g, l.b * r.b);
}

#endif  // COLOR_H_
