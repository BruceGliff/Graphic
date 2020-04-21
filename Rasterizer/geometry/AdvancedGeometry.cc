#include "AdvancedGeometry.h"

using namespace GR;

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
    m.vec.x *= norm_;
    m.vec.y *= norm_;
    m.vec.z *= norm_;

    return m;
}

inline Matrix3D rotate(Vector3D const & dir, Vector3D const &_up) noexcept
{
    //Vector3D const right = normalize(cross(_up, dir));
    Vector3D const right = _up.cross(dir).normalize();
    Vector3D const up = dir.cross(right).normalize();
    return Matrix3D
    {
        right.x, right.y, right.z,
        up.x,    up.y,    up.z,
        dir.x,   dir.y,   dir.z
    };
}