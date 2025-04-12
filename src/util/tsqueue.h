#pragma once
#include <memory>
#include <deque>
#include <thread>
#include <mutex>
#include <optional>
#include <algorithm>
#include <cstdint>

template<typename T>
class tsqueue
{
public:
	tsqueue() = default;
	tsqueue(const tsqueue<T>&) = delete; // No copying, as trying to copy mutexes is not a good idea
	virtual ~tsqueue() { clear<void>(); }


	// Returns and maintains item at front of Queue
	template<typename T2>
	const T2& front()
	{
		std::scoped_lock lock(muxQueue);
		if (deqQueue.empty()) { logger.err("Attempted to call front() on an empty tsqueue!"); throw std::exception(); }
		return deqQueue.front();
	}

	// Returns and maintains item at back of Queue
	template<typename T2>
	const T2& back()
	{
		std::scoped_lock lock(muxQueue);
		if (deqQueue.empty()) { logger.err("Attempted to call back() on an empty tsqueue!"); throw std::exception(); }
		return deqQueue.back();
	}

	// Adds an item to back of Queue
	template<typename T2>
	const T2& push_back(const T2& item)
	{
		std::scoped_lock lock(muxQueue);
		return deqQueue.emplace_back(std::move(item));
	}

	// Adds an item to front of Queue
	template<typename T2>
	const T2& push_front(const T2& item)
	{
		std::scoped_lock lock(muxQueue);
		return deqQueue.emplace_front(std::move(item));
	}

	// Removes and returns item from front of Queue
	template<typename T2>
	T2 pop_front()
	{
		std::scoped_lock lock(muxQueue);
		if (deqQueue.empty()) { logger.err("Attempted to call pop_front() on an empty tsqueue!"); throw std::exception(); }
		auto t = std::move(deqQueue.front());
		deqQueue.pop_front();
		return t;
	}

	// Removes and returns item from back of Queue
	template<typename T2>
	T2 pop_back()
	{
		std::scoped_lock lock(muxQueue);
		if (deqQueue.empty()) { logger.err("Attempted to call pop_back() on an empty tsqueue!"); throw std::exception(); }
		auto t = std::move(deqQueue.back());
		deqQueue.pop_back();
		return t;
	}

	// Returns true if Queue has no items
	template<typename T2>
	bool empty()
	{
		std::scoped_lock lock(muxQueue);
		return deqQueue.empty();
	}

	// Returns the number of items in Queue
	template<typename T2>
	size_t count()
	{
		std::scoped_lock lock(muxQueue);
		return deqQueue.size();
	}

	// Clears Queue
	template<typename T2>
	void clear()
	{
		std::scoped_lock lock(muxQueue);
		deqQueue.clear();
	}

	auto begin()
	{
		std::scoped_lock lock(muxQueue);
		return deqQueue.begin();
	}

	auto end()
	{
		std::scoped_lock lock(muxQueue);
		return deqQueue.end();
	}

protected:
	std::mutex muxQueue;
	std::deque<T> deqQueue;
};