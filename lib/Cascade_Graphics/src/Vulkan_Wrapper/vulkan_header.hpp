#pragma once

#if defined __linux__
#define VK_USE_PLATFORM_XCB_KHR
#elif defined _WIN32 || defined WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>