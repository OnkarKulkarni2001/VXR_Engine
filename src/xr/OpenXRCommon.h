#pragma once

// NOTE:
// We define XR_USE_* macros from CMake (recommended).
// This header only provides the correct include order for Win32 + Vulkan + OpenXR.

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
// Needed for LARGE_INTEGER and a bunch of Win32 typedefs used by openxr_platform.h
#include <windows.h>
// Needed for IUnknown (used by D3D-related OpenXR structs, still present in openxr_platform.h)
#include <unknwn.h>
#endif

// openxr_platform.h requires Vulkan types when XR_USE_GRAPHICS_API_VULKAN is enabled.
#include <vulkan/vulkan.h>

#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
