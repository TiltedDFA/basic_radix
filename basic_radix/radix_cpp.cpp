#include "radix.hpp"

#include <utility>
#include <vector>
#include <algorithm>


auto lsd_radix_sort_cpp(std::vector<int>& arr) -> void
{
	std::vector<int> buckets(0xFULL + 1ULL, 0);
	std::vector<int> cpy_buff(arr.size(), 0);

	int filter = 0xF;
	int const num_its = 32 / 4;
	for (int i{}; i < num_its; ++i)
	{
		std::ranges::fill(cpy_buff, 0);
		std::ranges::fill(buckets, 0);
		auto const get_val = [&]<typename T>(T&& it) -> int
			{
				return (it & filter) >> 4 * i;
			};
		std::ranges::for_each(std::as_const(arr),
			[&]<typename T>(T e)
		{
			++buckets[get_val(e)];
		});
		std::ranges::for_each(buckets, 
			[&, prev = 0]<typename T>(T& e) mutable
		{
			T const last = e;
			e = prev;
			prev += last;
		});
		std::ranges::for_each(std::as_const(arr),
			[&]<typename T> (T e)
		{
			int const idx = buckets[get_val(e)]++;
			cpy_buff[idx] = e;
		});
		std::copy(cpy_buff.cbegin(), cpy_buff.cend(), arr.begin());
		filter <<= 4;
	}
}