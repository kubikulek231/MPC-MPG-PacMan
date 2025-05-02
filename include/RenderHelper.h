#include "gl_includes.h"
#include <vector>
#include <numeric>

#ifndef RENDERHELPER_H
#define RENDERHELPER_H

static class RenderHelper {
private:
public:
   static float cubicBezier(float p0, float p1, float p2, float p3, float t);
   static void renderInnerRoundedCorner(float r, float height,
        float startAngle, float endAngle,
        int segs);
   static void renderBox(float x0, float x1,
       float y0, float y1,
       float z0, float z1,
       bool frontInward = false, bool backInward = false,
       bool leftInward = false, bool rightInward = false,
       bool topInward = false, bool bottomInward = false);
   static void renderOuterRoundedCorner(float radius, float height,
       float startAngle, float endAngle,
       int segs);
};

#endif