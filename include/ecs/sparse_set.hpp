#include <cassert>
#include <iostream>
#include <vector>

struct SparseSetBase {
	virtual ~SparseSetBase() = default;
	virtual size_t size() const = 0;
};

template <typename Key, typename Value>
class SparseSet : public SparseSetBase {
  private:
	std::vector<Key> sparse;   // Large, indexed by entity ID
	std::vector<Key> dense;    // Compact, stores entity IDs
	std::vector<Value> values; // Parallel to dense, stores values

  public:
	// Ensure the sparse array can accommodate the given entity ID
	void accommodate(Key key)
	{
		// TODO: CHECK FOR MAX_SIZE

		if (key >= sparse.size()) {
			sparse.resize(key + 100, -1); // Use -1 or max Key as invalid index
		}
	}

	// Associate a value with an entity
	void add(Key key, const Value &value)
	{
		accommodate(key);

		if (sparse[key] == -1) { // or max Key to indicate not set
			sparse[key] = values.size();
			dense.push_back(key);
			values.push_back(value);
		} else {
			// Key already has a value, optionally update it
			values[sparse[key]] = value;
		}
	}

	// Retrieve a value by entity ID
	const Value &get(Key key) const
	{
		if (contains(key)) {
			return values[sparse[key]];
		}
		throw std::runtime_error("Value not found");
	}

	// Retrieve a value by entity ID
	Value &get(Key key)
	{
		if (contains(key)) {
			return values[sparse[key]];
		}
		throw std::runtime_error("Value not found");
	}

	// Remove a value associated with an entity
	void remove(Key key)
	{
		if (key < sparse.size() && sparse[key] != -1) {
			// Move the last value to the removed spot to keep dense packed
			Key indexOfRemoved = sparse[key];
			values[indexOfRemoved] = values.back();
			dense[indexOfRemoved] = dense.back();

			// Update the sparse array for the moved entity
			sparse[dense.back()] = indexOfRemoved;

			// Shrink the dense array and values
			dense.pop_back();
			values.pop_back();

			// Mark the entity as not set
			sparse[key] = -1; // or max Key
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

	bool contains(Key key) const { return key < sparse.size() && sparse[key] != -1; }

	size_t size() const { return dense.size(); }

	const std::vector<Key> &getKeys() const { return dense; }

	std::vector<Value> &getValues() { return values; }
};