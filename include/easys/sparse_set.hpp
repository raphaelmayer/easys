#pragma once

#include <algorithm>
#include <cassert>
#include <concepts>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace Easys {

class KeyNotFoundException : public std::exception {
   public:
	KeyNotFoundException(const std::string &key) : msg_("KeyNotFoundException: " + key + " not found") {}

	const char *what() const noexcept override { return msg_.c_str(); }

   private:
	std::string msg_;
};

// Since SparseSet uses std::vector internally and the key is used to index we want to make sure to only use natural
// numbers. We could also use a map for the sparse container...
template <typename T>
concept UnsignedIntegral = std::is_integral_v<T> && std::is_unsigned_v<T>;

template <UnsignedIntegral Key, typename Value>
class SparseSet {
   private:
	std::vector<Key> sparse;    // Large, indexed by keys
	std::vector<Key> dense;     // Compact, stores keys
	std::vector<Value> values;  // Parallel to dense, stores values

   public:
	// Ensure the sparse array can accommodate the given key
	inline void accommodate(const Key key)
	{
		if (key >= maxSize())
		{
			throw std::length_error("Key exceeds the maximum size limit.");
		}

		if (key >= sparse.size())
		{
			sparse.resize(key * 2 + 1, std::numeric_limits<Key>::max());
		}
	}

	// Associate a value with a key

	// Associate a value with a key
	inline void set(const Key key, const Value &value)
	{
		accommodate(key);

		if (sparse[key] == std::numeric_limits<Key>::max())
		{  // max Key to indicate not set
			sparse[key] = static_cast<Key>(values.size());
			dense.push_back(key);
			values.push_back(value);
		} else
		{
			values[sparse[key]] = value;  // Key already has a value, update it
		}
	}

	// move semantics overload
	inline void set(const Key key, Value &&value)
	{
		accommodate(key);

		if (sparse[key] == std::numeric_limits<Key>::max())
		{
			sparse[key] = static_cast<Key>(values.size());
			dense.push_back(key);
			values.push_back(std::move(value));
		} else
		{
			values[sparse[key]] = std::move(value);
		}
	}

	// Retrieve a value by key
	inline const Value &get(const Key key) const
	{
		if (!contains(key))
		{
			throw KeyNotFoundException(std::to_string(key));
		}
		return values[sparse[key]];
	}

	// Retrieve a value by key
	inline Value &get(const Key key)
	{
		if (!contains(key))
		{
			throw KeyNotFoundException(std::to_string(key));
		}
		return values[sparse[key]];
	}

	inline const Value &operator[](const Key key) const { return values[sparse[key]]; }
	inline Value &operator[](const Key key) { return values[sparse[key]]; }

	// Remove a value associated with a key
	inline void remove(const Key key)
	{
		if (contains(key))
		{
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
	inline void forEach(Func f)
	{
		for (size_t i = 0; i < values.size(); ++i)
		{
			f(dense[i], values[i]);
		}
	}

	constexpr bool contains(const Key key) const
	{
		return key < sparse.size() && sparse[key] != std::numeric_limits<Key>::max();
	}

	constexpr size_t size() const { return dense.size(); }

	inline const std::vector<Key> &getKeys() const { return dense; }

	inline std::vector<Value> &getValues() { return values; }

	inline const std::vector<Value> &getValues() const { return values; }

	constexpr size_t maxSize() const noexcept
	{
		constexpr size_t maxKeyVal = static_cast<size_t>(std::numeric_limits<Key>::max());
		return std::min({maxKeyVal, dense.max_size(), values.max_size()});
	}

	inline void clear()
	{
		sparse.clear();
		dense.clear();
		values.clear();
	}
};

}  // namespace Easys