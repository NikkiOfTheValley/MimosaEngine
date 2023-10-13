#pragma once

// Duration has an accuracy of 100 nanoseconds. Warning, uses 100% of a core while blocking!
void BlockForNanoseconds(long long duration);