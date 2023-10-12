#include <algorithm>
#include <vector>
#include "config.h"

namespace linear_search {
	bool stl(val_t const* begin, val_t const* end, int val) {
		return (end != std::find(begin, end, val));
	}
}
