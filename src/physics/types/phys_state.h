#pragma once
#include <array>
#include <unordered_map>
#include "phys_obj.h"
#include "sle_solver.h"
#include "ode_solver.h"
#include "mlcp_solver.h"
#include "large_sparse_matrix.h"

struct PhysState
{
	// Make sure that no I/O can happen with other threads until the step has been completed
	bool isInStep = false;

	// Make sure to keep track of the current object index, so objects can be added sequentially without overwriting each other
	size_t objIndex = 0;
	std::array<PhysObj, MAX_PHYS_OBJECTS> objects;
	std::array<Constraint*, MAX_PHYS_OBJECTS * MAX_CONSTRAINTS_PER_PHYS_OBJ> objConstraints;

	std::unordered_map<std::string, size_t> nameToObjIndex;

	// `n` is the maximum number of objects in the scene

	// - ODE and SLE solver data -

	// A 6n by 6n diagonal matrix
	// Example matrix where `n` is 2 and all object "slots" are in use:
	// 
	// {{m1, 0,  0,  0,    0,    0,    0,  0,  0,  0,    0,    0},
	//  {0,  m1, 0,  0,    0,    0,    0,  0,  0,  0,    0,    0},
	//  {0,  0,  m1, 0,    0,    0,    0,  0,  0,  0,    0,    0},
	//  {0,  0,  0,  I1_0, I1_1, I1_2, 0,  0,  0,  0,    0,    0},
	//  {0,  0,  0,  I1_3, I1_4, I1_5, 0,  0,  0,  0,    0,    0},
	//  {0,  0,  0,  I1_6, I1_7, I1_8, 0,  0,  0,  0,    0,    0},
	//  {0,  0,  0,  0,    0,    0,    m2, 0,  0,  0,    0,    0},
	//  {0,  0,  0,  0,    0,    0,    0,  m2, 0,  0,    0,    0},
	//  {0,  0,  0,  0,    0,    0,    0,  0,  m2, 0,    0,    0},
	//  {0,  0,  0,  0,    0,    0,    0,  0,  0,  I2_0, I2_1, I2_2},
	//  {0,  0,  0,  0,    0,    0,    0,  0,  0,  I2_3, I2_4, I2_5},
	//  {0,  0,  0,  0,    0,    0,    0,  0,  0,  I2_6, I2_7, I2_8}}
	// 
	// Where `m` is the mass of the `i`th rigid body,
	// and I is the inertia tensor of the `i`th rigid body.
	LargeSparseMatrix<MAX_PHYS_OBJECTS * 6, MAX_PHYS_OBJECTS * 6> objPropertiesMatrix;

	// A state vector of size 6n
	// Example vector where `n` is 2 and all object "slots" are in use:
	// {p1x, p1y, p1z, a1x, a1y, a1z, p2x, p2y, p2z, a2x, a2y, a2z}
	// Where `p` is the position of the `i`th rigid body,
	// and `a` is the rotation of the `i`th rigid body.
	LargeVector<MAX_PHYS_OBJECTS * 6> objStateVector;

	// A force vector of size 6n
	// Example vector where `n` is 2 and all object "slots" are in use:
	// {f1x, f1y, f1z, t1x, t1y, t1z, f2x, f2y, f2z, t2x, t2y, t2z}
	// Where `f` is the translational force acting on the `i`th rigid body,
	// and `t` is the torque acting on the `i`th rigid body.
	LargeVector<MAX_PHYS_OBJECTS * 6> objForceVector;

	// A acceleration vector of size 6n
	// Example vector where `n` is 2 and all object "slots" are in use:
	// {a1x, a1y, a1z, ra1x, ra1y, ra1z, a2x, a2y, a2z, ra2x, ra2y, ra2z}
	// Where `a` is the acceleration acting on the `i`th rigid body,
	// and `ra` is the angular acceleration acting on the `i`th rigid body.
	LargeVector<MAX_PHYS_OBJECTS * 6> objAccelVector;

	// A velocity vector of size 6n
	// Example vector where `n` is 2 and all object "slots" are in use:
	// {v1x, v1y, v1z, rv1x, rv1y, rv1z, v2x, v2y, v2z, rv2x, rv2y, rv2z}
	// Where `v` is the velocity acting on the `i`th rigid body,
	// and `rv` is the angular velocity acting on the `i`th rigid body.
	LargeVector<MAX_PHYS_OBJECTS * 6> objVelVector;
};