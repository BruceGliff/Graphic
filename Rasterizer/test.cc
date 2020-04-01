#include "object/object_base.h"
#include "ParserObj.hpp"
#include <iostream>
void DumpMesh();
void DumpObj();
int main(int argc, char * argv[])
{
    if(argc == 2)
        DumpMesh();
    else
        DumpObj();
    
}


void DumpMesh()
{
    std::cerr << "MESH" << std::endl;
    Mesh mesh = import_obj("models/BG.obj");
    
    for(auto i = 0u; i < mesh.inds.size(); i += 3)
    {
        std::cout << "Triangle " << i / 3 << '\n';
        std::cout << "    position\n";
        Mesh::vertex const v0[3] =
        {
            mesh.verts[mesh.inds[i]],
            mesh.verts[mesh.inds[i + 1]],
            mesh.verts[mesh.inds[i + 2]]
        };
        for (int j = 0; j != 3; ++j)
        {
        std::cout << "        " << j <<": ";
            for (int k = 0; k != 3; ++k)
                std::cout << v0[j].pos[k] << ' ';
            std::cout << std::endl;
        }
        std::cout << "    texture\n";
        for (int j = 0; j != 3; ++j)
        {
        std::cout << "        " << j <<": ";
            for (int k = 0; k != 2; ++k)
                std::cout << v0[j].tex[k] << ' ';
            std::cout << std::endl;
        }        
        std::cout << "    norm\n";
        for (int j = 0; j != 3; ++j)
        {
        std::cout << "        " << j <<": ";
            for (int k = 0; k != 3; ++k)
                std::cout << v0[j].norm[k] << ' ';
            std::cout << std::endl;
        }
        std::cout << '\n';   

    }
}

void DumpObj()
{
    std::cerr << "OBJ" << std::endl;
    GR::object_base object{"models/BG.obj"};
    for (int i = 0; i != object.size(); ++i)
    {
        std::cout << "Triangle " << i << '\n';
        std::cout << "    position\n";
        for (int j = 0; j != 3; ++j)
        {
        std::cout << "        " << j <<": ";
            for (int k = 0; k != 3; ++k)
                std::cout << object.triangles[i].vertexes[j].local_position[k] << ' ';
            std::cout << std::endl;
        }
        std::cout << "    texture\n";
        for (int j = 0; j != 3; ++j)
        {
        std::cout << "        " << j <<": ";
            for (int k = 0; k != 2; ++k)
                std::cout << object.triangles[i].vertexes[j].texture_coords[k] << ' ';
            std::cout << std::endl;
        }
        std::cout << "    norm\n";
        for (int j = 0; j != 3; ++j)
        {
        std::cout << "        " << j <<": ";
            for (int k = 0; k != 3; ++k)
                std::cout << object.triangles[i].vertexes[j].norm_coords[k] << ' ';
            std::cout << std::endl;
        }
        std::cout << '\n';   
    }
}
