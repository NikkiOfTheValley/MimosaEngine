#include <thread>
#include <chrono>
#include "../utils.h"

bool RetryAndWait(std::function<bool()> func, size_t numTries, std::chrono::duration<float, std::milli> time)
{
    using namespace std::this_thread;
    using namespace std::chrono_literals;
    using std::chrono::steady_clock;

    for (size_t i = 0; i < numTries; i++)
    {
        if (!func())
        {
            sleep_for(time);
        }
        else
        {
            return true;
        }
    }
    return false;
}