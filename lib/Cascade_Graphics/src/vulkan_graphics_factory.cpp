#include "vulkan_graphics_factory.hpp"

#include "vulkan_graphics.hpp"
#include "vulkan_renderer.hpp"

namespace Cascade_Graphics
{
	Vulkan_Graphics_Factory::Vulkan_Graphics_Factory(Platform platform) :
		Graphics_Factory::Graphics_Factory(platform)
	{
	}

	Graphics* Vulkan_Graphics_Factory::Create_Graphics(Platform_Info* platform_info) const
	{
		return new Vulkan_Graphics(m_platform, platform_info);
	}

	Renderer* Vulkan_Graphics_Factory::Create_Renderer(Graphics* graphics_ptr, Window_Info* window_info_ptr) const
	{
		return new Vulkan_Renderer(graphics_ptr, m_platform, window_info_ptr);
	}
}	 // namespace Cascade_Graphics