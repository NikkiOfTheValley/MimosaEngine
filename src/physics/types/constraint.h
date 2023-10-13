#pragma once

class PhysObj;

class Constraint
{
public:
	virtual void Apply(PhysObj* obj);
};