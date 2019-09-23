#include "GUI.hpp"
#include <iostream>
using namespace ImGui;

void GUI::Initialize()
{
  // create the context
  CreateContext();
  ImGuiIO& io = GetIO();
  // init
  ImGui_ImplGlfwGL3_Init(glfw_window, true);
  // set style
  StyleColorsDark(); 
  io.DisplaySize = ImVec2(1024.f, 768.f);
  io.DisplayVisibleMax = ImVec2(1024.f, 768.f);
}

void GUI::Update()
{
  // New Frame of the gui
  ImGui_ImplGlfwGL3_NewFrame();

  if (cameraPos != deltaCam) cam_moved = true;

  if (camera_rotation)
  {
    if (cam_moved)
    {
      cameraPos = vec3(0.f, 0.f, 5.f);
      cam_moved = false;
      cam_angle = 0.f;
    }
    else
    {
      cameraPos.x = static_cast<float>(5.f * sin((cam_angle) *(M_PI / 180.f)));
      cameraPos.z = static_cast<float>(5.f * cos((cam_angle) *(M_PI / 180.f)));
      cam_angle += 0.2f;
      if (cam_angle >= 360.f) cam_angle -= 360.f;
    }
  }
  if (light_rotation)
  {
    if (moved)
    {
      for (int i = 0; i < 8; ++i)
      {
        if (i == 0)
        {
          lightPosition[i].x = 2.f;
          lightPosition[i].y = 0.f;
        }
        else if (i == 1)
        {
          lightPosition[i].x = -2.f;
          lightPosition[i].y = 0.f;
        }
        else if (i == 2)
        {
          lightPosition[i].x = 0.f;
          lightPosition[i].y = 2.f;
        }
        else if (i == 3)
        {
          lightPosition[i].x = 0.f;
          lightPosition[i].y = -2.f;
        }
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
      moved = false;
      angle = 0.f;
    }
    else
    {
      for (int i = 0; i < 8; ++i)
      {
        lightPosition[i].x = static_cast<float>(2.f * cos((angles[i] + angle) *(M_PI / 180.f)));
        lightPosition[i].y = static_cast<float>(2.f * sin((angles[i] + angle) *(M_PI / 180.f)));
      }
      angle += 0.5f;
      if (angle >= 360.f) angle -= 360.f;
    }
  }

  // main menu
  BeginMainMenuBar();
  Text("GUI");
  Separator();
  //Text("%d", moved);
  if (Button("Open Obj File")) Obj_Open = !Obj_Open;
  Separator();
  if (Button("Reset Lights"))
  {
    for (int i = 0; i < 8; ++i)
    {
      if (i == 0)
      {
        lightPosition[i].x = 2.f;
        lightPosition[i].y = 0.f;
      }
      else if (i == 1)
      {
        lightPosition[i].x = -2.f;
        lightPosition[i].y = 0.f;
      }
      else if (i == 2)
      {
        lightPosition[i].x = 0.f;
        lightPosition[i].y = 2.f;
      }
      else if (i == 3)
      {
        lightPosition[i].x = 0.f;
        lightPosition[i].y = -2.f;
      }
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
  Separator();
  if (Button("Pause/Play light rot")) light_rotation = !light_rotation;
  Separator();
  if (Button("Pause/Play cam rot")) camera_rotation = !camera_rotation;
  Separator();
  if (!EnvMap)
  {
    Text("UV Options : ");
    const char* uv_options[] =
    {
      "Cylindrical UV (vertex)",
      "Spherical UV (vertex)",
      "Cube UV (vertex)",
      "Cylindrical UV (normal)",
      "Spherical UV (normal)",
      "Cube UV (normal)"
    };
    Combo("", &UV_selection, uv_options, IM_ARRAYSIZE(uv_options));
  }
  EndMainMenuBar();
  // Toggle Window
  ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;
  Begin("Toggle Window", NULL, flags);
  if (Button("Toggle Vertex Normal")) vn_bool = !vn_bool;
  if (Button("Toggle Face Normal")) fn_bool = !fn_bool;
  if (Button("Toggle Environment Map")) EnvMap = !EnvMap;
  if (!camera_state)
  {
    //if (Button("Toggle Camera Movement")) camera_movement = !camera_movement;
    //SameLine();
    //Text("Buggy!");
  }
  Separator();
  const char* draw_options[] =
  {
    "Mesh",
    "Wireframe",
    "Points",
  };
  Text("Draw Options : ");
  Combo("", &draw_state, draw_options, IM_ARRAYSIZE(draw_options));
  if (!EnvMap)
  {
    const char* shader_options[] =
    {
      "Fragment Lighting",
      "Vertex Lighting",
      "Fragment Lighting + Blinn"
    };
    Text("Shader Options : ");
    Combo(" ", &shader_type, shader_options, IM_ARRAYSIZE(shader_options));
  }
  else
  {
    const char* shader_options[] = 
    {
      "Environment Map",
      "Environment Map + Emissive Color",
      "Refraction Map",
      "Both using the Fresnel approximation"
    };
    Text("Environment Map Options : ");
    Combo(" ", &environment_options, shader_options, IM_ARRAYSIZE(shader_options));
  }
  End();
  // Camera Window
  Begin("Camera Window", NULL, flags);
  const char* cam_options[] =
  {
    "Free Control",
    "Fixed Camera Position 1",
    "Fixed Camera Position 2",
  };
  Text("Camera Control : ");
  if (Combo("", &camera_state, cam_options, IM_ARRAYSIZE(cam_options)))
  {
    if (camera_state == 0) cameraPos = vec3();
  }
  End();
  // Light Window
  flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
  Begin("Light Controls", NULL, flags);
  if (EnvMap && environment_options > 1)
  {
    Text("Nt value : ");
    InputFloat("              ", &NT);
    if (NT < 0.f) NT = 0.f;
    if (NT > 100.f) NT = 100.f;
    Separator();
  }
  const char* light_options[] =
  {
    "Light 1",
    "Light 2",
    "Light 3",
    "Light 4",
    "Light 5",
    "Light 6",
    "Light 7",
    "Light 8",
    "All Lights"
  };
  Text("Light Control : ");
  Combo("", &Light_num, light_options, IM_ARRAYSIZE(light_options));
  if (Light_num != 8)
  {
    Text("Light Number %d", Light_num + 1);
    Separator();
    // Light Position
    if ((lightType[Light_num] != 2 && !light_rotation) || EnvMap)
    {
      Text("Light Position : ");
      float Pos[3] = { lightPosition[Light_num].x, lightPosition[Light_num].y, lightPosition[Light_num].z };
      ImGui::InputFloat3("", Pos, 5);
      if (lightPosition[Light_num].x != Pos[0]) moved = true;
      if (lightPosition[Light_num].y != Pos[1]) moved = true;
      if (lightPosition[Light_num].z != Pos[2]) moved = true;
      lightPosition[Light_num].x = Pos[0];
      lightPosition[Light_num].y = Pos[1];
      lightPosition[Light_num].z = Pos[2];
      Separator();
    }
    if (!EnvMap)
    {
      if (lightType[Light_num] != 0)
      {
        Text("Light Direction : ");
        float Dir[3] = { lightDirection[Light_num].x, lightDirection[Light_num].y, lightDirection[Light_num].z };
        ImGui::InputFloat3(" ", Dir, 5);
        lightDirection[Light_num].x = Dir[0];
        lightDirection[Light_num].y = Dir[1];
        lightDirection[Light_num].z = Dir[2];
        Separator();
      }
      if (lightType[Light_num] == 1)
      {
        Text("Inner Angle : ");
        float Angles[3] = { lightAngle[Light_num].x, lightAngle[Light_num].y, lightAngle[Light_num].z };
        ImGui::InputFloat("   ", &Angles[0]);
        lightAngle[Light_num].x = Angles[0];
        Text("Outer Angle : ");
        ImGui::InputFloat("    ", &Angles[1]);
        lightAngle[Light_num].y = Angles[1];
        Text("Spot falloff : ");
        ImGui::InputFloat("     ", &Angles[2]);
        lightAngle[Light_num].z = Angles[2];
        Separator();
      }
      // Light Activeness
      Text("Light Active : ");
      const char* light_activeness[] =
      {
        "Yes",
        "No"
      };
      Combo("  ", &lightActiveness[Light_num], light_activeness, IM_ARRAYSIZE(light_activeness));
      Separator();
      // Light Type
      Text("Light Type : ");
      const char* light_types[] =
      {
        "Point Light",
        "Spotlight",
        "Directional Light"
      };
      Combo(" ", &lightType[Light_num], light_types, IM_ARRAYSIZE(light_types));
      Separator();
      // Ambient Color 
      Text("Light Ambient Color Picker: ");
      float Clr[] = { lightAmbient[Light_num].r, lightAmbient[Light_num].g, lightAmbient[Light_num].b };
      ImGui::ColorPicker3("", Clr);
      lightAmbient[Light_num].r = Clr[0];
      lightAmbient[Light_num].g = Clr[1];
      lightAmbient[Light_num].b = Clr[2];
      Text("Ambient Color (normalized) : ");
      Text("%f, %f, %f", lightAmbient[Light_num].r, lightAmbient[Light_num].g, lightAmbient[Light_num].b);
      Separator();
      // Specular Color
      Text("Light Specular Color Picker: ");
      float Spc[] = { lightSpecular[Light_num].r, lightSpecular[Light_num].g, lightSpecular[Light_num].b };
      ImGui::ColorPicker3("   ", Spc);
      lightSpecular[Light_num].r = Spc[0];
      lightSpecular[Light_num].g = Spc[1];
      lightSpecular[Light_num].b = Spc[2];
      Text("Specular Color (normalized) : ");
      Text("%f, %f, %f", lightSpecular[Light_num].r, lightSpecular[Light_num].g, lightSpecular[Light_num].b);
      Separator();
    }
      // Diffuse Color
      Text("Light Diffuse Color Picker: ");
      float Dif[] = { lightDiffuse[Light_num].r, lightDiffuse[Light_num].g, lightDiffuse[Light_num].b };
      ImGui::ColorPicker3("    ", Dif);
      lightDiffuse[Light_num].r = Dif[0];
      lightDiffuse[Light_num].g = Dif[1];
      lightDiffuse[Light_num].b = Dif[2];
      Text("Diffuse Color (normalized) : ");
      Text("%f, %f, %f", lightDiffuse[Light_num].r, lightDiffuse[Light_num].g, lightDiffuse[Light_num].b);
      Separator();
    // Material Ambient
  }
  else
  {
    Text("All Lights");
    Separator();
    if (!EnvMap)
    {
      // All light Types
      Text("Light Type : ");
      const char* light_types[] =
      {
        "Point Light",
        "Spotlight",
        "Directional Light"
      };
      Combo(" ", &lightType[0], light_types, IM_ARRAYSIZE(light_types));
      for (int i = 1; i < 8; ++i)
        lightType[i] = lightType[0];
      Separator();
      // All light Ambient
      Text("Light Ambient Color Picker: ");
      float Clr[] = { lightAmbient[0].r, lightAmbient[0].g, lightAmbient[0].b };
      ImGui::ColorPicker3("", Clr);
      lightAmbient[0].r = Clr[0];
      lightAmbient[0].g = Clr[1];
      lightAmbient[0].b = Clr[2];
      Text("Ambient Color (normalized) : ");
      Text("%f, %f, %f", lightAmbient[0].r, lightAmbient[0].g, lightAmbient[0].b);
      for (int i = 1; i < 8; ++i)
        lightAmbient[i] = lightAmbient[0];
      Separator();
      // All light Specular
      Text("Light Specular Color Picker: ");
      float Spc[] = { lightSpecular[0].r, lightSpecular[0].g, lightSpecular[0].b };
      ImGui::ColorPicker3("  ", Spc);
      lightSpecular[0].r = Spc[0];
      lightSpecular[0].g = Spc[1];
      lightSpecular[0].b = Spc[2];
      Text("Specular Color (normalized) : ");
      Text("%f, %f, %f", lightSpecular[0].r, lightSpecular[0].g, lightSpecular[0].b);
      for (int i = 1; i < 8; ++i)
        lightSpecular[i] = lightSpecular[0];
      Separator();
    }
    // All Light Diffuse
    Text("Light Diffuse Color Picker: ");
    float Dif[] = { lightDiffuse[0].r, lightDiffuse[0].g, lightDiffuse[0].b };
    ImGui::ColorPicker3("   ", Dif);
    lightDiffuse[0].r = Dif[0];
    lightDiffuse[0].g = Dif[1];
    lightDiffuse[0].b = Dif[2];
    Text("Diffuse Color (normalized) : ");
    Text("%f, %f, %f", lightDiffuse[0].r, lightDiffuse[0].g, lightDiffuse[0].b);
    for (int i = 1; i < 8; ++i)
      lightDiffuse[i] = lightDiffuse[0];
    Separator();
  }
  End();
  // Global Window
  Begin("Global Controls", NULL, flags);
  // Attenuation Values
  if (!EnvMap)
  {
    Text("Fog Near Far Values");
    float near[2] = { NearFar.x, NearFar.y };
    Text("Near Fog: ");
    ImGui::InputFloat("        ", &near[0]);
    NearFar.x = near[0];
    Text("Far Fog: ");
    ImGui::InputFloat("          ", &near[1]);
    NearFar.y = near[1];
    Text("Attenuation Values : ");
    Text("C1 : ");
    float atten[3] = { Attenuation.x, Attenuation.y, Attenuation.z };
    ImGui::InputFloat("   ", &atten[0]);
    Attenuation.x = atten[0];
    Text("C2 : ");
    ImGui::InputFloat("    ", &atten[1]);
    Attenuation.y = atten[1];
    Text("C3 : ");
    ImGui::InputFloat("     ", &atten[2]);
    Attenuation.z = atten[2];
    Separator();
    // Fog Color Picker
    Text("Fog Color Picker : ");
    float fog[] = { FogColor.r, FogColor.g, FogColor.b };
    ImGui::ColorPicker3(" ", fog);
    FogColor.r = fog[0];
    FogColor.g = fog[1];
    FogColor.b = fog[2];
    Text("Fog Color (normalized) : ");
    Text("%f, %f, %f", FogColor.r, FogColor.g, FogColor.b);
    Separator();
    // Global Ambient Picker
    Text("Global Ambient Color Picker : ");
    float GA[] = { GlobalAmbient.r, GlobalAmbient.g, GlobalAmbient.b };
    ImGui::ColorPicker3("", GA);
    GlobalAmbient.r = GA[0];
    GlobalAmbient.g = GA[1];
    GlobalAmbient.b = GA[2];
    Text("Global Ambient (normalized) : ");
    Text("%f, %f, %f", GlobalAmbient.r, GlobalAmbient.g, GlobalAmbient.b);
    Separator();
  }
  // Global Emissive
  Text("Emissive Color Picker : ");
  float LE[] = { lightEmissive.r, lightEmissive.g, lightEmissive.b };
  ImGui::ColorPicker3("   ", LE);
  lightEmissive.r = LE[0];
  lightEmissive.g = LE[1];
  lightEmissive.b = LE[2];
  Text("Emissive Color (normalized) : ");
  Text("%f, %f, %f", lightEmissive.r, lightEmissive.g, lightEmissive.b);
  Separator();
  if (!EnvMap)
  {
    // Global Material Ambient
    Text("K Ambient Picker : ");
    float GM[] = { MatAmbient.r, MatAmbient.g, MatAmbient.b };
    ImGui::ColorPicker3("    ", GM);
    MatAmbient.r = GM[0];
    MatAmbient.g = GM[1];
    MatAmbient.b = GM[2];
    Text("K Ambient (normalized) : ");
    Text("%f, %f, %f", MatAmbient.r, MatAmbient.g, MatAmbient.b);
    Separator();
  }
  End();
  // Render
  Render();
  ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
  deltaCam = cameraPos;
}

GUI::~GUI()
{
  DestroyContext();
}
