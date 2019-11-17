#include "renderer/WindowManager.hpp"

#include <iostream>


namespace Rendering
{
WindowManager* WindowManager::_instance;

WindowManager* WindowManager::GetInstance()
{
    if(_instance == nullptr) _instance = new WindowManager();
    return _instance;
}

WindowManager::WindowManager()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

uint32_t WindowManager::CreateWindow(std::string title, uint32_t width, uint32_t height)
{
    
    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr)
    {
        throw std::exception("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);

    auto callback = [](GLFWwindow* window, int width, int height)
    {
        WindowManager::GetInstance()->FramebufferSizeCallback(window, width, height);
    };

    glfwSetFramebufferSizeCallback(window, callback);
    _windows.push_back(window);
    return uint32_t(_windows.size()-1);
}

bool WindowManager::WindowShouldClose(uint32_t window)
{
    return glfwWindowShouldClose(_windows[window]);
}

void WindowManager::SwapBuffers(uint32_t window)
{
    glfwSwapBuffers(_windows[window]);
}

void WindowManager::PollEvents()
{
    glfwPollEvents();
}


void WindowManager::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{

}
    
} // namespace Rendering
