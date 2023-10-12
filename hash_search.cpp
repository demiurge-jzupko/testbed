#include <algorithm>
#include <vector>
#include "config.h"

namespace hash_search {
	bool open(val_t const* begin, val_t const* end, int val) {
		size_t const n = (size_t)(end - begin);
		auto idx = fnv1a(val) & (n - 1);

		int other;
		while ((other = begin[idx].val)) {
			if (val == other) { return true; }
			idx = (idx + 1) & (n - 1);
		}
		return false;
	}
}
