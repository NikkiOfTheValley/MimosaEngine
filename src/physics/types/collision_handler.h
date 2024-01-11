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

	// The two contact points for objects A and B in world space
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

	// The results from the support function for object A
	std::vector<vec3> resultSupportA;

	// The results from the support function for object B
	std::vector<vec3> resultSupportB;
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
Uses the GJK algorithm to detect collisions, and EPA to get the collision information.
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

	/*
	Adds a new support vertex to the simplex, while also storing the individual results of the support function for objects A and B
	and checks if it went past the origin. If a support vertex doesn't go past the origin, the two blocks aren't colliding.
	*/
	bool AddSupport(
		std::vector<vec3>& simplex,
		std::vector<vec3>& supportA,
		std::vector<vec3>& supportB,
		const std::vector<collision_vert>& blockA, vec3 blockAPos, vec3 blockARot,
		const std::vector<collision_vert>& blockB, vec3 blockBPos, vec3 blockBRot,
		vec3 dir);

	gjk_collision_info GJK(
			const std::vector<collision_vert>& blockA, vec3 blockAPos, vec3 blockARot,
			const std::vector<collision_vert>& blockB, vec3 blockBPos, vec3 blockBRot);

	// Returns the face normals and distance to the origin in the format std::pair<normal, distance> for the given polytope
	std::vector<std::pair<vec3, float>> GetFaceNormalsAndDistance(const std::vector<vec3>& polytope, const std::vector<size_t>& faceIndices);

	// Returns the closest face to the origin and its distance using the normals and distance information from the GetFaceNormalsAndDistance function
	std::pair<size_t, float> GetClosestFace(const std::vector<std::pair<vec3, float>>& normalsAndDistance);

	/*
	Adds the edge a, b to the uniqueEdges vector if it's unique.
	Removes the existing edge if the edge a, b isn't unique, but doesn't add the new edge.
	*/
	void AddEdgeIfUnique(std::vector<std::pair<size_t, size_t>>& uniqueEdges, const std::vector<size_t>& faceIndices, size_t a, size_t b);

	epa_collision_info EPA(
		gjk_collision_info info,
		const std::vector<collision_vert>& blockA, vec3 blockAPos, vec3 blockARot,
		const std::vector<collision_vert>& blockB, vec3 blockBPos, vec3 blockBRot);

};