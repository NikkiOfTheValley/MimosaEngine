#pragma once
#include <vector>
#include <unordered_map>
#include "phys_obj.h"
#include "sle_solver.h"
#include "ode_solver.h"

struct PhysState
{
	std::vector<PhysObj> objects;
	std::unordered_map<std::string, size_t> nameToObjIndex;
	
	SLESolver sleSolver;
	ODESolver odeSolver;
};