#pragma once
#include <vector>
#include "../geometry/AdvancedGeometry.h"

#include <iostream>

namespace GR{

class object_base
{
protected:
    Vector3D pivot_position;
public:
    object_base() = default;
    object_base(Vector3D const & position)             noexcept : pivot_position(position) {}
    object_base(Vector3D  && position)                 noexcept : pivot_position(position) {}
    object_base & operator=(Vector3D const & position) noexcept { pivot_position = position; return *this; }
    object_base & operator=(Vector3D && position)      noexcept { pivot_position = position; return *this; }

    Vector3D const & GetPivot()                  const noexcept { return pivot_position; }

    virtual ~object_base() {}
};

class object_moveable : virtual public object_base
{
public:
    using object_base::GetPivot; // To solve problem with hide function
    void MovePivot(Vector3D const & dr)        noexcept { pivot_position = pivot_position + dr; }
    void SetPivot(Vector3D const & position)   noexcept { pivot_position = position; }
    Vector3D & GetPivot()                      noexcept { return pivot_position; }

    virtual ~object_moveable() {}
};

class Model final
{
    std::vector<Triangle> triangles;

public:
    Model(char const * objFile);

    Model(Model const &)                = delete;
    Model(Model &&)                     = delete;
    Model & operator= (Model const &)   = delete;
    Model & operator= (Model &&)        = delete;
    ~Model() {};

    int size() const noexcept { return triangles.size(); }

    std::vector<Triangle>::iterator       begin()       noexcept { return triangles.begin(); }
    std::vector<Triangle>::const_iterator begin() const noexcept { return triangles.begin(); }
    std::vector<Triangle>::iterator       end()         noexcept { return triangles.end(); }
    std::vector<Triangle>::const_iterator end()   const noexcept { return triangles.end(); }
};

class object_drawable : virtual public object_base
{
    // TODO add shaders
    Model model;
public:
    object_drawable(char const * objFile) : model{objFile} {}

    std::vector<Triangle>::iterator       begin()       noexcept { return model.begin(); }
    std::vector<Triangle>::const_iterator begin() const noexcept { return model.begin(); }
    std::vector<Triangle>::iterator       end()         noexcept { return model.end(); }
    std::vector<Triangle>::const_iterator end()   const noexcept { return model.end(); }

    int size() const noexcept { return model.size(); }

    virtual ~object_drawable() {}
};

class Static_mesh : public object_drawable, public object_moveable
{
    // TODO make static mesh and propreate constructor

public:
    Static_mesh(char const * objFile) : object_drawable{objFile} {}
};

}