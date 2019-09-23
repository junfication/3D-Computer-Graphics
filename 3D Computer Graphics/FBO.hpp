#pragma once

// include GLEW
# include <gl/glew.h>

// include GLFW
#include <GLFW/glfw3.h>

#include <iostream>

bool SetUpFBO(GLuint* FBOHandle, GLuint* renderedTexture, GLuint* depthrenderbuffer);
bool UseFBO(GLuint FBOHandle);
  