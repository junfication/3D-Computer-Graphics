// include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <cstring>
#include <windows.h>

// include GLEW
# include <gl/glew.h>

// include GLFW
#include <GLFW/glfw3.h>

// GLFW window
GLFWwindow* window;

#define GLM_ENABLE_EXPERIMENTAL

// include GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

// include shader io
#include "shader.hpp"

// include file io for obj file
#include "Parser.hpp"

#include <vector> // std::vector
#include <limits> // std::numerical_limits
#include <math.h> // cos sin

// include the imGui for the gui
#include "GUI.hpp"

// include FBO header
#include "FBO.hpp"

// include tga parser
#include "tga.hpp"

// define pi values
#define M_PI   3.14159265358979323846264338327950288
#define M_PI_2   3.14159265358979323846264338327950288 / 2

// projection matrix
glm::mat4 projectionMatrix;

GLfloat skyBoxVertices[]
{
  // positions          
  -1.f, -1.f, -1.f, // back
   1.f, -1.f, -1.f,
   1.f,  1.f, -1.f,
   1.f,  1.f, -1.f,
  -1.f,  1.f, -1.f,
  -1.f, -1.f, -1.f,

  -1.f, -1.f,  1.f, //front
   1.f, -1.f,  1.f,
   1.f,  1.f,  1.f,
   1.f,  1.f,  1.f,
  -1.f,  1.f,  1.f,
  -1.f, -1.f,  1.f,

  -1.f,  1.f,  1.f,
  -1.f,  1.f, -1.f, //left
  -1.f, -1.f, -1.f,
  -1.f, -1.f, -1.f,
  -1.f, -1.f,  1.f,
  -1.f,  1.f,  1.f,

   1.f,  1.f,  1.f,
   1.f,  1.f, -1.f,// right
   1.f, -1.f, -1.f,
   1.f, -1.f, -1.f,
   1.f, -1.f,  1.f,
   1.f,  1.f,  1.f,

  -1.f, -1.f, -1.f,
   1.f, -1.f, -1.f, // bottom
   1.f, -1.f,  1.f,
   1.f, -1.f,  1.f,
  -1.f, -1.f,  1.f,
  -1.f, -1.f, -1.f,

  -1.f,  1.f, -1.f,
   1.f,  1.f, -1.f,
   1.f,  1.f,  1.f,// top
   1.f,  1.f,  1.f,
  -1.f,  1.f,  1.f,
  -1.f,  1.f, -1.f,
};

GLfloat skyBoxUVs[]
{
  0.0f,  0.0f, // back
  1.0f,  0.0f, 
  1.0f,  1.0f, 
  1.0f,  1.0f, 
  0.0f,  1.0f, 
  0.0f,  0.0f, 

  1.0f,  0.0f, 
  0.0f,  0.0f, // front
  0.0f,  1.0f, 
  0.0f,  1.0f, 
  1.0f,  1.0f, 
  1.0f,  0.0f, 

  0.0f,  1.0f,
  1.0f,  1.0f, // left
  1.0f,  0.0f,
  1.0f,  0.0f,
  0.0f,  0.0f,
  0.0f,  1.0f,

  1.0f,  1.0f, 
  0.0f,  1.0f,  // right
  0.0f,  0.0f, 
  0.0f,  0.0f, 
  1.0f,  0.0f, 
  1.0f,  1.0f, 

  0.0f, 0.0f,
  0.0f, 1.0f,  // bottom
  1.0f, 1.0f,
  1.0f, 1.0f,
  1.0f, 0.0f,
  0.0f, 0.0f,

  0.0f, 1.0f,
  0.0f, 0.0f,
  1.0f, 0.0f,   // top
  1.0f, 0.0f,
  1.0f, 1.0f,
  0.0f, 1.0f
};

// default camera pos
// vec3 cameraPos = vec3(0.f, 0.f, 0.f);
int  state = 0; // 0 = fill, 1 = wireframe, 2 = points

// default object position;
vec3 objPos = vec3(0.f, 0.f, 0.f);

// global values to store mouse pos
double current_x = 0;
double current_y = 0;

// angles
float angle_x = 0.f;
float angle_y = 0.f;

// filename for open file dialog
char filename[MAX_PATH];
// capture states for keyboard press
int oldState[4] = { GLFW_RELEASE, GLFW_RELEASE, GLFW_RELEASE };

// class to generate sphere mesh
class Sphere
{
  // vertices of the sphere
  std::vector<GLfloat> vertices;
  // normals of the sphere
  std::vector<GLfloat> normals;
  // indices of the sphere
  std::vector<GLuint> indices;
public:
  // Function to generate the sphere
  void GenerateSphere(float radius, unsigned rings, unsigned sectors)
  {
    const float R = 1.f / static_cast<float>(rings - 1);
    const float S = 1.f / static_cast<float>(sectors - 1);
    // resize the vertices to store all the vertices and normals
    vertices.resize(rings * sectors * 3);
    normals.resize(rings * sectors * 3);
    // getting the vertices and normal begin iterator
    std::vector<GLfloat>::iterator v = vertices.begin();
    std::vector<GLfloat>::iterator n = normals.begin();
    // loop through every ring and sector of the circle, generating each point
    for(unsigned r = 0; r < rings; ++r)
      for (unsigned s = 0; s < sectors; ++s)
      {
        // calculating the x y z position of the point
        float y = static_cast<float>(sin(-M_PI_2 + M_PI * r * R));
        float x = static_cast<float>(cos(2 * M_PI * s * S) * sin(M_PI * r * R));
        float z = static_cast<float>(sin(2 * M_PI * s * S) * sin(M_PI * r * R));
        // as the points are in 1 unit radius, scale by radius
        *v++ = x * radius;
        *v++ = y * radius;
        *v++ = z * radius;
        // set the normal as the points to render the color as the normal
        *n++ = x;
        *n++ = y;
        *n++ = z;
      }
    // resize the indices to store all the indices
    indices.resize(rings * sectors * 6);
    // set the iterator to the head of the indices
    std::vector<GLuint>::iterator i = indices.begin();
    for(unsigned r = 0; r < rings; ++r)
      for(unsigned s = 0; s < sectors; ++s)
      {
        // triangle one
        *i++ = r * sectors + s;
        *i++ = (r + 1) * sectors + s;
        *i++ = r * sectors + (s + 1);
        //triangle two
        *i++ = (r + 1) * sectors + (s + 1);
        *i++ = r * sectors + (s + 1);
        *i++ = (r + 1) * sectors + s;
      }


  }
  // getters
  std::vector<GLfloat>& GetVertices() { return vertices; }
  std::vector<GLfloat>& GetNormals() { return normals; }
  std::vector<GLuint>& GetIndices() { return indices; }

};

