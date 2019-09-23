#include "Parser.hpp" // headers
#include <iostream> // std::cout
#include <fstream> // std::ifstream
#include <string> // std::getline
#include <algorithm> // std::unique

#define M_PI   3.14159265358979323846264338327950288f

int ParseObj(std::vector<GLuint>* indices, std::vector<GLfloat>* vertices, std::vector<GLfloat>* normals, 
	           std::vector<GLfloat>* color, float& scale, vec3& center, 
             std::vector<GLfloat>* fn, std::vector<GLfloat>* vn, 
             std::vector<GLfloat>* cylin_uv, std::vector<GLfloat>* sphere_uv, 
             std::vector<GLfloat>* cubic_uv, std::vector<GLfloat>* cylin_norm,
             std::vector<GLfloat>* sphere_norm, std::vector<GLfloat>* cubic_norm,
             const char* file)
{
  // set of normals
  std::vector<std::vector<vec3>> normalSet;
  // the max and mins
  float max[3];
  float min[3];
  for (int k = 0; k < 3; ++k)
  {
	max[k] = -1.f * std::numeric_limits<float>::infinity();
	min[k] = std::numeric_limits<float>::infinity();
  }
  // opening obj files
  std::ifstream obj_file;
  obj_file.open(file);
  // if cannot open obj
  if (!obj_file.is_open())
  {
    std::string err = "Impossible to open ";
    err += file;
    std::cout << err << std::endl;
    return 0;
  }
  // content of the string
  std::string content;
  while (std::getline(obj_file, content))
  {
    // reading in the vertices
    if (content[0] == 'v' && content[1] != 'n')
    {
      while (!content.empty())
      {
        size_t pos = content.find_first_of(" \t");
        size_t end = content.find_first_of(" \t//", pos + 1) - 1;
        std::string val = content.substr(pos + 1, end);
        std::string::size_type par = val.find("\\par");
        if (par != std::string::npos) val.erase(par, 4);
        float value = std::stof(val);
        content.erase(0, end + 1);
        vertices->push_back(value);
      }
	  normalSet.push_back(std::vector<vec3>{});
    }
    // reading in the indices
    else if(content[0] == 'f')
    {
      unsigned count = 0;
      std::vector<unsigned> ListOfVertices;
      while (!content.empty())
      {
        size_t pos = content.find_first_of(" \t");
        size_t end = content.find_first_of(" \t", pos + 1) - 1;
        std::string val = content.substr(pos + 1, end);
        std::string::size_type par = val.find("\\par");
        if (par != std::string::npos) val.erase(par, 4);
        unsigned value = std::stoi(val);
        content.erase(0, end + 1);
        ListOfVertices.push_back(value - 1);
        ++count;
      }
      if (count >= 3)
      {
        // triangulating the indices
        for (unsigned i = 0; i < (count - 2); ++i)
        {
		      vec3 pt[3];
          indices->push_back(ListOfVertices[0]);
          indices->push_back(ListOfVertices[i + 1]);
          indices->push_back(ListOfVertices[i + 2]);
		      unsigned tri_1 = ListOfVertices[0] * 3;
		      unsigned tri_2 = ListOfVertices[i + 1] * 3;
		      unsigned tri_3 = ListOfVertices[i + 2] * 3;
		      pt[0] = vec3(vertices->operator[](tri_1), vertices->operator[](tri_1 + 1), vertices->operator[](tri_1 + 2));
		      pt[1] = vec3(vertices->operator[](tri_2), vertices->operator[](tri_2 + 1), vertices->operator[](tri_2 + 2));
		      pt[2] = vec3(vertices->operator[](tri_3), vertices->operator[](tri_3 + 1), vertices->operator[](tri_3 + 2));
          vec3 vec_2 = pt[2] - pt[0];
		      vec3 vec_1 = pt[1] - pt[0];
		      vec3 norm = cross(vec_1, vec_2);
          norm = normalize(norm);
          // calculating the face normals
          vec3 centeroid = pt[0] + pt[1] + pt[2];
          centeroid /= 3.f;
          fn->push_back(centeroid.x);
          fn->push_back(centeroid.y);
          fn->push_back(centeroid.z);
          fn->push_back(centeroid.x + norm.x);
          fn->push_back(centeroid.y + norm.y);
          fn->push_back(centeroid.z + norm.z);
          // push into tmp vertex buffer
          std::vector<vec3>* norm_set = &(normalSet[ListOfVertices[0]]);
		      norm_set->push_back(norm);
		      norm_set = &(normalSet[ListOfVertices[i + 1]]);
		      norm_set->push_back(norm);
		      norm_set = &(normalSet[ListOfVertices[i + 2]]);
		      norm_set->push_back(norm);
          // calculating the biggest and smallest x,y,z, 
		      for (int j = 0; j < 3; ++j)
		      {
			      if (pt[j].x > max[0]) max[0] = pt[j].x;
			      if (pt[j].x < min[0]) min[0] = pt[j].x;
			      if (pt[j].y > max[1]) max[1] = pt[j].y;
			      if (pt[j].y < min[1]) min[1] = pt[j].y;
			      if (pt[j].z > max[2]) max[2] = pt[j].z;
			      if (pt[j].z < min[2]) min[2] = pt[j].z;
		      }
        }
      }
    }
    content.clear();
  }
  // create the bounding box and generate the center
  center = vec3((max[0] + min[0]) / 2.f, (max[1] + min[1]) / 2.f, (max[2] + min[2]) / 2.f);
  // find the biggest x
  float x = abs(max[0] - center.x);
  if (abs(min[0] - center.x) > x) x = abs(min[0] - center.x);
  // find the biggest y
  float y = abs(max[1] - center.y);
  if (abs(min[1] - center.y) > y) y = abs(min[1] - center.y);
  // find the biggest z
  float z = abs(max[2] - center.z);
  if (abs(min[2] - center.z) > z) z = abs(min[2] - center.z);

  // find the largest x, y, z and set the scale 
  scale = x;
  if (y > scale) scale = y;
  if (z > scale) scale = z;

  // transform the center to the after scaled position
  center /= scale;

  // normalize all the points
  for (unsigned i = 0; i < vertices->size(); ++i)
  {
    (*vertices)[i] /= scale;
    if ((i % 3) == 0) (*vertices)[i] -= center.x;
    else if ((i % 3) == 1) (*vertices)[i] -= center.y;
    else (*vertices)[i] -= center.z;
  }

  // generate and push back the vertex normal
  for (unsigned i = 0; i < normalSet.size(); ++i)
  {
	  std::vector<vec3>* norm_set = &(normalSet[i]);
    auto last = std::unique(norm_set->begin(), norm_set->end());
    norm_set->erase(last, norm_set->end());
    vec3 total{};
	  for (auto it = norm_set->begin(); it != norm_set->end(); ++it)
	  {
		  total += *it;
	  }
    total /= norm_set->size();
    total = normalize(total);
	  normals->push_back(total.x);
	  normals->push_back(total.y);
	  normals->push_back(total.z);

    vn->push_back((*vertices)[i * 3]);
    vn->push_back((*vertices)[i * 3 + 1]);
    vn->push_back((*vertices)[i * 3 + 2]);
    vn->push_back((*vertices)[i * 3] +  total.x * 0.05f);
    vn->push_back((*vertices)[i * 3 + 1] + total.y * 0.05f);
    vn->push_back((*vertices)[i * 3 + 2] + total.z * 0.05f);
  }
  // generate and push back the face normal
  for (unsigned i = 3; i < fn->size(); i += 6)
  {
    fn->operator[](i) = ((fn->operator[](i) - fn->operator[](i - 3)) *  0.05f * scale) +
                          fn->operator[](i - 3);
    fn->operator[](i + 1) = ((fn->operator[](i + 1) - fn->operator[](i - 2)) *  0.05f * scale) +
                              fn->operator[](i - 2);
    fn->operator[](i + 2) = ((fn->operator[](i + 2) - fn->operator[](i - 1)) *  0.05f * scale) +
                              fn->operator[](i - 1);
  }
  
  // calculate the uvs here
  for (unsigned i = 0; i < vertices->size(); i += 3)
  {
    vec3 vector;
    vector.x = vertices->operator[](i);
    vector.y = vertices->operator[](i + 1);
    vector.z = vertices->operator[](i + 2);
    // cylinderical uv
    float theta = atan2f(-vector.z, vector.x) / (2 * M_PI);
    cylin_uv->push_back(theta + 0.5f);
    cylin_uv->push_back((vector.y * 0.5f) + 0.5f);
    // spherical uv
    sphere_uv->push_back(theta + 0.5f);
    float radius = length(vector);
    float phi = acosf(vector.y / radius) / M_PI;
    sphere_uv->push_back(1.f - phi);
    // cubic uv
    float u = 0.f;
    float v = 0.f;
    float abs_x = fabs(vector.x);
    float abs_y = fabs(vector.y);
    float abs_z = fabs(vector.z);
    float max = abs_x;
    int side = 0; // 0 = X, 1 = Y, 2 = Z
    if (max < abs_y)
    {
      max = abs_y;
      side = 1;
    }
    if (max < abs_z)
    {
      max = abs_z;
      side = 2;
    }
    switch (side)
    {
    case 0: // +- X
      if (vector.x > 0.f) // positive X
      {
        u = (-vector.z) / max;
        v = (vector.y) / max;
      }
      else // negative X
      {
        u = (vector.z) / max;
        v = (vector.y) / max;
      }
      break;
    case 1: // +- Y
      if (vector.y > 0.f) // positive Y
      {
        u = (vector.x) / max;
        v = (-vector.z) / max;
      }
      else // negative Y
      {
        u = (vector.x) / max;
        v = (vector.z) / max;
      }
      break;
    case 2: // +- Z
      if (vector.z > 0.f) // positive Z
      {
        u = (vector.x) / max;
        v = (vector.y) / max;
      }
      else // negative Z
      {
        u = (-vector.x) / max;
        v = (vector.y) / max;
      }
      break;
    }
    cubic_uv->push_back((u + 1.f) / 2.0f);
    cubic_uv->push_back((v + 1.f) / 2.0f);
  }

  for (unsigned i = 0; i < normals->size(); i += 3)
  {
    // cubic 
    float u;
    float v;
    float x = normals->operator[](i);
    float y = normals->operator[](i + 1);
    float z = normals->operator[](i + 2);
    float abs_x = fabs(x);
    float abs_y = fabs(y);
    float abs_z = fabs(z);
    float max = abs_x;
    int side = 0; // 0 = X, 1 = Y, 2 = Z
    if (max < abs_y)
    {
      max = abs_y;
      side = 1;
    }
    if (max < abs_z)
    {
      max = abs_z;
      side = 2;
    }
    switch (side)
    {
    case 0: // +- X
      if (x > 0.f) // positive X
      {
        u = (-z) / max;
        v = (y) / max;
      }
      else // negative X
      {
        u = (z) / max;
        v = (y) / max;
      }
      break;
    case 1: // +- Y
      if (y > 0.f) // positive Y
      {
        u = (x) / max;
        v = (-z) / max;
      }
      else // negative Y
      {
        u = (x) / max;
        v = (z) / max;
      }
      break;
    case 2: // +- Z
      if (z > 0.f) // positive Z
      {
        u = (x) / max;
        v = (y) / max;
      }
      else // negative Z
      {
        u = (-x) / max;
        v = (y) / max;
      }
      break;
    }
    cubic_norm->push_back((u + 1.f) / 2.0f);
    cubic_norm->push_back((v + 1.f) / 2.0f);
    vec3 vector;
    vector.x = normals->operator[](i);
    vector.y = normals->operator[](i + 1);
    vector.z = normals->operator[](i + 2);
    // cylindrical uv
    float theta = atan2f(-vector.z, vector.x) / (2 * M_PI);
    cylin_norm->push_back(theta + 0.5f);
    cylin_norm->push_back((vector.y * 0.5f) + 0.5f);
    // spherical uv
    sphere_norm->push_back(theta + 0.5f);
    float radius = length(vector);
    float phi = acosf(vector.y / radius) / M_PI;
    sphere_norm->push_back(1.f - phi);
  }

  for (unsigned i = 0; i < fn->size(); ++i)
  {
    (*fn)[i] /= scale;
    if (i % 3 == 0) (*fn)[i] -= center.x;
    else if (i % 3 == 1) (*fn)[i] -= center.y;
    else (*fn)[i] -= center.z;
  }
  // close the ifstream file
  obj_file.close();
  // return 1
  return 1;
}

