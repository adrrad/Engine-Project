#include "renderer/Machine.hpp"

#include "renderer/Renderer.hpp"
#include "renderer/Debugging.hpp"

#include <glad/glad.h>
#include <iostream>

using namespace std;

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
            UPDATE_CALLINFO();
            Variable vao = queue->NextVariable();
            glBindVertexArray(vao);
            break;
        }
        case MachineCode::DRAW_ELEMENTS:
        {
            UPDATE_CALLINFO();
            Variable topology = queue->NextVariable();
            Variable elementcount = queue->NextVariable();
            glDrawElements(topology, elementcount, GL_UNSIGNED_INT, 0);
            break;
        }
        case MachineCode::BIND_FRAMEBUFFER:
        {
            UPDATE_CALLINFO();
            Variable fbo = queue->NextVariable();
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //TODO: Should the user have control over clearing the buffers?
            break;
        }
        case MachineCode::USE_SHADER:
        {
            UPDATE_CALLINFO();
            Variable shaderID = queue->NextVariable();
            glUseProgram(shaderID);
            break;
        }
        case MachineCode::BIND_TEXTURE:
        {
            UPDATE_CALLINFO();
            Variable uniformID = queue->NextVariable();
            Variable activeID = queue->NextVariable();
            Variable textureID = queue->NextVariable();
            Variable textureTarget = queue->NextVariable();
            glActiveTexture(activeID);
            glBindTexture(textureTarget, textureID);
            glUniform1i(uniformID, activeID-GL_TEXTURE0);
            break;
        }
        case MachineCode::BIND_UNIFORM_BUFFER_RANGE:
        {
            UPDATE_CALLINFO();
            Variable binding = queue->NextVariable();
            Variable buffer = queue->NextVariable();
            Variable offset = queue->NextVariable();
            Variable size = queue->NextVariable();
            glBindBufferRange(GL_UNIFORM_BUFFER, binding, buffer, offset, size);
            break;
        }
        case MachineCode::ENABLE_DEPTHMASK:
        {
            UPDATE_CALLINFO();
            glDepthMask(GL_TRUE);
            break;
        }
        case MachineCode::DISABLE_DEPTHMASK:
        {
            UPDATE_CALLINFO();
            glDepthMask(GL_FALSE);
            break;
        }
        case MachineCode::CLEAR_DEPTH_BUFFER:
        {
            UPDATE_CALLINFO();
            glClear(GL_DEPTH_BUFFER_BIT);
            break;
        }
        case MachineCode::CLEAR_COLOR_BUFFER:
        {
            UPDATE_CALLINFO();
            glClear(GL_COLOR_BUFFER_BIT);
            break;
        }
        case MachineCode::ENABLE_BLENDING:
        {
            UPDATE_CALLINFO();
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        }
        case MachineCode::DISABLE_BLENDING:
        {
            UPDATE_CALLINFO();
            glDisable(GL_BLEND);
            break;
        }
        default:
            break;
        }
    }
    queue->ResetIndexCounters();
}

}