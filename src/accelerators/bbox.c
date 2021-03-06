//
//  bbox.c
//  C-ray
//
//  Created by Valtteri Koskivuori on 28/04/2017.
//  Copyright © 2015-2020 Valtteri Koskivuori. All rights reserved.
//

#include "../includes.h"
#include "bbox.h"

#include "../datatypes/vertexbuffer.h"
#include "../datatypes/poly.h"
#include "../utils/assert.h"

/**
 Get the longest axis of an axis-aligned bounding box
 
 @param bbox Bounding box to compute longest axis for
 @return Longest axis as an enum
 */
enum bboxAxis getLongestAxis(const struct boundingBox *bbox) {
	float x = fabsf(bbox->start.x - bbox->end.x);
	float y = fabsf(bbox->start.y - bbox->end.y);
	float z = fabsf(bbox->start.z - bbox->end.z);

	return x > y && x > z ? X : y > z ? Y : Z;
}

/// Compute the surface area of a given bounding box
/// @param box Bounding box to compute surface area for
float findSurfaceArea(const struct boundingBox *box) {
	float width = box->end.x - box->start.x;
	float height = box->end.y - box->start.y;
	float length = box->end.z - box->start.z;
	return 2 * (length * width) + 2 * (length * height) + 2 * (width * height);
}

/**
 Compute the bounding box for a given array of polygons

 @param polys Indices to polygons to compute bounding box for
 @param count Amount of polygons indices given
 @return Axis-aligned bounding box
 */
struct boundingBox *computeBoundingBox(const int *polys, const int count) {
	ASSERT(polys);
	ASSERT(count > 0);
	struct boundingBox *bbox = calloc(1, sizeof(struct boundingBox));
	struct vector minPoint = vecWithPos(FLT_MAX, FLT_MAX, FLT_MAX);
	struct vector maxPoint = vecWithPos(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	
	for (int i = 0; i < count; ++i) {
		for (int j = 0; j < 3; ++j) {
			minPoint = vecMin(minPoint, vertexArray[polygonArray[polys[i]].vertexIndex[j]]);
			maxPoint = vecMax(maxPoint, vertexArray[polygonArray[polys[i]].vertexIndex[j]]);
		}
	}
	struct vector center = vecWithPos(0.5f * (minPoint.x + maxPoint.x), 0.5f * (minPoint.y + maxPoint.y), 0.5f * (minPoint.z + maxPoint.z));
	float maxDistance = 0.0f;
	for (int i = 0; i < count; ++i) {
		for (int j = 0; j < 3; ++j) {
			struct vector fromCenter = vecSub(vertexArray[polygonArray[polys[i]].vertexIndex[j]], center);
			maxDistance = max(maxDistance, pow(vecLength(fromCenter), 2));
		}
	}
	bbox->start = minPoint;
	bbox->end = maxPoint;
	bbox->midPoint = center;
	bbox->surfaceArea = findSurfaceArea(bbox);
	return bbox;
}


/**
 Check if a ray intersects with an axis-aligned bounding box

 @param box Given bounding box to check against
 @param ray Given light ray to intersect
 @param t Current max t value for the ray
 @return true if intersected, false otherwise
 */
bool rayIntersectWithAABB(const struct boundingBox *box, const struct lightRay *ray, float *t) {
	//If a mesh has no polygons, it won't have a root bbox either.
	if (!box) return false;
	
	struct vector dirfrac = vecWithPos(1.0f / ray->direction.x, 1.0f / ray->direction.y, 1.0f / ray->direction.z);

	float t1 = (box->start.x - ray->start.x)*dirfrac.x;
	float t2 = (box->  end.x - ray->start.x)*dirfrac.x;
	float t3 = (box->start.y - ray->start.y)*dirfrac.y;
	float t4 = (box->  end.y - ray->start.y)*dirfrac.y;
	float t5 = (box->start.z - ray->start.z)*dirfrac.z;
	float t6 = (box->  end.z - ray->start.z)*dirfrac.z;
	
	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));
	
	// if tmax < 0, ray is intersecting AABB, but the whole AABB is behind us
	if (tmax < 0) {
		*t = tmax;
		return false;
	}
	
	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax) {
		*t = tmax;
		return false;
	}
	
	*t = tmin;
	return true;
}
