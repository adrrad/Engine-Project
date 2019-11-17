#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>

namespace Rendering
{
    
class WindowManager
{

private:
    std::vector<GLFWwindow*> _windows;
    static WindowManager* _instance;
    void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    
    WindowManager();
public:
    static WindowManager* GetInstance();
    

    uint32_t CreateWindow(std::string title, uint32_t width, uint32_t height);
    bool WindowShouldClose(uint32_t window);
    void SwapBuffers(uint32_t window);
    void PollEvents();
};

} // namespace Rendering
