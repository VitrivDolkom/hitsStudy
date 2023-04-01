#ifndef CONSECUTIVEFILTERS_H_
#define CONSECUTIVEFILTERS_H_

#include <algorithm>
#include <vector>
#include <time.h>

#include "Defines.h"

void medianFilterConsistently(unsigned char* rgb, const int& n, const int& width, const int& filterSize) {
	const int shift = (filterSize / 2);
	int median, l, size;

	for (int i = 0; i < n; ++i) {
		std::vector<unsigned char> locality;

		for (int j = -shift; j <= shift; ++j) {
			for (int k = -shift; k <= shift; ++k) {
				l = i + width * j * CHANNEL_NUM + k * CHANNEL_NUM;

				if (l > -1 && l < (n - 2)) {
					locality.push_back(rgb[l]);
				}
			}
		}

		size = locality.size();
		std::sort(locality.begin(), locality.end());

		median = size % 2 ? locality[size / 2] : (locality[size / 2 - 1] + locality[size / 2]) / 2;
		rgb[i] = median;

		locality.clear();
		locality.shrink_to_fit();
	}
}

void negativeFilterConsistently(unsigned char* rgb, const int& width, const int& height) {
	const int RGB_MAX = 255;
	const int n = width * height * CHANNEL_NUM;

	for (int i = 0; i < n; ++i) {
		rgb[i] = RGB_MAX - rgb[i];
	}
}

#endif /* CONSECUTIVEFILTERS_H_ */
