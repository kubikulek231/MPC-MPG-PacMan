#include "RenderHelper.h"

// Tesselation callbacks for rendering rounded concave angles
static double g_tessY;
static void CALLBACK tessBeginCallback(GLenum type) { glBegin(type); }
static void CALLBACK tessEndCallback() { glEnd(); }
static void CALLBACK tessVertexCallback(void* data) {
    const GLdouble* xy = (const GLdouble*)data;
    glVertex3d(xy[0], g_tessY, xy[1]);
}

// Renders axis-aligned box [x0,x1]×[y0,y1]×[z0,z1]
void RenderHelper::renderBox(float x0, float x1,
    float y0, float y1,
    float z0, float z1)
{
    glBegin(GL_QUADS);
    // front (z=z1)
    glNormal3f(0, 0, 1);
    glVertex3f(x0, y0, z1);
    glVertex3f(x1, y0, z1);
    glVertex3f(x1, y1, z1);
    glVertex3f(x0, y1, z1);

    // back (z=z0)
    glNormal3f(0, 0, -1);
    glVertex3f(x1, y0, z0);
    glVertex3f(x0, y0, z0);
    glVertex3f(x0, y1, z0);
    glVertex3f(x1, y1, z0);

    // left (x=x0)
    glNormal3f(-1, 0, 0);
    glVertex3f(x0, y0, z0);
    glVertex3f(x0, y0, z1);
    glVertex3f(x0, y1, z1);
    glVertex3f(x0, y1, z0);

    // right (x=x1)
    glNormal3f(1, 0, 0);
    glVertex3f(x1, y0, z1);
    glVertex3f(x1, y0, z0);
    glVertex3f(x1, y1, z0);
    glVertex3f(x1, y1, z1);

    // top (y=y1)
    glNormal3f(0, 1, 0);
    glVertex3f(x0, y1, z1);
    glVertex3f(x1, y1, z1);
    glVertex3f(x1, y1, z0);
    glVertex3f(x0, y1, z0);

    // bottom (y=y0)
    glNormal3f(0, -1, 0);
    glVertex3f(x0, y0, z0);
    glVertex3f(x1, y0, z0);
    glVertex3f(x1, y0, z1);
    glVertex3f(x0, y0, z1);
    glEnd();
}

// Renders inner rounded corner using tesselation
void RenderHelper::renderInnerRoundedCorner(float r, float height,
    float startAngle, float endAngle,
    int segs) {

    float halfH = height * 0.5f;
    float delta = (endAngle - startAngle) / segs;

    // 1) Build the wall contour pts
    std::vector<std::pair<float, float>> pts;
    pts.emplace_back(0.0f, r);
    pts.emplace_back(-r, r);
    pts.emplace_back(-r, 0.0f);
    for (int i = 0; i <= segs; ++i) {
        float a = endAngle - i * delta;
        pts.emplace_back(cosf(a) * r, sinf(a) * r);
    }

    // 2) Draw the side walls (unchanged)
    glBegin(GL_QUAD_STRIP);
    for (auto& p : pts) {
        glVertex3f(p.first, +halfH, p.second);
        glVertex3f(p.first, -halfH, p.second);
    }
    // close
    glVertex3f(pts[0].first, +halfH, pts[0].second);
    glVertex3f(pts[0].first, -halfH, pts[0].second);
    glEnd();

    // 3) Copy pts into a flat GLdouble array for tessellation
    std::vector<GLdouble> contour;
    contour.reserve(pts.size() * 2);
    for (auto& p : pts) {
        contour.push_back(p.first);
        contour.push_back(p.second);
    }

    // 4) Create & configure the tessellator
    GLUtesselator* tess = gluNewTess();
    gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
    gluTessCallback(tess, GLU_TESS_BEGIN, (void(*)())tessBeginCallback);
    gluTessCallback(tess, GLU_TESS_END, (void(*)())tessEndCallback);
    gluTessCallback(tess, GLU_TESS_VERTEX, (void(*)())tessVertexCallback);

    // 5) Top cap (+halfH)
    g_tessY = +halfH;
    gluTessBeginPolygon(tess, nullptr);
    gluTessBeginContour(tess);
    for (size_t i = 0; i < contour.size() / 2; ++i) {
        gluTessVertex(tess,
            &contour[i * 2],
            &contour[i * 2]);
    }
    gluTessEndContour(tess);
    gluTessEndPolygon(tess);

    // 6) Bottom cap (–halfH), reverse winding
    g_tessY = -halfH;
    gluTessBeginPolygon(tess, nullptr);
    gluTessBeginContour(tess);
    for (int i = (int)contour.size() / 2 - 1; i >= 0; --i) {
        gluTessVertex(tess,
            &contour[i * 2],
            &contour[i * 2]);
    }
    gluTessEndContour(tess);
    gluTessEndPolygon(tess);

    gluDeleteTess(tess);
}

// Renders outer rounded corner
void RenderHelper::renderOuterRoundedCorner(float radius, float height, float angleStart, float angleEnd, int segments) {
    float delta = (angleEnd - angleStart) / segments;

    // Curved outer wall
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float angle = angleStart + i * delta;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;
        glVertex3f(x, -height * 0.5f, z);
        glVertex3f(x, height * 0.5f, z);
    }
    glEnd();

    // Bottom cap
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, -height * 0.5f, 0);  // center
    for (int i = 0; i <= segments; ++i) {
        float angle = angleStart + i * delta;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;
        glVertex3f(x, -height * 0.5f, z);
    }
    glEnd();

    // Top cap
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, height * 0.5f, 0);  // center
    for (int i = 0; i <= segments; ++i) {
        float angle = angleStart + i * delta;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;
        glVertex3f(x, height * 0.5f, z);
    }
    glEnd();

    // Side wall 1 (start angle)
    {
        float x = cosf(angleStart) * radius;
        float z = sinf(angleStart) * radius;
        glBegin(GL_QUADS);
        glVertex3f(0, -height * 0.5f, 0);
        glVertex3f(0, height * 0.5f, 0);
        glVertex3f(x, height * 0.5f, z);
        glVertex3f(x, -height * 0.5f, z);
        glEnd();
    }

    // Side wall 2 (end angle)
    {
        float x = cosf(angleEnd) * radius;
        float z = sinf(angleEnd) * radius;
        glBegin(GL_QUADS);
        glVertex3f(0, -height * 0.5f, 0);
        glVertex3f(0, height * 0.5f, 0);
        glVertex3f(x, height * 0.5f, z);
        glVertex3f(x, -height * 0.5f, z);
        glEnd();
    }
}
