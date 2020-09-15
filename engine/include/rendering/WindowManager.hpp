#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <functional>




namespace Engine::Rendering
{

typedef std::function<void(int button, int action)> MouseButtonCallback;
typedef std::function<void(double xoffset, double yoffset)> MouseWheelCallback;
typedef std::function<void(double x, double y)> MousePositionCallback;
typedef std::function<void(int key, int action)> KeyCallback;
typedef std::function<void(unsigned int character)> CharCallback;
typedef std::function<void(int button, int action)> WindowResizeCallback;



class WindowManager
{

private:
    static WindowManager* _instance;
   
    std::vector<GLFWwindow*> _windows;

    uint32_t _activeWindow = 0;

    std::vector<MousePositionCallback> _mpCallbacks;

    std::vector<KeyCallback> _keyCallbacks;

    std::vector<WindowResizeCallback> _winResizeCallbacks;

    void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    
    void SetupInputCallbacks(uint32_t window);

    WindowManager();
public:
    static WindowManager* GetInstance();
    

    uint32_t CreateWindow(std::string title, uint32_t width, uint32_t height, bool maximize);

    glm::ivec2 GetWindowSize(uint32_t window);

    glm::ivec2 GetFrameBufferSize(uint32_t window);

    void SetActivewindow(uint32_t window);

    uint32_t GetActiveWindow();

    GLFWwindow* GetGLFWWindow(uint32_t windowHandle);

    void MaximizeWindow(uint32_t windowHandle);

    bool WindowShouldClose(uint32_t window);
    
    void SwapBuffers(uint32_t window);
    
    void PollEvents();

    void LockCursor(uint32_t window);

    void UnlockCursor(uint32_t window);

    void RegisterMousePositionCallback(MousePositionCallback cb);

    void RegisterKeyCallback(KeyCallback cb);

    void RegisterWindowResizeCallback(WindowResizeCallback cb);
};

} // namespace Engine::Rendering
