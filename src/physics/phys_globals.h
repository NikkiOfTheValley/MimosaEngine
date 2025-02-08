#pragma once
#include <cstddef>

const size_t MAX_PHYS_OBJECTS = 256;
const size_t MAX_CONSTRAINTS_PER_PHYS_OBJ = 32;
const unsigned int PHYS_FPS = 40000;
const float COLLISION_EPSILON = 0.001f;

#define ODE_SOLVER_TYPE_EULER
//#define ODE_SOLVER_TYPE_RK4

const bool WARN_ON_SIM_LAG = false;

const float DRAG_CONSTANT = 0.3f;

// Densities of various materials in kg/m^3
const float DENSITY_CAST_IRON = 7200;
const float DENSITY_STEEL = 7820;
const float DENSITY_ALUMINUM = 2700;
const float DENSITY_WOOD = 650;

// Yield strength of various materials in MPa
const float YIELD_STRENGTH_CAST_IRON = 451.5;
const float YIELD_STRENGTH_STEEL = 690;
const float YIELD_STRENGTH_ALUMINUM = 276;
const float YIELD_STRENGTH_WOOD = 25.5;

enum ConstraintType
{
	DEFUALT,
	COLLISION,
	IMPULSE
};