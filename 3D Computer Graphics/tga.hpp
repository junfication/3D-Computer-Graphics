#pragma once
#include <vector>
#include <fstream>

typedef union PixelInfo
{
  std::uint32_t Colour;
  struct
  {
    std::uint8_t R, G, B, A;
  };
} *PPixelInfo;

class Tga
{
private:
  std::vector<std::uint8_t> Pixels;
  bool ImageCompressed;
  std::uint32_t width, height, size, BitsPerPixel;

public:
  Tga(const char* FilePath);
  std::vector<std::uint8_t> GetPixels() { return this->Pixels; }
  std::uint32_t GetWidth() const { return this->width; }
  std::uint32_t GetHeight() const { return this->height; }
  bool HasAlphaChannel() { return BitsPerPixel == 32; }
};