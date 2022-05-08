#pragma once

template<typename T>
class LockQueue
{
public:
	void Push(T item)
	{
		WRITE_LOCK;
		items_.push(item);
	}

	T Pop()
	{
		WRITE_LOCK;
		if (items_.empty())
			return T();

		T item = items_.front();
		items_.pop();

		return item;
	}

	void PopAll(OUT Vector<T>& items)
	{
		WRITE_LOCK;
		while (T item = Pop())
			items.push_back(item);
	}

	void Clear()
	{
		WRITE_LOCK;
		items_ = Queue<T>();
	}

private:
	USE_LOCK;
	Queue<T> items_;
};