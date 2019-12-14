#include <fstream>
#include <iostream>

int main()
{
    std::ifstream stat{"stat/stat.log"};
    double fps = 0.f;
    double avarage = 0.f;
    int count = 0;
    std::cout << "stats:" << std::endl;
    while (stat >> fps)
    {
        avarage = (avarage * count + fps) / (++count);
    }

    std::cout << avarage << std::endl;
}