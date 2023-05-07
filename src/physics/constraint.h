#pragma once

class PhysObj;

class Constraint
{
	virtual void Apply(PhysObj* obj);
};