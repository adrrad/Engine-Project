#include "renderer/Framebuffer.hpp"

#include <glad/glad.h>
#include <iostream>

namespace Rendering
{
Framebuffer* Framebuffer::Default;

typedef Framebuffer::Framebufferbuilder Framebufferbuilder;


Framebufferbuilder::Framebufferbuilder(uint32_t width, uint32_t height)
{
    _width = width;
    _height = height;
}

Framebufferbuilder& Framebufferbuilder::WithColorbuffer(std::string name, Format format)
{
    _colorbuffers.push_back(name);
    _formats.push_back(format);
    return *this;
}

Framebufferbuilder& Framebufferbuilder::WithDepthbuffer(std::string name)
{
    if(_depthbuffers.size() > 0) return *this;
    _depthbuffers.push_back(name);
    return *this;
}

Framebuffer* Framebufferbuilder::Build()
{
    return new Framebuffer(_width, _height, _colorbuffers, _depthbuffers, _formats);
}

Framebufferbuilder Framebuffer::Create(uint32_t width, uint32_t height)
{
    return Framebufferbuilder(width, height);
}

void Framebuffer::CreateFBO(std::vector<std::string> colorbuffers, std::vector<std::string> depthbuffers, std::vector<Format> formats)
{
    uint32_t cbCount = uint32_t(colorbuffers.size());
    uint32_t dbCount = uint32_t(depthbuffers.size());
    BufferHandle* cbHandles = new BufferHandle[cbCount];
    BufferHandle* dbHandles = new BufferHandle[dbCount];
    uint32_t* attachments = new uint32_t[cbCount];
    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    // Colorbuffers
    glGenTextures(cbCount, cbHandles);
    for(uint32_t cbi = 0; cbi < cbCount; cbi++)
    {
        _colorbuffers.insert({colorbuffers[cbi], new Texture(GL_TEXTURE_2D, cbHandles[cbi])});
        glBindTexture(GL_TEXTURE_2D, cbHandles[cbi]);
        attachments[cbi] = GL_COLOR_ATTACHMENT0+cbi;
        glTexImage2D(GL_TEXTURE_2D, 0, formats[cbi], _width, _height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[cbi], GL_TEXTURE_2D, cbHandles[cbi], 0);
    }
    //Set as draw buffers 
    glDrawBuffers(cbCount, attachments);
    // Depthbuffer
    if(dbCount > 0)
    {
        // FIXME: Currently only 1 depth buffer is used.
        glGenRenderbuffers(1, dbHandles);
        _depthbuffers.insert({depthbuffers[0], new Texture(GL_TEXTURE_2D, dbHandles[0])});
        glBindRenderbuffer(GL_RENDERBUFFER, dbHandles[0]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dbHandles[0]);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) throw std::exception("Framebuffer not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    delete[] cbHandles;
    delete[] dbHandles;
    delete[] attachments;
}

Framebuffer::Framebuffer()
{
    _fbo = 0;
}

Framebuffer::Framebuffer(uint32_t w, uint32_t h, std::vector<std::string> cbs, std::vector<std::string> dbs, std::vector<Format> formats)
{
    _width = w;
    _height = h;
    _formats = formats;
    CreateFBO(cbs, dbs, formats);
}

Framebuffer* Framebuffer::GetDefault()
{
    if(Default == nullptr) Default = new Framebuffer();
    return Default;
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &_fbo);
    for(auto pair : _colorbuffers)
    {
        BufferHandle id = pair.second->GetID();
        glDeleteTextures(1, &id);
    }
    for(auto pair : _depthbuffers)
    {
        BufferHandle id = pair.second->GetID();
        glDeleteTextures(1, &id);
    }
}

void Framebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
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
    return _fbo;
}

Texture* Framebuffer::GetColorbuffer(BufferName name)
{
    return _colorbuffers[name];
}

Texture* Framebuffer::GetDepthBuffer(BufferName name)
{
    return _depthbuffers[name];
}

};
