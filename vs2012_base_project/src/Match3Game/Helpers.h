#pragma once

template<class T>
struct Vec2
{
    Vec2<T>(T aX, T aY) : x(aX), y(aY) {};
    Vec2<T>() = default;

    Vec2<T> operator-(const Vec2<T> &other)
    {
        return Vec2<T>(x - other.x, y - other.y);
    };

    T x;
    T y;
};