#pragma once
#include <math/vec.h>
#include "collision_mesh.h"

struct PhysState;
class PhysObj;

/*!
 * @brief Defines a given collision
 * @var colliding True if the objects are colliding
 * @var collidingObjA A pointer to object A, or nullptr if there is no collision
 * @var collidingObjB A pointer to object B, or nullptr if there is no collision
 * @var initialPositionOffset The initial position of object B relative to object A (used for persistent collisions)
 * @var initialRotationOffset The initial rotation of object B relative to object A (used for persistent collisions)
 * @var contactPointA The contact point on object A in world space
 * @var contactPointB The contact point on object B in world space
 * @var contactNormal The normal of the collision plane
 * @var collisionDepth The depth of the collision
 * @var contactTangent1 The first contact tangent. Forms an orthonormal basis with the contact normal and the second contact tangent
 * @var contactTangent2 The second contact tangent. Forms an orthonormal basis with the contact normal and the first contact tangent
*/
struct collision
{
	bool colliding;

	PhysObj* collidingObjA;
	PhysObj* collidingObjB;

	math::vec3 initialPositionOffset;
	math::vec3 initialRotationOffset;

	math::vec3 contactPointA;
	math::vec3 contactPointB;

	math::vec3 contactNormal;
	float collisionDepth;

	math::vec3 contactTangent1;
	math::vec3 contactTangent2;
};

/*!
 * @brief The collision info returned by GJK
 * @var colliding True if the object are colliding
 * @var resultSimplex The simplex at the end of the GJK algorithm
 * @var resultSupportA The supports for object A at the end of the GJK algorithm
 * @var resultSupportB The supports for object B at the end of the GJK algorithm
*/
struct gjk_collision_info
{
	bool colliding;
	std::vector<math::vec3> resultSimplex;

	// The results from the support function for object A
	std::vector<math::vec3> resultSupportA;

	// The results from the support function for object B
	std::vector<math::vec3> resultSupportB;
};

/*!
 * @brief The collision information returned by EPA
 * @var contactPointA The contact point on objA in world space
 * @var contactPointB The contact point on objB in world space
 * @var contactNormal The normal of the plane of contact
 * @var collisionDepth The depth of the collision
*/
struct epa_collision_info
{
	math::vec3 contactPointA;
	math::vec3 contactPointB;

	math::vec3 contactNormal;
	float collisionDepth;
};

/*!
 * @brief Handles collisions in a given PhysState, and adds `CollisionConstraint`s for them.
 * 
 * Uses the GJK algorithm to detect collisions, and EPA to get the collision information.
 * Based on https://blog.hamaluik.ca/posts/building-a-collision-engine-part-1-2d-gjk-collision-detection/,
 * https://blog.hamaluik.ca/posts/building-a-collision-engine-part-3-3d-gjk-collision-detection/, and
 * https://winter.dev/articles/epa-algorithm.
*/
class CollisionHandler
{
public:
	/*!
	 * @brief Handles collisions for a given physics state
	 * @param[in,out] state The physics state
	*/
	void HandleCollisions(PhysState* state);

private:
	/*!
	 * @brief Handles the collisions for primitive `PhysObj`s
	 * @param state The physics state
	 * @param objA The object to handle collisions for
	 * @param[out] collisions The collisions that were detected
	*/
	void HandleCollisionsPrimitive(PhysState* state, PhysObj& objA, std::vector<collision>& collisions);

	/*!
	 * @brief Handles the collisions for convex decomposition `PhysObj`s
	 * @param state The physics state
	 * @param objA The object to handle collisions for
	 * @param[out] collisions The collisions that were detected
	*/
	void HandleCollisionsConvexDecomposition(PhysState* state, PhysObj& objA, std::vector<collision>& collisions);


	/*!
	 * @brief Generates the support vertex for the given block and support direction
	 * @param block The block to generate the support from
	 * @param blockPos The model-space position of the block
	 * @param objRot The rotation of the block
	 * @param dir The direction to generate the support in
	 * @return The support point. Returns vec3(NaN, NaN, NaN) if there was no support point
	*/
	math::vec3 GetSupport(const std::vector<collision_vert>& block, math::vec3 blockPos, math::vec3 objRot, math::vec3 dir);

