#pragma once

/*
Handles large-scale parallel processing tasks using either OpenCL or CUDA (CUDA not implemented yet, because I don't have an NVIDIA graphics card).
Selects between them automatically upon calling the init() function.
*/
class ParallelProcessingManager
{
public:
	void init();
};