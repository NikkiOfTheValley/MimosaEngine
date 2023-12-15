#pragma once

struct PhysState;

// Defines a given collision
struct collision
{
	bool colliding;

	vec3 collisionPoint;
	vec3 collisionNormal;
	float collisionDepth;

	PhysObj* collidingObjA;
	PhysObj* collidingObjB;
};

/*
Handles collisions in a given PhysState, and adds `CollisionConstraint`s for them.
Uses the GJK algorithm to detect collisions, and the EPA algorithm to get the collision information.
Based on https://blog.hamaluik.ca/posts/building-a-collision-engine-part-1-2d-gjk-collision-detection/ and
http://www.dyn4j.org/2010/05/epa-expanding-polytope-algorithm/.
*/
class CollisionHandler
{
public:
	void HandleCollisions(PhysState* state);

private:
	void GJK();

	void EPA();
};