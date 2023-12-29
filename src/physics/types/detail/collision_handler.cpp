#include "../collision_handler.h"
#include "../phys_state.h"
#include "../phys_obj.h"

void CollisionHandler::HandleCollisions(PhysState* state)
{
	for (auto& objA : state->objects)
	{
		// Skip object if it isn't initialized
		if (objA.index > MAX_PHYS_OBJECTS)
			continue;

		// First, we remove any existing collision constraints, as they aren't up to date anymore

		size_t startingIndex = objA.index * (MAX_CONSTRAINTS_PER_PHYS_OBJ - 1);

		for (size_t i = startingIndex; i < startingIndex + MAX_CONSTRAINTS_PER_PHYS_OBJ - 1; i++)
		{
			Constraint* constraint = state->objConstraints[i];

			if (constraint == nullptr)
				continue;

			if (constraint->Type() == ConstraintType::COLLISION)
				state->objConstraints[i] = nullptr;

		}

		std::vector<collision> collisions;

 		phys_obj_prop objAProperties = objA.GetProperties();

		// For each block, check if it collides with any other object's block(s) and add collisions if necessary
		for (auto& blockA : objA.collisionMesh.GetBlocks())
		{
			for (auto& objB : state->objects)
			{
				// Skip object if it isn't initialized or if it's the same as objA
				if (objB.index > MAX_PHYS_OBJECTS || objB.index == objA.index)
					continue;

				phys_obj_prop objBProperties = objB.GetProperties();

				for (auto& blockB : objB.collisionMesh.GetBlocks())
				{
					gjk_collision_info info = GJK(blockA, objAProperties.pos, objAProperties.rot, blockB, objBProperties.pos, objBProperties.rot);

					if (info.colliding)
					{
						//Logger::getInstance().log("detected collision");
						//Logger::getInstance().log("objA pos: " + (std::string)objAProperties.pos);

						collision collision;
						collision.colliding = true;
						collision.collidingObjA = &objA;
						collision.collidingObjB = &objB;
						
						//state->objVelVector.data[(objA.index * 3) + 1] = 0.f;
						//state->objAccelVector.data[(objA.index * 3) + 1] = 0.f;
						//state->objForceVector.data[(objA.index * 3) + 1] = 0.f;
						//state->objPositionVector.data[(objA.index * 3) + 1] = 0.f;

						epa_collision_info epa_info = EPA(info, blockA, objAProperties.pos, objAProperties.rot, blockB, objBProperties.pos, objBProperties.rot);

						collision.contactPointA = epa_info.contactPointA;
						collision.contactPointB = epa_info.contactPointB;

						collision.contactNormal = epa_info.contactNormal;
						collision.collisionDepth = epa_info.collisionDepth;

						// Weird way of computing an orthonormal basis from https://box2d.org/posts/2014/02/computing-a-basis/

						vec3 contactTangent1;
						if (collision.contactNormal.x >= 0.57735f)
							contactTangent1 = vec3(collision.contactNormal.y, -collision.contactNormal.x, 0.f);
						else
							contactTangent1 = vec3(0.f, collision.contactNormal.z, -collision.contactNormal.y);

						contactTangent1 = normalize(contactTangent1);
						vec3 contactTangent2 = cross(collision.contactNormal, contactTangent1);

						collision.contactTangent1 = contactTangent1;
						collision.contactTangent2 = contactTangent2;

						collisions.push_back(collision);
					}
				}
			}
		}

		// All collisions have been added, so add the collision constraints to the object
		for (auto& collision : collisions)
		{
			CollisionConstraint* constraint = new CollisionConstraint();
			constraint->collision = collision;

			startingIndex = objA.index * (MAX_CONSTRAINTS_PER_PHYS_OBJ - 1);

			bool wasConstraintSet = false;

			for (size_t i = startingIndex; i < startingIndex + MAX_CONSTRAINTS_PER_PHYS_OBJ - 1; i++)
			{
				// Wait until we hit a constraint that isn't set, then use that "slot" for the new constraint
				if (!state->objConstraints[i])
				{
					state->objConstraints[i] = constraint;
					wasConstraintSet = true;
					break;
				}
			}

			if (!wasConstraintSet)
				Logger::getInstance().err("Failed to set constraint because the number of constraints is >= MAX_CONSTRAINTS_PER_PHYS_OBJ");

		}

	}
}

