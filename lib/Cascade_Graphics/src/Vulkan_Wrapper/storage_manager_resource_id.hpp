#pragma once

#include <string>

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        struct Resource_ID
        {
            std::string label;
            uint32_t index;

            enum Resource_Type
            {
                BUFFER_RESOURCE,
                IMAGE_RESOURCE
            } resource_type;

            bool operator==(Resource_ID other_id)
            {
                return index == other_id.index && label == other_id.label && resource_type == other_id.resource_type;
            }
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics