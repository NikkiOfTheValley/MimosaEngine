#pragma once

#include <memory>
#include <deque>
#include <thread>
#include <mutex>
#include <optional>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cstdint>

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

#include "../../core/globals.h"
