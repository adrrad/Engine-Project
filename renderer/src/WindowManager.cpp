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

void WindowManager::SetupInputCallbacks(uint32_t window)
{
    glfwSetCursorPosCallback(_windows[_activeWindow], [](GLFWwindow *, double x, double y) 
    {
        static glm::vec2 oldPos = glm::vec2(0,0);
        WindowManager* winMan = WindowManager::GetInstance();
        glm::vec2 winSize = winMan->GetWindowSize(winMan->_activeWindow);

        float dx = oldPos.x - x;
        float dy = oldPos.y - y;
        oldPos.x = float(x);
        oldPos.y = float(y);
        dx /= winSize.x;
        dy /= winSize.y;
        for(auto callback : winMan->_mpCallbacks)
        {
            callback(dx, dy);
        }
    });

    glfwSetKeyCallback(_windows[window], [](GLFWwindow *, int key, int, int action, int)
    {
        WindowManager* winMan = WindowManager::GetInstance();
        for(auto callback : winMan->_keyCallbacks)
        {
            callback(key, action);
        }
    });
}

WindowManager::WindowManager()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
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
    uint32_t windowHanle = uint32_t(_windows.size()-1);
    SetupInputCallbacks(windowHanle);
    return windowHanle;
}

glm::ivec2 WindowManager::GetWindowSize(uint32_t window)
{
    glm::ivec2 size;
    glfwGetWindowSize(_windows[window], &size.x, &size.y);
    return size;
}

void WindowManager::SetActivewindow(uint32_t window)
{
    if(window > _windows.size()-1) throw std::exception("Wrong window handle!");
    _activeWindow = window;
}

uint32_t WindowManager::GetActiveWindow()
{
    return _activeWindow;
}

GLFWwindow* WindowManager::GetGLFWWindow(uint32_t windowHandle)
{
    return _windows[windowHandle];
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

void WindowManager::LockCursor(uint32_t window)
{
    glfwSetInputMode(_windows[window], GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetInputMode(_windows[window], GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void WindowManager::UnlockCursor(uint32_t window)
{
    glfwSetInputMode(_windows[window], GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void WindowManager::RegisterMousePositionCallback(MousePositionCallback cb)
{
    _mpCallbacks.push_back(cb);
}

void WindowManager::RegisterKeyCallback(KeyCallback cb)
{
    _keyCallbacks.push_back(cb);
}
    
} // namespace Rendering
