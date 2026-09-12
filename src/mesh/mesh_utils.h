/******************************************************************************
 * \file      mesh_utils.h  (DFN-Geometry standalone trim)
 * \brief     Minimal MeshUtils: only the self-contained geometric predicates
 *            used by the geometry pre-processing / DFN path.
 *
 * \details   The original mesh/mesh_utils.h (~1284 lines) also declares many
 *            routines that dereference Element/Nodal/Edge/DFN and whose bodies
 *            live in mesh/*.cpp (mesh connectivity, element coords, octree...).
 *            None of those are needed to build geometry and generate a mesh.
 *            This trimmed copy keeps ONLY the Vector2/Real/Eigen predicates the
 *            geometry code actually calls: isinBox, isonBox, isoutBox,
 *            isoutAABB, rotateSegment, isinCircle, isonCircle, isoutCircle,
 *            isonleftofPlane, isonrightofPlane, isonPlane (plus a few closely
 *            related helpers kept from the same contiguous block). The original
 *            project file is untouched; this is a project-local copy.
 ******************************************************************************/

#ifndef _OPENDFN_DFN_GEOMETRY_MESH_UTILS_TRIM_
#define _OPENDFN_DFN_GEOMETRY_MESH_UTILS_TRIM_

#include <algorithm>
#include <cmath>
#include <vector>
#include "common/memory_manager.h"   // buildmemory/freememory (used transitively by geometry)
#include "common/opendfn_common.h"
#include "common/opendfn_math.h"
#include "common/opendfn_message.h"  // OpenDFNMessage (used transitively by geometry)

using namespace ns_common;

namespace ns_mesh {
/**
 * \brief Standalone subset of MeshUtils holding only stateless geometric predicates.
 */
class MeshUtils {
public:
    MeshUtils() {}
    ~MeshUtils() {}

    /**
     * Returns true if the point p lies inside the rentangle[] with n vertices
     *
     * \param Point
     * \param region
     * \return
     */
    static inline bool isinBox(Vector2 Point, Real region[]) {
        bool ret = false;
        if (Point.x >= region[0] && Point.x <= region[1] && Point.y >= region[2] && Point.y <= region[3])
            ret = true;
        return ret;
    }

    /**
     * \brief Test whether a point lies strictly inside an axis-aligned box.
     *
     * Uses strict inequalities, so points on the box boundary are excluded.
     *
     * \param Point  the point to test.
     * \param region box bounds as [xmin, xmax, ymin, ymax].
     * \return true if the point is strictly inside the box.
     */
    static inline bool isinStrictBox(Vector2 Point, Real region[]) {
        bool ret = false;
        if (Point.x > region[0] && Point.x < region[1] && Point.y > region[2] && Point.y < region[3])
            ret = true;
        return ret;
    }
    /**
     * return true if the point is located on the box, which is defined by region.
     *
     * \param Point
     * \param region
     * \return
     */
    static inline bool isonBox(const Vector2 Point, const Real region[]) {
        bool ret = false;
        // on x edges
        if ((abs(Point.x - region[0]) <= OpenDFN_pos_epsilon || abs(Point.x - region[1]) <= OpenDFN_pos_epsilon)) {
            if (Point.y >= region[2] && Point.y <= region[3])
                ret = true;
        }
        // on y edges
        if (abs(Point.y - region[2]) <= OpenDFN_pos_epsilon || abs(Point.y - region[3]) <= OpenDFN_pos_epsilon) {
            if (Point.x >= region[0] && Point.x <= region[1])
                ret = true;
        }
        return ret;
    }

    /**
     * return true if the point is located outside the box, which is defined by region.
     *
     * \param Point
     * \param region
     * \return
     */
    static inline bool isoutBox(const Vector2 Point, const Real region[]) {
        bool ret = false;
        if ((Point.x < region[0] || Point.x > region[1]) && (Point.y < region[2] || Point.y > region[3]))
            ret = true;
        return ret;
    }

