#include "window/window_base.hpp"
#include "mouse.hpp"
#include "TriangleRast.hpp"
#include "object/object_base.h"

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
std::ostream & operator<<(std::ostream & os, GR::Vector3D const & vec)
{
    //os << "[X: " << vec.x << " Y: " << vec.y << " Z: " << vec.z << "]" << std::endl;
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
    return os;
}
Vector3D operator*(Vector3D const & lval, Vector3D const & rval)
{
    Vector3D tmp;
    for (int i = 0; i != 3; ++i)
        tmp.data[i] = lval.data[i] * rval.data[i];

    return tmp;
} 

using namespace GR;

int main()
{
    TTYContext context;
    int const w =  static_cast<int>(context.width_);
    int const h = static_cast<int>(context.height_);
    float *depth = new float[w * h];

    std::vector<Static_mesh * > meshes;
    meshes.push_back( new Static_mesh{"models/cube.obj"});
    meshes.push_back( new Static_mesh{"models/sphere.obj"});

    float const hand_len = 1;

    Vector3D const light = Vector3D{-1.f, -0.5f, -0.1f}.normalize();
    Vector3D const lightColor = Vector3D{1.f, 1.f, 1.f};
    Vector3D const PivotPosition {0.f, 0.f, 0.f};

    std::vector<Vector3D> colors;
    colors.push_back({0.f, 0.f, 1.f}); // cube
    colors.push_back({1.f, 0.f, 0.f}); // sphere

    while(1)
    {        
        float const s       = 0.5f;
        float const phi     = (3)* s; // px
        float const theta   = (70) * s / 100; // 70
        Vector3D const dir = Vector3D
        {
            std::cos(theta) * std::sin(phi),
            std::sin(theta),
            std::cos(theta) * std::cos(phi)
        };
        
        Vector3D const campos = dir * hand_len;

        context.Clear();
        for(int y = 0; y < h; ++y)
            for(int x = 0; x < w; ++x)
                depth[y * w + x] = 100.f;

        int const scale = 1000;

        Vector3D const UP {0.f, 1.f, 0.f};
        for (int y = 0; y < h; ++y)
        {
        //int y = 540;
        //int x = 919;
            for (int x = 0; x < w; ++x)
            {
                Vector3D const camDir = -campos.normalize();
                Vector3D const camRight = camDir.cross(UP).normalize();
                Vector3D const camDown  = camDir.cross(camRight).normalize();

                Vector3D const pixelPos = campos + camDown * ((static_cast<float>(y) - static_cast<float>(h) / 2) / scale) + camRight * ((static_cast<float>(x) - static_cast<float>(w) / 2) / scale);

                // std::vector <Triangle *> triangles;
                // triangles.push_back(&meshes[0]->model.triangles[0]);
                // triangles.push_back(&meshes[0]->model.triangles[40]);
                int mesh_index = -1;
                for (auto && mesh : meshes)
                //for (auto * t : triangles)
                {
                    mesh_index = (mesh_index + 1) % 2; // cube - 0, sphere - 1
                    
                    //Triangle & triangle = *t;
                    for (auto && triangle : *mesh)
                    //auto && triangle = meshes[0]->model.triangles[42];
                    {
                        // http://ray-tracing.ru/articles213.html
                        Vector3D const D = camDir;
                        Vector3D const E1 = triangle.vertexes[1].local_position - triangle.vertexes[0].local_position;
                        Vector3D const E2 = triangle.vertexes[2].local_position - triangle.vertexes[0].local_position;
                        Vector3D const T = pixelPos - triangle.vertexes[0].local_position;
                        Vector3D const P = D.cross(E2);
                        Vector3D const Q = T.cross(E1);

                        auto const clamp = [](float const x){float const eps = 1e-6; return x >= 1.f ? 1.f - eps : (x <= -1.f ? -1.f + eps : x);};
                        float const norm = clamp(P.dot(E1));
                        if (norm == 0)
                            continue;

                        float const u = P.dot(T) / norm;
                        float const v = Q.dot(D) / norm;
                        float const t1 = 1.f - u - v;

                        bool isCollide = true;
                        if (t1 < 0 || u < 0 || v < 0)
                            isCollide = false;

                        if (!isCollide)
                            continue;

                        Vertex const ver = mix(triangle.vertexes, u, v);
                        if(clamp(D.dot(ver.norm_coords)) >= 0)
                            continue;

                        float const distant = (pixelPos - ver.local_position).length();
                        if (depth[y * w + x] < distant)
                            continue;

                        depth[y * w + x] = distant;



                        Vector3D Normal = ver.norm_coords;
                        Vector3D Drop = camDir;
                        Vector3D pointPos = ver.local_position;
                        Vector3D RayTracingColor{0.f, 0.f, 0.f};
                        float const RTPower = 1.f;
                        int id = (mesh_index + 1) % 2;
                        int reflC = 1.f;
                        while (reflC--)
                        {
                            Vector3D const refl = Vector3D::reflect(Drop, Normal);
                            for (auto && tr : *meshes[id]) // take another mesh
                            {
                                Vector3D const D_rtx = refl;
                                Vector3D const E1_rtx = tr.vertexes[1].local_position - tr.vertexes[0].local_position;
                                Vector3D const E2_rtx = tr.vertexes[2].local_position - tr.vertexes[0].local_position;
                                Vector3D const T_rtx = pointPos - tr.vertexes[0].local_position;
                                Vector3D const P_rtx = D_rtx.cross(E2_rtx);
                                Vector3D const Q_rtx = T_rtx.cross(E1_rtx);

                                float const norm_rtx = clamp(P_rtx.dot(E1_rtx));
                                if (norm_rtx == 0.f)
                                    continue;

                                float const u_rtx = P_rtx.dot(T_rtx) / norm_rtx;
                                float const v_rtx = Q_rtx.dot(D_rtx) / norm_rtx;
                                float const t1_rtx = 1.f - u_rtx - v_rtx;

                                bool isCollide_rtx= true;
                                if (t1_rtx < 0 || u_rtx < 0 || v_rtx < 0)
                                    isCollide_rtx = false;
                                

                                if (!isCollide_rtx)
                                    continue;

                                Vertex const ver_rtx = mix(tr.vertexes, u_rtx, v_rtx);
                                // temporary check is not suppose to be..
                                if ((pointPos - ver_rtx.local_position).dot(D_rtx) > 0.f)
                                   continue;

                                if (ver_rtx.norm_coords.dot(D_rtx) >= 0)
                                    continue;
     
                                if(0){
                                std::cout << "{\n";
                                std::cout << y << ' ' << x << std::endl;
                                std::cout << "Camera pos: C=" << campos;
                                std::cout << "Camera dir: camdir=" << camDir;
                                std::cout << "Pixel  pos: P=" << pixelPos;
                                std::cout << "V0     pos: V0=" << triangle.vertexes[0].local_position;
                                std::cout << "V1     pos: V1=" << triangle.vertexes[1].local_position;
                                std::cout << "V2     pos: V2=" << triangle.vertexes[2].local_position;
                                
                                std::cout << "Hit    pos: H1P=" << ver.local_position;
                                std::cout << "Hit    nrm: hitnorm1=" << ver.norm_coords;
                                
                                std::cout << "Reflec dir: refl=" << D_rtx;

                                std::cout << "hitV0  pos: HV0=" << tr.vertexes[0].local_position;
                                std::cout << "hitV1  pos: HV1=" << tr.vertexes[1].local_position;
                                std::cout << "hitV2  pos: HV2=" << tr.vertexes[2].local_position;
                                
                                std::cout << "2Hit   nrm: h2norm=" << ver_rtx.norm_coords;
                                std::cout << "2Hit   pos: H2P=" << ver_rtx.local_position;
                                std::cout << "obj id    : " << id << std::endl;
                                std::cout << "t1 u1 v1  : " << t1_rtx << ' ' << u_rtx << ' ' << v_rtx << std::endl;
                                std::cout << "P*E1      : " << norm_rtx << std::endl;
                                std::cout << "D         : d=" << D_rtx;
                                std::cout << "E1        : e1=" << E1_rtx;
                                std::cout << "E2        : e2=" << E2_rtx;
                                std::cout << "T         : t=" << T_rtx;
                                std::cout << "P         : p=" << P_rtx;
                                std::cout << "Q         : q=" << Q_rtx;

                                std::cout << "}\n" << std::endl;
                                }

                                float const lightPow = std::max(0.f, -light.dot(ver_rtx.norm_coords));
                                RayTracingColor += colors[id] * ( lightPow * RTPower );

                                Drop = refl;
                                Normal = ver_rtx.norm_coords;
                                pointPos = ver_rtx.local_position;
                                id = (id + 1) % 2;
                                break;
                            }
                        }


                        float const diffPow = std::max(0.f, -light.dot(ver.norm_coords));

                        Vector3D const ambientColor = lightColor * 0.1;
                        Vector3D const diffuseColor = lightColor * diffPow;


                        Vector3D color = (ambientColor + diffuseColor) * (colors[mesh_index] + RayTracingColor);
                        for (int j = 0; j != 3; ++j)
                            if (color.data[j] > 1.f)
                                color.data[j] = 1.f;

                        context[y][x] = Color
                        {
                            static_cast<unsigned char>(color.z * 255),
                            static_cast<unsigned char>(color.y * 255),
                            static_cast<unsigned char>(color.x * 255),
                            255
                        };
                        if(0)
                        {
                            std::cout << "Cam pos: " << campos;
                            std::cout << "Cam dir: " << camDir;
                            std::cout << "Cam Rgh: " << camRight;
                            std::cout << "Cam Dwn: " << camDown;
                            std::cout << "Pxl pos: " << pixelPos;

                            std::cout << "Vx0 pos: " << triangle.vertexes[0].local_position;
                            std::cout << "Vx1 pos: " << triangle.vertexes[1].local_position;
                            std::cout << "Vx2 pos: " << triangle.vertexes[2].local_position;

                            std::cout << "Hit nrm: " << ver.norm_coords;
                            std::cout << "Hit pos: " << ver.local_position;

                            std::cout << "E1:      " << E1;
                            std::cout << "E2:      " << E2;
                            std::cout << "T:       " << T;
                            std::cout << "P:       " << P;
                            std::cout << "Q:       " << Q;
                            std::cout << "Y: " << y << " X: " << x << std::endl;

                            exit(-1);
                        }



                    }
                }
                //context.Update(); // draw each pixel
            }
            //context.Update(); // draw each line
        }
        context.DumpJPEG();
        context.Clear();
        exit(-1);
    }

    delete[] depth;
    for (auto && mesh : meshes)
        delete mesh;

    return 0;
}
