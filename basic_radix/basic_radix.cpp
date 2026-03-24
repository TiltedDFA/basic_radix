// basic_radix.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <algorithm>
#include <limits>
#include <cstdint>
#include <random>
#include <vector>
#include <chrono>
#include <array>
#include <format>
#include "radix.hpp"

extern "C"
{
#include "radix.h"
}

auto MakeRandomInts(std::size_t const n) -> std::vector<int>
{
    static thread_local std::mt19937 rng{ std::random_device{}() };

    std::uniform_int_distribution<int> dist
    {
        0,
        std::numeric_limits<int>::max()
    };

    std::vector<int> values(n);
    for (int& value : values)
    {
        value = dist(rng);
    }

    return values;
}

auto lsd_radix_sort_cpp_f1(std::vector<int>& arr) -> void
{
    if (arr.empty())
    {
        return;
    }

    thread_local std::vector<int> tmp;
    if (tmp.size() < arr.size())
    {
        tmp.resize(arr.size());
    }

    int* src = arr.data();
    int* dst = tmp.data();
    std::size_t const n = arr.size();

    for (int shift = 0; shift < 32; shift += 4)
    {
        std::size_t counts[16]{};
        std::size_t offsets[16]{};

        for (std::size_t i = 0; i < n; ++i)
        {
            unsigned int const bucket = (static_cast<unsigned int>(src[i]) >> shift) & 0xF;
            ++counts[bucket];
        }

        for (int i = 1; i < 16; ++i)
        {
            offsets[i] = offsets[i - 1] + counts[i - 1];
        }

        for (std::size_t i = 0; i < n; ++i)
        {
            int const value = src[i];
            unsigned int const bucket = (static_cast<unsigned int>(value) >> shift) & 0xF;
            dst[offsets[bucket]++] = value;
        }

        std::swap(src, dst);
    }

    if (src != arr.data())
    {
        std::copy(src, src + n, arr.data());
    }
}

auto lsd_radix_sort_cpp_f2(std::vector<int>& arr) -> void
{
    if (arr.empty())
    {
        return;
    }

    constexpr int radix_bits = 4;
    constexpr int bucket_count = 1 << radix_bits;
    constexpr int pass_count = 32 / radix_bits;
    constexpr std::uint32_t mask = bucket_count - 1;

    thread_local std::vector<int> scratch;
    if (scratch.size() < arr.size())
    {
        scratch.resize(arr.size());
    }

    int* src = arr.data();
    int* dst = scratch.data();
    std::size_t const n = arr.size();

    std::array<std::array<std::uint32_t, bucket_count>, pass_count> counts{};
    std::array<std::array<std::uint32_t, bucket_count>, pass_count> offsets{};

    for (std::size_t i = 0; i < n; ++i)
    {
        std::uint32_t const v = static_cast<std::uint32_t>(src[i]);

        ++counts[0][(v >> 0) & mask];
        ++counts[1][(v >> 4) & mask];
        ++counts[2][(v >> 8) & mask];
        ++counts[3][(v >> 12) & mask];
        ++counts[4][(v >> 16) & mask];
        ++counts[5][(v >> 20) & mask];
        ++counts[6][(v >> 24) & mask];
        ++counts[7][(v >> 28) & mask];
    }

    for (int pass = 0; pass < pass_count; ++pass)
    {
        std::uint32_t sum = 0;
        for (int bucket = 0; bucket < bucket_count; ++bucket)
        {
            offsets[pass][bucket] = sum;
            sum += counts[pass][bucket];
        }
    }

    for (int pass = 0; pass < pass_count; ++pass)
    {
        int const shift = pass * radix_bits;
        auto pos = offsets[pass];

        for (std::size_t i = 0; i < n; ++i)
        {
            int const value = src[i];
            std::uint32_t const bucket = (static_cast<std::uint32_t>(value) >> shift) & mask;
            dst[pos[bucket]++] = value;
        }

        std::swap(src, dst);
    }

    if (src != arr.data())
    {
        std::copy_n(src, n, arr.data());
    }
}

#if defined(_MSC_VER)
#define RADIX_FORCEINLINE __forceinline
#define RADIX_RESTRICT __restrict
#else
#define RADIX_FORCEINLINE inline
#define RADIX_RESTRICT
#endif

namespace
{
    template<int Shift>
    RADIX_FORCEINLINE auto ScatterPass(
        int const* RADIX_RESTRICT src,
        int* RADIX_RESTRICT dst,
        std::size_t const n,
        std::uint32_t* RADIX_RESTRICT pos) -> void
    {
        for (std::size_t i = 0; i < n; ++i)
        {
            int const value = src[i];
            std::uint32_t const bucket =
                (static_cast<std::uint32_t>(value) >> Shift) & 0xF;
            dst[pos[bucket]++] = value;
        }
    }
}

