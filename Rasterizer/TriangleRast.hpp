#pragma once
#include <cmath>
#include <vector>
#include "geometry/AdvancedGeometry.h"

namespace GR{

class TriangleRasterizer
{
    float x0, y0;
    float w_2, h_2;
protected:
    int float_to_x(float const fx) const noexcept
    {
        return std::lround((fx + 1.f) * w_2 + x0);
    }
    float x_to_float(int const ix) const noexcept
    {
        return (ix - x0) / w_2 - 1.f;
    }
    int float_to_y(float const fy) const noexcept
    {
        return std::lround((fy + 1.f) * h_2 + y0);
    }
    float y_to_float(int const iy) const noexcept
    {
        return (iy - y0) / h_2 - 1.f;
    }
public:
    void set_viewport(int x, int y, int w, int h) noexcept
    {
        x0 = x - 0.5f;
        y0 = y - 0.5f;
        w_2 = w * 0.5f;
        h_2 = h * 0.5f;
    }
    
    struct output
    {   
        int x;
        int y;
        float depth;
        float b;
        float c;
    };

    void rasterize(Vector4D const p[3], std::vector<output> &out) const noexcept
    {
        if(p[0].w > 0.f || p[1].w > 0.f || p[2].w > 0.f)
            return;

        Vector3D const v[3] = {Vector3D(p[0]), Vector3D(p[1]), Vector3D(p[2])};
        float const dx1 = v[1].x - v[0].x;
        float const dx2 = v[2].x - v[0].x;
        float const dy1 = v[1].y - v[0].y;
        float const dy2 = v[2].y - v[0].y;
        float const det = dx1 * dy2 - dy1 * dx2;

        auto const clamp = [](float const x){float const eps = 1e-6; return x >= 1.f ? 1.f - eps : (x <= -1.f ? -1.f + eps : x);};

        int const xmin = float_to_x(clamp(std::min(std::min(v[0].x, v[1].x), v[2].x)));
        int const xmax = float_to_x(clamp(std::max(std::max(v[0].x, v[1].x), v[2].x)));
        int const ymin = float_to_y(clamp(std::min(std::min(v[0].y, v[1].y), v[2].y)));
        int const ymax = float_to_y(clamp(std::max(std::max(v[0].y, v[1].y), v[2].y)));

        for(int y = ymin; y <= ymax; ++y)
        {
            float const fy = y_to_float(y);
            float const dy = fy - v[0].y;
            for(int x = xmin; x <= xmax; ++x)
            {
                float const fx = x_to_float(x);
                float const dx = fx - v[0].x;

                float const det1 = dx * dy2 - dy * dx2;
                float const det2 = dx1 * dy - dy1 * dx;

                float const b0 = det1 / det;
                float const c0 = det2 / det;
                float const a0 = 1.f - b0 - c0;
                if(a0 < 0.f || b0 < 0.f || c0 < 0.f)
                    continue;
                float const depth = v[0].z * a0 + v[1].z * b0 + v[2].z * c0;
                float const a = a0 / p[0].w;
                float const b = b0 / p[1].w;
                float const c = c0 / p[2].w;
                float const sum = a + b + c;
                out.push_back({x, y, depth, b / sum, c / sum});
            }
        }
    }
};

}