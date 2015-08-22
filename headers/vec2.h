#ifndef VEC2_H
#define VEC2_H

#include <cmath>

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

// vec * scalar
template <typename V, typename S>
inline vec2<V> operator* (vec2<V> vec, S scalar) {
    vec.x *= scalar;
    vec.y *= scalar;
    return vec;
}
template <typename V, typename S>
inline void operator*= (vec2<V> &vec, S scalar) {
    vec.x *= scalar;
    vec.y *= scalar;
}

// vec +/- vec
template <typename T>
inline vec2<T> operator+ (vec2<T> v, vec2<T> u) {
    v.x += u.x;
    v.y += u.y;
    return v;
}
template <typename T>
inline void operator+= (vec2<T> &v, vec2<T> u) {
    v.x += u.x;
    v.y += u.y;
}
template <typename T>
inline vec2<T> operator- (vec2<T> v, vec2<T> u) {
    v.x -= u.x;
    v.y -= u.y;
    return v;
}
template <typename T>
inline void operator-= (vec2<T> &v, vec2<T> u) {
    v.x -= u.x;
    v.y -= u.y;
}

// vec +/- scalar
template <typename V, typename S>
inline vec2<V> operator+ (vec2<V> vec, S scalar) {
    vec.x += scalar;
    vec.y += scalar;
    return vec;
}
template <typename V, typename S>
inline void operator+= (vec2<V> &vec, S scalar) {
    vec.x += scalar;
    vec.y += scalar;
}
template <typename V, typename S>
inline vec2<V> operator- (vec2<V> vec, S scalar) {
    vec.x -= scalar;
    vec.y -= scalar;
    return vec;
}
template <typename V, typename S>
inline void operator-= (vec2<V> &vec, S scalar) {
    vec.x -= scalar;
    vec.y -= scalar;
}
template <typename T>
inline vec2<T> operator- (vec2<T> v) {
    v *= -1;
    return v;
}

template <typename T>
inline vec2<T> perpendicularTo(vec2<T> v) {
    T tmp = v.x;
    v.x = -v.y;
    v.y = tmp;
    return v;
}
template <typename T>
void rotateVec2(vec2<T>& vec, double rotateSpeed) {
    double oldX = vec.x;
    vec.x = oldX * cos(rotateSpeed) - vec.y * sin(rotateSpeed);
    vec.y = oldX * sin(rotateSpeed) + vec.y * cos(rotateSpeed);
}

#endif // VEC2_H