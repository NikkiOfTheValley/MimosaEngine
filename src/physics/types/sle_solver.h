#pragma once
#include "../core/globals.h"
#include "large_matrix.h"
#include "large_vector.h"

// A very basic SLE solver used for solving constraints
class SLESolver
{
	// `n` is the maximum number of objects in the scene

	// A 3n by 3n diagonal matrix
	// Example matrix where `n` is 2 and all object "slots" are in use:
	// {{m1, 0, 0, 0, 0, 0},
	//  {0, m1, 0, 0, 0, 0},
	//  {0, 0, I1, 0, 0, 0},
	//  {0, 0, 0, m2, 0, 0},
	//  {0, 0, 0, 0, m2, 0},
	//  {0, 0, 0, 0, 0, I2}}
	// Where `m` is the mass of the `i`th rigid body,
	// and I is the moment of inertia of the `i`th rigid body.
	LargeMatrix<maxPhysObjects * 3, maxPhysObjects * 3> objPropertiesMatrix;

	// A state vector of size 6n
	// Example vector where `n` is 2 and all object "slots" are in use:
	// {p1x, p1y, p1z, a1x, a1y, a1z, p2x, p2y, p2z, a2x, a2y, a2z}
	// Where `p` is the position of the `i`th rigid body,
	// and `a` is the rotation of the `i`th rigid body.
	LargeVector<maxPhysObjects * 6> objStateVector;

	// A force vector of size 6n
	// Example vector where `n` is 2 and all object "slots" are in use:
	// {f1x, f1y, f1z, t1x, t1y, t1z, f2x, f2y, f2z, t2x, t2y, t2z}
	// Where `f` is the translational force acting on the `i`th rigid body,
	// and `t` is the torque acting on the `i`th rigid body.
	LargeVector<maxPhysObjects * 6> objForceVector;
};