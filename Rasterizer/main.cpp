#include "window/window_base.hpp"
#include "mouse.hpp"
#include "TriangleRast.hpp"
#include "object/object_base.h"

#include "thread_pool/thread_pool.h"

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

int main()
{
    unsigned int frame_count = 0;

    std::ofstream stats{"stat/stat.log"};

    GR::TTYContext context;
    int const w = 1920;
    int const h = 1080;
    float *depth = new float[w * h];

    Mouse mouse;

    GR::TriangleRasterizer rast;
    rast.set_viewport(0, 0, w, h);

    GR::Static_mesh const Mesh{"models/BG.obj"};

    int px = 0;
    int py = 0;

    float const ratio = static_cast<float>(w) / h;
    float const near = 0.2f;
    float const far  = 100.f;
    GR::Vector3D const hand{0.5f, 0.2f, 1.f};
    float const c1 = (far + near) / (far - near);
    float const c2 = 2.f * near * far / (far - near);

    GR::Vector3D const light = GR::Vector3D{1.f, 1.f, 1.f}.normalize();
    GR::Vector3D const PivotPosition{0.f, 0.f, 0.f};

    ThreadPool pool;
    std::vector<std::future<int>> waitPool{};


    while(1)
    {
        ++frame_count;
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
                //::system("stat/stat");
                std::cout << frame_count << '\n';
                delete[] depth;
                return 0;
            }
        }

        float const s       = 0.005f;
        float const phi     = px * s;
        float const theta   = py * s;
        GR::Vector3D const dir = GR::Vector3D
        {
            std::cos(theta) * std::sin(phi),
            std::sin(theta),
            std::cos(theta) * std::cos(phi)
        };
        
        GR::Vector3D const campos = dir;

        context.Clear();
        for(int y = 0; y < h; ++y)
            for(int x = 0; x < w; ++x)
                depth[y * w + x] = 1.f;

        GR::Quatro const A{GR::Vector3D{0.f, 1.f, 0.f}, phi};
        GR::Quatro const B{GR::Vector3D{1.f, 0.f, 0.f}, theta};

        GR::Quatro const Rotation{B * A};
        GR::Quatro const Rotation_rev{Rotation.revers()};

        waitPool.reserve(Mesh.size());
        for (auto && triangle : Mesh)
        {
            waitPool.emplace_back(pool.enqueue([&]{

            std::vector<GR::TriangleRasterizer::output> rout;
            GR::Vector4D point[3];

            //      vertex shader       //

            for(int j = 0; j < 3; ++j)
            {
                // TODO make shader part of object?
                GR::Quatro const e{triangle.vertexes[j].local_position + PivotPosition};
                GR::Vector3D const r = (Rotation * e * Rotation_rev).vec - hand;
                
                point[j].x = -r.x / ratio;
                point[j].y = -r.y;
                point[j].z = c1 * r.z + c2;
                point[j].w = r.z;
            }
            //                          //

            rast.rasterize(point, rout);

            for(auto const &p : rout)
            {
                if(p.depth < -1.f || p.depth > depth[p.y * w + p.x])
                    continue;

                // TODO check also this part
                GR::Vertex const v = mix(triangle.vertexes, p.b, p.c);
                // fragment shader  //
                float const NL = std::max(0.f, v.norm_coords.dot(light));
                GR::Vector3D const cam = (campos - v.local_position).normalize();
                GR::Vector3D const halfway = (cam + light).normalize();
                float spec = std::max(0.f, v.norm_coords.dot(halfway));
                for(int j = 0; j < 4; ++j)
                    spec *= spec;
                GR::Vector3D const model_color = GR::Vector3D{1.f, 1.f, 1.f};
                GR::Vector3D const light_color = GR::Vector3D{1.f, 0.9f, 0.77f};
                GR::Vector3D const c = model_color * (0.2f + 0.4f * NL) + light_color * (0.4f * spec);

                // BIG PROBLEM WITH CRITICAL SECTION. There is no problem!!!
                depth[p.y * w + p.x] = p.depth;
                context[p.y][p.x] = GR::Color
                {
                    static_cast<unsigned char>(c.z * 255),
                    static_cast<unsigned char>(c.y * 255),
                    static_cast<unsigned char>(c.x * 255),
                    255
                };
            }
            return 0;
            }));

        }

        for (auto && x : waitPool)
            x.get();
        waitPool.clear();
        context.Update();

        stats << (1.f / (::clock() - time)) * CLOCKS_PER_SEC << std::endl;
    }

    delete[] depth;
    stats.close();

    ::system("stat/stat");
    return 0;
}