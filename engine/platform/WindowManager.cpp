#include "platform/WindowManager.hpp"

#include "core/EngineSettings.hpp"

#include <iostream>


namespace Platform
{

void WindowManager::SetupInputCallbacks(u32 window)
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

}

void WindowManager::Initialise()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
}

u32 WindowManager::CreateMainWindow()
{
    const Core::EngineSettings& settings = Core::EngineSettings::Get();
    return CreateWindow(settings.Project.ProjectName, settings.Window.Width, settings.Window.Height, settings.Window.Maximized);
}

u32 WindowManager::CreateWindow(std::string title, u32 width, u32 height, bool maximize)
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
    u32 windowHanle = u32(m_windows.size()-1);
    SetupInputCallbacks(windowHanle);
    return windowHanle;
}

glm::ivec2 WindowManager::GetWindowSize(u32 window)
{
    glm::ivec2 size;
    glfwGetWindowSize(m_windows[window], &size.x, &size.y);
    return size;
}

glm::ivec2 WindowManager::GetFrameBufferSize(u32 window)
{
    glm::ivec2 size;
    glfwGetFramebufferSize(m_windows[window], &size.x, &size.y);
    return size;
}

void WindowManager::SetActivewindow(u32 window)
{
    if(window > m_windows.size()-1) throw std::exception("Wrong window handle!");
    m_activeWindow = window;
}

u32 WindowManager::GetActiveWindow()
{
    return m_activeWindow;
}

GLFWwindow* WindowManager::GetGLFWWindow(u32 windowHandle)
{
    return m_windows[windowHandle];
}

void WindowManager::MaximizeWindow(u32 windowHandle)
{
    glfwMaximizeWindow(m_windows[windowHandle]);
}

bool WindowManager::WindowShouldClose(u32 window)
{
    return glfwWindowShouldClose(m_windows[window]);
}

void WindowManager::SwapBuffers(u32 window)
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

void WindowManager::LockCursor(u32 window)
{
    glfwSetInputMode(m_windows[window], GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetInputMode(m_windows[window], GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void WindowManager::UnlockCursor(u32 window)
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
    
} // namespace Rendering
