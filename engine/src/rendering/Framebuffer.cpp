#include "rendering/Framebuffer.hpp"

#include "rendering/Debugging.hpp"
#include "utilities/StdUtilities.hpp"


#include <glad/glad.h>
#include <iostream>

namespace Engine::Rendering
{
Framebuffer* Framebuffer::Default;

typedef Framebuffer::Framebufferbuilder Framebufferbuilder;

void Framebuffer::CreateFBO(std::vector<std::string> colorbuffers, std::vector<std::string> depthbuffers, std::vector<Format> formats)
{
    uint32_t cbCount = uint32_t(colorbuffers.size());
    uint32_t dbCount = uint32_t(depthbuffers.size());
    BufferHandle* cbHandles = new BufferHandle[cbCount];
    BufferHandle* dbHandles = new BufferHandle[dbCount];
    uint32_t* attachments = new uint32_t[cbCount];
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    // Colorbuffers
    glGenTextures(cbCount, cbHandles);
    //If framebuffer is being rebuild, just update the textures' ids
    if(m_colorNames.size() > 0)
    {
        for(uint32_t cbi = 0; cbi < cbCount; cbi++)
        {
            m_colorTextures[cbi]->m_id = cbHandles[cbi];
        }
    }
    //Else make new textures etc.
    else
    {
        for(uint32_t cbi = 0; cbi < cbCount; cbi++)
        {
            m_colorNames.push_back(colorbuffers[cbi]);
            m_colorTextures.push_back(new Texture(GL_TEXTURE_2D, cbHandles[cbi]));
        }
    }
    //Create texture objects
    for(uint32_t cbi = 0; cbi < cbCount; cbi++)
    {
        glBindTexture(GL_TEXTURE_2D, cbHandles[cbi]);
        attachments[cbi] = GL_COLOR_ATTACHMENT0+cbi;
        glTexImage2D(GL_TEXTURE_2D, 0, formats[cbi], m_width, m_height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[cbi], GL_TEXTURE_2D, cbHandles[cbi], 0);
    }
    //Set as draw buffers 
    glDrawBuffers(cbCount, attachments);
    // Depthbuffer
    //If rebuilding
    if(dbCount > 0)
    {
        if(m_depthNames.size() > 0)
        {
            m_depthTextures[0]->m_id = dbHandles[0];
        }
        //Else make new textures etc.
        else
        {
            m_depthNames.push_back(depthbuffers[0]);
            m_depthTextures.push_back(new Texture(GL_TEXTURE_2D, dbHandles[0]));
        }
        glGenRenderbuffers(1, dbHandles);
        glBindRenderbuffer(GL_RENDERBUFFER, dbHandles[0]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dbHandles[0]);
    }

    UPDATE_CALLINFO2("Creating framebuffer object.")
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) throw std::exception("Framebuffer not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    delete[] cbHandles;
    delete[] dbHandles;
    delete[] attachments;
}

Framebuffer::Framebuffer()
{
    m_fbo = 0;
}

Framebuffer::Framebuffer(uint32_t w, uint32_t h, std::vector<std::string> cbs, std::vector<std::string> dbs, std::vector<Format> formats)
{
    m_width = w;
    m_height = h;
    m_formats = formats;
    CreateFBO(cbs, dbs, formats);
}

void Framebuffer::DeleteBuffers()
{
    //TODO: Manage the texture objects! Currently not deleting them and will leak memory. Maybe make a texture pool that can allocate and delete them.
    glDeleteFramebuffers(1, &m_fbo);
    for(auto tex : m_colorTextures)
    {
        BufferHandle id = tex->GetID();
        glDeleteTextures(1, &id);
    }
    for(auto tex : m_depthTextures)
    {
        BufferHandle id = tex->GetID();
        glDeleteTextures(1, &id);
    }
}

Framebuffer* Framebuffer::GetDefault()
{
    if(Default == nullptr) Default = new Framebuffer();
    return Default;
}

Framebuffer::~Framebuffer()
{
    DeleteBuffers();
}

void Framebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void Framebuffer::BindDefault()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Clear(uint32_t mask)
{
    glClear(mask == 0 ? (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) : mask);
}

uint32_t Framebuffer::GetFBO()
{
    return m_fbo;
}

Texture* Framebuffer::GetColorbuffer(BufferName name)
{
    int index = Utilities::IndexOf(m_colorNames, name);
    if(index >= 0) return m_colorTextures[index];
    return nullptr;
}

Texture* Framebuffer::GetDepthBuffer(BufferName name)
{
    int index = Utilities::IndexOf(m_depthNames, name);
    if(index >= 0) return m_depthTextures[index];
    return nullptr;
}

void Framebuffer::Rebuild(Size width, Size height)
{
    DeleteBuffers();
    m_width = width;
    m_height = height;
    CreateFBO(m_colorNames, m_depthNames, m_formats);
}

Framebufferbuilder::Framebufferbuilder(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;
}

Framebufferbuilder& Framebufferbuilder::WithColorbuffer(std::string name, Format format)
{
    m_colorbuffers.push_back(name);
    m_formats.push_back(format);
    return *this;
}

Framebufferbuilder& Framebufferbuilder::WithDepthbuffer(std::string name)
{
    if(m_depthbuffers.size() > 0) return *this;
    m_depthbuffers.push_back(name);
    return *this;
}

Framebuffer* Framebufferbuilder::Build()
{
    return new Framebuffer(m_width, m_height, m_colorbuffers, m_depthbuffers, m_formats);
}

Framebufferbuilder Framebuffer::Create(uint32_t width, uint32_t height)
{
    return Framebufferbuilder(width, height);
}

};
