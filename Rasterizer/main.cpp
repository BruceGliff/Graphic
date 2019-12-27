#include "tty_context.hpp"
#include "mouse.hpp"
#include "TriangleRast.hpp"
#include "ParserObj.hpp"
#include "JPG_CR.hpp"
#include <fstream>
#include <ctime>
#include <iostream>
#include <sstream>
#include <cstdlib>

Mesh::vertex mix(Mesh::vertex const v[3], float const b, float const c) noexcept
{
    float const a = 1.f - b - c;

    Mesh::vertex out;
    float *f = reinterpret_cast<float *>(&out);
    float const * const f0 = reinterpret_cast<float const *>(v);
    float const * const f1 = reinterpret_cast<float const *>(v + 1);
    float const * const f2 = reinterpret_cast<float const *>(v + 2);
    for(unsigned int i = 0u; i < sizeof(Mesh::vertex) / sizeof(float); ++i)
        f[i] = a * f0[i] + b * f1[i] + c * f2[i];
    return out;
}
using color = TTYContext::color;
void Dump(TTYContext const & context)
{
    static int frame = 0;
    int const w = context.width;
    int const h = context.height;

    IMG_DATA img(w, h, 100);
    for(int i = 0; i < h; ++i)
        for(int j = 0; j < w; ++j)
        {
            color const * const ptr = context.memory + w * i + j;
            
            JSAMPLE * p = img.image_buffer + (w * i + j) * 3;
            *p = static_cast<JSAMPLE>(ptr->r);
            *(p+1) = static_cast<JSAMPLE>(ptr->g);
            *(p+2) = static_cast<JSAMPLE>(ptr->b);           
        }

    std::ostringstream out_path_stream;
    out_path_stream << "direct_tr/" << frame << ".jpeg";

    write_JPEG_file(out_path_stream.str().c_str(), img);
    ++frame;
}

int main(int argc, char * argv[])
{
    std::ofstream stats{"stat/stat.log"};

    TTYContext context;
    int const w = context.width;
    int const h = context.height;
    float *depth = new float[w * h];

    //Mouse mouse;

    TriangleRasterizer rast;
    rast.set_viewport(0, 0, w, h);
    std::vector<TriangleRasterizer::output> rout;

    Mesh mesh;
    if (argc == 1)
    {
        mesh = import_obj("models/lastBG.obj");
    }
    else
    {
        mesh = import_obj(argv[1]);
    }

    int px = 0;
    int py = 0;

    float const ratio = static_cast<float>(w) / h;
    float const near = 0.2f;
    float const far  = 100.f;
    Vector3D const hand{0.5f, 0.2f, 1.f};
    float const c1 = (far + near) / (far - near);
    float const c2 = 2.f * near * far / (far - near);

    Vector3D const light = Vector3D{1.f, 1.f, 1.f}.normalize();
    Vector3D const PivotPosition{0.f, 0.f, 0.f};


    // setup actions of 4 rotations
    bool firstXCounterClockWise = false;
    bool secondXClockWise = false;
    bool thirdYClockWise = false;
    bool fourthYCounterClockWise = false;

    while(!fourthYCounterClockWise)
    {
        if (!firstXCounterClockWise)
        {
            --px;
            if (px == -85)
                firstXCounterClockWise = true;
        } else if (!secondXClockWise)
        {
            ++px;
            if (px == 250)
                secondXClockWise = true;
        } else if (!thirdYClockWise)
        {
            ++py;
            if (py == 100)
                thirdYClockWise = true;
        } else if (!fourthYCounterClockWise)
        {
            --py;
            if (py == -100)
                fourthYCounterClockWise = true;
        }

        int time = ::clock();

        //Mouse::event e;
        /*while(mouse.poll(e))
        {
            px += e.dx;
            py += e.dy;

            if(e.left_button())
            {
                std::ofstream out("out.ppm");
                out << "P6\n" << w << " " << h << "\n255\n";
                for(int i = 0; i < h; ++i)
                    for(int j = 0; j < w; ++j)
                    {
                        color const * const ptr = context.memory + w * i + j;
                        color const c = {ptr->r, ptr->g, ptr->b, 255};
                        out.write(reinterpret_cast<char const *>(&c), 3);
                    } 
                stats.close();
                ::system("stat/stat");
                delete[] depth;
                return 0;
            }
        }
        */

        float const s       = 0.005f;
        float const phi     = px * s;
        float const theta   = py * s;
        Vector3D const dir = Vector3D
        (
            std::cos(theta) * std::sin(phi),
            std::sin(theta),
            std::cos(theta) * std::cos(phi)
        );
        
        Vector3D const campos = dir;

        context.clear();
        for(int y = 0; y < h; ++y)
            for(int x = 0; x < w; ++x)
                depth[y * w + x] = 1.f;

        Quatro const A{Vector3D{0.f, 1.f, 0.f}, phi};
        Quatro const B{Vector3D{1.f, 0.f, 0.f}, theta};

        Quatro const Rotation{B * A};
        Quatro const Rotation_rev{Rotation.revers()};


        for(auto i = 0u; i < mesh.inds.size(); i += 3)
        {
            Mesh::vertex const v0[3] =
            {
                mesh.verts[mesh.inds[i]],
                mesh.verts[mesh.inds[i + 1]],
                mesh.verts[mesh.inds[i + 2]]
            };
            Vector4D p[3];

            //      vertex shader       //
            for(int i = 0; i < 3; ++i)
            {
                Quatro const e{v0[i].pos + PivotPosition};
                Vector3D const r = (Rotation * e * Rotation_rev).vec - hand;
                
                p[i].x = -r.x / ratio;
                p[i].y = -r.y;
                p[i].z = c1 * r.z + c2;
                p[i].w = r.z;
            }
            //                          //

            rast.rasterize(p, rout);

            for(auto const &p : rout)
            {
                if(p.depth < -1.f || p.depth > depth[p.y * w + p.x])
                    continue;
                depth[p.y * w + p.x] = p.depth;

                Mesh::vertex const v = mix(v0, p.b, p.c);
                
                // fragment shader  //
                float const NL = std::max(0.f, v.norm.dot(light));
                Vector3D const cam = (campos - v.pos).normalize();
                Vector3D const halfway = (cam + light).normalize();
                float spec = std::max(0.f, v.norm.dot(halfway));
                for(int i = 0; i < 4; ++i)
                    spec *= spec;
                Vector3D const cat_color = Vector3D{1.f, 1.f, 1.f};
                Vector3D const light_color = Vector3D{1.f, 0.9f, 0.77f};
                Vector3D const c = cat_color * (0.2f + 0.4f * NL) + light_color * (0.4f * spec);

                context[p.y][p.x] = color
                {
                    static_cast<unsigned char>(c.z * 255),
                    static_cast<unsigned char>(c.y * 255),
                    static_cast<unsigned char>(c.x * 255),
                    255
                };
            }
            rout.clear();
        }
        //context.update();

        stats << (1.f / (::clock() - time)) * CLOCKS_PER_SEC << std::endl;
        Dump(context);
    }

    delete[] depth;
    stats.close();
    ::system("stat/stat");
    return 0;
}
