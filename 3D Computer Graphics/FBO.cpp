#include "FBO.hpp"

bool SetUpFBO(GLuint * FBOHandle, GLuint* renderedTexture, GLuint* depthrenderbuffer)
{
  glGenFramebuffers(1, FBOHandle);
  glBindFramebuffer(GL_FRAMEBUFFER, *FBOHandle);

  glGenTextures(6, renderedTexture);

  for (int i = 0; i < 6; ++i)
  {
    glBindTexture(GL_TEXTURE_2D, renderedTexture[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 768, 0, GL_RGB, GL_FLOAT, NULL);
  }

  glGenRenderbuffers(1, depthrenderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, *depthrenderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *depthrenderbuffer);


  // Set "renderedTexture" as our// "6" is the size of DrawBuffers

  // Always check that our framebuffer is ok
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    return false;
  return true;
}

bool UseFBO(GLuint FBOHandle)
{
  glBindFramebuffer(GL_FRAMEBUFFER, FBOHandle);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "[UseFBO] Cannot switch to FBO - " << FBOHandle << std::endl;
    std::cout << "[UseFBO] FBO not complete" << std::endl;
    return false;
  }
  // glViewport(0, 0, 1024, 768);
  return true;
}


