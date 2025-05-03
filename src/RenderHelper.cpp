#include "RenderHelper.h"
#include "GameLighting.h"
#include "TileWall.h"
#include <cmath>

float RenderHelper::cubicBezier(float p0, float p1, float p2, float p3, float t) {
    float u = 1 - t;
    return u * u * u * p0 + 3 * u * u * t * p1 + 3 * u * t * t * p2 + t * t * t * p3;
}

void RenderHelper::renderBox(float x0, float x1,
    float y0, float y1,
    float z0, float z1,
    bool frontInward, bool backInward,
    bool leftInward, bool rightInward,
    bool topInward, bool bottomInward)
{
    glBegin(GL_QUADS);

    // Front face (+Z)
    glNormal3f(0, 0, frontInward ? -1 : 1);
    if (!frontInward) {
        glVertex3f(x0, y0, z1);
        glVertex3f(x1, y0, z1);
        glVertex3f(x1, y1, z1);
        glVertex3f(x0, y1, z1);
    }
    else {
        glVertex3f(x0, y1, z1);
        glVertex3f(x1, y1, z1);
        glVertex3f(x1, y0, z1);
        glVertex3f(x0, y0, z1);
    }

    // Back face (-Z)
    glNormal3f(0, 0, backInward ? 1 : -1);
    if (!backInward) {
        glVertex3f(x1, y0, z0);
        glVertex3f(x0, y0, z0);
        glVertex3f(x0, y1, z0);
        glVertex3f(x1, y1, z0);
    }
    else {
        glVertex3f(x1, y1, z0);
        glVertex3f(x0, y1, z0);
        glVertex3f(x0, y0, z0);
        glVertex3f(x1, y0, z0);
    }

    // Left face (-X)
    glNormal3f(leftInward ? 1 : -1, 0, 0);
    if (!leftInward) {
        glVertex3f(x0, y0, z0);
        glVertex3f(x0, y0, z1);
        glVertex3f(x0, y1, z1);
        glVertex3f(x0, y1, z0);
    }
    else {
        glVertex3f(x0, y1, z0);
        glVertex3f(x0, y1, z1);
        glVertex3f(x0, y0, z1);
        glVertex3f(x0, y0, z0);
    }

    // Right face (+X)
    glNormal3f(rightInward ? -1 : 1, 0, 0);
    if (!rightInward) {
        glVertex3f(x1, y0, z1);
        glVertex3f(x1, y0, z0);
        glVertex3f(x1, y1, z0);
        glVertex3f(x1, y1, z1);
    }
    else {
        glVertex3f(x1, y1, z1);
        glVertex3f(x1, y1, z0);
        glVertex3f(x1, y0, z0);
        glVertex3f(x1, y0, z1);
    }

    // Top face (+Y)
    glNormal3f(0, topInward ? -1 : 1, 0);
    if (!topInward) {
        glVertex3f(x0, y1, z1);
        glVertex3f(x1, y1, z1);
        glVertex3f(x1, y1, z0);
        glVertex3f(x0, y1, z0);
    }
    else {
        glVertex3f(x0, y1, z0);
        glVertex3f(x1, y1, z0);
        glVertex3f(x1, y1, z1);
        glVertex3f(x0, y1, z1);
    }

    // Bottom face (-Y)
    glNormal3f(0, bottomInward ? 1 : -1, 0);
    if (!bottomInward) {
        glVertex3f(x0, y0, z0);
        glVertex3f(x1, y0, z0);
        glVertex3f(x1, y0, z1);
        glVertex3f(x0, y0, z1);
    }
    else {
        glVertex3f(x0, y0, z1);
        glVertex3f(x1, y0, z1);
        glVertex3f(x1, y0, z0);
        glVertex3f(x0, y0, z0);
    }

    glEnd();
}


// Renders inner rounded corner with cap on top
void RenderHelper::renderInnerRoundedCorner(float r, float height,
    float startAngle, float endAngle,
    int segs)
{
    float halfH = height * 0.5f;
    float delta = (endAngle - startAngle) / segs;

    // Build the contour points for an inner rounded corner
    std::vector<std::pair<float, float>> pts;
    pts.emplace_back(0.0f, r);     // vertical segment
    pts.emplace_back(-r, r);      // 90° corner
    pts.emplace_back(-r, 0.0f);   // horizontal segment
    for (int i = 0; i <= segs; ++i) {
        float a = endAngle - i * delta;
        pts.emplace_back(cosf(a) * r, sinf(a) * r);
    }

    // Render the side walls (vertical quads along contour)
    glBegin(GL_QUAD_STRIP);
    for (auto& p : pts) {
        // Normal is the outward-facing 2D normal, pointing away from center (0, 0)
        float dx = p.first;
        float dz = p.second;
        float len = std::sqrt(dx * dx + dz * dz);
        float nx = (len > 0.001f) ? dx / len : 0.0f;
        float nz = (len > 0.001f) ? dz / len : 0.0f;

        glNormal3f(-nx, 0.0f, -nz); // Same normal for both top and bottom of this vertical strip
        glVertex3f(p.first, +halfH, p.second);
        glVertex3f(p.first, -halfH, p.second);
    }
    // Close the quad strip with the first point
    glNormal3f(0.0f, 0.0f, 1.0f); // Arbitrary consistent normal for last cap
    glVertex3f(pts[0].first, +halfH, pts[0].second);
    glVertex3f(pts[0].first, -halfH, pts[0].second);
    glEnd();

    // Render top cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f); // Upward-facing normal
    glVertex3f(-r, +halfH, +r); // Center of the fan

    for (int i = 0; i <= segs; ++i) {
        float a = startAngle + i * delta;
        float x = cosf(a) * r;
        float z = sinf(a) * r;
        glVertex3f(x, +halfH, z);
    }
    glEnd();
}


// Renders outer rounded corner
void RenderHelper::renderOuterRoundedCorner(float radius, float height, float angleStart, float angleEnd, int segments) {
    float delta = (angleEnd - angleStart) / segments;
    float halfH = height * 0.5f;

    // Curved outer wall
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float angle = angleStart + i * delta;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;

        // Normal points outward from center
        float nx = cosf(angle);
        float nz = sinf(angle);

        glNormal3f(nx, 0.0f, nz);
        glVertex3f(x, -halfH, z);
        glVertex3f(x, +halfH, z);
    }
    glEnd();

    // Side wall 1 (start angle)
    {
        float angle = angleStart;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;

        // Normal: perpendicular to the side plane
        float nx = sinf(angle); // tangent vector rotated -90°
        float nz = -cosf(angle);

        glBegin(GL_QUADS);
        glNormal3f(nx, 0.0f, nz);
        glVertex3f(0, -halfH, 0);
        glVertex3f(0, +halfH, 0);
        glVertex3f(x, +halfH, z);
        glVertex3f(x, -halfH, z);
        glEnd();
    }

    // Side wall 2 (end angle)
    {
        float angle = angleEnd;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;

        float nx = -sinf(angle); // tangent vector rotated +90°
        float nz = cosf(angle);

        glBegin(GL_QUADS);
        glNormal3f(nx, 0.0f, nz);
        glVertex3f(0, -halfH, 0);
        glVertex3f(0, +halfH, 0);
        glVertex3f(x, +halfH, z);
        glVertex3f(x, -halfH, z);
        glEnd();
    }

    // Top cap
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f); // Upward-facing normal
    glVertex3f(0, +halfH, 0);  // center
    for (int i = 0; i <= segments; ++i) {
        float angle = angleStart + i * delta;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;
        glVertex3f(x, +halfH, z);
    }
    glEnd();
}