	/*!
	 * @brief Adds a new support vertex to the simplex
	 * 
	 * Stores the individual results of the support function for blocks A and B, and checks if it went past the origin.
	 * If any support vertex doesn't go past the origin, the two blocks aren't colliding.
	 * 
	 * @param[in, out] simplex The simplex to add the support to
	 * @param[in, out] supportA The support vertices of block A
	 * @param[in, out] supportB The support vertices of block B
	 * @param blockA The vertices of block A
	 * @param blockAPos The model-space position of block A
	 * @param blockARot The rotation of block A
	 * @param blockB The vertices of block B
	 * @param blockBPos The model-space position of block A
	 * @param blockBRot The rotation of block A
	 * @param dir The direction to add the support vertex in
	 * @return True if the vertex went past the origin, false otherwise
	*/
	bool AddSupport(
		std::vector<math::vec3>& simplex,
		std::vector<math::vec3>& supportA,
		std::vector<math::vec3>& supportB,
		const std::vector<collision_vert>& blockA, math::vec3 blockAPos, math::vec3 blockARot,
		const std::vector<collision_vert>& blockB, math::vec3 blockBPos, math::vec3 blockBRot,
		math::vec3 dir);

	/*!
	 * @brief Implements the GJK collision detection algorithm
	 * @param blockA A convex "block" of object A
	 * @param blockAPos The model-space position of block A
	 * @param blockARot The rotation of block A
	 * @param blockB A convex "block" of object B
	 * @param blockBPos The model-space position of block B
	 * @param blockBRot The rotation of block B
	 * @return Information about the collision, including whether a collision was detected at all
	*/
	gjk_collision_info GJK(
			const std::vector<collision_vert>& blockA, math::vec3 blockAPos, math::vec3 blockARot,
			const std::vector<collision_vert>& blockB, math::vec3 blockBPos, math::vec3 blockBRot);

	/*!
	 * @brief Returns the face normals and distance to the origin for the given polytope
	 * @param polytope The polytope to compute the face normals and distance for
	 * @param faceIndices The face indices of the polytope
	 * @return The face formals and distance to the origin in the format std::pair<normal, distance>
	 * @warning Causes a fatal error if the faceIndices vector is empty.
	*/
	std::vector<std::pair<math::vec3, float>> GetFaceNormalsAndDistance(const std::vector<math::vec3>& polytope, const std::vector<size_t>& faceIndices);

	/*!
	 * @brief Computes closest face to the origin and its distance using the normals and distance information from `GetFaceNormalsAndDistance`
	 * @param normalsAndDistance The normals and distance information from `GetFaceNormalsAndDistance`
	 * @return The closest face in the format std::pair<faceIndex, distance>
	 * @warning Causes a fatal error if normalsAndDistance is empty.
	 * @warning Causes a fatal error if there is a face with NaN distance to the origin, or if there is no closest face.
	*/
	std::pair<size_t, float> GetClosestFace(const std::vector<std::pair<math::vec3, float>>& normalsAndDistance);

	/*!
	 * @brief Adds the edge a, b to the uniqueEdges vector if it's unique
	 * 
	 * Removes the existing edge if the edge a, b isn't unique, but doesn't add the new edge.
	 * 
	 * @param[in,out] uniqueEdges The vector to add and remove edges from
	 * @param faceIndices The face indices
	 * @param a The index of the first point of the edge
	 * @param b The index of the second point of the edge
	*/
	void AddEdgeIfUnique(std::vector<std::pair<size_t, size_t>>& uniqueEdges, const std::vector<size_t>& faceIndices, size_t a, size_t b);

	/*!
	 * @brief Implements the Expanding Polytope Algorithm
	 * @param info The collision information from GJK
	 * @param blockA A convex "block" of object A
	 * @param blockAPos The model-space position of block A
	 * @param blockARot The rotation of block A
	 * @param blockB A convex "block" of object B
	 * @param blockBPos The model-space position of block B
	 * @param blockBRot The rotation of block B
	 * @return Information about the collision, used for resolving collisions
	*/
	epa_collision_info EPA(
		gjk_collision_info info,
		const std::vector<collision_vert>& blockA, math::vec3 blockAPos, math::vec3 blockARot,
		const std::vector<collision_vert>& blockB, math::vec3 blockBPos, math::vec3 blockBRot);
};