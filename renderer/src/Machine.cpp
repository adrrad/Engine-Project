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
            Variable topology = queue->NextVariable();
            Variable elementcount = queue->NextVariable();
            glDrawElements(topology, elementcount, GL_UNSIGNED_INT, 0);
            break;
        }
        case MachineCode::BIND_FRAMEBUFFER:
        {
            Variable fbo = queue->NextVariable();
            glBindBuffer(GL_FRAMEBUFFER, fbo);
            break;
        }
        case MachineCode::USE_SHADER:
        {
            Variable shaderID = queue->NextVariable();
            glUseProgram(shaderID);
            break;
        }
        case MachineCode::BIND_TEXTURE:
        {
            Variable uniformID = queue->NextVariable();
            Variable activeID = queue->NextVariable();
            Variable textureID = queue->NextVariable();
            Variable textureTarget = queue->NextVariable();
            glActiveTexture(activeID);
            glBindTexture(textureTarget, textureID);
            glUniform1i(uniformID, activeID-GL_TEXTURE0);
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