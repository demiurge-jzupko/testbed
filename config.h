#ifndef CONFIG_H
#define CONFIG_H

#include <chrono>

// Padding (in 4 byte increments - actual padding will be this value * 4)
// to add to the structure used in the test arrays.
#define STRUCT_PADDING 0

// Value used in structures for testing.
struct val_t {
	int val;
#if STRUCT_PADDING
	int padding[STRUCT_PADDING];
#endif

	bool operator==(int bval) const { return val == bval; }
	bool operator<(int bval) const { return (val < bval); }
	bool operator==(const val_t& b) const { return val == b.val; }
	bool operator<(const val_t& b) const { return (val < b.val); }
};

static constexpr bool operator==(int a, const val_t& b) { return a == b.val; }
static constexpr bool operator<(int a, const val_t& b) { return a < b.val; }

struct ret_t {
	std::chrono::nanoseconds duration;
	int64_t iter;
	int retval;
};
typedef std::pair<int, val_t*> (*gen_t)(bool reset);
typedef bool (*search_t)(val_t const* begin, val_t const* end, int val);

static constexpr uint32_t next_powof2(uint32_t u) {
	if (!u) { return 1; }

	u--;
	u |= (u >> 1);
	u |= (u >> 2);
	u |= (u >> 4);
	u |= (u >> 8);
	u |= (u >> 16);
	u++;

	return u;
}

static constexpr uint32_t hash_cap(uint32_t n) {
	return next_powof2(n + ((n * 4) / 5));
}

static constexpr uint32_t prime = 0x01000193;
static constexpr uint32_t seed = 0x811C9DC5;

static constexpr uint32_t fnv1a(int val, uint32_t hash = seed) {
	uint8_t const* p = (uint8_t const*)&val;
	hash = (p[0] ^ hash) * prime;
	hash = (p[1] ^ hash) * prime;
	hash = (p[2] ^ hash) * prime;
	hash = (p[3] ^ hash) * prime;
	return hash;
}

#endif
