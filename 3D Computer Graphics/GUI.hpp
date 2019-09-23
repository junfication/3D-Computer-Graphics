#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include <stdio.h>
#include <glm/glm.hpp>
#include "src/GL/glew.h"
#include "src/GLFW/glfw3.h"
#include <vector>

#define M_PI   3.14159265358979323846264338327950288

using namespace ImGui;

using namespace glm;

class GUI
{
  GLFWwindow* glfw_window;
public:
  GUI(GLFWwindow* window)
  {
    glfw_window = window;
    vn_bool = fn_bool = false;
    camera_movement = true;
    camera_state = 1;
    draw_state = 0;
	  shader_type = 0;
    lightPosition.resize(8);
    lightAmbient.resize(8);
    lightType.resize(8);
    lightActiveness.resize(8);
    lightSpecular.resize(8);
    lightDiffuse.resize(8);
    lightDirection.resize(8);
    lightAngle.resize(8);
    for (int i = 0; i < 8; ++i)
    {
      if (i == 0) lightPosition[i].x = 2.f;
      else if (i == 1) lightPosition[i].x = -2.f;
      else if (i == 2) lightPosition[i].y = 2.f;
      else if (i == 3) lightPosition[i].y = -2.f;
      else if (i == 4)
      {
        lightPosition[i].x = static_cast<float>(2.f * cos(45.f *(M_PI / 180.f)));
        lightPosition[i].y = static_cast<float>(2.f * sin(45.f *(M_PI / 180.f)));
      }
      else if (i == 5)
      {
        lightPosition[i].x = static_cast<float>(2.f * cos(135.f *(M_PI / 180.f)));
        lightPosition[i].y = static_cast<float>(2.f * sin(135.f *(M_PI / 180.f)));
      }
      else if (i == 6)
      {
        lightPosition[i].x = static_cast<float>(2.f * cos(225.f *(M_PI / 180.f)));
        lightPosition[i].y = static_cast<float>(2.f * sin(225.f *(M_PI / 180.f)));
      }
      else
      {
        lightPosition[i].x = static_cast<float>(2.f * cos(315.f *(M_PI / 180.f)));
        lightPosition[i].y = static_cast<float>(2.f * sin(315.f *(M_PI / 180.f)));
      }
    }
  }
  void Initialize();
  void Update();
  // GUI_Buttons
  bool vn_bool;
  bool fn_bool;
  bool camera_movement;
  int  camera_state; // 0 = free control, 1 = front, 2 = side, 3 = top
  bool Obj_Open;
  int  draw_state;
  int  shader_type;
  std::vector<int>  lightType;
  std::vector<vec4> lightPosition;
  float angles[8] = { 0.f, 180.f, 90.f, 270.f, 45.f, 135.f, 225.f, 315.f };
  float angle = 0.f;
  float c_angle = 0.f;
  std::vector<vec4> lightAmbient;
  vec4 lightEmissive; // only one
  std::vector<int>  lightActiveness;
  std::vector<vec4> lightSpecular;
  std::vector<vec4> lightDiffuse;
  std::vector<vec4> lightDirection;
  std::vector<vec4> lightAngle;
  vec4 MatAmbient; // only one
  vec4 Attenuation;
  vec4 FogColor;
  vec4 GlobalAmbient;
  int  Light_num = 0;
  int  UV_selection = 0;
  bool light_rotation = false;
  bool camera_rotation = false;
  bool moved = false;
  vec3 cameraPos = vec3(0.f, 0.f, 5.f);
  vec3 deltaCam = vec3(0.f);
  bool cam_moved = false;
  float cam_angle = 0.f;
  vec4 NearFar = vec4(0.001f, 10.f, 0.f, 0.f);
  bool EnvMap = false;
  int environment_options = 0;
  ~GUI();
  float NT = 1.33f;
};