    /**
     * \brief Test whether a point lies outside the axis-aligned bounding box of a segment.
     *
     * Builds the AABB from the two segment endpoints and handles degenerate
     * (vertical or horizontal) segments where the box collapses to a line.
     *
     * \param Point         the point to test.
     * \param LineContainer segment endpoints as [x0, y0, x1, y1].
     * \return true if the point is outside the segment's bounding box.
     */
    static inline bool isoutAABB(const Vector2 Point, const Real LineContainer[]) {
        // LineContainer: [x0 y0 x1 y1] — two endpoints of the line segment
        Real region[4];
        // rearrange x axis: region[0]=xmin, region[1]=xmax
        if (LineContainer[0] <= LineContainer[2]) {
            region[0] = LineContainer[0];
            region[1] = LineContainer[2];
        } else {
            region[0] = LineContainer[2];
            region[1] = LineContainer[0];
        }
        // rearrange y axis: region[2]=ymin, region[3]=ymax
        if (LineContainer[1] <= LineContainer[3]) {
            region[2] = LineContainer[1];
            region[3] = LineContainer[3];
        } else {
            region[2] = LineContainer[3];
            region[3] = LineContainer[1];
        }
        // degenerate case: vertical segment (x0 == x1) — AABB has zero width,
        // so only constrain by y range; x axis is unbounded on this segment
        if (abs(region[1] - region[0]) <= OpenDFN_pos_epsilon)
            return (Point.y < region[2] || Point.y > region[3]);
        // degenerate case: horizontal segment (y0 == y1) — AABB has zero height,
        // so only constrain by x range; y axis is unbounded on this segment
        if (abs(region[3] - region[2]) <= OpenDFN_pos_epsilon)
            return (Point.x < region[0] || Point.x > region[1]);
        // general case: point is outside if it falls outside the 2D bounding box
        bool ret = true;
        if (isinBox(Point, region) | isonBox(Point, region))
            ret = false;
        return ret;
    }

    /**
     * \brief Rotate a segment's end point about its start point.
     *
     * Translates the end point relative to the start, applies a 2D rotation
     * (counterclockwise positive), and translates back.
     *
     * \param startPoint    pivot / start of the segment.
     * \param endPoint      end point to be rotated.
     * \param rotationAngle rotation angle in radians (counterclockwise positive).
     * \return the rotated end point.
     */
    static inline Vector2 rotateSegment(const Vector2& startPoint, const Vector2& endPoint, Real rotationAngle) {
        // translation
        Real translatedX = endPoint.x - startPoint.x;
        Real translatedY = endPoint.y - startPoint.y;

        // rotation, counterclockwise is postive
        Real rotatedX = translatedX * std::cos(rotationAngle) + translatedY * std::sin(rotationAngle);
        Real rotatedY = -translatedX * std::sin(rotationAngle) + translatedY * std::cos(rotationAngle);

        // retranslation
        Real finalX = rotatedX + startPoint.x;
        Real finalY = rotatedY + startPoint.y;

        return Vector2(finalX, finalY);
    }

