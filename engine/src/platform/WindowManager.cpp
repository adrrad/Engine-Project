#include "platform/WindowManager.hpp"

#include <iostream>


namespace Engine::Platform
{
WindowManager* WindowManager::m_instance;

WindowManager* WindowManager::GetInstance()
{
    if(m_instance == nullptr) m_instance = new WindowManager();
    return m_instance;
}

void WindowManager::SetupInputCallbacks(uint32_t window)
{
    glfwSetCursorPosCallback(m_windows[m_activeWindow], [](GLFWwindow *, double x, double y) 
    {
        static glm::vec2 oldPos = glm::vec2(0,0);
        WindowManager* winMan = WindowManager::GetInstance();
        glm::vec2 winSize = winMan->GetWindowSize(winMan->m_activeWindow);

        float dx = oldPos.x - x;
        float dy = oldPos.y - y;
        oldPos.x = float(x);
        oldPos.y = float(y);
        dx /= winSize.x;
        dy /= winSize.y;
        for(auto callback : winMan->m_mpCallbacks)
        {
            callback(dx, dy);
        }
    });

    glfwSetKeyCallback(m_windows[window], [](GLFWwindow *, int key, int, int action, int)
    {
        WindowManager* winMan = WindowManager::GetInstance();
        for(auto callback : winMan->m_keyCallbacks)
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

uint32_t WindowManager::CreateWindow(std::string title, uint32_t width, uint32_t height, bool maximize)
{
    
    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr)
    {
        throw std::exception("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);
    if(maximize) glfwMaximizeWindow(window);

    auto callback = [](GLFWwindow* window, int width, int height)
    {
        WindowManager::GetInstance()->FramebufferSizeCallback(window, width, height);
    };

    glfwSetFramebufferSizeCallback(window, callback);
    m_windows.push_back(window);
    uint32_t windowHanle = uint32_t(m_windows.size()-1);
    SetupInputCallbacks(windowHanle);
    return windowHanle;
}

glm::ivec2 WindowManager::GetWindowSize(uint32_t window)
{
    glm::ivec2 size;
    glfwGetWindowSize(m_windows[window], &size.x, &size.y);
    return size;
}

glm::ivec2 WindowManager::GetFrameBufferSize(uint32_t window)
{
    glm::ivec2 size;
    glfwGetFramebufferSize(m_windows[window], &size.x, &size.y);
    return size;
}

void WindowManager::SetActivewindow(uint32_t window)
{
    if(window > m_windows.size()-1) throw std::exception("Wrong window handle!");
    m_activeWindow = window;
}

uint32_t WindowManager::GetActiveWindow()
{
    return m_activeWindow;
}

GLFWwindow* WindowManager::GetGLFWWindow(uint32_t windowHandle)
{
    return m_windows[windowHandle];
}

void WindowManager::MaximizeWindow(uint32_t windowHandle)
{
    glfwMaximizeWindow(m_windows[windowHandle]);
}

bool WindowManager::WindowShouldClose(uint32_t window)
{
    return glfwWindowShouldClose(m_windows[window]);
}

void WindowManager::SwapBuffers(uint32_t window)
{
    glfwSwapBuffers(m_windows[window]);
}

void WindowManager::PollEvents()
{
    glfwPollEvents();
}


void WindowManager::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    for(auto resizeCallback : m_winResizeCallbacks)
    {
        resizeCallback(width, height);
    }
}

void WindowManager::LockCursor(uint32_t window)
{
    glfwSetInputMode(m_windows[window], GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetInputMode(m_windows[window], GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void WindowManager::UnlockCursor(uint32_t window)
{
    glfwSetInputMode(m_windows[window], GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void WindowManager::RegisterMousePositionCallback(MousePositionCallback cb)
{
    m_mpCallbacks.push_back(cb);
}

void WindowManager::RegisterKeyCallback(KeyCallback cb)
{
    m_keyCallbacks.push_back(cb);
}

void WindowManager::RegisterWindowResizeCallback(WindowResizeCallback cb)
{
    m_winResizeCallbacks.push_back(cb);
}
    
} // namespace Engine::Rendering
