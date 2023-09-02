#pragma once
#include <glm/glm.hpp>
#include <algorithm>

using namespace std;

class Ray
{
public:
   Ray(const glm::vec3& orig, const glm::vec3& dir) : orig(orig), dir(dir)
   {
      invdir = glm::vec3(1.0f) / dir;
      sign[0] = (invdir.x < 0);
      sign[1] = (invdir.y < 0);
      sign[2] = (invdir.z < 0);
   }
   glm::vec3 orig, dir;       // ray orig and dir
   glm::vec3 invdir;
   int sign[3];
};


class Box3
{
   enum Bound {
      V_MIN,
      V_MAX
   };
public:

   Box3(const glm::vec3& vmin, const glm::vec3& vmax)
   {
      bounds[V_MIN] = vmin;
      bounds[V_MAX] = vmax;
   }
   glm::vec3 bounds[2];

   bool intersect(glm::vec3 point)
   {
      return (
         point.x >= bounds[V_MIN].x &&
         point.x <= bounds[V_MAX].x &&
         point.y >= bounds[V_MIN].y &&
         point.y <= bounds[V_MAX].y &&
         point.z >= bounds[V_MIN].z &&
         point.z <= bounds[V_MAX].z
         );
   }

   float intersect(const Ray& r)
   {
      float tmin, tmax, tymin, tymax, tzmin, tzmax;

      tmin = (bounds[r.sign[0]].x - r.orig.x) * r.invdir.x;
      tmax = (bounds[1 - r.sign[0]].x - r.orig.x) * r.invdir.x;
      tymin = (bounds[r.sign[1]].y - r.orig.y) * r.invdir.y;
      tymax = (bounds[1 - r.sign[1]].y - r.orig.y) * r.invdir.y;

      if ((tmin > tymax) || (tymin > tmax))
         return -1.0f;

      if (tymin > tmin)
         tmin = tymin;
      if (tymax < tmax)
         tmax = tymax;

      tzmin = (bounds[r.sign[2]].z - r.orig.z) * r.invdir.z;
      tzmax = (bounds[1 - r.sign[2]].z - r.orig.z) * r.invdir.z;

      if ((tmin > tzmax) || (tzmin > tmax))
         return -1.0f;

      if (tzmin > tmin)
         tmin = tzmin;
      if (tzmax < tmax)
         tmax = tzmax;

      return min(tmax, tmin);
   }
};
