#pragma once

#include <glm/glm.hpp>
#include"boundingBox.h"

namespace ntn
{

struct Ray
{
	glm::vec3 Origin;
	glm::vec3 Direction;
};

struct Plane_
{
	glm::vec3 normal;
	glm::vec3 point;

	// Constructor to define a plane using a normal vector and a point
	Plane_(const glm::vec3& normal, const glm::vec3& point) :
		normal(glm::normalize(normal)), point(point) {}

	// Constructor to define a plane using a point and two vectors in the plane
	Plane_(const glm::vec3& point, const glm::vec3& v1, const glm::vec3& v2)
	{
		normal = glm::normalize(glm::cross(v1, v2));
		this->point = point;
	}
};

struct HitPayload
{
	float HitDistance;
	glm::vec3 WorldPosition;
	glm::vec3 WorldNormal;

	int ObjectIndex;
};

// Project the movement vector onto the ground plane
static glm::vec3 projectOntoGround(const glm::vec3& originalPosition, const glm::vec3& movementVector, const glm::vec3& groundNormal)
{
	// Ensure the ground normal is normalized
	glm::vec3 groundNormalNormalized = glm::normalize(groundNormal);

	// Project the movement vector onto the plane defined by the ground normal
	glm::vec3 projectedVector = movementVector - glm::dot(movementVector, groundNormalNormalized) * groundNormalNormalized;

	// Return the new position after projection
	return originalPosition + projectedVector;
}

static bool RayIntersectsPlane(const Ray& ray, Plane_& plane, glm::vec3& intersectPts)
{
	// Check if the ray and plane are not parallel
	float denom = glm::dot(ray.Direction, plane.normal);
	if (std::abs(denom) > 1e-6)
	{
		// Calculate the parameter along the ray where it intersects the plane
		float t = glm::dot(plane.point - ray.Origin, plane.normal) / denom;

		// Check if the intersection point is in front of the ray origin
		if (t >= 0.0f)
		{
			intersectPts = ray.Origin + t * ray.Direction;
			return true;
		}
	}
	// No intersection
	return false;
}

}
