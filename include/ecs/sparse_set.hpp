#pragma once

#include <cassert>
#include <concepts>
#include <iostream>
#include <limits>
#include <vector>

// Since SparseSet uses std::vector internally and the key is used to index we want to make sure to only use natural
// numbers. We could also use a map for the sparse container...
template <typename T>
concept UnsignedIntegral = std::is_integral_v<T> && std::is_unsigned_v<T>;

struct SparseSetBase {
	virtual ~SparseSetBase() = default;
	virtual size_t size() const = 0;
};

template <UnsignedIntegral Key, typename Value>
class SparseSet : public SparseSetBase {
  private:
	std::vector<Key> sparse;   // Large, indexed by keys
	std::vector<Key> dense;    // Compact, stores keys
	std::vector<Value> values; // Parallel to dense, stores values

  public:
	// Ensure the sparse array can accommodate the given key
	void accommodate(Key key)
	{
		// TODO: CHECK FOR MAX_SIZE

		if (key >= sparse.size()) {
			sparse.resize(key + 100, std::numeric_limits<Key>::max()); // Use max Key as invalid index
		}
	}

	// Associate a value with a key
	void add(Key key, const Value &value)
	{
		accommodate(key);

		if (sparse[key] == std::numeric_limits<Key>::max()) { // max Key to indicate not set
			sparse[key] = static_cast<Key>(values.size());
			dense.push_back(key);
			values.push_back(value);
		} else {
			values[sparse[key]] = value; // Key already has a value, update it
		}
	}

	// Retrieve a value by key
	const Value &get(Key key) const
	{
		if (contains(key)) {
			return values[sparse[key]];
		}
		throw std::runtime_error("Value not found");
	}

	// Retrieve a value by key
	Value &get(Key key)
	{
		if (contains(key)) {
			return values[sparse[key]];
		}
		throw std::runtime_error("Value not found");
	}

	// Remove a value associated with a key
	void remove(Key key)
	{
		if (key < sparse.size() && sparse[key] != std::numeric_limits<Key>::max()) {
			// Move the last value to the removed spot to keep dense packed
			Key indexOfRemoved = sparse[key];
			values[indexOfRemoved] = values.back();
			dense[indexOfRemoved] = dense.back();

			// Update the sparse array for the moved key
			sparse[dense.back()] = indexOfRemoved;

			// Shrink the dense array and values
			dense.pop_back();
			values.pop_back();

			// Mark the key as not set
			sparse[key] = std::numeric_limits<Key>::max();
		}
	}

	// Iterate over all values
	template <typename Func>
	void forEach(Func f)
	{
		for (size_t i = 0; i < values.size(); ++i) {
			f(dense[i], values[i]);
		}
	}

	bool contains(Key key) const { return key < sparse.size() && sparse[key] != std::numeric_limits<Key>::max(); }

	size_t size() const { return dense.size(); }

	const std::vector<Key> &getKeys() const { return dense; }

	std::vector<Value> &getValues() { return values; }
};