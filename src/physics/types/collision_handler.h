#pragma once
#include "../rendering/types/vec.h"
#include "collision_mesh.h"

struct PhysState;
class PhysObj;

// Defines a given collision
struct collision
{
	bool colliding;

	PhysObj* collidingObjA;
	PhysObj* collidingObjB;

	// The initial position of object B relative to object A
	// Used for persistent collisions
	vec3 initialPositionOffset;

	// The initial rotation of object B relative to object A
	// Used for persistent collisions
	vec3 initialRotationOffset;

	vec3 contactPointA;
	vec3 contactPointB;

	vec3 contactNormal;
	float collisionDepth;

	vec3 contactTangent1;
	vec3 contactTangent2;
};

struct gjk_collision_info
{
	bool colliding;
	std::vector<vec3> resultSimplex;
};

struct epa_collision_info
{
	vec3 contactPointA;
	vec3 contactPointB;

	vec3 contactNormal;
	float collisionDepth;
};

/*
Handles collisions in a given PhysState, and adds `CollisionConstraint`s for them.
Uses the GJK algorithm to detect collisions, and the EPA algorithm to get the collision information.
Based on https://blog.hamaluik.ca/posts/building-a-collision-engine-part-1-2d-gjk-collision-detection/,
https://blog.hamaluik.ca/posts/building-a-collision-engine-part-3-3d-gjk-collision-detection/, and
https://winter.dev/articles/epa-algorithm.
*/
class CollisionHandler
{
public:
	void HandleCollisions(PhysState* state);

private:
	// Get the support vertex for the given block and support direction
	vec3 GetSupport(const std::vector<collision_vert>& block, vec3 blockPos, vec3 objRot, vec3 dir);

	// Adds a new support vertex to the simplex, and checks if it went past the origin. If a support vertex doesn't go past the origin, the two blocks aren't colliding.
	bool AddSupport(
		std::vector<vec3>& simplex,
		const std::vector<collision_vert>& blockA, vec3 blockAPos, vec3 blockARot,
		const std::vector<collision_vert>& blockB, vec3 blockBPos, vec3 blockBRot,
		vec3 dir);

	gjk_collision_info GJK(
			const std::vector<collision_vert>& blockA, vec3 blockAPos, vec3 blockARot,
			const std::vector<collision_vert>& blockB, vec3 blockBPos, vec3 blockBRot);

	// Get the closest face to the origin for the given block
	std::vector<size_t> GetClosestFace(const std::vector<collision_vert>& block, vec3 blockPos, vec3 objRot);

	epa_collision_info EPA(
		gjk_collision_info info,
		const std::vector<collision_vert>& blockA, vec3 blockAPos, vec3 blockARot,
		const std::vector<collision_vert>& blockB, vec3 blockBPos, vec3 blockBRot);

};