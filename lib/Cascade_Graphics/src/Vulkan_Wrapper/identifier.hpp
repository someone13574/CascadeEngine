#pragma once

#include <string>

namespace Cascade_Graphics
{
    namespace Vulkan_Backend
    {
        struct Identifier
        {
            std::string label;
            uint32_t index;

            bool operator==(Identifier other_id)
            {
                return label == other_id.label && index == other_id.index;
            }

            std::string Get_Identifier_String()
            {
                return "'" + label + "-" + std::to_string(index) + "'";
            }
        };
    } // namespace Vulkan_Backend
} // namespace Cascade_Graphics