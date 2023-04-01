#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <omp.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../libs/stb_image_write.h"

#define CHANNEL_NUM 3
#define EXECUTE_NUM 100

void rgbaToRgb(unsigned char* img, unsigned char* rgb, const int& n, const int& channels) {
	int j = -1;

	for (int i = 0; i < n; i += channels) {
		rgb[++j] = img[i];
		rgb[++j] = img[i + 1];
		rgb[++j] = img[i + 2];
	}
}

__global__ void calculateIndex(int* indexes, const int& i, const int& channels, const int& width,
		const int& n, const int& filterSize, unsigned int* rgb) {
	int j = blockIdx.x * blockDim.x + threadIdx.x;
	int k = blockIdx.y * blockDim.y + threadIdx.y;

	int index = i + width * (j - filterSize / 2) * CHANNEL_NUM + (k - -filterSize / 2) * CHANNEL_NUM;

	if (index > -1 && index < n) {
		int to = (j + filterSize / 2) * filterSize + k + filterSize / 2;
		indexes[to] = rgb[index];
	}
}

void medianFilter(unsigned char* rgb, unsigned char* output, const int& n, const int& width,
		const int& filterSize) {

	const int shift = (filterSize / 2);
	int median, l, size;
	int varSize = sizeof(int) * filterSize;

	int heighbourNum = filterSize * filterSize + 1;
	int* lindex = new int[heighbourNum];

	for (int i = 0; i < n; ++i) {
		int* d_lindex = new int[heighbourNum];

		cudaMalloc((void**) &d_lindex, varSize);

		dim3 threadsPerBlock(filterSize, filterSize);
		dim3 numBlocks(filterSize / threadsPerBlock.x, filterSize / threadsPerBlock.y);
		calculateIndex<<<numBlocks, threadsPerBlock>>>(d_lindex, i, CHANNEL_NUM, width, n, filterSize);

		cudaMemcpy(lindex, d_lindex, varSize, cudaMemcpyHostToDevice);

		size = heighbourNum;
		std::sort(lindex, lindex + heighbourNum);

		median = size % 2 ? lindex[size / 2] : (lindex[size / 2 - 1] + lindex[size / 2]) / 2;
		rgb[i] = median;
	}
}

__global__ void negative(unsigned char* rgb, unsigned char* output) {
	int i = threadIdx.x;
	output[i] = 255 - rgb[i];
}

void negativeFilter(unsigned char* rgb, unsigned char* output, int n) {
	unsigned char* d_rgb, * d_output;
	int size = (sizeof(unsigned char)) * n;

	cudaMalloc((void**) &d_rgb, size);
	cudaMalloc((void**) &d_output, size);

	cudaMemcpy(d_rgb, rgb, size, cudaMemcpyHostToDevice);

	negative<<<1, n>>>(d_rgb, d_output);

	cudaMemcpy(output, d_output, size, cudaMemcpyHostToDevice);

	cudaFree(d_rgb);
	cudaFree(d_output);
}

int openClmain() {
	int width, height, channels;

	unsigned char* img = stbi_load("./images/base/300x300.png", &width, &height, &channels, 0);

	int n = width * height * CHANNEL_NUM;
	const int size = sizeof(unsigned char) * n;

	unsigned char* rgb = new unsigned char[n];
	unsigned char* output = new unsigned char[n];

	if (channels > CHANNEL_NUM) {
		rgb = new unsigned char[n];
		rgbaToRgb(img, rgb, width * height * channels, channels);
	} else {
		rgb = img;
	}

	double startTime = omp_get_wtime();

	for (int i = 0; i < EXECUTE_NUM; ++i) {
		negativeFilter(rgb, output, n);
		medianFilter(rgb, output, n, width, 7);
	}

	double endTime = omp_get_wtime();
	double executeTime = (endTime - startTime) / EXECUTE_NUM;

	std::cout << width << "x" << width << "px - " << executeTime << "\n";

	stbi_write_png("./images/result/300x300.png", width, height, CHANNEL_NUM, output, width * CHANNEL_NUM);
	stbi_image_free(img);

	return 0;
}
