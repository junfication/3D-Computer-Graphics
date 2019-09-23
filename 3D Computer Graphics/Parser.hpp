#pragma once
#include <vector>
#include <gl\glew.h>
#include <glm\glm.hpp>

using namespace glm;

int ParseObj(std::vector<GLuint>* indices, std::vector<GLfloat>* vertices, 
	           std::vector<GLfloat>* normals, std::vector<GLfloat>* color, 
	           float& scale, vec3& center, std::vector<GLfloat>* vn,
             std::vector<GLfloat>* fn, std::vector<GLfloat>* cylin_uv,
             std::vector<GLfloat>* sphere_uv, std::vector<GLfloat>* cubic_uv, 
             std::vector<GLfloat>* cylin_norm,
             std::vector<GLfloat>* sphere_norm, std::vector<GLfloat>* cubic_norm,
             const char* file = "Obj/cube.obj");

int ParseTex(std::vector<GLfloat>* diffuse, std::vector<GLfloat>* specular, 
             int& diffuse_height, int& diffuse_width, 
             int& specular_width, int& specular_height,
             const char* d_texture = "Textures/metal_roof_diff_512x512.ppm",
             const char* s_texture = "Textures/metal_roof_spec_512x512.ppm");