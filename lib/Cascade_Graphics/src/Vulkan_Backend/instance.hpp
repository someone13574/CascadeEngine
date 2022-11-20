#pragma once

#include "vulkan_header.hpp"

namespace Cascade_Graphics
{
    namespace Vulkan
    {
        class Instance
        {
            friend class Instance_Builder;

        private:
            VkInstance m_instance;

        private:
            Instance();

        public:
            ~Instance();

            VkInstance* Get();
        };
    } // namespace Vulkan
} // namespace Cascade_Graphics