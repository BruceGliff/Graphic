#pragma once
#include <cmath>
union Vector2D
{
    struct
    {
        float x, y;
    };
    float data[2];

    Vector2D() = default;
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
    float data[3];

    Vector3D() = default;
    Vector3D(float const a, float const b, float const c) noexcept
        : data{a, b, c}
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

