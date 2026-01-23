#pragma once

#include <algorithm>
#include <cassert>
#include <concepts>
#include <iterator>
#include <limits>
#include <utility>
#include <vector>

namespace Easys {

template <typename T>
concept UnsignedIntegral = std::is_integral_v<T> && std::is_unsigned_v<T>;

template <UnsignedIntegral Key, typename Value>
class SparseSet {
   private:
	static constexpr Key nullKey = std::numeric_limits<Key>::max();

	std::vector<Key> sparse;    // Large, indexed by keys
	std::vector<Key> dense;     // Compact, stores keys
	std::vector<Value> values;  // Parallel to dense, stores values

   public:
	SparseSet() = default;

	// Ensure the sparse array can accommodate the given key
	void accommodate(const Key key)
	{
		if (key >= sparse.size())
		{
			size_t new_size = std::max(static_cast<size_t>(key + 1), sparse.size() * 2);
			sparse.resize(new_size, nullKey);
		}
	}

	template <typename... Args>
	Value& emplace(const Key key, Args&&... args)
	{
		accommodate(key);

		if (sparse[key] != nullKey)
		{
			values[sparse[key]] = Value(std::forward<Args>(args)...);
			return values[sparse[key]];
		} else
		{
			sparse[key] = static_cast<Key>(dense.size());
			dense.push_back(key);
			values.emplace_back(std::forward<Args>(args)...);
			return values.back();
		}
	}

	inline void set(const Key key, const Value& value) { emplace(key, value); }
	inline void set(const Key key, Value&& value) { emplace(key, std::move(value)); }

	Value* try_get(const Key key) noexcept
	{
		if (!contains(key)) return nullptr;
		return &values[sparse[key]];
	}
	const Value* try_get(const Key key) const noexcept
	{
		if (!contains(key)) return nullptr;
		return &values[sparse[key]];
	}

	inline const Value& operator[](const Key key) const
	{
		assert(contains(key) && "SparseSet::operator[] Key not present");
		return values[sparse[key]];
	}
	inline Value& operator[](const Key key)
	{
		assert(contains(key) && "SparseSet::operator[] Key not present");
		return values[sparse[key]];
	}

	Value& at(const Key key)
	{
		if (!contains(key)) throw std::out_of_range("Key not found in SparseSet");
		return values[sparse[key]];
	}

	void remove(const Key key)
	{
		if (!contains(key)) return;

		const Key index_of_removed = sparse[key];
		const Key index_of_last = static_cast<Key>(dense.size() - 1);
		const Key key_of_last = dense.back();

		if (index_of_removed != index_of_last)
		{
			values[index_of_removed] = std::move(values.back());
			dense[index_of_removed] = key_of_last;
			sparse[key_of_last] = index_of_removed;
		}

		values.pop_back();
		dense.pop_back();

		sparse[key] = nullKey;
	}

	auto begin() noexcept { return values.begin(); }
	auto end() noexcept { return values.end(); }
	const auto begin() const noexcept { return values.begin(); }
	const auto end() const noexcept { return values.end(); }

	// Iterate over all values Func(Entity, Component)
	template <typename Func>
	inline void forEach(Func&& f)
	{
		for (size_t i = 0; i < values.size(); ++i)
		{
			f(dense[i], values[i]);
		}
	}

	bool empty() const noexcept { return dense.empty(); }
	size_t size() const noexcept { return dense.size(); }
	bool contains(const Key key) const noexcept { return key < sparse.size() && sparse[key] != nullKey; }
	// Returns the capacity of the dense array (actual components)
	size_t capacity() const noexcept { return dense.capacity(); }
	// Returns the theoretical max size based on the sparse array size
	size_t extent() const noexcept { return sparse.size(); }

	inline const std::vector<Key>& getKeys() const { return dense; }
	inline const std::vector<Value>& getValues() const { return values; }
	inline std::vector<Value>& getValues() { return values; }

	inline void clear()
	{
		std::fill(sparse.begin(), sparse.end(), nullKey);
		dense.clear();
		values.clear();
	}

	void reserve(size_t capacity)
	{
		dense.reserve(capacity);
		values.reserve(capacity);
		// Sparse size is unpredictable, but we can guess based on expected max entity ID
		if (sparse.size() < capacity)
		{
			sparse.resize(capacity, nullKey);
		}
	}
};

}  // namespace Easys