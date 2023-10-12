#include <algorithm>
#include <chrono>
#include <vector>
#include "config.h"

namespace test {
	// Wiggle around the below target time.
	static constexpr std::chrono::milliseconds target_slop{1};

	// Time in milliseconds that we want a single test (between start/stop chrono calls)
	// to run.
	static constexpr std::chrono::milliseconds target_time{50};

	// Once we've found the number of iterations to hit the target time per test,
	// this is the number of times we run a test (taking the minimum measured
	// time of all as the profiling time).
	static constexpr int testruns = 10;

	typedef std::pair<std::chrono::nanoseconds, int> run_result_t;
	static run_result_t runimpl(const int n, const int64_t iter, search_t search, gen_t gen) {
		gen(true); // reset.

		int ret = 0;
		auto const first = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < iter; ++i) {
			auto const pair = gen(false);
			auto const val = pair.first;
			auto const arr = pair.second;
			auto const begin = &arr[0];
			auto const end = &arr[n];
			ret += search(begin, end, val) ? 1 : 0;
		}
		auto const last = std::chrono::high_resolution_clock::now();
		return { (last - first), ret };
	}

	int64_t measure_iter(int n, search_t search, gen_t gen) {
		// Initial iteration count we try to hit the target time - optimized
		// larger or smaller using measurements in the next loop.
		int64_t ret = 100;
		for (int i = 0; i < 100; ++i) {
			auto const tmp = runimpl(n, ret, search, gen);

			auto const time = tmp.first;
			if (target_time - target_slop <= time && time <= target_time + target_slop) {
				break;
			}

			auto const adj = ((target_time - time).count() * ret) / time.count();
			ret += adj;
		}

		return ret;
	}

	ret_t run(int n, int64_t iter, search_t search, gen_t gen) {
		run_result_t best = { std::chrono::nanoseconds::max(), 0 };
		for (int i = 0; i < testruns; ++i) {
			auto const sample = runimpl(n, iter, search, gen);
			if (sample.first < best.first) {
				best = sample;
			}
		}

		return ret_t{ best.first, iter, best.second };
	}
}
