#include "../collision_handler.h"
#include "../phys_state.h"
#include "../phys_obj.h"
#include "debug/obj_writer.h"

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

						collision collision;
						collision.colliding = true;
						collision.collidingObjA = &objA;
						collision.collidingObjB = &objB;

						epa_collision_info epa_info = EPA(info, blockA, objAProperties.pos, objAProperties.rot, blockB, objBProperties.pos, objBProperties.rot);

						collision.initialPositionOffset = objAProperties.pos - objBProperties.pos;
						collision.initialRotationOffset = objAProperties.rot - objBProperties.rot;

						collision.contactPointA = epa_info.contactPointA;
						collision.contactPointB = epa_info.contactPointB;
						 
						//Logger::getInstance().log("contact point A: " + (std::string)epa_info.contactPointA);
						//Logger::getInstance().log("contact point B: " + (std::string)epa_info.contactPointB);

						//Logger::getInstance().log("collision depth: " + std::to_string(epa_info.collisionDepth));

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


						PhysObj* cube;
						objA.index == 0 ? cube = &objA : cube = &objB;

						//Logger::getInstance().log(collision.contactNormal);

						vec3 positionOffset = collision.contactNormal * collision.collisionDepth;

						state->objPositionVector.data[(cube->index * 3) + 0] += positionOffset.x;
						state->objPositionVector.data[(cube->index * 3) + 1] += positionOffset.y;
						state->objPositionVector.data[(cube->index * 3) + 2] += positionOffset.z;

						//state->objVelVector.data[(cube->index * 3) + 1] *= -1.f;

						//Logger::getInstance().log(cube->GetProperties().pos);
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
	float farthestDistance = FLT_MIN;

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
	std::vector<vec3>& supportA,
	std::vector<vec3>& supportB,
	const std::vector<collision_vert>& blockA, vec3 blockAPos, vec3 blockARot,
	const std::vector<collision_vert>& blockB, vec3 blockBPos, vec3 blockBRot,
	vec3 dir)
{
	vec3 newSupportA = GetSupport(blockA, blockAPos, blockARot, dir);
	vec3 newSupportB = GetSupport(blockB, blockBPos, blockBRot, dir * -1.f);

	vec3 newVert = newSupportA - newSupportB;

	simplex.push_back(newVert);
	supportA.push_back(newSupportA);
	supportB.push_back(newSupportB);

	return dot(dir, newVert) >= 0;
}

gjk_collision_info CollisionHandler::GJK(
	const std::vector<collision_vert>& blockA, vec3 blockAPos, vec3 blockARot,
	const std::vector<collision_vert>& blockB, vec3 blockBPos, vec3 blockBRot)
{
	std::vector<vec3> simplex;

	// The results from the support function for object A
	std::vector<vec3> supportA;

	// The results from the support function for object B
	std::vector<vec3> supportB;


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
	if (!AddSupport(simplex, supportA, supportB, blockARotated, blockAPos, blockARot, blockBRotated, blockBPos, blockBRot, dir))
	{
		// We don't need to specify the simplex, as EPA won't be used if the blocks aren't colliding
		gjk_collision_info info;
		info.colliding = false;

		return info;
	}


	// Flip direction
	dir *= -1.f;

	if (!AddSupport(simplex, supportA, supportB, blockARotated, blockAPos, blockARot, blockBRotated, blockBPos, blockBRot, dir))
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

	if (!AddSupport(simplex, supportA, supportB, blockARotated, blockAPos, blockARot, blockBRotated, blockBPos, blockBRot, dir))
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

	if (!AddSupport(simplex, supportA, supportB, blockARotated, blockAPos, blockARot, blockBRotated, blockBPos, blockBRot, dir))
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

			// TODO: Refactor this mess to use AddSupport rather than reimplementing it 3 times

			vec3 newSupportA = GetSupport(blockARotated, blockAPos, blockARot, dir);
			vec3 newSupportB = GetSupport(blockBRotated, blockBPos, blockBRot, dir * -1.f);

			c = newSupportA - newSupportB;
			simplex.erase(simplex.begin() + 2);
			simplex.push_back(c);

			supportA.erase(supportA.begin() + 2);
			supportA.push_back(newSupportA);

			supportB.erase(supportB.begin() + 2);
			supportB.push_back(newSupportB);

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

			vec3 newSupportA = GetSupport(blockARotated, blockAPos, blockARot, dir);
			vec3 newSupportB = GetSupport(blockBRotated, blockBPos, blockBRot, dir * -1.f);

			a = newSupportA - newSupportB;
			simplex.erase(simplex.begin());
			simplex.push_back(a);

			supportA.erase(supportA.begin());
			supportA.push_back(newSupportA);

			supportB.erase(supportB.begin());
			supportB.push_back(newSupportB);
				
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

			vec3 newSupportA = GetSupport(blockARotated, blockAPos, blockARot, dir);
			vec3 newSupportB = GetSupport(blockBRotated, blockBPos, blockBRot, dir * -1.f);

			b = newSupportA - newSupportB;
			simplex.erase(simplex.begin() + 1);
			simplex.push_back(b);

			supportA.erase(supportA.begin() + 1);
			supportA.push_back(newSupportA);

			supportB.erase(supportB.begin() + 1);
			supportB.push_back(newSupportB);

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
			info.resultSupportA = supportA;
			info.resultSupportB = supportB;

			return info;
		}
	}
}

