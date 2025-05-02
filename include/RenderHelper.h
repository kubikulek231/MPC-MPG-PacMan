#include "gl_includes.h"
#include <vector>
#include <numeric>

#ifndef RENDERHELPER_H
#define RENDERHELPER_H

static class RenderHelper {
private:
public:
   static void renderInnerRoundedCorner(float r, float height,
        float startAngle, float endAngle,
        int segs);
   static void renderBox(float x0, float x1,
       float y0, float y1,
       float z0, float z1);
   static void renderOuterRoundedCorner(float radius, float height,
       float startAngle, float endAngle,
       int segs);
};

#endif