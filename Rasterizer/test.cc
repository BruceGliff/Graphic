#include <iostream>
#include <vector>
#include <chrono>

#include "object/object_base.h"

void someF(GR::Vector3D const & c)
{
    std::cout << c;
}

int main()
{
    GR::object_moveable obj;

    obj.MovePivot(GR::Vector3D{1,2,3});

    someF(obj.GetPivot());
    
    
    return 0;
}