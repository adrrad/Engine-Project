#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "EngineComponent.hpp"
#include "EngineTypedefs.hpp"

#include <vector>
#include <string>
#include <functional>



namespace Platform
{
typedef std::function<void(int button, int action)> MouseButtonCallback;
typedef std::function<void(double xoffset, double yoffset)> MouseWheelCallback;
typedef std::function<void(double x, double y)> MousePositionCallback;
typedef std::function<void(int key, int action)> KeyCallback;
typedef std::function<void(unsigned int character)> CharCallback;
typedef std::function<void(int button, int action)> WindowResizeCallback;



class WindowManager : public EngineSubsystem<WindowManager>
{
friend class EngineSubsystem<WindowManager>;

private:
   
    std::vector<GLFWwindow*> m_windows;

    u32 m_activeWindow = 0;

    std::vector<MousePositionCallback> m_mpCallbacks;

    std::vector<KeyCallback> m_keyCallbacks;

    std::vector<WindowResizeCallback> m_winResizeCallbacks;

    void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    
    void SetupInputCallbacks(u32 window);

    WindowManager();
public:

    void Initialise() override;    

    u32 CreateMainWindow();

    u32 CreateWindow(std::string title, u32 width, u32 height, bool maximize);

    glm::ivec2 GetWindowSize(u32 window);

    glm::ivec2 GetFrameBufferSize(u32 window);

    void SetActivewindow(u32 window);

    u32 GetActiveWindow();

    GLFWwindow* GetGLFWWindow(u32 windowHandle);

    void MaximizeWindow(u32 windowHandle);

    bool WindowShouldClose(u32 window);
    
    void SwapBuffers(u32 window);
    
    void PollEvents();

    void LockCursor(u32 window);

    void UnlockCursor(u32 window);

    void RegisterMousePositionCallback(MousePositionCallback cb);

    void RegisterKeyCallback(KeyCallback cb);

    void RegisterWindowResizeCallback(WindowResizeCallback cb);
};

} // namespace Platform