auto lsd_radix_sort_cpp_f3(std::vector<int>& arr) -> void
{
    if (arr.empty())
    {
        return;
    }

    constexpr int pass_count = 8;
    constexpr int bucket_count = 16;

    thread_local std::vector<int> scratch;
    if (scratch.size() < arr.size())
    {
        scratch.resize(arr.size());
    }

    alignas(64) std::uint32_t counts[pass_count][bucket_count]{};

    std::size_t const n = arr.size();
    int* RADIX_RESTRICT arr_ptr = arr.data();
    int* RADIX_RESTRICT scratch_ptr = scratch.data();

    std::size_t i = 0;
    for (; i + 4 <= n; i += 4)
    {
        std::uint32_t const v0 = static_cast<std::uint32_t>(arr_ptr[i + 0]);
        std::uint32_t const v1 = static_cast<std::uint32_t>(arr_ptr[i + 1]);
        std::uint32_t const v2 = static_cast<std::uint32_t>(arr_ptr[i + 2]);
        std::uint32_t const v3 = static_cast<std::uint32_t>(arr_ptr[i + 3]);

        ++counts[0][(v0 >> 0) & 0xF];
        ++counts[1][(v0 >> 4) & 0xF];
        ++counts[2][(v0 >> 8) & 0xF];
        ++counts[3][(v0 >> 12) & 0xF];
        ++counts[4][(v0 >> 16) & 0xF];
        ++counts[5][(v0 >> 20) & 0xF];
        ++counts[6][(v0 >> 24) & 0xF];
        ++counts[7][(v0 >> 28) & 0xF];

        ++counts[0][(v1 >> 0) & 0xF];
        ++counts[1][(v1 >> 4) & 0xF];
        ++counts[2][(v1 >> 8) & 0xF];
        ++counts[3][(v1 >> 12) & 0xF];
        ++counts[4][(v1 >> 16) & 0xF];
        ++counts[5][(v1 >> 20) & 0xF];
        ++counts[6][(v1 >> 24) & 0xF];
        ++counts[7][(v1 >> 28) & 0xF];

        ++counts[0][(v2 >> 0) & 0xF];
        ++counts[1][(v2 >> 4) & 0xF];
        ++counts[2][(v2 >> 8) & 0xF];
        ++counts[3][(v2 >> 12) & 0xF];
        ++counts[4][(v2 >> 16) & 0xF];
        ++counts[5][(v2 >> 20) & 0xF];
        ++counts[6][(v2 >> 24) & 0xF];
        ++counts[7][(v2 >> 28) & 0xF];

        ++counts[0][(v3 >> 0) & 0xF];
        ++counts[1][(v3 >> 4) & 0xF];
        ++counts[2][(v3 >> 8) & 0xF];
        ++counts[3][(v3 >> 12) & 0xF];
        ++counts[4][(v3 >> 16) & 0xF];
        ++counts[5][(v3 >> 20) & 0xF];
        ++counts[6][(v3 >> 24) & 0xF];
        ++counts[7][(v3 >> 28) & 0xF];
    }

    for (; i < n; ++i)
    {
        std::uint32_t const v = static_cast<std::uint32_t>(arr_ptr[i]);

        ++counts[0][(v >> 0) & 0xF];
        ++counts[1][(v >> 4) & 0xF];
        ++counts[2][(v >> 8) & 0xF];
        ++counts[3][(v >> 12) & 0xF];
        ++counts[4][(v >> 16) & 0xF];
        ++counts[5][(v >> 20) & 0xF];
        ++counts[6][(v >> 24) & 0xF];
        ++counts[7][(v >> 28) & 0xF];
    }

    for (int pass = 0; pass < pass_count; ++pass)
    {
        std::uint32_t sum = 0;
        for (int bucket = 0; bucket < bucket_count; ++bucket)
        {
            std::uint32_t const c = counts[pass][bucket];
            counts[pass][bucket] = sum;
            sum += c;
        }
    }

    ScatterPass< 0>(arr_ptr, scratch_ptr, n, counts[0]);
    ScatterPass< 4>(scratch_ptr, arr_ptr, n, counts[1]);
    ScatterPass< 8>(arr_ptr, scratch_ptr, n, counts[2]);
    ScatterPass<12>(scratch_ptr, arr_ptr, n, counts[3]);
    ScatterPass<16>(arr_ptr, scratch_ptr, n, counts[4]);
    ScatterPass<20>(scratch_ptr, arr_ptr, n, counts[5]);
    ScatterPass<24>(arr_ptr, scratch_ptr, n, counts[6]);
    ScatterPass<28>(scratch_ptr, arr_ptr, n, counts[7]);
}

