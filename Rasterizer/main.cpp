#include "window/window_base.hpp"
#include "mouse.hpp"
#include "TriangleRast.hpp"
#include "object/object_base.h"

#include <fstream>
#include <ctime>
#include <iostream>
#include <sstream>
#include <cstdlib>

GR::Vertex mix(GR::Vertex const v[3], float const b, float const c) noexcept
{
    float const a = 1.f - b - c;

    GR::Vertex out;
    float *f = reinterpret_cast<float *>(&out);
    float const * const f0 = reinterpret_cast<float const *>(v);
    float const * const f1 = reinterpret_cast<float const *>(v + 1);
    float const * const f2 = reinterpret_cast<float const *>(v + 2);
    for(unsigned int i = 0u; i < sizeof(GR::Vertex) / sizeof(float); ++i)
        f[i] = a * f0[i] + b * f1[i] + c * f2[i];
    return out;
}

using namespace GR;

int main(int argc, char * argv[])
{
    std::ofstream stats{"stat/stat.log"};

    TTYContext context;
    int const w = 1920;
    int const h = 1080;
    float *depth = new float[w * h];

    Mouse mouse;

    TriangleRasterizer rast;
    rast.set_viewport(0, 0, w, h);
    std::vector<TriangleRasterizer::output> rout;

    object_base Mesh{"models/BG.obj"};

    int px = 0;
    int py = 0;

    float const ratio = static_cast<float>(w) / h;
    float const near = 0.2f;
    float const far  = 100.f;
    Vector<3> const hand{0.5f, 0.2f, 1.f};
    float const c1 = (far + near) / (far - near);
    float const c2 = 2.f * near * far / (far - near);

    Vector<3> const light = Vector<3> {1.f, 1.f, 1.f}.normalize();
    Vector<3> const PivotPosition{0.f, 0.f, 0.f};


    while(1)
    {
        int time = ::clock();

        Mouse::event e;
        while(mouse.poll(e))
        {
            px += e.dx;
            py += e.dy;

            if(e.left_button())
            {
                // std::ofstream out("out.ppm");
                // out << "P6\n" << w << " " << h << "\n255\n";
                // for(int i = 0; i < h; ++i)
                //     for(int j = 0; j < w; ++j)
                //     {
                //         Color const * const ptr = context.memory + w * i + j;
                //         Color const c = {ptr->r, ptr->g, ptr->b, 255};
                //         out.write(reinterpret_cast<char const *>(&c), 3);
                //     } 
                stats.close();
                ::system("stat/stat");
                delete[] depth;
                return 0;
            }
        }

        float const s       = 0.005f;
        float const phi     = px * s;
        float const theta   = py * s;
        Vector<3> const dir = Vector<3>
        {
            std::cos(theta) * std::sin(phi),
            std::sin(theta),
            std::cos(theta) * std::cos(phi)
        };
        
        Vector<3> const campos = dir;

        context.Clear();
        for(int y = 0; y < h; ++y)
            for(int x = 0; x < w; ++x)
                depth[y * w + x] = 1.f;

        Quatro const A{Vector<3>{0.f, 1.f, 0.f}, phi};
        Quatro const B{Vector<3>{1.f, 0.f, 0.f}, theta};

        Quatro const Rotation{B * A};
        Quatro const Rotation_rev{Rotation.revers()};

        for (auto && triangle : Mesh.triangles)
        {
            Vector<4> point[3];

            //      vertex shader       //

            for(int j = 0; j < 3; ++j)
            {
                // TODO make shader part of object?
                Quatro const e{triangle.vertexes[j].local_position + PivotPosition};
                Vector<3> const r = (Rotation * e * Rotation_rev).vec - hand;
                
                point[j].x() = -r.x() / ratio;
                point[j].y() = -r.y();
                point[j].z() = c1 * r.z() + c2;
                point[j].w() = r.z();
            }
            //                          //

            rast.rasterize(point, rout);

            for(auto const &p : rout)
            {
                if(p.depth < -1.f || p.depth > depth[p.y * w + p.x])
                    continue;
                depth[p.y * w + p.x] = p.depth;

                // TODO check also this part
                GR::Vertex const v = mix(triangle.vertexes, p.b, p.c);
                // fragment shader  //
                float const NL = std::max(0.f, v.norm_coords.dot(light));
                Vector<3> const cam = (campos - v.local_position).normalize();
                Vector<3> const halfway = (cam + light).normalize();
                float spec = std::max(0.f, v.norm_coords.dot(halfway));
                for(int j = 0; j < 4; ++j)
                    spec *= spec;
                Vector<3> const cat_color = Vector<3>{1.f, 1.f, 1.f};
                Vector<3> const light_color = Vector<3>{1.f, 0.9f, 0.77f};
                Vector<3> const c = cat_color * (0.2f + 0.4f * NL) + light_color * (0.4f * spec);

                context[p.y][p.x] = Color
                {
                    static_cast<unsigned char>(c.z() * 255),
                    static_cast<unsigned char>(c.y() * 255),
                    static_cast<unsigned char>(c.x() * 255),
                    255
                };
            }
            rout.clear();
        }
        context.Update();

        stats << (1.f / (::clock() - time)) * CLOCKS_PER_SEC << std::endl;
    }

    delete[] depth;
    stats.close();

    ::system("stat/stat");
    return 0;
}
