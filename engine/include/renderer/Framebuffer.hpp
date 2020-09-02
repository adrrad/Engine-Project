#pragma once

#include "renderer/Texture.hpp"
#include "renderer/RenderingTypedefs.hpp"

#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>

namespace Rendering
{

class Framebuffer
{
private:
    static Framebuffer* Default;
    FrameBufferHandle _fbo;
    uint32_t _width;
    uint32_t _height;
    std::vector<std::string> _colorNames;
    std::vector<std::string> _depthNames;
    std::vector<Texture*> _colorTextures;
    std::vector<Texture*> _depthTextures;
    std::vector<Format> _formats;

    Framebuffer();

    Framebuffer(uint32_t w, uint32_t h, std::vector<std::string> cbs, std::vector<std::string> dbs, std::vector<Format> formats);

    void CreateFBO(std::vector<std::string> colorbuffers, std::vector<std::string> depthbuffers, std::vector<Format> formats);

    /**
     * @brief Deletes the OpenGL buffers allocated by this object.
     * 
     */
    void DeleteBuffers();

public:
    static Framebuffer* GetDefault();

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
    Texture* GetColorbuffer(BufferName name);
    
    /**
     * @brief Get the Depth Buffer object handle.
     * 
     * @param name The name of the buffer.
     * 
     * @return uint32_t Handle to the depthbuffer object.
     */
    Texture* GetDepthBuffer(BufferName name);

    /**
     * @brief Rebuilds the Framebuffer with new size and the previously defined buffers.
     * 
     */
    void Rebuild(Size width, Size height);

    class Framebufferbuilder
    {
    friend class Framebuffer;
    private:
        std::vector<std::string> _colorbuffers;
        std::vector<std::string> _depthbuffers;
        std::vector<Format> _formats;
        uint32_t _width = 0;
        uint32_t _height = 0;

        Framebufferbuilder(uint32_t width, uint32_t height);
    public:

        /**
         * @brief Adds a color buffer to the framebuffer.
         * 
         * @param name The reference name of the color bufffer.
         * @param format The format of the color buffer.
         * @return Framebufferbuilder& the updated builder object.
         */
        Framebufferbuilder& WithColorbuffer(std::string name, Format format);

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
     * @param width The width of the framebuffer.
     * @param height The height of the framebuffer.
     * @return Framebufferbuilder - builder object allowing to customise the constructed framebuffer.
     */
    static Framebufferbuilder Create(uint32_t width, uint32_t height);
};

}