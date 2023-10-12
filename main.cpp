#include <algorithm>
#include <chrono>
#include <inttypes.h>
#include <vector>
#include "config.h"

namespace binary_search { bool simple(val_t const* begin, val_t const* end, int val); }
namespace binary_search { bool stl(val_t const* begin, val_t const* end, int val); }
namespace hash_search { bool open(val_t const* begin, val_t const* end, int val); }
namespace linear_search { bool simple(val_t const* begin, val_t const* end, int val); }
namespace linear_search { bool stl(val_t const* begin, val_t const* end, int val); }
namespace test { int64_t measure_iter(int n, search_t search, gen_t gen); }
namespace test { ret_t run(int n, int64_t iter, search_t search, gen_t gen); }

// Test values of n
static const int sizes[] = { 10, 20, 30, 40, 50, 100, 200, 400, 500, 1000, 5000 };
static const char* ithlabel[] = { "0", "n/8", "n/4", "n/2", "3n/4", "7n/8", "n-1", "n" };

static const int arrcount = 256;
static const int sentinel = INT_MAX;

static int targetidx = 0;
static bool hashing = false;
static int nextarr = 0x811C9DC5;
static val_t* hashed[arrcount];
static val_t* sorted[arrcount];

static void end_gen() {
	for (int i = arrcount - 1; i >= 0; --i) {
		delete [] hashed[i];
		delete [] sorted[i];
	}
	memset(hashed, 0, sizeof(hashed));
	memset(sorted, 0, sizeof(sorted));
	targetidx = 0;
	hashing = false;
	nextarr = 0x811C9DC5;
}

static void ready_gen(int n) {
	targetidx = 0;
	hashing = false;
	nextarr = 0x811C9DC5;

	// sorted.
	for (int i = 0; i < arrcount; ++i) {
		auto& a = sorted[i];
		a = new val_t[n+1]; // space for the sentinel.
		for (int j = 0; j < n; ++j) {
			auto val = rand();
			// convenience, don't use 0 so it can be reserved as null
			// placeholder in hashed array.
			while (!val) { val = rand(); }
			// likewise, don't insert a sentinel, we use that at 'n' to allow
			// for searches that will find no results.
			while (val == sentinel) { val = rand(); }
			// commit.
			a[j] = { val };
		}
		a[n] = { sentinel }; // placeholder, used for "can't find".
		std::sort(&a[0], &a[n]);
	}

	// hashed.
	for (int i = 0; i < arrcount; ++i) {
		auto const& input = sorted[i];
		auto& output = hashed[i];
		auto const cap = hash_cap(n);
		output = new val_t[cap];
		memset(output, 0, sizeof(*output) * cap);

		for (int j = 0; j < n; ++j) {
			auto const v = input[j];
			auto const hash = fnv1a(v.val);
			auto idx = hash & (cap - 1);
			while (output[idx].val) {
				idx = (idx + 1) & (cap - 1);
			}
			output[idx] = {v};
		}
	}
}

static std::pair<int, val_t*> gen(bool reset) {
	if (reset) {
		nextarr = 0x811C9DC5;
		return { 0, nullptr };
	} else {
		nextarr = (nextarr * 0x01000193);
		auto const idx = (nextarr & (arrcount - 1));
		auto const target = sorted[idx][targetidx].val;
		auto const arrs = (hashing ? hashed : sorted);
		return { target, arrs[idx] };
	}
}

struct test_entry_t {
	const char* name;
	search_t search;
	bool hashing;
};
static const test_entry_t entries[] = {
	{ "std::binary_search",  binary_search::stl, false },
	{ "std::find",           linear_search::stl, false },
	{ "hash (open address)", hash_search::open,  true  },
};

#define ARRAY_COUNT(arr) (sizeof(arr) / (sizeof(arr[0])))

extern "C" {
	int main(int argc, char** argv) {
		// Headers for results.
		fprintf(stdout, "n,ith,iters");
		for (auto const& entry : entries) {
			fprintf(stdout, ",\"%s (ns)\"", entry.name);
		}

		ret_t results[ARRAY_COUNT(entries)];
		for (auto const n : sizes) {
			ready_gen(n);
			// Using linear_search::stl since it becomes slowest as n grows -
			// note that this may have the tradeoff of reducing the quality
			// of results for other searches at larger values of n (as the
			// binary/hash/linear searches diverge in time).
			hashing = false;
			targetidx = n;
			auto const iter = test::measure_iter(n, linear_search::stl, gen);

			int const ithvalue[] = { 0, n / 8, n / 4, n / 2, (3 * n) / 4, (7 * n) / 8, n - 1, n };
			for (int lbl = 0; lbl < ARRAY_COUNT(ithlabel); ++lbl) {
				auto const ith = ithvalue[lbl];
				targetidx = ith;

				for (size_t i = 0; i < ARRAY_COUNT(entries); ++i) {
					hashing = entries[i].hashing;
					auto const size = (hashing ? hash_cap(n) : n);
					results[i] = test::run(size, iter, entries[i].search, gen);
				}

				// Verify.
				for (size_t i = 1; i < ARRAY_COUNT(entries); ++i) {
					if (results[i-1].retval != results[i].retval) {
						fprintf(stderr, "%d,\"%s\",%" PRId64 ",%s != %s: %d != %d\n",
							n, ithlabel[lbl], iter, entries[i-1].name,
							entries[i].name,
							results[i-1].retval,
							results[i].retval);
						return 1;
					}
				}

				// Report results.
				fprintf(stdout, "\n%d,\"%s\",%" PRId64, n, ithlabel[lbl], iter);
				for (size_t i = 0; i < ARRAY_COUNT(entries); ++i) {
					auto const res = results[i];
					auto const duration = (double)((int64_t)res.duration.count()) / (double)res.iter;
					fprintf(stdout, ",%.2f", duration);
				}
				fflush(stdout);
			}
			end_gen();
		}

		return 0;
	}
}
