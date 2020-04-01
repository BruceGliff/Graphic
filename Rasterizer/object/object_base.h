#pragma once
#include <vector>
#include "../geometry/AdvancedGeometry.h"


namespace GR{

class object_base
{
protected:
    Vector<3> pivot_position;
public:
    object_base() = default;
    object_base(Vector<3> const & position)             noexcept : pivot_position(position) {}
    object_base(Vector<3>  && position)                 noexcept : pivot_position(position) {}
    object_base & operator=(Vector<3> const & position) noexcept { pivot_position = position; return *this; }
    object_base & operator=(Vector<3> && position)      noexcept { pivot_position = position; return *this; }

    Vector<3> const & GetPivot()          const noexcept { return pivot_position; }
};

class object_moveable : virtual public object_base
{
public:
    void MovePivot(Vector<3> const & dr)        noexcept { pivot_position = pivot_position + dr; }
    void SetPivot(Vector<3> const & position)   noexcept { pivot_position = position; }
    Vector<3> & GetPivot()                      noexcept { return pivot_position; }
};

class model : virtual public object_base
{
    std::vector<Triangle> triangles;

public:
    model(char const * objFile);

    model(model const &) = delete;
    model(model &&) = delete;
    model & operator= (model const &) = delete;
    model & operator= (model &&) = delete;
    ~model() {};

    int size() { return triangles.size(); }

    std::vector<Triangle>::iterator       begin()       noexcept { return triangles.begin(); }
    std::vector<Triangle>::const_iterator begin() const noexcept { return triangles.begin(); }
    std::vector<Triangle>::iterator       end()         noexcept { return triangles.end(); }
    std::vector<Triangle>::const_iterator end()   const noexcept { return triangles.end(); }
};

class object_drawable : virtual public model
{
    // TODO
    // add shaders
};

class static_mesh : public object_drawable, public object_moveable
{
    // TODO make static mesh and propreate constructor
};

}