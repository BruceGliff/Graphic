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

    Vector<3> const & GetPivot()                  const noexcept { return pivot_position; }

    virtual ~object_base() {}
};

class object_moveable : virtual public object_base
{
public:
    void MovePivot(Vector<3> const & dr)        noexcept { pivot_position = pivot_position + dr; }
    void SetPivot(Vector<3> const & position)   noexcept { pivot_position = position; }
    Vector<3> & GetPivot()                      noexcept { return pivot_position; }

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

    object_drawable(object_drawable const &)                = delete;
    object_drawable(object_drawable &&)                     = delete;
    object_drawable & operator= (object_drawable const &)   = delete;
    object_drawable & operator= (object_drawable &&)        = delete;

    std::vector<Triangle>::iterator       begin()       noexcept { return model.begin(); }
    std::vector<Triangle>::const_iterator begin() const noexcept { return model.begin(); }
    std::vector<Triangle>::iterator       end()         noexcept { return model.end(); }
    std::vector<Triangle>::const_iterator end()   const noexcept { return model.end(); }

    virtual ~object_drawable() {}
};

class Static_mesh : public object_drawable, public object_moveable
{
    // TODO make static mesh and propreate constructor

public:
    Static_mesh(char const * objFile) : object_drawable{objFile} {}
};

}