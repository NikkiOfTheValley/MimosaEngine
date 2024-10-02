#pragma once
#include "rendering/types/vec.h"
#include <vector>

// Predefine Mesh and PhysObj so we don't have to `include` them
class Mesh;
class PhysObj;

struct collision_vert
{
	vec3 pos;
	vec3 normal;
};

enum CollisionPrimitiveType
{
	Cube,
	AABB,
	Plane,
	Sphere,
	Cylinder
};

/*!
 * @brief An optimized primitive collision mesh
 * 
 * @var type The type of this primitive
 * @var offset The offset from the model-space origin for this primitive
 * @var rotation The rotation of the primitive (does not apply to AABBs)
 * @var scale The scale in the XYZ axes for this primitive
*/
struct CollisionPrimitive
{
	CollisionPrimitiveType type;
	vec3 offset;
	vec3 rotation;
	vec3 scale;
};

/*!
 * @brief A collision mesh.
*/
class CollisionMesh
{
public:
	/*!
	 * @brief Adds a optimized primitive shape.
	 * 
	 * Can be called more than once to create multi-primitive collision meshes.
	 * 
	 * @param primitive The primitive to add
	*/
	void AddPrimitive(CollisionPrimitive primitive);

	/*!
	 * @brief Loads a CollisionMesh file.
	 * 
	 * @param path The path to the file
	*/
	void LoadFromFile(std::string path);

	const std::vector<CollisionPrimitive> GetPrimitives();

private:
	std::vector<CollisionPrimitive> primitives;
};