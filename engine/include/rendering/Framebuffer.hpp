#pragma once

#include "rendering/Texture.hpp"
#include "rendering/RenderingTypedefs.hpp"

#include <glm/glm.hpp>

#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>

namespace Engine::Rendering
{

class Framebuffer
{
private:
    static Framebuffer* Default;
    std::string m_name = "";
    FrameBufferHandle m_fbo;
    uint32_t m_x = 0;
    uint32_t m_y = 0;
    uint32_t m_width;
    uint32_t m_height;
    bool m_withDepthRenderBuffer;
    BufferHandle m_depthRenderBuffer = 0;
    std::vector<std::string> m_colorNames;
    std::vector<std::string> m_depthNames;
    std::vector<Texture*> m_colorTextures;
    std::vector<Texture*> m_depthTextures;
    std::vector<Format> m_formats;

    Framebuffer();

    Framebuffer(std::string name, uint32_t w, uint32_t h, std::vector<std::string> cbs, std::vector<std::string> dbs, std::vector<Format> formats, bool withDepthComponent);

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
     * @brief Get the name of the framebuffer.
     * 
     * @return std::string A string containing the frambuffer name.
     */
    std::string GetName();

    /**
     * @brief Get the framebuffer object OpenGL handle.
     * 
     * @return uint32_t Handle to the framebuffer object.
     */
    uint32_t GetFBO();

    /**
     * @brief Get the Color Buffer names.
     * 
     * @return std::vector<std::string*> A vector containing the color buffer names.
     */
    std::vector<std::string> GetColorBufferNames();

    /**
     * @brief Get the Depth Buffer names.
     * 
     * @return std::vector<std::string*> A vector containing the depth buffer names.
     */
    std::vector<std::string> GetDepthBufferNames();
    
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

    /**
     * @brief Get the Viewport position and dimensions as a 4-component vector.
     * 
     * @return glm::vec4 The viewport dimensions {x, y, w, h}.
     */
    inline glm::vec4 GetViewportDimensions() { return {m_x, m_y, m_width, m_height}; }

    class Framebufferbuilder
    {
    friend class Framebuffer;
    private:
        std::vector<std::string> m_colorbuffers;
        std::vector<std::string> m_depthbuffers;
        std::vector<Format> m_formats;
        bool m_withDepthComponent = false;
        uint32_t m_width = 0;
        uint32_t m_height = 0;
        std::string m_name = "";

        Framebufferbuilder(std::string name, uint32_t width, uint32_t height);
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
         * @brief Adds a depth renderbuffer to the framebuffer.
         * 
         * @return Framebufferbuilder& the updated builder object.
         */
        Framebufferbuilder& WithDepthRenderbuffer();

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
    static Framebufferbuilder Create(std::string name, uint32_t width, uint32_t height);
};

}