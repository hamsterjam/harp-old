#ifndef __HARP_VEC2_H
#define __HARP_VEC2_H

#include <cmath> // for sqrt

#include <iostream> // DEBUG

template <typename T>
class Vec2 {
    public:
        T x;
        T y;

        Vec2<T> (T x, T y) {
            this->x = x;
            this->y = y;
        }

        Vec2<T> operator*=(T scalar) {
            this->x *= scalar;
            this->y *= scalar;
        }

        Vec2<T> operator/=(T scalar) {
            this->x /= scalar;
            this->y /= scalar;
        }

        Vec2<T> operator+=(Vec2<T> rhs) {
            this->x += rhs.x;
            this->y += rhs.y;
        }

        Vec2<T> operator-=(Vec2<T> rhs) {
            this->x -= rhs.x;
            this->y -= rhs.y;
        }

        T dot(Vec2<T> rhs) {
            return (this->x * rhs.x) + (this->y * rhs.y);
        }

        Vec2<T> cross() {
            Vec2<T> ret(this->y, - this->x);
            return ret;
        }

        T normSq() {
            return this->dot(*this);
        }

        T norm() {
            return sqrt(this->normSq());
        }

        Vec2<T> normalize() {
            T norm = this->norm();
            Vec2 ret(this->x / norm, this->y / norm);
            return ret;
        }
};

// Scalar multiplication
template <typename T>
Vec2<T> operator*(T scalar, Vec2<T> vec) {
    vec *= scalar;
    return vec;
}

template <typename T>
Vec2<T> operator*(Vec2<T> vec, T scalar) {
    vec *= scalar;
    return vec;
}

template <typename T>
Vec2<T> operator/(Vec2<T> vec, T scalar) {
    vec /= scalar;
    return vec;
}

// Vector addition
template <typename T>
Vec2<T> operator+(Vec2<T> lhs, Vec2<T> rhs) {
    lhs += rhs;
    return lhs;
}

template <typename T>
Vec2<T> operator-(Vec2<T> lhs, Vec2<T> rhs) {
    lhs -= rhs;
    return lhs;
}

#endif
