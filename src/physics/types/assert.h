#pragma once
#include <string>
#include <sstream>
#include <immintrin.h>
#include "core/log.h"
#include "core/globals.h"

#ifndef NDEBUG
#define assert(condition, message) \
    do { \
        _Pragma("warning(push, 0)"); \
        if (!(condition)) { \
            std::stringstream output; \
            output << "Assertion `" #condition "` failed in " << __FILE__ << " line " << __LINE__ << ": " << message; \
            logger.err(output.str()); \
            logger.dumpLog(); \
            /*__debugbreak();*/ \
            exit(EXIT_ASSERT_FAIL); \
        } \
        _Pragma("warning(pop)"); \
    } while(false)
#else
#define assert(condition, message) do {} while(false)
#endif