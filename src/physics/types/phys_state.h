#pragma once
#include <array>
#include <unordered_map>
#include "phys_obj.h"
#include "sle_solver.h"
#include "ode_solver.h"
#include "mlcp_solver.h"
#include "collision_handler.h"
#include "large_sparse_matrix.h"
#include "rendering/types/mat3x3.h"

struct PhysState
{
	// Make sure that no I/O can happen with other threads until the step has been completed
	bool isInStep = false;

	// Make sure to keep track of the current object index, so objects can be added sequentially without overwriting each other
	size_t objIndex = 0;
	std::array<PhysObj, MAX_PHYS_OBJECTS> objects;
	std::array<Constraint*, MAX_PHYS_OBJECTS * MAX_CONSTRAINTS_PER_PHYS_OBJ> objConstraints;
	std::array<mat3x3f, MAX_PHYS_OBJECTS> objInertiaMatrices;

	std::unordered_map<std::string, size_t> nameToObjIndex;

	// `n` is the maximum number of objects in the scene

	// - ODE and SLE solver data -


	// A mass vector of size 3n
	// Example matrix where `n` is 2 and all object "slots" are in use:
	// {m1, m1, m1, m2, m2, m2}
	// Where `m` is the mass of the `i`th rigid body.
	LargeVector<MAX_PHYS_OBJECTS * 3> objMass;


	// A position vector of size 3n
	// Example vector where `n` is 2 and all object "slots" are in use:
	// {p1x, p1y, p1z, p2x, p2y, p2z}
	// Where `p` is the position of the `i`th rigid body.
	LargeVector<MAX_PHYS_OBJECTS * 3> objPositionVector;

	// A rotation vector of size 3n
	// Example vector where `n` is 2 and all object "slots" are in use:
	// {r1x, r1y, r1z, r2x, r2y, r2z}
	// Where `r` is the rotation of the `i`th rigid body.
	LargeVector<MAX_PHYS_OBJECTS * 3> objRotationVector;

	// A velocity vector of size 3n
	// Example vector where `n` is 2 and all object "slots" are in use:
	// {v1x, v1y, v1z, v2x, v2y, v2z}
	// Where `v` is the velocity acting on the `i`th rigid body.
	LargeVector<MAX_PHYS_OBJECTS * 3> objVelVector;

	// A angular velocity vector of size 3n
	// Example vector where `n` is 2 and all object "slots" are in use:
	// {av1x, av1y, av1z, av2x, av2y, av2z}
	// Where `av` is the angular velocity acting on the `i`th rigid body.
	LargeVector<MAX_PHYS_OBJECTS * 3> objAngVelVector;

	// A acceleration vector of size 3n
	// Example vector where `n` is 2 and all object "slots" are in use:
	// {a1x, a1y, a1z, a2x, a2y, a2z}
	// Where `a` is the acceleration acting on the `i`th rigid body.
	LargeVector<MAX_PHYS_OBJECTS * 3> objAccelVector;

	// A velocity vector of size 3n
	// Example vector where `n` is 2 and all object "slots" are in use:
	// {aa1x, aa1y, aa1z, aa2x, aa2y, aa2z}
	// Where `aa` is the angular acceleration acting on the `i`th rigid body.
	LargeVector<MAX_PHYS_OBJECTS * 3> objAngAccelVector;

	// A force vector of size 3n
	// Example vector where `n` is 2 and all object "slots" are in use:
	// {f1x, f1y, f1z, f2x, f2y, f2z}
	// Where `f` is the translational force acting on the `i`th rigid body.
	LargeVector<MAX_PHYS_OBJECTS * 3> objForceVector;

	// A torque vector of size 3n
	// Example vector where `n` is 2 and all object "slots" are in use:
	// {t1x, t1y, t1z, t2x, t2y, t2z}
	// Where `t` is the torque acting on the `i`th rigid body.
	LargeVector<MAX_PHYS_OBJECTS * 3> objTorqueVector;

};