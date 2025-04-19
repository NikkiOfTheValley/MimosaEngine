#include <functional>
#include <stddef.h>
#include <chrono>

// Utility header for networking stuff

/*!
 * @brief Attempts to run a given function a given number of times with a wait time in between
 * @param func The function to run
 * @param numTries The number of times to try
 * @param time The time to wait in between calls
 * @return Whether the function executed successfully or not
 */
bool RetryAndWait(std::function<bool()> func, size_t numTries, std::chrono::duration<float, std::milli> time);