// Generate the orbit of the sphere on the xy plane, based on the degree and the radius
void GenerateOrbit(std::vector<GLfloat>* ov, float degree = 5.f, float radius = 2.f)
{
  // pushing all the points into the vector
  for (float i = 0; i < 360.f; i += degree)
  {
    // calculating the x and y of the orbit
    float x = static_cast<float>(radius * cos(i * (M_PI / 180.f)));
    float y = static_cast<float>(radius * sin(i * (M_PI / 180.f)));
    ov->push_back(x);
    ov->push_back(y);
    ov->push_back(0.f);  
  }
}

int main(void)
{
  // buffer for ubo
  GLubyte* buffer = nullptr;
  GLint    buffer_size = 0;
  // mouse pos
  double mouse_x = 0;
  double mouse_y = 0;

  // default direction of the camera is looking at
  vec3 direction = vec3(0, 0, 1.f);

  // FBO Handles
  GLuint FBOHandle;
  GLuint renderedTexture[6];
  GLuint depthrenderbuffer;

  // skybox
  Tga SkyFront("skybox/front.tga");
  Tga SkyBack ("skybox/back.tga");
  Tga SkyUp   ("skybox/up.tga");
  Tga SkyDown ("skybox/down.tga");
  Tga SkyLeft ("skybox/left.tga");
  Tga SkyRight("skybox/right.tga");

  GLuint skyboxTexture[6];

  // setting the default mouse x and y position
  glfwGetCursorPos(window, &mouse_x, &mouse_y);

  // all the containers of the vetices, indices, normals and etc
  std::vector<GLuint>  indices;
  std::vector<GLfloat> normals;
  std::vector<GLfloat> color;
  std::vector<GLfloat> vertices;
  std::vector<GLfloat> vertex_normal;
  std::vector<GLfloat> face_normal;
  std::vector<GLfloat> orbit_vertices;
  std::vector<GLfloat> normal_clr;
  std::vector<GLfloat> planar_uv;
  std::vector<GLfloat> spherical_uv;
  std::vector<GLfloat> cylindrical_uv;
  std::vector<GLfloat> specular_map;
  std::vector<GLfloat> diffuse_map;
  std::vector<GLfloat> cubic_uv;
  std::vector<GLfloat> spherical_normal;
  std::vector<GLfloat> cylindrical_normal;
  std::vector<GLfloat> cubic_normal;

  // Generating the orbit vertices
  GenerateOrbit(&orbit_vertices, 5.f, 2.f);

  // The center of the mesh (bounding box method)
  vec3 center;
  // The scale of the mesh to be scaled down
  float scale = 0.f;
  int diffuse_width, diffuse_height, specular_width, specular_height;
  // Parse the obj into the indices, vertices, normals, color, scale and etc
  if (!ParseObj(&indices, &vertices, &normals, &color, scale, center, &face_normal, &vertex_normal, &cylindrical_uv, &spherical_uv, &cubic_uv, &cylindrical_normal, &spherical_normal, &cubic_normal)) return -1;

  // Parse the texture into the vectors
  if (!ParseTex(&diffuse_map, &specular_map, diffuse_height, diffuse_width, specular_width, specular_height)) return -1;

  std::cout << "Diffuse Texture : " << diffuse_height << " " << diffuse_width << std::endl;

  std::cout << "Specular Texture : " << specular_height << " " << specular_width << std::endl;

  // setting color for the vertex normal
  size_t b_size = vertex_normal.size();
  if (b_size < face_normal.size()) b_size = face_normal.size();
  if (b_size < orbit_vertices.size()) b_size = orbit_vertices.size();

  for (size_t i = 0; i < b_size; ++i) normal_clr.push_back(1.0f);

  // Create a sphere
  Sphere object;
  // generate the mesh
  object.GenerateSphere(0.125f, 9, 18);

  // init GLFW
  if (!glfwInit())
  {
    fprintf(stderr, "Failed to initialize GLFW\n");
    getchar();
    return -1;
  }
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow(1024, 768, "Sample 1.2 - Using GLSL Pipeline Objects", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 4.0 compatible. Try the 2.1 version of the tutorials.\n");
    getchar();
    glfwTerminate();
    return -1;
  }
  // initialize the gui
  GUI hud{ window };
  hud.Initialize();

  glfwMakeContextCurrent(window);

  // Initialize GLEW
  glewExperimental = GL_TRUE; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return -1;
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  // Create and bind the vertex array
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  // Create and compile our GLSL program from the shaders
  GLuint programID = LoadShaders("Shaders/SimpleVertexShader.vertexshader", "Shaders/SimpleFragmentShader.fragmentshader");

  // Create Light Source Shaders
  GLuint LightID = LoadShaders("Shaders/LightSourceShader.vertexshader", "Shaders/LightSourceShader.fragmentshader");

  // Create and compile the other type of shaders
  GLuint vertexLightingID = LoadShaders("Shaders/VertexLighting.vertexshader", "Shaders/VertexLighting.fragmentshader");

  // Create and compile the blinn shaders 
  GLuint blinnLightingID = LoadShaders("Shaders/BlinnLighting.vertexshader", "Shaders/BlinnLighting.fragmentshader");

  // Create and compile the FrameBuffer Shader
  GLuint FSQProgramID = LoadShaders("Shaders/FSQ.vertexshader", "Shaders/FSQ.fragmentshader");

  // Create and compile the Skybox shader
  GLuint SkyBoxID = LoadShaders("Shaders/SkyBoxShader.vertexshader", "Shaders/SkyBoxShader.fragmentshader");

  // Create and compile the environment map shader
  GLuint EnvMapID = LoadShaders("Shaders/EnvironmentMap.vertexshader", "Shaders/EnvironmentMap.fragmentshader");

  // Create and compile the environment map + emissive
  GLuint FSQEmissive = LoadShaders("Shaders/FSQ_Emissive.vertexshader", "Shaders/FSQ_Emissive.fragmentshader");

  // Create and compile the refraction shader
  GLuint RefractionID = LoadShaders("Shaders/Refraction.vertexshader", "Shaders/Refraction.fragmentshader");

  // create and compile the fresnal shader
  GLuint FresnelID = LoadShaders("Shaders/Fresnel.vertexshader", "Shaders/Fresnel.fragmentshader");

  // generate the vertex buffer, 0 = position, 1 = color, 2 = normals, 3 = indices, 4 = uv
  GLuint vertexbuffers[5];
  glGenBuffers(5, vertexbuffers);

  GLuint uboHandle;
  glGenBuffers(1, &uboHandle);

  GLuint texturebuffer[2];
  glGenTextures(2, texturebuffer);
  
  glBindTexture(GL_TEXTURE_2D, texturebuffer[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, diffuse_width, diffuse_height, 0, GL_RGB, GL_FLOAT, diffuse_map.data());

  glBindTexture(GL_TEXTURE_2D, texturebuffer[1]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, specular_width, specular_height, 0, GL_RGB, GL_FLOAT, specular_map.data());

  // generate skybox texture
  glGenTextures(6, skyboxTexture);
  glBindTexture(GL_TEXTURE_2D, skyboxTexture[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, SkyFront.HasAlphaChannel() ? GL_RGBA : GL_RGB, SkyFront.GetWidth(), SkyFront.GetWidth(), 0, SkyFront.HasAlphaChannel() ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, SkyFront.GetPixels().data());

  glBindTexture(GL_TEXTURE_2D, skyboxTexture[1]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, SkyBack.HasAlphaChannel() ? GL_RGBA : GL_RGB, SkyBack.GetWidth(), SkyBack.GetWidth(), 0, SkyBack.HasAlphaChannel() ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, SkyBack.GetPixels().data());

  glBindTexture(GL_TEXTURE_2D, skyboxTexture[2]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, SkyUp.HasAlphaChannel() ? GL_RGBA : GL_RGB, SkyUp.GetWidth(), SkyUp.GetWidth(), 0, SkyUp.HasAlphaChannel() ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, SkyUp.GetPixels().data());

  glBindTexture(GL_TEXTURE_2D, skyboxTexture[3]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, SkyDown.HasAlphaChannel() ? GL_RGBA : GL_RGB, SkyDown.GetWidth(), SkyDown.GetWidth(), 0, SkyDown.HasAlphaChannel() ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, SkyDown.GetPixels().data());

  glBindTexture(GL_TEXTURE_2D, skyboxTexture[4]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, SkyLeft.HasAlphaChannel() ? GL_RGBA : GL_RGB, SkyLeft.GetWidth(), SkyLeft.GetWidth(), 0, SkyLeft.HasAlphaChannel() ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, SkyLeft.GetPixels().data());

  glBindTexture(GL_TEXTURE_2D, skyboxTexture[5]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, SkyRight.HasAlphaChannel() ? GL_RGBA : GL_RGB, SkyRight.GetWidth(), SkyRight.GetWidth(), 0, SkyRight.HasAlphaChannel() ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, SkyRight.GetPixels().data());

  // enable depth test
  glClearDepth(1);
  glEnable(GL_DEPTH_TEST);

  if (!SetUpFBO(&FBOHandle, renderedTexture, &depthrenderbuffer))
  {
    std::cout << "Error Setting Up FBO" << std::endl;
    return -1;
  }

  do {

    // Pass 1: Render to the texture object
   
    glUseProgram(programID);
    glUniform1i(glGetUniformLocation(programID, "diffuse"), 0);
    glUniform1i(glGetUniformLocation(programID, "specular"), 1);

    glUseProgram(vertexLightingID);
    glUniform1i(glGetUniformLocation(vertexLightingID, "diffuse"), 0);
    glUniform1i(glGetUniformLocation(vertexLightingID, "specular"), 1);

    glUseProgram(blinnLightingID);
    glUniform1i(glGetUniformLocation(blinnLightingID, "diffuse"), 0);
    glUniform1i(glGetUniformLocation(blinnLightingID, "specular"), 1);

    glUseProgram(SkyBoxID);
    glUniform1i(glGetUniformLocation(SkyBoxID, "skyBoxTexture"), 0);
    
    glUseProgram(EnvMapID);
    glUniform1i(glGetUniformLocation(EnvMapID, "texSampler"), 0);
    
    glUseProgram(FSQProgramID);
    glUniform1i(glGetUniformLocation(FSQProgramID, "up"), 0);
    glUniform1i(glGetUniformLocation(FSQProgramID, "down"), 1);
    glUniform1i(glGetUniformLocation(FSQProgramID, "left"), 2);
    glUniform1i(glGetUniformLocation(FSQProgramID, "right"), 3);
    glUniform1i(glGetUniformLocation(FSQProgramID, "front"), 4);
    glUniform1i(glGetUniformLocation(FSQProgramID, "back"), 5);

    glUseProgram(FSQEmissive);
    glUniform1i(glGetUniformLocation(FSQEmissive, "up"), 0);
    glUniform1i(glGetUniformLocation(FSQEmissive, "down"), 1);
    glUniform1i(glGetUniformLocation(FSQEmissive, "left"), 2);
    glUniform1i(glGetUniformLocation(FSQEmissive, "right"), 3);
    glUniform1i(glGetUniformLocation(FSQEmissive, "front"), 4);
    glUniform1i(glGetUniformLocation(FSQEmissive, "back"), 5);    
    
    glUseProgram(RefractionID);
    glUniform1i(glGetUniformLocation(RefractionID, "up"), 0);
    glUniform1i(glGetUniformLocation(RefractionID, "down"), 1);
    glUniform1i(glGetUniformLocation(RefractionID, "left"), 2);
    glUniform1i(glGetUniformLocation(RefractionID, "right"), 3);
    glUniform1i(glGetUniformLocation(RefractionID, "front"), 4);
    glUniform1i(glGetUniformLocation(RefractionID, "back"), 5);

    glUseProgram(FresnelID);
    glUniform1i(glGetUniformLocation(FresnelID, "up"), 0);
    glUniform1i(glGetUniformLocation(FresnelID, "down"), 1);
    glUniform1i(glGetUniformLocation(FresnelID, "left"), 2);
    glUniform1i(glGetUniformLocation(FresnelID, "right"), 3);
    glUniform1i(glGetUniformLocation(FresnelID, "front"), 4);
    glUniform1i(glGetUniformLocation(FresnelID, "back"), 5);

    // Dark blue background
    glClearColor(hud.FogColor.r, hud.FogColor.g, hud.FogColor.b, 0.0f);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // file open dialog press o
    int newState = glfwGetKey(window, GLFW_KEY_O);
    if ((oldState[0] == GLFW_RELEASE && newState == GLFW_PRESS) || hud.Obj_Open)
    {
      OPENFILENAME ofn;
      ZeroMemory(&filename, sizeof(filename));
      ZeroMemory(&ofn, sizeof(ofn));
      ofn.lStructSize = sizeof(ofn);
      ofn.hwndOwner = NULL;
      ofn.lpstrFilter = "OBJ Files\0*.obj\0";
      ofn.lpstrFile = filename;
      ofn.nMaxFile = MAX_PATH;
      ofn.lpstrTitle = "Select an OBJ File :)";
      ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
      // Get the file name and path of the file opens
      if (GetOpenFileNameA(&ofn))
      {
        std::cout << "You chose the file \"" << filename << "\"\n";
        // clear the containers
        indices.clear();
        normals.clear();
        color.clear();
        vertices.clear();
        vertex_normal.clear();
        face_normal.clear();
	      normal_clr.clear();
		    cylindrical_uv.clear();
	    	spherical_uv.clear();
        cubic_uv.clear();
        cylindrical_normal.clear();
        spherical_normal.clear();
        cubic_normal.clear();
        // load in the new mesh from obj
        if (!ParseObj(&indices, &vertices, &normals, &color, scale, center, &face_normal, &vertex_normal, &cylindrical_uv, &spherical_uv, &cubic_uv, &cylindrical_normal, &spherical_normal, &cubic_normal, filename)) return -1;

		// setting color of the normals
		size_t b_size = vertex_normal.size();
		if (b_size < face_normal.size()) b_size = face_normal.size();
		if (b_size < orbit_vertices.size()) b_size = orbit_vertices.size();

		for (size_t i = 0; i < b_size; ++i) normal_clr.push_back(1.0f);

      }
      // set the bool for the file open dialog in hud to false
      hud.Obj_Open = !hud.Obj_Open;
    }
    // set old state as new state to prevent multiple file dialog box
    oldState[0] = newState;

    // set the shader
    GLuint shader = programID;
    if (hud.shader_type == 1) shader = vertexLightingID;
    if (hud.shader_type == 2) shader = blinnLightingID;

    // set the environment map shader
    GLuint EnvShader = FSQProgramID;
    if (hud.environment_options == 1) EnvShader = FSQEmissive;
    if (hud.environment_options == 2) EnvShader = RefractionID;
    if (hud.environment_options == 3) EnvShader = FresnelID;

    // 1st attribute buffer : vertex positions (model space)
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[0]);
    glVertexAttribPointer(
      0,                  // attribute 0. MUST match the layout in the shader.
      3,                  // size
      GL_FLOAT,           // type
      GL_FALSE,           // normalized?
      0,                  // stride
      (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : vertex color
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[1]);
    glVertexAttribPointer(
      1,                  // attribute 1. MUST match the layout in the shader.
      3,                  // size
      GL_FLOAT,           // type
      GL_FALSE,           // normalized?
      0,                  // stride
      (void*)0            // array buffer offset
    );

    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[2]);
    glVertexAttribPointer(
      2,                  // attribute 1. MUST match the layout in the shader.
      3,                  // size
      GL_FLOAT,           // type
      GL_FALSE,           // normalized?
      0,                  // stride
      (void*)0            // array buffer offset
    );

    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[4]);
    glVertexAttribPointer(
      3,                  // attribute 1. MUST match the layout in the shader.
      2,                  // size
      GL_FLOAT,           // type
      GL_FALSE,           // normalized?
      0,                  // stride
      (void*)0            // array buffer offset
    );

    // Loading of uniform blocks
    GLuint   uboIndex;
    GLint    uboSize;

    GLuint   ubo_indices[15];
    GLint   offset[15];
    const GLchar* names[] =
    {
      "LightArray.LightActive",
      "LightArray.LightType",
      "LightArray.LightPosition",
      "LightArray.Ambient",
      "LightArray.Emissive",
      "LightArray.I_Specular",
      "LightArray.I_Diffuse",
      "LightArray.LightDirection",
      "LightArray.LightAngle",
      "LightArray.K_Ambient",
      "LightArray.Attenuation",
      "LightArray.FogColor",
      "LightArray.GlobalAmbient",
      "LightArray.NearFar",
      "LightArray.ViewVec"
    };

    // get block index
    uboIndex = glGetUniformBlockIndex(shader, "LightArray");

    if (uboIndex != GL_INVALID_INDEX)
    {
      vec4 NearfarPlane = hud.NearFar;
      vec4 viewvec = vec4(vec3(objPos - hud.cameraPos), 0.f);

      // get block size
      glGetActiveUniformBlockiv(shader, uboIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uboSize);

      if (buffer == nullptr)
      {
        buffer_size = uboSize;
        buffer =  new GLubyte[uboSize];
      }
      else if (uboSize != buffer_size)
      {
        buffer_size = uboSize;
        delete[](buffer);
        buffer = new GLubyte[uboSize];
      }
      glGetUniformIndices(shader, 15, names, ubo_indices);
      glGetActiveUniformsiv(shader, 15, ubo_indices, GL_UNIFORM_OFFSET, offset);
      std::memcpy(buffer + offset[0], hud.lightActiveness.data(), sizeof(int) * hud.lightActiveness.size());
      std::memcpy(buffer + offset[1], hud.lightType.data(), sizeof(int) * hud.lightType.size());
      std::memcpy(buffer + offset[2], hud.lightPosition.data(), sizeof(vec4) * hud.lightPosition.size());
      std::memcpy(buffer + offset[3], hud.lightAmbient.data(), sizeof(vec4) * hud.lightAmbient.size());
      std::memcpy(buffer + offset[4], &hud.lightEmissive, sizeof(hud.lightEmissive));
      std::memcpy(buffer + offset[5], hud.lightSpecular.data(), sizeof(vec4) * hud.lightSpecular.size());
      std::memcpy(buffer + offset[6], hud.lightDiffuse.data(), sizeof(vec4) * hud.lightDiffuse.size());
      std::memcpy(buffer + offset[7], hud.lightDirection.data(), sizeof(vec4) * hud.lightDirection.size());
      std::memcpy(buffer + offset[8], hud.lightAngle.data(), sizeof(vec4) * hud.lightAngle.size());
      std::memcpy(buffer + offset[9], &hud.MatAmbient, sizeof(hud.MatAmbient));
      std::memcpy(buffer + offset[10], &hud.Attenuation, sizeof(hud.Attenuation));
      std::memcpy(buffer + offset[11], &hud.FogColor, sizeof(hud.FogColor));
      std::memcpy(buffer + offset[12], &hud.GlobalAmbient, sizeof(hud.GlobalAmbient));
      std::memcpy(buffer + offset[13], &NearfarPlane, sizeof(NearfarPlane));
      std::memcpy(buffer + offset[14], &viewvec, sizeof(viewvec));

      glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);
      glBufferData(GL_UNIFORM_BUFFER, uboSize, buffer, GL_DYNAMIC_DRAW);
      glBindBufferBase(GL_UNIFORM_BUFFER, uboIndex, uboHandle);

    }

    //toggle vertex normal
    // int vn_state = glfwGetKey(window, GLFW_KEY_4);
    // if (vn_state == GLFW_PRESS && oldState[1] == GLFW_RELEASE)
    //   hud.vn_bool = !hud.vn_bool;
    // oldState[1] = vn_state;

    // toggle the face normal 
    // int fn_state = glfwGetKey(window, GLFW_KEY_5);
    // if (fn_state == GLFW_PRESS && oldState[2] == GLFW_RELEASE)
    //   hud.fn_bool = !hud.fn_bool;
    // oldState[2] = fn_state;

    // calculate the new direction of the camera which it is facing
    vec3 new_direction = rotateY(rotateX(direction, static_cast<float>(current_y - mouse_y) * -0.01f),
                                         static_cast<float>(current_x - mouse_x) * -0.01f);
    // calculate the diff of mouse pos
    double diff_x = current_x - mouse_x;
    double diff_y = current_y - mouse_y;

    // toggle the camera movement
    // int cam_state = glfwGetKey(window, GLFW_KEY_ENTER);
    // if (cam_state == GLFW_PRESS && oldState[3] == GLFW_RELEASE)
    //   hud.camera_movement = !hud.camera_movement;
    // oldState[3] = cam_state;

    // toggle mesh drawing
    // if (glfwGetKey(window, GLFW_KEY_1)) hud.draw_state = 0;
    // toggle wireframe drawing
    // if (glfwGetKey(window, GLFW_KEY_2)) hud.draw_state = 1;
    // toggle points drawing
    // if (glfwGetKey(window, GLFW_KEY_3)) hud.draw_state = 2;

    // camera movement
    if (hud.camera_movement && !hud.camera_state)
    {
      glfwGetCursorPos(window, &current_x, &current_y);
      new_direction = rotateY(rotateX(direction, static_cast<float>(angle_y) * -0.01f),
                                      static_cast<float>(angle_x) * -0.01f);
      vec3 right_vector = cross(new_direction, vec3(0.f, 1.f, 0.f));
      right_vector = normalize(right_vector);
      // A and D to move left and right respectively
      if (glfwGetKey(window, GLFW_KEY_S)) hud.cameraPos -= right_vector * 0.005f;
      if (glfwGetKey(window, GLFW_KEY_F)) hud.cameraPos += right_vector * 0.005f;
      // W and S to move forward and back respectively
      if (glfwGetKey(window, GLFW_KEY_E)) hud.cameraPos += normalize(new_direction) * 0.005f;
      if (glfwGetKey(window, GLFW_KEY_D)) hud.cameraPos -= normalize(new_direction) * 0.005f;
      // Q and E to move the top and down repectively
      if (glfwGetKey(window, GLFW_KEY_W)) hud.cameraPos.y += 0.005f;
      if (glfwGetKey(window, GLFW_KEY_R)) hud.cameraPos.y -= 0.005f;
      // reset the camera to center
      if (glfwGetKey(window, GLFW_KEY_SPACE)) hud.cameraPos = vec3();
      if (glfwGetKey(window, GLFW_KEY_Q)) angle_x += 0.5f;
      if (glfwGetKey(window, GLFW_KEY_A)) angle_x -= 0.5f;
      if (glfwGetKey(window, GLFW_KEY_T)) angle_y += 0.5f;
      if (glfwGetKey(window, GLFW_KEY_G)) angle_y -= 0.5f;
    }
    else
    {
      angle_x = 0.f;
      angle_y = 0.f;
      // if camera movement is disabled
      glfwGetCursorPos(window, &current_x, &current_y);
      mouse_x = current_x - diff_x;
      mouse_y = current_y - diff_y;
    }
    // MVP matrix (uniform matrix)
    mat4 viewMatrix;
    // projection matrix
    projectionMatrix = perspective(45.f, 1024.f / 768.f, 0.001f, 100.f);
    // switch between different views
    if (!hud.camera_rotation)
    {
      switch (hud.camera_state)
      {
        // from the front

        case 1:
        {
          hud.cameraPos = vec3(0, 0, 5);
          viewMatrix = glm::lookAt(
            hud.cameraPos, // eye
            vec3(0, 0, 0),
            vec3(0, 1, 0)  // up vector (set to 0,-1,0 to look upside-down)
          );
          break;
        }
        // from the side
        case 2:
        {
          hud.cameraPos = vec3(5, 0, 0);
          viewMatrix = glm::lookAt(
            hud.cameraPos,
            vec3(0, 0, 0),
            vec3(0, 1, 0)
          );
          break;
        }
        // free control of the camera
        default:
        {
          viewMatrix = glm::lookAt(
            hud.cameraPos, // eye
            hud.cameraPos + new_direction, // tgt
            vec3(0, 1, 0)  // up vector (set to 0,-1,0 to look upside-down)
          );
          break;
        }
      }
    }
    else
    {
      viewMatrix = glm::lookAt(
        hud.cameraPos,
        vec3(0, 0, 0),
        vec3(0, 1, 0)
      );
    }
    
    mat4 modelMatrix = mat4(1.f);
    mat4 skyBoxMatrix = mat4(1.f);
    skyBoxMatrix = glm::translate(vec3(hud.cameraPos.x, hud.cameraPos.y, hud.cameraPos.z)) * glm::scale(vec3(10.f, 10.f, 10.f));
    mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;
    GLuint vTransformLoc;
    if (hud.EnvMap)
    {
      UseFBO(0);
      glDrawBuffer(GL_BACK_LEFT);

      glUseProgram(SkyBoxID);

      glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[0]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(skyBoxVertices), skyBoxVertices, GL_STATIC_DRAW);

      glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[4]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(skyBoxUVs), skyBoxUVs, GL_STATIC_DRAW);

      glDisable(GL_DEPTH_TEST);

      // concate the mvp matrix
      MVP = projectionMatrix * viewMatrix * skyBoxMatrix;
      // get the uniform location of the vertex transform
      vTransformLoc = glGetUniformLocation(SkyBoxID, "vertexTransform");
      // set the mvp to the uniform vertex transform
      glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &MVP[0][0]);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, skyboxTexture[4]);
      glDrawArrays(GL_TRIANGLES, 12, 6);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, skyboxTexture[5]);
      glDrawArrays(GL_TRIANGLES, 18, 6);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, skyboxTexture[3]);
      glDrawArrays(GL_TRIANGLES, 24, 6);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, skyboxTexture[2]);
      glDrawArrays(GL_TRIANGLES, 30, 6);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, skyboxTexture[1]);
      glDrawArrays(GL_TRIANGLES, 0, 6);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, skyboxTexture[0]);
      glDrawArrays(GL_TRIANGLES, 6, 6);

      MVP = projectionMatrix * viewMatrix * modelMatrix;

      ///////////////////////////////////////////////////////////////////////////////////////////////////

      UseFBO(FBOHandle);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

      for (int i = 0; i < 6; ++i)
      {
        vec3 black = vec3(0.f);

        glUseProgram(EnvMapID);

        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyBoxVertices), skyBoxVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[4]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyBoxUVs), skyBoxUVs, GL_STATIC_DRAW);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturebuffer[0]);

        glBindTexture(GL_TEXTURE_2D, renderedTexture[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture[i], 0);
        GLenum drawbuffers[] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, drawbuffers);
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR);

        mat4 tmpModel = mat4(1.f);
        tmpModel = glm::translate(vec3(hud.cameraPos.x, hud.cameraPos.y, hud.cameraPos.z)) * glm::scale(vec3(10.f, 10.f, 10.f));

        mat4 tmpProj = perspective(static_cast<float>(M_PI / 2.f), 1.f, 0.001f, 100.f);
        vec3 lookDirection;
        vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
        if (i == 0)
        {
          cameraUp = vec3(0, 0, 1);
          lookDirection = vec3(0, 1, 0);
        }
        if (i == 1)
        {
          cameraUp = vec3(0, 0, -1);
          lookDirection = vec3(0, -1, 0);
        }
        if (i == 2)
          lookDirection = vec3(-1, 0, 0);
        if (i == 3)
          lookDirection = vec3(1, 0, 0);
        if (i == 4)
          lookDirection = vec3(0, 0, 1);
        if (i == 5)
          lookDirection = vec3(0, 0, -1);

        mat4 tmpView = glm::lookAt(
          vec3(0, 0, 0),
          lookDirection,
          cameraUp
        );
        mat4 tmpMVP = tmpProj * tmpView * tmpModel;
        // get the uniform location of the vertex transform
        GLuint tmpLoc = glGetUniformLocation(EnvMapID, "matrix");
        // set the mvp to the uniform vertex transform
        glUniformMatrix4fv(tmpLoc, 1, GL_FALSE, &tmpMVP[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skyboxTexture[4]);
        glDrawArrays(GL_TRIANGLES, 12, 6);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skyboxTexture[5]);
        glDrawArrays(GL_TRIANGLES, 18, 6);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skyboxTexture[3]);
        glDrawArrays(GL_TRIANGLES, 24, 6);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skyboxTexture[2]);
        glDrawArrays(GL_TRIANGLES, 30, 6);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skyboxTexture[1]);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skyboxTexture[0]);
        glDrawArrays(GL_TRIANGLES, 6, 6);

        mat4 circleMatrix = mat4(1.f);


        glUseProgram(LightID);
        for (int j = 0; j < 8; ++j)
        {
          // get the lightColor
          GLint lightColorLoc = glGetUniformLocation(LightID, "lightColor");
          if (hud.lightActiveness[j] == 0)
            // set the lightColor
            glUniform3fv(lightColorLoc, 1, value_ptr(hud.lightDiffuse[j]));
          else glUniform3fv(lightColorLoc, 1, value_ptr(black));

          // set the new model matrix
          circleMatrix = mat4(1.f);

          // generate the position of the spheres
          circleMatrix[3][0] = hud.lightPosition[j].x;
          circleMatrix[3][1] = hud.lightPosition[j].y;
          circleMatrix[3][2] = hud.lightPosition[j].z;
          // concate the mvp matrix
          tmpMVP = tmpProj * tmpView * circleMatrix;
          // get the unform matrix
          vTransformLoc = glGetUniformLocation(LightID, "vertexTransform");
          // set the mvp matrix
          glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &tmpMVP[0][0]);

          // bind and draw the spheres
          glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[0]);
          glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * object.GetVertices().size(), object.GetVertices().data(), GL_STATIC_DRAW);
          glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[2]);
          glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * object.GetNormals().size(), object.GetNormals().data(), GL_STATIC_DRAW);
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexbuffers[3]);
          glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * object.GetIndices().size(), object.GetIndices().data(), GL_STATIC_DRAW);

          glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(object.GetIndices().size()), GL_UNSIGNED_INT, 0);
        }
      }

      ///////////////////////////////////////////////////////////////////////////////////////////////////

      glEnable(GL_DEPTH_TEST);
    }
    projectionMatrix = perspective(45.0f, 1024.f / 768.f, 0.001f, 100.f);

    // binding the position array buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // binding the color array buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * color.size(), color.data(), GL_STATIC_DRAW);

    // binding the normal array buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * normals.size(), normals.data(), GL_STATIC_DRAW);

    // binding the indices array buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexbuffers[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // binding the uv array buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[4]);
    if (hud.UV_selection == 0)
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cylindrical_uv.size(), cylindrical_uv.data(), GL_STATIC_DRAW);
    else if (hud.UV_selection == 1)
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * spherical_uv.size(), spherical_uv.data(), GL_STATIC_DRAW);
    else if (hud.UV_selection == 2)
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cubic_uv.size(), cubic_uv.data(), GL_STATIC_DRAW);
    else if (hud.UV_selection == 3)
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cylindrical_normal.size(), cylindrical_normal.data(), GL_STATIC_DRAW);
    else if (hud.UV_selection == 4)
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * spherical_normal.size(), spherical_normal.data(), GL_STATIC_DRAW);
    else
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cubic_normal.size(), cubic_normal.data(), GL_STATIC_DRAW);

    if (!hud.EnvMap)
    {
      // Use our shader
      glUseProgram(shader);

      // get the uniform location of the vertex transform
      vTransformLoc = glGetUniformLocation(shader, "vertexTransform");
      // set the mvp to the uniform vertex transform
      glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &MVP[0][0]);
      // get the uniform location of the vertex transform
      GLuint worldLoc = glGetUniformLocation(shader, "worldTransform");
      // set the mvp to the uniform vertex transform
      glUniformMatrix4fv(worldLoc, 1, GL_FALSE, &modelMatrix[0][0]);
      // get the camera pos
      GLuint cameraTransformLoc = glGetUniformLocation(shader, "cameraPosition");
      // set the camera pos
      glUniform3fv(cameraTransformLoc, 1, value_ptr(hud.cameraPos));

      // switch the drawing type from wireframe to mesh to points
      if (!hud.draw_state) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      else if (hud.draw_state == 1) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      else glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

      UseFBO(0);
      glDrawBuffer(GL_BACK_LEFT);

      // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, texturebuffer[0]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, texturebuffer[1]);

      // draw the obj mesh
      glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    }
    if (!hud.draw_state) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else if (hud.draw_state == 1) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    // Clear the screen
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    if (hud.EnvMap)
    {
      UseFBO(0);
      glDrawBuffer(GL_BACK_LEFT);

      glUseProgram(EnvShader);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, renderedTexture[0]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, renderedTexture[1]);
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, renderedTexture[2]);
      glActiveTexture(GL_TEXTURE3);
      glBindTexture(GL_TEXTURE_2D, renderedTexture[3]);
      glActiveTexture(GL_TEXTURE4);
      glBindTexture(GL_TEXTURE_2D, renderedTexture[4]);
      glActiveTexture(GL_TEXTURE5);
      glBindTexture(GL_TEXTURE_2D, renderedTexture[5]);

      glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[0]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

      glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[2]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * normals.size(), normals.data(), GL_STATIC_DRAW);

      // binding the indices array buffer
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexbuffers[3]);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

      // binding the uv array buffer
      glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[4]);
      if (hud.UV_selection == 0)
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cylindrical_uv.size(), cylindrical_uv.data(), GL_STATIC_DRAW);
      else if (hud.UV_selection == 1)
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * spherical_uv.size(), spherical_uv.data(), GL_STATIC_DRAW);
      else if (hud.UV_selection == 2)
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cubic_uv.size(), cubic_uv.data(), GL_STATIC_DRAW);
      else if (hud.UV_selection == 3)
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cylindrical_normal.size(), cylindrical_normal.data(), GL_STATIC_DRAW);
      else if (hud.UV_selection == 4)
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * spherical_normal.size(), spherical_normal.data(), GL_STATIC_DRAW);
      else
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cubic_normal.size(), cubic_normal.data(), GL_STATIC_DRAW);

      vTransformLoc = glGetUniformLocation(EnvShader, "vertexTransform");
      glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &MVP[0][0]);
      GLuint worldLoc = glGetUniformLocation(EnvShader, "worldTransform");
      // set the mvp to the uniform vertex transform
      glUniformMatrix4fv(worldLoc, 1, GL_FALSE, &modelMatrix[0][0]);
      GLuint reflectTransformLoc = glGetUniformLocation(EnvShader, "cameraPosition");
      // set the camera pos
      glUniform3fv(reflectTransformLoc, 1, value_ptr(hud.cameraPos));
      // Add Emissive if the shader is the emissive shader
      if (EnvShader == FSQEmissive)
      {
        vec3 emissive_Light = vec3(hud.lightEmissive.x, hud.lightEmissive.y, hud.lightEmissive.z);
        // set the emissive light
        GLuint emissiveLoc = glGetUniformLocation(EnvShader, "Emissive");
        glUniform3fv(emissiveLoc, 1, value_ptr(emissive_Light));
      }
      if (EnvShader == RefractionID || EnvShader == FresnelID)
      {
        GLuint ntLoc = glGetUniformLocation(EnvShader, "Nt");
        glUniform1f(ntLoc, hud.NT);
      }


      glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    }
    // Switch back to Normal to draw the hud

    vec3 black = vec3(0.f);
    vec3 white = vec3(1.f);

	  glUseProgram(LightID);

    GLint lightColorLoc = glGetUniformLocation(LightID, "lightColor");
    // set the lightColor
    glUniform3fv(lightColorLoc, 1, value_ptr(white));

	  vTransformLoc = glGetUniformLocation(LightID, "vertexTransform");
	  // set the mvp to the uniform vertex transform
	  glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &MVP[0][0]);

	  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[2]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * normal_clr.size(), normal_clr.data(), GL_STATIC_DRAW);

    // if vertex normal is toggled, draw the vertex normal
    if (hud.vn_bool)
    {
      glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[0]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_normal.size(), vertex_normal.data(), GL_STATIC_DRAW);
      
      glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertex_normal.size()));
    }
    // if the face normal is toogled, draw the face normal
    if (hud.fn_bool)
    {
      glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[0]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * face_normal.size(), face_normal.data(), GL_STATIC_DRAW);

      glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(face_normal.size()));
    }
    //orbit drawing
    mat4 circleMatrix = mat4(1.f);
    // concate the mvp matrix with model as identity
    MVP = projectionMatrix * viewMatrix * circleMatrix;
	// get the lightColor

    // get the uniform vertex transform matrix
    vTransformLoc = glGetUniformLocation(LightID, "vertexTransform");
    // set the matrix
    glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &MVP[0][0]);
    // bind the orbit position vector and draw
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * orbit_vertices.size(), orbit_vertices.data(), GL_STATIC_DRAW);

    glDrawArrays(GL_LINE_LOOP, 0, static_cast<GLsizei>(orbit_vertices.size() / 3));

    // sphere drawing
    for (int i = 0; i < 8; ++i)
    {
	  // get the lightColor
	  GLint lightColorLoc = glGetUniformLocation(LightID, "lightColor");
	  if(hud.lightActiveness[i] == 0)
	    // set the lightColor
		glUniform3fv(lightColorLoc, 1, value_ptr(hud.lightDiffuse[i]));
	  else glUniform3fv(lightColorLoc, 1, value_ptr(black));
	  
      // set the new model matrix
      circleMatrix = mat4(1.f);

      // generate the position of the spheres
      circleMatrix[3][0] = hud.lightPosition[i].x;
      circleMatrix[3][1] = hud.lightPosition[i].y;
      circleMatrix[3][2] = hud.lightPosition[i].z;
      // concate the mvp matrix
      MVP = projectionMatrix * viewMatrix * circleMatrix;
      // get the unform matrix
      vTransformLoc = glGetUniformLocation(LightID, "vertexTransform");
      // set the mvp matrix
      glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &MVP[0][0]);

      // bind and draw the spheres
      glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[0]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * object.GetVertices().size(), object.GetVertices().data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[2]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * object.GetNormals().size(), object.GetNormals().data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexbuffers[3]);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * object.GetIndices().size(), object.GetIndices().data(), GL_STATIC_DRAW);

      glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(object.GetIndices().size()), GL_UNSIGNED_INT, 0);
    }

    // update the hud
    hud.Update();

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  } // Check if the ESC key was pressed or the window was closed
  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0);

  // Cleanup VBO
  glDeleteFramebuffers(1, &FBOHandle);
  glDeleteTextures(6, renderedTexture);
  glDeleteTextures(6, skyboxTexture);
  glDeleteBuffers(5, vertexbuffers);
  glDeleteVertexArrays(1, &VertexArrayID);
  glDeleteProgram(programID);
  glDeleteProgram(LightID);
  glDeleteProgram(FSQProgramID);
  glDeleteProgram(FSQEmissive);
  glDeleteProgram(RefractionID);
  glDeleteProgram(FresnelID);
  glDeleteProgram(vertexLightingID);
  glDeleteProgram(blinnLightingID);
  glDeleteProgram(EnvMapID);
  glDeleteProgram(SkyBoxID);
  glDeleteTextures(2, texturebuffer);
  glDeleteBuffers(1, &uboHandle);
  glDeleteRenderbuffers(1, &depthrenderbuffer);
  if (buffer != nullptr) delete[](buffer);

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}