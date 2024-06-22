#include "IVDebugOverlay.h"

#include <vector>
#include <stack>

#include "../Render.h"
#include "../Globals.h"

static std::vector<OverlayBox_t> s_OverlayBoxes;

Vector2 p0;

Vector2 nextToTop(std::stack<Vector2>& S)
{
    Vector2 p = S.top();
    S.pop();
    Vector2 res = S.top();
    S.push(p);
    return res;
}

// A utility function to swap two points
void swap(Vector2& p1, Vector2& p2)
{
    Vector2 temp = p1;
    p1 = p2;
    p2 = temp;
}

// A utility function to return square of distance
// between p1 and p2
int distSq(Vector2 p1, Vector2 p2)
{
    return (p1.x - p2.x) * (p1.x - p2.x) +
        (p1.y - p2.y) * (p1.y - p2.y);
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are collinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(Vector2 p, Vector2 q, Vector2 r)
{
    int val = (q.y - p.y) * (r.x - q.x) -
        (q.x - p.x) * (r.y - q.y);

    if (val == 0) return 0;  // collinear
    return (val > 0) ? 1 : 2; // clock or counterclock wise
}

// A function used by library function qsort() to sort an array of
// points with respect to the first point
int compare(const void* vp1, const void* vp2)
{
    Vector2* p1 = (Vector2*)vp1;
    Vector2* p2 = (Vector2*)vp2;

    // Find orientation
    int o = orientation(p0, *p1, *p2);
    if (o == 0)
        return (distSq(p0, *p2) >= distSq(p0, *p1)) ? -1 : 1;

    return (o == 2) ? -1 : 1;
}

// Prints convex hull of a set of n points.
std::vector<Vector2> convexHull(Vector2 points[], int n)
{
    // Find the bottommost point
    int ymin = points[0].y, min = 0;
    for (int i = 1; i < n; i++)
    {
        int y = points[i].y;

        // Pick the bottom-most or choose the left
        // most point in case of tie
        if ((y < ymin) || (ymin == y &&
            points[i].x < points[min].x))
            ymin = points[i].y, min = i;
    }

    // Place the bottom-most point at first position
    swap(points[0], points[min]);

    // Sort n-1 points with respect to the first point.
    // A point p1 comes before p2 in sorted output if p2
    // has larger polar angle (in counterclockwise
    // direction) than p1
    p0 = points[0];
    qsort(&points[1], n - 1, sizeof(Vector2), compare);

    // If two or more points make same angle with p0,
    // Remove all but the one that is farthest from p0
    // Remember that, in above sorting, our criteria was
    // to keep the farthest point at the end when more than
    // one points have same angle.
    int m = 1; // Initialize size of modified array
    for (int i = 1; i < n; i++)
    {
        // Keep removing i while angle of i and i+1 is same
        // with respect to p0
        while (i < n - 1 && orientation(p0, points[i],
            points[i + 1]) == 0)
            i++;


        points[m] = points[i];
        m++;  // Update size of modified array
    }

    std::vector<Vector2> result;
    // If modified array of points has less than 3 points,
    // convex hull is not possible
    if (m < 3) return result;

    // Create an empty stack and push first three points
    // to it.
    std::stack<Vector2> S;
    S.push(points[0]);
    S.push(points[1]);
    S.push(points[2]);

    // Process remaining n-3 points
    for (int i = 3; i < m; i++)
    {
        // Keep removing top while the angle formed by
        // points next-to-top, top, and points[i] makes
        // a non-left turn
        while (S.size() > 1 && orientation(nextToTop(S), S.top(), points[i]) != 2)
            S.pop();
        S.push(points[i]);
    }

    // Now stack has the output points, print contents of stack
    while (!S.empty())
    {
        Vector2 p = S.top();
        result.push_back(p);
        S.pop();
    }

    return result;
}

void IVDebugOverlay::AddBox(const Vector& origin, const Vector& mins, const Vector& maxs, Color col, float duration) {
	s_OverlayBoxes.push_back({
        origin,
        mins,
        maxs,
        col,
        GlobalVars->realtime + duration
	});
}

void IVDebugOverlay::RenderOverlays() {
    for (auto it = s_OverlayBoxes.begin(); it != s_OverlayBoxes.end();) {
        if (GlobalVars->realtime > it->end_time) {
            it = s_OverlayBoxes.erase(it);
            continue;
        }

        Vector verts[] = {
            it->origin + it->mins,
            it->origin + Vector(it->mins.x, it->maxs.y, it->mins.z),
            it->origin + Vector(it->maxs.x, it->maxs.y, it->mins.z),
            it->origin + Vector(it->maxs.x, it->mins.y, it->mins.z),

            it->origin + Vector(it->mins.x, it->mins.y, it->maxs.z),
            it->origin + Vector(it->mins.x, it->maxs.y, it->maxs.z),
            it->origin + it->maxs,
            it->origin + Vector(it->maxs.x, it->mins.y, it->maxs.z),
        };

        Vector2 scr_verts[8];

        bool failed = false;
        for (int i = 0; i < 8; i++) {
            Vector2 scr = Render->WorldToScreen(verts[i]);

            if (scr.Invalid()) {
                failed = true;
                break;
            }

            scr_verts[i] = scr;
        }

        if (failed) {
            it++;
            continue;
        }

        Vector2 cpy_scr[8];
        memcpy(cpy_scr, scr_verts, sizeof(Vector2) * 8);
        auto background = convexHull(cpy_scr, 8);

        Render->PolyFilled(background, it->col);

        Color edge_color = it->col.alpha_modulate(min((int)(it->col.a * 1.8f), 255));

        Render->PolyLine({ scr_verts[0], scr_verts[1], scr_verts[2], scr_verts[3], scr_verts[0] }, edge_color);
        Render->PolyLine({ scr_verts[4], scr_verts[5], scr_verts[6], scr_verts[7], scr_verts[4] }, edge_color);
        Render->Line(scr_verts[0], scr_verts[4], edge_color);
        Render->Line(scr_verts[1], scr_verts[5], edge_color);
        Render->Line(scr_verts[2], scr_verts[6], edge_color);
        Render->Line(scr_verts[3], scr_verts[7], edge_color);

        it++;
    }
}