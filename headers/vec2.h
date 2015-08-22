#ifndef VEC2_H
#define VEC2_H

template <class T>
struct vec2
{
  T x;
  T y;

  vec2(T _x, T _y) : x(_x), y(_y) {}
  vec2() : x(0), y(0) {}
};

typedef vec2<double> vec2d;
typedef vec2<int> vec2i;

#endif // VEC2_H