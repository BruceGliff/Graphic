#pragma once
#include <vector>
#include "../geometry/AdvancedGeometry.h"


namespace GR{

class object_base
{
    // TODO make private
public:
    Vector<3> pivot_position;
    std::vector<Triangle> triangles;
public:
    object_base(char const * objFile);

    object_base(object_base const &) = delete;
    object_base(object_base &&) = delete;
    object_base & operator= (object_base const &) = delete;
    object_base & operator= (object_base &&) = delete;
    ~object_base() {};

    int size() { return triangles.size(); }
};

}