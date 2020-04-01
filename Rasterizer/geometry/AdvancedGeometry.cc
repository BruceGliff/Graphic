#include "AdvancedGeometry.h"

using namespace GR;

Vector<3>::Vector(Vector<4> const & vec) noexcept : 
    Vector_base<3>::Vector_base<3>{}
{
    data_[0] = vec[0] / vec[3];
    data_[1] = vec[1] / vec[3];
    data_[2] = vec[2] / vec[3];
}

Vector<3> Vector<3>::cross(Vector const & vec) const noexcept
{
    return Vector{y() * vec.z() - z() * vec.y(), z() * vec.x() - x() * vec.z(), x() * vec.y() - y() * vec.x()};
}
Vector<3> Vector<3>::normalize() const noexcept
{
    float const len = length();
    return Vector{x() / len, y() / len, z() / len};
}


Quatro Quatro::operator*(Quatro const & q) const noexcept
{
    return Quatro
    {
        scal * q.scal - vec.dot(q.vec) ,
        (q.vec * scal) + (vec * q.scal) + vec.cross(q.vec)
    };
}


Quatro Quatro::revers() const noexcept
{
    Quatro m{scal, -vec};
    float const norm_ = 1.f / (scal * m.scal - vec.dot(m.vec));
    m.scal *= norm_;
    m.vec.x() *= norm_;
    m.vec.y() *= norm_;
    m.vec.z() *= norm_;

    return m;
}