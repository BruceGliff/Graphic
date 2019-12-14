#include "Geometry.hpp"
#include <iostream>

int main()
{
    Vector3D point{1,0,0};
    Quatro A{Vector3D{0, 0, 1}, 0.707};
    Quatro p{0, point};
    Quatro A_ = A.revers();

    Quatro C = A * p * A_;
    std::cout << C.vec.x << ' '  << C.vec.y << ' '  << C.vec.z << ' '  << C.scal << std::endl;

}