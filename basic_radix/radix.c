#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "radix.h"

void lsd_radix_sort(int* arr, int size)
{
	int const num_its = (int)ceil(32 / 4);
	int const bucket_count = 0xF + 1;
	int* buckets = malloc(bucket_count * sizeof(int));
	int* copy_buffer = malloc(size * sizeof(int));
	int filter = 0xF;
	for (int i = 0; i < num_its; ++i)
	{
		memset(buckets, 0, bucket_count * sizeof(int));
		memset(copy_buffer, 0, size * sizeof(int));

		for (int j = 0; j < size; ++j)
		{
			int const val = (arr[j] & filter) >> i * 4;
			++buckets[val];
		}
		// 3 4 5 -> 0 3 7
		int old_bucket_val = buckets[0];
		buckets[0] = 0;
		for (int j = 1; j < bucket_count; ++j)
		{
			int prev = buckets[j];
			buckets[j] = old_bucket_val;
			old_bucket_val += prev;
		}

		for (int j = 0; j < size; ++j)
		{
			int const val = (arr[j] & filter) >> i * 4;
			int const idx = buckets[val]++;
			copy_buffer[idx] = arr[j];
		}
		memmove(arr, copy_buffer, size * sizeof(int));
		filter <<= 4;
	}

	free(copy_buffer);
	free(buckets);
}