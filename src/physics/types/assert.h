#pragma once
#include <string>
#include <sstream>
#include <intrin.h>
#include "../core/log.h"
#include "../core/globals.h"

#ifndef NDEBUG
#define assert(condition, message) \
    do { \
        _Pragma("warning(push, 0)"); \
        if (!(condition)) { \
            std::stringstream output; \
            output << "Assertion `" #condition "` failed in " << __FILE__ << " line " << __LINE__ << ": " << message; \
            Logger::getInstance().err(output.str()); \
            Logger::getInstance().dumpLog(); \
            __debugbreak(); \
            exit(EXIT_ASSERT_FAIL); \
        } \
        _Pragma("warning(pop)"); \
    } while(false)
#else
#define assert(condition, message) do {} while(false)
#endif