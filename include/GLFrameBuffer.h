/*
 *  GLFrameBuffer.h
 *  
 *
 *  Created by Richard S. Wright Jr.
 *  rwright@starstonesoftware.com
 */
/* Copyright (c) 2005-2010, Richard S. Wright Jr.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list 
of conditions and the following disclaimer.

Neither the name of Richard S. Wright Jr. nor the names of other contributors may be used 
to endorse or promote products derived from this software without specific prior 
written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __GLT_FBO__
#define __GLT_FBO__

#include <GLTools.h>

#ifdef QT_IS_AVAILABLE
#include <QOpenGLExtraFunctions>
class GLFrameBuffer : public QOpenGLExtraFunctions
#else
class GLFrameBuffer
#endif
    {
    public:
        GLFrameBuffer(void)
            {
            fboHandle = 0;
            depthStencilHandle = 0;
            }
            
            
        ~GLFrameBuffer(void)
            {
            glDeleteRenderbuffers(1, &depthStencilHandle);
            glDeleteFramebuffers(1, &fboHandle);
            }
        
        
        bool Initialize(GLsizei nWidth, GLsizei nHeight, GLenum fboTarget)  
            {
#ifdef QT_IS_AVAILABLE
            initializeOpenGLFunctions();
#endif
            glGenFramebuffers(1, &fboHandle);
            glGenRenderbuffers(1, &depthStencilHandle);
            glGenTextures(1, &textureHandle);

            textureWidth = nWidth;
            textureHeight = nHeight;
            
            // Initialize FBO
            glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
        
            glBindTexture(fboTarget, textureHandle);
            
            // Reserve space
            if(fboTarget == GL_TEXTURE_2D)  // star field is drawn with this method
                {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8_SNORM, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureHandle, 0); 
                }
            else
                {
                // Pre-allocate the correct sized cube map
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB8, nWidth, nHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
                glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB8, nWidth, nHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB8, nWidth, nHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
                glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB8, nWidth, nHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB8, nWidth, nHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
                glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB8, nWidth, nHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);

                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                }
                                        
            // Must attach texture to framebuffer. Has Stencil and depth
            glBindRenderbuffer(GL_RENDERBUFFER, depthStencilHandle);
            glRenderbufferStorage(GL_RENDERBUFFER, /*GL_DEPTH_STENCIL*/GL_DEPTH24_STENCIL8, nWidth, nHeight);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthStencilHandle);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilHandle);
                                       
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            
            return true;
            }
            
        
        // Call this to switch to the buffer object for rendering operations. Can be used
        // for general or texture render operations
        inline void Bind(void)
            {
            glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
            }
            
        // Call this when done with the buffer object
        inline void Unbind(void)
            {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
            
            
        // Specifically for rendering into the sides of a cube map texture. 
        inline void BindToCubeFace(GLenum textureTarget)
            {
            glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, textureHandle, 0);
            }
        
            
         inline GLuint GetTextureHandle(void) { return textureHandle; }
         inline GLsizei Width(void) { return textureWidth; }
         inline GLsizei Height(void) { return textureHeight; }
        
    protected:
        GLuint  fboHandle;
        GLuint  depthStencilHandle;
        GLuint  textureHandle;
        
        GLsizei textureWidth;
        GLsizei textureHeight;
    };

#endif