vec3 CollisionHandler::GetSupport(const std::vector<collision_vert>& block, vec3 objPos, vec3 objRot, vec3 dir)
{
	vec3 farthestVert;
	float farthestDistance = -INFINITY;

	mat3x3f blockRotationMatrix;
	blockRotationMatrix.rotate(objRot);

	for (auto& vert : block)
	{
		vec3 rot = dir * blockRotationMatrix;

		float distance = dot(vert.pos + objPos, rot);

		if (distance > farthestDistance)
		{
			farthestDistance = distance;
			farthestVert = vert.pos;
		}
	}

	return farthestVert + objPos;
}

bool CollisionHandler::AddSupport(
	std::vector<vec3>& simplex,
	const std::vector<collision_vert>& blockA, vec3 blockAPos, vec3 blockARot,
	const std::vector<collision_vert>& blockB, vec3 blockBPos, vec3 blockBRot,
	vec3 dir)
{
	vec3 newVert = GetSupport(blockA, blockAPos, blockARot, dir) - GetSupport(blockB, blockBPos, blockBRot, dir * -1.f);

	simplex.push_back(newVert);

	return dot(dir, newVert) >= 0;
}

gjk_collision_info CollisionHandler::GJK(
	const std::vector<collision_vert>& blockA, vec3 blockAPos, vec3 blockARot,
	const std::vector<collision_vert>& blockB, vec3 blockBPos, vec3 blockBRot)
{
	std::vector<vec3> simplex;

	// Pre-rotate the blocks so the GetSupport function doesn't have to do a matrix-vector multiplication for every vert

	mat3x3f blockARotationMatrix;
	mat3x3f blockBRotationMatrix;

	blockARotationMatrix.rotate(blockARot);
	blockBRotationMatrix.rotate(blockBRot);

	std::vector<collision_vert> blockARotated = blockA;
	std::vector<collision_vert> blockBRotated = blockB;

	for (auto& vertA : blockARotated)
		vertA.pos = vertA.pos * blockARotationMatrix;

	for (auto& vertB : blockBRotated)
		vertB.pos = vertB.pos * blockBRotationMatrix;


	// The direction should be the vector between the two block's centers, as that is more efficent for most cases
	vec3 dir = blockAPos - blockBPos;

	// If the AddSupport function returns false, the blocks cannot be colliding, so we should exit early
	if (!AddSupport(simplex, blockARotated, blockAPos, blockARot, blockBRotated, blockBPos, blockBRot, dir))
	{
		// We don't need to specify the simplex, as EPA won't be used if the blocks aren't colliding
		gjk_collision_info info;
		info.colliding = false;

		return info;
	}


	// Flip direction
	dir *= -1.f;

	if (!AddSupport(simplex, blockARotated, blockAPos, blockARot, blockBRotated, blockBPos, blockBRot, dir))
	{
		gjk_collision_info info;
		info.colliding = false;

		return info;
	}


	// Calculate vertex c from vertices a and b to create a triangle

	vec3 a = simplex[0];
	vec3 b = simplex[1];

	// The line ab is formed from the first two vertices
	vec3 ab = b - a;

	// The line a0 is the line from a to the origin
	vec3 a0 = a * -1.f;

	// The direction should be the line perpendicular to the line ab in the direction of the origin
	dir = tripleProduct(ab, a0, ab);

	if (!AddSupport(simplex, blockARotated, blockAPos, blockARot, blockBRotated, blockBPos, blockBRot, dir))
	{
		gjk_collision_info info;
		info.colliding = false;

		return info;
	}


	// Calculate vertex d from vertices a, b, and c to create a tetrahedron

	a = simplex[0];
	b = simplex[1];
	vec3 c = simplex[2];

	// The direction should be the normal of the triangle abc towards the origin

	// Calculate the triangle's normal using the usual method
	vec3 ac = c - a;
	ab = b - a;
	dir = cross(ac, ab);

	// Since the usual method doesn't guarantee that the normal faces towards the origin,
	// we just flip the normal if it's pointing away from the origin

	a0 = a * -1.f;
	if (dot(dir, a0) < 0)
		dir *= -1.f;

	if (!AddSupport(simplex, blockARotated, blockAPos, blockARot, blockBRotated, blockBPos, blockBRot, dir))
	{
		gjk_collision_info info;
		info.colliding = false;

		return info;
	}


	// Check if the origin is contained within the tetrahedron formed by a, b, c, and d
	// If it isn't, evolve the simplex until we've either made sure that the simplex
	// cannot possibly contain the origin (and thus, that the blocks aren't colliding),
	// or that the origin is contained within the simplex (and thus, that the blocks are colliding).

	// This while loop is fine, as the algorithm is guaranteed to eventually return
	while (true)
	{
		a = simplex[0];
		b = simplex[1];
		c = simplex[2];
		vec3 d = simplex[3];

		// Get the three edges connected to vertex d
		vec3 da = d - a;
		vec3 db = d - b;
		vec3 dc = d - c;

		// The line d0 is the line from d to the origin
		vec3 d0 = d * -1.f;

		// Calculate normals of the three triangles connected to vertex d
		vec3 abdNormal = cross(da, db);
		vec3 bcdNormal = cross(db, dc);
		vec3 cadNormal = cross(dc, da);

		if (dot(abdNormal, d0) > 0)
		{
			// If the origin is "outside" of the triangle formed by abd, move c to the
			// support vertex in the direction of the triangle's normal, as it is useless in this case

			c = GetSupport(blockARotated, blockAPos, blockARot, dir) - GetSupport(blockBRotated, blockBPos, blockBRot, dir * -1.f);
			simplex.erase(simplex.begin() + 2);
			simplex.push_back(c);

			// If c didn't pass the origin, then the simplex cannot possibly contain the origin, so we exit early
			if (dot(dir, c) >= 0)
			{
				gjk_collision_info info;
				info.colliding = false;

				return info;
			}
		}
		else if (dot(bcdNormal, d0) > 0)
		{
			// If the origin is "outside" of the triangle formed by bcd, move a to the
			// support vertex in the direction of the triangle's normal, as it is useless in this case

			a = GetSupport(blockARotated, blockAPos, blockARot, dir) - GetSupport(blockBRotated, blockBPos, blockBRot, dir * -1.f);
			simplex.erase(simplex.begin());
			simplex.push_back(a);
				
			// If a didn't pass the origin, then the simplex cannot possibly contain the origin, so we exit early
			if (dot(dir, a) >= 0)
			{
				gjk_collision_info info;
				info.colliding = false;

				return info;
			}
		}
		else if (dot(cadNormal, d0) > 0)
		{
			// If the origin is "outside" of the triangle formed by cad, move b to the
			// support vertex in the direction of the triangle's normal, as it is useless in this case

			b = GetSupport(blockARotated, blockAPos, blockARot, dir) - GetSupport(blockBRotated, blockBPos, blockBRot, dir * -1.f);
			simplex.erase(simplex.begin() + 1);
			simplex.push_back(b);

			// If b didn't pass the origin, then the simplex cannot possibly contain the origin, so we exit early
			if (dot(dir, b) >= 0)
			{
				gjk_collision_info info;
				info.colliding = false;

				return info;
			}
		}
		else
		{
			// If the origin is within triangles abd, bcd, and cad, we found an intersection,
			// as those triangles define the tetrahedron, even though abc is missing.

			gjk_collision_info info;
			info.colliding = true;
			info.resultSimplex = simplex;

			return info;
		}
	}
}

// Get the closest face to the origin for the given block
std::vector<size_t> CollisionHandler::GetClosestFace(const std::vector<collision_vert>& /*block*/, vec3 /*blockPos*/, vec3 /*objRot*/)
{

}

epa_collision_info CollisionHandler::EPA(
	gjk_collision_info info,
	const std::vector<collision_vert>& /*blockA*/, vec3 /*blockAPos*/, vec3 /*blockARot*/,
	const std::vector<collision_vert>& /*blockB*/, vec3 /*blockBPos*/, vec3 /*blockBRot*/)
{
	// We initialize the polytope to the final simplex from GJK, as that is a good starting point
	std::vector<vec3> polytope = info.resultSimplex;

	// We also need a way of defining each face (triangle) of the polytope,
	// as the result from GJK doesn't include face information

	// We initialize the face indices with counterclockwise winding
	std::vector<size_t> faceIndices = {
		0, 1, 2,
		0, 3, 1,
		0, 2, 3,
		1, 3, 2
	};





	return {};
}