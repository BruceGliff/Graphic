#pragma once
#include <initializer_list>
#include <algorithm>
#include <ostream>
#include <cmath>

namespace GR{

template <int Dim>
class Vector;

template <int Dim>
class Vector_base
{
protected:
    float data_[Dim] = {0};
    
    typedef float       * iterator;
    typedef float const * const_iterator;

public:
    Vector_base() = default;
    Vector_base(std::initializer_list<float> const & list)  noexcept;    
    Vector_base(Vector_base const & vec)                    noexcept;
    Vector_base & operator=  (Vector_base const & vec)      noexcept;

    Vector_base(Vector_base &&)                    = delete;
    Vector_base & operator=  (Vector_base &&)      = delete;

    float       & operator[] (int i)       noexcept { return data_[i]; }
    float const & operator[] (int i) const noexcept { return data_[i]; }

    Vector<Dim> operator-()                         const noexcept;
    Vector<Dim> operator*(float const k)            const noexcept;
    Vector<Dim> operator+(Vector_base const & vec)  const noexcept;
    Vector<Dim> operator-(Vector_base const & vec)  const noexcept;
    
    float dot(Vector_base const & vec)  const noexcept;
    float length()                      const noexcept;

    iterator       begin()       noexcept { return data_; }
    const_iterator begin() const noexcept { return data_; }
    iterator       end()         noexcept { return data_ + Dim; }
    const_iterator end()   const noexcept { return data_ + Dim; }

};

template <int Dim>
class Vector : public Vector_base<Dim>
{
public:
    Vector() = default;
    Vector(std::initializer_list<float> const & list)   noexcept : Vector_base<Dim>{list} {}
    Vector(Vector_base<Dim> const & vec)                noexcept : Vector_base<Dim>{vec}  {}
};

template<>
class Vector<4> : public Vector_base<4>
{
public:
    Vector() = default;
    Vector(std::initializer_list<float> const & list)   noexcept : Vector_base<4>{list} {}
    Vector(Vector_base<4> const & vec)                  noexcept : Vector_base<4>{vec}  {}
    float const & x() const noexcept { return data_[0]; }
    float const & y() const noexcept { return data_[1]; }
    float const & z() const noexcept { return data_[2]; }
    float const & w() const noexcept { return data_[3]; }
    float       & x()       noexcept { return data_[0]; }
    float       & y()       noexcept { return data_[1]; }
    float       & z()       noexcept { return data_[2]; }
    float       & w()       noexcept { return data_[3]; }
};

template <>
class Vector<3> : public Vector_base<3>
{
public:
    Vector() = default;
    Vector(std::initializer_list<float> const & list)   noexcept : Vector_base<3>{list} {}
    Vector(Vector_base<3> const & vec)                  noexcept : Vector_base<3>{vec}  {}
    Vector(Vector<4> const & vec)                       noexcept;

    float const & x() const noexcept { return data_[0]; }
    float const & y() const noexcept { return data_[1]; }
    float const & z() const noexcept { return data_[2]; }
    float       & x()       noexcept { return data_[0]; }
    float       & y()       noexcept { return data_[1]; }
    float       & z()       noexcept { return data_[2]; }

    Vector cross(Vector const & vec)    const noexcept;
    Vector normalize()                  const noexcept;
};

template<int Dim>
class Matrix
{
    float data_[Dim * Dim] = {0};
public:
    Matrix() = default;
    Matrix(std::initializer_list<float> const & list)   noexcept;
    Matrix(Matrix const & mat)                          noexcept;
    float       *operator[](int i)       noexcept { return &data_[i * Dim]; }
    float const *operator[](int i) const noexcept { return &data_[i * Dim]; }

    Vector<Dim> operator*(Vector<Dim> const & vec)    const noexcept;

    static Matrix<3> rotate(Vector<3> const & dir, Vector<3> const &_up) noexcept
    {
            Vector<3> const right = _up.cross(dir).normalize();
            Vector<3> const up = dir.cross(right).normalize();
            return Matrix<3>
            {
                right.x(), right.y(), right.z(),
                up.x(),    up.y(),    up.z(),
                dir.x(),   dir.y(),   dir.z()
            };
    }
};

struct Quatro
{
    Vector<3> vec;
    float scal;

    Quatro() = default;
    Quatro(float const degree, Vector<3> const & a) : vec(a), scal(degree) {}
    Quatro(Vector<3> const & a, float const degree) : vec(a.normalize() * std::sin(degree/2)), scal(std::cos(degree/2)) {}
    Quatro(Vector<3> const & a) : vec(a), scal(0.f) {}

    Quatro operator*(Quatro const & q) const noexcept;

    Quatro revers() const noexcept;
};

struct Vertex
{
    Vector<3> local_position;
    Vector<2> texture_coords;
    Vector<3> norm_coords;
};

struct Triangle
{
    Vertex const vertexes[3];

    Triangle(Vertex const & v1, Vertex const & v2, Vertex const & v3) : vertexes{v1, v2, v3} {}

    // TODO operator=

    // TODO check this!
    //Triangle(Triangle const &) = delete;
    //Triangle(Triangle &&) = delete;
    //Triangle & operator= (Triangle const &) = delete;
    //Triangle & operator= (Triangle &&) = delete;
};

#include "AdvanceGeometry.hpp"

}//end of namespace