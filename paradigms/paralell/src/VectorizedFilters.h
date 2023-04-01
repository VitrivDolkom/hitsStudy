#ifndef VECTORIZEDFILTERS_H_
#define VECTORIZEDFILTERS_H_

#include <algorithm>
#include <vector>
#include <x86intrin.h>
#include <bits/stdc++.h>

using namespace std;

void medianFilterVectorized(unsigned char* rgb, const int& n, const int& width, const int& filterSize) {
	const int shift = (filterSize / 2);
	int size, median, i;

	for (i = 0; i < n; ++i) {
		std::vector<__m128i> localityIndexes;
		std::vector<unsigned char> pixels;

		for (int j = -shift; j <= shift; j++) {
			__m128i jVector = _mm_set1_epi32(j);
			__m128i iVector = _mm_set1_epi32(i);
			__m128i channelNumVector = _mm_set1_epi32(CHANNEL_NUM);
			__m128i widthVector = _mm_set1_epi32(width);

			for (int k = -shift; k <= shift; k += 4) {
				__m128i kVector = _mm_set1_epi32(k);
				__m128i lVector = _mm_add_epi32(
						_mm_add_epi32(iVector,
								_mm_mul_epu32(_mm_mul_epu32(jVector, channelNumVector), widthVector)),
						_mm_mul_epu32(kVector, channelNumVector));

				int32_t index = _mm_extract_epi32(lVector, 0);

				if (index > -1 && index < n) {
					pixels.push_back(rgb[index]);
				}

				localityIndexes.push_back(lVector);
			}
		}

		size = pixels.size();
		std::sort(pixels.begin(), pixels.end());

		median = size % 2 ? pixels[size / 2] : (pixels[size / 2 - 1] + pixels[size / 2]) / 2;
		rgb[i] = median;

		localityIndexes.clear();
		localityIndexes.shrink_to_fit();

		pixels.clear();
		pixels.shrink_to_fit();
	}
}

void negativeFilterVectorized(unsigned char* rgb, const int& width, const int& height) {
	const int RGB_MAX = 255;
	const int n = width * height * CHANNEL_NUM;
	__m128i* pRgb = reinterpret_cast<__m128i* >(rgb);
	int i;

	for (i = 0; i < n / 16; ++i) {
		__m128i rgbMax = _mm_set1_epi8(RGB_MAX);
		__m128i negative = _mm_load_si128(pRgb + i);
		negative = _mm_sub_epi8(rgbMax, negative);
		_mm_store_si128(pRgb + i, negative);
	}
}

#endif /* VECTORIZEDFILTERS_H_ */