int ParseTex(std::vector<GLfloat>* diffuse, std::vector<GLfloat>* specular, 
             int& diffuse_height, int& diffuse_width,
             int& specular_width, int& specular_height,
             const char * d_texture, const char * s_texture)
{
  // opening obj files
  std::ifstream dif_file;
  dif_file.open(d_texture);
  // if cannot open obj
  if (!dif_file.is_open())
  {
    std::string err = "Impossible to open ";
    err += d_texture;
    std::cout << err << std::endl;
    return 0;
  }
  // content of the string
  std::string content;
  int d_count = 0;
  int max = 0;
  while (std::getline(dif_file, content))
  {
    if (content[0] < '0' || content[0] > '9') continue;
    while (!content.empty())
    {
      size_t end = content.find_first_of(" ");
      std::string val = content.substr(0, end);
      unsigned value = std::stoi(val);
      if(end != std::string::npos) content.erase(0, end + 1);
      else content.erase(0, end);
      if (d_count == 0) diffuse_width = value;
      else if (d_count == 1) diffuse_height = value;
      else if (d_count == 2) max = std::stoi(val);
      else if (d_count >= 3)
      {
        float normalized = static_cast<float>(value) / static_cast<float>(max);
        diffuse->push_back(normalized);
      }
      ++d_count;
    }
  }
  // sanity check
  std::cout << "Diffuse Texture contains " << d_count << " number of lines" << std::endl;
  dif_file.close();

  // opening obj files
  std::ifstream spec_file;
  spec_file.open(s_texture);
  // if cannot open obj
  if (!spec_file.is_open())
  {
    std::string err = "Impossible to open ";
    err += s_texture;
    std::cout << err << std::endl;
    return 0;
  }
  int s_count = 0;
  max = 0;
  while (std::getline(spec_file, content))
  {
    if (content[0] < '0' || content[0] > '9') continue;
    while (!content.empty())
    {
      size_t end = content.find_first_of(" ");
      std::string val = content.substr(0, end);
      unsigned value = std::stoi(val);
      if (end != std::string::npos) content.erase(0, end + 1);
      else content.erase(0, end);
      if (s_count == 0) specular_width = value;
      else if (s_count == 1) specular_height = value;
      else if (s_count == 2) max = std::stoi(val);
      else if (d_count >= 3)
      {
        float normalized = static_cast<float>(value) / static_cast<float>(max);
        specular->push_back(normalized);
      }
      ++s_count;
    }
  }
  // sanity check
  std::cout << "Specular Texture contains " << s_count << " number of lines" << std::endl;
  spec_file.close();
  return 1;
}