std::vector<std::pair<vec3, float>> CollisionHandler::GetFaceNormalsAndDistance(const std::vector<vec3>& polytope, const std::vector<size_t>& faceIndices)
{
	std::vector<std::pair<vec3, float>> result;

	for (size_t i = 0; i < faceIndices.size(); i += 3)
	{
		vec3 a = polytope[faceIndices[i]];
		vec3 b = polytope[faceIndices[i + 1]];
		vec3 c = polytope[faceIndices[i + 2]];

		//vec3 ac = c - a;
		//vec3 ab = b - a;
		//vec3 normal = cross(ac, ab);

		vec3 normal = normalize(cross(b - a, c - a));

		if (is_nan(normal))
			Logger::getInstance().err("Element of normal is NaN in GetFaceNormalsAndDistance! Face index is " + std::to_string(i) +
				", and triangle coords are " + (std::string)a + " | " + (std::string)b + " | " + (std::string)c);

		float distance = dot(normal, a);

		// Flip normal and distance if the winding isn't correct
		if (distance < 0)
		{
			normal *= -1.f;
			distance *= -1.f;
		}

		if (std::isnan(distance))
		{
			Logger::getInstance().err("Distance to origin is NaN in GetFaceNormalsAndDistance! Face index is " + std::to_string(i) +
				", normal is " + (std::string)normal + ", and triangle coords are " + (std::string)a + " | " + (std::string)b + " | " + (std::string)c);
			continue;
		}
			

		result.emplace_back(normal, distance);
	}

	return result;
}

std::pair<size_t, float> CollisionHandler::GetClosestFace(const std::vector<std::pair<vec3, float>>& normalsAndDistance)
{
	size_t closestFaceIndex = (size_t)-1;
	float closestDistance = FLT_MAX;

	if (normalsAndDistance.size() == 0)
		Logger::getInstance().fatal("normalsAndDistance.size() == 0");

	for (size_t i = 0; i < normalsAndDistance.size(); i++)
	{
		float distance = normalsAndDistance[i].second;

		if (std::isnan(distance))
			Logger::getInstance().fatal("Distance to origin for the closest face in EPA is NaN!");

		if (distance < closestDistance)
		{
			closestFaceIndex = i;
			closestDistance = distance;
		}
	}

	if (closestFaceIndex == -1)
		Logger::getInstance().fatal("There is no closest face to the origin! This shouldn't be possible!");
		

	return std::make_pair(closestFaceIndex, closestDistance);
}

void CollisionHandler::AddEdgeIfUnique(std::vector<std::pair<size_t, size_t>>& uniqueEdges, const std::vector<size_t>& faceIndices, size_t a, size_t b)
{
	// Attempt to find the edge a, b in uniqueEdges
	auto edgeAB = std::find(
		uniqueEdges.begin(),
		uniqueEdges.end(),
		std::make_pair(faceIndices[b], faceIndices[a])
	);

	// If it is found, remove the existing edge
	if (edgeAB != uniqueEdges.end())
		uniqueEdges.erase(edgeAB);
	else
	// If it isn't found, add it
		uniqueEdges.emplace_back(faceIndices[a], faceIndices[b]);
}

