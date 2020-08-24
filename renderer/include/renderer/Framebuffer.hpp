#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>

namespace Rendering
{
typedef uint32_t BufferHandle;
typedef uint32_t FrameBufferHandle;
typedef std::string BufferName;

class Framebuffer
{
private:
    int32_t _format;
    FrameBufferHandle _fbo;
    uint32_t _width;
    uint32_t _height;
    std::unordered_map<BufferName, BufferHandle> _colorbuffers;
    std::unordered_map<BufferName, BufferHandle> _depthbuffers;

    Framebuffer(uint32_t f, uint32_t w, uint32_t h, std::vector<std::string> _cbs, std::vector<std::string> _dbs);

    void CreateFBO(std::vector<std::string> colorbuffers, std::vector<std::string> depthbuffers);

public:
    class Framebufferbuilder
    {
    friend class Framebuffer;
    private:
        int32_t _format = 0;
        std::vector<std::string> _colorbuffers;
        std::vector<std::string> _depthbuffers;
        uint32_t _width = 0;
        uint32_t _height = 0;

        Framebufferbuilder(int format, uint32_t width, uint32_t height);
    public:

        /**
         * @brief Adds a color buffer to the framebuffer.
         * 
         * @param name The reference name of the color bufffer.
         * @return Framebufferbuilder& the updated builder object.
         */
        Framebufferbuilder& WithColorbuffer(std::string name);

        /**
         * @brief Adds a depth buffer to the framebuffer. (WARNING: CURRENTLY ONLY ONE ALLOWED)
         * 
         * @param name The reference name of the depth buffer.
         * 
         * @return Framebufferbuilder& the updated builder object.
         */
        Framebufferbuilder& WithDepthbuffer(std::string name);

        /**
         * @brief Constructs the Framebuffer object with the specified properties.
         * 
         * @return Framebuffer* - pointer to the constructed framebuffer object.
         */
        Framebuffer* Build();
    };

    /**
     * @brief Creates a builder object for a Framebuffer.
     * 
     * @param format The format of the color buffer.
     * @param width The width of the framebuffer.
     * @param height The height of the framebuffer.
     * @return Framebufferbuilder - builder object allowing to customise the constructed framebuffer.
     */
    static Framebufferbuilder Create(int format, uint32_t width, uint32_t height);


    ~Framebuffer();

    /**
     * @brief Binds the framebuffer as current.
     * 
     */
    void Bind();

    /**
     * @brief Binds the default framebuffer as current.
     * 
     */
    static void BindDefault();

    /**
     * @brief Clears the specified buffers on currently bound framebuffer.
     * 
     * @param mask Masks that indicate the buffers to be cleared.
     */
    static void Clear(uint32_t mask = 0);

    /**
     * @brief Get the framebuffer object OpenGL handle.
     * 
     * @return uint32_t Handle to the framebuffer object.
     */
    uint32_t GetFBO();
    
    /**
     * @brief Get the Colorbuffer object handle.
     * 
     * @param name The name of the buffer.
     * 
     * @return uint32_t Handle to the colorbuffer object.
     */
    BufferHandle GetColorbuffer(BufferName name);
    
    /**
     * @brief Get the Depth Buffer object handle.
     * 
     * @param name The name of the buffer.
     * 
     * @return uint32_t Handle to the depthbuffer object.
     */
    BufferHandle GetDepthBuffer(BufferName name);


};

}