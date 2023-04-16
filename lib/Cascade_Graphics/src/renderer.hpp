#pragma once

namespace Cascade_Graphics
{
    class Renderer
    {
    public:
        virtual ~Renderer();

        virtual void Render_Frame() = 0;
    };
}    // namespace Cascade_Graphics