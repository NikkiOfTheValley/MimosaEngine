#pragma once

namespace conv
{
	double deg_to_rad(double deg);

	const float SEC_TO_NANOSEC = 1000000000.f;
	const float NANOSEC_TO_SEC = 1.f / 1000000000.f;

	const float SEC_TO_MICROSEC = 1000000.f;
	const float MICROSEC_TO_SEC = 1.f / 1000000.f;

	const float SEC_TO_MILLISEC = 1000.f;
	const float MILLISEC_TO_SEC = 1.f / 1000.f;
}