epa_collision_info CollisionHandler::EPA(
	gjk_collision_info info,
	const std::vector<collision_vert>& blockA, vec3 blockAPos, vec3 blockARot,
	const std::vector<collision_vert>& blockB, vec3 blockBPos, vec3 blockBRot)
{
	// Pre-rotate the blocks

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


	// We initialize the polytope and support information to the final simplex from GJK, as that is a good starting point
	std::vector<vec3> polytope = info.resultSimplex;
	std::vector<vec3> supportA = info.resultSupportA;
	std::vector<vec3> supportB = info.resultSupportB;

	// We also need a way of defining each face (triangle) of the polytope,
	// as the result from GJK doesn't include face information

	// We initialize the face indices with counterclockwise winding
	std::vector<size_t> faceIndices = {
		0, 1, 2,
		0, 3, 1,
		0, 2, 3,
		1, 3, 2
	};

	std::vector<std::pair<vec3, float>> normalsAndDistance = GetFaceNormalsAndDistance(polytope, faceIndices);

	vec3 closestNormal = vec3();
	float closestDistance = FLT_MAX;
	size_t closestFace = (size_t)-1;

	// Iterate until a closest face to the origin on the Minkowski difference has been found, but limit it
	// to 100 iterations to keep from locking up the physics engine if some weird geometry causes an infinite loop

	size_t iterations = 100;
	while (closestDistance == FLT_MAX && iterations > 0)
	{


		closestFace = GetClosestFace(normalsAndDistance).first;

		closestNormal = normalsAndDistance[closestFace].first;
		closestDistance = normalsAndDistance[closestFace].second;

#ifndef NDEBUG
		obj::WriteOBJ("polytope" + std::to_string(iterations) + ".obj", polytope, faceIndices);
#endif
		// Calculate a new support point in the direction of the face's normal, to check for any faces on the
		// Minkowski difference which aren't part of the existing polytope that may be closer to the origin

		vec3 newSupportA = GetSupport(blockARotated, blockAPos, blockARot, closestNormal);
		vec3 newSupportB = GetSupport(blockBRotated, blockBPos, blockBRot, closestNormal * -1.f);
		vec3 support = newSupportA - newSupportB;

		float supportDistance = dot(closestNormal, support);


		// If the difference between closestDistance and supportDistance exceeds COLLISION_EPSILON,
		// then the support vertex is closer to the origin than the old closestDistance, so we need to
		// recalculate the polytope and update the closestNormal and closestDistance.
		// If the difference doesn't exceed COLLISION_EPSILON, we exit the loop, as we have found
		// the closest face to the origin.
		if (abs(supportDistance - closestDistance) > COLLISION_EPSILON)
		{
			// Set closestDistance to FLT_MAX so the loop doesn't exit
			closestDistance = FLT_MAX;

			// Remove every face that's pointing in the same direction as the support point,
			// while keeping track of the unique edges that are removed
			
			std::vector<std::pair<size_t, size_t>> uniqueEdges;

			for (size_t i = 0; i < normalsAndDistance.size(); i++)
			{
				vec3 normal = normalsAndDistance[i].first;

				// Check if the current face's normal is pointing in the same
				// direction as the support
				if (dot(normalsAndDistance[i].first, support) > dot(normalsAndDistance[i].first, polytope[faceIndices[i * 3]]))
				{
					size_t faceIndex = i * 3;
					auto faceIndexAsIterator = faceIndices.begin() + faceIndex;

					// Add the 3 edges of the face if they're unique, and remove any existing edges if a duplicate is found
					AddEdgeIfUnique(uniqueEdges, faceIndices, faceIndex,     faceIndex + 1);
					AddEdgeIfUnique(uniqueEdges, faceIndices, faceIndex + 1, faceIndex + 2);
					AddEdgeIfUnique(uniqueEdges, faceIndices, faceIndex + 2, faceIndex    );

					// Remove the face and its normal and distance

					faceIndices.erase(faceIndexAsIterator, faceIndexAsIterator + 2);
					normalsAndDistance.erase(normalsAndDistance.begin() + i);

					i--;
				}
			}

			// Get the new faces required to repair the polytope

			std::vector<size_t> newFaceIndices;

			// Add a new face based on each unique edge 
			for (std::pair<size_t, size_t> edge : uniqueEdges)
			{
				newFaceIndices.push_back(edge.first);
				newFaceIndices.push_back(edge.second);
				newFaceIndices.push_back(polytope.size());
			}

			// Add the support point
			polytope.push_back(support);
			supportA.push_back(newSupportA);
			supportB.push_back(newSupportB);

			// We only need the normals for the new faces, so we only calculate those to save a bit of CPU time
			std::vector<std::pair<vec3, float>> newNormalsAndDistance = GetFaceNormalsAndDistance(polytope, newFaceIndices);

			// We also only changed the new faces, so we only need to calculate the new closest face based on the new faces
			size_t newClosestFace = GetClosestFace(newNormalsAndDistance).first;

			// Get the old closest distance, as it was overwritten by previous code
			float oldClosestDistance = GetClosestFace(normalsAndDistance).second;
			
			// If the new face is closer to the origin than oldClosestDistance, set the closest face to the new face,
			// correcting for the fact that the new face will be added to the end of the normalsAndDistance vector
			if (newNormalsAndDistance[newClosestFace].second < oldClosestDistance)
				closestFace = newClosestFace + normalsAndDistance.size();

			faceIndices.insert(faceIndices.end(), newFaceIndices.begin(), newFaceIndices.end());
			normalsAndDistance.insert(normalsAndDistance.end(), newNormalsAndDistance.begin(), newNormalsAndDistance.end());
		}

#ifndef NDEBUG
		Logger::getInstance().log(std::to_string(closestDistance));
		Logger::getInstance().log("");
#endif

		iterations--;
	}

	if (iterations <= 0)
	{
		Logger::getInstance().warn(
			"EPA iteration limit reached! This is probably a result of trying to calculate collisions for very complex or invalid geometry! This will cause broken collisions!"
		);
	}

	epa_collision_info epa_info;
	 
	epa_info.contactNormal = closestNormal;

	// We add COLLISION_EPSILON to the depth to push objects slightly away from each other, to decrease the likelihood of
	// small collisions that don't affect the final result much and just waste CPU time
	epa_info.collisionDepth = closestDistance + COLLISION_EPSILON;

	vec3 closestTriA = polytope[faceIndices[(closestFace * 3)]];
	vec3 closestTriB = polytope[faceIndices[(closestFace * 3) + 1]];
	vec3 closestTriC = polytope[faceIndices[(closestFace * 3) + 2]];

	vec3 closestPoint = projectPointOntoTri(closestTriA, closestTriB, closestTriC, vec3(0, 0, 0));

	vec3 closestPointBarycentric = barycentricTriangle(closestTriA, closestTriB, closestTriC, closestPoint);

	// The contact points are a linear combination of the support points using the closest point in barycentric
	// coordinates relative to the closest triangle

	epa_info.contactPointA =
		(supportA[faceIndices[(closestFace * 3)]] * closestPointBarycentric.x) +
		(supportA[faceIndices[(closestFace * 3) + 1]] * closestPointBarycentric.y) +
		(supportA[faceIndices[(closestFace * 3) + 2]] * closestPointBarycentric.z);

	epa_info.contactPointB =
		(supportB[faceIndices[(closestFace * 3)]] * closestPointBarycentric.x) +
		(supportB[faceIndices[(closestFace * 3) + 1]] * closestPointBarycentric.y) +
		(supportB[faceIndices[(closestFace * 3) + 2]] * closestPointBarycentric.z);

	// Convert contact points to world space
	
	// We already rotated the simplex/polytope in GJK and in this function, so we only need to translate the contact points
	epa_info.contactPointA -= blockAPos;
	epa_info.contactPointB -= blockBPos;

	return epa_info;
}