#pragma once

#include "cascade_logging.hpp"

#include "vulkan_header.hpp"

#define VALIDATE_VKRESULT(vkresult, error_message)                            \
    if (vkresult != VK_SUCCESS)                                               \
    {                                                                         \
        LOG_ERROR << error_message << " - failed with VkResult " << vkresult; \
        exit(EXIT_FAILURE);                                                   \
    }
