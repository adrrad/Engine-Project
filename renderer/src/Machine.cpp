#include "renderer/Machine.hpp"

#include "renderer/Renderer.hpp"


#include <glad/glad.h>

namespace Rendering
{

Machine::Machine()
{
    _renderer = Renderer::GetInstance();
}

void Machine::Run(Renderqueue* queue)
{
    while(queue->HasNextInstruction())
    {
        switch (queue->NextInstruction())
        {
        case MachineCode::BIND_VAO:
        {
            Variable vao = queue->NextVariable();
            glBindVertexArray(vao);
            break;
        }
        case MachineCode::DRAW_ELEMENTS:
        {
            Variable top = queue->NextVariable();
            Variable elc = queue->NextVariable();
            glDrawElements(top, elc, GL_UNSIGNED_INT, 0);
            break;
        }
        case MachineCode::UPDATE_UNIFORMS:
        {
            Components::MeshComponent* mat = (Components::MeshComponent*)queue->NextVariable(); //TODO: any other way?
            _renderer->UpdateUniforms(mat);
            // mat->UpdateUniforms();
            break;
        }
        case MachineCode::BIND_FRAMEBUFFER:
        {
            Variable fbo = queue->NextVariable();
            glBindBuffer(GL_FRAMEBUFFER, fbo);
            break;
        }
        case MachineCode::ENABLE_DEPTHMASK:
        {
            glDepthMask(true);
            break;
        }
        case MachineCode::DISABLE_DEPTHMASK:
        {
            glDepthMask(false);
            break;
        }
        default:
            break;
        }
    }
    queue->ResetIndexCounters();
}

}