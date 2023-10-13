#include "accurate_timer.h"
#include <chrono>

// Duration has an accuracy of 100 nanoseconds. Warning, uses 100% of a core while waiting!
void BlockForNanoseconds(long long duration)
{
	using namespace std::chrono;

	auto startTime = high_resolution_clock::now();

	// Loop until the difference between the time now and the startTime exceeds the timer duration.
	// Yes, I know this is incredibly inefficent, but Windows timers (and std::this_thread::sleep_for) are insanely
	// imprecise.

	while (high_resolution_clock::now() - startTime < nanoseconds(duration))
	{

	}
}
