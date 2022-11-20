#pragma once

#ifdef __linux__
#define VK_USE_PLATFORM_XCB_KHR
#elif defined _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>