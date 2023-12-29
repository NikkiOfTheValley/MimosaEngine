#include "../constraint.h"

// Blank constraint, does nothing

void Constraint::Calculate(PhysState*, LargeVector<MAX_PHYS_OBJECTS * 3>, LargeVector<MAX_PHYS_OBJECTS * 3>, double) {}

ConstraintType Constraint::Type()
{
	return ConstraintType::DEFUALT;
}