auto AllMatchStd(
    std::vector<int> const& v_std,
    std::vector<int> const& v_c,
    std::vector<int> const& v_cpp,
    std::vector<int> const& v_f1,
    std::vector<int> const& v_f2,
    std::vector<int> const& v_f3) -> bool
{
    return
        v_c == v_std &&
        v_cpp == v_std &&
        v_f1 == v_std &&
        v_f2 == v_std &&
        v_f3 == v_std;
}

auto PrintStdFailures(
    std::vector<int> const& v_c,
    std::vector<int> const& v_cpp,
    std::vector<int> const& v_f1,
    std::vector<int> const& v_f2,
    std::vector<int> const& v_f3,
    std::vector<int> const& v_std) -> void
{
    std::cout << std::format(
        "chk | c {} | cpp {} | f1 {} | f2 {} | f3 {}\n",
        v_c == v_std,
        v_cpp == v_std,
        v_f1 == v_std,
        v_f2 == v_std,
        v_f3 == v_std
    );
}

int main()
{
    using clock = std::chrono::steady_clock;
    using ms = std::chrono::duration<double, std::milli>;

    constexpr std::size_t element_count = 10'000'000;
    constexpr int iterations = 10;

    double total_c = 0.0;
    double total_cpp = 0.0;
    double total_f1 = 0.0;
    double total_f2 = 0.0;
    double total_f3 = 0.0;
    double total_std = 0.0;

    for (int iter = 0; iter < iterations; ++iter)
    {
        std::vector<int> rand_vals = MakeRandomInts(element_count);

        std::vector<int> v_c(rand_vals);
        std::vector<int> v_std(rand_vals);
        std::vector<int> v_cpp(rand_vals);
        std::vector<int> v_f1(rand_vals);
        std::vector<int> v_f2(rand_vals);
        std::vector<int> v_f3(rand_vals);

        auto const t0 = clock::now();
        lsd_radix_sort(v_c.data(), static_cast<int>(v_c.size()));
        auto const t1 = clock::now();

        auto const t2 = clock::now();
        lsd_radix_sort_cpp(v_cpp);
        auto const t3 = clock::now();

        auto const t4 = clock::now();
        lsd_radix_sort_cpp_f1(v_f1);
        auto const t5 = clock::now();

        auto const t6 = clock::now();
        lsd_radix_sort_cpp_f2(v_f2);
        auto const t7 = clock::now();

        auto const t8 = clock::now();
        lsd_radix_sort_cpp_f3(v_f3);
        auto const t9 = clock::now();

        auto const t10 = clock::now();
        std::ranges::sort(v_std);
        auto const t11 = clock::now();

        double const c_ms = ms(t1 - t0).count();
        double const cpp_ms = ms(t3 - t2).count();
        double const f1_ms = ms(t5 - t4).count();
        double const f2_ms = ms(t7 - t6).count();
        double const f3_ms = ms(t9 - t8).count();
        double const std_ms = ms(t11 - t10).count();

        total_c += c_ms;
        total_cpp += cpp_ms;
        total_f1 += f1_ms;
        total_f2 += f2_ms;
        total_f3 += f3_ms;
        total_std += std_ms;

        bool const ok = AllMatchStd(v_std, v_c, v_cpp, v_f1, v_f2, v_f3);

        std::cout << std::format(
            "it {:2} | c {:8.3f} | cpp {:8.3f} | f1 {:8.3f} | f2 {:8.3f} | f3 {:8.3f} | std {:8.3f} | ok {}\n",
            iter + 1,
            c_ms,
            cpp_ms,
            f1_ms,
            f2_ms,
            f3_ms,
            std_ms,
            ok
        );

        if (!ok)
        {
            PrintStdFailures(v_c, v_cpp, v_f1, v_f2, v_f3, v_std);
        }
    }

    std::cout << '\n';

    std::cout << std::format(
        "tot\n"
        "c   {:8.3f}\n"
        "cpp {:8.3f}\n"
        "f1  {:8.3f}\n"
        "f2  {:8.3f}\n"
        "f3  {:8.3f}\n"
        "std {:8.3f}\n\n",
        total_c,
        total_cpp,
        total_f1,
        total_f2,
        total_f3,
        total_std
    );

    std::cout << std::format(
        "avg\n"
        "c   {:8.3f}\n"
        "cpp {:8.3f}\n"
        "f1  {:8.3f}\n"
        "f2  {:8.3f}\n"
        "f3  {:8.3f}\n"
        "std {:8.3f}\n",
        total_c / iterations,
        total_cpp / iterations,
        total_f1 / iterations,
        total_f2 / iterations,
        total_f3 / iterations,
        total_std / iterations
    );
}