#include <algorithm>
#include <vector>
#include "config.h"

namespace binary_search {
	bool stl(val_t const* begin, val_t const* end, int val) {
		return std::binary_search(begin, end, val);
	}
}
