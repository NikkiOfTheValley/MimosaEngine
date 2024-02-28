#include "../sle_solver.h"
#include "../phys_state.h"

impulse SLESolver::SolveConstraints(
	PhysState* initialState,
	LargeVector<MAX_PHYS_OBJECTS * 3> objPosVector,
	LargeVector<MAX_PHYS_OBJECTS * 3> objRotVector,
	double fixedDeltaTime)
{

	impulse totalConstraintImpulse;


	for (auto& obj : initialState->objects)
	{
		// Skip objects that aren't initialized
		if (obj.index > MAX_PHYS_OBJECTS)
			continue;

		size_t index = obj.index;
		size_t startConstraintIndex = index * (MAX_CONSTRAINTS_PER_PHYS_OBJ - 1);
		size_t endConstraintIndex = startConstraintIndex + MAX_CONSTRAINTS_PER_PHYS_OBJ;

		for (size_t i = startConstraintIndex; i < endConstraintIndex; i++)
		{
			// Skip over constraints that aren't initalized
			if (initialState->objConstraints[i] == nullptr)
				continue;

			Constraint* constraint = initialState->objConstraints[i];

			constraint->Calculate(initialState, objPosVector, objRotVector, fixedDeltaTime);

			std::unique_ptr<LargeMatrix<4, 6>> constraintJacobianTransposed = constraint->jacobian.transpose();

			




		}

	}

	return {};
}