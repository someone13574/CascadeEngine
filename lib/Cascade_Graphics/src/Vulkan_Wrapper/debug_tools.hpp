#pragma once

#include "cascade_logging.hpp"

#include "vulkan_header.hpp"

const char* VkResult_To_Message(VkResult vkresult);

#define VALIDATE_VKRESULT(vkresult, error_message)                                                                              \
    if (vkresult != VK_SUCCESS && (unsigned int)vkresult != 0)                                                                  \
    {                                                                                                                           \
        LOG_FATAL << error_message << " - failed with VkResult '" << VkResult_To_Message(vkresult) << "' (" << vkresult << ")"; \
        exit(EXIT_FAILURE);                                                                                                     \
    }
