#include <fstream>
#include "object_base.h"

GR::Model::Model(char const * objFile)
{
    std::ifstream in{objFile};
    if(!in.is_open())
        throw std::invalid_argument{"can not find file " + std::string{objFile}};

    std::vector<Vector<3>> pos;
    std::vector<Vector<2>> tex;
    std::vector<Vector<3>> norm;

    std::string line;

    while(std::getline(in, line))
    {
        char const * const cstr = line.c_str();
        if(cstr[0] == 'v')
        {
            Vector<3> v;
            Vector<2> vt;
            if(sscanf(cstr + 1, " %f %f %f", &v.x(), &v.y(), &v.z()) == 3)
                pos.push_back(v);
            else if(sscanf(cstr + 1, "t %f %f", &vt[0], &vt[1]) == 2)
                tex.push_back(vt);
            else if(sscanf(cstr + 1, "n %f %f %f", &v.x(), &v.y(), &v.z()) == 3)
                norm.push_back(v);
        }
        else if(cstr[0] == 'f')
        {
            uint32_t idx[3];
            //auto const vsize = allVertexes.size();

            char const *cptr = cstr + 2;
            int eaten = 0;

            std::vector<Vertex> surface;
            surface.reserve(3);
            
            if (line.find("//") == std::string::npos)
                while(sscanf(cptr, "%u/%u/%u%n", idx, idx + 1, idx + 2, &eaten) == 3)
                {
                    surface.push_back({pos[idx[0] - 1], tex[idx[1] - 1], norm[idx[2] - 1]});
                    cptr += eaten;
                }
            else
                while(sscanf(cptr, "%u//%u%n", idx, idx + 2, &eaten) == 2)
                {
                    surface.push_back({pos[idx[0] - 1], Vector<2>{}, norm[idx[2] - 1]});
                    cptr += eaten;
                }
            triangles.push_back(Triangle{surface[0], surface[1], surface[2]});

            Triangle A{surface[0], surface[1], surface[2]};
        }
    }

}