#pragma once

#include <vector>
#include <string>
#include <unordered_map>

namespace sc2 {

// See implementation for attribution.
// TODO: Use std::hash?
uint32_t MurmurHash2(const void * key, int len, uint32_t seed);

class SimpleHash {
public:
	uint32_t current_hash_ = 1315423911;

	void Accumulate(const std::vector<uint32_t>& data) {
        if (data.size() < 1) {
            return;
        }
        current_hash_ = MurmurHash2(&data[0], int(data.size() * sizeof(uint32_t)), current_hash_);
	}

	void Accumulate(uint32_t data) {
        current_hash_ = MurmurHash2(&data, sizeof(uint32_t), current_hash_);
	}
};

}
