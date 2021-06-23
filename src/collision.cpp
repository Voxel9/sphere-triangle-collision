#include "collision.h"

//--------------------------------------------------------------------------------
// Name: IsIntersectingSphereTriangle
// Desc: Performs a test to check if a given sphere intersects with a triangle
//       
//       Adapted from:
//       http://realtimecollisiondetection.net/blog/?p=103
//--------------------------------------------------------------------------------
bool IsIntersectingSphereTriangle(CollisionPacket& collisionPacket, vec3 A, vec3 B, vec3 C, vec3 P, float r) {
    // Transform the triangle vertices to sphere-space
    A = A - P;
    B = B - P;
    C = C - P;
    
    // Is sphere intersecting triangle plane?
    float rr = r * r;
    vec3 V = normalize(cross(B - A, C - A));
    float d = dot(A, V);
    
    // Extra optimization to ignore collision from behind the triangle
    if(d > 0.25f)
        return false;
    
    float e = dot(V, V);
    int sep1 = d * d > rr * e;
    
    if (sep1)
        return false;
    
    // Is sphere intersecting point A?
    float aa = dot(A, A);
    float ab = dot(A, B);
    float ac = dot(A, C);
    int sep2 = (aa > rr) & (ab > aa) & (ac > aa);
    
    if (sep2)
        return false;
    
    // Is sphere intersecting point B?
    float bb = dot(B, B);
    float bc = dot(B, C);
    int sep3 = (bb > rr) & (ab > bb) & (bc > bb);
    
    if (sep3)
        return false;
    
    // Is sphere intersecting point C?
    float cc = dot(C, C);
    int sep4 = (cc > rr) & (ac > cc) & (bc > cc);
    
    if (sep4)
        return false;
    
    // Calculate triangle edge deltas
    vec3 AB = B - A;
    vec3 BC = C - B;
    vec3 CA = A - C;
    
    // Is sphere intersecting edge A to B?
    float d1 = ab - aa;
    float e1 = dot(AB, AB);
    
    vec3 Q1 = A * e1 - AB * d1;
    vec3 QC = C * e1 - Q1;
    int sep5 = (dot(Q1, Q1) > rr * e1 * e1) & (dot(Q1, QC) > 0);
    
    if (sep5)
        return false;
    
    // Is sphere intersecting edge B to C?
    float d2 = bc - bb;
    float e2 = dot(BC, BC);
    
    vec3 Q2 = B * e2 - BC * d2;
    vec3 QA = A * e2 - Q2;
    int sep6 = (dot(Q2, Q2) > rr * e2 * e2) & (dot(Q2, QA) > 0);
    
    if (sep6)
        return false;
    
    // Is sphere intersecting edge C to A?
    float d3 = ac - cc;
    float e3 = dot(CA, CA);
    
    vec3 Q3 = C * e3 - CA * d3;
    vec3 QB = B * e3 - Q3;
    int sep7 = (dot(Q3, Q3) > rr * e3 * e3) & (dot(Q3, QB) > 0);
    
    if (sep7)
        return false;
    
    // Sphere intersects triangle; calculate amount to push sphere back
    collisionPacket.normal = V;
    collisionPacket.distance = d;
    
    return true;
}
