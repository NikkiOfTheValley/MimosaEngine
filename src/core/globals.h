#pragma once
#include "log.h"

struct vec2;

//#define NO_SIMD

// The CPU must be x64 for SIMD to work, so we need it as a flag
const uint16_t HW_x64 = 1;

// SIMD: 128-bit support
const uint16_t HW_SSE = 2;
const uint16_t HW_SSE2 = 4;
const uint16_t HW_SSE3 = 8;

// SIMD: 256-bit support
const uint16_t HW_AVX = 16;
const uint16_t HW_AVX2 = 32;
const uint16_t HW_FMA3 = 64;
const uint16_t HW_FMA4 = 128;

// SIMD: 512-bit support
const uint16_t HW_AVX512F = 256; // AVX512 Foundation
const uint16_t HW_AVX512CD = 512; // AVX512 Conflict Detection
const uint16_t HW_AVX512PF = 1024; // AVX512 Prefetch
const uint16_t HW_AVX512ER = 2048; // AVX512 Exponential + Reciprocal
const uint16_t HW_AVX512VL = 4096; // AVX512 Vector Length Extensions
const uint16_t HW_AVX512BW = 8192; // AVX512 Byte + Word
const uint16_t HW_AVX512DQ = 16384; // AVX512 Doubleword + Quadword
const uint16_t HW_AVX512VBMI = 32768; // AVX512 Vector Byte Manipulation Instructions

extern uint16_t supportedFeatures;


extern vec2 mousePos;
extern bool lmbPressed;

extern bool inUI;
extern bool useTextInput;

extern std::string* currentlySelectedText;

const std::string ENG_VERSION_STR = "v0.3.0";
const std::string VERSION_STR = "v0.0.1";
const std::string NAME_STR = "Example Application";

const float epsilon = 0.005f;

const float clearColorR = 0.2f;
const float clearColorG = 0.3f;
const float clearColorB = 0.3f;