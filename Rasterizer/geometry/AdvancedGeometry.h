#pragma once
#include <initializer_list>
#include <algorithm>
#include <ostream>
#include <cmath>
#include <xmmintrin.h>

namespace GR{

union Vector2D
{
    struct
    {
        float x, y;
    };
    float data[2];

    Vector2D() = default;
    float       & operator[] (int i)       noexcept { return data[i]; }
    float const & operator[] (int i) const noexcept { return data[i]; }
};

union Vector4D
{
    struct 
    {
        float x, y, z, w;
    };
    float data[4];

    Vector4D() = default;
    float       & operator[] (int i)       noexcept { return data[i]; }
    float const & operator[] (int i) const noexcept { return data[i]; }
};

union Vector3D
{
    struct 
    {
        float x, y, z, w;
    };
    float data[4];

    Vector3D() = default;
    Vector3D(float const a, float const b, float const c) noexcept
        : data{a, b, c, 0.f}
    {}
    Vector3D(Vector4D const & vec) noexcept
        : data{vec.x / vec.w, vec.y / vec.w, vec.z / vec.w}
    {}

    float       & operator[] (int i)       noexcept { return data[i]; }
    float const & operator[] (int i) const noexcept { return data[i]; }
    Vector3D operator-()             const noexcept { return Vector3D{-x, -y, -z}; }

    float dot(Vector3D const & vec) const noexcept
    {
        return x * vec.x + y * vec.y + z * vec.z;
    }
    Vector3D cross(Vector3D const & vec) const noexcept
    {
        return Vector3D(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x);
    }
    Vector3D operator*(float const k) const noexcept
    {
        return Vector3D{x * k, y * k, z * k};
    } 

    Vector3D operator+(Vector3D const & vec) const noexcept
    {
        return Vector3D{x + vec.x, y + vec.y, z + vec.z};
    }
    Vector3D operator-(Vector3D const & vec) const noexcept
    {
        return Vector3D{x - vec.x, y - vec.y, z - vec.z};
    }
    float length() const noexcept
    {
        return std::sqrt(dot(*this));
    }
    Vector3D normalize() const noexcept
    {
        float const len = length();
        return Vector3D{x / len, y / len, z / len};
    }

};


struct Matrix3D
{
    float data[3][3];

    Matrix3D() = default;    
    float       *operator[](int i)       noexcept {return data[i];}
    float const *operator[](int i) const noexcept {return data[i];}

    Vector3D operator*(Vector3D const & vec) const noexcept
    {
        Vector3D out = Vector3D{0.f, 0.f, 0.f};
        for(int i = 0; i < 3; ++i)
            for(int j = 0; j < 3; ++j)
                out[i] += data[i][j] * vec[j];
        return out;
    }
};


struct Quatro
{
    Vector3D vec;
    float scal;

    Quatro() = default;
    Quatro(float const degree, Vector3D const & a) : vec(a), scal(degree) {}
    Quatro(Vector3D const & a, float const degree) : vec(a.normalize() * std::sin(degree/2)), scal(std::cos(degree/2)) {}
    Quatro(Vector3D const & a) : vec(a), scal(0.f) {}

    Quatro operator*(Quatro const & q) const noexcept;
    Quatro revers() const noexcept;
};

struct Vertex
{
    Vector3D local_position;
    Vector2D texture_coords;
    Vector3D norm_coords;
};

struct Triangle
{
    
    Vertex const vertexes[3];
    
    Triangle(Vertex const & v1, Vertex const & v2, Vertex const & v3) : vertexes{v1, v2, v3} {}

    // TODO operator=

    Triangle(Triangle const &) = delete;
    Triangle(Triangle && tr) : vertexes(tr.vertexes) {}
    Triangle & operator= (Triangle const &) = delete;
    Triangle & operator= (Triangle &&) = delete;
};




inline Matrix3D rotate(Vector3D const & dir, Vector3D const &_up) noexcept;
}//end of namespace

