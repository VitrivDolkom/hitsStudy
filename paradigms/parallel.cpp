#include <stdint.h>
#include <iostream>

#include <string>
#include <filesystem>
#include <algorithm>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace fs = std::filesystem;

#define CHANNEL_NUM 3
#define MEDIAN_FILTER_SIZE 7

struct Pixel {
	int r, g, b, index;

	Pixel(int r, int g, int b, int index) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->index = index;
	}
};

bool comparaTwoPixels(Pixel p1, Pixel p2) {
	if (p1.r != p2.r) {
		return p1.r < p2.r;
	}

	if (p1.g != p2.g) {
		return p1.g < p2.g;
	}

	return p1.b < p2.b;
}

void medianFilter(unsigned char* rgb, int n, int width, int filterSize) {
	std::vector<Pixel> locality;
	int shift = (filterSize / 2);
	int medianIndex;

	for (int i = 0; i < n; i += CHANNEL_NUM) {
		locality = std::vector<Pixel>();

		for (int j = -shift; j <= shift; ++j) {
			for (int k = -shift; k <= shift; ++k) {
				int l = i + width * j * CHANNEL_NUM + k * CHANNEL_NUM;

				if (l > -1 && l < (n - 2)) {
					Pixel p(rgb[l], rgb[l + 1], rgb[l + 2], l);
					locality.push_back(p);
				}
			}
		}

		std::nth_element(locality.begin(), locality.begin() + locality.size() / 2, locality.end(),
				comparaTwoPixels);

		medianIndex = locality[locality.size() / 2].index;
		rgb[i] = rgb[medianIndex];
		rgb[i + 1] = rgb[medianIndex + 1];
		rgb[i + 2] = rgb[medianIndex + 2];
	}
}

void changeChannelsToThree(unsigned char* img, unsigned char* rgb, int n, int channels) {
	int j = -1;

	for (int i = 0; i < n; i += 4) {
		rgb[++j] = img[i];
		rgb[++j] = img[i + 1];
		rgb[++j] = img[i + 2];
	}
}

void negativeFilter(unsigned char* rgb, int n) {
	const int RGB_MAX = 255;

	for (int i = 0; i < n; ++i) {
		rgb[i] = RGB_MAX - rgb[i];
	}
}

void filterImage(const char* pathToImage, const char* pathToResult) {
	int width, height, channels;

	unsigned char* img = stbi_load(pathToImage, &width, &height, &channels, 0);
	int n = width * height * CHANNEL_NUM;
	unsigned char* rgb;

	if (channels > CHANNEL_NUM) {
		rgb = new unsigned char[n];
		changeChannelsToThree(img, rgb, width * height * channels, channels);
	} else {
		rgb = img;
	}

//	negativeFilter(rgb, n);
	medianFilter(rgb, n, width, MEDIAN_FILTER_SIZE);

	stbi_write_png(pathToResult, width, height, CHANNEL_NUM, rgb, width * CHANNEL_NUM);
	stbi_image_free(img);
}

std::string getFileName(std::string path) {
	return path.substr(path.find_last_of("/\\") + 1);
}

int main() {
	std::string baseFolderPath = "./images/base/";
	std::string resultFolderPath = "./images/result/";
	std::string pathToResultImage, fileName;

//	filterImage("./images/base/400x400.png", "./images/result/400x400.png");
//	filterImage("./images/base/300x300.png", "./images/result/300x300.png");

	for (const auto& file : fs::directory_iterator(baseFolderPath)) {
		fileName = getFileName(file.path().generic_string())
		std::cout << fileName << "\n";
		pathToResultImage = resultFolderPath + fileName;

		filterImage(fileName.c_str(), pathToResultImage.c_str());
	}

	return 0;
}