    /**
     * \brief Test whether a point lies inside a polygon using the ray-crossing rule.
     *
     * \param polygon polygon vertices in order (open or closed).
     * \param point   the point to test.
     * \return true if the point is inside the polygon.
     */
    static inline bool isInPolygon(std::vector<Vector2>& polygon, Vector2& point) {
        bool inside = false;
        Int  i, j = 0;
        Int  n = polygon.size();

        for (i = 0, j = n - 1; i < n; j = i++) {
            if (((polygon[i].y > point.y) != (polygon[j].y > point.y)) &&
                (point.x <
                 (polygon[j].x - polygon[i].x) * (point.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x)) {
                inside = !inside;
            }
        }
        return inside;
    }

    /**
     * \brief Test whether a point lies on any edge of a polygon.
     *
     * A point is on an edge when its distances to the two endpoints sum to
     * the edge length.
     *
     * \param polygon polygon vertices in order.
     * \param point   the point to test.
     * \return true if the point lies on a polygon edge.
     */
    static inline bool isOnPolygon(std::vector<Vector2>& polygon, Vector2& point) {
        int n = polygon.size();
        for (int i = 0; i < n; ++i) {
            Vector2 p1 = polygon[i];
            Vector2 p2 = polygon[(i + 1) % n];

            // Determine if a point is equal to a side of a polygon
            if (point.getDistancefrom(p1) + point.getDistancefrom(p2) == p1.getDistancefrom(p2)) {
                return true;
            }
        }
        return false;
    }

    /**
     * \brief Test whether a point lies outside a polygon using the ray-crossing rule.
     *
     * \param polygon polygon vertices in order.
     * \param point   the point to test.
     * \return true if the point is outside the polygon.
     */
    static inline bool isOutPolygon(std::vector<Vector2>& polygon, Vector2& point) {
        Int  n         = polygon.size();
        bool isOutside = true;

        for (Int i = 0, j = n - 1; i < n; j = i++) {
            if (((polygon[i].y > point.y) != (polygon[j].y > point.y)) &&
                (point.x <
                 (polygon[j].x - polygon[i].x) * (point.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x)) {
                // Points are on or inside the sides of the polygon, not on the outside

                isOutside = false;
                break;
            }
        }

        return isOutside;
    }

    /**
     * Returns true if the point p lies inside the circle with n vertices
     *
     * \param Point
     * \param region
     * \return
     */
    static inline bool isinCircle(const Vector2 Point, const Real region[]) {
        bool ret = false;
        if (sqrt(pow(Point.x - region[0], 2) + pow(Point.y - region[1], 2)) - region[2] < OpenDFN_pos_epsilon)
            ret = true;
        return ret;
    }

    /**
     * \brief Returns true if the point lies on the circle (within epsilon).
     *
     * \param Point  the point to test.
     * \param region circle as [cx, cy, radius].
     * \return true if the point is on the circle boundary.
     */
    static inline bool isonCircle(const Vector2 Point, const Real region[]) {
        bool ret = false;
        if (abs(sqrt(pow(Point.x - region[0], 2) + pow(Point.y - region[1], 2)) - region[2]) < OpenDFN_pos_epsilon)
            ret = true;
        return ret;
    }

    /**
     * Returns true if the point p lies inside the circle with n vertices
     *
     * \param Point
     * \param region
     * \return
     */
    static inline bool isoutCircle(const Vector2 Point, const Real region[]) {
        bool ret = false;
        if (sqrt(pow(Point.x - region[0], 2) + pow(Point.y - region[1], 2)) - region[2] >= OpenDFN_pos_epsilon)
            ret = true;
        return ret;
    }

    /**
     * to check whether the point is on the left of the plane. The plane information is
     * stored in array
     *
     * \param Point
     * \param region
     * \return
     */
    static inline bool isonleftofPlane(const Vector2 Point, const Real region[]) {
        bool    ret = false;
        Vector2 line1, line2;
        line1.x = region[2] - region[0];
        line1.y = region[3] - region[1];
        line2.x = Point.x - region[0];
        line2.y = Point.y - region[1];
        // use cross product if l.ij x l.in > 0.0, on the left
        if (VectorCross(line1, line2) > 0.0)
            ret = true;
        return ret;
    }

    /**
     * Returns true if the point p lies inside the circle with n vertices
     *
     * \param Point
     * \param p1
     * \param p2
     * \return
     */
    static inline bool isonleftofPlane(const Vector2 Point, const Vector2 p1, const Vector2 p2) {
        bool    ret = false;
        Vector2 line1, line2;
        line1.x = p2.x - p1.x;
        line1.y = p2.y - p1.y;
        line2.x = Point.x - p1.x;
        line2.y = Point.y - p1.y;
        // use cross product if l.ij x l.in > 0.0, on the left
        if (VectorCross(line1, line2) > 0.0)
            ret = true;
        return ret;
    }
    /**
     * to check whether the point is on the right of the plane. The plane information is
     * stored in array
     *
     * \param Point
     * \param region
     * \return
     */
    static inline bool isonrightofPlane(const Vector2 Point, const Real region[]) {
        bool ret = false;

        Vector2 line1, line2;
        line1.x = region[2] - region[0];
        line1.y = region[3] - region[1];
        line2.x = Point.x - region[0];
        line2.y = Point.y - region[1];
        // use cross product if l.ij x l.in > 0.0, on the left
        if (VectorCross(line1, line2) < 0.0)
            ret = true;
        return ret;
    }
    /**
     * to check whether the point is on ringt of the plane.
     *
     * \param Point
     * \param p1
     * \param p2
     * \return
     */
    static inline bool isonrightofPlane(const Vector2 Point, const Vector2 p1, const Vector2 p2) {
        bool ret = false;

        Vector2 line1, line2;
        line1.x = p2.x - p1.x;
        line1.y = p2.y - p1.y;
        line2.x = Point.x - p1.x;
        line2.y = Point.y - p1.y;
        // use cross product if l.ij x l.in > 0.0, on the left
        if (VectorCross(line1, line2) < 0.0)
            ret = true;
        return ret;
    }
    /**
     * to check whether the point is on the plane. The plane information is
     * stored in array
     *
     * \param Point
     * \param region
     * \return
     */
    static inline bool isonPlane(const Vector2 Point, const Real region[]) {
        bool ret = false;

        Vector2 line1, line2;
        line1.x = region[2] - region[0];
        line1.y = region[3] - region[1];
        line2.x = Point.x - region[0];
        line2.y = Point.y - region[1];
        // use cross product if l.ij x l.in > 0.0, on the left
        if (abs(VectorCross(line1, line2)) < OpenDFN_pos_epsilon)
            ret = true;
        return ret;
    }
    /**
     * to check whether the point is on the plane.
     *
     * \param Point
     * \param p1
     * \param p2
     * \return
     */
    static inline bool isonPlane(const Vector2 Point, const Vector2 p1, const Vector2 p2) {
        bool    ret = false;
        Vector2 line1, line2;
        line1.x = p2.x - p1.x;
        line1.y = p2.y - p1.y;
        line2.x = Point.x - p1.x;
        line2.y = Point.y - p1.y;
        // use cross product if l.ij x l.in > 0.0, on the left
        if (abs(VectorCross(line1, line2)) < OpenDFN_pos_epsilon)
            ret = true;
        return ret;
    }

    // A C++ program to check if a given point lies inside a given polygon
    // Refer https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
    // for explanation of functions onSegment(), orientation() and doIntersect()

    // Given three collinear points p, q, r, the function checks if
    // point q lies on line segment 'pr'
    static bool onSegment(Vector2 p, Vector2 q, Vector2 r) {
        if (q.x <= OpenDFN_max(p.x, r.x) && q.x >= OpenDFN_min(p.x, r.x) && q.y <= OpenDFN_max(p.y, r.y) &&
            q.y >= OpenDFN_min(p.y, r.y))
            return true;
        return false;
    }
    /**
     * .
     *
     * \param p
     * \param s1
     * \param s2
     * \return
     */
    static Real PointDistancetoSegment(Vector2 p, Vector2 s1, Vector2 s2) {
        Real dx            = s2.x - s1.x;
        Real dy            = s2.y - s1.y;
        Real segmentLength = dx * dx + dy * dy;

        // If the segment has zero length, return the distance to one of the endpoints
        if (segmentLength == 0.0) {
            return VectorDistance(p, s1);
        }

        Real t = ((p.x - s1.x) * dx + (p.y - s1.y) * dy) / segmentLength;

        // Clamp t to the range [0, 1]
        t = std::max(0.0, std::min(1.0, t));

        Real closestX = s1.x + t * dx;
        Real closestY = s1.y + t * dy;

        return VectorDistance(p, {closestX, closestY});
    }
    /**
     * get the slope of a line by two points.
     *
     * \param p1
     * \param p2
     * \return
     */
    static Real LineSlope(Vector2 p1, Vector2 p2) {
        return (p2.y - p1.y) / (p2.x - p1.x);
    }

    static Real angleBetweenVectors(Vector2 v1, Vector2 v2) {
        Real dotProduct  = v1.x * v2.x + v1.y * v2.y;
        Real magnitudeV1 = VectorDistance({0, 0}, v1);
        Real magnitudeV2 = VectorDistance({0, 0}, v2);

        return std::acos(std::fabs(dotProduct) / (magnitudeV1 * magnitudeV2));
    }
    /**
     * return the intersected angle between two lines,, in radians.
     *
     * \param slope1
     * \param slope2
     * \return
     */
    static Real intersectedAngle(Real slope1, Real slope2) {
        return std::atan(std::fabs((slope2 - slope1) / (1 + slope1 * slope2)));
    }
    // To find orientation of ordered triplet (p, q, r).
    // The function returns following values
    // 0 --> p, q and r are collinear
    // 1 --> Clockwise
    // 2 --> Counterclockwise
    static int orientation(Vector2 p, Vector2 q, Vector2 r) {
        Real val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

        if (val == 0)
            return 0;              // collinear
        return (val > 0) ? 1 : 2;  // clock or counterclock wise
    }

    /**
     * The function that returns true if line segment 'p1q1'
     * and 'p2q2' intersect.
     *
     * \param p1
     * \param q1
     * \param p2
     * \param q2
     * \return
     */
    static bool doIntersect(Vector2 p1, Vector2 q1, Vector2 p2, Vector2 q2) {
        // Find the four orientations needed for general and
        // special cases
        int o1 = orientation(p1, q1, p2);
        int o2 = orientation(p1, q1, q2);
        int o3 = orientation(p2, q2, p1);
        int o4 = orientation(p2, q2, q1);

        // General case
        if (o1 != o2 && o3 != o4)
            return true;

        // Special Cases
        // p1, q1 and p2 are collinear and p2 lies on segment p1q1
        if (o1 == 0 && onSegment(p1, p2, q1))
            return true;

        // p1, q1 and p2 are collinear and q2 lies on segment p1q1
        if (o2 == 0 && onSegment(p1, q2, q1))
            return true;

        // p2, q2 and p1 are collinear and p1 lies on segment p2q2
        if (o3 == 0 && onSegment(p2, p1, q2))
            return true;

        // p2, q2 and q1 are collinear and q1 lies on segment p2q2
        if (o4 == 0 && onSegment(p2, q1, q2))
            return true;

        return false;  // Doesn't fall in any of the above cases
    }
};
}  // namespace ns_mesh

#endif  // !_OPENDFN_DFN_GEOMETRY_MESH_UTILS_TRIM_
