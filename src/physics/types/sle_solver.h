#pragma once
#include "../phys_globals.h"
#include "../core/globals.h"
#include "large_matrix.h"
#include "large_vector.h"

struct PhysState;

// A very basic SLE solver used for solving equality constraints
class SLESolver
{
public:
	// Solves constaints on a given physics state
	void Solve(PhysState* state);
};