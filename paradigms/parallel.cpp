#include <iostream>
#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

struct Pixel {
  int R, G, B;
};

Pixel** getImagePixels(unsigned char* rgbSequence, int width, int height) {
  Pixel** pixels = new Pixel*[height];

  for (int i = 0; i < width; ++i) {
    pixels[i] = new Pixel[width];
  }

  for (int i = 0; i < (width * height * 3); i += 3) {
    pixels[i / width / 3][i % width].R = rgbSequence[i];
    pixels[i / width / 3][i % width].G = int(rgbSequence[i + 1]);
    pixels[i / width / 3][i % width].B = int(rgbSequence[i + 2]);
  }

  return pixels;
}

Pixel** negativeFilter(Pixel** pixels, int width, int height) {
  Pixel** negativePixels = new Pixel*[height];

  for (int i = 0; i < width; ++i) {
    negativePixels[i] = new Pixel[width];
  }

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      negativePixels[i][j].R = 255 - pixels[i][j].R;
      negativePixels[i][j].G = 255 - pixels[i][j].G;
      negativePixels[i][j].B = 255 - pixels[i][j].B;
    }
  }

  return negativePixels;
}

void rgbSequenceFromPixels(Pixel** pixels, unsigned char* rgbSequence, int width, int height) {
  for (int i = 0; i < (width * height * 3); i += 3) {
    rgbSequence[i] = pixels[i / width / 3][i % width].R;
    rgbSequence[i + 1] = pixels[i / width / 3][i % width].G;
    rgbSequence[i + 2] = pixels[i / width / 3][i % width].B;
  }
}

void printRgbSequence(unsigned char* rgb, int n) {
  for (int i = 0; i < n; ++i) {
    std::cout << int(rgb[i]) << " ";
  }
}

void getFilteredImage(const char* pathToImage) {
  int width, height, channels;

  unsigned char* rgbSequence = stbi_load(pathToImage, &width, &height, &channels, 3);

  for (int i = 0; i < width * height * 3; ++i) {
    rgbSequence[i] = 255 - rgbSequence[i];
  }
  //	Pixel** pixels = getImagePixels(rgbSequence, width, height);
  //	Pixel** negativePixels = negativeFilter(pixels, width, height);
  //	rgbSequenceFromPixels(pixels, rgbSequence, width, height);

  stbi_write_png("./images/result/img.png", width, height, channels, rgbSequence, width * channels);

  stbi_image_free(rgbSequence);
}

int main() {
  const char* path = "./images/base/img.png";
  getFilteredImage(path);

  return 0;
}
